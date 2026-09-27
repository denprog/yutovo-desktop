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

//Arithmetic form
TEST_F(SolverComplexTest, solver1)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("1.2", true);
    document.InsertPlus(true);
    document.InsertString("3.4i", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1.2</mi>"\
                            "<mo>+</mo>"\
                            "<mi>3.4i</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>1.2</mi>"\
                                "<mo>+</mo>"\
                                "<mi>3.4</mi>"\
                                "<mi>i</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Arithmetic form
TEST_F(SolverComplexTest, solver2)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("12000000000", true);
    document.InsertPlus(true);
    document.InsertString("340000000000i", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>12000000000</mi>"\
                            "<mo>+</mo>"\
                            "<mi>340000000000i</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>1.2</mi>"\
                                "<mo>×</mo>"\
                                "<msup>"\
                                    "<mrow>"\
                                        "<mi>10</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>10</mi>"\
                                    "</mrow>"\
                                "</msup>"\
                                "<mo>+</mo>"\
                                "<mi>3.4</mi>"\
                                "<mo>×</mo>"\
                                "<msup>"\
                                    "<mrow>"\
                                        "<mi>10</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>11</mi>"\
                                    "</mrow>"\
                                "</msup>"\
                                "<mi>i</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Exponential form
TEST_F(SolverComplexTest, solver3)
{
    Start(600);
    
    document.GetConfig(config);
    config.complex_result.form = ComplexForm::Exponential;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("1.2", true);
    document.InsertPlus(true);
    document.InsertString("3.4i", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1.2</mi>"\
                            "<mo>+</mo>"\
                            "<mi>3.4i</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>3.606</mi>"\
                                "<msup>"\
                                    "<mrow>"\
                                        "<mi>e</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>1.232</mi>"\
                                        "<mi>i</mi>"\
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

//Exponential form
TEST_F(SolverComplexTest, solver4)
{
    Start(600);
    
    document.GetConfig(config);
    config.complex_result.form = ComplexForm::Exponential;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("12000000000", true);
    document.InsertPlus(true);
    document.InsertString("340000000000i", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>12000000000</mi>"\
                            "<mo>+</mo>"\
                            "<mi>340000000000i</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>3.402</mi>"\
                                "<mo>×</mo>"\
                                "<msup>"\
                                    "<mrow>"\
                                        "<mi>10</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>11</mi>"\
                                    "</mrow>"\
                                "</msup>"\
                                "<msup>"\
                                    "<mrow>"\
                                        "<mi>e</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>1.536</mi>"\
                                        "<mi>i</mi>"\
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

//Trigonometric form
TEST_F(SolverComplexTest, solver5)
{
    Start(600);
    
    document.GetConfig(config);
    config.complex_result.form = ComplexForm::Trigonometric;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("1.2", true);
    document.InsertPlus(true);
    document.InsertString("3.4i", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1.2</mi>"\
                            "<mo>+</mo>"\
                            "<mi>3.4i</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>3.606</mi>"\
                                "<mo>(</mo>" \
                                "<mi>cos</mi>"\
                                "<mo>(</mo>" \
                                "<mi>1.232</mi>"\
                                "<mo>)</mo>" \
                                "<mo>+</mo>"\
                                "<mi>i</mi>"\
                                "<mo>×</mo>"\
                                "<mi>sin</mi>"\
                                "<mo>(</mo>" \
                                "<mi>1.232</mi>"\
                                "<mo>)</mo>" \
                                "<mo>)</mo>" \
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Trigonometric form
TEST_F(SolverComplexTest, solver6)
{
    Start(600);
    
    document.GetConfig(config);
    config.complex_result.form = ComplexForm::Trigonometric;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("12000000000", true);
    document.InsertPlus(true);
    document.InsertString("340000000000i", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>12000000000</mi>"\
                            "<mo>+</mo>"\
                            "<mi>340000000000i</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>3.402</mi>"\
                                "<mo>×</mo>"\
                                "<msup>"\
                                    "<mrow>"\
                                        "<mi>10</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>11</mi>"\
                                    "</mrow>"\
                                "</msup>"\
                                "<mo>(</mo>" \
                                "<mi>cos</mi>"\
                                "<mo>(</mo>" \
                                "<mi>1.536</mi>"\
                                "<mo>)</mo>" \
                                "<mo>+</mo>"\
                                "<mi>i</mi>"\
                                "<mo>×</mo>"\
                                "<mi>sin</mi>"\
                                "<mo>(</mo>" \
                                "<mi>1.536</mi>"\
                                "<mo>)</mo>" \
                                "<mo>)</mo>" \
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Arithmetic form, only imajinary part
TEST_F(SolverComplexTest, solver7)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("3.4i", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"3.4i=3.4i"
        ) << ToBasicString(document.ToText());
}

//Arithmetic form, only imajinary part
TEST_F(SolverComplexTest, solver8)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertMinus(true);
    document.InsertString("3.4i", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"-3.4i=-3.4i"
        ) << ToBasicString(document.ToText());
}

//Arithmetic form, only imajinary part
TEST_F(SolverComplexTest, solver9)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("3.4i", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"3.4i=3.4i"
        ) << ToBasicString(document.ToText());
}

//Arithmetic form, only real part
TEST_F(SolverComplexTest, solver10)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("3.4", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"3.4=3.4"
        ) << ToBasicString(document.ToText());
}

//Arithmetic form, only real part
TEST_F(SolverComplexTest, solver11)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("-3.4", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"-3.4=-3.4"
        ) << ToBasicString(document.ToText());
}

//Arithmetic form
TEST_F(SolverComplexTest, solver12)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertMinus(true);
    document.InsertString("1.2", true);
    document.InsertPlus(true);
    document.InsertString("3.4i", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"-1.2+3.4i=-1.2+3.4i"
        ) << ToBasicString(document.ToText());
}

//Arithmetic form
TEST_F(SolverComplexTest, solver13)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertMinus(true);
    document.InsertString("1.2", true);
    document.InsertMinus(true);
    document.InsertString("3.4i", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"-1.2-3.4i=-1.2-3.4i"
        ) << ToBasicString(document.ToText());
}

//Arithmetic form
TEST_F(SolverComplexTest, solver14)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertMinus(true);
    document.InsertString("12000000000", true);
    document.InsertMinus(true);
    document.InsertString("340000000000i", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"-12000000000-340000000000i=-1.2*pow(10,10)-3.4*pow(10,11)i"
        ) << ToBasicString(document.ToText());
}

//Arithmetic form
TEST_F(SolverComplexTest, solver15)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertMinus(true);
    document.InsertString("12000000000", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"-12000000000=-1.2*pow(10,10)"
        ) << ToBasicString(document.ToText());
}

//Arithmetic form
TEST_F(SolverComplexTest, solver16)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertMinus(true);
    document.InsertString("340000000000i", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"-340000000000i=-3.4*pow(10,11)i"
        ) << ToBasicString(document.ToText());
}

