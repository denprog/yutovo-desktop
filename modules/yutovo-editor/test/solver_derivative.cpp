/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"
#include "formulas/division.h"
#include "formulas/power.h"
#include "formulas/code_string.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace yutovo_solver;
using namespace std::chrono_literals;

TEST_F(SolverAutoTest, derivative1)
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
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
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

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative(pow(x,2),x)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, derivative2)
{
    Start(600);

    document.WaitTask(document.InsertDerivative(U"d", 1, true));
    document.InsertString("sin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("x", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"derivative(sin(x),x)=cos(x)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mfrac>"
                                "<mrow>"
                                    "<mi>d</mi>"
                                    "<mi>sin</mi>"
                                    "<mo>(</mo>"
                                    "<mi>x</mi>"
                                    "<mo>)</mo>"
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
                                "<mi>cos</mi>"
                                "<mo>(</mo>"
                                "<mi>x</mi>"
                                "<mo>)</mo>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(SolverAutoTest, derivative_second1)
{
    Start(600);
    CreateDerivativeDivision(2, U"x^3", U"x");
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"derivative(derivative(pow(x,3),x),x)=6*x") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, derivative_third1)
{
    Start(600);
    CreateDerivativeDivision(3, U"x^3", U"x");
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"derivative(derivative(derivative(pow(x,3),x),x),x)=6") << ToBasicString(document.ToText());
}

