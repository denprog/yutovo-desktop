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
#include "export.h"

namespace yutovo_calc_test
{

using namespace yutovo_calculator;
using namespace std::chrono_literals;

TEST_F(CalcTestRational, arithmetic1)
{
    Rational res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"10%75;");
    ASSERT_TRUE(res.ToString() == U"15/2") << res.ToStdString();
}

TEST_F(CalcTestRational, arithmetic2)
{
    Rational res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-(1)/(3);");
    ASSERT_TRUE(res.ToString() == U"-1/3") << res.ToStdString();
}

TEST_F(CalcTestRational, variables1)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"п=3/2;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"п;") == parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"3/2;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"п;").ToStdString();
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"пр=3/2;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 1, 0, 1}, U"пр+5;") == parser.Parse(LogicalId{0, 0, 1, 0, 1}, U"13/2;")) << 
        parser.Parse(LogicalId{0, 0, 1, 0, 1}, U"пр+5;").ToStdString();
}

TEST_F(CalcTestRational, variables2)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a=5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"(3)/(4)a;") == parser.Parse(LogicalId{0, 0, 2}, U"(15)/(4);")) << 
        parser.Parse(LogicalId{0, 0, 2}, U"(3)/(4)a;");
}

TEST_F(CalcTestRational, variables3)
{
    parser.Parse(LogicalId{0, 0, 1}, U"d=4;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"5d;") == parser.Parse(LogicalId{0, 0, 2}, U"20;")) << 
        parser.Parse(LogicalId{0, 0, 2}, U"5d;");
    parser.Parse(LogicalId{0, 0, 3}, U"d2=5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"5d+3d2;") == parser.Parse(LogicalId{0, 0, 4}, U"35;")) << 
        parser.Parse(LogicalId{0, 0, 4}, U"5d+3d2;");
}

TEST_F(CalcTestRational, variables4)
{
    parser.Parse(LogicalId{0, 0, 1}, U"d=1/4;");
    parser.Parse(LogicalId{0, 0, 2}, U"d{2}=3/4;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 3}, U"d+d{2};") == parser.Parse(LogicalId{0, 0, 3}, U"1;")) << 
        parser.Parse(LogicalId{0, 0, 3}, U"d+d{2};");
    parser.Parse(LogicalId{0, 0, 4}, U"d{2}=5/2;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 5}, U"5d{2}+3d;") == parser.Parse(LogicalId{0, 0, 5}, U"13(1/4);")) << 
        parser.Parse(LogicalId{0, 0, 5}, U"5d{2}+3d;");
}

TEST_F(CalcTestRational, functions1)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"f(x)=2*x/3;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"f(2);") == parser.Parse(LogicalId{0, 0, 0, 0, 2, 0}, U"4/3;"));
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x)=x/4;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 2, 3, 1}, U"f(2);") == parser.Parse(LogicalId{0, 0, 0, 2, 3, 1}, U"4/3;")) << 
        parser.Parse(LogicalId{0, 0, 0, 2, 3, 1}, U"f(2);").ToStdString();
}

TEST_F(CalcTestRational, functions2)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"3pow(2,3);", 3);
    ASSERT_TRUE(r.ToStdString() == "24") << r.ToStdString();
}

TEST_F(CalcTestRational, functions3)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"pow(2,1/3);", 3), yutovo_calculator::MathException) << 
        parser.Parse(LogicalId{0, 0, 1}, U"pow(2,1/3);").ToStdString();

    auto r = parser.Parse(LogicalId{0, 0, 1}, U"pow(2,6/3);", 3);
    ASSERT_TRUE(r.ToStdString() == "4") << r.ToStdString();
}

TEST_F(CalcTestRational, functions4)
{
    parser.Parse(LogicalId{0, 0, 0, 2}, U"f(x)=(x)/(3);");
    auto r = parser.Parse(LogicalId{0, 0, 0, 3}, U"f(4);", 3);
    ASSERT_TRUE(r.ToStdString() == "4/3") << r.ToStdString();
}

TEST_F(CalcTestRational, functions5)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"abs(-(2)/(3));", 3);
    ASSERT_TRUE(r.ToStdString() == "2/3") << r.ToStdString();
    r = parser.Parse(LogicalId{0, 0, 1}, U"abs((2)/(3));", 3);
    ASSERT_TRUE(r.ToStdString() == "2/3") << r.ToStdString();
}

TEST_F(CalcTestRational, functions6)
{
    Rational res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow((5)/(2),-3);");
    ASSERT_TRUE(res.ToString() == U"8/125") << res.ToStdString();
}

TEST_F(CalcTestRational, functions7)
{
    Rational res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow((5)/(2),0);");
    ASSERT_TRUE(res.ToString() == U"1") << res.ToStdString();
}

TEST_F(CalcTestRational, functions8)
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

