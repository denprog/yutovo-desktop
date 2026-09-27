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

TEST_F(FormulaTest, product1)
{
    Start(600);

    document.WaitTask(document.InsertProduct(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<munderover>"\
                            "<mo>∏</mo>"\
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
                            "<mo>∏</mo>"\
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
    ASSERT_TRUE(document.ToText() == U"prod(=,,)") << ToBasicString(document.ToText());

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
    ASSERT_TRUE(document.ToText() == U"prod(i=0,10,1)") << ToBasicString(document.ToText());

    for (int i = 0; i < 5; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"1") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"prod(i=0,10,1)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

//Replace a string after selection
TEST_F(FormulaTest, product2)
{
    Start(600);

    document.InsertProduct(true);
    document.InsertString("i", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    for (int i = 0; i < 5; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    document.InsertString("123", true);
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.ToText() == U"prod(i=2,123,)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 2, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("5", true));
    ASSERT_TRUE(document.ToText() == U"prod(i=2,5,)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"prod(i=2,123,)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 2, 1})) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.InsertString("55", true);
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 3, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("5", true));
    ASSERT_TRUE(document.ToText() == U"prod(i=2,123,5)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"prod(i=2,123,55)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 3, 1})) << document.GetEditorState().ToString();
}

}