//User-defined function f(x,y); mixed partial derivative by x and y
TEST_F(SolverAutoTest, derivative_mixed_func)
{
    Start(600);

    //f(x,y) = pow(x,2)*sin(y)
    document.InsertCode(false, true);
    document.InsertString("f", true);
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
    document.InsertCloseRoundBracket(true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.WaitTask(document.MoveCaretRight(false));
    document.InsertParagraph(true);

    //d^2 f(x,y) / d x d y
    CreateDerivativeDivision(2, U"f(x,y)", std::vector<std::u32string>{U"x", U"y"});
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"f(x,y)=pow(x,2)*sin(y)\nderivative(derivative(f(x,y),y),x)=2*x*cos(y)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, derivative_mixed_func_g)
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
    document.InsertCloseRoundBracket(true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.WaitTask(document.MoveCaretRight(false));
    document.InsertParagraph(true);

    //d^2 g(x,y) / d x d y
    CreateDerivativeDivision(2, U"g(x,y)", std::vector<std::u32string>{U"x", U"y"});
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"g(x,y)=pow(x,2)*sin(y)\nderivative(derivative(g(x,y),y),x)=2*x*cos(y)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, derivative_tan)
{
    Start(600);
    CreateDerivativeDivision(1, U"x^2*tan(x)", U"x");
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"derivative(pow(x,2)*tan(x),x)=2*x*tan(x)+pow(x,2)*(1+pow(tan(x),2))") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, derivative_cos)
{
    Start(600);
    CreateDerivativeDivision(1, U"cos(x)", U"x");
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"derivative(cos(x),x)=-sin(x)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, derivative_exp)
{
    Start(600);
    CreateDerivativeDivision(1, U"exp(x)", U"x");
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"derivative(exp(x),x)=pow(e,x)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, derivative_x_sin)
{
    Start(600);
    CreateDerivativeDivision(1, U"x*sin(x)", U"x");
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"derivative(x*sin(x),x)=sin(x)+x*cos(x)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, derivative_tan_simple)
{
    Start(600);
    CreateDerivativeDivision(1, U"tan(x)", U"x");
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"derivative(tan(x),x)=1+pow(tan(x),2)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, derivative_x4)
{
    Start(600);
    document.WaitTask(document.InsertDerivative(U"d", 1, true));
    document.InsertString("x", true);
    document.InsertPower(true);
    document.InsertString("4", true);
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("x", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"derivative(pow(x,4),x)=4*pow(x,3)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, derivative_second_sin)
{
    Start(600);
    document.WaitTask(document.InsertDerivative(U"d", 2, true));
    document.InsertString("sin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("x", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"derivative(derivative(sin(x),x),x)=-sin(x)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, partial_dx_x2_plus_y2)
{
    Start(600);
    CreateDerivativeDivision(1, U"x^2+y^2", U"x");
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"derivative(pow(x,2)+pow(y,2),x)=2*x") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, partial_mixed_x2_y3)
{
    Start(600);
    CreateDerivativeDivision(2, U"x^2*y^3", std::vector<std::u32string>{U"x", U"y"});
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"derivative(derivative(pow(x,2)*pow(y,3),y),x)=6*x*pow(y,2)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, partial_mixed_sin_cos)
{
    Start(600);
    CreateDerivativeDivision(2, U"sin(x)*cos(y)", std::vector<std::u32string>{U"x", U"y"});
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"derivative(derivative(sin(x)*cos(y),y),x)=-cos(x)*sin(y)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, partial_dy_x2_y)
{
    Start(600);
    CreateDerivativeDivision(1, U"x^2*y", U"y");
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"derivative(pow(x,2)*y,y)=pow(x,2)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, derivative_edit_function_power)
{
    Start(600);

    //d(x^2)/dx = 2*x
    document.WaitTask(document.InsertDerivative(U"d", 1, true));
    document.WaitTask(document.InsertString(U"x", true));
    document.WaitTask(document.InsertPower(true));
    document.WaitTask(document.InsertString(U"2", true));
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.InsertString(U"x", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"derivative(pow(x,2),x)=2*x") << ToBasicString(document.ToText());

    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    for (int i = 0; i < 4; ++i)
        document.MoveCaretRight(false);
    document.DeleteElements(false, true);
    document.WaitTask(document.InsertString(U"3", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"derivative(pow(x,3),x)=3*pow(x,2)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, derivative_edit_variable)
{
    Start(600);

    // d(x^2)/dx = 2*x
    document.WaitTask(document.InsertDerivative(U"d", 1, true));
    document.WaitTask(document.InsertString(U"x", true));
    document.WaitTask(document.InsertPower(true));
    document.WaitTask(document.InsertString(U"2", true));
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.InsertString(U"x", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"derivative(pow(x,2),x)=2*x") << ToBasicString(document.ToText());

    // Undo the equation and edit the variable: replace x with y
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.DeleteElements(true, true);
    document.WaitTask(document.InsertString(U"y", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"derivative(pow(x,2),y)=0") << ToBasicString(document.ToText());
}

//Derivative at a point: d(x^2)/dx |_{x=2} = 4
TEST_F(SolverAutoTest, derivative_at_point1)
{
    Start(600);

    document.WaitTask(document.InsertDerivative(U"d", 1, true));
    document.WaitTask(document.InsertString("x", true));
    document.WaitTask(document.InsertPower(true));
    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.InsertString("x", true));
    document.WaitTask(document.MoveCaretRight(false));

    document.WaitTask(document.InsertEvaluationBarSubscript(true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("x", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(false));

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"derivative(pow(x,2))[x=2]=4.") << ToBasicString(document.ToText());
}

//Derivative at a point: d(sin(x))/dx |_{x=0} = 1
TEST_F(SolverAutoTest, derivative_at_point2)
{
    Start(600);

    document.InsertDerivative(U"d", 1, true);
    document.InsertString("sin", true);
    document.WaitTask(document.InsertOpenRoundBracket(true));
    document.WaitTask(document.InsertString("x", true));
    document.WaitTask(document.InsertCloseRoundBracket(true));
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.InsertString("x", true));
    document.WaitTask(document.MoveCaretRight(false));

    document.WaitTask(document.InsertEvaluationBarSubscript(true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("x", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("0", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(false));

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"derivative(sin(x))[x=0]=1.") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, derivative_at_point3)
{
    Start(600);

    document.Load("../../test/tests/derivative_at_point3.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    document.WaitSolver();
    std::this_thread::sleep_for(3s);
    ASSERT_TRUE(document.ToText() == 
        U"f(x,y)=pow(x,2)*y+sin(y)\n"
        U"derivative(f(x,y))[x=1,y=3]=6."
        ) << ToBasicString(document.ToText());
}

}
