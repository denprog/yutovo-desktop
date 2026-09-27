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

TEST_F(FormulaTest, square_root1)
{
    Start(600);

    document.WaitTask(document.InsertSquareRoot(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msqrt>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</msqrt>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msqrt>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</msqrt>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("2", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msqrt>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                        "</msqrt>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, square_root2)
{
    Start(600);

    document.InsertSquareRoot(true);
    document.InsertString("12", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>12</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msqrt>"\
                            "<mrow>"\
                                "<mi>12</mi>"\
                            "</mrow>"\
                        "</msqrt>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>12</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, square_root3)
{
    Start(600);

    document.InsertSquareRoot(true);
    document.InsertString("12", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("3", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msqrt>"\
                            "<mrow>"\
                                "<mi>12</mi>"\
                                "<mo>+</mo>"\
                                "<mi>3</mi>"\
                            "</mrow>"\
                        "</msqrt>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 2, 1})) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>12</mi>"\
                        "<mo>+</mo>"\
                        "<mi>3</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msqrt>"\
                            "<mrow>"\
                                "<mi>12</mi>"\
                                "<mo>+</mo>"\
                                "<mi>3</mi>"\
                            "</mrow>"\
                        "</msqrt>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>12</mi>"\
                        "<mo>+</mo>"\
                        "<mi>3</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Save/Load
TEST_F(FormulaTest, square_root4)
{
    Start(600);

    document.InsertString("Square root", true);
    document.InsertSquareRoot(true);
    document.InsertString("12", true);
    document.InsertPlus(true);
    document.InsertString("3", true);
    document.InsertSquareRoot(true);
    document.WaitTask(document.InsertString("567", true));
    document.Save("square_root4_1.yut");

    document.WaitTask(document.New());
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    std::this_thread::sleep_for(200ms);
    document.Load("square_root4_1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Square root</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msqrt>"\
                            "<mrow>"\
                                "<mi>12</mi>"\
                                "<mo>+</mo>"\
                                "<mi>3</mi>"\
                                "<msqrt>"\
                                    "<mrow>"\
                                        "<mi>567</mi>"\
                                    "</mrow>"\
                                "</msqrt>"\
                            "</mrow>"\
                        "</msqrt>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 1, 3, 1, 0, 3})) << document.GetEditorState().ToString();
}

//Insert a char in the operation sign
TEST_F(FormulaTest, square_root5)
{
    Start(600);

    document.WaitTask(document.InsertSquareRoot(true));
    document.WaitTask(document.MoveCaretLeft(false));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.InsertString("3", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msqrt>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</msqrt>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Don't insert paragraph inside a square root
TEST_F(FormulaTest, square_root6)
{
    Start(600);

    document.InsertSquareRoot(true);
    document.InsertString("1", true);
    document.InsertString("2", true);
    document.InsertString("3", true);
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"sqrt(123)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 1, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0, 1, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"sqrt()") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, square_root7)
{
    Start(600);

    document.InsertSquareRoot(true);
    document.InsertString("123", true);
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"sqrt()") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"sqrt(123)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, square_root8)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    for (int i = 0; i < 5; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertSquareRoot(true));
    ASSERT_TRUE(document.ToText() == U"sqrt(123+1)") << ToBasicString(document.ToText());
}

//Insert division after selection
TEST_F(FormulaTest, square_root9)
{
    Start(600);

    document.InsertSquareRoot(true);
    document.InsertString("123", true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToText() == U"(sqrt(123))/()") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Check font
TEST_F(FormulaTest, square_root10)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetBold(true));
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);

    document.WaitTask(document.InsertSquareRoot(true));
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 1, 0}, format));
    ASSERT_TRUE(format.bold);
}

//Insert power after selection
TEST_F(FormulaTest, square_root11)
{
    Start(600);

    document.Load("../../test/tests/power23.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(1s);

    document.WaitTask(document.InsertSquareRoot(true));
    ASSERT_TRUE(document.ToText() == U"expand(sqrt(pow(x,3)+(x)/(2)))") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 1, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"expand(pow(x,3)+(x)/(2))") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 5}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 2, 3})) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertSquareRoot(true));
    ASSERT_TRUE(document.ToText() == U"expand(sqrt(pow(x,3)+(x)/(2)))") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 1, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"expand(pow(x,3)+(x)/(2))") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 2, 3})) << document.GetEditorState().ToString();
}

}
