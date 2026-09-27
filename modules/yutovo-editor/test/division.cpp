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

TEST_F(FormulaTest, division1)
{
    Start(600);
    
    document.WaitTask(document.InsertDivision(true));
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

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

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
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("1", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>1</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.InsertString("2", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>1</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, division2)
{
    Start(600);
    
    document.WaitTask(document.InsertCodeString("123", true));
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
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

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

    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(100ms);
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

    document.Undo();
    document.WaitUndo();
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

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
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
}

TEST_F(FormulaTest, division3)
{
    Start(600);
    
    document.InsertString("Text", true);
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
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

    document.MoveCaretEnd(false);
    for (int i = 0; i < 6; ++i)
        document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
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
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, division4)
{
    Start(600);
    
    document.InsertString("Text", true);
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
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

    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 4})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, division5)
{
    Start(600);
    
    document.InsertString("Text", true);
    document.WaitTask(document.InsertDivision(true));
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
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
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertString("1", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>1</mi>"\
                                "<mfrac>"\
                                    "<mrow>"\
                                        "<mi></mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi></mi>"\
                                    "</mrow>"\
                                "</mfrac>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, division6)
{
    Start(600);
    
    document.InsertDivision(true);
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
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
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
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
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.InsertString("3", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.InsertString("4", true));
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mfrac>"\
                                    "<mrow>"\
                                        "<mi>3</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>4</mi>"\
                                    "</mrow>"\
                                "</mfrac>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

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
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mfrac>"\
                                    "<mrow>"\
                                        "<mi>3</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>4</mi>"\
                                    "</mrow>"\
                                "</mfrac>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertString("5", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mfrac>"\
                                    "<mrow>"\
                                        "<mi>3</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>4</mi>"\
                                    "</mrow>"\
                                "</mfrac>"\
                                "<mi>5</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
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

TEST_F(FormulaTest, division7)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("Text", true));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Tex</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>t</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Text</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Tex</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>t</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, division8)
{
    Start(600);
    
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.InsertString("45", true));
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
                                "<mi>45</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>12345</mi>"\
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
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>45</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>12345</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, division9)
{
    Start(600);
    
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.MoveCaretRight(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, division10)
{
    Start(600);
    
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));

    for (int i = 0; i < 5; ++i)
        document.WaitTask(document.MoveCaretRight(false));
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("456", true));

    for (int i = 0; i < 5; ++i)
        document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("Text", true));
    std::this_thread::sleep_for(100ms);
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
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>456</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2, 4})) << document.GetEditorState().ToString();

    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
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
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0})) << document.GetEditorState().ToString();
}

//Save/Load
TEST_F(FormulaTest, division11)
{
    Start(600);
    
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("3", true));
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.InsertString("2", true));
    document.Save("division11_1.yut");

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
    document.Load("division11_1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>3</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.InsertString("123", true));
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertString("Text", true));
    document.Save("division11_2.yut");

    document.WaitTask(document.New());
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    std::this_thread::sleep_for(200ms);
    document.Load("division11_2.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">123</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>3</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2, 4})) << document.GetEditorState().ToString();
}

//Insert a char in the operation sign
TEST_F(FormulaTest, division12)
{
    Start(600);
    
    document.WaitTask(document.InsertDivision(true));
    document.WaitTask(document.MoveCaretDown(false));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.InsertString("3", true));
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
}

//Insert division after selection
TEST_F(FormulaTest, division13)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("45", true);
    document.InsertPlus(true);
    document.InsertString("7", true);
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    for (int i = 0; i < 2; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToText() == U"1(23)/()+45+7") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Insert division after selection
TEST_F(FormulaTest, division14)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("45", true);
    document.InsertPlus(true);
    document.InsertString("7", true);
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToText() == U"1(23+4)/()5+7") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Insert division after selection
TEST_F(FormulaTest, division15)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPower(true);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertPlus(true);
    document.InsertString("45", true);
    document.InsertPlus(true);
    document.InsertString("7", true);
    document.MoveCaretHome(false);
    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToText() == U"(pow(123,2)+4)/()5+7") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Insert division after selection
TEST_F(FormulaTest, division16)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("88", true);
    document.InsertMultiply(true);
    document.InsertString("7", true);
    document.InsertDivision(true);
    document.InsertString("12", true);
    document.MoveCaretRight(false);
    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToText() == U"(88*(7)/(12))/()") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Insert division after selection
TEST_F(FormulaTest, division17)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("88", true);
    document.InsertMultiply(true);
    document.InsertString("7", true);
    document.InsertDivision(true);
    document.InsertString("12", true);
    document.MoveCaretRight(false);
    document.InsertPlus(true);
    document.InsertString("3", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToText() == U"8(8*(7)/(12))/()+3") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Insert division after selection
TEST_F(FormulaTest, division18)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("88", true);
    document.InsertMultiply(true);
    document.InsertString("7", true);
    document.InsertDivision(true);
    document.InsertString("12", true);
    document.MoveCaretRight(false);
    document.InsertPlus(true);
    document.InsertString("3", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToText() == U"88*((7)/(12))/()+3") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Insert division after selection
TEST_F(FormulaTest, division19)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("88", true);
    document.InsertMultiply(true);
    document.InsertString("7", true);
    document.InsertDivision(true);

    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 1})) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.InsertString("12", true);
    document.MoveCaretRight(false);
    document.InsertPlus(true);
    document.InsertString("3", true);
    for (int i = 0; i < 6; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToText() == U"(88*(7)/(12)+3)/()") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Replace a string after selection
TEST_F(FormulaTest, division20)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("88", true);
    document.InsertDivision(true);
    document.InsertString("789", true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.InsertString("5", true));
    ASSERT_TRUE(document.ToText() == U"(88)/(5)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"(88)/(789)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 2, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"(88)/(5)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("6", true));
    ASSERT_TRUE(document.ToText() == U"(88)/(56)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 2})) << document.GetEditorState().ToString();

    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertString("5", true));
    ASSERT_TRUE(document.ToText() == U"(5)/(56)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"(88)/(56)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
}

//Select a division upside
TEST_F(FormulaTest, division21)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("88", true);
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("789", true));
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Select a division downside
TEST_F(FormulaTest, division22)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("88", true);
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("789", true));
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();
}

}
