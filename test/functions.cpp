/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

//sin
TEST_F(FormulaTest, functions1)
{
    Start(600);

    document.WaitTask(document.InsertFunction("sin", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>sin</mi>" \
                        "<mo>(</mo>" \
                        "<mo>)</mo>" \
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>sin</mi>" \
                        "<mo>(</mo>" \
                        "<mo>)</mo>" \
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();
}

//Recalculate a dependent expression
TEST_F(FormulaTest, functions2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertAssignment(true));
    document.InsertString("x", true);
    document.InsertMultiply(true);
    document.WaitTask(document.InsertString("2", true));
    document.WaitSolver();

    document.MoveCaretRight(false);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.WaitTask(document.InsertString("5", true));
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"f(x)=x*2\n" \
        U"f(5)=10.") << ToBasicString(document.ToText());
    
    document.MoveCaretUp(false);
    for (int i = 0; i < 6; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("2", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"f(x)=x*22\n"\
        U"f(5)=110.") << ToBasicString(document.ToText());
}

//Recalculate a dependent expression
TEST_F(FormulaTest, functions3)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertAssignment(true));
    document.WaitTask(document.InsertString("x", true));
    document.WaitSolver();
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.WaitTask(document.InsertString("2", true));
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.InsertAssignment(true);
    document.InsertString("x", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.WaitTask(document.InsertString("3", true));
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"f(x)=x\n" \
        U"f(2)=2.\n" \
        U"f(x)=x+5\n" \
        U"f(3)=8.") << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
    
    document.MoveCaretUp(false);
    for (int i = 0; i < 6; ++i)
        document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("2", true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"f(x)=x\n" \
        U"f(2)=2.\n" \
        U"f(x)=x+25\n" \
        U"f(3)=28.") << ToBasicString(document.ToText());
    
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertPlus(true));
    document.WaitTask(document.InsertString("4", true));
    document.WaitSolver();
    std::this_thread::sleep_for(4s);
    ASSERT_TRUE(document.ToText() == 
        U"f(x)=x+4\n" \
        U"f(2)=6.\n" \
        U"f(x)=x+25\n" \
        U"f(3)=28.") << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

//Insert a function after selection
TEST_F(FormulaTest, functions4)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("pi", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.InsertMinus(true);
    document.InsertDivision(true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.InsertFunction("sin", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>sin</mi>"\
                        "<mo>(</mo>"\
                        "<mi>pi</mi>"\
                        "<mo>+</mo>"\
                        "<mi>1</mi>"\
                        "<mo>)</mo>" \
                        "<mo>-</mo>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 5})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>pi</mi>"\
                        "<mo>+</mo>"\
                        "<mi>1</mi>"\
                        "<mo>-</mo>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 0, 3})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>sin</mi>"\
                        "<mo>(</mo>"\
                        "<mi>pi</mi>"\
                        "<mo>+</mo>"\
                        "<mi>1</mi>"\
                        "<mo>)</mo>" \
                        "<mo>-</mo>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 6})) << document.GetEditorState().ToString();
}

//User functions
TEST_F(FormulaTest, user_functions1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertComma(true);
    document.InsertString("y", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertAssignment(true));
    document.InsertString("x", true);
    document.InsertPower(true);
    document.InsertString("y", true);
    document.WaitSolver();

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("2", true);
    document.InsertComma(true);
    document.InsertString("3", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"f(x,y)=pow(x,y)\n" \
        U"f(2,3)=8."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

//Insert a function inside a code block before
TEST_F(FormulaTest, user_functions2)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.InsertString("h", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();

    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("E", true);
    document.InsertAssignment(true);
    document.InsertString("h", true);
    document.InsertMultiply(true);
    document.InsertString("m", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("5", true);
    document.WaitTask(document.InsertCloseRoundBracket(true));
    document.WaitSolver();

    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("E", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"h=5\n" \
        U"E=h*m(5)\n" \
        U"E=Unknown identifier"
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 1, 0, 0, 0, 0, 0, 2, 2}, start, size)) << ErrorMarks();

    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.MoveCaretUp(false));
    document.InsertString("m", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("x", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"m(x)=x\n" \
        U"h=5\n" \
        U"E=h*m(5)\n" \
        U"E=25."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

}
