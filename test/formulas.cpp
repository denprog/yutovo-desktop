/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"
#include "formulas/code_string.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

TEST_F(FormulaTest, delete1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertPlus(true));
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.InsertPlus(true);
    document.InsertPlus(true);
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();
}

//Delete with undo a text and a code block
TEST_F(FormulaTest, delete2)
{
    Start(600);

    document.InsertString("Text", true);
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(true);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Te</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Te</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Delete with undo a text and a code block and a text
TEST_F(FormulaTest, delete3)
{
    Start(600);

    document.InsertString("Text", true);
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.WaitTask(document.MoveCaretEnd(false));
    document.InsertString("Block", true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    for (int i = 0; i < 5; ++i)
        document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 2}, 0, 3})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Teck</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Block</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 2}, 0, 3})) << document.GetEditorState().ToString();
    
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    for (int i = 0; i < 5; ++i)
        document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 2}, 0, 3})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Teck</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Block</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 2}, 0, 3})) << document.GetEditorState().ToString();
}

//Delete a selected code block at the beginning of the text
TEST_F(FormulaTestCustom, delete4)
{
    Start(443);

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0})) << document.GetEditorState().ToString();
}

//Deletion of a selected formula
TEST_F(FormulaTestCustom, delete5)
{
    Start(450);

    document.WaitTask(document.InsertDivision(true));
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Deletion of a selected formula with a text
TEST_F(FormulaTestCustom, delete6)
{
    Start(380);

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.InsertDivision(true));
    for (int i = 0; i < 17; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.InsertDivision(true));
    for (int i = 0; i < 14; ++i)
        document.MoveCaretLeft(false);
    for (int i = 0; i < 16; ++i)
        document.MoveCaretRight(true);
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
                "<span style=\"font-family:'Arial';font-size:14px;\">Th text itself is a little strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
                "<span style=\"font-family:'Arial';font-size:14px;\"> the text itself </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">is a little strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 3, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 1}, 2, 11},
        ElementSelectionState{ElementId{0, 0, 0}, 2, 1},
        ElementSelectionState{ElementId{0, 0, 0, 3}, 0, 4})) << document.GetEditorState().ToString();
}

//Delete an element on the left of a string
TEST_F(FormulaTestCustom, delete7)
{
    Start(380);

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.InsertString("55", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>d55</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>d</mi>"\
                        "<mo>+</mo>"\
                        "<mi>55</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>d55</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Delete a character from the right
TEST_F(FormulaTestCustom, delete8)
{
    Start(380);

    document.InsertCode(false, true);
    document.InsertString("45", true);
    document.InsertPlus(true);
    document.InsertString("6", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>4</mi>"\
                        "<mo>+</mo>"\
                        "<mi>6</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                        "<mi>6</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>4</mi>"\
                        "<mo>+</mo>"\
                        "<mi>6</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Delete an element from the right
TEST_F(FormulaTestCustom, delete9)
{
    Start(380);

    document.InsertCode(false, true);
    document.InsertString("45", true);
    document.InsertPlus(true);
    document.InsertPlus(true);
    document.InsertMinus(true);
    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                        "<mo>+</mo>"\
                        "<mo>-</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                        "<mo>-</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                        "<mo>+</mo>"\
                        "<mo>-</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();
}

//Delete a code block on the left
TEST_F(FormulaTestCustom, delete10)
{
    Start(378);

    document.InsertString("The source of the text itself is a little mysterious.", true);
    document.WaitTask(document.InsertCode(false, true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 11})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">mysterious.</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 2})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, delete11)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertPlus(true);
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, delete12)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("45", true);
    document.MoveCaretHome(false);
    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"45") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"123+45") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"45") << ToBasicString(document.ToText());
}

TEST_F(FormulaTest, delete13)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("45", true);
    document.MoveCaretHome(false);
    for (int i = 0; i < 3; ++i)
        document.MoveCaretRight(false);
    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"123") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"123+45") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"123") << ToBasicString(document.ToText());
}

