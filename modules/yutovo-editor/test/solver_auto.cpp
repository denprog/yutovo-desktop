/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include <filesystem>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace yutovo_solver;
using namespace std::chrono_literals;

TEST_F(SolverAutoTest, solver1)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>1.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>1</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>1.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(SolverAutoTest, solver2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("2", true);
    document.InsertPlus(true);
    document.InsertString("3", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2</mi>"\
                            "<mo>+</mo>"\
                            "<mi>3</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>5.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>2</mi>"\
                        "<mo>+</mo>"\
                        "<mi>3</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2</mi>"\
                            "<mo>+</mo>"\
                            "<mi>3</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>5.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(SolverAutoTest, solver3)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("2", true);
    document.InsertPlus(true);
    document.InsertString("3", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.Save("solver3_1.yut");
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

    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.Load("solver3_1.yut"));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2</mi>"\
                            "<mo>+</mo>"\
                            "<mi>3</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>5.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//log
TEST_F(SolverAutoTest, solver4)
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

    document.WaitTask(document.InsertCodeString("2", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertCodeString("8", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>3.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Solve after changing element
TEST_F(SolverAutoTest, solver5)
{
    Start(600);
    
    document.InsertDivision(true);
    document.InsertString("3345", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("2", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertString("2", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>3345</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>22</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>152.045</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Solve with errors
TEST_F(SolverAutoTest, solver6)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.WaitTask(document.InsertDivision(true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>Syntax error</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.InsertString("2", true);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.InsertString("3", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>1.5</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.WaitTask(document.DeleteElements(true, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>2</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>Syntax error</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>1.5</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Remove the equation sign
TEST_F(SolverAutoTest, solver7)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("12", true);
    document.InsertPlus(true);
    document.InsertString("15", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>12</mi>"\
                            "<mo>+</mo>"\
                            "<mi>15</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>27.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>12</mi>"\
                            "<mo>+</mo>"\
                            "<mi>15</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>27.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>12</mi>"\
                        "<mo>+</mo>"\
                        "<mi>15</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>12</mi>"\
                            "<mo>+</mo>"\
                            "<mi>15</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>27.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Replace the row with equation below and change it
TEST_F(SolverAutoTest, solver8)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("2", true);
    document.InsertPlus(true);
    document.InsertString("3", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    document.MoveCaretHome(false);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("3", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>32</mi>"\
                                "<mo>+</mo>"\
                                "<mi>3</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>35.</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                                "<mo>+</mo>"\
                                "<mi>3</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>5.</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Insert a paragraph in the row with equation
TEST_F(SolverAutoTest, solver9)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("235", true);
    document.InsertPlus(true);
    document.InsertString("345", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"235+3=238.\n" \
        U"45"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"235+345=580."
        ) << ToBasicString(document.ToText());
}

//Solve a big number, result must be uneditable
TEST_F(SolverAutoTest, solver10)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("235235435345", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"235235435345=2.352*pow(10,11)"
        ) << ToBasicString(document.ToText());
    
    for (int i = 0; i < 5; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"235235435345=2.352*pow(10,11)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"235235435345"
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"235235435345=2.352*pow(10,11)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == 
        U"235235435345=2.352*pow(10,11)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"235235435345"
        ) << ToBasicString(document.ToText());
}

//Change the order of results
TEST_F(SolverAutoTest, solver11)
{
    Start(600);

    document.InsertDivision(true);
    document.InsertString("1", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(1)/(2)=0.5"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    ResultType order1[4] = {ResultType::RATIONAL, ResultType::INTEGER, ResultType::REAL, ResultType::COMPLEX};
    std::copy(order1, order1 + 4, config.auto_result.results_order);
    document.WaitTask(document.SetConfig(config, true));

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertDivision(true);
    document.InsertString("1", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(1)/(2)=0.5\n"
        U"(1)/(2)=(1)/(2)"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    ResultType order2[4] = {ResultType::INTEGER, ResultType::RATIONAL, ResultType::REAL, ResultType::COMPLEX};
    std::copy(order2, order2 + 4, config.auto_result.results_order);
    document.WaitTask(document.SetConfig(config, true));

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertDivision(true);
    document.InsertString("3", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(1)/(2)=0.5\n"
        U"(1)/(2)=(1)/(2)\n"
        U"(3)/(2)=1(dec)"
        ) << ToBasicString(document.ToText());
}

//Change a sub-element of an equation
TEST_F(SolverAutoTest, solver12)
{
    Start(600);

    document.InsertDivision(true);
    document.InsertString("1234", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(1234)/(2)=617."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertString("2", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(1234)/(22)=56.091"
        ) << ToBasicString(document.ToText());
}

//Solve after deleting a division and undo
TEST_F(SolverAutoTest, solver13)
{
    Start(600);

    document.InsertDivision(true);
    document.InsertString("1234", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(1234)/(2)=617."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"12342=12342."
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(1234)/(2)=617."
        ) << ToBasicString(document.ToText());
}

//Solve after deleting a division and undo
TEST_F(SolverAutoTest, solver14)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillOnce([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertDivision(true);
    document.InsertString("12", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(12)/(2)=6."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretLeft(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"=Expression expected"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(12)/(2)=6."
        ) << ToBasicString(document.ToText());
}

//Change precision
TEST_F(SolverAutoTest, solver15)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("12.3456789012345", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"12.3456789012345=12.346"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetPrecision({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 7, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"12.3456789012345=12.3456789"
        ) << ToBasicString(document.ToText());
}

//Change exponential threshold
TEST_F(SolverAutoTest, solver16)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("123456789012", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"123456789012=1.235*pow(10,11)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetExp({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 12, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"123456789012=123456789012."
        ) << ToBasicString(document.ToText());
}

//Change angle measure
TEST_F(SolverAutoTest, solver17)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("arcsin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("1", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(1)=1.571(rad)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetAngleMeasure({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, AngleMeasure::Radian, AngleMeasure::Degree, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(1)=90.(deg)"
        ) << ToBasicString(document.ToText());
}

//Change fraction form on auto result
TEST_F(SolverAutoTest, solver18)
{
    Start(600);

    document.GetConfig(config);
    ResultType order[4] = {ResultType::RATIONAL, ResultType::INTEGER, ResultType::REAL, ResultType::COMPLEX};
    std::copy(order, order + 4, config.auto_result.results_order);
    config.auto_result.rational_result.fraction_form = FractionForm::Improper;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertDivision(true);
    document.InsertString("11", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("5", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(11)/(5)=(11)/(5)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetFractionForm({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, FractionForm::Proper, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(11)/(5)=2(1)/(5)"
        ) << ToBasicString(document.ToText());
}

//Change the order of results
TEST_F(SolverAutoTest, solver19)
{
    Start(600);

    document.GetConfig(config);
    ResultType order1[4] = {ResultType::RATIONAL, ResultType::INTEGER, ResultType::REAL, ResultType::COMPLEX};
    std::copy(order1, order1 + 4, config.auto_result.results_order);
    document.WaitTask(document.SetConfig(config, true));

    document.InsertDivision(true);
    document.InsertString("1", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertPlus(true);
    document.InsertString("1.2", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(1)/(2)+1.2=1(7)/(10)"
        ) << ToBasicString(document.ToText());
}

//Save and load with changing a result parameter
TEST_F(SolverAutoTest, solver20)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("234.12345678", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitTask(document.SetPrecision({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 7, true));
    document.WaitSolver();
    document.Save("solver21_1.yut");

    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.WaitTask(document.Load("solver21_1.yut"));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"234.12345678=234.1234568") << ToBasicString(document.ToText());
}

//Change the order of results
TEST_F(SolverAutoTest, solver21)
{
    Start(600);

    document.GetConfig(config);
    ResultType order1[4] = {ResultType::RATIONAL, ResultType::INTEGER, ResultType::REAL, ResultType::COMPLEX};
    std::copy(order1, order1 + 4, config.auto_result.results_order);
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("cos", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("0", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"cos(0)=1."
        ) << ToBasicString(document.ToText());
}

//Delete the equation sign
TEST_F(SolverAutoTest, solver22)
{
    Start(600);

    document.InsertDivision(true);
    document.InsertString("3", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"(3)/(3)=1.") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
    document.MoveCaretRight(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"(3)/(3)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"(3)/(3)=1.") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"(3)/(3)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Complex numbers
TEST_F(SolverAutoTest, solver23)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1.2", true);
    document.InsertPlus(true);
    document.InsertString("3.4i", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"1.2+3.4i=1.2+3.4i"
        ) << ToBasicString(document.ToText());
}

//Change complex form on auto result
TEST_F(SolverAutoTest, solver24)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1.2", true);
    document.InsertPlus(true);
    document.InsertString("3.4i", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"1.2+3.4i=1.2+3.4i"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetComplexForm({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, ComplexForm::Trigonometric, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"1.2+3.4i=3.606(cos(1.232)+i*sin(1.232))"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"1.2+3.4i=1.2+3.4i"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetComplexForm({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, ComplexForm::Exponential, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"1.2+3.4i=3.606pow(e,1.232i)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"1.2+3.4i=1.2+3.4i"
        ) << ToBasicString(document.ToText());
}

//Edit the left expression
TEST_F(SolverAutoTest, solver25)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("234", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"234=234."
        ) << ToBasicString(document.ToText());

    document.MoveCaretLeft(false);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("6", true));
    document.WaitTask(document.InsertString("7", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"234+67=301."
        ) << ToBasicString(document.ToText());
    
    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"234+=Syntax error"
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"234+67=301."
        ) << ToBasicString(document.ToText());
}

//The code block is wider then a row
TEST_F(SolverAutoTest, solver26)
{
    Start(610);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"In literary theory, a text is any object that can be read, whether this object is a work of literature123=123."
        ) << ToBasicString(document.ToText());
    
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertString("456789", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"In literary theory, a text is any object that can be read, whether this object is a work of literature123456789=123456789."
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.InsertString("123456789", true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"In literary theory, a text is any object that can be read, whether this object is a work of literature123456789123456789=1.235*pow(10,17)"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 2, 0, 0, 0, 0, 0, 0, 18})) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"In literary theory, a text is any object that can be read, whether this object is a work of literature123456789=123456789."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 2, 0, 0, 0, 0, 0, 0, 9})) << document.GetEditorState().ToString();
}

//Solve after changing the result element position
TEST_F(SolverAutoTest, solver27)
{
    Start(600);

    document.config.solve_delay = 1000;

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"\nIn literary theory, a text is any object that can be read, whether this object is a work of literature123=123."
        ) << ToBasicString(document.ToText());
    
    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(1500ms);
    ASSERT_TRUE(document.ToText() == 
        U"In literary theory, a text is any object that can be read, whether this object is a work of literature123=123."
        ) << ToBasicString(document.ToText());
}

//Change angle measure
TEST_F(SolverAutoTest, solver28)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("arcsin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("i", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(i)=0.881i(rad),3.142-0.881i(rad)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetAngleMeasure({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, AngleMeasure::Radian, AngleMeasure::Degree, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(i)=50.499i(deg),180.-50.499i(deg)"
        ) << ToBasicString(document.ToText());
}

//Two code blocks with the same locale
TEST_F(SolverAutoTest, solver29)
{
    Start(600);
    
    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("6.5", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"6.5=6.5") << ToBasicString(document.ToText());

    document.MoveCaretToDocumentEnd(false);
    document.InsertParagraph(true);
    document.InsertCode(true, true);
    document.InsertString("5.5", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"6.5=6.5\n"
        U"5.5=5.5"
        ) << ToBasicString(document.ToText());
}

//Solve a sum
TEST_F(SolverAutoTest, solver30)
{
    Start(600);

    document.WaitTask(document.InsertSum(true));
    document.InsertString(U"i", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"10", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString(U"i", true));
    document.WaitTask(document.MoveCaretRight(false));

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"sum(i=0,10,i)=55."
        ) << ToBasicString(document.ToText());
}

//Solve a product
TEST_F(SolverAutoTest, solver31)
{
    Start(600);

    document.WaitTask(document.InsertProduct(true));
    document.InsertString(U"i", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"1", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"10", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString(U"i", true));
    document.WaitTask(document.MoveCaretRight(false));

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"prod(i=1,10,i)=3628800."
        ) << ToBasicString(document.ToText());
}

//Delete a part of the left expression
TEST_F(SolverAutoTest, solver32)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("23", true);
    document.InsertPlus(true);
    document.InsertString("5", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"23+5=28."
        ) << ToBasicString(document.ToText());
    
    for (int i = 0; i < 4; ++i)
        document.MoveCaretLeft(false);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"+5=5."
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"23+5=28."
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"+5=5."
        ) << ToBasicString(document.ToText());
}

//Delete a part of the left expression
TEST_F(SolverAutoTest, solver33)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("23", true);
    document.InsertPlus(true);
    document.InsertString("5", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"23+5=28."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"23=23."
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"23+5=28."
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"23=23."
        ) << ToBasicString(document.ToText());
}

//Interrupting a solving
TEST_F(SolverAutoTest, solver34)
{
    Start(600);

    document.WaitTask(document.InsertSum(true));
    document.InsertString(U"i", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"100000", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString(U"i", true));
    document.WaitTask(document.MoveCaretRight(false));

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    std::this_thread::sleep_for(1s);
    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"sum(i=0,1000,i)=500500."
        ) << ToBasicString(document.ToText());
}

//Solving after changing font
TEST_F(SolverAutoTest, solver35)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertParagraph(true);
    document.InsertString("55", true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetBold(true));
    document.WaitTask(document.MoveCaretRight(false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"123\n"\
        U"55=55."
        ) << ToBasicString(document.ToText());
}

//Re-solving many code blocks
TEST_F(SolverAutoTest, solver36)
{
    Start(600);

    document.InsertString("String", true);

    for (int i = 0; i < 5; ++i)
    {
        document.WaitTask(document.MoveCaretToDocumentEnd(false));
        document.InsertCode(false, true);
        document.InsertNthRoot(true);
        document.InsertString("3", true);
        document.InsertDivision(true);
        document.InsertString("2", true);
        document.MoveCaretRight(false);
        document.MoveCaretRight(false);
        document.MoveCaretRight(false);
        document.InsertString("2", true);
        document.MoveCaretRight(false);
        document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
        document.WaitSolver();
    }
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"Stringroot(2,(3)/(2))=1.587root(2,(3)/(2))=1.587root(2,(3)/(2))=1.587root(2,(3)/(2))=1.587root(2,(3)/(2))=1.587") << 
        ToBasicString(document.ToText());

    document.WaitTask(document.ReSolve(ElementId{}));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"Stringroot(2,(3)/(2))=1.587root(2,(3)/(2))=1.587root(2,(3)/(2))=1.587root(2,(3)/(2))=1.587root(2,(3)/(2))=1.587") << 
        ToBasicString(document.ToText());
}

//Break a solving with a complex grammar
TEST_F(SolverAutoTest, solver37)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("234", true);
    document.InsertDivision(true);
    document.InsertString("3", true);
    document.InsertDivision(true);
    document.InsertString("4", true);
    document.InsertDivision(true);
    document.InsertString("6", true);
    document.InsertDivision(true);
    document.InsertString("7", true);
    document.InsertDivision(true);
    document.InsertString("6", true);
    document.InsertDivision(true);
    document.InsertString("7", true);
    document.InsertDivision(true);
    document.InsertString("2", true);
    document.InsertDivision(true);
    document.InsertString("4", true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.MoveCaretLeft(false));
    time_t t = time(0);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    std::this_thread::sleep_for(1s);

    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.DeleteElements(false, true));
    document.InsertString("234", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);
    ASSERT_TRUE(document.ToText() == 
        U"234=234.") << 
        ToBasicString(document.ToText());
    ASSERT_TRUE(time(0) - t <= 6) << (time(0) - t);
}

//Break a solving with a complex grammar
TEST_F(SolverAutoTest, solver38)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("234", true);
    document.InsertDivision(true);
    document.InsertString("3", true);
    document.InsertDivision(true);
    document.InsertString("4", true);
    document.InsertDivision(true);
    document.InsertString("6", true);
    document.InsertDivision(true);
    document.InsertString("7", true);
    document.InsertDivision(true);
    document.InsertString("6", true);
    document.InsertDivision(true);
    document.InsertString("7", true);
    document.InsertDivision(true);
    document.InsertString("2", true);
    document.InsertDivision(true);
    document.InsertString("4", true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.MoveCaretLeft(false));
    time_t t = time(0);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    std::this_thread::sleep_for(2s);

    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(1s);

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("234", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(6s);
    ASSERT_TRUE(document.ToText() == 
        U"(234)/((3)/((4)/((6)/((7)/((6)/((7)/((2)/(4))))))))\n"\
        U"234=234.") << 
        ToBasicString(document.ToText());
    ASSERT_TRUE(time(0) - t <= 12) << (time(0) - t);
}

//Inserting braces in a result
TEST_F(SolverAutoTest, solver39)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("234", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertRoundBrackets(true));
    ASSERT_TRUE(document.ToText() == 
        U"234=234.") << 
        ToBasicString(document.ToText());

    document.WaitTask(document.InsertString("5", true));
    ASSERT_TRUE(document.ToText() == 
        U"234=234.") << 
        ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"234") << 
        ToBasicString(document.ToText());
}

//Check there is no redo at result
TEST_F(SolverAutoTest, solver40)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("234", true);
    document.InsertPower(true);
    document.InsertString("56", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"pow(234,56)=4.743*pow(10,132)"
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(!document.CanRedo());
    document.WaitTask(document.InsertString("4", true));
    ASSERT_TRUE(document.CanUndo());
    ASSERT_TRUE(!document.CanRedo());
}

//Set font attributes
TEST_F(SolverAutoTest, solver41)
{
    Start(600);

    document.config.solve_delay = 1000;

    document.InsertCode(false, true);
    document.InsertString("12345678901234", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"12345678901234=1.235*pow(10,13)"
        ) << ToBasicString(document.ToText());
    
    document.WaitTask(document.SelectAll());
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.SetItalic(true));
    document.WaitTask(document.SetUnderline(true));
    StringFormat format;
    ElementId id = document.FindByString({0}, U"12345678901234")->id;
    ASSERT_TRUE(document.GetStringFormat(id, format));
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(format.underline);

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    //don't wait solving
    ASSERT_TRUE(document.GetStringFormat(id, format));
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(!format.underline);

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    //don't wait solving
    ASSERT_TRUE(document.GetStringFormat(id, format));
    ASSERT_TRUE(!format.italic);
    ASSERT_TRUE(!format.underline);
}

//Don't recalculate on save
TEST_F(SolverAutoTest, solver42)
{
    Start(600);
    
    document.Load("../../test/tests/solver42.yut");
    document.WaitLoad();
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    document.Save("solver42.yut");
    std::this_thread::sleep_for(100ms);
    auto el = document.FindByString({0}, U"~");
    ASSERT_TRUE(!el);
}

//Solve with errors
TEST_F(SolverAutoTest, errors1)
{
    Start(600);
    
    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertString("3", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>3</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>3.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.WaitTask(document.InsertDivision(true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>3</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>Syntax error</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.WaitTask(document.InsertString("4", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>0.75</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.DeleteElements(false, true));
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
                                "<mi>4</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
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
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>0.75</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>3</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>Syntax error</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Solve with errors
TEST_F(SolverAutoTest, errors2)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertSquareRoot(true);
    document.InsertString("2", true);
    document.InsertDivision(true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"sqrt((2)/())=Syntax error") << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 0 && size == 3);

    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertString("3", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"sqrt((2)/(3))=0.816") << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0}));

    document.MoveCaretUp(false);
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"sqrt(23)=4.796") << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0}));
}

