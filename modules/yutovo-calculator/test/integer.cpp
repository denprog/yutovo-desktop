/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include <chrono>

namespace yutovo_calc_test
{

using namespace yutovo_calculator;
using namespace std::chrono_literals;

TEST_F(CalcTestInteger, integers1)
{
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1+1;") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2;"));
}

TEST_F(CalcTestInteger, integers2)
{
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2/3;") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2/3;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-(2/3);") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-(2/3);");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-1/3;") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-1/3;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(-1)/3;") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(-1)/3;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(-1)/(-3);") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(-1)/(-3);");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1/(-3);") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1/(-3);");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-2/3;") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-2/3;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-2/(-3);") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-2/(-3);");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(-2)/3;") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(-2)/3;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(-2)/(-3);") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(-2)/(-3);");
}

TEST_F(CalcTestInteger, integers3)
{
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"4/3;") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"4/3;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-(4/3);") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-1;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-(4/3);");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-4/3;") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-1;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-4/3;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(-4)/3;") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-1;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(-4)/3;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(-4)/(-3);") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(-4)/(-3);");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"4/(-3);") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-1;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"4/(-3);");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-7/3;") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-2;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-7/3;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-7/(-3);") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-7/(-3);");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(-7)/3;") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-2;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(-7)/3;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(-7)/(-3);") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(-7)/(-3);");
}

TEST_F(CalcTestInteger, division_int_operands)
{
    // Non-exact division must use truncated division, not mpz_divexact.
    Integer a(7);
    Integer b = a / 3;
    ASSERT_TRUE(b == 2) << b.ToStdString(10);

    Integer c = 7 / Integer(3);
    ASSERT_TRUE(c == 2) << c.ToStdString(10);

    Integer d(7);
    d /= Integer(3);
    ASSERT_TRUE(d == 2) << d.ToStdString(10);

    // Exact division must still work.
    Integer e(6);
    Integer f = e / 3;
    ASSERT_TRUE(f == 2) << f.ToStdString(10);

    Integer g = 6 / Integer(3);
    ASSERT_TRUE(g == 2) << g.ToStdString(10);

    // Negative operands.
    ASSERT_TRUE(Integer(-7) / 3 == -2) << (Integer(-7) / 3).ToStdString(10);
    ASSERT_TRUE(Integer(7) / -3 == -2) << (Integer(7) / -3).ToStdString(10);
    ASSERT_TRUE(Integer(-7) / -3 == 2) << (Integer(-7) / -3).ToStdString(10);
    ASSERT_TRUE(-7 / Integer(3) == -2) << (-7 / Integer(3)).ToStdString(10);
    ASSERT_TRUE(7 / Integer(-3) == -2) << (7 / Integer(-3)).ToStdString(10);
    ASSERT_TRUE(-7 / Integer(-3) == 2) << (-7 / Integer(-3)).ToStdString(10);

    // Dividend smaller than divisor.
    ASSERT_TRUE(Integer(2) / 5 == 0) << (Integer(2) / 5).ToStdString(10);
    ASSERT_TRUE(2 / Integer(5) == 0) << (2 / Integer(5)).ToStdString(10);
    ASSERT_TRUE(Integer(-2) / 5 == 0) << (Integer(-2) / 5).ToStdString(10);
    ASSERT_TRUE(-2 / Integer(5) == 0) << (-2 / Integer(5)).ToStdString(10);

    // Division by one.
    ASSERT_TRUE(Integer(123456) / 1 == 123456) << (Integer(123456) / 1).ToStdString(10);
    ASSERT_TRUE(123456 / Integer(1) == 123456) << (123456 / Integer(1)).ToStdString(10);

    // Division by zero throws.
    ASSERT_THROW(Integer(5) / 0, MathException);
    ASSERT_THROW(5 / Integer(0), MathException);
}

TEST_F(CalcTestInteger, integers4)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"+5;");
    ASSERT_TRUE(res.ToString(10) == U"5") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-5;");
    ASSERT_TRUE(res.ToString(10) == U"-5") << res.ToStdString(10);
}

TEST_F(CalcTestInteger, integers5)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"5(4*2);");
    ASSERT_TRUE(res.ToString(10) == U"40") << res.ToStdString(10);
}

TEST_F(CalcTestInteger, logical1)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"¬5;");
    ASSERT_TRUE(res.ToString(10) == U"2") << res.ToStdString(10);
}

