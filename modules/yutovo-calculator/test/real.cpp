/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include <chrono>
#include <condition_variable>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <thread>
#include "parser_exception.h"
#include "export.h"

namespace yutovo_calc_test
{

using namespace yutovo_calculator;
using namespace std::chrono_literals;

TEST_F(CalcTestReal, numbers1)
{
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1;").ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1;").ToStdString(3, 3);
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 1}, U"0.2;").ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0.2;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 0, 1}, U"0.2;").ToStdString(3, 3);
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 1}, U"0.2E+0;").ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"0.2;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 0, 1}, U"0.2;").ToStdString(3, 3);
}

TEST_F(CalcTestReal, numbers2)
{
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2(3/4);").ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2.75;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2(3/4);").ToStdString(3, 3);
}

TEST_F(CalcTestReal, numbers3)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(1+2);");
    ASSERT_TRUE(res.angle_measure == AngleMeasure::None);
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"3;").ToStdString(3, 3));

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(1-2);");
    ASSERT_TRUE(res.angle_measure == AngleMeasure::None);
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-1;").ToStdString(3, 3));

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(1*2);");
    ASSERT_TRUE(res.angle_measure == AngleMeasure::None);
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2;").ToStdString(3, 3));

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(1/2);");
    ASSERT_TRUE(res.angle_measure == AngleMeasure::None);
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U".5;").ToStdString(3, 3));
}

TEST_F(CalcTestReal, numbers4)
{
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"21.2;").ToStdString(3, 3) == "21.2E+0");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"21.2E+0;").ToStdString(3, 3) == "21.2E+0");
}

TEST_F(CalcTestReal, numbers5)
{
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"+5.46;");
    ASSERT_TRUE(res.ToString(3, 3) == U"5.46E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-5.55;");
    ASSERT_TRUE(res.ToString(3, 3) == U"-5.55E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, numbers6)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2(4+5);").ToStdString(3, 3);
    ASSERT_TRUE(s == "18.E+0") << s;
}

TEST_F(CalcTestReal, numbers7)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"E;"), yutovo_calculator::SyntaxException);
    auto s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1E+0;").ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0") << s;
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"E+0;"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"E+1;"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"E-1;"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestReal, numbers8)
{
    parser.SetLocale(Language::Russian);
    auto res = parser.Parse(LogicalId{0, 0, 2}, U"21.2;").ToStdString(3, 3);
    ASSERT_TRUE(res == "21.2E+0") << res;
    res = parser.Parse(LogicalId{0, 0, 2}, U"21.2;").ToStdString(3, 3);
    ASSERT_TRUE(res == "21.2E+0") << res;
}

TEST_F(CalcTestReal, numbers9)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"."), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"..2"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1..2"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.2.3"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2.3.4"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U".1.2"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.2."), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestReal, numbers10)
{
    EXPECT_THROW(Real(53, "abc"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(Real(53, "1.2.3"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(Real(53, "x+y"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestReal, scientific1)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.23e-5;").ToStdString(3, 3);
    ASSERT_TRUE(res == "1.23E-5") << res;
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-1.23e-5;").ToStdString(3, 3);
    ASSERT_TRUE(res == "-1.23E-5") << res;
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-1.23e+5;").ToStdString(3, 3);
    ASSERT_TRUE(res == "-1.23E+5") << res;
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.23e+5;").ToStdString(3, 3);
    ASSERT_TRUE(res == "1.23E+5") << res;
}

TEST_F(CalcTestReal, arithmetic1)
{
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"10%75;");
    ASSERT_TRUE(res.ToString(3, 3) == U"7.5E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, arithmetic2)
{
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2(3+4);");
    ASSERT_TRUE(res.ToString(3, 3) == U"14.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, functions1)
{
    Real r = parser.Parse(LogicalId{0, 0, 1}, U"sin(1);");
    ASSERT_TRUE(r.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"0.841E+0;").ToStdString(3, 3)) << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 1}, U"ln(4);");
    ASSERT_TRUE(r.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"1.386E+0;").ToStdString(3, 3)) << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 1}, U"lg(4);");
    ASSERT_TRUE(r.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"0.602E+0;").ToStdString(3, 3)) << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 1}, U"log(2, 4);");
    ASSERT_TRUE(r.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"2.0E+0;").ToStdString(3, 3)) << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"log(2, 3);");
    ASSERT_TRUE(r.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"1.585E+0;").ToStdString(3, 3)) << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"log:2,3;");
    ASSERT_TRUE(r.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"1.585E+0;").ToStdString(3, 3)) << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"log:2,1+3;");
    ASSERT_TRUE(r.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"3.E+0;").ToStdString(3, 3)) << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"log:2,(1+3);");
    ASSERT_TRUE(r.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"2.E+0;").ToStdString(3, 3)) << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, functions2)
{
    Real res = parser.Parse(LogicalId{0, 0, 1}, U"rad(pi/2);", AngleMeasure::Radian, AngleMeasure::Degree);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Degree && res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"90;").ToStdString(3, 3)) << 
        res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"deg(180);", AngleMeasure::Degree, AngleMeasure::Radian);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Radian && res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"pi;").ToStdString(3, 3)) << 
        res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"grad(1);", AngleMeasure::Grad, AngleMeasure::Degree);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Degree && res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"0.9;").ToStdString(3, 3)) << 
        res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, functions3)
{
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1!;");
    ASSERT_TRUE(res.ToString(3, 3) == U"1.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"5.!;");
    ASSERT_TRUE(res.ToString(3, 3) == U"120.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(5+2)!;");
    ASSERT_TRUE(res.ToString(3, 3) == U"5.04E+3") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, functions4)
{
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"root(4,2);");
    ASSERT_TRUE(res.ToString(3, 3) == U"2.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, functions5)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"exp(1);", 5);
    ASSERT_TRUE(r.ToStdString(3, 5) == "2.71828E+0") << r.ToStdString(3, 5);
}

TEST_F(CalcTestReal, functions6)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"2exp(1);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "5.437E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, functions7)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"3pow(2,3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "24.E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, functions8)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"a=2;");
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"a!;");
    ASSERT_TRUE(res.ToString(3, 3) == U"2.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, functions9)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"abs(-2.3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "2.3E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"abs(2.3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "2.3E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, functions10)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"integer(-2.3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "-2.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"integer(-2.7);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "-2.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"integer(2.3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "2.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"integer(2.6);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "2.E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, functions11)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"fract(-2.3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "-0.3E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"fract(2.3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "0.3E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, functions12)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"ceil(-2.6);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "-2.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"ceil(-2.3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "-2.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"ceil(2.3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "3.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"ceil(2.5);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "3.E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, functions13)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"floor(-2.6);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "-3.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"floor(-2.3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "-3.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"floor(2.3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "2.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"floor(2.5);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "2.E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, functions14)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"round(-2.3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "-2.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"round(-2.5);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "-3.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"round(2.3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "2.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"round(2.5);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "3.E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, functions15)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"trunc(-2.3);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "-2.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"trunc(-2.5);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "-2.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"trunc(2.5);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "2.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"trunc(2.1);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "2.E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, functions16)
{
    try
    {
        parser.Parse(LogicalId{0, 0, 1}, U"pow(0,0);");
    }
    catch (yutovo_calculator::MathException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 1}) && ex.ex_id == ParserExceptionCode::Overflow && ex.pos == 0) << ex.ex_id;
        return;
    }
    ASSERT_FALSE(true);
}

TEST_F(CalcTestReal, functions17)
{
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(0, 2);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "0.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, functions18)
{
    try
    {
        parser.Parse(LogicalId{0, 0, 1}, U"pow(0,-2);");
    }
    catch (yutovo_calculator::MathException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 1}) && ex.ex_id == ParserExceptionCode::Overflow && ex.pos == 0) << ex.ex_id;
        return;
    }
    ASSERT_FALSE(true);
}

TEST_F(CalcTestReal, functions19)
{
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ln(5.2940000000000005e-23);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "-51.293E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ln(0.00069399999999999996);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "-7.273E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, user_functions1)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=5;");
    parser.Parse(LogicalId{0, 0, 2}, U"f(x)=x+5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 3}, U"f(2);") == parser.Parse(LogicalId{0, 0, 3}, U"7;")) << 
        parser.Parse(LogicalId{0, 0, 3}, U"f(2);").ToStdString(3, 3);
}

TEST_F(CalcTestReal, user_functions2)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"f(2);") == parser.Parse(LogicalId{0, 0, 2}, U"2;")) << 
        parser.Parse(LogicalId{0, 0, 2}, U"f(2);").ToStdString(3, 3);
    parser.Parse(LogicalId{0, 0, 3}, U"f(x)=x+7;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"f(2);") == parser.Parse(LogicalId{0, 0, 4}, U"9;")) << 
        parser.Parse(LogicalId{0, 0, 4}, U"f(2);").ToStdString(3, 3);
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"f(3);") == parser.Parse(LogicalId{0, 0, 2}, U"3;")) << 
        parser.Parse(LogicalId{0, 0, 2}, U"f(3);").ToStdString(3, 3);
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"f(3);") == parser.Parse(LogicalId{0, 0, 4}, U"10;")) << 
        parser.Parse(LogicalId{0, 0, 4}, U"f(10);").ToStdString(3, 3);
}

TEST_F(CalcTestReal, user_functions3)
{
    std::vector<std::u32string> dependencies;
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x*2;");
    parser.Parse(LogicalId{0, 0, 2}, U"f(5);", &dependencies);
    ASSERT_TRUE(std::find(dependencies.begin(), dependencies.end(), U"f") != dependencies.end());
}

TEST_F(CalcTestReal, user_functions4)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x;");
    Real res = parser.Parse(LogicalId{0, 0, 2}, U"f(5);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "5.E+0") << res.ToStdString(3, 3);

    parser.Parse(LogicalId{0, 0, 3}, U"f(x,y)=x+y;");
    res = parser.Parse(LogicalId{0, 0, 4}, U"f(5,4);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "9.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, user_functions5)
{
    parser.Parse(LogicalId{0, 0, 5}, U"p(a)=a+1;");
    parser.Parse(LogicalId{0, 0, 1}, U"p(a)=a+5;");
    Real res = parser.Parse(LogicalId{0, 0, 6}, U"p(5);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "6.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 2}, U"p(5);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "10.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, user_functions6)
{
    parser.Parse(LogicalId{0, 0, 8}, U"p(a)=a+15;");
    Real res = parser.Parse(LogicalId{0, 0, 9}, U"p(5);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "20.E+0") << res.ToStdString(3, 3);
    parser.Parse(LogicalId{0, 0, 5}, U"p(a)=a+1;");
    parser.Parse(LogicalId{0, 0, 1}, U"p(a)=a+5;");
    res = parser.Parse(LogicalId{0, 0, 2}, U"p(5);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "10.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 6}, U"p(5);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "6.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 2}, U"p(5);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "10.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 9}, U"p(5);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "20.E+0") << res.ToStdString(3, 3);
    parser.Parse(LogicalId{0, 0, 12}, U"p(a)=a+25;");
    res = parser.Parse(LogicalId{0, 0, 9}, U"p(5);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "20.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, user_functions7)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x,y)=pow(x,y);");
    Real res = parser.Parse(LogicalId{0, 0, 2}, U"f(5,2);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "25.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, user_functions8)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x,y)=(x)/(y);");
    Real res = parser.Parse(LogicalId{0, 0, 2}, U"f(8,2);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "4.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, user_functions9)
{
    parser.Parse(LogicalId{0, 0, 1}, U"y=(sin((pi)/(2)))/(3.4);");
    parser.Parse(LogicalId{0, 0, 2}, U"r=sqrt(y);");
    Real res = parser.Parse(LogicalId{0, 0, 3}, U"r;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "0.542E+0") << res.ToStdString(3, 3);
    ASSERT_TRUE(res.GetBitPrecision() < 1000);
    parser.Parse(LogicalId{0, 0, 4}, U"f(t)=t*cos(r*2);");
    res = parser.Parse(LogicalId{0, 0, 5}, U"f(3.4);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.589E+0") << res.ToStdString(3, 3);
    ASSERT_TRUE(res.GetBitPrecision() < 2000) << res.GetBitPrecision();
}

TEST_F(CalcTestReal, user_functions10)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=1/x;");
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 2}, U"f(0);"), yutovo_calculator::MathException);
    Real res = parser.Parse(LogicalId{0, 0, 3}, U"f(2);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "0.5E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, str1)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"в;"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestReal, str2)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"d33;"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"33t;"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestReal, variables1)
{
    std::vector<std::u32string> dependencies;
    parser.Parse(LogicalId{0, 0, 1}, U"v=555;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"v;") == parser.Parse(LogicalId{0, 0, 2}, U"555;")) << 
        parser.Parse(LogicalId{0, 0, 2}, U"v;").ToStdString(3, 3);
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 1}, U"v=55.5;") == parser.Parse(LogicalId{0, 0, 1}, U"v=55.5;")) << 
        parser.Parse(LogicalId{0, 0, 1}, U"v=55.5;").ToStdString(3, 3);
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"v=55.5;v;") == parser.Parse(LogicalId{0, 0, 2}, U"55.5;")) << 
        parser.Parse(LogicalId{0, 0, 2}, U"v=55.5;v;").ToStdString(3, 3);
    parser.Parse(LogicalId{0, 0, 3}, U"vp=55.5;", &dependencies);
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 3, 0}, U"vp+5;", &dependencies) == parser.Parse(LogicalId{0, 0, 3, 0}, U"60.5;")) << 
        parser.Parse(LogicalId{0, 0, 3, 0}, U"vp+5;").ToStdString(3, 3);
    ASSERT_TRUE(std::find(dependencies.begin(), dependencies.end(), U"vp") != dependencies.end());
}

