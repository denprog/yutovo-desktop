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
using namespace yutovo_solver;
using namespace std::chrono_literals;

TEST_F(SolverSymbolicTest, solver1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>x</mi>"
                            "<mo>+</mo>"
                            "<mi>1</mi>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mi>1</mi>"
                                "<mo>+</mo>"
                                "<mi>x</mi>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mi>x</mi>"
                        "<mo>+</mo>"
                        "<mi>1</mi>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
}

TEST_F(SolverSymbolicTest, solver2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("sin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>sin</mi>"
                            "<mo>(</mo>"
                            "<mi>x</mi>"
                            "<mo>)</mo>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mi>sin</mi>"
                                "<mo>(</mo>"
                                "<mi>x</mi>"
                                "<mo>)</mo>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mi>sin</mi>"
                        "<mo>(</mo>"
                        "<mi>x</mi>"
                        "<mo>)</mo>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
}

TEST_F(SolverSymbolicTest, solver3)
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
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>derivative</mi>"
                            "<mo>(</mo>"
                            "<msup>"
                                "<mrow>"
                                    "<mi>x</mi>"
                                "</mrow>"
                                "<mrow>"
                                    "<mi>2</mi>"
                                "</mrow>"
                            "</msup>"
                            "<mo>,</mo>"
                            "<mi>x</mi>"
                            "<mo>)</mo>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mi>2</mi>"
                                "<mo>×</mo>"
                                "<mi>x</mi>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mi>derivative</mi>"
                        "<mo>(</mo>"
                        "<msup>"
                            "<mrow>"
                                "<mi>x</mi>"
                            "</mrow>"
                            "<mrow>"
                                "<mi>2</mi>"
                            "</mrow>"
                        "</msup>"
                        "<mo>,</mo>"
                        "<mi>x</mi>"
                        "<mo>)</mo>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
}

TEST_F(SolverSymbolicTest, solver4)
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
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>expand</mi>"
                            "<mo>(</mo>"
                            "<mi>pow</mi>"
                            "<mo>(</mo>"
                            "<mo>(</mo>"
                            "<mi>x</mi>"
                            "<mo>+</mo>"
                            "<mi>1</mi>"
                            "<mo>)</mo>"
                            "<mo>,</mo>"
                            "<mi>2</mi>"
                            "<mo>)</mo>"
                            "<mo>)</mo>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mi>1</mi>"
                                "<mo>+</mo>"
                                "<mi>2</mi>"
                                "<mo>×</mo>"
                                "<mi>x</mi>"
                                "<mo>+</mo>"
                                "<msup>"
                                    "<mrow>"
                                        "<mi>x</mi>"
                                    "</mrow>"
                                    "<mrow>"
                                        "<mi>2</mi>"
                                    "</mrow>"
                                "</msup>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mi>expand</mi>"
                        "<mo>(</mo>"
                        "<mi>pow</mi>"
                        "<mo>(</mo>"
                        "<mo>(</mo>"
                        "<mi>x</mi>"
                        "<mo>+</mo>"
                        "<mi>1</mi>"
                        "<mo>)</mo>"
                        "<mo>,</mo>"
                        "<mi>2</mi>"
                        "<mo>)</mo>"
                        "<mo>)</mo>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
}

TEST_F(SolverSymbolicTest, solver5)
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
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>subs</mi>"
                            "<mo>(</mo>"
                            "<msup>"
                                "<mrow>"
                                    "<mi>x</mi>"
                                "</mrow>"
                                "<mrow>"
                                    "<mi>2</mi>"
                                "</mrow>"
                            "</msup>"
                            "<mo>,</mo>"
                            "<mi>x</mi>"
                            "<mo>,</mo>"
                            "<mi>5</mi>"
                            "<mo>)</mo>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mi>25</mi>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mi>subs</mi>"
                        "<mo>(</mo>"
                        "<msup>"
                            "<mrow>"
                                "<mi>x</mi>"
                            "</mrow>"
                            "<mrow>"
                                "<mi>2</mi>"
                            "</mrow>"
                        "</msup>"
                        "<mo>,</mo>"
                        "<mi>x</mi>"
                        "<mo>,</mo>"
                        "<mi>5</mi>"
                        "<mo>)</mo>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
}