TEST_F(CalcTestInteger, logical2)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"10∧12;");
    ASSERT_TRUE(res.ToString(10) == U"8") << res.ToStdString(10);
}

TEST_F(CalcTestInteger, logical3)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"10∨12;");
    ASSERT_TRUE(res.ToString(10) == U"14") << res.ToStdString(10);
}

TEST_F(CalcTestInteger, logical4)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"10⊕12;");
    ASSERT_TRUE(res.ToString(10) == U"6") << res.ToStdString(10);
}

TEST_F(CalcTestInteger, power1)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(2,3);");
    ASSERT_TRUE(res.ToString(10) == U"8") << res.ToStdString(10);
}

TEST_F(CalcTestInteger, variables1)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"a=5;");
    parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"a=5+6;");
    parser.Parse(LogicalId{0, 0, 0, 1, 0}, U"b=a+7;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 1, 1, 0}, U"b+3;") == parser.Parse(LogicalId{0, 0, 1, 1, 0}, U"21;"));
}

TEST_F(CalcTestInteger, variables2)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"п=55;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"п;") == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"55;")) << 
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"п;").ToStdString();
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"пр=55;");
    Integer r = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"пр+5;");
    ASSERT_TRUE(r == parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"60;")) << r.ToStdString();
    parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"fп=55;");
    r = parser.Parse(LogicalId{0, 0, 0, 0, 4}, U"fп+5;");
    ASSERT_TRUE(r == parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"60;")) << r.ToStdString();
}

TEST_F(CalcTestInteger, variables3)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"a=5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 1, 1, 0}, U"3a;") == parser.Parse(LogicalId{0, 0, 1, 1, 0}, U"15;")) << 
        parser.Parse(LogicalId{0, 0, 1, 1, 0}, U"3a;").ToStdString();
}

TEST_F(CalcTestInteger, variables4)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"a=5;");
    parser.Parse(LogicalId{0, 0, 0, 1, 1}, U"b=6;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2, 1, 0}, U"3a+4b+3;") == parser.Parse(LogicalId{0, 0, 2, 1, 0}, U"42;")) << 
        parser.Parse(LogicalId{0, 0, 2, 1, 0}, U"3a+4b+3;").ToStdString();
}

TEST_F(CalcTestInteger, variables5)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a{1}=5;");
    parser.Parse(LogicalId{0, 0, 1}, U"a{1}=6;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"a{1};").ToStdString() == parser.Parse(LogicalId{0, 0, 2}, U"6;").ToStdString()) << 
        parser.Parse(LogicalId{0, 0, 2}, U"a{1};").ToStdString();
    parser.Parse(LogicalId{0, 0, 3}, U"a{2}=7;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"a{1}+a{2};").ToStdString() == parser.Parse(LogicalId{0, 0, 4}, U"13;").ToStdString()) << 
        parser.Parse(LogicalId{0, 0, 4}, U"a{1}+a{2};").ToStdString();
}

TEST_F(CalcTestInteger, functions1)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x)=x+5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 0, 2, 0}, U"f(2);") == parser.Parse(LogicalId{0, 0, 0, 0, 2, 0}, U"7;"));
    parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"a=5+6;");
    parser.Parse(LogicalId{0, 0, 0, 2, 3, 0}, U"f(t)=a*t;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 0, 2, 3, 1}, U"f(2);") == parser.Parse(LogicalId{0, 0, 0, 2, 3, 1}, U"22;")) << 
        parser.Parse(LogicalId{0, 0, 0, 2, 3, 1}, U"f(2);").ToStdString();
}

TEST_F(CalcTestInteger, functions2)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"3pow(2,3);", 3);
    ASSERT_TRUE(r.ToStdString() == "24") << r.ToStdString();
}

TEST_F(CalcTestInteger, functions3)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"a=5;");
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"pow(2,3)a;", 3);
    ASSERT_TRUE(r.ToStdString() == "40") << r.ToStdString();
}

TEST_F(CalcTestInteger, functions4)
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

TEST_F(CalcTestInteger, functions5)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(0, 2);");
    ASSERT_TRUE(res.ToStdString() == "0") << res.ToStdString();
}

TEST_F(CalcTestInteger, functions6)
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