TEST_F(CalcTestRational, functions9)
{
    Rational res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(0, 2);");
    ASSERT_TRUE(res.ToString() == U"0") << res.ToStdString();
}

TEST_F(CalcTestRational, functions10)
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

TEST_F(CalcTestRational, symbols1)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"•;"), yutovo_calculator::SyntaxException) << parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"•;").ToStdString();
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"‣;"), yutovo_calculator::SyntaxException) << parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"‣;").ToStdString();
}

TEST_F(CalcTestRational, errors1)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"123"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"123+(2/3)"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"123+3;45"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestRational, errors2)
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

TEST_F(CalcTestRational, errors3)
{
    try
    {
        parser.Parse(LogicalId{0, 0, 1}, U"func(3)+5;");
    }
    catch (yutovo_calculator::SyntaxException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 1}) && ex.ex_id == ParserExceptionCode::UnknownIdentifier && ex.pos == 0 && ex.size == 4) << ex.ex_id;
        return;
    }
    ASSERT_FALSE(true);
}

TEST_F(CalcTestRational, errors4)
{
    try
    {
        parser.Parse(LogicalId{0, 0, 1}, U"pow(23423423,234234234234234);");
    }
    catch (yutovo_calculator::MathException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 1}) && ex.ex_id == ParserExceptionCode::Overflow && ex.pos == 0) << ex.ex_id;
        return;
    }
    ASSERT_FALSE(true);
}

TEST_F(CalcTestRational, errors5)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"(1)/(0);"), yutovo_calculator::MathException);
}

TEST_F(CalcTestRational, errors9)
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

TEST_F(CalcTestRational, rationals1)
{
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2/4;") == parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"1/2;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2/4;").ToStdString();
}

TEST_F(CalcTestRational, rationals2)
{
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2(1/4);") == parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"9/4;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2(1/4);").ToStdString();
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"3*22(3/5);") == parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"339/5;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"3*22(3/5);").ToStdString();
}

TEST_F(CalcTestRational, rationals3)
{
    Rational res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"+3/4;");
    ASSERT_TRUE(res.ToStdString() == "3/4") << res.ToStdString();
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-3/4;");
    ASSERT_TRUE(res.ToStdString() == "-3/4") << res.ToStdString();
}

TEST_F(CalcTestRational, rationals4)
{
    Rational res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2(5/(4*2));");
    ASSERT_TRUE(res.ToStdString() == "5/4") << res.ToStdString();
}

TEST_F(CalcTestRational, rationals5)
{
    Rational res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2.5;");
    ASSERT_TRUE(res.ToStdString() == "5/2") << res.ToStdString();
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"22.12;");
    ASSERT_TRUE(res.ToStdString() == "553/25") << res.ToStdString();
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U".12;");
    ASSERT_TRUE(res.ToStdString() == "3/25") << res.ToStdString();
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0.12;");
    ASSERT_TRUE(res.ToStdString() == "3/25") << res.ToStdString();
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"12.;");
    ASSERT_TRUE(res.ToStdString() == "12") << res.ToStdString();
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"12.0;");
    ASSERT_TRUE(res.ToStdString() == "12") << res.ToStdString();
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"54.990;");
    ASSERT_TRUE(res.ToStdString() == "5499/100") << res.ToStdString();
}

TEST_F(CalcTestRational, rationals9)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"."), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"..2"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1..2"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.2.3"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2.3.4"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U".1.2"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.2."), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestRational, rationals10)
{
    Rational r;
    EXPECT_THROW(r = U"abc", yutovo_calculator::SyntaxException);
    EXPECT_THROW(r = U"x/y", yutovo_calculator::SyntaxException);
    EXPECT_THROW(r = U"1/2/3", yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestRational, rationals6)
{
    parser.SetLocale(Language::Russian);
    Rational res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2.5;");
    ASSERT_TRUE(res.ToStdString() == "5/2") << res.ToStdString();
}

TEST_F(CalcTestRational, rationals7)
{
    Rational res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-2.3;");
    ASSERT_TRUE(res.ToStdString() == "-23/10") << res.ToStdString();
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-0.12;");
    ASSERT_TRUE(res.ToStdString() == "-3/25") << res.ToStdString();
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-.12;");
    ASSERT_TRUE(res.ToStdString() == "-3/25") << res.ToStdString();
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-2.5;");
    ASSERT_TRUE(res.ToStdString() == "-5/2") << res.ToStdString();
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-12.0;");
    ASSERT_TRUE(res.ToStdString() == "-12") << res.ToStdString();
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-54.990;");
    ASSERT_TRUE(res.ToStdString() == "-5499/100") << res.ToStdString();
}

TEST_F(CalcTestRational, proper1)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(5/4);");
    Integer i, n, d;
    res.ToProper(i, n, d);
    ASSERT_TRUE(i.ToStdString() == "1");
    ASSERT_TRUE(n.ToStdString() == "1");
    ASSERT_TRUE(d.ToStdString() == "4");
}

