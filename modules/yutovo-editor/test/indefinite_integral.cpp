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

//Insert indefinite integral and edit it
TEST_F(FormulaTest, indefinite_integral1)
{
    Start(600);

    document.WaitTask(document.InsertIndefiniteIntegral(true));
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>∫</mo>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                        "<mi>d</mi>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
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
                        "<mo>∫</mo>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                        "<mi>d</mi>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    //fill the editable fields
    document.InsertString(U"x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString(U"y", true));
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,y)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 1})) << document.GetEditorState().ToString();

    //move caret on the integral sign and delete the whole formula
    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"x") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,y)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Insert division in the integrand expression
TEST_F(FormulaTest, indefinite_integral2)
{
    Start(600);

    document.WaitTask(document.InsertIndefiniteIntegral(true));
    document.InsertOpenRoundBracket(true);
    document.InsertString("2pi", true);
    document.InsertCloseRoundBracket(true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>∫</mo>"\
                        "<mrow>"\
                            "<mo>(</mo>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>2pi</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                            "<mo>)</mo>"\
                        "</mrow>"\
                        "<mi>d</mi>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();
}

//Check selection between elements
TEST_F(FormulaTest, indefinite_integral3)
{
    Start(600);

    document.InsertIndefiniteIntegral(true);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("y", true));
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,y)") << ToBasicString(document.ToText());

    for (int i = 0; i < 2; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 0},
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0}, 3, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 1})) << document.GetEditorState().ToString();
}

//Copy-paste
TEST_F(FormulaTest, indefinite_integral4)
{
    Start(600);

    document.InsertIndefiniteIntegral(true);
    for (int i = 0; i < 3; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0},
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() ==
        U"indefinite_integral(,)indefinite_integral(,)"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() ==
        U"indefinite_integral(,)"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Replace a string after selection
TEST_F(FormulaTest, indefinite_integral5)
{
    Start(600);

    document.InsertIndefiniteIntegral(true);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.InsertString("y", true);
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,y)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 0},
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 3, 1})) << document.GetEditorState().ToString();

    //replace the integration variable
    document.WaitTask(document.InsertString("t", true));
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,t)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,y)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 0},
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 3, 1})) << document.GetEditorState().ToString();

    //replace the integrand
    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("z", true));
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(z,y)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,y)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
}

//The "d" string is not editable and is skipped by the caret
TEST_F(FormulaTest, indefinite_integral6)
{
    Start(600);

    document.InsertIndefiniteIntegral(true);
    document.InsertString("x", true);

    //caret skips the "d" and lands in the integration variable
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 0})) << document.GetEditorState().ToString();

    //typing goes into the variable, not the "d"
    document.WaitTask(document.InsertString("5", true));
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,5)") << ToBasicString(document.ToText());

    //caret left from the variable also skips the "d" and lands at the end of the integrand
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 1})) << document.GetEditorState().ToString();
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,5)") << ToBasicString(document.ToText());
}

//Delete the whole formula by Backspace and Delete around it
TEST_F(FormulaTest, indefinite_integral7)
{
    Start(600);

    document.InsertIndefiniteIntegral(true);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("y", true));
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,y)") << ToBasicString(document.ToText());

    //Backspace after the formula deletes it
    document.WaitTask(document.MoveCaretEnd(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,y)") << ToBasicString(document.ToText());

    //Delete before the formula deletes it
    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,y)") << ToBasicString(document.ToText());
}

//Clear the editable fields by selection and delete
TEST_F(FormulaTest, indefinite_integral8)
{
    Start(600);

    document.InsertIndefiniteIntegral(true);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("y", true));
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,y)") << ToBasicString(document.ToText());

    //clear the variable
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 0},
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 3, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,)") << ToBasicString(document.ToText());

    //clear the integrand
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(,)") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,)") << ToBasicString(document.ToText());
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,y)") << ToBasicString(document.ToText());
}

//Save and load a document with the integral
TEST_F(FormulaTest, indefinite_integral9)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.InsertIndefiniteIntegral(true);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("y", true));
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,y)") << ToBasicString(document.ToText());

    document.WaitTask(document.Save("indefinite_integral1.yut"));
    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Load("indefinite_integral1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,y)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>∫</mo>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mi>d</mi>"\
                        "<mrow>"\
                            "<mi>y</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();
}

//Check selection between elements
TEST_F(FormulaTest, indefinite_integral10)
{
    Start(600);

    document.InsertIndefiniteIntegral(true);
    document.InsertString("x", true);
    document.InsertDivision(true);
    document.InsertString("y", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 1})) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0})) << document.GetEditorState().ToString();
}

}