TEST_F(CalcTestInteger, functions7)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"reminder(3, 2);");
    ASSERT_TRUE(res.ToStdString() == "1") << res.ToStdString();
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"reminder(35, 3);");
    ASSERT_TRUE(res.ToStdString() == "2") << res.ToStdString();
}

TEST_F(CalcTestInteger, functions8)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(1, -5);");
    ASSERT_TRUE(res.ToStdString() == "1") << res.ToStdString();

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(-1, -4);");
    ASSERT_TRUE(res.ToStdString() == "1") << res.ToStdString();

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(-1, -3);");
    ASSERT_TRUE(res.ToStdString() == "-1") << res.ToStdString();

    try
    {
        parser.Parse(LogicalId{0, 0, 1}, U"pow(2, -1);");
    }
    catch (yutovo_calculator::MathException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 1}) && ex.ex_id == ParserExceptionCode::Overflow && ex.pos == 0) << ex.ex_id;
        return;
    }
    ASSERT_FALSE(true);
}

TEST_F(CalcTestInteger, symbols1)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"•"), yutovo_calculator::SyntaxException) << parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"•").ToStdString();
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"‣"), yutovo_calculator::SyntaxException) << parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"‣").ToStdString();
}

TEST_F(CalcTestInteger, str1)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"d33;"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"33ttt;"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestInteger, error1)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"123"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"123+(24/2)"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"123+3;45"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestInteger, errors2)
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

TEST_F(CalcTestInteger, errors3)
{
    try
    {
        parser.Parse(LogicalId{0, 0, 1}, U"d23+5;");
    }
    catch (yutovo_calculator::SyntaxException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 1}) && ex.ex_id == ParserExceptionCode::UnknownIdentifier && ex.pos == 0 && ex.size == 3) << ex.ex_id;
        return;
    }
    ASSERT_FALSE(true);
}

TEST_F(CalcTestInteger, errors4)
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

TEST_F(CalcTestInteger, errors5)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"(1)/(0);"), yutovo_calculator::MathException);
}

TEST_F(CalcTestInteger, errors6)
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

TEST_F(CalcTestInteger, notation1)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"123;");
    ASSERT_TRUE(res.ToString(2) == U"1111011") << res.ToStdString(2);
    ASSERT_TRUE(res.ToString(8) == U"173") << res.ToStdString(8);
    ASSERT_TRUE(res.ToString(10) == U"123") << res.ToStdString(10);
    ASSERT_TRUE(res.ToString(16) == U"7b") << res.ToStdString(16);
}

TEST_F(CalcTestInteger, notation2)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"bin[1010];");
    ASSERT_TRUE(res.ToString(2) == U"1010") << res.ToStdString(2);
    ASSERT_TRUE(res.ToString(10) == U"10") << res.ToStdString(10);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"bin[123];"), yutovo_calculator::MathException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"bin[101+111];"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestInteger, notation3)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"oct[2345];");
    ASSERT_TRUE(res.ToString(10) == U"1253") << res.ToStdString(10);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"oct[985];"), yutovo_calculator::MathException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"oct[10/345];"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestInteger, notation4)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"dec[4569];");
    ASSERT_TRUE(res.ToString(10) == U"4569") << res.ToStdString(10);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"dec[3ef];"), yutovo_calculator::MathException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"dec[101+35];"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestInteger, notation5)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"hex[56fe];");
    ASSERT_TRUE(res.ToString(10) == U"22270") << res.ToStdString(10);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"hex[dt45];"), yutovo_calculator::MathException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"hex[101+f1];"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestInteger, notation6)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1010;", Notation::Binary);
    ASSERT_TRUE(res.ToString(10) == U"10") << res.ToStdString(10);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"123;", Notation::Binary), yutovo_calculator::MathException);
}

TEST_F(CalcTestInteger, notation7)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2345;", Notation::Octal);
    ASSERT_TRUE(res.ToString(10) == U"1253") << res.ToStdString(10);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"128;", Notation::Octal), yutovo_calculator::MathException);
}

TEST_F(CalcTestInteger, notation8)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1010;", Notation::Decimal);
    ASSERT_TRUE(res.ToString(10) == U"1010") << res.ToStdString(10);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"123f;", Notation::Decimal), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestInteger, notation9)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"56fe;", Notation::Hexadecimal);
    ASSERT_TRUE(res.ToString(10) == U"22270") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"56FE;", Notation::Hexadecimal);
    ASSERT_TRUE(res.ToString(10) == U"22270") << res.ToStdString(10);
}