//Delete an empty symbol
TEST_F(FormulaTest, delete14)
{
    Start(600);

    document.Load("../../test/tests/delete14.yut");
    document.WaitLoad();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                        "<mi></mi>"\
                        "<mi>f</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                        "<mi></mi>"\
                        "<mi>f</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                        "<mi>f</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                        "<mi></mi>"\
                        "<mi>f</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 0})) << document.GetEditorState().ToString();
}

//Delete an empty symbol
TEST_F(FormulaTest, delete15)
{
    Start(600);

    document.Load("../../test/tests/delete15.yut");
    document.WaitLoad();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>x</mi>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                        "<mi></mi>"\
                        "<mi>f</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>x</mi>"\
                        "<mi></mi>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                        "<mi></mi>"\
                        "<mi>f</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>x</mi>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                        "<mi></mi>"\
                        "<mi>f</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>x</mi>"\
                        "<mi></mi>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                        "<mi></mi>"\
                        "<mi>f</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1, 0})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTestCustom, insert1)
{
    Start(460);

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("Tradicionalmente, el medio de un documento era el papel y la información", true));
    std::this_thread::sleep_for(400ms);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Tradicionalmente, el medio de </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">un</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> documento era el papel </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">y la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Tradicionalmente, el medio de </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">un documento era el papel y </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Tradicionalmente, el medio de </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">un</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> documento era el papel </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">y la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Insert code in text and reformat text
TEST_F(FormulaTestCustom, insert2)
{
    Start(443);

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.WaitTask(document.MoveCaretWordLeft(false));
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
                "<span style=\"font-family:'Arial';font-size:14px;\">strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 42})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
                "<span style=\"font-family:'Arial';font-size:14px;\">strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Insert formula, remake and undo
TEST_F(FormulaTestCustom, insert3)
{
    Start(380);

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    std::this_thread::sleep_for(200ms);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertDivision(true));
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 42})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
                "<span style=\"font-family:'Arial';font-size:14px;\">strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Insert a formula at the end of a string
TEST_F(FormulaTestCustom, insert4)
{
    Start(432);

    document.WaitTask(document.InsertString("Text Text Text Text Text Text Text Text 3", true));
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(100ms);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.InsertString("f", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text Text Text Text Text Text Text Text 3f</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 42})) << document.GetEditorState().ToString();
}

//Insert a paragraph
TEST_F(FormulaTestCustom, insert5)
{
    Start(432);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("56890", true);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                            "<mo>+</mo>"\
                            "<mi>568</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>90</mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mi>56890</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 3})) << document.GetEditorState().ToString();

    for (int i = 0; i < 4; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mo>+</mo>"\
                            "<mi>56890</mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mi>56890</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Inserting a paragraph is prohibited
TEST_F(FormulaTestCustom, insert6)
{
    Start(432);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertDivision(true);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretEnd(false);
    document.InsertString("56", true);
    document.InsertPower(true);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>56</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1, 1})) << document.GetEditorState().ToString();
}

//Insert a paragraph
TEST_F(FormulaTestCustom, insert7)
{
    Start(432);

    document.InsertCode(false, true);
    document.InsertString("3", true);
    document.InsertPlus(true);
    document.InsertString("123", true);
    document.InsertDivision(true);
    for (int i = 0; i < 6; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>3</mi>"\
                            "<mo>+</mo>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>123</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>3</mi>"\
                        "<mo>+</mo>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Insert a paragraph
TEST_F(FormulaTestCustom, insert8)
{
    Start(432);

    document.InsertCode(false, true);
    document.InsertString("3", true);
    document.InsertMultiply(true);
    document.InsertString("56", true);
    document.InsertPower(true);
    for (int i = 0; i < 5; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>3</mi>"\
                            "<mo>×</mo>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<msup>"\
                                "<mrow>"\
                                    "<mi>56</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</msup>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>3</mi>"\
                        "<mo>×</mo>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>56</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Insert, resize, undo and redo
TEST_F(FormulaTestCustom, insert9)
{
    Start(500);

    int width = 500;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("Tradicionalmente, el medio de un documento era el papel y la información", true);
    document.InsertCode(false, true);
    document.WaitTask(document.InsertSquareRoot(true));

    width = 800;
    document.WaitTask(document.Resize(width, 400));

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();

    ASSERT_TRUE(document.ToText() == 
        U"Tradicionalmente, el medio de un documento era el papel y la información"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 72})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"Tradicionalmente, el medio de un documento era el papel y la información"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"Tradicionalmente, el medio de un documento era el papel y la informaciónsqrt()"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();
}

//Insert minus inside a string
TEST_F(FormulaTestCustom, insert10)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("750", true);
    document.InsertMinus(true);
    document.InsertString("18", true);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertMinus(true));
    ASSERT_TRUE(document.ToText() == 
        U"750-1-8"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"750-18"
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"750-1-8"
        ) << ToBasicString(document.ToText());
}

//Insert minus inside a string
TEST_F(FormulaTestCustom, insert11)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("750", true);
    document.InsertMinus(true);
    document.InsertString("18", true);
    document.MoveCaretHome(true);
    document.InsertDivision(true);
    document.InsertString("3", true);
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertMinus(true));
    ASSERT_TRUE(document.ToText() == 
        U"(750-1-8)/(3)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"(750-18)/(3)"
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"(750-1-8)/(3)"
        ) << ToBasicString(document.ToText());
}