//Solve with errors
TEST_F(SolverAutoTest, errors3)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillOnce([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertSquareRoot(true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"sqrt()=Wrong arguments count") << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 0}, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 0 && size == 1);
}

//Delete the equation sign
TEST_F(SolverAutoTest, errors4)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });
    
    document.InsertDivision(true);
    document.InsertString("3", true);
    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"(3)/()=Syntax error") << ToBasicString(document.ToText());

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"(3)/()=Syntax error") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
    ASSERT_TRUE(document.HasErrorMarks(ElementId{0, 0, 0, 0, 0}));

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"(3)/()") << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks(ElementId{0, 0, 0, 0, 0}));

    document.Redo();
    document.WaitRedo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"(3)/()=Syntax error") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.HasErrorMarks(ElementId{0, 0, 0, 0, 0}));
}

//Service timeout
TEST_F(SolverAutoTest, errors5)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.GetConfig(config);
    config.service_timeout = 1;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertDivision(true);
    document.InsertString("6kg", true);
    document.InsertMultiply(true);
    document.InsertString("2m", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("4", true);
    document.InsertMultiply(true);
    document.InsertString("s", true);
    document.InsertPower(true);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
#ifdef REMOTE_SOLVER
    ASSERT_TRUE(document.ToText() == U"(6kg*2m)/(4*pow(s,2))=Solver timeout") << ToBasicString(document.ToText());
#else
    ASSERT_TRUE(document.ToText() == U"(6kg*2m)/(4*pow(s,2))=Solving time exceeded") << ToBasicString(document.ToText());
#endif
}