TEST_F(SolverSymbolicTest, solver6)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.InsertDivision(true);
    document.InsertString("2", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>x</mi>"
                            "<mo>+</mo>"
                            "<mfrac>"
                                "<mrow>"
                                    "<mi>1</mi>"
                                "</mrow>"
                                "<mrow>"
                                    "<mi>2</mi>"
                                "</mrow>"
                            "</mfrac>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mi>0.5</mi>"
                                "<mo>+</mo>"
                                "<mi>x</mi>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mi>x</mi>"
                        "<mo>+</mo>"
                        "<mfrac>"
                            "<mrow>"
                                "<mi>1</mi>"
                            "</mrow>"
                            "<mrow>"
                                "<mi>2</mi>"
                            "</mrow>"
                        "</mfrac>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
}

TEST_F(SolverSymbolicTest, solver7)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("i", true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>x</mi>"
                            "<mo>+</mo>"
                            "<mi>1</mi>"
                            "<mo>+</mo>"
                            "<mi>i</mi>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mi>1</mi>"
                                "<mo>+</mo>"
                                "<mi>i</mi>"
                                "<mo>+</mo>"
                                "<mi>x</mi>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mi>x</mi>"
                        "<mo>+</mo>"
                        "<mi>1</mi>"
                        "<mo>+</mo>"
                        "<mi>i</mi>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
}

TEST_F(SolverSymbolicTest, solver8)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1", true);
    document.InsertDivision(true);
    document.InsertString("x", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mfrac>"
                                "<mrow>"
                                    "<mi>1</mi>"
                                "</mrow>"
                                "<mrow>"
                                    "<mi>x</mi>"
                                "</mrow>"
                            "</mfrac>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<msup>"
                                "<mrow>"
                                    "<mi>x</mi>"
                                "</mrow>"
                                "<mrow>"
                                    "<mo>-</mo>"
                                    "<mi>1</mi>"
                                "</mrow>"
                            "</msup>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mfrac>"
                            "<mrow>"
                                "<mi>1</mi>"
                            "</mrow>"
                            "<mrow>"
                                "<mi>x</mi>"
                            "</mrow>"
                        "</mfrac>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
}

TEST_F(SolverSymbolicTest, solver9)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertPower(true);
    document.InsertString("-1", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<msup>"
                                "<mrow>"
                                    "<mi>x</mi>"
                                "</mrow>"
                                "<mrow>"
                                    "<mi>-1</mi>"
                                "</mrow>"
                            "</msup>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<msup>"
                                "<mrow>"
                                    "<mi>x</mi>"
                                "</mrow>"
                                "<mrow>"
                                    "<mo>-</mo>"
                                    "<mi>1</mi>"
                                "</mrow>"
                            "</msup>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<msup>"
                            "<mrow>"
                                "<mi>x</mi>"
                            "</mrow>"
                            "<mrow>"
                                "<mi>-1</mi>"
                            "</mrow>"
                        "</msup>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
}

TEST_F(SolverSymbolicTest, solver10)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.InsertDivision(true);
    document.InsertString("2", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_RATIONAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>x</mi>"
                            "<mo>+</mo>"
                            "<mfrac>"
                                "<mrow>"
                                    "<mi>1</mi>"
                                "</mrow>"
                                "<mrow>"
                                    "<mi>2</mi>"
                                "</mrow>"
                            "</mfrac>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mfrac>"
                                    "<mrow>"
                                        "<mi>1</mi>"
                                    "</mrow>"
                                    "<mrow>"
                                        "<mi>2</mi>"
                                    "</mrow>"
                                "</mfrac>"
                                "<mo>+</mo>"
                                "<mi>x</mi>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mi>x</mi>"
                        "<mo>+</mo>"
                        "<mfrac>"
                            "<mrow>"
                                "<mi>1</mi>"
                            "</mrow>"
                            "<mrow>"
                                "<mi>2</mi>"
                            "</mrow>"
                        "</mfrac>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
}

TEST_F(SolverSymbolicTest, solver11)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("i", true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>x</mi>"
                            "<mo>+</mo>"
                            "<mi>1</mi>"
                            "<mo>+</mo>"
                            "<mi>i</mi>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mi>1</mi>"
                                "<mo>+</mo>"
                                "<mi>i</mi>"
                                "<mo>+</mo>"
                                "<mi>x</mi>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mi>x</mi>"
                        "<mo>+</mo>"
                        "<mi>1</mi>"
                        "<mo>+</mo>"
                        "<mi>i</mi>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
}

