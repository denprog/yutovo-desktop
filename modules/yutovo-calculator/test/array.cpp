/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include <chrono>
#include "parser_exception.h"

namespace yutovo_calc_test
{

using namespace yutovo_calculator;
using namespace std::chrono_literals;

TEST_F(CalcTestArrayReal, arrays1)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"[1,2,3];");
    ASSERT_TRUE(res.ToStdString(3, 3) == "[1.E+0,2.E+0,3.E+0]") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"[];");
    ASSERT_TRUE(res.ToStdString(3, 3) == "[]") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"[123];");
    ASSERT_TRUE(res.ToStdString(3, 3) == "[123.E+0]") << res.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, arrays2)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"[1+2,2.3,345E+1];");
    ASSERT_TRUE(res.ToStdString(3, 3) == "[3.E+0,2.3E+0,3.45E+3]") << res.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, arrays3)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x,y)=x+y;");
    auto res = parser.Parse(LogicalId{0, 0, 2}, U"[1,f(2,3),2.3,6/2];");
    ASSERT_TRUE(res.ToStdString(3, 3) == "[1.E+0,5.E+0,2.3E+0,3.E+0]") << res.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, arrays4)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"[1234567890];");
    ASSERT_TRUE(res.ToStdString(3, 3) == "[1.235E+9]") << res.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, functions1)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"[exp(1)];", 5);
    ASSERT_TRUE(r.ToStdString(3, 5) == "[2.71828E+0]") << r.ToStdString(3, 5);
}

TEST_F(CalcTestArrayReal, functions2)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"3pow(2,3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[24.E+0]") << r.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, functions3)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"pow([2,3,4],[3,2,1]);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[8.E+0,9.E+0,4.E+0]") << r.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, functions4)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"[abs(-2.3),abs(2.3)];", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[2.3E+0,2.3E+0]") << r.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, functions5)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"[integer(-2.3),integer(-2.7),integer(2.3),integer(2.6)];", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[-2.E+0,-2.E+0,2.E+0,2.E+0]") << r.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, functions6)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"fract(-2.3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[-0.3E+0]") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"fract(2.3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[0.3E+0]") << r.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, functions7)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"[ceil(-2.6),ceil(-2.3),ceil(2.3),ceil(2.5)];", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[-2.E+0,-2.E+0,3.E+0,3.E+0]") << r.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, functions8)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"[floor(-2.6),floor(2.5),floor(-2.3),floor(2.3)];", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[-3.E+0,2.E+0,-3.E+0,2.E+0]") << r.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, functions9)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"round(-2.3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[-2.E+0]") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"round(-2.5);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[-3.E+0]") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"[round(2.3),round(2.5)];", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[2.E+0,3.E+0]") << r.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, functions10)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"trunc(-2.3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[-2.E+0]") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"trunc(-2.5);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[-2.E+0]") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"[trunc(2.5)];", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[2.E+0]") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"[trunc(2.1)];", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[2.E+0]") << r.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, sum1)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"var=[1,2,3];", 3);
    r = parser.Parse(LogicalId{0, 0, 2}, U"loop(i=0,(i<=(size(var)-1)),i=i+1,t=0,t=t+var{i});");
    ASSERT_TRUE(r.ToStdString(3, 3) == "[6.E+0]") << r.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, sum2)
{
    auto r = parser.Parse(LogicalId{0, 0, 2}, U"loop(i=1,(i<=3),i=i+1,t=0,t=t+[1,2,3]);");
    ASSERT_TRUE(r.ToStdString(3, 3) == "[3.E+0,6.E+0,9.E+0]") << r.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, sum3)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"d=[1.23,pow(2,3),-55,4];");
    r = parser.Parse(LogicalId{0, 0, 2}, U"loop(n=0,(n<=size(d)-1),n=n+1,n_=0,n_=n_+d{n});");
    ASSERT_TRUE(r.ToStdString(3, 3) == "[-41.77E+0]") << r.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, variables1)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"t=[2,3,4];", 3);
    r = parser.Parse(LogicalId{0, 0, 2}, U"t;", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[2.E+0,3.E+0,4.E+0]") << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 2}, U"t{0};", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[2.E+0]") << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 2}, U"t{1};", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[3.E+0]") << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 2}, U"t{2};", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "[4.E+0]") << r.ToStdString(3, 3);

    try
    {
        parser.Parse(LogicalId{0, 0, 2}, U"t{3};", 3);
        ASSERT_FALSE(true);
    }
    catch (yutovo_calculator::SyntaxException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 2}) && ex.ex_id == ParserExceptionCode::ArgumentIsOver && ex.pos == 0) << LogicalIdToString(ex.id);
    }

    try
    {
        parser.Parse(LogicalId{0, 0, 2}, U"t{-1};", 3);
        ASSERT_FALSE(true);
    }
    catch (yutovo_calculator::SyntaxException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 2}) && ex.ex_id == ParserExceptionCode::SyntaxError && ex.pos == 2) << LogicalIdToString(ex.id);
    }

    try
    {
        parser.Parse(LogicalId{0, 0, 2}, U"t{n};", 3);
        ASSERT_FALSE(true);
    }
    catch (yutovo_calculator::SyntaxException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 2}) && ex.ex_id == ParserExceptionCode::ArgumentIsOver && ex.pos == 0) << LogicalIdToString(ex.id);
    }
}