TEST_F(CalcTestReal, variables2)
{
    parser.Parse(LogicalId{0, 0, 1}, U"п=555;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"п;") == parser.Parse(LogicalId{0, 0, 1}, U"555;")) << 
        parser.Parse(LogicalId{0, 0, 2}, U"п;").ToStdString(3, 3);
    parser.Parse(LogicalId{0, 0, 3}, U"п=55.5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 3, 3}, U"п;") == parser.Parse(LogicalId{0, 0, 3, 3}, U"55.5;")) << 
        parser.Parse(LogicalId{0, 0, 3, 3}, U"п;").ToStdString(3, 3);
    parser.Parse(LogicalId{0, 0, 3, 3, 0}, U"пр=55.5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 3, 3, 1}, U"пр+5;") == parser.Parse(LogicalId{0, 0, 1}, U"60.5;")) << 
        parser.Parse(LogicalId{0, 0, 3, 3, 1}, U"пр+5;").ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables3)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a=5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"a;") == parser.Parse(LogicalId{0, 0, 2}, U"5;")) << parser.Parse(LogicalId{0, 0, 2}, U"a;").ToStdString(3, 3);
    parser.Parse(LogicalId{0, 0, 3}, U"a=7;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"a;") == parser.Parse(LogicalId{0, 0, 4}, U"7;")) << parser.Parse(LogicalId{0, 0, 4}, U"a;").ToStdString(3, 3);
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"a+1;") == parser.Parse(LogicalId{0, 0, 2}, U"6;")) << parser.Parse(LogicalId{0, 0, 2}, U"a+1;").ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables4)
{
    parser.Parse(LogicalId{0, 0, 2}, U"a=77;");
    parser.Parse(LogicalId{0, 0, 1}, U"a=5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 3}, U"a;") == parser.Parse(LogicalId{0, 0, 3}, U"77;")) << parser.Parse(LogicalId{0, 0, 3}, U"a;").ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables5)
{
    parser.Parse(LogicalId{0, 0, 1}, U"d=4;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"d+5;") == parser.Parse(LogicalId{0, 0, 2}, U"9;")) << parser.Parse(LogicalId{0, 0, 2}, U"d+5;").ToStdString(3, 3);
    parser.Parse(LogicalId{0, 0, 3}, U"d=45+d;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"d+5;") == parser.Parse(LogicalId{0, 0, 4}, U"54;")) << parser.Parse(LogicalId{0, 0, 4}, U"d+5;").ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables6)
{
    parser.Parse(LogicalId{0, 0, 1}, U"d=4;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"5d;").ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 2}, U"20;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 0, 2}, U"5d;").ToStdString(3, 3);
    parser.Parse(LogicalId{0, 0, 1}, U"d=5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"5.1d;").ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 2}, U"25.5;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 0, 2}, U"5.1d;").ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables7)
{
    parser.Parse(LogicalId{0, 0, 1}, U"d=4;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"d*5;").ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 2}, U"20;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 0, 2}, U"d+5;").ToStdString(3, 3);
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"5.1*d;").ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 2}, U"20.4;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 0, 2}, U"5.1*d;").ToStdString(3, 3);
    parser.Parse(LogicalId{0, 0, 1}, U"d=5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"5.1*d;").ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 2}, U"25.5;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 0, 2}, U"5.1*d;").ToStdString(3, 3);
    parser.Parse(LogicalId{0, 0, 3}, U"d=4.1*d;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"d+5;").ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 4}, U"25.5;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 0, 4}, U"d+5;").ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables8)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a2=2.3;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"2a2;").ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 2}, U"4.6;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 0, 2}, U"2a2;").ToStdString(3, 3);
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"2.3a2;").ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 2}, U"5.29;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 0, 2}, U"2.3a2;").ToStdString(3, 3);
    parser.Parse(LogicalId{0, 0, 3}, U"a2_=3.3;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 1, 4}, U"2a2+1.2a2_;").ToStdString(3, 3) == parser.Parse(LogicalId{0, 1, 4}, U"8.56;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 1, 4}, U"2a2+1.2a2_;").ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables9)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a2=2.3;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"2*a2;").ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 2}, U"4.6;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 0, 2}, U"2*a2;").ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables10)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a2=2.3;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"2a2;").ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 2}, U"4.6;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 0, 2}, U"2a2;").ToStdString(3, 3);
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"2.3a2;").ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 2}, U"5.29;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 0, 2}, U"2.3a2;").ToStdString(3, 3);
    parser.Parse(LogicalId{0, 0, 3}, U"a22=3.3;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 1, 4}, U"2a2+1.2a22;").ToStdString(3, 3) == parser.Parse(LogicalId{0, 1, 4}, U"8.56;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 1, 4}, U"2a2+1.2a22;").ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables11)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a=5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"(3)/(4)a;").ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 2}, U"3.75;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 0, 2}, U"(3)/(4)a;").ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables12)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a{1}=5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"a{1};").ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 2}, U"5;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 0, 2}, U"a{1};").ToStdString(3, 3);
    parser.Parse(LogicalId{0, 0, 3}, U"a=7;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"a{1}+a;").ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 4}, U"12;").ToStdString(3, 3)) << 
        parser.Parse(LogicalId{0, 0, 4}, U"a{1}+a;").ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables13)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"pi;");
    ASSERT_TRUE(r.ToStdString(3, 3) == "3.142E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"pi;", 5);
    ASSERT_TRUE(r.ToStdString(3, 5) == "3.14159E+0") << r.ToStdString(3, 5);
    r = parser.Parse(LogicalId{0, 0, 1}, U"π;", 5);
    ASSERT_TRUE(r.ToStdString(3, 5) == "3.14159E+0") << r.ToStdString(3, 5);
}

TEST_F(CalcTestReal, variables14)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"e;");
    ASSERT_TRUE(r.ToStdString(3, 3) == "2.718E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"e;", 5);
    ASSERT_TRUE(r.ToStdString(3, 5) == "2.71828E+0") << r.ToStdString(3, 5);
}

TEST_F(CalcTestReal, variables15)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"G;");
    ASSERT_TRUE(r.ToStdString(3, 3) == "6.674E-11((m^3)/(kg*s^2))") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables16)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a=5.5;");
    auto r = parser.Parse(LogicalId{0, 0, 2}, U"a;", 5);
    ASSERT_TRUE(r.ToStdString(3, 3) == "5.5E+0") << r.ToStdString(3, 3);

    parser.SetLocale(Language::Russian);
    r = parser.Parse(LogicalId{0, 0, 2}, U"a;", 5);
    ASSERT_TRUE(r.ToStdString(3, 3) == "5.5E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables17)
{
    parser.SetLocale(Language::Russian);
    parser.Parse(LogicalId{0, 0, 1}, U"α=1;");
    parser.Parse(LogicalId{0, 0, 2}, U"a=1см;");
    parser.Parse(LogicalId{0, 0, 3}, U"b=2см;");
    parser.Parse(LogicalId{0, 0, 4}, U"S=a*b*sin(α);");
    auto r = parser.GetSuitableUnit(LogicalId{0, 0, 5}, parser.Parse(LogicalId{0, 0, 5}, U"S;"));
    ASSERT_TRUE(r.ToStdString(3, 3) == u8"1.683E+0(см^2)") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables18)
{
    parser.SetLocale(Language::Russian);
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"G;");
    ASSERT_TRUE(r.ToStdString(3, 3) == u8"6.674E-11((м^3)/(кг*с^2))") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables19)
{
    parser.SetLocale(Language::Russian);
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"g_a;");
    ASSERT_TRUE(r.ToStdString(3, 3) == u8"9.807E+0((м)/(с^2))") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables20)
{
    parser.SetLocale(Language::Russian);
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"A=5;");
    r = parser.Parse(LogicalId{0, 0, 2}, U"B=45;");
    r = parser.Parse(LogicalId{0, 0, 3}, U"A%B;");
    ASSERT_TRUE(r.ToStdString(3, 3) == "2.25E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables21)
{
    try
    {
        parser.Parse(LogicalId{0, 0, 1}, U"sin;");
        ASSERT_FALSE(true);
    }
    catch (yutovo_calculator::SyntaxException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 1}) && ex.ex_id == ParserExceptionCode::UnknownIdentifier && ex.pos == 0) << LogicalIdToString(ex.id);
    }

    parser.Parse(LogicalId{0, 0, 1}, U"sin=2;");
    Real res = parser.Parse(LogicalId{0, 0, 2}, U"sin;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "2.E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 3}, U"sin(2);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "0.909E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables22)
{
    parser.Parse(LogicalId{0, 0, 5}, U"b=4;");
    parser.Parse(LogicalId{0, 0, 3}, U"b=3;");
    parser.Parse(LogicalId{0, 0, 1}, U"b=2;");
    Real res = parser.Parse(LogicalId{0, 0, 6}, U"b;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "4.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 4}, U"b;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "3.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 2}, U"b;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "2.E+0") << res.ToStdString(3, 3);

    parser.Parse(LogicalId{0, 0, 1}, U"b=5;");
    res = parser.Parse(LogicalId{0, 0, 2}, U"b;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "5.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 4}, U"b;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "3.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 6}, U"b;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "4.E+0") << res.ToStdString(3, 3);

    parser.Parse(LogicalId{0, 0, 5}, U"b=6;");
    res = parser.Parse(LogicalId{0, 0, 6}, U"b;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "6.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 2}, U"b;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "5.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 4}, U"b;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "3.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, variables23)
{
    std::vector<std::u32string> dependencies;
    parser.Parse(LogicalId{0, 0, 1}, U"v{12}=555;");
    auto res = parser.Parse(LogicalId{0, 0, 2}, U"v{12};", &dependencies).ToStdString(3, 3);
    ASSERT_TRUE(res == "555.E+0") << res;
    ASSERT_TRUE(std::find(dependencies.begin(), dependencies.end(), U"v{12}") != dependencies.end());
}