//Insert a formula before a code block
TEST_F(FormulaTestCustom, insert12)
{
    Start(500);

    int width = 500;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.InsertSquareRoot(true));
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
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();
}

//Replace mode
TEST_F(FormulaTestCustom, insert13)
{
    Start(500);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("1234", true));
    document.SwitchInsertMode();
    document.MoveCaretHome(false);
    document.WaitTask(document.InsertString("5", true));
    ASSERT_TRUE(document.ToText() == 
        U"5234"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"1234"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Replace mode
TEST_F(FormulaTestCustom, insert14)
{
    Start(500);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("1234", true));
    document.SwitchInsertMode();
    document.MoveCaretHome(false);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.InsertString("5", true));
    ASSERT_TRUE(document.ToText() == 
        U"534"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.InsertString("8", true));
    ASSERT_TRUE(document.ToText() == U"584") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"534"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"1234"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();
}

//Replace mode
TEST_F(FormulaTestCustom, insert15)
{
    Start(500);

    document.InsertDivision(true);
    document.WaitTask(document.InsertString("3", true));
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.InsertString("25", true));
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretHome(false));
    document.SwitchInsertMode();
    document.WaitTask(document.InsertString("5", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>5</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"(3)/(25)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertPower(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"(3)/(25)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Replace mode
TEST_F(FormulaTestCustom, insert16)
{
    Start(500);

    document.InsertString("1234", true);
    document.SwitchInsertMode();
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToText() == U"123()/()") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.SwitchInsertMode();
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"1234") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">123</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Replace mode
TEST_F(FormulaTestCustom, insert17)
{
    Start(500);

    document.InsertString("1234", true);
    document.SwitchInsertMode();
    document.MoveCaretHome(false);
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToText() == U"()/()234") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.SwitchInsertMode();
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"1234") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
                "<span style=\"font-family:'Arial';font-size:14px;\">234</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Replace mode
TEST_F(FormulaTestCustom, insert18)
{
    Start(500);

    document.InsertString("1", true);
    document.SwitchInsertMode();
    document.MoveCaretHome(false);
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToText() == U"()/()") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.SwitchInsertMode();
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"1") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Replace mode
TEST_F(FormulaTestCustom, insert19)
{
    Start(500);

    document.InsertString("1", true);
    document.SwitchInsertMode();
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToText() == U"1()/()") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.SwitchInsertMode();
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"1") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">1</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Replace mode
TEST_F(FormulaTestCustom, insert20)
{
    Start(500);

    document.InsertString("1234", true);
    document.SwitchInsertMode();
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToText() == U"12()/()4") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.SwitchInsertMode();
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"1234") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">12</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
                "<span style=\"font-family:'Arial';font-size:14px;\">4</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Selection of a formula
TEST_F(FormulaTestCustom, select1)
{
    Start(600);

    document.InsertDivision(true);
    document.WaitTask(document.InsertString("3", true));
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.InsertString("25", true));
    document.MoveCaretLeft(false);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection of a formula at the beginning of a text