TEST_F(CalcTestArrayReal, trigonometric1)
{
    auto res = parser.Parse(LogicalId{0, 0, 1}, U"sin(0);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "[0.E+0]") << res.ToStdString(3, 3);
    
    res = parser.Parse(LogicalId{0, 0, 1}, U"arcsin(0);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Radian && res.ToStdString(3, 3) == "[0.E+0]") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"[sin(0),arcsin(0)];");
    ASSERT_TRUE(res.ToStdString(3, 3) == "[0.E+0,0.E+0]") << res.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, trigonometric2)
{
    auto res = parser.Parse(LogicalId{0, 0, 1}, U"sin(90);", AngleMeasure::Degree, AngleMeasure::Degree);
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "[1.E+0]") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arcsin(1);", AngleMeasure::Degree, AngleMeasure::Degree);
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Degree && res.ToStdString(3, 3) == "[90.E+0]") << res.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, trigonometric3)
{
    auto res = parser.Parse(LogicalId{0, 0, 1}, U"sin(deg(90));", AngleMeasure::Radian, AngleMeasure::Radian);
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "[1.E+0]") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"deg(arcsin(1));", AngleMeasure::Radian, AngleMeasure::Degree);
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Degree && res.ToStdString(3, 3) == "[90.E+0]") << res.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, trigonometric4)
{
    auto res = parser.Parse(LogicalId{0, 0, 1}, U"sin(90°);", AngleMeasure::Radian, AngleMeasure::Radian);
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "[1.E+0]") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"sin((pi)/(2)rad);", AngleMeasure::Radian, AngleMeasure::Radian);
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "[1.E+0]") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"[sin(90°),sin((pi)/(2)rad)];", AngleMeasure::Radian, AngleMeasure::Radian);
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "[1.E+0,1.E+0]") << res.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, graph1)
{
    auto res = parser.Parse(LogicalId{0, 0, 1}, U"graph_line(sin(x),x,0,1,0,1,50);", AngleMeasure::Radian, AngleMeasure::Radian);
    ASSERT_TRUE(res.ToStdString(3, 3).substr(0, 100) == 
        "[0.E+0,1.E+0,0.E+0,1.E+0,0.E+0,0.E+0,0.02E+0,0.02E+0,0.04E+0,0.04E+0,0.06E+0,0.06E+0,0.08E+0,0.0799E") <<
        res.ToStdString(3, 3).substr(0, 100);
}

TEST_F(CalcTestArrayReal, graph2)
{
    yutovo_calculator::ParserContext parser_context;
    parser_context.Init(100);
    auto res = parser.Parse(LogicalId{0, 0, 1}, U"graph_line(sin(x),x,0,100,0,100,100);", &parser_context);
    ASSERT_TRUE(res.ToStdString(3, 3).substr(0, 100) == 
        "[0.E+0,100.E+0,0.E+0,100.E+0,0.E+0,0.E+0,1.E+0,0.841E+0,2.E+0,0.909E+0,3.E+0,0.141E+0,4.E+0,-0.757E+") << 
        res.ToStdString(3, 3).substr(0, 100);
}