TEST_F(CalcTestRational, proper2)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(3/4);");
    Integer i, n, d;
    res.ToProper(i, n, d);
    ASSERT_TRUE(i.ToStdString() == "0");
    ASSERT_TRUE(n.ToStdString() == "3");
    ASSERT_TRUE(d.ToStdString() == "4");
}

TEST_F(CalcTestRational, proper3)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-(2/3);");
    Integer i, n, d;
    res.ToProper(i, n, d);
    ASSERT_TRUE(i.ToStdString() == "0") << i.ToStdString();
    ASSERT_TRUE(n.ToStdString() == "-2") << n.ToStdString();
    ASSERT_TRUE(d.ToStdString() == "3") << d.ToStdString();
}

TEST_F(CalcTestRational, proper4)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-(2/(-3));");
    Integer i, n, d;
    res.ToProper(i, n, d);
    ASSERT_TRUE(i.ToStdString() == "0") << i.ToStdString();
    ASSERT_TRUE(n.ToStdString() == "2") << n.ToStdString();
    ASSERT_TRUE(d.ToStdString() == "3") << d.ToStdString();
}

TEST_F(CalcTestRational, proper5)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"((-2)/(-3));");
    Integer i, n, d;
    res.ToProper(i, n, d);
    ASSERT_TRUE(i.ToStdString() == "0") << i.ToStdString();
    ASSERT_TRUE(n.ToStdString() == "2") << n.ToStdString();
    ASSERT_TRUE(d.ToStdString() == "3") << d.ToStdString();
}

TEST_F(CalcTestRational, proper6)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"((-7)/3);");
    Integer i, n, d;
    res.ToProper(i, n, d);
    ASSERT_TRUE(i.ToStdString() == "-2") << i.ToStdString();
    ASSERT_TRUE(n.ToStdString() == "1") << n.ToStdString();
    ASSERT_TRUE(d.ToStdString() == "3") << d.ToStdString();
}

TEST_F(CalcTestRational, proper7)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(7/(-3));");
    Integer i, n, d;
    res.ToProper(i, n, d);
    ASSERT_TRUE(i.ToStdString() == "-2") << i.ToStdString();
    ASSERT_TRUE(n.ToStdString() == "1") << n.ToStdString();
    ASSERT_TRUE(d.ToStdString() == "3") << d.ToStdString();
}

TEST_F(CalcTestRational, proper8)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(2/(-3));");
    Integer i, n, d;
    res.ToProper(i, n, d);
    ASSERT_TRUE(i.ToStdString() == "0") << i.ToStdString();
    ASSERT_TRUE(n.ToStdString() == "-2") << n.ToStdString();
    ASSERT_TRUE(d.ToStdString() == "3") << d.ToStdString();
}

TEST_F(CalcTestRational, units1)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1m;")).ToStdString();
    ASSERT_TRUE(s == "1(m)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2/3*m;")).ToStdString();
    ASSERT_TRUE(s == "2/3(m)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(2)/(3)m*2m;")).ToStdString();
    ASSERT_TRUE(s == "4/3(m^2)") << s;
}

TEST_F(CalcTestRational, units2)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2/m;")).ToStdString();
    ASSERT_TRUE(s == "2(1/(m))") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2*(1/m);")).ToStdString();
    ASSERT_TRUE(s == "2(1/(m))") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2m*3*(1/m);")).ToStdString();
    ASSERT_TRUE(s == "6") << s;
}

TEST_F(CalcTestRational, units3)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"3*(2)/(m);")).ToStdString();
    ASSERT_TRUE(s == "6(1/(m))") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"3*(6m)/(3s);")).ToStdString();
    ASSERT_TRUE(s == "6((m)/(s))") << s;
}

TEST_F(CalcTestRational, units4)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(2)/(3)m+3m;")).ToStdString();
    ASSERT_TRUE(s == "11/3(m)") << s;
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"3m+4;"), yutovo_calculator::MathException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"4/5+3m+4;"), yutovo_calculator::MathException);
}

TEST_F(CalcTestRational, units5)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(5)/(3)m-3m;")).ToStdString();
    ASSERT_TRUE(s == "-4/3(m)") << s;
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"3m-4;"), yutovo_calculator::MathException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"4/5-3m-4;"), yutovo_calculator::MathException);
}

TEST_F(CalcTestRational, units6)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(3m,2);")).ToStdString();
    ASSERT_TRUE(s == "9(m^2)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(3m/s,2);")).ToStdString();
    ASSERT_TRUE(s == "9(Gy)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"5m/pow(s,2);")).ToStdString();
    ASSERT_TRUE(s == "5((m)/(s^2))") << s;
}