TEST_F(SolverSymbolicTest, solver12)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("y", true);
    document.InsertPlus(true);
    document.InsertString("y", true);
    document.InsertDivision(true);
    document.InsertString("3", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"y+(y)/(3)=1.333*y") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"y+(y)/(3)") << ToBasicString(document.ToText());
}

TEST_F(SolverSymbolicTest, solver13)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("y", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.InsertDivision(true);
    document.InsertString("3", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"y+(1)/(3)=0.333+y") << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretRight(false));
    auto id = document.FindCurrentParentByType(ElementType::SYMBOLIC_REAL_RESULT);
    ASSERT_FALSE(id.empty());
    document.WaitTask(document.SetPrecision(id, 1, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"y+(1)/(3)=0.3+y") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"y+(1)/(3)=0.333+y") << ToBasicString(document.ToText());
}

TEST_F(SolverSymbolicTest, solver14)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("y", true);
    document.InsertPlus(true);
    document.InsertString("7", true);
    document.InsertDivision(true);
    document.InsertString("3", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"y+(7)/(3)=2.333+y") << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretRight(false));
    auto id = document.FindCurrentParentByType(ElementType::SYMBOLIC_REAL_RESULT);
    ASSERT_FALSE(id.empty());
    document.WaitTask(document.SetPrecision(id, 7, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == U"y+(7)/(3)=2.3333333+y") << ToBasicString(document.ToText());
}

