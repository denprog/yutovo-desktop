/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"
#include "str.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

TEST_F(FormulaTest, power1)
{
    Start(600);

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
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
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

    document.WaitTask(document.InsertString("2", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("3", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>3</mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, power2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertPower(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
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
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, power3)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPower(true);
    document.InsertString("5", true);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>1235</mi>"\
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
                        "<msup>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>1235</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, power4)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertPower(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>12</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>3</mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, power5)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.MoveCaretLeft(false));
    document.InsertPower(true);
    document.WaitTask(document.MoveCaretRight(false));
    document.InsertPower(true);
    document.WaitTask(document.InsertString("5", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>12</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<msup>"\
                                    "<mrow>"\
                                        "<mi>3</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>5</mi>"\
                                    "</mrow>"\
                                "</msup>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>12</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>3</mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>12</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<msup>"\
                                    "<mrow>"\
                                        "<mi>3</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>5</mi>"\
                                    "</mrow>"\
                                "</msup>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>12</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>35</mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>12</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<msup>"\
                                    "<mrow>"\
                                        "<mi>3</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>5</mi>"\
                                    "</mrow>"\
                                "</msup>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

//Save/Load
TEST_F(FormulaTest, power6)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.MoveCaretLeft(false));
    document.InsertPower(true);
    document.WaitTask(document.MoveCaretRight(false));
    document.InsertPower(true);
    document.InsertString("5", true);
    document.Save("power6_1.yut");

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

    document.Load("power6_1.yut");
    document.WaitLoad();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>12</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<msup>"\
                                    "<mrow>"\
                                        "<mi>3</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>5</mi>"\
                                    "</mrow>"\
                                "</msup>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

//Power element in a division
TEST_F(FormulaTest, power7)
{
    Start(600);

    document.WaitTask(document.InsertDivision(true));
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("4567", true);
    document.WaitTask(document.InsertPower(true));
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
                                "<msup>"\
                                    "<mrow>"\
                                        "<mi>4567</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi></mi>"\
                                    "</mrow>"\
                                "</msup>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0})) << document.GetEditorState().ToString();

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
                                "<mi></mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>4567</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 4})) << document.GetEditorState().ToString();
}

//Insert power after selection
TEST_F(FormulaTest, power8)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("45", true);
    document.InsertPlus(true);
    document.InsertString("7", true);
    document.MoveCaretHome(false);
    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.InsertPower(true));
    ASSERT_TRUE(document.ToText() == U"pow(123,)+45+7") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Insert power after selection
TEST_F(FormulaTest, power9)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("45", true);
    document.InsertPlus(true);
    document.InsertString("7", true);
    document.MoveCaretHome(false);
    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.InsertPower(true));
    ASSERT_TRUE(document.ToText() == U"pow((123+),)45+7") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"123+45+7") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.InsertPower(true));
    ASSERT_TRUE(document.ToText() == U"pow((123+4),)5+7") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Insert power after selection
TEST_F(FormulaTest, power10)
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
    document.WaitTask(document.InsertPower(true));
    ASSERT_TRUE(document.ToText() == U"1pow(23,)+45+7") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Insert power after selection
TEST_F(FormulaTest, power11)
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
    document.WaitTask(document.InsertPower(true));
    ASSERT_TRUE(document.ToText() == U"1pow((23+4),)5+7") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Check fonts sizes
TEST_F(FormulaTest, power12)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPower(true);
    document.WaitTask(document.InsertString("45", true));
    auto el = document.FindByString({0}, U"45");
    String* str = (String*)el.get();
    ASSERT_TRUE(str->format->size == 12);

    document.InsertPower(true);
    document.WaitTask(document.InsertString("6", true));
    el = document.FindByString({0}, U"123");
    str = (String*)el.get();
    ASSERT_TRUE(str->format->size == 14);
    el = document.FindByString({0}, U"45");
    str = (String*)el.get();
    ASSERT_TRUE(str->format->size == 12);
    el = document.FindByString({0}, U"6");
    str = (String*)el.get();
    ASSERT_TRUE(str->format->size == 10);

    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"pow(123,456)") << ToBasicString(document.ToText());
    el = document.FindByString({0}, U"123");
    str = (String*)el.get();
    ASSERT_TRUE(str->format->size == 14);
    el = document.FindByString({0}, U"456");
    str = (String*)el.get();
    ASSERT_TRUE(str->format->size == 12);

    document.Undo();
    document.WaitUndo();
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"pow(123,456)") << ToBasicString(document.ToText());
    el = document.FindByString({0}, U"123");
    str = (String*)el.get();
    ASSERT_TRUE(str->format->size == 14);
    el = document.FindByString({0}, U"456");
    str = (String*)el.get();
    ASSERT_TRUE(str->format->size == 12);
}

//Check inner power elements
TEST_F(FormulaTest, power13)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPower(true);
    document.InsertString("45", true);
    document.InsertPower(true);
    document.InsertString("7", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("88", true));
    ASSERT_TRUE(document.ToText() == U"pow(123,pow(45,7+88))") << ToBasicString(document.ToText());

    document.MoveCaretHome(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>457</mi>"\
                                "<mo>+</mo>"\
                                "<mi>88</mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"pow(123,pow(45,7+88))") << ToBasicString(document.ToText());
}