//Set complex form
TEST_F(SolverComplexTest, solver17)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.complex_result.show_angle_measure = true;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("1.2", true);
    document.InsertPlus(true);
    document.InsertString("3.4i", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"1.2+3.4i=1.2+3.4i"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetComplexForm({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, ComplexForm::Trigonometric, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"1.2+3.4i=3.606(cos(1.232)+i*sin(1.232))"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"1.2+3.4i=1.2+3.4i"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetComplexForm({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, ComplexForm::Exponential, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"1.2+3.4i=3.606pow(e,1.232i)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"1.2+3.4i=1.2+3.4i"
        ) << ToBasicString(document.ToText());
}

//Get two results
TEST_F(SolverComplexTest, solver18)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertSquareRoot(true);
    document.InsertString("i", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"sqrt(i)=0.707+0.707i,-0.707-0.707i"
        ) << ToBasicString(document.ToText());
}

//Calculate arcsin
TEST_F(SolverComplexTest, solver19)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("arcsin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("i", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(i)=0.881i(rad),3.142-0.881i(rad)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetAngleMeasure({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, AngleMeasure::Radian, AngleMeasure::Degree, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(i)=50.499i(deg),180.-50.499i(deg)"
        ) << ToBasicString(document.ToText());
}

//Check precision
TEST_F(SolverComplexTest, solver20)
{
    Start(600);

    document.GetConfig(config);
    config.complex_result.precision = 9;
    config.auto_result.complex_result.precision = 9;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("123.123456789i", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"123.123456789i=123.123456789i"
        ) << ToBasicString(document.ToText());
}

//Check no errors
TEST_F(SolverComplexTest, solver21)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("z", true);
    document.InsertAssignment(true);
    document.InsertString("2", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("3i", true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"z=2+3i"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

//Check complex functions
TEST_F(SolverComplexTest, solver22)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("z", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("4", true));
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("arg", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("z", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"z=4\n"
        U"arg(z)=0.(rad)"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("mod", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("z", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"z=4\n"
        U"arg(z)=0.(rad)\n"
        U"mod(z)=4."
        ) << ToBasicString(document.ToText());
}

//Recalculate dependent expressions after fixing an unknown identifier
TEST_F(SolverComplexTest, solver23)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });
    
    document.InsertCode(false, true);
    document.InsertString("z", true);
    document.InsertAssignment(true);
    document.InsertString("2.3", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("4.5j", true));
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("re", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("z", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("im", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("z", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("abs", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("z", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"z=2.3+4.5j\n"
        U"re(z)=Unknown identifier\n"
        U"im(z)=Unknown identifier\n"
        U"abs(z)=Unknown identifier"
        ) << ToBasicString(document.ToText());

    //replace "j" with "i"
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretUp(false));
    document.MoveCaretEnd(false);
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitTask(document.InsertString("i", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"z=2.3+4.5i\n"
        U"re(z)=2.3\n"
        U"im(z)=4.5\n"
        U"abs(z)=5.054"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

TEST_F(SolverComplexTest, definite_integral1)
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
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("x", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,1,x,x)=0.5") << ToBasicString(document.ToText());
}

TEST_F(SolverComplexTest, definite_integral2)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertDefiniteIntegral(true));
    document.InsertString("0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("x", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,2,x,x)=2.") << ToBasicString(document.ToText());
}

}