TEST_F(FormulaTestCustom, select2)
{
    Start(600);

    document.InsertString("text", true);
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.InsertDivision(true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection of a formula at the end of a row
TEST_F(FormulaTestCustom, select3)
{
    Start(430);

    int width = 430;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(200ms);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.MoveCaretRight(true));
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 2})) << document.GetEditorState().ToString();

    width = 400;
    document.WaitTask(document.Resize(width, 400));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 1, 2}, 
        ElementSelectionState{ElementId{0, 0, 1}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 1, 1}, 0, 2})) << document.GetEditorState().ToString();
}

//Selection of a child element of a formula at the end of a row
TEST_F(FormulaTestCustom, select4)
{
    Start(400);

    int width = 400;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 1, 0, 0, 0, 0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();

    width = 445;
    document.WaitTask(document.Resize(width, 400));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 1, 0, 0, 0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Selection of a part of row, a formula and a part of row
TEST_F(FormulaTestCustom, select5)
{
    Start(400);

    int width = 400;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    std::this_thread::sleep_for(200ms);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertDivision(true));
    for (int i = 0; i < 6; ++i)
        document.MoveCaretLeft(false);
    for (int i = 0; i < 7; ++i)
        document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 1, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 39, 3},
        ElementSelectionState{ElementId{0, 0, 1}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 1, 1}, 0, 4})) << document.GetEditorState().ToString();

    width = 420;
    document.WaitTask(document.Resize(width, 400));
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 39, 3},
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 4})) << document.GetEditorState().ToString();

    width = 360;
    document.WaitTask(document.Resize(width, 400));
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 2, 4}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 4, 3},
        ElementSelectionState{ElementId{0, 0, 1}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 1, 2}, 0, 4})) << document.GetEditorState().ToString();

    width = 410;
    document.WaitTask(document.Resize(width, 400));
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 39, 3},
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 4})) << document.GetEditorState().ToString();
}

//Selection rows in a code block
TEST_F(FormulaTest, select6)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertDivision(true);
    document.InsertString("234", true);
    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("123", true);
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    ASSERT_TRUE(document.ToText() == 
        U"(123)/(234)\n" \
        U"(123)/(123)"
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection rows in a code block
TEST_F(FormulaTest, select7)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("12", true);
    document.InsertParagraph(true);
    document.InsertString("23", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("123245345", true));
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection rows in a code block
TEST_F(FormulaTest, select8)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("12", true);
    document.InsertParagraph(true);
    document.InsertString("23", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("123245345", true));
    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0}, 1, 2},
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 1, 2},
        ElementSelectionState{ElementId{0, 0, 0, 0, 1, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Select a code block
TEST_F(FormulaTest, select9)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Select a code block
TEST_F(FormulaTest, select10)
{
    Start(600);

    document.InsertDivision(true);
    document.InsertString("123", true);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection rows in a code block
TEST_F(FormulaTest, select11)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("234", true);
    document.InsertParagraph(true);
    document.InsertString("434", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("3453", true));
    for (int i = 0; i < 5; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 1, 2},
        ElementSelectionState{ElementId{0, 0, 0, 0, 1, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection after a power
TEST_F(FormulaTest, select12)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPower(true);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertPlus(true);
    document.InsertString("45", true);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.ToText() == U"pow(123,2)+45") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 2}, 0, 1})) << document.GetEditorState().ToString();
}

//Replace a selection
TEST_F(FormulaTest, select13)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertPlus(true));
    ASSERT_TRUE(document.ToText() == U"+") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"123") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Replace a selection
TEST_F(FormulaTest, select14)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertPlus(true));
    document.InsertString("55", true);
    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertMinus(true));
    ASSERT_TRUE(document.ToText() == U"123-") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"123+55") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Select a symbol and a complex element