TEST_F(CalcTestArrayReal, graph3)
{
    yutovo_calculator::ParserContext parser_context;
    parser_context.Init(100);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"graph_line(sin(x),x,1,1,0,100,100);", &parser_context), yutovo_calculator::MathException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"graph_line(sin(x),x,-1,1,10,10,10);", &parser_context), yutovo_calculator::MathException);
}

TEST_F(CalcTestArrayReal, graph4)
{
    yutovo_calculator::ParserContext parser_context;
    parser_context.Init(100);
    try
    {
        parser.Parse(LogicalId{0, 0, 1}, U"graph_line(abs(x),x{y},-1,1,0,1,100);", &parser_context);
        ASSERT_FALSE(true);
    }
    catch (yutovo_calculator::SyntaxException& ex)
    {
        ASSERT_TRUE(ex.ex_id == ParserExceptionCode::UnknownIdentifier && ex.pos == 18);
    }
}

TEST_F(CalcTestArrayReal, graph5)
{
    yutovo_calculator::ParserContext parser_context;
    parser_context.Init(100);
    auto res = parser.Parse(LogicalId{0, 0, 1}, U"graph_line((pow(x,2))/(sqrt(x)),x,0,10,0,10,100);", &parser_context);
    ASSERT_TRUE(res.ToStdString(3, 3).substr(0, 100) == 
        "[0.E+0,10.E+0,0.E+0,10.E+0,0.E+0,NAN,0.1E+0,0.0316E+0,0.2E+0,0.0894E+0,0.3E+0,0.164E+0,0.4E+0,0.253E") << 
        res.ToStdString(3, 3).substr(0, 100);
}

TEST_F(CalcTestArrayReal, graph6)
{
    yutovo_calculator::ParserContext parser_context;
    parser_context.Init(100);
    auto res = parser.Parse(LogicalId{0, 0, 1}, U"graph_line((pow(x,2))/(sqrt(x)),x,-0.5,10,-1,10,100);", &parser_context);
    ASSERT_TRUE(res.ToStdString(3, 3).substr(0, 200) == 
        "[-0.5E+0,10.E+0,-1.E+0,10.E+0,-0.5E+0,NAN,-0.395E+0,NAN,-0.29E+0,NAN,-0.185E+0,NAN,-0.08E+0,NAN,0.025E+0,0.00395E+0,"\
        "0.13E+0,0.0469E+0,0.235E+0,0.114E+0,0.34E+0,0.198E+0,0.445E+0,0.297E+0,0.55E+0,0.408") << 
        res.ToStdString(3, 3).substr(0, 200);
}

TEST_F(CalcTestArrayReal, units1)
{
    parser.SetLocale(Language::Russian);
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"[5м,3*(м/с)];");
    ASSERT_TRUE(r.ToStdString(3, 3) == "[5.E+0(м),3.E+0((м)/(с))]") << r.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, errors1)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"(1)/(0);"), yutovo_calculator::MathException);
}

TEST_F(CalcTestArrayReal, min_max1)
{
    auto res = parser.Parse(LogicalId{0, 0, 1}, U"min([3,1,4,1,5,9,2,6]);", 3);
    ASSERT_TRUE(res.ToStdString(3, 3) == "[1.E+0]") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"max([3,1,4,1,5,9,2,6]);", 3);
    ASSERT_TRUE(res.ToStdString(3, 3) == "[9.E+0]") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"min([-2.5,-3.5,-1.5]);", 3);
    ASSERT_TRUE(res.ToStdString(3, 3) == "[-3.5E+0]") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"max([-2.5,-3.5,-1.5]);", 3);
    ASSERT_TRUE(res.ToStdString(3, 3) == "[-1.5E+0]") << res.ToStdString(3, 3);
}

TEST_F(CalcTestArrayReal, definite_integral1)
{
    auto res = parser.Parse(LogicalId{0, 0, 1}, U"definite_integral(0,1,x,x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "[0.5E+0]") << res.ToStdString(10, 10);
}

TEST_F(CalcTestArrayReal, definite_integral2)
{
    auto res = parser.Parse(LogicalId{0, 0, 1}, U"definite_integral(-1,4,sin(x),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "[1.1939459267E+0]") << res.ToStdString(10, 10);
}

}