TEST_F(CalcTestRational, units7)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 0, 1}, U"km~1000m;");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"1km;")).ToStdString();
    ASSERT_TRUE(s == "1(km)") << s;
}

TEST_F(CalcTestRational, units8)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"2m;")).ToStdString();
    ASSERT_TRUE(s == "2(m)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"2/100*m;")).ToStdString();
    ASSERT_TRUE(s == "2(cm)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"(2)/(1000)m;")).ToStdString();
    ASSERT_TRUE(s == "2(mm)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"200m;")).ToStdString();
    ASSERT_TRUE(s == "200(m)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"(3000)/(2)m;")).ToStdString();
    ASSERT_TRUE(s == "3/2(km)") << s;
}

TEST_F(CalcTestRational, units9)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"(1)/(5)m/s;")).ToStdString();
    ASSERT_TRUE(s == "2((dm)/(s))") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"5000m/s;")).ToStdString();
    ASSERT_TRUE(s == "5((km)/(s))") << s;
}

TEST_F(CalcTestRational, units10)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"1Hz;")).ToStdString();
    ASSERT_TRUE(s == "1(Hz)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"2000Hz;")).ToStdString();
    ASSERT_TRUE(s == "2(kHz)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"50*(1/s);")).ToStdString();
    ASSERT_TRUE(s == "50(Hz)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"50*(1)/(s);")).ToStdString();
    ASSERT_TRUE(s == "50(Hz)") << s;
}

TEST_F(CalcTestRational, units11)
{
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"10kg*10m/(2*pow(s,2));")).ToStdString();
    ASSERT_TRUE(s == "50(N)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"1000kg*10m/(2*pow(s,2));")).ToStdString();
    ASSERT_TRUE(s == "5(kN)") << s;
}

TEST_F(CalcTestRational, units12)
{
    std::string t = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"liniya{rus};")).ToStdString();
    ASSERT_TRUE(t == "1(liniya){rus}") << t;
}

TEST_F(CalcTestRational, units13)
{
    std::string t = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"2*km;")).ToStdString();
    ASSERT_TRUE(t == "2(km)") << t;
}

TEST_F(CalcTestRational, units14)
{
    std::string t = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"7fut{rus};")).ToStdString();
    ASSERT_TRUE(t == "1(sazhen){rus}") << t;
}

TEST_F(CalcTestRational, units15)
{
    LogicalId id{0, 0, 0, 0, 0, 0, 0, 2, 0};
    auto val = parser.Parse(id, U"5*(km/hour);");
    std::vector<Unit> cast_units;
    parser.GetCastUnits(id, val, cast_units);
    ASSERT_TRUE(FindUnit(cast_units, Unit(U"m", U"s")));
    ASSERT_TRUE(FindUnit(cast_units, Unit(U"km", U"hour")));
    std::string t = parser.GetSuitableUnit(id, val).ToStdString();
    ASSERT_TRUE(t == "5((km)/(hour))") << t;
}

TEST_F(CalcTestRational, units16)
{
    std::string t = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"(10)/(4s);")).ToStdString();
    ASSERT_TRUE(t == "5/2(Hz)") << t;
    t = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"(50)/(4s);")).ToStdString();
    ASSERT_TRUE(t == "25/2(Hz)") << t;
}

TEST_F(CalcTestRational, units17)
{
    std::string t = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"(1m)/(3s);")).ToStdString();
    ASSERT_TRUE(t == "20((m)/(min))") << t;
}

TEST_F(CalcTestRational, units18)
{
    std::string t = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"(1/3)m;")).ToStdString();
    ASSERT_TRUE(t == "1/3(m)") << t;
}

TEST_F(CalcTestRational, units19)
{
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"-(1m-2m);");
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString();
    ASSERT_TRUE(s == "1(m)") << s;

    r = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"+(2m-1m);");
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, r).ToStdString();
    ASSERT_TRUE(s == "1(m)") << s;
}

TEST_F(CalcTestRational, units20)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"pow(2,3)m;", 3);
    ASSERT_TRUE(r.ToStdString() == "8(m)") << r.ToStdString();
}

TEST_F(CalcTestRational, units21)
{
    parser.SetLocale(Language::Spanish);
    LogicalId id{0, 0, 0, 0, 0, 0, 0, 2, 0};
    auto val = parser.Parse(id, U"5*(km/hora);");
    std::vector<Unit> cast_units;
    parser.GetCastUnits(id, val, cast_units);
    ASSERT_TRUE(FindUnit(cast_units, Unit(U"m", U"s")));
    ASSERT_TRUE(FindUnit(cast_units, Unit(U"km", U"hora")));
    std::string t = parser.GetSuitableUnit(id, val).ToStdString();
    ASSERT_TRUE(t == "5((km)/(hora))") << t;
}