TEST_F(CalcTestInteger, notation10)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"56fe+dec[10];", Notation::Hexadecimal);
    ASSERT_TRUE(res.ToString(10) == U"22280") << res.ToStdString(10);
}

TEST_F(CalcTestInteger, notation11)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"hex[da]+1011;", Notation::Binary);
    ASSERT_TRUE(res.ToString(10) == U"229") << res.ToStdString(10);
}

TEST_F(CalcTestInteger, notation12)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"hex[da]+1011;", Notation::Decimal);
    ASSERT_TRUE(res.ToString(10) == U"1229") << res.ToStdString(10);
}

TEST_F(CalcTestInteger, notation13)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"da;", Notation::Hexadecimal);
    ASSERT_TRUE(res.ToString(10) == U"218") << res.ToStdString(10);
}

TEST_F(CalcTestInteger, builtin_functions1)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1!;");
    ASSERT_TRUE(res.ToString(10) == U"1") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"5!;");
    ASSERT_TRUE(res.ToString(10) == U"120") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(5+2)!;");
    ASSERT_TRUE(res.ToString(10) == U"5040") << res.ToStdString(10);
}

TEST_F(CalcTestInteger, builtin_functions2)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"a=2;");
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"a!;");
    ASSERT_TRUE(res.ToString(10) == U"2") << res.ToStdString(10);
}

TEST_F(CalcTestInteger, builtin_functions3)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"abs(-2);", 3);
    ASSERT_TRUE(r.ToStdString(10) == "2") << r.ToStdString(10);
    r = parser.Parse(LogicalId{0, 0, 1}, U"abs(2);", 3);
    ASSERT_TRUE(r.ToStdString(10) == "2") << r.ToStdString(10);
}

TEST_F(CalcTestInteger, compare1)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"(0<10);").ToStdString();
    ASSERT_TRUE(s == "1") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"(10==10);").ToStdString();
    ASSERT_TRUE(s == "1") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"(10<>10);").ToStdString();
    ASSERT_TRUE(s == "0") << s;
}

TEST_F(CalcTestInteger, compare2)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 0}, U"a=5;");
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(a<10);").ToStdString();
    ASSERT_TRUE(s == "1") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(a>10);").ToStdString();
    ASSERT_TRUE(s == "0") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(a==10);").ToStdString();
    ASSERT_TRUE(s == "0") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(a<>10);").ToStdString();
    ASSERT_TRUE(s == "1") << s;
}

TEST_F(CalcTestInteger, sum1)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(i=0,(i<=10),i=i+1,t=0,t=t+1);").ToStdString();
    ASSERT_TRUE(s == "11") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(i=1,(i<=10),i=i+1,t=0,t=t+i);").ToStdString();
    ASSERT_TRUE(s == "55") << s;
}

TEST_F(CalcTestInteger, sum2)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(i=0,(i<=10),i=i+1,t1=0,t1=t1+loop(j=0,(j<=20),j=j+1,t2=0,t2=t2+i+j));").ToStdString();
    ASSERT_TRUE(s == "3465") << s;
}

TEST_F(CalcTestInteger, prod1)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(i=0,(i<=10),i=i+1,t=1,t=t*2);").ToStdString();
    ASSERT_TRUE(s == "2048") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(i=1,(i<=10),i=i+1,t=1,t=t*i);").ToStdString();
    ASSERT_TRUE(s == "3628800") << s;
}

TEST_F(CalcTestInteger, user_functions1)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=5;");
    parser.Parse(LogicalId{0, 0, 2}, U"f(x)=x+5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 3}, U"f(2);") == parser.Parse(LogicalId{0, 0, 3}, U"7;")) << 
        parser.Parse(LogicalId{0, 0, 3}, U"f(2);").ToStdString();
}

TEST_F(CalcTestInteger, user_functions2)
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

TEST_F(CalcTestInteger, user_functions3)
{
    std::vector<std::u32string> dependencies;
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x*2;");
    parser.Parse(LogicalId{0, 0, 2}, U"f(5);", &dependencies);
    ASSERT_TRUE(std::find(dependencies.begin(), dependencies.end(), U"f") != dependencies.end());
}

