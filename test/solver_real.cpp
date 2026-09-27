/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"
#include <yutovo-calculator/unit.h>

namespace yutovo_test
{

using namespace yutovo;
using namespace yutovo_solver;
using namespace std::chrono_literals;

//Present Auto result as Real result
TEST_F(SolverRealTest, solver1)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("1.23456789", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1.23456789</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>1.235</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetResultType({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1.23456789</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>1.235</mi>"\
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
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"1.23456789=1.235"
        ) << ToBasicString(document.ToText());
}

//Show angle measure
TEST_F(SolverRealTest, solver2)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("arcsin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("1", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(1)=1.571(rad)"
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("sin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("1", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(1)=1.571(rad)\n"
        U"sin(1)=0.841"
        ) << ToBasicString(document.ToText());

    Config config;
    document.GetConfig(config);
    config.real_result.show_angle_measure = false;
    document.SetConfig(config, true);

    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("arcsin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("1", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(1)=1.571(rad)\n"
        U"arcsin(1)=1.571\n"
        U"sin(1)=0.841"
        ) << ToBasicString(document.ToText());
}

//Change result angle measure
TEST_F(SolverRealTest, solver3)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("arccos", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("0.5", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"arccos(0.5)=1.047(rad)"
        ) << ToBasicString(document.ToText());

    Config config;
    document.GetConfig(config);
    config.real_result.result_angle_measure = AngleMeasure::Degree;
    document.SetConfig(config, true);

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("arccos", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("0.5", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"arccos(0.5)=1.047(rad)\n"
        U"arccos(0.5)=60.(deg)"
        ) << ToBasicString(document.ToText());
}

//Change precision
TEST_F(SolverRealTest, solver4)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("12.3456789", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"12.3456789=12.346"
        ) << ToBasicString(document.ToText());

    Config config;
    document.GetConfig(config);
    config.real_result.precision = 5;
    document.SetConfig(config, true);

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("12.3456789", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"12.3456789=12.346\n"
        U"12.3456789=12.34568"
        ) << ToBasicString(document.ToText());
}

//Change exponential threshold
TEST_F(SolverRealTest, solver5)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("123456789", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"123456789=123456789."
        ) << ToBasicString(document.ToText());

    Config config;
    document.GetConfig(config);
    config.real_result.exp = 5;
    document.SetConfig(config, true);

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("1234567", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"123456789=123456789.\n"
        U"1234567=1.235*pow(10,6)"
        ) << ToBasicString(document.ToText());
}

//Change precision
TEST_F(SolverRealTest, solver6)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("12.3456789012345", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"12.3456789012345=12.346"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetPrecision({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 7, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"12.3456789012345=12.3456789"
        ) << ToBasicString(document.ToText());
}

//Change exponential threshold
TEST_F(SolverRealTest, solver7)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("123456789012", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"123456789012=1.235*pow(10,11)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetExp({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 12, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"123456789012=123456789012."
        ) << ToBasicString(document.ToText());
}

//Change angle measure
TEST_F(SolverRealTest, solver8)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("arcsin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("1", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(1)=1.571(rad)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetAngleMeasure({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, AngleMeasure::Radian, AngleMeasure::Degree, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(1)=90.(deg)"
        ) << ToBasicString(document.ToText());
}

//Save and load
TEST_F(SolverRealTest, solver9)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("234.45", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    document.Save("solver4_1.yut");

    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.WaitTask(document.Load("solver4_1.yut"));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"234.45=234.45") << ToBasicString(document.ToText());
}

//Save and load with changing a result parameter
TEST_F(SolverRealTest, solver10)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("234.12345678", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitTask(document.SetPrecision({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 7, true));
    document.WaitSolver();
    document.Save("solver10_1.yut");

    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.WaitTask(document.Load("solver10_1.yut"));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"234.12345678=234.1234568") << ToBasicString(document.ToText());

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("1", true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"1234.12345678=1234.1234568") << ToBasicString(document.ToText());
}

//Factorial
TEST_F(SolverRealTest, solver11)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("5", true);
    document.InsertExclamation(true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"5!=120."
        ) << ToBasicString(document.ToText());
}

//Percent
TEST_F(SolverRealTest, solver12)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("11", true);
    document.InsertPercent(true);
    document.InsertString("234", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"11%234=25.74"
        ) << ToBasicString(document.ToText());
}

//Change angle measure
TEST_F(SolverRealTest, solver13)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("sin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("1", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"sin(1)=0.841"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetAngleMeasure({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, AngleMeasure::Degree, AngleMeasure::Radian, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"sin(1)=0.0175"
        ) << ToBasicString(document.ToText());
}

//Changing unit of result
TEST_F(SolverRealTest, units1)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("1m", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"1m=1.m") << ToBasicString(document.ToText());

    std::vector<yutovo_calculator::Unit> cast_units;
    document.GetCastUnits({0, 0, 0, 0, 0, 0, 0, 2, 0}, cast_units);
    ASSERT_TRUE(std::find(cast_units.begin(), cast_units.end(), yutovo_calculator::Unit(U"m")) != cast_units.end());
    ASSERT_TRUE(std::find(cast_units.begin(), cast_units.end(), yutovo_calculator::Unit(U"mm")) != cast_units.end());
    ASSERT_TRUE(std::find(cast_units.begin(), cast_units.end(), yutovo_calculator::Unit(U"km")) != cast_units.end());

    yutovo_calculator::Unit unit(U"mm");
    document.WaitTask(document.SetUnit({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, unit, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"1m=1000.mm") << ToBasicString(document.ToText());
}

//Change unit of result and remove the upper paragraph
TEST_F(SolverRealTest, units2)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("20ms", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"\n20ms=20.ms") << ToBasicString(document.ToText());

    yutovo_calculator::Unit unit(U"s");
    document.WaitTask(document.SetUnit({0, 0, 0, 0, 1, 0, 0, 2, 0, 0}, unit, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"\n20ms=0.02s") << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"20ms=0.02s") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"\n20ms=0.02s") << ToBasicString(document.ToText());
}

//Change result unit and then change the expression
TEST_F(SolverRealTest, units3)
{
    Start(600);
    
    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123с", true));
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"123с=2.05мин"
        ) << ToBasicString(document.ToText());

    yutovo_calculator::Unit unit;
    unit.FromString(U"мс");
    document.WaitTask(document.SetUnit({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, unit, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"123с=123000.мс"
        ) << ToBasicString(document.ToText());
    
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitTask(document.InsertString("м", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"123м=123.м"
        ) << ToBasicString(document.ToText());
}

TEST_F(SolverRealTest, definite_integral1)
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
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,1,x,x)=0.5") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<munderover>"
                                "<mo>∫</mo>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>0</mi>"
                                    "</mrow>"
                                "</mrow>"
                                "<mrow>"
                                    "<mrow>"
                                        "<mi>1</mi>"
                                    "</mrow>"
                                "</mrow>"
                            "</munderover>"
                            "<mrow>"
                                "<mi>x</mi>"
                            "</mrow>"
                            "<mi>d</mi>"
                            "<mrow>"
                                "<mi>x</mi>"
                            "</mrow>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mi>0.5</mi>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
}

TEST_F(SolverRealTest, definite_integral2)
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
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"definite_integral(0,2,x,x)=2.") << ToBasicString(document.ToText());
}

}