TEST_F(FormulaTest, select15)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("67", true);
    document.InsertPlus(true);
    document.InsertSquareRoot(true);
    document.InsertString("3", true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Select an element and insert an element
TEST_F(FormulaTest, select16)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("3", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.InsertDivision(true);
    document.InsertString("2", true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertSquareRoot(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msqrt>"\
                            "<mrow>"\
                                "<mi>3</mi>"\
                            "</mrow>"\
                        "</msqrt>"\
                        "<mo>+</mo>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>1</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>3</mi>"\
                        "<mo>+</mo>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>1</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
}

//Check caret after undo
TEST_F(FormulaTest, select17)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertDivision(true);
    document.InsertString("34", true);
    document.InsertPlus(true);
    document.InsertString("45", true);
    document.InsertDivision(true);
    document.InsertString("67", true);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.InsertSquareRoot(true));
    ASSERT_TRUE(document.ToText() == 
        U"(123)/(sqrt(34+(45)/(67)))"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.ToText() == 
        U"(123)/(34+(45)/(67))"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertPlus(true));
    ASSERT_TRUE(document.ToText() == 
        U"(123)/(34+(45)/(67)+)"
        ) << ToBasicString(document.ToText());
}

//Cannot select with mouse from a shape
TEST_F(FormulaTest, select18)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertSquareRoot(true);
    document.WaitTask(document.InsertString("123", true));
    auto el = document.FindByType(ElementId{0}, ElementType::SQUARE_ROOT);
    Rect rect1;
    document.GetElementRect(el->id, rect1);
    document.WaitTask(document.Select(rect1.left + 5, rect1.top + 5, rect1.GetRight() + 20, rect1.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    auto el2 = document.FindByType(ElementId{0}, ElementType::CODE_STRING);
    Rect rect2;
    document.GetElementRect(el2->id, rect2);
    document.WaitTask(document.Select(rect1.left + 5, rect1.top + 5, rect2.GetRight() - 5, rect2.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Cannot select with mouse from a shape
TEST_F(FormulaTest, select19)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("56", true));
    auto el = document.FindByType(ElementId{0}, ElementType::SHAPE);
    Rect rect;
    document.GetElementRect(el->id, rect);
    document.WaitTask(document.Select(rect.left + 5, rect.top + 1, rect.left + 10, rect.top + 20));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();
}

//Check caret after moving after selection
TEST_F(FormulaTest, select20)
{
    Start(600);

    document.Load("../../test/tests/select20.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, fonts1)
{
    Start(600);

    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.SetFontSize(12));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
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
    auto s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 12);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
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
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 14);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Set font
TEST_F(FormulaTest, fonts2)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.SetFontSize(12);
    document.WaitTask(document.InsertString("56", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mi>56</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    auto s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 14);
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 1});
    ASSERT_TRUE(s && s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 12);

    document.SetBold(true);
    document.WaitTask(document.InsertString("77", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mi>56</mi>"\
                        "<mi>77</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 2});
    ASSERT_TRUE(s && s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 12);

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mi>56</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 14);
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 1});
    ASSERT_TRUE(s && s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 12);

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 14);
}

//Set color
TEST_F(FormulaTest, fonts3)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.SetColor(Color::Red());
    document.WaitTask(document.InsertString("56", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mi>56</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    auto s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->text_color == Color::Black());
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 1});
    ASSERT_TRUE(s && s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->text_color == Color::Red());

    document.SetBgColor(Color::Blue());
    document.WaitTask(document.InsertString("77", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mi>56</mi>"\
                        "<mi>77</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 2});
    ASSERT_TRUE(s && s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->text_bg_color == Color::Blue());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mi>56</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->text_color == Color::Black());
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 1});
    ASSERT_TRUE(s && s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->text_color == Color::Red());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->text_color == Color::Black());
}

//Set a font attribute
TEST_F(FormulaTest, fonts4)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetBold(true));
    auto s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->bold == true);

    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);

    document.WaitTask(document.SetBold(false));
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->bold == false);
}

//Set a font attribute
TEST_F(FormulaTest, fonts5)
{
    Start(600);

    document.InsertDivision(true);
    document.InsertString("123", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("55", true));
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetItalic(true));

    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, format));
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0}, format));
    ASSERT_FALSE(format.italic);

    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetBold(true));

    // ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0}, format));
    // ASSERT_TRUE(format.bold);
    // ASSERT_FALSE(format.italic);

    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, format));
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(format.bold);

    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);

    document.WaitTask(document.SetBold(false));

    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0}, format));
    ASSERT_FALSE(format.bold);

    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, format));
    ASSERT_TRUE(format.italic);
    ASSERT_FALSE(format.bold);
}