TEST_F(CalcTestInteger, user_functions4)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x;");
    Integer res = parser.Parse(LogicalId{0, 0, 2}, U"f(5);");
    ASSERT_TRUE(res.ToStdString() == "5") << res.ToStdString();

    parser.Parse(LogicalId{0, 0, 3}, U"f(x,y)=x+y;");
    res = parser.Parse(LogicalId{0, 0, 4}, U"f(5,4);");
    ASSERT_TRUE(res.ToStdString() == "9") << res.ToStdString();
}

TEST_F(CalcTestInteger, user_functions5)
{
    parser.Parse(LogicalId{0, 0, 5}, U"p(a)=a+1;");
    parser.Parse(LogicalId{0, 0, 1}, U"p(a)=a+5;");
    Integer res = parser.Parse(LogicalId{0, 0, 6}, U"p(5);");
    ASSERT_TRUE(res.ToStdString() == "6") << res.ToStdString();
    res = parser.Parse(LogicalId{0, 0, 2}, U"p(5);");
    ASSERT_TRUE(res.ToStdString() == "10") << res.ToStdString();
}

TEST_F(CalcTestInteger, user_functions6)
{
    parser.Parse(LogicalId{0, 0, 8}, U"p(a)=a+15;");
    Integer res = parser.Parse(LogicalId{0, 0, 9}, U"p(5);");
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

TEST_F(CalcTestInteger, user_functions7)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x,y)=pow(x,y);");
    Integer res = parser.Parse(LogicalId{0, 0, 2}, U"f(5,2);");
    ASSERT_TRUE(res.ToStdString() == "25") << res.ToStdString();
}

TEST_F(CalcTestInteger, user_functions8)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x,y)=(x)/(y);");
    Integer res = parser.Parse(LogicalId{0, 0, 2}, U"f(8,2);");
    ASSERT_TRUE(res.ToStdString() == "4") << res.ToStdString();
}

TEST(UtilsTest, ElementIdFromString1)
{
    auto id = ElementIdFromString("1,2,3");
    ASSERT_EQ(id.size(), 3u);
    ASSERT_EQ(id[0], 1);
    ASSERT_EQ(id[1], 2);
    ASSERT_EQ(id[2], 3);
}

TEST(UtilsTest, ElementIdFromString2)
{
    auto id = ElementIdFromString("");
    ASSERT_TRUE(id.empty());
}

TEST(UtilsTest, ElementIdFromString3)
{
    EXPECT_THROW(ElementIdFromString("1,a,3"), yutovo_calculator::ParserException);
}

TEST(UtilsTest, ElementIdFromString4)
{
    EXPECT_THROW(ElementIdFromString("1,2,abc"), yutovo_calculator::ParserException);
}

TEST(UtilsTest, LogicalIdFromString5)
{
    auto id = LogicalIdFromString("0,1,2,3");
    ASSERT_EQ(id.size(), 4u);
    ASSERT_EQ(id[0], 0);
    ASSERT_EQ(id[1], 1);
    ASSERT_EQ(id[2], 2);
    ASSERT_EQ(id[3], 3);
}

TEST(UtilsTest, LogicalIdFromString6)
{
    auto id = LogicalIdFromString("");
    ASSERT_TRUE(id.empty());
}

TEST(UtilsTest, LogicalIdFromString7)
{
    EXPECT_THROW(LogicalIdFromString("0,1,abc"), yutovo_calculator::ParserException);
}

TEST_F(CalcTestInteger, fact1)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0!;");
    ASSERT_TRUE(res.ToString(10) == U"1") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"10!;");
    ASSERT_TRUE(res.ToString(10) == U"3628800") << res.ToStdString(10);
}

TEST_F(CalcTestInteger, fact2)
{
    yutovo_calculator::ParserContext parser_context;
    parser_context.Init(100);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1000000!;", &parser_context),
        yutovo_calculator::TimeExceedException);
}

TEST_F(CalcTestInteger, min_max1)
{
    Integer res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"min(2,3);");
    ASSERT_TRUE(res.ToString(10) == U"2") << res.ToStdString(10);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"max(2,3);");
    ASSERT_TRUE(res.ToString(10) == U"3") << res.ToStdString(10);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"min(-5,-1);");
    ASSERT_TRUE(res.ToString(10) == U"-5") << res.ToStdString(10);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"max(-5,-1);");
    ASSERT_TRUE(res.ToString(10) == U"-1") << res.ToStdString(10);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"min(7,7);");
    ASSERT_TRUE(res.ToString(10) == U"7") << res.ToStdString(10);
}

}