TEST_F(CalcTestRational, units22)
{
    parser.SetLocale(Language::Russian);
    std::string s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(1)/(1*Гц*1*мкФ);")).ToStdString();
    ASSERT_TRUE(s == "1(МОм)") << s;
    s = parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 1}, parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(1)/(Гц*мкФ);")).ToStdString();
    ASSERT_TRUE(s == "1(МОм)") << s;
}

TEST_F(CalcTestRational, units23)
{
    auto exports = std::make_shared<yutovo_calculator::Export>();

    Unit meter_unit(U"meter");
    Unit foot_unit(U"foot");
    Rational meter_value(U"328084/100000");
    meter_value.unit = foot_unit;
    Rational foot_value(U"3048/10000");
    foot_value.unit = meter_unit;

    exports->AddUnit<Rational>(CustomUnit<Rational>(LogicalId{0, 0, 0, 0, 1}, U"meter", U"SI", meter_value, false));
    exports->AddUnit<Rational>(CustomUnit<Rational>(LogicalId{0, 0, 0, 0, 2}, U"foot", U"SI", foot_value, false));

    yutovo_calculator::ParserContext parser_context;
    parser_context.Init(1000);
    parser_context.exports = exports;
    auto r = parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"5meter;", &parser_context);
    EXPECT_NO_THROW(parser.GetSuitableUnit(LogicalId{0, 0, 0, 0, 3}, r).ToStdString());
}

TEST_F(CalcTestRational, compare1)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"(0<(2/3));").ToStdString();
    ASSERT_TRUE(s == "1") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"(2/3==2/3);").ToStdString();
    ASSERT_TRUE(s == "1") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"(4/3<>4/3);").ToStdString();
    ASSERT_TRUE(s == "0") << s;
}

TEST_F(CalcTestRational, compare2)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 0}, U"a=5/2;");
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(a<10);").ToStdString();
    ASSERT_TRUE(s == "1") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(a>10);").ToStdString();
    ASSERT_TRUE(s == "0") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(a==5/2);").ToStdString();
    ASSERT_TRUE(s == "1") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(a<>10);").ToStdString();
    ASSERT_TRUE(s == "1") << s;
}
TEST_F(CalcTestRational, sum1)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(i=0,(i<=10),i=i+1,t=0,t=t+1);").ToStdString();
    ASSERT_TRUE(s == "11") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(i=1,(i<=10),i=i+1,t=0,t=t+i);").ToStdString();
    ASSERT_TRUE(s == "55") << s;
}

TEST_F(CalcTestRational, sum2)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(i=0,(i<=10),i=i+1,t1=0,t1=t1+loop(j=1,(j<=10),j=j+1,t2=0,t2=t2+i/j));").ToStdString();
    ASSERT_TRUE(s == "81191/504") << s;
}

TEST_F(CalcTestRational, prod1)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(i=0,(i<=10),i=i+1,t=1,t=t*2/3);").ToStdString();
    ASSERT_TRUE(s == "2048/177147") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(i=1,(i<=10),i=i+1,t=1,t=t*i/5);").ToStdString();
    ASSERT_TRUE(s == "145152/390625") << s;
}

TEST_F(CalcTestRational, money1)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"1₽;", 3);
    ASSERT_TRUE(r.ToStdString() == "1(₽)") << r.ToStdString();
    r = parser.GetSuitableUnit(LogicalId{0, 0, 1}, parser.Parse(LogicalId{0, 0, 1}, U"1kop;", 3));
    ASSERT_TRUE(r.ToStdString() == "1(kop)") << r.ToStdString();

    r = parser.Parse(LogicalId{0, 0, 1}, U"1$;", 3);
    ASSERT_TRUE(r.ToStdString() == "1($)") << r.ToStdString();
    r = parser.GetSuitableUnit(LogicalId{0, 0, 1}, parser.Parse(LogicalId{0, 0, 1}, U"1cent;", 3));
    ASSERT_TRUE(r.ToStdString() == "1(¢)") << r.ToStdString();
    r = parser.GetSuitableUnit(LogicalId{0, 0, 1}, parser.Parse(LogicalId{0, 0, 1}, U"33¢;", 3));
    ASSERT_TRUE(r.ToStdString() == "33(¢)") << r.ToStdString();

    r = parser.Parse(LogicalId{0, 0, 1}, U"1€;", 3);
    ASSERT_TRUE(r.ToStdString() == "1(€)") << r.ToStdString();
    r = parser.GetSuitableUnit(LogicalId{0, 0, 1}, parser.Parse(LogicalId{0, 0, 1}, U"1eurocent;", 3));
    ASSERT_TRUE(r.ToStdString() == "1(ct)") << r.ToStdString();
}