//Set a font attribute
TEST_F(FormulaTest, fonts6)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetBold(true));
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);

    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);

    document.WaitTask(document.InsertString("f", true));
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, format));
    ASSERT_TRUE(format.bold);

    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetUnderline(true));
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(format.underline);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_FALSE(format.underline);

    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertPlus(true));
    document.WaitTask(document.InsertString("55", true));
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 2}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_FALSE(format.underline);

    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetFontSize(16));
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 2}, format));
    ASSERT_TRUE(format.size == 16);
    ASSERT_TRUE(format.bold);
    ASSERT_FALSE(format.underline);

    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertString("1", true));
    ASSERT_TRUE(document.ToText() == 
        U"(123)/(f1)+55"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_FALSE(format.underline);
    ASSERT_TRUE(format.size == 16);

    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_FALSE(format.underline);
    ASSERT_TRUE(format.size == 14);

    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.SetFontSize(22));
    document.WaitTask(document.InsertString("77", true));
    ASSERT_TRUE(document.ToText() == 
        U"(123)/(f1)+5577"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 2}, format));
    ASSERT_TRUE(format.size == 16);
    ASSERT_TRUE(format.bold);
    ASSERT_FALSE(format.underline);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 3}, format));
    ASSERT_TRUE(format.size == 22);
    ASSERT_TRUE(format.bold);
    ASSERT_FALSE(format.underline);
}

//Check font between elements
TEST_F(FormulaTest, fonts7)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("exp", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("345", true);
    document.InsertPlus(true);
    document.InsertString("23", true);
    document.WaitTask(document.InsertCloseRoundBracket(true));
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetFontFamily("Verdana"));

    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(false));
    StringFormat format;
    auto el = document.FindByString({0}, U"exp");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.family == "Verdana");

    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    std::this_thread::sleep_for(200ms);
    StringFormatPtr f;
    document.GetCurrentStringFormat(f);
    ASSERT_TRUE(f->family == "Verdana") << f->family;
}

//Check font between elements
TEST_F(FormulaTest, fonts8)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("exp", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("345", true);
    document.InsertPlus(true);
    document.InsertString("23", true);
    document.InsertCloseRoundBracket(true);
    document.InsertParagraph(true);
    document.InsertString("55", true);
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetFontFamily("Verdana"));

    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(false));
    StringFormat format;
    auto el = document.FindByString({0}, U"exp");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.family == "Verdana");

    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    StringFormatPtr f;
    document.GetCurrentStringFormat(f);
    ASSERT_TRUE(f->family == "Verdana") << f->family;

    document.MoveCaretEnd(false);
    document.GetCurrentStringFormat(f);
    ASSERT_TRUE(f->family == "Verdana") << f->family;
}

//Check font after delete
TEST_F(FormulaTest, fonts9)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPower(true);
    document.InsertString("2", true);
    document.InsertPower(true);
    document.InsertString("3", true);
    document.InsertDivision(true);
    document.InsertString("5", true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == 
        U"pow(123,2(3)/(5))"
        ) << ToBasicString(document.ToText());

    StringFormat format;
    auto el = document.FindByString({0}, U"5");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 12);
}

//Check font after changing it at a part of a row
TEST_F(FormulaTest, fonts10)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("exp", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("23", true);
    document.InsertPlus(true);
    document.InsertString("4", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.MoveCaretHome(true));
    document.InsertDivision(true);
    document.InsertString("5", true);
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    for (int i = 0; i < 6; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetFontSize(18));

    auto el = document.FindByString({0}, U"23");
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 18);
}

//Change font of a result
TEST_F(FormulaTest, fonts11)
{
    Start(600);

    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.ChangeStringFormat("Times New Roman", 22, false, false, false, false, false, false, Color::Black(), Color::White(), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>123.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 1}, 
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();

    auto el = document.FindByString({0}, U"123");
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 22);
        
    el = document.FindByString({0}, U"123.");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 22);

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    document.WaitSolver();

    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>123.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 1}, 
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();

    el = document.FindByString({0}, U"123");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 14);

    el = document.FindByString({0}, U"123.");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 14);
}

