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

//Insert sum and edit it
TEST_F(FormulaTest, sum1)
{
    Start(600);

    document.WaitTask(document.InsertSum(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<munderover>"\
                            "<mo>Σ</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                                "<mo>=</mo>"\
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
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

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
                            "<mo>Σ</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                                "<mo>=</mo>"\
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
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"sum(=,,)") << ToBasicString(document.ToText());

    document.MoveCaretRight(false);
    document.InsertString(U"i", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"10", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString(U"1", true));
    ASSERT_TRUE(document.ToText() == U"sum(i=0,10,1)") << ToBasicString(document.ToText());

    for (int i = 0; i < 5; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"1") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"sum(i=0,10,1)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

//Insert division in a selected string inside expression
TEST_F(FormulaTest, sum2)
{
    Start(600);

    document.WaitTask(document.InsertSum(true));
    for (int i = 0; i < 5; ++i)
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
                            "<mo>Σ</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                                "<mo>=</mo>"\
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
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 1, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Check selection between elements
TEST_F(FormulaTest, sum3)
{
    Start(600);

    document.InsertSum(true);
    document.InsertString("n", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("1", true);
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.InsertString("20", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("123", true));

    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0}, 3, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 2})) << document.GetEditorState().ToString();
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.MoveCaretRight(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, 1, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretLeft(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0, 3, 0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0, 3, 0}, 0, 2})) << document.GetEditorState().ToString();
}

}