TEST_F(CalcTestRational, money2)
{
    parser.SetLocale(Language::Russian);

    auto r = parser.Parse(LogicalId{0, 0, 1}, U"1₽;", 3);
    ASSERT_TRUE(r.ToStdString() == "1(₽)") << r.ToStdString();
    r = parser.GetSuitableUnit(LogicalId{0, 0, 1}, parser.Parse(LogicalId{0, 0, 1}, U"1коп;", 3));
    ASSERT_TRUE(r.ToStdString() == "1(коп)") << r.ToStdString();

    r = parser.Parse(LogicalId{0, 0, 1}, U"1$;", 3);
    ASSERT_TRUE(r.ToStdString() == "1($)") << r.ToStdString();
    r = parser.GetSuitableUnit(LogicalId{0, 0, 1}, parser.Parse(LogicalId{0, 0, 1}, U"1цент;", 3));
    ASSERT_TRUE(r.ToStdString() == "1(цент)") << r.ToStdString();

    r = parser.Parse(LogicalId{0, 0, 1}, U"1€;", 3);
    ASSERT_TRUE(r.ToStdString() == "1(€)") << r.ToStdString();
    r = parser.GetSuitableUnit(LogicalId{0, 0, 1}, parser.Parse(LogicalId{0, 0, 1}, U"1евроцент;", 3));
    ASSERT_TRUE(r.ToStdString() == "1(евроцент)") << r.ToStdString();
}

TEST_F(CalcTestRational, money3)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"1¥;", 3);
    ASSERT_TRUE(r.ToStdString() == "1(¥)") << r.ToStdString();
    r = parser.Parse(LogicalId{0, 0, 1}, U"1yuan;", 3);
    ASSERT_TRUE(r.ToStdString() == "1(¥)") << r.ToStdString();

    r = parser.Parse(LogicalId{0, 0, 1}, U"1₹;", 3);
    ASSERT_TRUE(r.ToStdString() == "1(₹)") << r.ToStdString();
    r = parser.Parse(LogicalId{0, 0, 1}, U"1rupee;", 3);
    ASSERT_TRUE(r.ToStdString() == "1(₹)") << r.ToStdString();

    r = parser.Parse(LogicalId{0, 0, 1}, U"1R$;", 3);
    ASSERT_TRUE(r.ToStdString() == "1(R$)") << r.ToStdString();
    r = parser.Parse(LogicalId{0, 0, 1}, U"1real;", 3);
    ASSERT_TRUE(r.ToStdString() == "1(R$)") << r.ToStdString();
}

TEST_F(CalcTestRational, user_functions1)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=5;");
    parser.Parse(LogicalId{0, 0, 2}, U"f(x)=x+5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 3}, U"f(2);") == parser.Parse(LogicalId{0, 0, 3}, U"7;")) << 
        parser.Parse(LogicalId{0, 0, 3}, U"f(2);").ToStdString();
}

TEST_F(CalcTestRational, user_functions2)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"f(2);") == parser.Parse(LogicalId{0, 0, 2}, U"2;")) << 
        parser.Parse(LogicalId{0, 0, 2}, U"f(2);").ToStdString();
    parser.Parse(LogicalId{0, 0, 3}, U"f(x)=x+7;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"f(2);") == parser.Parse(LogicalId{0, 0, 4}, U"9;")) << 
        parser.Parse(LogicalId{0, 0, 4}, U"f(2);").ToStdString();
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"f(3);") == parser.Parse(LogicalId{0, 0, 2}, U"3;")) << 
        parser.Parse(LogicalId{0, 0, 2}, U"f(3);").ToStdString();
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"f(3);") == parser.Parse(LogicalId{0, 0, 4}, U"10;")) << 
        parser.Parse(LogicalId{0, 0, 4}, U"f(10);").ToStdString();
}

TEST_F(CalcTestRational, user_functions3)
{
    std::vector<std::u32string> dependencies;
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x*2;");
    parser.Parse(LogicalId{0, 0, 2}, U"f(5);", &dependencies);
    ASSERT_TRUE(std::find(dependencies.begin(), dependencies.end(), U"f") != dependencies.end());
}

TEST_F(CalcTestRational, user_functions4)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x;");
    Rational res = parser.Parse(LogicalId{0, 0, 2}, U"f(5);");
    ASSERT_TRUE(res.ToStdString() == "5") << res.ToStdString();

    parser.Parse(LogicalId{0, 0, 3}, U"f(x,y)=x+y;");
    res = parser.Parse(LogicalId{0, 0, 4}, U"f(5,4);");
    ASSERT_TRUE(res.ToStdString() == "9") << res.ToStdString();
}

TEST_F(CalcTestRational, user_functions5)
{
    parser.Parse(LogicalId{0, 0, 5}, U"p(a)=a+1;");
    parser.Parse(LogicalId{0, 0, 1}, U"p(a)=a+5;");
    Rational res = parser.Parse(LogicalId{0, 0, 6}, U"p(5);");
    ASSERT_TRUE(res.ToStdString() == "6") << res.ToStdString();
    res = parser.Parse(LogicalId{0, 0, 2}, U"p(5);");
    ASSERT_TRUE(res.ToStdString() == "10") << res.ToStdString();
}

