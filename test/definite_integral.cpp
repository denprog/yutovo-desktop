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

//Insert integral and edit it
TEST_F(FormulaTest, definite_integral1)
{
    Start(600);

    document.WaitTask(document.InsertDefiniteIntegral(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<munderover>"\
                            "<mo>∫</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</munderover>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

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
                        "<munderover>"\
                            "<mo>∫</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</munderover>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    //fill all the editable fields
    document.InsertString(U"0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"10", true);
    document.MoveCaretRight(false);
    document.InsertString(U"x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString(U"y", true));
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,10,x,y)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 5, 0, 1})) << document.GetEditorState().ToString();

    //move caret on the integral sign and delete the whole formula
    for (int i = 0; i < 7; ++i)
        document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"x") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,10,x,y)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    //vertical caret movements between the limits
    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Insert division in the integrand expression
TEST_F(FormulaTest, definite_integral2)
{
    Start(600);

    document.WaitTask(document.InsertDefiniteIntegral(true));
    for (int i = 0; i < 3; ++i)
        document.MoveCaretRight(false);
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
                        "<munderover>"\
                            "<mo>∫</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</munderover>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 1, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Check selection between elements
TEST_F(FormulaTest, definite_integral3)
{
    Start(600);

    document.InsertDefiniteIntegral(true);
    document.InsertString("0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("20", true);
    document.MoveCaretRight(false);
    document.InsertString("123", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("x", true));
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,20,123,x)") << ToBasicString(document.ToText());

    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 5, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0}, 5, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 5, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 3})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 2})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0, 3, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    //select the lower limit as an element
    document.WaitTask(document.MoveCaretLeft(false));
    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
}

//Copy-paste
TEST_F(FormulaTest, definite_integral4)
{
    Start(600);

    document.InsertDefiniteIntegral(true);
    for (int i = 0; i < 5; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"definite_integral(,,,)definite_integral(,,,)"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"definite_integral(,,,)"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"definite_integral(,,,)definite_integral(,,,)"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Replace a string after selection
TEST_F(FormulaTest, definite_integral5)
{
    Start(600);

    document.InsertDefiniteIntegral(true);
    document.InsertString("0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("123", true);
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,123,,)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 2, 1})) << document.GetEditorState().ToString();

    //replace the upper limit
    document.WaitTask(document.InsertString("5", true));
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,5,,)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,123,,)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 2, 1})) << document.GetEditorState().ToString();

    //replace the integrand
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.InsertString("55", true);
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 3, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("x", true));
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,123,x,)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,123,55,)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 3, 1})) << document.GetEditorState().ToString();

    //replace the integration variable
    for (int i = 0; i < 2; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("t", true));
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 5, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 5, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("y", true));
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,123,55,y)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 5, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,123,55,t)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 5, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 5, 1})) << document.GetEditorState().ToString();

    //replace the lower limit
    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("1", true));
    ASSERT_TRUE(document.ToText() == U"definite_integral(1,123,55,t)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,123,55,t)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
}

//The "d" string is not editable and is skipped by the caret
TEST_F(FormulaTest, definite_integral6)
{
    Start(600);

    document.InsertDefiniteIntegral(true);
    document.InsertString("0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);

    //caret skips the "d" and lands in the integration variable
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 5, 0, 0})) << document.GetEditorState().ToString();

    //typing goes into the variable, not the "d"
    document.WaitTask(document.InsertString("5", true));
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,1,x,5)") << ToBasicString(document.ToText());

    //caret left from the variable also skips the "d" and lands at the end of the integrand
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 5, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 1})) << document.GetEditorState().ToString();
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,1,x,5)") << ToBasicString(document.ToText());
}

//Delete the whole formula by Backspace and Delete around it
TEST_F(FormulaTest, definite_integral7)
{
    Start(600);

    document.InsertDefiniteIntegral(true);
    document.InsertString("0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("x", true));
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,1,x,)") << ToBasicString(document.ToText());

    //Backspace after the formula deletes it
    document.WaitTask(document.MoveCaretEnd(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,1,x,)") << ToBasicString(document.ToText());

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
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,1,x,)") << ToBasicString(document.ToText());
}

//Clear the editable fields by selection and delete
TEST_F(FormulaTest, definite_integral8)
{
    Start(600);

    document.InsertDefiniteIntegral(true);
    document.InsertString("0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("10", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("x", true));
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,10,x,x)") << ToBasicString(document.ToText());

    //clear the variable
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 5, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 5, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,10,x,)") << ToBasicString(document.ToText());

    //clear the integrand
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,10,,)") << ToBasicString(document.ToText());

    //clear the upper limit
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 2})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,,,)") << ToBasicString(document.ToText());

    //clear the lower limit
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"definite_integral(,,,)") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,,,)") << ToBasicString(document.ToText());
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,10,,)") << ToBasicString(document.ToText());
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,10,x,)") << ToBasicString(document.ToText());
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,10,x,x)") << ToBasicString(document.ToText());
}

//Save and load a document with the integral
TEST_F(FormulaTest, definite_integral9)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.InsertDefiniteIntegral(true);
    document.InsertString("0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("10", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("x", true));
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,10,x,x)") << ToBasicString(document.ToText());

    document.WaitTask(document.Save("integral1.yut"));
    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Load("integral1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,10,x,x)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<munderover>"\
                            "<mo>∫</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>0</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>10</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</munderover>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mi>d</mi>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

}