TEST_F(CalcTestReal, variables24)
{
    yutovo_calculator::ParserContext parser_context;
    parser.Parse(LogicalId{0, 0, 1}, U"v=555;", &parser_context);
    ASSERT_TRUE(parser_context.no_result == true);
    parser.Parse(LogicalId{0, 0, 2}, U"v;", &parser_context);
    ASSERT_TRUE(parser_context.no_result == false);
}

TEST_F(CalcTestReal, variables25)
{
    parser.SetLocale(Language::Russian);
    yutovo_calculator::ParserContext parser_context;
    auto res = parser.Parse(LogicalId{0, 0, 1}, U"k=1м;", &parser_context);
    ASSERT_TRUE(parser_context.no_result == true);
    res = parser.Parse(LogicalId{0, 0, 2}, U"k;", &parser_context);
    ASSERT_TRUE(res == "1.E+0(м)") << res;
    ASSERT_TRUE(parser_context.no_result == false);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"k=1мk;"), yutovo_calculator::SyntaxException);
    res = parser.Parse(LogicalId{0, 0, 1}, U"k=1м;", &parser_context);
    ASSERT_TRUE(parser_context.no_result == true);
    res = parser.Parse(LogicalId{0, 0, 1}, U"k=1м;", &parser_context);
    ASSERT_TRUE(parser_context.no_result == true);
    res = parser.Parse(LogicalId{0, 0, 2}, U"k;", &parser_context);
    ASSERT_TRUE(res == "1.E+0(м)") << res;
    ASSERT_TRUE(parser_context.no_result == false);
}

TEST_F(CalcTestReal, variables26)
{
    parser.SetLocale(Language::Russian);
    yutovo_calculator::ParserContext parser_context;
    auto res = parser.Parse(LogicalId{0, 0, 1}, U"k{e}=1м;", &parser_context);
    ASSERT_TRUE(parser_context.no_result == true);
    res = parser.Parse(LogicalId{0, 0, 2}, U"k{e};", &parser_context);
    ASSERT_TRUE(res == "1.E+0(м)") << res;
    ASSERT_TRUE(parser_context.no_result == false);

    parser.RemoveIdentifier(LogicalId{0, 0, 1}, U"k{e}");
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 2}, U"k{e};"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestReal, symbols1)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"•;"), yutovo_calculator::SyntaxException) << parser.Parse(LogicalId{0, 0, 1}, U"•;").ToStdString(3, 3);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"‣;"), yutovo_calculator::SyntaxException) << parser.Parse(LogicalId{0, 0, 1}, U"‣;").ToStdString(3, 3);
}

TEST_F(CalcTestReal, errors1)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"123"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"123+sin(2)"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"123+3;45"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestReal, errors2)
{
    try
    {
        parser.Parse(LogicalId{0, 0, 1}, U"d=4+f;");
        parser.Parse(LogicalId{0, 0, 2}, U"d+5;");
    }
    catch (yutovo_calculator::SyntaxException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 1}) && ex.ex_id == ParserExceptionCode::UnknownIdentifier && ex.pos == 4) << LogicalIdToString(ex.id);
        return;
    }
    ASSERT_FALSE(true);
}

TEST_F(CalcTestReal, errors3)
{
    try
    {
        parser.Parse(LogicalId{0, 0, 1}, U"5+d;");
    }
    catch (yutovo_calculator::SyntaxException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 1}) && ex.ex_id == ParserExceptionCode::UnknownIdentifier && ex.pos == 2) << LogicalIdToString(ex.id);
        return;
    }
    ASSERT_FALSE(true);
}

TEST_F(CalcTestReal, errors4)
{
    try
    {
        parser.Parse(LogicalId{0, 0, 1}, U"d=5+d;");
        parser.Parse(LogicalId{0, 0, 2}, U"33455-d;");
    }
    catch (yutovo_calculator::SyntaxException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 1}) && ex.ex_id == ParserExceptionCode::UnknownIdentifier && ex.pos == 4) << LogicalIdToString(ex.id);
        return;
    }
    ASSERT_FALSE(true);
}

TEST_F(CalcTestReal, errors5)
{
    try
    {
        parser.Parse(LogicalId{0, 0, 1}, U"sqrt();");
    }
    catch (yutovo_calculator::SyntaxException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 1}) && ex.ex_id == ParserExceptionCode::WrongArgumentsCount && ex.pos == 0 && ex.size == 6) << ex.ex_id;
        return;
    }
    ASSERT_FALSE(true);
}

TEST_F(CalcTestReal, errors6)
{
    try
    {
        parser.Parse(LogicalId{0, 0, 1}, U";");
    }
    catch (yutovo_calculator::SyntaxException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 1}) && ex.ex_id == ParserExceptionCode::ExpressionExpected && ex.pos == 0) << ex.ex_id;
        return;
    }
    ASSERT_FALSE(true);
}

TEST_F(CalcTestReal, errors7)
{
    try
    {
        parser.Parse(LogicalId{0, 0, 1}, U"2+d+5;");
    }
    catch (yutovo_calculator::SyntaxException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 1}) && ex.ex_id == ParserExceptionCode::UnknownIdentifier && ex.pos == 2 && ex.size == 1) << ex.ex_id;
        return;
    }
    ASSERT_FALSE(true);
}

TEST_F(CalcTestReal, errors8)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"(1)/(0);"), yutovo_calculator::MathException);
}

TEST_F(CalcTestReal, errors9)
{
    try
    {
        parser.Parse(LogicalId{0, 0, 1}, U"2**5;");
    }
    catch (yutovo_calculator::SyntaxException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 1}) && ex.ex_id == ParserExceptionCode::SyntaxError && ex.pos == 2 && ex.size == 0) << ex.ex_id;
        return;
    }
    ASSERT_FALSE(true);
}

TEST_F(CalcTestReal, trigonometric1)
{
    Real res = parser.Parse(LogicalId{0, 0, 1}, U"sin(0);");
    ASSERT_TRUE(res.angle_measure == AngleMeasure::None && res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"0;").ToStdString(3, 3)) << 
        res.ToStdString(3, 3) << "\n" << (int)res.angle_measure;
    
    res = parser.Parse(LogicalId{0, 0, 1}, U"arcsin(0);");
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Radian && res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"0;").ToStdString(3, 3)) << 
        res.ToStdString(3, 3) << "\n" << (int)res.angle_measure;
}

TEST_F(CalcTestReal, trigonometric2)
{
    Real res = parser.Parse(LogicalId{0, 0, 1}, U"sin(90);", AngleMeasure::Degree, AngleMeasure::Degree);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::None && res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"1;").ToStdString(3, 3)) << 
        res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arcsin(1);", AngleMeasure::Degree, AngleMeasure::Degree);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Degree && res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"90;").ToStdString(3, 3)) << 
        res.ToStdString(3, 3) << "\n" << (int)res.angle_measure;
}

TEST_F(CalcTestReal, trigonometric3)
{
    Real res = parser.Parse(LogicalId{0, 0, 1}, U"sin(deg(90));", AngleMeasure::Radian, AngleMeasure::Radian);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::None && res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"1;").ToStdString(3, 3)) << 
        res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"deg(arcsin(1));", AngleMeasure::Radian, AngleMeasure::Degree);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Degree && res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"90;").ToStdString(3, 3)) << 
        res.ToStdString(3, 3) << "\n" << (int)res.angle_measure;
}

TEST_F(CalcTestReal, trigonometric4)
{
    Real res = parser.Parse(LogicalId{0, 0, 1}, U"sin(90°);", AngleMeasure::Radian, AngleMeasure::Radian);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::None && res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"1;").ToStdString(3, 3)) << 
        res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"sin((pi)/(2)rad);", AngleMeasure::Radian, AngleMeasure::Radian);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::None && res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"1;").ToStdString(3, 3)) << 
        res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, trigonometric5)
{
    Real res = parser.Parse(LogicalId{0, 0, 1}, U"90°;", AngleMeasure::Radian, AngleMeasure::Degree);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Degree && res.ToStdString(3, 3) == "90.E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"90deg;", AngleMeasure::Radian, AngleMeasure::Degree);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Degree && res.ToStdString(3, 3) == "90.E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"1rad;", AngleMeasure::Radian, AngleMeasure::Radian);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Radian && res.ToStdString(3, 3) == "1.E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"1grad;", AngleMeasure::Radian, AngleMeasure::Grad);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Grad && res.ToStdString(3, 3) == "1.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, trigonometric6)
{
    Real res = parser.Parse(LogicalId{0, 0, 1}, U"90°;", AngleMeasure::Radian, AngleMeasure::Radian);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Radian && res.ToStdString(3, 3) == "1.571E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"2rad;", AngleMeasure::Radian, AngleMeasure::Degree);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Degree && res.ToStdString(3, 3) == "114.592E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"2grad;", AngleMeasure::Radian, AngleMeasure::Grad);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Grad && res.ToStdString(3, 3) == "2.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, trigonometric7)
{
    Real res = parser.Parse(LogicalId{0, 0, 1}, U"90°;", AngleMeasure::Radian, AngleMeasure::Radian, 10);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Radian && res.ToStdString(3, 10) == "1.5707963268E+0") << res.ToStdString(3, 10);

    res = parser.Parse(LogicalId{0, 0, 1}, U"2rad;", AngleMeasure::Radian, AngleMeasure::Degree, 10);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Degree && res.ToStdString(3, 10) == "114.5915590262E+0") << res.ToStdString(3, 10);

    res = parser.Parse(LogicalId{0, 0, 1}, U"2grad;", AngleMeasure::Radian, AngleMeasure::Radian, 10);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Radian && res.ToStdString(3, 10) == "0.0314159257E+0") << res.ToStdString(3, 10);
}

TEST_F(CalcTestReal, trigonometric8)
{
    //180deg - rad(pi) should be 0 after converting to a common angle measure.
    Real res = parser.Parse(LogicalId{0, 0, 1}, U"180deg - rad(pi);", AngleMeasure::Radian, AngleMeasure::Degree);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Degree && res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"0;").ToStdString(3, 3)) <<
        res.ToStdString(3, 3) << "\n" << (int)res.angle_measure;

    //180deg / rad(pi) should be 1 after converting to a common angle measure.
    res = parser.Parse(LogicalId{0, 0, 1}, U"180deg / rad(pi);", AngleMeasure::Radian, AngleMeasure::Degree);
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Degree && res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"1;").ToStdString(3, 3)) <<
        res.ToStdString(3, 3) << "\n" << (int)res.angle_measure;
}

