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

TEST_F(FormulaTest, plus1)
{
    Start(600);

    document.WaitTask(document.InsertPlus(true));
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
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, plus2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("45", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mi>45</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertPlus(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    for (int i = 0; i < 3; ++i)
        document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, plus3)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertPlus(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>+</mo>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, plus4)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("45", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("3456", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                        "<mi>3456</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 4, 4})) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 3}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 1, 2}, ElementSelectionState{{0, 0, 0, 0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2, 1}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 1, 2}, ElementSelectionState{{0, 0, 0, 0, 0, 0}, 1, 1},
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 2}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2, 2}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 1, 2}, ElementSelectionState{{0, 0, 0, 0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 4, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 1, 2}, ElementSelectionState{{0, 0, 0, 0, 0, 0}, 1, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 4, 1}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 1, 2}, ElementSelectionState{{0, 0, 0, 0, 0, 0}, 1, 3}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 4}, 0, 1})) << document.GetEditorState().ToString();

    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 4, 4}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 1, 2}, ElementSelectionState{{0, 0, 0, 0, 0, 0}, 1, 4})) << document.GetEditorState().ToString();
    
    document.MoveCaretRight(false);
    document.MoveCaretLeft(false);
    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 3}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 3, 1}, ElementSelectionState{{0, 0, 0, 0, 0, 0, 4}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2, 1}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 3, 1}, ElementSelectionState{{0, 0, 0, 0, 0, 0, 4}, 0, 3}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 2}, 1, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 1, 3},
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 4}, 0, 3})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 1, 2},
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 1, 3},
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 4}, 0, 3})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, plus5)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertPlus(true));
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 0})) << document.GetEditorState().ToString();

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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Save/Load
TEST_F(FormulaTest, plus6)
{
    Start(600);
    
    document.InsertString("123", true);
    document.InsertCode(false, true);
    document.WaitTask(document.InsertPlus(true));
    document.Save("plus6_1.yut");

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
    document.Load("plus6_1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">123</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertString("5", true));
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("67", true));

    document.Save("plus6_2.yut");

    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    std::this_thread::sleep_for(200ms);
    document.Load("plus6_2.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">123</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>5</mi>"\
                        "<mo>+</mo>"\
                        "<mi>67</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 2, 2})) << document.GetEditorState().ToString();
}

//Copy-paste
TEST_F(FormulaTest, plus7)
{
    Start(600);

    document.InsertPlus(true);
    document.MoveCaretRight(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretRight(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>+</mo>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"+"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"++"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

}
