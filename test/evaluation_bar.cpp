/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "formulas/division.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

//Insert two variables with values
TEST_F(FormulaTest, evaluation_bar_subscript1)
{
    Start(600);

    document.WaitTask(document.InsertEvaluationBarSubscript(true));
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("x", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("2", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"
                            "<mo fence=\"false\" stretchy=\"true\">|</mo>"
                            "<mrow>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>x</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>2</mi>"
                                    "</mrow>"
                                "</mrow>"
                            "</mrow>"
                        "</msub>"
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.WaitTask(document.InsertParagraph(true));
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("y", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("5", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"
                            "<mo fence=\"false\" stretchy=\"true\">|</mo>"
                            "<mrow>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>x</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>2</mi>"
                                    "</mrow>"
                                "</mrow>"
                                "<mo>,</mo>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>y</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>5</mi>"
                                    "</mrow>"
                                "</mrow>"
                            "</mrow>"
                        "</msub>"
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

//Save/Load
TEST_F(FormulaTest, evaluation_bar_subscript2)
{
    Start(600);

    document.WaitTask(document.InsertEvaluationBarSubscript(true));
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("x", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.InsertParagraph(true));
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("y", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("5", true));
    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.InsertParagraph(true));
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("z", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("3", true));
    document.Save("evaluation_bar2.yut");

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
    document.Load("evaluation_bar2.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(200ms);

    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"
                            "<mo fence=\"false\" stretchy=\"true\">|</mo>"
                            "<mrow>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>x</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>2</mi>"
                                    "</mrow>"
                                "</mrow>"
                                "<mo>,</mo>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>z</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>3</mi>"
                                    "</mrow>"
                                "</mrow>"
                                "<mo>,</mo>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>y</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>5</mi>"
                                    "</mrow>"
                                "</mrow>"
                            "</mrow>"
                        "</msub>"
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

//Don't insert other elements in the rows
TEST_F(FormulaTest, evaluation_bar_subscript3)
{
    Start(600);
    
    document.WaitTask(document.InsertEvaluationBarSubscript(true));
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("x", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.InsertParagraph(true));
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("y", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("5", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("6", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"
                            "<mo fence=\"false\" stretchy=\"true\">|</mo>"
                            "<mrow>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>x</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>2</mi>"
                                    "</mrow>"
                                "</mrow>"
                                "<mo>,</mo>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>y</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>5</mi>"
                                    "</mrow>"
                                "</mrow>"
                            "</mrow>"
                        "</msub>"
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.WaitTask(document.InsertString("t", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"
                            "<mo fence=\"false\" stretchy=\"true\">|</mo>"
                            "<mrow>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>x</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>2</mi>"
                                    "</mrow>"
                                "</mrow>"
                                "<mo>,</mo>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>y</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>5</mi>"
                                    "</mrow>"
                                "</mrow>"
                            "</mrow>"
                        "</msub>"
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0})) << document.GetEditorState().ToString();
}

//Delete elements
TEST_F(FormulaTest, evaluation_bar_subscript4)
{
    Start(600);

    document.Load("../../test/tests/evaluation_bar_subscript_1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"
                            "<mo fence=\"false\" stretchy=\"true\">|</mo>"
                            "<mrow>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>z</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>3</mi>"
                                    "</mrow>"
                                "</mrow>"
                                "<mo>,</mo>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>y</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>5</mi>"
                                    "</mrow>"
                                "</mrow>"
                            "</mrow>"
                        "</msub>"
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"
                            "<mo fence=\"false\" stretchy=\"true\">|</mo>"
                            "<mrow>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>x</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>2</mi>"
                                    "</mrow>"
                                "</mrow>"
                                "<mo>,</mo>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>z</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>3</mi>"
                                    "</mrow>"
                                "</mrow>"
                                "<mo>,</mo>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>y</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>5</mi>"
                                    "</mrow>"
                                "</mrow>"
                            "</mrow>"
                        "</msub>"
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"
                            "<mo fence=\"false\" stretchy=\"true\">|</mo>"
                            "<mrow>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>y</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>5</mi>"
                                    "</mrow>"
                                "</mrow>"
                            "</mrow>"
                        "</msub>"
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"
                            "<mo fence=\"false\" stretchy=\"true\">|</mo>"
                            "<mrow>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi></mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi></mi>"
                                    "</mrow>"
                                "</mrow>"
                            "</mrow>"
                        "</msub>"
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"
                            "<mo fence=\"false\" stretchy=\"true\">|</mo>"
                            "<mrow>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>y</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>5</mi>"
                                    "</mrow>"
                                "</mrow>"
                            "</mrow>"
                        "</msub>"
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Selection
TEST_F(FormulaTest, evaluation_bar_subscript5)
{
    Start(600);

    document.Load("../../test/tests/evaluation_bar_subscript_1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0, 1}, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0, 1}, 0, 2})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 2, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Insert elements
TEST_F(FormulaTest, evaluation_bar_subscript6)
{
    Start(600);

    document.Load("../../test/tests/evaluation_bar_subscript_1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"
                            "<mo fence=\"false\" stretchy=\"true\">|</mo>"
                            "<mrow>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>x</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>2</mi>"
                                    "</mrow>"
                                "</mrow>"
                                "<mo>,</mo>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi></mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi></mi>"
                                    "</mrow>"
                                "</mrow>"
                                "<mo>,</mo>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>z</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>3</mi>"
                                    "</mrow>"
                                "</mrow>"
                                "<mo>,</mo>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>y</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>5</mi>"
                                    "</mrow>"
                                "</mrow>"
                            "</mrow>"
                        "</msub>"
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"
                            "<mo fence=\"false\" stretchy=\"true\">|</mo>"
                            "<mrow>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>x</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>2</mi>"
                                    "</mrow>"
                                "</mrow>"
                                "<mo>,</mo>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>z</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>3</mi>"
                                    "</mrow>"
                                "</mrow>"
                                "<mo>,</mo>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>y</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>5</mi>"
                                    "</mrow>"
                                "</mrow>"
                            "</mrow>"
                        "</msub>"
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Delete elements
TEST_F(FormulaTest, evaluation_bar_subscript7)
{
    Start(600);

    document.Load("../../test/tests/evaluation_bar_subscript_1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"
                            "<mo fence=\"false\" stretchy=\"true\">|</mo>"
                            "<mrow>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>x</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>2</mi>"
                                    "</mrow>"
                                "</mrow>"
                                "<mo>,</mo>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>z</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>3</mi>"
                                    "</mrow>"
                                "</mrow>"
                            "</mrow>"
                        "</msub>"
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0})) << document.GetEditorState().ToString();
}

//Delete an empty element
TEST_F(FormulaTest, evaluation_bar_subscript8)
{
    Start(600);

    document.WaitTask(document.InsertEvaluationBarSubscript(true));
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("x", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("2", true));

    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"
                            "<mo fence=\"false\" stretchy=\"true\">|</mo>"
                            "<mrow>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi></mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi></mi>"
                                    "</mrow>"
                                "</mrow>"
                            "</mrow>"
                        "</msub>"
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"
                            "<mo fence=\"false\" stretchy=\"true\">|</mo>"
                            "<mrow>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi></mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi></mi>"
                                    "</mrow>"
                                "</mrow>"
                            "</mrow>"
                        "</msub>"
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"
                            "<mo fence=\"false\" stretchy=\"true\">|</mo>"
                            "<mrow>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>x</mi>"
                                    "</mrow>"
                                    "<mo>=</mo>"
                                    "<mrow>"
                                        "<mi>2</mi>"
                                    "</mrow>"
                                "</mrow>"
                            "</mrow>"
                        "</msub>"
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();
}

}