TEST_F(SolverSymbolicTest, solver15)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("subs", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("yy", true);
    document.InsertPower(true);
    document.InsertString("3", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.InsertComma(true);
    document.InsertString("yy", true);
    document.InsertComma(true);
    document.InsertString("5", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"subs(pow(yy,3),yy,5)=125") << ToBasicString(document.ToText());
}

TEST_F(SolverSymbolicTest, solver16)
{
    Start(600);

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.InsertString("123.", true);
    document.InsertString(" ", true);
    document.InsertString("ее", true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"123.ее=123*ее") << ToBasicString(document.ToText());
}

TEST_F(SolverSymbolicTest, solver17)
{
    Start(600);

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.InsertString("123.", true);
    document.InsertString(" ", true);
    document.InsertString("ее", true);
    document.InsertDivision(true);
    document.InsertString("п", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>123.</mi>"
                            "<mfrac>"
                                "<mrow>"
                                    "<mi>ее</mi>"
                                "</mrow>"
                                "<mrow>"
                                    "<mi>п</mi>"
                                "</mrow>"
                            "</mfrac>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mfrac>"
                                "<mrow>"
                                    "<mi>123</mi>"
                                    "<mo>×</mo>"
                                    "<mi>ее</mi>"
                                "</mrow>"
                                "<mrow>"
                                    "<mi>п</mi>"
                                "</mrow>"
                            "</mfrac>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
}

TEST_F(SolverSymbolicTest, solver18)
{
    Start(600);

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.InsertString("4", true);
    document.InsertString(" ", true);
    document.InsertString("м", true);
    document.InsertDivision(true);
    document.InsertString("с", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>4</mi>"
                            "<mfrac>"
                                "<mrow>"
                                    "<mi>м</mi>"
                                "</mrow>"
                                "<mrow>"
                                    "<mi>с</mi>"
                                "</mrow>"
                            "</mfrac>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mfrac>"
                                "<mrow>"
                                    "<mi>4</mi>"
                                    "<mo>×</mo>"
                                    "<mi>м</mi>"
                                "</mrow>"
                                "<mrow>"
                                    "<mi>с</mi>"
                                "</mrow>"
                            "</mfrac>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
}

TEST_F(SolverSymbolicTest, solver19)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1.23456789", true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"1.23456789=1.235") << ToBasicString(document.ToText());
}

TEST_F(SolverSymbolicTest, solver20)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("12345", true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    document.WaitTask(document.SetExp({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 2, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>12345</mi>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mi>1.234</mi>"
                                "<mo>×</mo>"
                                "<msup>"
                                    "<mrow>"
                                        "<mi>10</mi>"
                                    "</mrow>"
                                    "<mrow>"
                                        "<mi>4</mi>"
                                    "</mrow>"
                                "</msup>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
}

TEST_F(SolverSymbolicTest, solver21)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("12345", true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    document.WaitTask(document.SetExp({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 10, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>12345</mi>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mi>12345</mi>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
}

TEST_F(SolverSymbolicTest, solver22)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("12345", true);
    document.InsertPlus(true);
    document.InsertString("67890", true);
    document.InsertString("i", true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_COMPLEX, true));
    document.WaitSolver();
    document.WaitTask(document.SetExp({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 2, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>12345</mi>"
                            "<mo>+</mo>"
                            "<mi>67890i</mi>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mi>1.234</mi>"
                                "<mo>×</mo>"
                                "<msup>"
                                    "<mrow>"
                                        "<mi>10</mi>"
                                    "</mrow>"
                                    "<mrow>"
                                        "<mi>4</mi>"
                                    "</mrow>"
                                "</msup>"
                                "<mo>+</mo>"
                                "<mi>6.789</mi>"
                                "<mo>×</mo>"
                                "<msup>"
                                    "<mrow>"
                                        "<mi>10</mi>"
                                    "</mrow>"
                                    "<mrow>"
                                        "<mi>4</mi>"
                                    "</mrow>"
                                "</msup>"
                                "<mo>×</mo>"
                                "<mi>i</mi>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
}

TEST_F(SolverSymbolicTest, indefinite_integral1)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertIndefiniteIntegral(true));
    document.InsertString("x", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("x", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"indefinite_integral(x,x)=0.5*pow(x,2)") << ToBasicString(document.ToText());
}

TEST_F(SolverSymbolicTest, definite_integral1)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertDefiniteIntegral(true));
    document.InsertString("0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.InsertMultiply(true);
    document.InsertString("y", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("y", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,1,x*y,y)=0.5*x") << ToBasicString(document.ToText());
}

TEST_F(SolverSymbolicTest, derivative1)
{
    Start(600);

    document.WaitTask(document.InsertDerivative(U"d", 1, true));
    document.InsertString("x", true);
    document.InsertPower(true);
    document.InsertString("2", true);
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("x", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"derivative(pow(x,2),x)=2*x") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mfrac>"
                                "<mrow>"
                                    "<mi>d</mi>"
                                    "<msup>"
                                        "<mrow>"
                                            "<mi>x</mi>"
                                        "</mrow>"
                                        "<mrow>"
                                            "<mi>2</mi>"
                                        "</mrow>"
                                    "</msup>"
                                "</mrow>"
                                "<mrow>"
                                    "<mi>d</mi>"
                                    "<mi>x</mi>"
                                "</mrow>"
                            "</mfrac>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mi>2</mi>"
                                "<mo>×</mo>"
                                "<mi>x</mi>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(SolverSymbolicTest, derivative_mixed_func)
{
    Start(600);

    //g(x,y) := x^2*sin(y)
    document.InsertCode(false, true);
    document.InsertString("g", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertComma(true);
    document.InsertString("y", true);
    document.InsertCloseRoundBracket(true);
    document.InsertAssignment(true);
    document.InsertString("x", true);
    document.InsertPower(true);
    document.InsertString("2", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.InsertMultiply(true);
    document.InsertString("sin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("y", true);
    document.WaitTask(document.InsertCloseRoundBracket(true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.WaitTask(document.MoveCaretRight(false));
    document.InsertParagraph(true);

    //d^2 g(x,y) / d x d y, result type is explicitly REAL
    CreateDerivativeDivision(2, U"g(x,y)", std::vector<std::u32string>{U"x", U"y"});
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"g(x,y)=pow(x,2)*sin(y)\nderivative(derivative(g(x,y),y),x)=2*x*cos(y)") << ToBasicString(document.ToText());
}

TEST_F(SolverSymbolicTest, derivative_mixed_func_real)
{
    Start(600);

    //g(x,y) = pow(x,2)*sin(y)
    document.InsertCode(false, true);
    document.InsertString("g", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertComma(true);
    document.InsertString("y", true);
    document.InsertCloseRoundBracket(true);
    document.InsertAssignment(true);
    document.InsertString("x", true);
    document.InsertPower(true);
    document.InsertString("2", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.InsertMultiply(true);
    document.InsertString("sin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("y", true);
    document.WaitTask(document.InsertCloseRoundBracket(true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.WaitTask(document.MoveCaretRight(false));
    document.InsertParagraph(true);

    //d^2 g(x,y) / d x d y with explicit REAL result type
    CreateDerivativeDivision(2, U"g(x,y)", std::vector<std::u32string>{U"x", U"y"});
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"g(x,y)=pow(x,2)*sin(y)\nderivative(derivative(g(x,y),y),x)=2*x*cos(y)") << ToBasicString(document.ToText());
}

}