//Don't change font of a part of result
TEST_F(FormulaTest, fonts12)
{
    Start(600);

    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("1234", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    document.MoveCaretRight(false);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.ChangeStringFormat("Times New Roman", 22, false, false, false, false, false, false, Color::Black(), Color::White(), true));

    auto el = document.FindByString({0}, U"1234");
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 14);
        
    el = document.FindByString({0}, U"1234.");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 14);

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);

    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>1234</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    el = document.FindByString({0}, U"1234");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 14);
}

//Change font of an equation and check result font
TEST_F(FormulaTest, fonts13)
{
    Start(600);

    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("1234", true);
    document.InsertPlus(true);
    document.InsertString("45", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);

    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.ChangeStringFormat("Times New Roman", 22, true, false, false, false, false, false, Color::Black(), Color::White(), true));

    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    auto el = document.FindByString({0}, U"51279.");
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 22);
    ASSERT_TRUE(format.bold);

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"\n"
        U"1234+45=1279."
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"\n"
        U"1234+45=1279."
        ) << ToBasicString(document.ToText());

    el = document.FindByString({0}, U"1279.");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 14);
    ASSERT_TRUE(format.family == "FreeMono");
    ASSERT_TRUE(!format.bold);

    el = document.FindByString({0}, U"1234");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 14);
    ASSERT_TRUE(format.family == "FreeMono");
    ASSERT_TRUE(!format.bold);
}

//Change font of an equation and check result font
TEST_F(FormulaTest, fonts14)
{
    Start(600);

    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("1234", true);
    document.InsertPlus(true);
    document.InsertString("45", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("88", true);
    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.ChangeStringFormat("Times New Roman", 22, true, false, false, false, false, false, Color::Black(), Color::White(), true));

    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    auto el = document.FindByString({0}, U"51279.");
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 22);
    ASSERT_TRUE(format.bold);

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"\n"
        U"1234+45=1279.\n"
        U"88"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"\n"
        U"1234+45=1279.\n"
        U"88"
        ) << ToBasicString(document.ToText());

    el = document.FindByString({0}, U"1279.");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 14);
    ASSERT_TRUE(format.family == "FreeMono");
    ASSERT_TRUE(!format.bold);

    el = document.FindByString({0}, U"1234");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 14);
    ASSERT_TRUE(format.family == "FreeMono");
    ASSERT_TRUE(!format.bold);
}

//Don't set color after selection
TEST_F(FormulaTest, fonts15)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertParagraph(true);
    document.InsertString("4567", true);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetColor(Color::Red()));
    auto s = document.FindByString({0}, U"123");
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->text_color == Color::Black());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"\
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Check font after solving when caret is inside a string with a different font
TEST_F(FormulaTest, fonts16)
{
    Start(600);

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertParagraph(true);
    document.InsertString("7", true);
    document.InsertString(" ", true);
    document.InsertString("фут", true);
    document.InsertSubscript(true);
    document.InsertString("us", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(4s);

    {
        auto s = document.FindByString({0}, U"7.");
        ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 14);

        document.MoveCaretUp(false);
        document.WaitTask(document.MoveCaretHome(true));
        document.WaitTask(document.SetFontSize(20));
        document.WaitTask(document.MoveCaretRight(false));
        document.WaitTask(document.ReSolve(ElementId{}));
        document.WaitSolver();
        std::this_thread::sleep_for(3s);

        s = document.FindByString({0}, U"us");
        ASSERT_TRUE(((CodeString*)s.get())->GetStringFormat()->size == 12) << ((CodeString*)s.get())->GetStringFormat()->size;
    }
}

//Graph should not recalculate because of reformating paragraph
TEST_F(FormulaTest, fonts17)
{
    Start(700);

    document.Load("../../test/tests/fonts17.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);

    auto s = document.FindByString({0}, U"7.");
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 14) << 
        ((CodeString*)s.get())->GetStringFormat()->size;
    s = document.FindByString(s->parent->id, U"us");
    ASSERT_TRUE(((CodeString*)s.get())->GetStringFormat()->size == 12) << ((CodeString*)s.get())->GetStringFormat()->size;
}

//Replace string
TEST_F(FormulaTest, replace1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertPlus(true, true));
    ASSERT_TRUE(document.ToText() == 
        U"+"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"+"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

}
