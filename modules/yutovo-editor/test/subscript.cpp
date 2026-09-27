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

TEST_F(FormulaTest, subscript1)
{
    Start(600);

    document.WaitTask(document.InsertSubscript(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</msub>"\
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
                        "<msub>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</msub>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("x", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"\
                            "<mrow>"\
                                "<mi>x</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</msub>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("3", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"\
                            "<mrow>"\
                                "<mi>x</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>3</mi>"\
                            "</mrow>"\
                        "</msub>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, subscript2)
{
    Start(600);

    document.WaitTask(document.InsertSubscriptFunction("log", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"\
                            "<mrow>"\
                                "<mi>log</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</msub>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

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
                        "<msub>"\
                            "<mrow>"\
                                "<mi>log</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</msub>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Copy-paste
TEST_F(FormulaTest, subscript3)
{
    Start(600);

    document.InsertSubscript(true);
    document.MoveCaretLeft(false);
    document.MoveCaretRight(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretRight(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"{}{}"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"{}"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"{}{}"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Check fonts after undo
TEST_F(FormulaTest, subscript4)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("log", true);
    document.InsertSubscript(true);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("8", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"\
                            "<mrow>"\
                                "<mi>log</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                        "</msub>"\
                        "<mi>8</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    for (int i = 0; i < 3; ++i)
    {
        document.Undo();
        document.WaitUndo();
    }

    std::this_thread::sleep_for(100ms);
    document.InsertSubscript(true);
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);
    std::this_thread::sleep_for(100ms);
    StringFormatPtr cur_fmt;
    document.GetCurrentStringFormat(cur_fmt);
    document.WaitTask(document.InsertString("8", true));

    StringFormat format;
    auto el = document.FindByString({0}, U"log");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 14);
    el = document.FindByString({0}, U"2");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 12) << format.size;
    el = document.FindByString({0}, U"8");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 14) << format.size;
}

}