TEST_F(CalcTestRational, user_functions6)
{
    parser.Parse(LogicalId{0, 0, 8}, U"p(a)=a+15;");
    Rational res = parser.Parse(LogicalId{0, 0, 9}, U"p(5);");
    ASSERT_TRUE(res.ToStdString() == "20") << res.ToStdString();
    parser.Parse(LogicalId{0, 0, 5}, U"p(a)=a+1;");
    parser.Parse(LogicalId{0, 0, 1}, U"p(a)=a+5;");
    res = parser.Parse(LogicalId{0, 0, 2}, U"p(5);");
    ASSERT_TRUE(res.ToStdString() == "10") << res.ToStdString();
    res = parser.Parse(LogicalId{0, 0, 6}, U"p(5);");
    ASSERT_TRUE(res.ToStdString() == "6") << res.ToStdString();
    res = parser.Parse(LogicalId{0, 0, 2}, U"p(5);");
    ASSERT_TRUE(res.ToStdString() == "10") << res.ToStdString();
    res = parser.Parse(LogicalId{0, 0, 9}, U"p(5);");
    ASSERT_TRUE(res.ToStdString() == "20") << res.ToStdString();
    parser.Parse(LogicalId{0, 0, 12}, U"p(a)=a+25;");
    res = parser.Parse(LogicalId{0, 0, 9}, U"p(5);");
    ASSERT_TRUE(res.ToStdString() == "20") << res.ToStdString();
}

TEST_F(CalcTestRational, user_functions7)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x,y)=pow(x,y);");
    Rational res = parser.Parse(LogicalId{0, 0, 2}, U"f(5,2);");
    ASSERT_TRUE(res.ToStdString() == "25") << res.ToStdString();
}

TEST_F(CalcTestRational, user_functions8)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x,y)=(x)/(y);");
    Rational res = parser.Parse(LogicalId{0, 0, 2}, U"f(8,2);");
    ASSERT_TRUE(res.ToStdString() == "4") << res.ToStdString();
}

TEST_F(CalcTestRational, list1)
{
    parser.Parse(LogicalId{0, 0, 1}, U"materials=\"cuprum\",\"ferrum\";");
    parser.Parse(LogicalId{0, 0, 2}, U"ro{cuprum}=1.68*pow(10,-8);");
    parser.Parse(LogicalId{0, 0, 3}, U"ro{ferrum}=9.71*pow(10,-8);");
    parser.Parse(LogicalId{0, 0, 4}, U"material{materials}=\"cuprum\";");
    auto r = parser.Parse(LogicalId{0, 0, 5}, U"ro{material};");
    ASSERT_TRUE(r.ToStdString() == "21/1250000000") << r.ToStdString();

    parser.Parse(LogicalId{0, 0, 4}, U"material{materials}=\"ferrum\";");
    r = parser.Parse(LogicalId{0, 0, 5}, U"ro{material};");
    ASSERT_TRUE(r.ToStdString() == "971/10000000000") << r.ToStdString();
}

TEST_F(CalcTestRational, list2)
{
    yutovo_calculator::ParserContext parser_context;
    parser_context.include_document = true;
    parser.Parse(LogicalId{0, 0, 1}, U"materials=\"cuprum\",\"ferrum\";", &parser_context);
    parser.Parse(LogicalId{0, 0, 2}, U"ro{cuprum}=1.68*pow(10,-8)*(Ohm*m);", &parser_context);
    parser.Parse(LogicalId{0, 0, 3}, U"ro{ferrum}=9.71*pow(10,-8)*(Ohm*m);", &parser_context);
    parser.Parse(LogicalId{0, 0, 4}, U"length=1m;", &parser_context);
    parser.Parse(LogicalId{0, 0, 5}, U"section=1*pow(mm,2);", &parser_context);
    parser.Parse(LogicalId{0, 0, 6}, U"material{materials}=\"ferrum\";", &parser_context);
    parser.Parse(LogicalId{0, 0, 7}, U"resistance=ro{material}*(length)/(section);", &parser_context);
    auto r = parser.GetSuitableUnit(LogicalId{0, 0, 8}, parser.Parse(LogicalId{0, 0, 8}, U"resistance;", &parser_context));
    ASSERT_TRUE(r.ToStdString() == "97100(mcOhm)") << r.ToStdString();

    parser_context.include_document = false;
    parser.Parse(LogicalId{0, 1, 1}, U"length=4m;", &parser_context);
    parser.Parse(LogicalId{0, 1, 2}, U"section=2*pow(mm,2);", &parser_context);
    parser.Parse(LogicalId{0, 1, 3}, U"material{materials}=\"cuprum\";", &parser_context);
    r = parser.GetSuitableUnit(LogicalId{0, 1, 4}, parser.Parse(LogicalId{0, 1, 4}, U"resistance;", &parser_context));
    ASSERT_TRUE(r.ToStdString() == "168/5(mOhm)") << r.ToStdString();
}