//Angle measures and arithmetic operations
TEST_F(CalcTestReal, trigonometric9)
{
    Real deg180 = parser.Parse(LogicalId{0, 0, 1}, U"180deg;", AngleMeasure::Radian, AngleMeasure::Degree);
    Real rad_pi = parser.Parse(LogicalId{0, 0, 1}, U"rad(pi);", AngleMeasure::Radian, AngleMeasure::Radian);
    Real one(deg180.GetBitPrecision(), 1);
    Real half(deg180.GetBitPrecision(), 0.5f);

    Real res = deg180 - 1;
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Radian) << (int)res.angle_measure;
    ASSERT_TRUE(res.ToStdString(3, 3) == (rad_pi - one).ToStdString(3, 3)) << res.ToStdString(3, 3);

    res = 1 - deg180;
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Radian) << (int)res.angle_measure;
    ASSERT_TRUE(res.ToStdString(3, 3) == (one - rad_pi).ToStdString(3, 3)) << res.ToStdString(3, 3);

    res = deg180 + 1;
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Radian) << (int)res.angle_measure;
    ASSERT_TRUE(res.ToStdString(3, 3) == (rad_pi + one).ToStdString(3, 3)) << res.ToStdString(3, 3);

    res = 1 + deg180;
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Radian) << (int)res.angle_measure;
    ASSERT_TRUE(res.ToStdString(3, 3) == (one + rad_pi).ToStdString(3, 3)) << res.ToStdString(3, 3);

    res = deg180 - 0.5f;
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Radian) << (int)res.angle_measure;
    ASSERT_TRUE(res.ToStdString(3, 3) == (rad_pi - half).ToStdString(3, 3)) << res.ToStdString(3, 3);

    res = 0.5f - deg180;
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Radian) << (int)res.angle_measure;
    ASSERT_TRUE(res.ToStdString(3, 3) == (half - rad_pi).ToStdString(3, 3)) << res.ToStdString(3, 3);

    res = deg180 + 0.5f;
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Radian) << (int)res.angle_measure;
    ASSERT_TRUE(res.ToStdString(3, 3) == (rad_pi + half).ToStdString(3, 3)) << res.ToStdString(3, 3);

    res = 0.5f + deg180;
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Radian) << (int)res.angle_measure;
    ASSERT_TRUE(res.ToStdString(3, 3) == (half + rad_pi).ToStdString(3, 3)) << res.ToStdString(3, 3);

    Real plain = parser.Parse(LogicalId{0, 0, 1}, U"5;", AngleMeasure::Radian, AngleMeasure::Radian);
    res = plain - 2;
    ASSERT_TRUE(res.angle_measure == AngleMeasure::None) << (int)res.angle_measure;
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"3;", AngleMeasure::Radian, AngleMeasure::Radian).ToStdString(3, 3)) << 
        res.ToStdString(3, 3);

    res = deg180 * 2;
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Degree) << (int)res.angle_measure;
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"360deg;", AngleMeasure::Radian, AngleMeasure::Degree).ToStdString(3, 3)) << 
        res.ToStdString(3, 3);

    res = deg180 / 2;
    ASSERT_TRUE(res.angle_measure == AngleMeasure::Degree) << (int)res.angle_measure;
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"90deg;", AngleMeasure::Radian, AngleMeasure::Degree).ToStdString(3, 3)) << 
        res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, units1)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1m;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(m)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2.3*m;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "2.3E+0(m)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2.3m*2m;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "4.6E+0(m^2)") << s;
}

TEST_F(CalcTestReal, units2)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2/m;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "2.E+0(1/(m))") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2*(1/m);")).ToStdString(3, 3);
    ASSERT_TRUE(s == "2.E+0(1/(m))") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2m*3*(1/m);")).ToStdString(3, 3);
    ASSERT_TRUE(s == "6.E+0") << s;
}

TEST_F(CalcTestReal, units3)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"3(2)/(m);")).ToStdString(3, 3);
    ASSERT_TRUE(s == "6.E+0(1/(m))") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"3(6m)/(3s);")).ToStdString(3, 3);
    ASSERT_TRUE(s == "6.E+0((m)/(s))") << s;
}

TEST_F(CalcTestReal, units4)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2.3m+3m;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "5.3E+0(m)") << s;
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"3m+4;"), yutovo_calculator::MathException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"4.5+3m+4;"), yutovo_calculator::MathException);
}

TEST_F(CalcTestReal, units5)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"5.3m-3m;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "2.3E+0(m)") << s;
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"3m-4;"), yutovo_calculator::MathException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"4.5-3m-4;"), yutovo_calculator::MathException);
}

TEST_F(CalcTestReal, units6)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(3m,2);")).ToStdString(3, 3);
    ASSERT_TRUE(s == "9.E+0(m^2)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(3m/s,2);")).ToStdString(3, 3);
    ASSERT_TRUE(s == "9.E+0(Gy)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"5m/pow(s,2);")).ToStdString(3, 3);
    ASSERT_TRUE(s == "5.E+0((m)/(s^2))") << s;
}

TEST_F(CalcTestReal, units7)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 0, 1}, U"km~1000m;");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"1km;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(km)") << s;
}

TEST_F(CalcTestReal, units8)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"0.2m;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "2.E+0(dm)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"0.02m;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "2.E+0(cm)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"0.002m;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "2.E+0(mm)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"200m;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "0.2E+0(km)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"2200m;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "2.2E+0(km)") << s;
}

TEST_F(CalcTestReal, units9)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"0.2m/s;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "2.E+0((dm)/(s))") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"5000m/s;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "5.E+0((km)/(s))") << s;
}

TEST_F(CalcTestReal, units10)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"1Hz;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(Hz)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"2000Hz;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "2.E+0(kHz)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"50*(1/s);")).ToStdString(3, 3);
    ASSERT_TRUE(s == "50.E+0(Hz)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"50(1)/(s);")).ToStdString(3, 3);
    ASSERT_TRUE(s == "50.E+0(Hz)") << s;
}

TEST_F(CalcTestReal, units11)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"10kg*10m/(2*pow(s,2));")).ToStdString(3, 3);
    ASSERT_TRUE(s == "50.E+0(N)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"1000kg*10m/(2*pow(s,2));")).ToStdString(3, 3);
    ASSERT_TRUE(s == "5.E+0(kN)") << s;
}

TEST_F(CalcTestReal, units12)
{
    std::string t = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"liniya{rus};")).ToStdString(3, 3);
    ASSERT_TRUE(t == "1.E+0(liniya){rus}") << t;
}

TEST_F(CalcTestReal, units13)
{
    std::string t = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"2*km;")).ToStdString(3, 3);
    ASSERT_TRUE(t == "2.E+0(km)") << t;
}

TEST_F(CalcTestReal, units14)
{
    std::string t = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"400sazhen{rus};")).ToStdString(3, 3);
    ASSERT_TRUE(t == "0.8E+0(versta){rus}") << t;
}

TEST_F(CalcTestReal, units15)
{
    std::string t = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"7fut{rus};")).ToStdString(3, 3);
    ASSERT_TRUE(t == "1.E+0(sazhen){rus}") << t;
}

TEST_F(CalcTestReal, units16)
{
    std::string t = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"1fut{rus}*1fut{rus};")).ToStdString(3, 3);
    ASSERT_TRUE(t == "1.E+0(fut^2){rus}") << t;
}

TEST_F(CalcTestReal, units17)
{
    std::string t = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"2m*2m;")).ToStdString(3, 3);
    ASSERT_TRUE(t == "4.E+0(m^2)") << t;
}

TEST_F(CalcTestReal, units18)
{
    std::string t = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"2km*2km;")).ToStdString(3, 3);
    ASSERT_TRUE(t == "4.E+0(km^2)") << t;
}

TEST_F(CalcTestReal, units19)
{
    auto val = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"2cm*3cm;");
    std::string t = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, val).ToStdString(3, 3);
    ASSERT_TRUE(t == "6.E+0(cm^2)") << t;
}

TEST_F(CalcTestReal, units20)
{
    auto val = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"700fut{rus};");
    std::string t = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, val).ToStdString(3, 3);
    ASSERT_TRUE(t == "0.2E+0(versta){rus}") << t;
}

TEST_F(CalcTestReal, units21)
{
    auto val = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"1m;");
    std::string t = parser.CastToUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, val, Unit(U"mm")).ToStdString(3, 3);
    ASSERT_TRUE(t == "1.E+3(mm)") << t;
}

TEST_F(CalcTestReal, units22)
{
    auto val = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"20*pow(mm, 2);");
    std::string t = parser.CastToUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, val, Unit(U"cm", 2)).ToStdString(3, 3);
    ASSERT_TRUE(t == "0.2E+0(cm^2)") << t;
}

TEST_F(CalcTestReal, units23)
{
    auto val = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"5*(m/min);");
    std::string t = parser.CastToUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, val, Unit(U"km", U"min")).ToStdString(3, 3);
    ASSERT_TRUE(t == "0.005E+0((km)/(min))") << t;
    t = parser.CastToUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, val, Unit(U"m", U"min")).ToStdString(3, 3);
    ASSERT_TRUE(t == "5.E+0((m)/(min))") << t;
    t = parser.CastToUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, val, Unit(U"m", U"s")).ToStdString(3, 3);
    ASSERT_TRUE(t == "0.0833E+0((m)/(s))") << t;
}

TEST_F(CalcTestReal, units24)
{
    LogicalId id{0, 0, 0, 0, 0, 0, 0, 2, 0};
    auto val = parser.Parse(id, U"5*(km/hour);");
    std::vector<Unit> cast_units;
    parser.GetCastUnits(id, val, cast_units);
    ASSERT_TRUE(FindUnit(cast_units, Unit(U"m", U"s")));
    ASSERT_TRUE(FindUnit(cast_units, Unit(U"km", U"hour")));
    std::string t = parser.GetSuitableUnit(id, val).ToStdString(3, 3);
    ASSERT_TRUE(t == "5.E+0((km)/(hour))") << t;
}

TEST_F(CalcTestReal, units25)
{
    auto val = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"0.001s;");
    std::string t = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, val).ToStdString(3, 3);
    ASSERT_TRUE(t == "1.E+0(ms)") << t;
}

TEST_F(CalcTestReal, units26)
{
    auto val = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"1/(1000m);");
    std::string t = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, val).ToStdString(3, 3);
    ASSERT_TRUE(t == "1.E+0(1/(km))") << t;
}

TEST_F(CalcTestReal, units27)
{
    LogicalId id{0, 0, 0, 0, 0, 0, 0, 2, 0};
    auto val = parser.Parse(id, U"2(m/s);");
    std::string s = val.ToStdString(3, 3);
    ASSERT_TRUE(s == "2.E+0((m)/(s))") << s;
    s = parser.GetSuitableUnit(id, val).ToStdString(3, 3);
    ASSERT_TRUE(s == "2.E+0((m)/(s))") << s;
    std::vector<Unit> cast_units;
    parser.GetCastUnits(id, val, cast_units);
    ASSERT_TRUE(FindUnit(cast_units, Unit(U"km", U"hour")));
}

TEST_F(CalcTestReal, units28)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"2*(m/s);").ToStdString(3, 3);
    ASSERT_TRUE(s == "2.E+0((m)/(s))") << s;
}

TEST_F(CalcTestReal, units29)
{
    parser.SetLocale(Language::Russian);
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"2*(м/с);").ToStdString(3, 3);
    ASSERT_TRUE(s == u8"2.E+0((м)/(с))") << s;
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"2*(m/s);"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestReal, units30)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"(1/2)m;").ToStdString(3, 3);
    ASSERT_TRUE(s == u8"0.5E+0(m)") << s;
}

TEST_F(CalcTestReal, units31)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"(2.4/2)m;").ToStdString(3, 3);
    ASSERT_TRUE(s == u8"1.2E+0(m)") << s;
}

TEST_F(CalcTestReal, units32)
{
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"5mm;");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == u8"5.E+0(mm)") << s;

    r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"50mm;");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == u8"5.E+0(cm)") << s;
}

TEST_F(CalcTestReal, units33)
{
    parser.SetLocale(Language::Russian);
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"500сажень{rus};");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == u8"1.E+0(верста){rus}") << s;
}

TEST_F(CalcTestReal, units34)
{
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"1rad;");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0") << s;

    r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"1°;");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == u8"1.E+0") << s;

    r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"1';");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0") << s;

    r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"1'';");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0") << s;
}

TEST_F(CalcTestReal, units35)
{
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"1°C;");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(K)") << s;

    r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"1K;");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(K)") << s;
}

TEST_F(CalcTestReal, units36)
{
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"dm~1mm;");
    r = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"dm;");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(mm)") << s;

    std::string t = parser.CastToUnit(LogicalId{0, 0, 0, 0, 2}, r, Unit(U"dm")).ToStdString(3, 3);
    ASSERT_TRUE(t == "1.E+0(dm)") << t;
}