//Copy-paste
TEST_F(FormulaTest, power14)
{
    Start(600);

    document.InsertPower(true);
    document.MoveCaretLeft(false);
    document.MoveCaretRight(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"pow(,)pow(,)"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"pow(,)"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"pow(,)pow(,)"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, power15)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    for (int i = 0; i < 5; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertPower(true));
    ASSERT_TRUE(document.ToText() == U"pow((123+1),)") << ToBasicString(document.ToText());
}

//Select an element and insert power
TEST_F(FormulaTest, power16)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("3", true);
    document.InsertDivision(true);
    document.InsertString("12", true);
    document.InsertPlus(true);
    document.InsertString("2", true);
    document.InsertDivision(true);
    document.InsertString("3", true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertPower(true));
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
                                "<msup>"\
                                    "<mrow>"\
                                        "<mi>12</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi></mi>"\
                                    "</mrow>"\
                                "</msup>"\
                                "<mo>+</mo>"\
                                "<mfrac>"\
                                    "<mrow>"\
                                        "<mi>2</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>3</mi>"\
                                    "</mrow>"\
                                "</mfrac>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
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
                                "<mi>12</mi>"\
                                "<mo>+</mo>"\
                                "<mfrac>"\
                                    "<mrow>"\
                                        "<mi>2</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>3</mi>"\
                                    "</mrow>"\
                                "</mfrac>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0, 2}, 0, 1})) << document.GetEditorState().ToString();
}

//Insert power after a closing bracket
TEST_F(FormulaTest, power17)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("3", true);
    document.InsertPlus(true);
    document.InsertString("2", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertPower(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mo>(</mo>" \
                                "<mi>3</mi>"\
                                "<mo>+</mo>"\
                                "<mi>2</mi>"\
                                "<mo>)</mo>" \
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>(</mo>" \
                        "<mi>3</mi>"\
                        "<mo>+</mo>"\
                        "<mi>2</mi>"\
                        "<mo>)</mo>" \
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 5})) << document.GetEditorState().ToString();
}

//Select a power upside
TEST_F(FormulaTest, power18)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("345", true);
    document.InsertPower(true);
    document.WaitTask(document.InsertString("56", true));
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Select a power downside
TEST_F(FormulaTest, power19)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("345", true);
    document.InsertPower(true);
    document.WaitTask(document.InsertString("56", true));
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Insert brackets if a row contains more then one element
TEST_F(FormulaTest, power20)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("345", true);
    document.InsertPlus(true);
    document.InsertString("77", true);
    document.MoveCaretHome(true);
    document.WaitTask(document.InsertPower(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mo>(</mo>" \
                                "<mi>345</mi>"\
                                "<mo>+</mo>"\
                                "<mi>77</mi>"\
                                "<mo>)</mo>" \
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>345</mi>"\
                        "<mo>+</mo>"\
                        "<mi>77</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mo>(</mo>" \
                                "<mi>345</mi>"\
                                "<mo>+</mo>"\
                                "<mi>77</mi>"\
                                "<mo>)</mo>" \
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Insert brackets if a row contains more then one element
TEST_F(FormulaTest, power21)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("345", true);
    document.InsertPlus(true);
    document.InsertString("77", true);
    document.MoveCaretHome(false);
    document.MoveCaretEnd(true);
    document.WaitTask(document.InsertPower(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mo>(</mo>" \
                                "<mi>345</mi>"\
                                "<mo>+</mo>"\
                                "<mi>77</mi>"\
                                "<mo>)</mo>" \
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>345</mi>"\
                        "<mo>+</mo>"\
                        "<mi>77</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2, 2}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mo>(</mo>" \
                                "<mi>345</mi>"\
                                "<mo>+</mo>"\
                                "<mi>77</mi>"\
                                "<mo>)</mo>" \
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Set font to both parts
TEST_F(FormulaTest, power22)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPower(true);
    document.InsertString("45", true);
    document.MoveCaretHome(true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetFontSize(24));

    auto el = document.FindByString({0}, U"123");
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 24);

    el = document.FindByString({0}, U"45");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 22);
}

//Insert power after selection
TEST_F(FormulaTest, power23)
{
    Start(600);

    document.Load("../../test/tests/power23.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(1s);

    document.WaitTask(document.InsertPower(true));
    document.WaitTask(document.InsertString("2", true));
    ASSERT_TRUE(document.ToText() == U"expand(pow((pow(x,3)+(x)/(2)),2))") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"expand(pow(x,3)+(x)/(2))") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 5}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 2, 3})) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertPower(true));
    document.WaitTask(document.InsertString("2", true));
    ASSERT_TRUE(document.ToText() == U"expand(pow((pow(x,3)+(x)/(2)),2))") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"expand(pow(x,3)+(x)/(2))") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 2, 3})) << document.GetEditorState().ToString();
}

}