TEST_F(CalcTestRational, pow_int)
{
    Rational base(2);
    Rational res = pow(base, 0);
    ASSERT_TRUE(res.ToStdString() == "1") << res.ToStdString();
    res = pow(base, 1);
    ASSERT_TRUE(res.ToStdString() == "2") << res.ToStdString();
    res = pow(base, 2);
    ASSERT_TRUE(res.ToStdString() == "4") << res.ToStdString();
    res = pow(base, 3);
    ASSERT_TRUE(res.ToStdString() == "8") << res.ToStdString();
    res = pow(base, 10);
    ASSERT_TRUE(res.ToStdString() == "1024") << res.ToStdString();
    res = pow(base, -1);
    ASSERT_TRUE(res.ToStdString() == "1/2") << res.ToStdString();
    res = pow(base, -2);
    ASSERT_TRUE(res.ToStdString() == "1/4") << res.ToStdString();
    res = pow(base, -3);
    ASSERT_TRUE(res.ToStdString() == "1/8") << res.ToStdString();
}

TEST_F(CalcTestRational, min_max1)
{
    auto res = parser.Parse(LogicalId{0, 0, 1}, U"min(1/2,1/3);", 3);
    ASSERT_TRUE(res.ToStdString() == "1/3") << res.ToStdString();

    res = parser.Parse(LogicalId{0, 0, 1}, U"max(1/2,1/3);", 3);
    ASSERT_TRUE(res.ToStdString() == "1/2") << res.ToStdString();

    res = parser.Parse(LogicalId{0, 0, 1}, U"min(-3/2,-1/2);", 3);
    ASSERT_TRUE(res.ToStdString() == "-3/2") << res.ToStdString();

    res = parser.Parse(LogicalId{0, 0, 1}, U"max(-3/2,-1/2);", 3);
    ASSERT_TRUE(res.ToStdString() == "-1/2") << res.ToStdString();
}

TEST_F(CalcTestRational, definite_integral1)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,1,x,x);");
    ASSERT_TRUE(res.ToStdString() == "1/2") << res.ToStdString();
}

TEST_F(CalcTestRational, definite_integral2)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,1,pow(x,2),x);");
    ASSERT_TRUE(res.ToStdString() == "1/3") << res.ToStdString();
}

TEST_F(CalcTestRational, definite_integral3)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,2,pow(x,2),x);");
    ASSERT_TRUE(res.ToStdString() == "8/3") << res.ToStdString();
}

TEST_F(CalcTestRational, definite_integral4)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,1,pow(x,3),x);");
    ASSERT_TRUE(res.ToStdString() == "1/4") << res.ToStdString();
}

TEST_F(CalcTestRational, definite_integral5)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,1,pow((x+1),2),x);");
    ASSERT_TRUE(res.ToStdString() == "7/3") << res.ToStdString();
}

TEST_F(CalcTestRational, definite_integral6)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,10,x,x);");
    ASSERT_TRUE(res.ToStdString() == "50") << res.ToStdString();
}

TEST_F(CalcTestRational, definite_integral7)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,1,2*pow(x,2)+3*x+1,x);");
    ASSERT_TRUE(res.ToStdString() == "19/6") << res.ToStdString();
}

TEST_F(CalcTestRational, derivative_at_point1)
{
    Rational res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x*x, [x=3]);");
    ASSERT_TRUE(res == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"6;")) << res.ToStdString();
}

TEST_F(CalcTestRational, derivative_at_point2)
{
    Rational res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,3), [x=2]);");
    ASSERT_TRUE(res == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"12;")) << res.ToStdString();
}

TEST_F(CalcTestRational, derivative_at_point3)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x)=x*x;");
    Rational res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x), [x=2]);");
    ASSERT_TRUE(res == parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"4;")) << res.ToStdString();
}

TEST_F(CalcTestRational, derivative_at_point_multi1)
{
    Rational res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x*y, [x=2, y=3]);");
    ASSERT_TRUE(res == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"3;")) << res.ToStdString();
}

TEST_F(CalcTestRational, derivative_at_point_multi2)
{
    Rational res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x*y, [y=3, x=2]);");
    ASSERT_TRUE(res == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2;")) << res.ToStdString();
}

TEST_F(CalcTestRational, derivative_at_point_multi3)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x,y)=pow(x,2)*y;");
    Rational res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x,y), [x=2, y=3]);");
    ASSERT_TRUE(res == parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"12;")) << res.ToStdString();
}

}