TEST_F(CalcTestReal, units37)
{
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"dm~1.2mm;");
    r = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"dm;");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(dm)") << s;

    std::string t = parser.CastToUnit(LogicalId{0, 0, 0, 0, 2}, r, Unit(U"mm")).ToStdString(3, 3);
    ASSERT_TRUE(t == "1.2E+0(mm)") << t;
}

TEST_F(CalcTestReal, units38)
{
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"5mm;");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "5.E+0(mm)") << s;

    parser.SetLocale(Language::Russian);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"5mm;"), yutovo_calculator::SyntaxException);
    r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"5мм;");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == u8"5.E+0(мм)") << s;
}

TEST_F(CalcTestReal, units39)
{
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"d_m`dmeter`~1.5mm;");
    r = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"d_m;");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(d_m)") << s;
}

TEST_F(CalcTestReal, units40)
{
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"5N;");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "5.E+0(N)") << s;
}

TEST_F(CalcTestReal, units41)
{
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"5V;");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "5.E+0(V)") << s;
}

TEST_F(CalcTestReal, units42)
{
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"pow(5m,2);");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "25.E+0(m^2)") << s;
}

TEST_F(CalcTestReal, units43)
{
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"sqrt(pow(5m,2));");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "5.E+0(m)") << s;

    r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"sqrt(pow(5m,2)/pow(3s,2));");
    s = parser.CastToUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r, Unit(U"m", U"s")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.667E+0((m)/(s))") << s;

    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"sqrt(5m);"), yutovo_calculator::MathException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"sqrt(pow(5m,3));"), yutovo_calculator::MathException);
}

TEST_F(CalcTestReal, units44)
{
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"root(pow(5m,2),2);");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "5.E+0(m)") << s;

    r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"root(pow(5m,3),3);");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "5.E+0(m)") << s;

    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"root(pow(5m,2),3);"), yutovo_calculator::MathException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"root(pow(5m,3),2);"), yutovo_calculator::MathException);
}

TEST_F(CalcTestReal, units45)
{
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"-(1m-2m);");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(m)") << s;

    r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"+(2m-1m);");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(m)") << s;
}

TEST_F(CalcTestReal, units46)
{
    auto val = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"5*(m/min);");
    Unit u;
    ASSERT_TRUE(u.FromString(U"km/hour"));
    std::string t = parser.CastToUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, val, u).ToStdString(3, 3);
    ASSERT_TRUE(t == "0.3E+0((km)/(hour))") << t;
}

TEST_F(CalcTestReal, units47)
{
    auto val = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"5*(N);");
    Unit u;
    ASSERT_TRUE(u.FromString(U"(kg*m)/(s^2)"));
    std::string t = parser.CastToUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, val, u).ToStdString(3, 3);
    ASSERT_TRUE(t == "5.E+0((kg*m)/(s^2))") << t;
}

TEST_F(CalcTestReal, units48)
{
    parser.SetLocale(Language::Russian);
    auto val = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"5*(м/с{SI});");
    Unit u;
    ASSERT_TRUE(u.FromString(U"(км)/(час)"));
    std::string t = parser.CastToUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, val, u).ToStdString(3, 3);
    ASSERT_TRUE(t == u8"18.E+0((км)/(час))") << t;
}

TEST_F(CalcTestReal, units49)
{
    parser.SetLocale(Language::Russian);
    auto val = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"5*(м{SI});");
    Unit u;
    ASSERT_TRUE(u.FromString(U"сотка{rus}"));
    std::string t = parser.CastToUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, val, u).ToStdString(3, 3);
    ASSERT_TRUE(t == u8"234.346E+0(сотка){rus}") << t;
}

TEST_F(CalcTestReal, units50)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"pow(2,3)m;", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "8.E+0(m)") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, units51)
{
    parser.SetLocale(Language::Russian);
    auto val = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"G;");
    Unit u;
    ASSERT_TRUE(u.FromString(U"((м^3)/(кг*с^2))"));
    std::string t = parser.CastToUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, val, u).ToStdString(3, 3);
    ASSERT_TRUE(t == u8"6.674E-11((м^3)/(кг*с^2))") << t;
}

TEST_F(CalcTestReal, units52)
{
    parser.SetLocale(Language::Russian);
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"((м/pow(с,2))*pow(м,2))/(pow(м,3)/(кг*pow(с,2)));");
    ASSERT_TRUE(r.ToStdString(3, 3) == u8"1.E+0(кг)") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, units53)
{
    parser.SetLocale(Language::Russian);
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"(м/с)/(м/с);");
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, units54)
{
    parser.SetLocale(Language::Russian);
    auto val = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"1Ом;");
    Unit u;
    ASSERT_TRUE(u.FromString(U"((Дж)/(нс*мкА^2))"));
    std::string t = parser.CastToUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, val, u).ToStdString(3, 3);
    ASSERT_TRUE(t == u8"1.E-21((Дж)/(нс*мкА^2))") << t;
    ASSERT_TRUE(u.FromString(U"((Дж)/(сутки*мА^2))"));
    t = parser.CastToUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, val, u).ToStdString(3, 3);
    ASSERT_TRUE(t == u8"0.0864E+0((Дж)/(сутки*мА^2))") << t;
}

TEST_F(CalcTestReal, units55)
{
    parser.SetLocale(Language::Russian);
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"кг*м*А/(pow(с,2));");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == u8"1.E+0(Н*А)") << s;
}

TEST_F(CalcTestReal, units56)
{
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"b=1kg;");
    r = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"h=1b;");
    r = parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"E=b*g_a*h;");
    r = parser.Parse(LogicalId{0, 0, 0, 0, 4}, U"E;");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 4}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "9.807E+0(N*kg)") << s;
}

TEST_F(CalcTestReal, units57)
{
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(kg,2)*pow(m,2)/pow(s,4);");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 4}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(N^2)") << s;
}

TEST_F(CalcTestReal, units58)
{
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1/(pow(kg,2)*pow(m,2)/pow(s,4));");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 4}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(1/(N^2))") << s;
}

TEST_F(CalcTestReal, units59)
{
    parser.SetLocale(Language::Russian);
    LogicalId id{0, 0, 0, 0, 0, 0, 0, 2, 0};
    auto r = parser.Parse(id, U"с;");
    std::vector<Unit> cast_units;
    parser.GetCastUnits(id, r, cast_units);
    ASSERT_TRUE(std::adjacent_find(cast_units.begin(), cast_units.end()) == cast_units.end());
    std::string s = parser.GetSuitableUnit(id, r).ToStdString(3, 3);
    ASSERT_TRUE(s == u8"1.E+0(с)") << s;
}

TEST_F(CalcTestReal, units60)
{
    parser.SetLocale(Language::Russian);
    LogicalId id{0, 0, 0, 0, 0, 1};
    auto r = parser.Parse(id, U"1км;");
    std::string s = parser.GetSuitableUnit(id, r).ToStdString(3, 3);
    ASSERT_TRUE(s == u8"1.E+0(км)") << s;
    auto t = parser.CastToUnit(id, r, Unit(U"см")).ToStdString(3, 3);
    ASSERT_TRUE(t == u8"1.E+5(см)") << t;
    r = parser.Parse(id, U"1с;");
    s = parser.GetSuitableUnit(id, r).ToStdString(3, 3);
    ASSERT_TRUE(s == u8"1.E+0(с)") << s;
}

TEST_F(CalcTestReal, units61)
{
    parser.SetLocale(Language::Russian);
    LogicalId id{0, 0, 0, 0, 0, 1};
    auto r = parser.Parse(id, U"кг*pow(м,2)/(pow(с,2)*моль*К);");
    std::string s = parser.GetSuitableUnit(id, r).ToStdString(3, 3);
    ASSERT_TRUE(s == u8"1.E+0((Дж)/(моль*К))") << s;
}

TEST_F(CalcTestReal, units62)
{
    parser.SetLocale(Language::Russian);
    std::vector<std::u32string> dependencies;
    LogicalId id{0, 0, 1};
    parser.Parse(id, U"d_m~10м;");
    auto r = parser.Parse(LogicalId{0, 0, 2}, U"10м;", &dependencies);
    ASSERT_TRUE(r.ToStdString(3, 3) == u8"10.E+0(м)") << r.ToStdString(3, 3);
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 2}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(d_m)") << s;
    ASSERT_TRUE(std::find(dependencies.begin(), dependencies.end(), U"м") != dependencies.end());
}

TEST_F(CalcTestReal, units63)
{
    parser.SetLocale(Language::Russian);
    LogicalId id{0, 0, 1};
    parser.Parse(id, U"d_m~10м;");
    auto r = parser.Parse(LogicalId{0, 0, 2}, U"10м;");
    ASSERT_TRUE(r.ToStdString(3, 3) == u8"10.E+0(м)") << r.ToStdString(3, 3);
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 2}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(d_m)") << s;

    parser.RemoveIdentifier(id, U"d_m");
    r = parser.Parse(LogicalId{0, 0, 2}, U"10м;");
    ASSERT_TRUE(r.ToStdString(3, 3) == u8"10.E+0(м)") << r.ToStdString(3, 3);
    s = parser.GetSuitableUnit(LogicalId{0, 0, 2}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == u8"10.E+0(м)") << s;
}

TEST_F(CalcTestReal, units64)
{
    parser.SetLocale(Language::Russian);
    LogicalId id{0, 0, 0, 1};
    auto r = parser.Parse(id, U"кг*м/с;");
    std::vector<Unit> cast_units;
    parser.GetCastUnits(id, r, cast_units);
    Unit u(U"кг", U"с");
    u.system = U"rus";
    u.unit.push_back(std::make_pair(U"м", 1));
    ASSERT_FALSE(FindUnit(cast_units, u));
}

TEST_F(CalcTestReal, units65)
{
    parser.SetLocale(Language::Russian);
    LogicalId id{0, 0, 0, 1};
    auto r = parser.Parse(id, U"10Вт;");
    auto s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 2}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == u8"10.E+0(Вт)") << s;
}

TEST_F(CalcTestReal, units66)
{
    LogicalId id{0, 0, 0, 1};
    auto r = parser.Parse({0, 0, 0, 1}, U"x=50*(pow(m,3));");
    r = parser.Parse({0, 0, 0, 2}, U"cube_m~50*(pow(m,3));");
    r = parser.Parse({0, 0, 0, 3}, U"4*x;");
    auto s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 4}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == u8"4.E+0(cube_m)") << s;
}

TEST_F(CalcTestReal, units67)
{
    parser.SetLocale(Language::Russian);
    try
    {
        parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"pow(1.008,с);");
    }
    catch (yutovo_calculator::MathException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 0, 0, 2}) && ex.ex_id == ParserExceptionCode::UnitsAreIncompatible && ex.pos == 0) << LogicalIdToString(ex.id);
        return;
    }
    ASSERT_FALSE(true);
}

TEST_F(CalcTestReal, units68)
{
    parser.SetLocale(Language::Spanish);
    LogicalId id{0, 0, 0, 1};
    auto r = parser.Parse(id, U"10m;");
    auto s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 2}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == u8"10.E+0(m)") << s;
}

TEST_F(CalcTestReal, units69)
{
    parser.SetLocale(Language::Spanish);
    LogicalId id{0, 0, 0, 1};
    auto r = parser.Parse(id, U"2día;");
    auto s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 2}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == u8"2.E+0(día)") << s;
}

TEST_F(CalcTestReal, units70)
{
    parser.SetLocale(Language::English);
    LogicalId id{0, 0, 0, 1};
    auto r = parser.Parse(id, U"2inch{us};");
    auto s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 2}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "2.E+0(inch){us}") << s;
    r = parser.Parse(id, U"2in{us};");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 2}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "2.E+0(inch){us}") << s;
}