//Solve empty expression
TEST_F(SolverAutoTest, errors6)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"=Expression expected") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
    ASSERT_TRUE(document.HasErrorMarks(ElementId{0, 0, 0, 0, 0}));

    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertString("2", true));
    document.WaitSolver();
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"=Expression expected") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
    ASSERT_TRUE(document.HasErrorMarks(ElementId{0, 0, 0, 0, 0}));
}

//Solve expressions with errors
TEST_F(SolverAutoTest, errors7)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.InsertCode(false, true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"123=123.=Expression expected") << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks(ElementId{0, 0, 0, 0}));
    ASSERT_TRUE(document.HasErrorMarks(ElementId{0, 0, 0, 1}));
}

TEST_F(SolverAutoTest, units1)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("1m", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1m</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>1.</mi>"\
                                "<mi>m</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(SolverAutoTest, units2)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("2cm", true);
    document.InsertMultiply(true);
    document.InsertString("3cm", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2cm</mi>"\
                            "<mo>×</mo>"\
                            "<mi>3cm</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>6.</mi>"\
                                "<msup>"\
                                    "<mrow>"\
                                        "<mi>cm</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>2</mi>"\
                                    "</mrow>"\
                                "</msup>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(SolverAutoTest, units3)
{
    Start(600);
    
    document.InsertDivision(true);
    document.InsertString("2m", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("4s", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"(2m)/(4s)=5.(dm)/(s)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, units4)
{
    Start(600);
    
    document.GetConfig(config);
    config.service_timeout = 20000;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertDivision(true);
    document.InsertString("6kg", true);
    document.InsertMultiply(true);
    document.InsertString("2m", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("4s", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"(6kg*2m)/(4s)=3.(kg*m)/(s)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, units5)
{
    Start(600);
    
    document.GetConfig(config);
    config.service_timeout = 20000;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertDivision(true);
    document.InsertString("6kg", true);
    document.InsertMultiply(true);
    document.InsertString("2m", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("4", true);
    document.InsertMultiply(true);
    document.InsertString("s", true);
    document.InsertPower(true);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"(6kg*2m)/(4*pow(s,2))=3.N") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, units6)
{
    Start(600);

    document.InsertCode(1, true);
    document.InsertString("fut", true);
    document.WaitTask(document.InsertSubscript(true));
    document.InsertString("rus", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<msub>"\
                                "<mrow>"\
                                    "<mi>fut</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>rus</mi>"\
                                "</mrow>"\
                            "</msub>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>1.</mi>"\
                                "<msub>"\
                                    "<mrow>"\
                                        "<mi>fut</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>rus</mi>"\
                                    "</mrow>"\
                                "</msub>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Set language
TEST_F(SolverAutoTest, units7)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.GetConfig(config);
    config.service_timeout = 20000;
    document.WaitTask(document.SetConfig(config, true));

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(1, true);
    document.InsertDivision(true);
    document.InsertString("6кг", true);
    document.InsertMultiply(true);
    document.InsertString("2м", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("4с", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == U"(6кг*2м)/(4с)=3.(кг*м)/(с)") << ToBasicString(document.ToText());
}

//Change language
TEST_F(SolverAutoTest, units8)
{
    Start(600);

    document.GetConfig(config);
    config.service_timeout = 30000;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("4N", true);
    document.InsertMultiply(true);
    document.InsertString("m", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("4.2Н", true);
    document.InsertMultiply(true);
    document.InsertString("м", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"4N*m=4.J\n"\
        U"4.2Н*м=4.2Дж") << ToBasicString(document.ToText());
}

//Implicit multiplication of division
TEST_F(SolverAutoTest, units9)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertDivision(true);
    document.InsertString("1", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("m", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"(1)/(2)m=5.dm"
        ) << ToBasicString(document.ToText());
}

//Implicit multiplication of division
TEST_F(SolverAutoTest, units10)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertDivision(true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("3", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("m", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"(1+3)/(2)m=2.m"
        ) << ToBasicString(document.ToText());
}

//Change result unit, save and load the document
TEST_F(SolverAutoTest, units11)
{
    Start(600);
    
    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("2км", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    std::this_thread::sleep_for(600ms);
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2км=2.км"
        ) << ToBasicString(document.ToText());

    std::vector<yutovo_calculator::Unit> cast_units;
    document.GetCastUnits({0, 0, 0, 0, 0, 0, 0, 2, 0}, cast_units);
    yutovo_calculator::Unit unit(U"м");
    document.WaitTask(document.SetUnit({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, unit, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"2км=2000.м") << ToBasicString(document.ToText());

    document.Save("units11_1.yut");

    document.WaitTask(document.New());
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Load("units11_1.yut");
    document.WaitLoad();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"2км=2000.м") << ToBasicString(document.ToText());
}

//Change result unit
TEST_F(SolverAutoTest, units12)
{
    Start(600);
    
    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("1Ом", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    std::this_thread::sleep_for(1s);
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"1Ом=1.Ом"
        ) << ToBasicString(document.ToText());

    yutovo_calculator::Unit unit;
    unit.FromString(U"((Дж)/(нс*мкА^2))");
    document.WaitTask(document.SetUnit({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, unit, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"1Ом=1.*pow(10,-21)(Дж)/(нс*pow(мкА,2))") << ToBasicString(document.ToText());

    unit.FromString(U"((Дж)/(сутки*А^2))");
    document.WaitTask(document.SetUnit({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, unit, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"1Ом=86400.(Дж)/(сутки*pow(А,2))") << ToBasicString(document.ToText());
}

//Change result unit and then change the expression
TEST_F(SolverAutoTest, units13)
{
    Start(600);

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("1с", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"1с=1.с"
        ) << ToBasicString(document.ToText());

    yutovo_calculator::Unit unit;
    unit.FromString(U"мс");
    document.WaitTask(document.SetUnit({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, unit, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"1с=1000.мс"
        ) << ToBasicString(document.ToText());
    
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitTask(document.InsertString("м", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"1м=м"
        ) << ToBasicString(document.ToText());
}

//Solve an array
TEST_F(SolverAutoTest, arrays1)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("2.3", true);
    document.InsertComma(true);
    document.InsertString("4.56", true);
    document.InsertCloseSquareBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mo>[</mo>"\
                            "<mi>1</mi>"\
                            "<mo>,</mo>"\
                            "<mi>2.3</mi>"\
                            "<mo>,</mo>"\
                            "<mi>4.56</mi>"\
                            "<mo>]</mo>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                "<mo>[</mo>"\
                                "<mi>1.</mi>"\
                                "<mo>,</mo>"\
                                "<mi>2.3</mi>"\
                                "<mo>,</mo>"\
                                "<mi>4.56</mi>"\
                                "<mo>]</mo>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Solve with a list
TEST_F(SolverAutoTest, lists1)
{
    Start(600);
    
    document.InsertCode(false, true);
    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.Load("../../test/tests/lists1.yut");
    document.WaitLoad();
    document.WaitSolver();
    std::this_thread::sleep_for(4s);
    ASSERT_TRUE(document.ToText() == 
        U"металлы=\"серебро\",\"медь\",\"алюминий\"\n"\
        U"ρ{серебро}=0.0155мкОм*м\n"\
        U"ρ{медь}=0.0175мкОм*м\n"\
        U"ρ{алюминий}=0.027мкОм*м\n"\
        U"проводник=\"алюминий\"\n"\
        U"длина=1м\n"\
        U"сечение=1pow(мм,2)\n"\
        U"сопротивление=ρ{проводник}*(длина)/(сечение)\n"\
        U"сопротивление=27.мОм"\
        ) << ToBasicString(document.ToText());
    
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"металлы=\"серебро\",\"медь\",\"алюминий\"\n"\
        U"ρ{серебро}=0.0155мкОм*м\n"\
        U"ρ{медь}=0.0175мкОм*м\n"\
        U"ρ{алюминий}=0.027мкОм*м\n"\
        U"проводник=\"люминий\"\n"\
        U"длина=1м\n"\
        U"сечение=1pow(мм,2)\n"\
        U"сопротивление=ρ{проводник}*(длина)/(сечение)\n"\
        U"сопротивление=Unknown identifier"\
        ) << ToBasicString(document.ToText());
}

//AUTO mode should fallback to SYMBOLIC for expressions with undefined variables
TEST_F(SolverAutoTest, symbolic1)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == U"x+1=1+x") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == U"x+1") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, symbolic2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("sin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"sin(x)=sin(x)") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"sin(x)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, symbolic3)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("derivative", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertPower(true);
    document.InsertString("2", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.InsertComma(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"derivative(pow(x,2),x)=2*x") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative(pow(x,2),x)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, symbolic4)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("expand", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("pow", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.InsertCloseRoundBracket(true);
    document.InsertComma(true);
    document.InsertString("2", true);
    document.InsertCloseRoundBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"expand(pow((x+1),2))=1+2*x+pow(x,2)") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"expand(pow((x+1),2))") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, symbolic5)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("subs", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertPower(true);
    document.InsertString("2", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.InsertComma(true);
    document.InsertString("x", true);
    document.InsertComma(true);
    document.InsertString("5", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"subs(pow(x,2),x,5)=25") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"subs(pow(x,2),x,5)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, symbolic6)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.InsertDivision(true);
    document.InsertString("2", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"x+(1)/(2)=0.5+x") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x+(1)/(2)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, symbolic7)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("i", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"x+1+i=1+i+x") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x+1+i") << ToBasicString(document.ToText());
}

//Interrupt a long solving
TEST_F(FormulaTest, long_solving)
{
    document.config.service_timeout = 3000;
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertDefiniteIntegral(true);
    document.InsertString(U"0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"1", true);
    document.MoveCaretRight(false);
    document.InsertString(U"1", true);
    document.InsertDivision(true);
    document.InsertString(U"x", true);
    document.InsertPlus(true);
    document.InsertString(U"j", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"x", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,1,(1)/(x+j),x)=Solving time exceeded") << ToBasicString(document.ToText());
}

TEST_F(MultiDocumentSolverAutoTest, load_recent_files_simultaneously)
{
    const std::filesystem::path tests_dir = std::filesystem::path(__FILE__).parent_path() / "tests";
    const char* filenames[doc_count] = 
        {
            "solve Diferenciação.yut",
            "solve Substituição.yut",
            "solve Integração.yut",
            "solve Derivada em um ponto.yut",
            "solve Integral definida.yut",
            "solve Derivada de uma função.yut",
            "solve Integral indefinida.yut",
            "solve Derivada segunda.yut",
            "solve Matrizes.yut",
            "solve Registro de usuário.yut"
        };

    //issue all loads first so calculations start simultaneously
    uint load_ids[doc_count];
    load_ids[0] = document.Load((tests_dir / filenames[0]).string());
    for (size_t i = 0; i < docs.size(); ++i)
        load_ids[i + 1] = docs[i]->document.Load((tests_dir / filenames[i + 1]).string());

    //wait for every load to finish
    document.WaitTask(load_ids[0]);
    for (size_t i = 0; i < docs.size(); ++i)
        docs[i]->document.WaitTask(load_ids[i + 1]);

    //wait for all solvers to finish (some files may have no equations)
    auto deadline = std::chrono::steady_clock::now() + 120s;
    bool all_solved = false;
    while (!all_solved)
    {
        all_solved = true;
        for (size_t i = 0; i < doc_count; ++i)
        {
            Document& doc = (i == 0) ? document : docs[i - 1]->document;
            if (doc.ToText().find(U"~") != std::u32string::npos)
            {
                all_solved = false;
                break;
            }
        }
        if (!all_solved)
        {
            if (std::chrono::steady_clock::now() >= deadline)
            {
                std::string pending;
                for (size_t i = 0; i < doc_count; ++i)
                {
                    Document& doc = (i == 0) ? document : docs[i - 1]->document;
                    std::u32string text = doc.ToText();
                    if (text.find(U"~") != std::u32string::npos)
                    {
                        pending += "document " + std::to_string(i) + " still pending: " + ToBasicString(text) + "\n";
                    }
                }
                ASSERT_TRUE(false) << "timeout waiting for calculations\n" << pending;
            }
            std::this_thread::sleep_for(100ms);
        }
    }
}

}