TEST_F(CalcTestReal, units71)
{
    parser.SetLocale(Language::Russian);
    LogicalId id{0, 0, 0, 1};
    auto r = parser.Parse(id, U"2дюйм{us};");
    auto s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 2}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "2.E+0(дюйм){us}") << s;
}

TEST_F(CalcTestReal, units72)
{
    parser.SetLocale(Language::Spanish);
    LogicalId id{0, 0, 0, 1};
    auto r = parser.Parse(id, U"2pulgada{us};");
    auto s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 2}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "2.E+0(pulgada){us}") << s;
    r = parser.Parse(id, U"2in{us};");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 2}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "2.E+0(pulgada){us}") << s;
}

TEST_F(CalcTestReal, units73)
{
    parser.SetLocale(Language::Russian);
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(1)/(1*Гц*1*мкФ);")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(МОм)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(1)/(Гц*мкФ);")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(МОм)") << s;
}

TEST_F(CalcTestReal, units74)
{
    parser.SetLocale(Language::English);
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0.789*m;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "0.789E+0(m)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0.871*m;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "0.871E+0(m)") << s;
}

TEST_F(CalcTestReal, units75)
{
    parser.SetLocale(Language::Spanish);
    parser.Parse({0, 0, 0, 0, 1}, U"L~pow(m,3);");
    parser.Parse({0, 0, 0, 0, 2}, U"r~2*s;");
    parser.Parse({0, 0, 0, 0, 3}, U"rp~((r)/(s));");
    parser.Parse({0, 0, 0, 0, 5, 0, 0}, U"Dt=root(L,3);");
    Real r = parser.Parse({0, 0, 0, 5, 0, 1}, U"Dt;");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 5, 0, 1}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(m)") << s;

    std::vector<Unit> cast_units;
    parser.ClearCastUnits();
    parser.GetCastUnits({0, 0, 0, 5, 0, 1}, r, cast_units);
    ASSERT_TRUE(FindUnit(cast_units, Unit(U"m")));

    Unit u;
    ASSERT_TRUE(u.FromString(U"arshin{rus}"));
    s = parser.CastToUnit(LogicalId{0, 0, 0, 5, 0, 1}, r, u).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.406E+0(arshin){rus}") << s;
}

TEST_F(CalcTestReal, units76)
{
    parser.SetLocale(Language::Russian);
    Real r = parser.Parse({0, 0, 0, 0, 1}, U"0.*Кл;");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "0.E+0(Кл)") << s;
}

TEST_F(CalcTestReal, units77)
{
    auto exports = std::make_shared<yutovo_calculator::Export>();

    Unit meter_unit(U"meter");
    Unit foot_unit(U"foot");
    Real meter_value(3, "3.28084");
    meter_value.unit = foot_unit;
    Real foot_value(3, "0.3048");
    foot_value.unit = meter_unit;

    exports->AddUnit<Real>(CustomUnit<Real>(LogicalId{0, 0, 0, 0, 1}, U"meter", U"SI", meter_value, false));
    exports->AddUnit<Real>(CustomUnit<Real>(LogicalId{0, 0, 0, 0, 2}, U"foot", U"SI", foot_value, false));

    yutovo_calculator::ParserContext parser_context;
    parser_context.Init(1000);
    parser_context.exports = exports;
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"5meter;", &parser_context);
    EXPECT_NO_THROW(parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 3}, r).ToStdString(3, 3));
}

TEST_F(CalcTestReal, units78)
{
    yutovo_calculator::Solver<yutovo_calculator::Real> solver(3, yutovo_calculator::AngleMeasure::Radian);

    yutovo_calculator::Unit meter_unit(U"meter");
    yutovo_calculator::Unit foot_unit(U"foot");
    yutovo_calculator::Real meter_value(3, "3.28084");
    meter_value.unit = foot_unit;
    yutovo_calculator::Real foot_value(3, "0.3048");
    foot_value.unit = meter_unit;

    solver.symbols->units.push_back(yutovo_calculator::CustomUnit<yutovo_calculator::Real>(LogicalId{0, 0, 0, 0, 1}, U"meter", U"SI", meter_value, false));
    solver.symbols->units.push_back(yutovo_calculator::CustomUnit<yutovo_calculator::Real>(LogicalId{0, 0, 0, 0, 2}, U"foot", U"SI", foot_value, false));

    yutovo_calculator::Real val(3, "5");
    val.unit = meter_unit;
    yutovo_calculator::ParserContext parser_context;
    parser_context.Init(1000);
    solver.SetParserContext(&parser_context);

    std::vector<yutovo_calculator::Unit> cast_units;
    EXPECT_NO_THROW(solver.GetCastUnits(LogicalId{0, 0, 0, 0, 3}, val, U"SI", cast_units));
}

TEST_F(CalcTestReal, compare1)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"(0<10);").ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"(10==10);").ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"(10<>10);").ToStdString(3, 3);
    ASSERT_TRUE(s == "0.E+0") << s;
}

TEST_F(CalcTestReal, compare2)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 0}, U"a=5;");
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(a<10);").ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(a>10);").ToStdString(3, 3);
    ASSERT_TRUE(s == "0.E+0") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(a==10);").ToStdString(3, 3);
    ASSERT_TRUE(s == "0.E+0") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(a<>10.);").ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(a<=10.);").ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0") << s;
}

TEST_F(CalcTestReal, sum1)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(i=0,(i<=10),i=i+1,t=0,t=t+1);").ToStdString(3, 3);
    ASSERT_TRUE(s == "11.E+0") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(i=1,(i<=10),i=i+1,t=0,t=t+i);").ToStdString(3, 3);
    ASSERT_TRUE(s == "55.E+0") << s;

    try
    {
        parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"i;");
    }
    catch (yutovo_calculator::SyntaxException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 0, 0, 2}) && ex.ex_id == ParserExceptionCode::UnknownIdentifier && ex.pos == 0) << LogicalIdToString(ex.id);
        return;
    }
    ASSERT_FALSE(true);
}

TEST_F(CalcTestReal, sum2)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(i=0,(i<=10),i=i+1,t1=0,t1=t1+loop(j=0,(j<=20),j=j+1,t2=0,t2=t2+i+j));").ToStdString(3, 3);
    ASSERT_TRUE(s == "3.465E+3") << s;
}

TEST_F(CalcTestReal, prod1)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(i=0,(i<=10),i=i+1,t=1,t=t*2);").ToStdString(3, 3);
    ASSERT_TRUE(s == "2.048E+3") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(i=1,(i<=10),i=i+1,t=1,t=t*i);").ToStdString(3, 3);
    ASSERT_TRUE(s == "3.629E+6") << s;
}

TEST_F(CalcTestReal, prod2)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"loop(i=1,(i<=10),i=i+1,i_=1,i_=i_*(i));").ToStdString(3, 3);
    ASSERT_TRUE(s == "3.629E+6") << s;
}

TEST_F(CalcTestReal, max_time1)
{
    yutovo_calculator::ParserContext parser_context;
#ifdef NDEBUG
    parser_context.Init(100);
#elif defined(_WIN32)
    parser_context.Init(200);
#else
    parser_context.Init(1000);
#endif
    parser.SetMaxCastUnitSize(3);
    EXPECT_THROW(parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"10kg*10m/(2*pow(s,2));", 
        &parser_context)).ToStdString(3, 3), yutovo_calculator::TimeExceedException);
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(1+2);");
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"3;").ToStdString(3, 3));

    parser_context.Init(20000);
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"10kg*10m/(2*pow(s,2));", 
        &parser_context)).ToStdString(3, 3);
    ASSERT_TRUE(s == "50.E+0(N)") << s;
}

TEST_F(CalcTestReal, max_time2)
{
    yutovo_calculator::ParserContext parser_context;
#ifdef NDEBUG
    parser_context.Init(2000);
#else
    parser_context.Init(5000);
#endif
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(234)/((3)/((4)/((6)/((7)/((6)/((7)/((2)/(4))))))));", &parser_context),
        yutovo_calculator::TimeExceedException);
    parser_context.Init(0);
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"123;", &parser_context).ToStdString(3, 3);
    ASSERT_TRUE(s == "123.E+0") << s;
}

TEST_F(CalcTestReal, list_identifiers1)
{
    std::vector<std::pair<std::u32string, std::u32string>> variables;
    parser.ListUserVariables(variables);
    ASSERT_TRUE(std::find_if(variables.begin(), variables.end(), 
        [](auto& p)
        {
            return p.first == U"c" && p.second == U"speed of light";
        }) != variables.end());
    
    variables.clear();
    parser.SetLocale(Language::Russian);
    parser.ListUserVariables(variables);
    ASSERT_TRUE(std::find_if(variables.begin(), variables.end(), 
        [](auto& p)
        {
            return p.first == U"ск_св" && p.second == U"скорость света";
        }) != variables.end());
}

TEST_F(CalcTestReal, list_identifiers2)
{
    std::vector<CustomUnit<Real>> units;
    parser.ListUserUnits(units);
    ASSERT_TRUE(std::find_if(units.begin(), units.end(), 
        [](auto& p)
        {
            return p.name == U"km" && p.description == U"kilometre";
        }) != units.end());
    
    units.clear();
    parser.SetLocale(Language::Russian);
    parser.ListUserUnits(units);
    ASSERT_TRUE(std::find_if(units.begin(), units.end(), 
        [](auto& p)
        {
            return p.name == U"км" && p.description == U"километр";
        }) != units.end());
}

TEST_F(CalcTestReal, list_identifiers3)
{
    std::vector<std::pair<std::u32string, std::u32string>> units;
    parser.ListBuiltinUnits(units);
    ASSERT_TRUE(std::find_if(units.begin(), units.end(), 
        [](auto& p)
        {
            return p.first == U"m" && p.second == U"metre";
        }) != units.end());
    
    units.clear();
    parser.SetLocale(Language::Russian);
    parser.ListBuiltinUnits(units);
    ASSERT_TRUE(std::find_if(units.begin(), units.end(), 
        [](auto& p)
        {
            return p.first == U"м" && p.second == U"метр";
        }) != units.end());
}

TEST_F(CalcTestReal, money1)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"1₽;", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(₽)") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"0.1₽;", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "0.1E+0(₽)") << r.ToStdString(3, 3);
    r = parser.GetSuitableUnit(LogicalId{0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 2}, U"1kop;", 3));
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(kop)") << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 1}, U"1$;", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0($)") << r.ToStdString(3, 3);
    r = parser.GetSuitableUnit(LogicalId{0, 0, 1}, parser.Parse(LogicalId{0, 0, 1}, U"1cent;", 3));
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(¢)") << r.ToStdString(3, 3);
    r = parser.GetSuitableUnit(LogicalId{0, 0, 1}, parser.Parse(LogicalId{0, 0, 1}, U"33¢;", 3));
    ASSERT_TRUE(r.ToStdString(3, 3) == "33.E+0(¢)") << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 1}, U"1€;", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(€)") << r.ToStdString(3, 3);
    r = parser.GetSuitableUnit(LogicalId{0, 0, 2}, parser.Parse(LogicalId{0, 0, 2}, U"1euro;", 3));
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(€)") << r.ToStdString(3, 3);
    r = parser.GetSuitableUnit(LogicalId{0, 0, 3}, parser.Parse(LogicalId{0, 0, 3}, U"1eurocent;", 3));
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(ct)") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, money2)
{
    parser.SetLocale(Language::Russian);

    auto r = parser.Parse(LogicalId{0, 0, 1}, U"1₽;", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(₽)") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"0.1₽;", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "0.1E+0(₽)") << r.ToStdString(3, 3);
    r = parser.GetSuitableUnit(LogicalId{0, 0, 0, 2}, parser.Parse(LogicalId{0, 0, 0, 2}, U"1коп;", 3));
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(коп)") << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 1}, U"1$;", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0($)") << r.ToStdString(3, 3);
    r = parser.GetSuitableUnit(LogicalId{0, 0, 1}, parser.Parse(LogicalId{0, 0, 1}, U"1цент;", 3));
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(цент)") << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 1}, U"1€;", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(€)") << r.ToStdString(3, 3);
    r = parser.GetSuitableUnit(LogicalId{0, 0, 2}, parser.Parse(LogicalId{0, 0, 2}, U"1евро;", 3));
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(€)") << r.ToStdString(3, 3);
    r = parser.GetSuitableUnit(LogicalId{0, 0, 3}, parser.Parse(LogicalId{0, 0, 3}, U"1евроцент;", 3));
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(евроцент)") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, money3)
{
    parser.SetLocale(Language::Russian);

    auto r = parser.Parse(LogicalId{0, 0, 1}, U"1¥;", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(¥)") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"1юань;", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(¥)") << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 1}, U"1₹;", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(₹)") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"1рупия;", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(₹)") << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 1}, U"1R$;", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(R$)") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"1реал;", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(R$)") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, list1)
{
    parser.Parse(LogicalId{0, 0, 1}, U"materials=\"cuprum\",\"ferrum\";", 3);
    parser.Parse(LogicalId{0, 0, 2}, U"ro{cuprum}=1.68E-8;", 3);
    parser.Parse(LogicalId{0, 0, 3}, U"ro{ferrum}=9.71E-8;", 3);
    parser.Parse(LogicalId{0, 0, 4}, U"material{materials}=\"cuprum\";", 3);
    auto r = parser.Parse(LogicalId{0, 0, 5}, U"ro{material};", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.68E-8") << r.ToStdString(3, 3);

    parser.Parse(LogicalId{0, 0, 4}, U"material{materials}=\"ferrum\";", 3);
    r = parser.Parse(LogicalId{0, 0, 5}, U"ro{material};", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "9.71E-8") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, list2)
{
    yutovo_calculator::ParserContext parser_context;
    parser_context.include_document = true;
    parser.Parse(LogicalId{0, 0, 1}, U"materials=\"cuprum\",\"ferrum\";", &parser_context);
    parser.Parse(LogicalId{0, 0, 2}, U"ro{cuprum}=1.68E-8*(Ohm*m);", &parser_context);
    parser.Parse(LogicalId{0, 0, 3}, U"ro{ferrum}=9.71E-8*(Ohm*m);", &parser_context);
    parser.Parse(LogicalId{0, 0, 4}, U"length=1m;", &parser_context);
    parser.Parse(LogicalId{0, 0, 5}, U"section=1*pow(mm,2);", &parser_context);
    parser.Parse(LogicalId{0, 0, 6}, U"material{materials}=\"ferrum\";", &parser_context);
    parser.Parse(LogicalId{0, 0, 7}, U"resistance=ro{material}*(length)/(section);", &parser_context);
    auto r = parser.GetSuitableUnit(LogicalId{0, 0, 8}, parser.Parse(LogicalId{0, 0, 8}, U"resistance;", &parser_context));
    ASSERT_TRUE(r.ToStdString(3, 3) == "97.1E+0(mOhm)") << r.ToStdString(3, 3);

    parser_context.include_document = false;
    parser.Parse(LogicalId{0, 1, 1}, U"length=4m;", &parser_context);
    parser.Parse(LogicalId{0, 1, 2}, U"section=2*pow(mm,2);", &parser_context);
    parser.Parse(LogicalId{0, 1, 3}, U"material{materials}=\"cuprum\";", &parser_context);
    r = parser.GetSuitableUnit(LogicalId{0, 1, 4}, parser.Parse(LogicalId{0, 1, 4}, U"resistance;", &parser_context));
    ASSERT_TRUE(r.ToStdString(3, 3) == "33.6E+0(mOhm)") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, pt_BR_language_to_string)
{
    ASSERT_TRUE(LanguageToString(Language::BrazilianPortuguese) == "Portuguese (Brazil)");
}

TEST_F(CalcTestReal, pt_BR_si_units_distance)
{
    parser.SetLocale(Language::BrazilianPortuguese);

    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 1}, parser.Parse(LogicalId{0, 0, 1}, U"1m;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(m)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 2}, parser.Parse(LogicalId{0, 0, 2}, U"1km;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(km)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 3}, parser.Parse(LogicalId{0, 0, 3}, U"1000m;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(km)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 4}, parser.Parse(LogicalId{0, 0, 4}, U"1cm;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(cm)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 5}, parser.Parse(LogicalId{0, 0, 5}, U"1mm;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(mm)") << s;
}

TEST_F(CalcTestReal, pt_BR_si_units_time)
{
    parser.SetLocale(Language::BrazilianPortuguese);

    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 1}, parser.Parse(LogicalId{0, 0, 1}, U"1s;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(s)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 2}, parser.Parse(LogicalId{0, 0, 2}, U"1min;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(min)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 3}, parser.Parse(LogicalId{0, 0, 3}, U"60s;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(min)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 4}, parser.Parse(LogicalId{0, 0, 4}, U"1ms;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(ms)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 5}, parser.Parse(LogicalId{0, 0, 5}, U"1000ms;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(s)") << s;
}

TEST_F(CalcTestReal, pt_BR_si_units_mass)
{
    parser.SetLocale(Language::BrazilianPortuguese);

    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 1}, parser.Parse(LogicalId{0, 0, 1}, U"1kg;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(kg)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 2}, parser.Parse(LogicalId{0, 0, 2}, U"1000g;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(kg)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 3}, parser.Parse(LogicalId{0, 0, 3}, U"1ton;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(ton)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 4}, parser.Parse(LogicalId{0, 0, 4}, U"1mg;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(g)") << s;
}

TEST_F(CalcTestReal, pt_BR_si_units_electric)
{
    parser.SetLocale(Language::BrazilianPortuguese);

    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 1}, parser.Parse(LogicalId{0, 0, 1}, U"1A;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(A)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 2}, parser.Parse(LogicalId{0, 0, 2}, U"1V;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(V)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 3}, parser.Parse(LogicalId{0, 0, 3}, U"1Ohm;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(Ohm)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 4}, parser.Parse(LogicalId{0, 0, 4}, U"1F;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(F)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 5}, parser.Parse(LogicalId{0, 0, 5}, U"1W;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(W)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 6}, parser.Parse(LogicalId{0, 0, 6}, U"1J;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(J)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 7}, parser.Parse(LogicalId{0, 0, 7}, U"1Pa;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(Pa)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 8}, parser.Parse(LogicalId{0, 0, 8}, U"1T;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(T)") << s;
}

TEST_F(CalcTestReal, pt_BR_other_units)
{
    parser.SetLocale(Language::BrazilianPortuguese);

    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 1}, parser.Parse(LogicalId{0, 0, 1}, U"1l;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(l)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 2}, parser.Parse(LogicalId{0, 0, 2}, U"1ml;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(ml)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 3}, parser.Parse(LogicalId{0, 0, 3}, U"1byte;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "8.E+0(bit)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 4}, parser.Parse(LogicalId{0, 0, 4}, U"1kbyte;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(kbyte)") << s;

    s = parser.GetSuitableUnit(LogicalId{0, 0, 5}, parser.Parse(LogicalId{0, 0, 5}, U"1ha;")).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(ha)") << s;
}

TEST_F(CalcTestReal, pt_BR_physical_constants)
{
    parser.SetLocale(Language::BrazilianPortuguese);

    auto r = parser.Parse(LogicalId{0, 0, 1}, U"v_l;");
    ASSERT_TRUE(r.ToStdString(3, 3).find("2.998E+8") == 0) << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 2}, U"G;");
    ASSERT_TRUE(r.ToStdString(3, 3).find("6.674E-11") == 0) << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 3}, U"h;");
    ASSERT_TRUE(r.ToStdString(3, 3).find("6.626E-34") == 0) << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 4}, U"k;");
    ASSERT_TRUE(r.ToStdString(3, 3).find("1.381E-23") == 0) << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 5}, U"e_c;");
    ASSERT_TRUE(r.ToStdString(3, 3).find("1.602E-19") != std::string::npos) << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 6}, U"a_g;");
    ASSERT_TRUE(r.ToStdString(3, 3).find("9.807E+0") == 0) << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, pt_BR_us_units)
{
    parser.SetLocale(Language::BrazilianPortuguese);

    auto r = parser.Parse(LogicalId{0, 0, 1}, U"1polegada{us};");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 1}, r).ToStdString(3, 3);
    ASSERT_TRUE(s.find("polegada") != std::string::npos) << s;

    r = parser.Parse(LogicalId{0, 0, 2}, U"1jarda{us};");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 2}, r).ToStdString(3, 3);
    ASSERT_TRUE(s.find("jarda") != std::string::npos) << s;

    r = parser.Parse(LogicalId{0, 0, 3}, U"1milha{us};");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 3}, r).ToStdString(3, 3);
    ASSERT_TRUE(s.find("milha") != std::string::npos) << s;

    r = parser.Parse(LogicalId{0, 0, 4}, U"1libra{us};");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 4}, r).ToStdString(3, 3);
    ASSERT_TRUE(s.find("libra") != std::string::npos) << s;

    r = parser.Parse(LogicalId{0, 0, 5}, U"1galão{us};");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 5}, r).ToStdString(3, 3);
    ASSERT_TRUE(s.find("galão") != std::string::npos) << s;
}

TEST_F(CalcTestReal, pt_BR_russian_units)
{
    parser.SetLocale(Language::BrazilianPortuguese);

    auto r = parser.Parse(LogicalId{0, 0, 1}, U"1ponto{rus};");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 1}, r).ToStdString(3, 3);
    ASSERT_TRUE(s.find("ponto") != std::string::npos) << s;

    r = parser.Parse(LogicalId{0, 0, 2}, U"10linha{rus};");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 2}, r).ToStdString(3, 3);
    ASSERT_TRUE(s.find("polegada") != std::string::npos) << s;

    r = parser.Parse(LogicalId{0, 0, 3}, U"1copo{rus};");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 3}, r).ToStdString(3, 3);
    ASSERT_TRUE(s.find("copo") != std::string::npos) << s;

    r = parser.Parse(LogicalId{0, 0, 4}, U"500sazhen{rus};");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 4}, r).ToStdString(3, 3);
    ASSERT_TRUE(s.find("versta") != std::string::npos) << s;
}

TEST_F(CalcTestReal, pt_BR_currency)
{
    parser.SetLocale(Language::BrazilianPortuguese);

    auto r = parser.Parse(LogicalId{0, 0, 1}, U"1R$;");
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(R$)") << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 2}, U"1real;");
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(R$)") << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 3}, U"1dólar;");
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0($)") << r.ToStdString(3, 3);

    r = parser.Parse(LogicalId{0, 0, 4}, U"1euro;");
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0(€)") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, pt_locale_switch_back)
{
    parser.SetLocale(Language::BrazilianPortuguese);
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"1hora;");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 1}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(hora)") << s;

    parser.SetLocale(Language::English);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 2}, U"1hora;"), yutovo_calculator::SyntaxException);
    r = parser.Parse(LogicalId{0, 0, 3}, U"1hour;");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 3}, r).ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0(hour)") << s;
}


TEST_F(CalcTestReal, sin_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"sin(pow(10,20));");
    ASSERT_FALSE(r == Real(3, 0)) << r.ToStdString(3, 10);
    ASSERT_TRUE(r.ToStdString(3, 3) == "-0.645E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, cos_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"cos(pow(10,20));");
    ASSERT_FALSE(r == Real(3, 1)) << r.ToStdString(3, 10);
    ASSERT_FALSE(r == Real(3, -1)) << r.ToStdString(3, 10);
    ASSERT_TRUE(r.ToStdString(3, 3) == "0.764E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, trigonometric_special)
{
    Real res = parser.Parse(LogicalId{0, 0, 1}, U"sin(pi);");
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"0;").ToStdString(3, 3)) << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 1}, U"cos(pi);");
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"-1;").ToStdString(3, 3)) << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 1}, U"sin((pi)/(2));");
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 1}, U"1;").ToStdString(3, 3)) << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, tg_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"tg(pow(10,20));");
    ASSERT_FALSE(r == Real(3, 0)) << r.ToStdString(3, 10);
    ASSERT_TRUE(r.ToStdString(3, 3) == "-0.845E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, ctg_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"ctg(pow(10,20));");
    ASSERT_FALSE(r == Real(3, 0)) << r.ToStdString(3, 10);
    ASSERT_TRUE(r.ToStdString(3, 3) == "-1.184E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, sec_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"sec(pow(10,20));");
    ASSERT_FALSE(r == Real(3, 1)) << r.ToStdString(3, 10);
    ASSERT_FALSE(r == Real(3, -1)) << r.ToStdString(3, 10);
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.309E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, cosec_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"cosec(pow(10,20));");
    ASSERT_FALSE(r == Real(3, 0)) << r.ToStdString(3, 10);
    ASSERT_TRUE(r.ToStdString(3, 3) == "-1.55E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, ln_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"ln(pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "46.052E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, lg_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"lg(pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "20.E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, log_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"log(2,pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "66.439E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, exp_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"exp(pow(10,20));");
    ASSERT_TRUE(r.IsInfinity()) << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, arctg_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"arctg(pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.571E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, arcctg_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"arcctg(pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "0.E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, arcsec_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"arcsec(pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.571E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, arccosec_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"arccosec(pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E-20") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, th_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"th(pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, cth_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"cth(pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, sch_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"sch(pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "0.E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, csch_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"csch(pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "0.E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, arsh_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"arsh(pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "46.745E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, arch_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"arch(pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "46.745E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, arcth_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"arcth(pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E-20") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, arsch_large_arg)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"arsch(pow(10,20));"), yutovo_calculator::MathException);
}

TEST_F(CalcTestReal, arcsch_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"arcsch(pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E-20") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, sqrt_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"sqrt(pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+10") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, root_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"root(pow(10,20),3);");
    ASSERT_TRUE(r.ToStdString(3, 3) == "4.642E+6") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, pow_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"pow(10,20);");
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+20") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, abs_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"abs(-pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+20") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, floor_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"floor(pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.E+20") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, fract_large_arg)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"fract(pow(10,20));");
    ASSERT_TRUE(r.ToStdString(3, 3) == "0.E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestReal, fact1)
{
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0!;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1!;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"5!;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "120.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"10!;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "3.629E+6") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, fact2)
{
    yutovo_calculator::ParserContext parser_context;
    parser_context.Init(100);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1000000!;", &parser_context),
        yutovo_calculator::TimeExceedException);
}

TEST_F(CalcTestReal, min_max1)
{
    auto res = parser.Parse(LogicalId{0, 0, 1}, U"min(2.5,3.7);", 3);
    ASSERT_TRUE(res.ToStdString(3, 3) == "2.5E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"max(2.5,3.7);", 3);
    ASSERT_TRUE(res.ToStdString(3, 3) == "3.7E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"min(-1.2,-1.5);", 3);
    ASSERT_TRUE(res.ToStdString(3, 3) == "-1.5E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"max(-1.2,-1.5);", 3);
    ASSERT_TRUE(res.ToStdString(3, 3) == "-1.2E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, definite_integral1)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,1,x,x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "0.5E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, definite_integral2)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(-1,4,sin(x),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "1.1939459267E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, definite_integral_alias)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,1,x,x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "0.5E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, definite_integral3)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,pi,sin(x),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "2.E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, definite_integral4)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(1,2,1/x,x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "0.6931471806E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, definite_integral5)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,100,x,x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "5000.E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, definite_integral6)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(2,2,pow(x,3),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "0.E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, definite_integral7)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,3,2*pow(x,2)+3*x+1,x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "34.5E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, definite_integral8)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,pi/2,cos(x),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "1.E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, definite_integral9)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,1,exp(x),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "1.7182818285E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, definite_integral10)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(1,exp(1),ln(x),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "1.E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, definite_integral11)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,10,pow(x,4),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "20000.E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, definite_integral12)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,1,pow(x,2)*sin(x),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "0.2232442755E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, definite_integral_inf1)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,∞,exp(-x),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "1.E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, definite_integral_inf2)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,∞,1/(1+pow(x,2)),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "1.5707963268E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, definite_integral_inf3)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,∞,1/pow((x+1),2),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "1.E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, definite_integral_inf4)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(1,∞,1/pow(x,2),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "1.E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, definite_integral_inf5)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,∞,x*exp(-x),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "1.E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, definite_integral_wide_interval)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(-3,33,pow(x,2)*sin(x),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "72.649339474E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestReal, threads_integrals)
{
    constexpr int threads_count = 50;
    constexpr int iterations = 20;

    Parser<Real> expected_parser(3, Language::English);
    std::vector<std::string> expected1(threads_count);
    std::vector<std::string> expected2(threads_count);
    std::vector<std::string> expected3(threads_count);
    std::vector<std::string> expected4(threads_count);
    for (int i = 0; i < threads_count; ++i)
    {
        std::u32string coeff = ToUtfString(std::to_string(i));
        expected1[i] = expected_parser.Parse(LogicalId{0, 0, 0, 0, 1}, coeff + U"/2;", 10).ToStdString(10, 10);
        expected2[i] = expected_parser.Parse(LogicalId{0, 0, 0, 0, 1}, coeff + U"/4;", 10).ToStdString(10, 10);
        expected3[i] = expected_parser.Parse(LogicalId{0, 0, 0, 0, 1}, coeff + U"/8;", 10).ToStdString(10, 10);
        expected4[i] = expected_parser.Parse(LogicalId{0, 0, 0, 0, 1}, coeff + U"/16;", 10).ToStdString(10, 10);
    }

    std::vector<std::vector<std::string>> actual1(iterations, std::vector<std::string>(threads_count));
    std::vector<std::vector<std::string>> actual2(iterations, std::vector<std::string>(threads_count));
    std::vector<std::vector<std::string>> actual3(iterations, std::vector<std::string>(threads_count));
    std::vector<std::vector<std::string>> actual4(iterations, std::vector<std::string>(threads_count));
    std::vector<std::thread> threads;
    threads.reserve(threads_count);

    std::mutex m;
    std::condition_variable cv_start;
    std::condition_variable cv_done;
    int current_iter = -1;
    int done_count = 0;

    for (int i = 0; i < threads_count; ++i)
    {
        threads.emplace_back(
            [&, i]()
            {
                Parser<Real> p(3, Language::English);
                std::u32string coeff = ToUtfString(std::to_string(i));
                int iter = -1;
                while (true)
                {
                    std::unique_lock<std::mutex> lock(m);
                    cv_start.wait(lock,
                        [&]()
                        {
                            return current_iter != iter;
                        });
                    iter = current_iter;
                    if (iter >= iterations)
                        break;
                    lock.unlock();

                    auto res1 = p.Parse(LogicalId{0, 0, 1}, U"definite_integral(0,1," + coeff + U"*x,x);", 10);
                    actual1[iter][i] = res1.ToStdString(10, 10);

                    auto res2 = p.Parse(LogicalId{0, 0, 2}, U"definite_integral(0,1," + coeff + U"*pow(x,3),x);", 10);
                    actual2[iter][i] = res2.ToStdString(10, 10);

                    auto res3 = p.Parse(LogicalId{0, 0, 3}, U"definite_integral(0,1," + coeff + U"*pow(x,7),x);", 10);
                    actual3[iter][i] = res3.ToStdString(10, 10);

                    auto res4 = p.Parse(LogicalId{0, 0, 4}, U"definite_integral(0,1," + coeff + U"*pow(x,15),x);", 10);
                    actual4[iter][i] = res4.ToStdString(10, 10);

                    lock.lock();
                    ++done_count;
                    if (done_count == threads_count)
                        cv_done.notify_one();
                }
            });
    }

    for (int iter = 0; iter < iterations; ++iter)
    {
        {
            std::lock_guard<std::mutex> lock(m);
            current_iter = iter;
            done_count = 0;
        }
        cv_start.notify_all();

        std::unique_lock<std::mutex> lock(m);
        cv_done.wait(lock,
            [&]()
            {
                return done_count == threads_count;
            });
        lock.unlock();

        for (int i = 0; i < threads_count; ++i)
        {
            ASSERT_TRUE(!expected1[i].empty());
            ASSERT_TRUE(actual1[iter][i] == expected1[i]) << "iter=" << iter << " i=" << i << " actual=" << actual1[iter][i] << " expected=" << expected1[i];
            ASSERT_TRUE(!expected2[i].empty());
            ASSERT_TRUE(actual2[iter][i] == expected2[i]) << "iter=" << iter << " i=" << i << " actual=" << actual2[iter][i] << " expected=" << expected2[i];
            ASSERT_TRUE(!expected3[i].empty());
            ASSERT_TRUE(actual3[iter][i] == expected3[i]) << "iter=" << iter << " i=" << i << " actual=" << actual3[iter][i] << " expected=" << expected3[i];
            ASSERT_TRUE(!expected4[i].empty());
            ASSERT_TRUE(actual4[iter][i] == expected4[i]) << "iter=" << iter << " i=" << i << " actual=" << actual4[iter][i] << " expected=" << expected4[i];
        }
        actual1[iter].clear();
        actual2[iter].clear();
        actual3[iter].clear();
        actual4[iter].clear();
    }

    {
        std::lock_guard<std::mutex> lock(m);
        current_iter = iterations;
    }
    cv_start.notify_all();

    for (auto& t : threads)
        t.join();
}

TEST_F(CalcTestReal, derivative_at_point1)
{
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,2), [x=3]);");
    ASSERT_TRUE(res == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"6;")) << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, derivative_at_point2)
{
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,3), [x=2]);");
    ASSERT_TRUE(res == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"12;")) << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, derivative_at_point3)
{
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(sin(x), [x=0]);");
    ASSERT_TRUE(res == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1;")) << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, derivative_at_point4)
{
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,2), [y=3]);");
    ASSERT_TRUE(res == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0;")) << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, derivative_at_point5)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x)=2*x;");
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x), [x=2]);");
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"2;").ToStdString(3, 3)) << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, derivative_at_point6)
{
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,2), [x=3])+1;");
    ASSERT_TRUE(res == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"7;")) << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, derivative_at_point_error1)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,2), [1=3]);"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestReal, derivative_at_point7)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x,y)=x*y;");
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x,5), [x=2]);");
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"5;").ToStdString(3, 3)) << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, derivative_at_point8)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x,y)=x+y;");
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(4,y), [y=3]);");
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"1;").ToStdString(3, 3)) << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, derivative_at_point_multi1)
{
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x*y, [x=2, y=3]);");
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"3;").ToStdString(3, 3)) << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, derivative_at_point_multi2)
{
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x*y, [y=3, x=2]);");
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2;").ToStdString(3, 3)) << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, derivative_at_point_multi3)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x,y)=pow(x,2)*y;");
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x,y), [x=2, y=3]);");
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"12;").ToStdString(3, 3)) << res.ToStdString(3, 3);
}

TEST_F(CalcTestReal, definite_integral_user_function)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"iff(t)=5*pow(e,-2t);");
    Real res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"definite_integral(0,1,iff(t),t);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "2.1616617919E+0") << res.ToStdString(10, 10);
}

}
