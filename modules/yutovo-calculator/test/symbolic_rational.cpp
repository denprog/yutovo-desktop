/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"

namespace yutovo_calc_test
{

using namespace yutovo_calculator;

TEST_F(CalcTestSymbolicRational, numbers1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1;");
    ASSERT_TRUE(res.ToStdString(0) == "1") << res.ToStdString(0);

    res = parser.Parse(LogicalId{0, 0, 1}, U"1/2;");
    ASSERT_TRUE(res.ToStdString(0) == "1/2") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, numbers2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"12345678901234567890;");
    ASSERT_TRUE(res.ToStdString(0) == "12345678901234567890") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, numbers3)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"12345678901234567890/9876543210987654321;");
    ASSERT_TRUE(res.ToStdString(0) == "137174210/109739369") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, numbers4)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2.3;");
    ASSERT_TRUE(res.ToStdString(0) == "23/10") << res.ToStdString(0);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-2.3;");
    ASSERT_TRUE(res.ToStdString(0) == "-23/10") << res.ToStdString(0);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0.12;");
    ASSERT_TRUE(res.ToStdString(0) == "3/25") << res.ToStdString(0);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U".12;");
    ASSERT_TRUE(res.ToStdString(0) == "3/25") << res.ToStdString(0);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"12.0;");
    ASSERT_TRUE(res.ToStdString(0) == "12") << res.ToStdString(0);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"54.990;");
    ASSERT_TRUE(res.ToStdString(0) == "5499/100") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, numbers5)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2.3*x;");
    ASSERT_TRUE(res.ToStdString(0) == "(23/10)*x") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, numbers6)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"."), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"..2"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1..2"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.2.3"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2.3.4"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U".1.2"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.2."), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestSymbolicRational, eval1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x;");
    ASSERT_TRUE(res.ToStdString(0) == "x") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, addition1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x + 1;");
    ASSERT_TRUE(res.ToStdString(0) == "1+x") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, subtraction1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x - 1;");
    ASSERT_TRUE(res.ToStdString(0) == "-1+x") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, multiplication1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x * 2;");
    ASSERT_TRUE(res.ToStdString(0) == "2*x") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, division1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x / 3;");
    ASSERT_TRUE(res.ToStdString(0) == "(1/3)*x") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, mixed_arithmetic1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x + 2*x;");
    ASSERT_TRUE(res.ToStdString(0) == "3*x") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, mixed_arithmetic2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2*pow(x,2) + 3*x - 5;");
    ASSERT_TRUE(res.ToStdString(0) == "-5+3*x+2*pow(x,2)") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, power1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x,2);");
    ASSERT_TRUE(res.ToStdString(0) == "pow(x,2)") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, power2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x,10);");
    ASSERT_TRUE(res.ToStdString(0) == "pow(x,10)") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, power3)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1/pow(10,100);");
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s == "1/10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000") << s;
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json == "{\"type\":46,\"elements\":[{\"type\":14,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"1\"}]},{\"type\":10,\"elements\":[]},{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\"}]}]}]}") << json;
}

TEST_F(CalcTestSymbolicRational, diff1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,2), x);");
    ASSERT_TRUE(res.ToStdString(0) == "2*x") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, indefinite_integral1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"indefinite_integral(pow(x,2), x);");
    ASSERT_TRUE(res.ToStdString(0) == "(1/3)*pow(x,3)") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, indefinite_integral2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"indefinite_integral(x*y, y);");
    ASSERT_TRUE(res.ToStdString(0) == "(1/2)*x*pow(y,2)") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, indefinite_integral_nonsymbol)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"indefinite_integral(x, 1);"), yutovo_calculator::ParserException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"indefinite_integral(x, x+1);"), yutovo_calculator::ParserException);
}

TEST_F(CalcTestSymbolicRational, definite_integral1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(2,34,3y,x);");
    ASSERT_TRUE(res.ToStdString(0) == "96*y") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, expand1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"expand(pow((x+1),2));");
    ASSERT_TRUE(res.ToStdString(0) == "1+2*x+pow(x,2)") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, subs1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(pow(x,2), x, 5);");
    ASSERT_TRUE(res.ToStdString(0) == "25") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, subs2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(pow(yy,3), yy, 5);");
    ASSERT_TRUE(res.ToStdString(0) == "125") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, subs3)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(x+y,y,2);");
    ASSERT_TRUE(res.ToStdString(0) == "2+x") << res.ToStdString(0);
    ASSERT_TRUE(res.ToJson(10) ==
        R"({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"2"},{"type":11,"symbol":"+"},{"type":8,"elements":"x"}]}]})" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, subs4)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(pow(x,2)+pow(y,2), x, 3);");
    ASSERT_TRUE(res.ToStdString(0) == "9+pow(y,2)") << res.ToStdString(0);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":8,"elements":"9"},)"
                R"({"type":11,"symbol":"+"},)"
                R"({"type":15,"elements":[)"
                    R"({"type":7,"elements":[)"
                        R"({"type":8,"elements":"y"})"
                    R"(]},)"
                    R"({"type":10,"elements":[]},)"
                    R"({"type":7,"elements":[)"
                        R"({"type":8,"elements":"2"})"
                    R"(]})"
                R"(]})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicRational, subs5)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(log(x,2),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, subs6)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(x+y,y,2);");
    ASSERT_TRUE(res.ToStdString(10) == "2+x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, subs7)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(log(2,x),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "0") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, subs8)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(log(x,2),x,2);");
    ASSERT_TRUE(res.ToStdString(10) == "1") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, definite_integral3)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,1,x*y,y);");
    ASSERT_TRUE(res.ToStdString(0) == "(1/2)*x") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, subscript1)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x{2};"), SyntaxException);
}

TEST_F(CalcTestSymbolicRational, subscript2)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x{us};"), SyntaxException);
}

TEST_F(CalcTestSymbolicRational, subscript3)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x{ab};"), SyntaxException);
}

TEST_F(CalcTestSymbolicRational, subscript4)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x{2}+y;"), SyntaxException);
}

TEST_F(CalcTestSymbolicRational, subscript5)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2x{2};"), SyntaxException);
}

TEST_F(CalcTestSymbolicRational, subscript6)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x{2});"), SyntaxException);
}

TEST_F(CalcTestSymbolicRational, infinity_syntax_error)
{
    for (size_t n = 2; n <= 5; ++n)
    {
        std::u32string expr(n, U'∞');
        expr += U';';
        ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, expr), SyntaxException) << "n=" << n;
    }
}

TEST_F(CalcTestSymbolicRational, infinity_in_identifier)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x∞;"), SyntaxException);
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"∞x;"), SyntaxException);
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ab∞;"), SyntaxException);
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"∞ab;"), SyntaxException);
}

TEST_F(CalcTestSymbolicRational, infinity_in_number)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1∞;"), SyntaxException);
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"12.3∞;"), SyntaxException);
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"∞1;"), SyntaxException);
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"∞12.3;"), SyntaxException);
}

TEST_F(CalcTestSymbolicRational, variables1)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a=2;");
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 2}, U"a*x;");
    ASSERT_TRUE(res.ToStdString(0) == "2*x") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, user_functions1)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x+1;");
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 2}, U"f(x);");
    ASSERT_TRUE(res.ToStdString(0) == "1+x") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, eval2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x+x;");
    ASSERT_TRUE(res.ToStdString(0) == "2*x") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, eval3)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"y+2*y+2;");
    ASSERT_TRUE(res.ToStdString(0) == "2+3*y") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, eval4)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"+-x;");
    ASSERT_TRUE(res.ToString(10) == U"-x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, eval5)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"+--x;");
    ASSERT_TRUE(res.ToString(10) == U"x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, eval6)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x)--2356*x;");
    ASSERT_TRUE(res.ToString(10) == U"2356*x+sin(x)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, eval7)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-sin(x)++2356*x;");
    ASSERT_TRUE(res.ToString(10) == U"2356*x-sin(x)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, eval8)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"---sin(x)++2356*x;");
    ASSERT_TRUE(res.ToString(10) == U"2356*x-sin(x)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, unary_plus1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"+x;");
    ASSERT_TRUE(res.ToStdString(0) == "x") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, unary_minus1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-x;");
    ASSERT_TRUE(res.ToStdString(0) == "-x") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, sin1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x);");
    ASSERT_TRUE(res.ToStdString(0) == "sin(x)") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, cos1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cos(x);");
    ASSERT_TRUE(res.ToStdString(0) == "cos(x)") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, log1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"log(2, 8);");
    ASSERT_TRUE(res.ToStdString(0) == "log(8)/log(2)") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, log2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"log(x,y);");
    ASSERT_TRUE(res.ToStdString(0, Language::Russian) == "ln(y)/ln(x)") << res.ToStdString(0, Language::Russian);
    ASSERT_TRUE(res.ToJson(10, Language::Russian) ==
        R"r({"type":46,"elements":[{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"ln"},{"type":19,"symbol":"("},{"type":8,"elements":"y"},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"ln"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]}]})r" ) << res.ToJson(10, Language::Russian);
}

TEST_F(CalcTestSymbolicRational, root1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"root(8, 3);");
    ASSERT_TRUE(res.ToStdString(0) == "2") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, simplify1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(x+x);");
    ASSERT_TRUE(res.ToStdString(0) == "2*x") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, sin_pi_12)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(sin(pi/12));");
    ASSERT_TRUE(res.ToStdString(0) == "(1/4)*sqrt(2)*(-1+sqrt(3))") << res.ToStdString(0);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json == R"xxx({"type":46,"elements":[{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"1"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"4"}]}]},{"type":13,"symbol":"·"},{"type":16,"elements":[{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]},{"type":13,"symbol":"·"},{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"1"},{"type":12,"symbol":"-"},{"type":16,"elements":[{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"3"}]}]}]},{"type":20,"symbol":")"}]}]}]})xxx") << json;
}

TEST_F(CalcTestSymbolicRational, mul_neg_add)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x*(-1+sqrt(3));");
    ASSERT_TRUE(res.ToStdString(0) == "x*(-1+sqrt(3))") << res.ToStdString(0);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json == R"xxx({"type":46,"elements":[{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":8,"elements":"x"},{"type":13,"symbol":"·"},{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"1"},{"type":12,"symbol":"-"},{"type":16,"elements":[{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"3"}]}]}]},{"type":20,"symbol":")"}]}]}]})xxx") << json;
}

TEST_F(CalcTestSymbolicRational, diff2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x+x, x);");
    ASSERT_TRUE(res.ToStdString(0) == "2") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, diff3)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,2), x);");
    ASSERT_TRUE(res.ToStdString(0) == "2*x") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, diff4)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(sin(x), x);");
    ASSERT_TRUE(res.ToStdString(0) == "cos(x)") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, variables2)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a=5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"a;") == parser.Parse(LogicalId{0, 0, 2}, U"5;")) << parser.Parse(LogicalId{0, 0, 2}, U"a;").ToStdString(0);
    parser.Parse(LogicalId{0, 0, 3}, U"a=7;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"a;") == parser.Parse(LogicalId{0, 0, 4}, U"7;")) << parser.Parse(LogicalId{0, 0, 4}, U"a;").ToStdString(0);
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"a+1;") == parser.Parse(LogicalId{0, 0, 2}, U"6;")) << parser.Parse(LogicalId{0, 0, 2}, U"a+1;").ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, variables5)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a=5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"a*x;") == parser.Parse(LogicalId{0, 0, 2}, U"5*x;")) << parser.Parse(LogicalId{0, 0, 2}, U"a*x;").ToStdString(0);
    parser.Parse(LogicalId{0, 0, 3}, U"b=x+1;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"a*b;") == parser.Parse(LogicalId{0, 0, 4}, U"5*(1+x);")) << parser.Parse(LogicalId{0, 0, 4}, U"a*b;").ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, long_number_with_symbol)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"12345678901234567890*x;");
    ASSERT_TRUE(res.ToStdString(0) == "12345678901234567890*x") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, errors1)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"sin(x;"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestSymbolicRational, errors2)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"sinx);"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestSymbolicRational, tojson1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x;");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":8,"elements":"x"})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicRational, tojson2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1/2;");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[)"
            R"({"type":14,"elements":[)"
                R"({"type":7,"elements":[)"
                    R"({"type":8,"elements":"1"})"
                R"(]},)"
                R"({"type":10,"elements":[]},)"
                R"({"type":7,"elements":[)"
                    R"({"type":8,"elements":"2"})"
                R"(]})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicRational, tojson3)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1+x;");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":8,"elements":"1"},)"
                R"({"type":11,"symbol":"+"},)"
                R"({"type":8,"elements":"x"})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicRational, tojson4)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-1/2;");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":12,"symbol":"-"},)"
                R"({"type":14,"elements":[)"
                    R"({"type":7,"elements":[)"
                        R"({"type":8,"elements":"1"})"
                    R"(]},)"
                    R"({"type":10,"elements":[]},)"
                    R"({"type":7,"elements":[)"
                        R"({"type":8,"elements":"2"})"
                    R"(]})"
                R"(]})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicRational, tojson5)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2.3;");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[)"
            R"({"type":14,"elements":[)"
                R"({"type":7,"elements":[)"
                    R"({"type":8,"elements":"23"})"
                R"(]},)"
                R"({"type":10,"elements":[]},)"
                R"({"type":7,"elements":[)"
                    R"({"type":8,"elements":"10"})"
                R"(]})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicRational, tojson6)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-2.3;");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":12,"symbol":"-"},)"
                R"({"type":14,"elements":[)"
                    R"({"type":7,"elements":[)"
                        R"({"type":8,"elements":"23"})"
                    R"(]},)"
                    R"({"type":10,"elements":[]},)"
                    R"({"type":7,"elements":[)"
                        R"({"type":8,"elements":"10"})"
                    R"(]})"
                R"(]})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicRational, tojson7)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(3456)/(34)*x;");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":14,"elements":[)"
                    R"({"type":7,"elements":[)"
                        R"({"type":8,"elements":"1728"})"
                    R"(]},)"
                    R"({"type":10,"elements":[]},)"
                    R"({"type":7,"elements":[)"
                        R"({"type":8,"elements":"17"})"
                    R"(]})"
                R"(]},)"
                R"({"type":13,"symbol":"·"},)"
                R"({"type":8,"elements":"x"})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicRational, tojson8)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x*(234)/(67);");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":14,"elements":[)"
                    R"({"type":7,"elements":[)"
                        R"({"type":8,"elements":"234"})"
                    R"(]},)"
                    R"({"type":10,"elements":[]},)"
                    R"({"type":7,"elements":[)"
                        R"({"type":8,"elements":"67"})"
                    R"(]})"
                R"(]},)"
                R"({"type":13,"symbol":"·"},)"
                R"({"type":8,"elements":"x"})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicRational, tojson9)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x-1/2;");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":12,"symbol":"-"},)"
                R"({"type":14,"elements":[)"
                    R"({"type":7,"elements":[)"
                        R"({"type":8,"elements":"1"})"
                    R"(]},)"
                    R"({"type":10,"elements":[]},)"
                    R"({"type":7,"elements":[)"
                        R"({"type":8,"elements":"2"})"
                    R"(]})"
                R"(]},)"
                R"({"type":11,"symbol":"+"},)"
                R"({"type":8,"elements":"x"})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicRational, tojson10)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x-3/4*y;");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":8,"elements":"x"},)"
                R"({"type":12,"symbol":"-"},)"
                R"({"type":14,"elements":[)"
                    R"({"type":7,"elements":[)"
                        R"({"type":8,"elements":"3"})"
                    R"(]},)"
                    R"({"type":10,"elements":[]},)"
                    R"({"type":7,"elements":[)"
                        R"({"type":8,"elements":"4"})"
                    R"(]})"
                R"(]},)"
                R"({"type":13,"symbol":"·"},)"
                R"({"type":8,"elements":"y"})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicRational, tojson11)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"y+3.4*y;");
    ASSERT_TRUE(res.ToStdString(0) == "(22/5)*y") << res.ToStdString(0);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":46,"elements":[{"type":7,"elements":[{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"22"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"5"}]}]},{"type":13,"symbol":"·"},{"type":8,"elements":"y"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, sqrt1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sqrt(4);");
    ASSERT_TRUE(res.ToStdString(0) == "2") << res.ToStdString(0);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":8,"elements":"2"})"
            R"(]})"
        R"(]})" ) << json;

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sqrt(x);");
    ASSERT_TRUE(res.ToStdString(0) == "sqrt(x)") << res.ToStdString(0);
    json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[)"
            R"({"type":16,"elements":[)"
                R"({"type":10,"elements":[]},)"
                R"({"type":7,"elements":[)"
                    R"({"type":8,"elements":"x"})"
                R"(]})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicRational, pow1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x, 3);");
    ASSERT_TRUE(res.ToStdString(0) == "pow(x,3)") << res.ToStdString(0);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[)"
            R"({"type":15,"elements":[)"
                R"({"type":7,"elements":[)"
                    R"({"type":8,"elements":"x"})"
                R"(]},)"
                R"({"type":10,"elements":[]},)"
                R"({"type":7,"elements":[)"
                    R"({"type":8,"elements":"3"})"
                R"(]})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicRational, expand2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"expand((x+1)*(x-1));");
    ASSERT_TRUE(res.ToStdString(0) == "-1+pow(x,2)") << res.ToStdString(0);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":12,"symbol":"-"},{"type":8,"elements":"1"},)"
                R"({"type":11,"symbol":"+"},)"
                R"({"type":15,"elements":[)"
                    R"({"type":7,"elements":[)"
                        R"({"type":8,"elements":"x"})"
                    R"(]},)"
                    R"({"type":10,"elements":[]},)"
                    R"({"type":7,"elements":[)"
                        R"({"type":8,"elements":"2"})"
                    R"(]})"
                R"(]})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicRational, log3)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"log(8, 2);");
    std::string s = res.ToStdString(0);
    ASSERT_TRUE(s == "log(2)/log(8)") << s;
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"r({"type":46,"elements":[)r"
            R"r({"type":14,"elements":[)r"
                R"r({"type":7,"elements":[)r"
                    R"r({"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"2"},{"type":20,"symbol":")"})r"
                R"r(]},)r"
                R"r({"type":10,"elements":[]},)r"
                R"r({"type":7,"elements":[)r"
                    R"r({"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"8"},{"type":20,"symbol":")"})r"
                R"r(]})r"
            R"r(]})r"
        R"r(]})r" ) << json;
}

TEST_F(CalcTestSymbolicRational, diff_tojson)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,3)+2*pow(x,2)-5*x+1, x);");
    std::string s = res.ToStdString(0);
    ASSERT_TRUE(s == "-5+4*x+3*pow(x,2)") << s;
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":12,"symbol":"-"},{"type":8,"elements":"5"},)"
                R"({"type":11,"symbol":"+"},)"
                R"({"type":8,"elements":"4"},)"
                R"({"type":13,"symbol":"·"},)"
                R"({"type":8,"elements":"x"},)"
                R"({"type":11,"symbol":"+"},)"
                R"({"type":8,"elements":"3"},)"
                R"({"type":13,"symbol":"·"},)"
                R"({"type":15,"elements":[)"
                    R"({"type":7,"elements":[)"
                        R"({"type":8,"elements":"x"})"
                    R"(]},)"
                    R"({"type":10,"elements":[]},)"
                    R"({"type":7,"elements":[)"
                        R"({"type":8,"elements":"2"})"
                    R"(]})"
                R"(]})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicRational, expand3)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"expand(pow((x+1),3));");
    ASSERT_TRUE(res.ToStdString(0) == "1+3*x+3*pow(x,2)+pow(x,3)") << res.ToStdString(0);
    ASSERT_TRUE(res.ToJson(10) ==
        R"({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"1"},{"type":11,"symbol":"+"},{"type":8,"elements":"3"},{"type":13,"symbol":"·"},{"type":8,"elements":"x"},{"type":11,"symbol":"+"},{"type":8,"elements":"3"},{"type":13,"symbol":"·"},{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"x"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]},{"type":11,"symbol":"+"},{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"x"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"3"}]}]}]}]})" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, trig_power1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(pow(sin(x),2)+pow(cos(x),2));");
    ASSERT_TRUE(res.ToStdString(0) == "pow(cos(x),2)+pow(sin(x),2)") << res.ToStdString(0);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":46,"elements":[{"type":7,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"cos"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]},{"type":11,"symbol":"+"},{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"sin"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, complex_mul_symbol1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2*i;");
    ASSERT_TRUE(res.ToStdString(0) == "2*i") << res.ToStdString(0);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"2"},{"type":13,"symbol":"·"},{"type":8,"elements":"i"}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicRational, division_nested1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(x+1)/(x-1);");
    ASSERT_TRUE(res.ToStdString(0) == "(1+x)/(-1+x)") << res.ToStdString(0);
    ASSERT_TRUE(res.ToJson(10) ==
        R"({"type":46,"elements":[{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"1"},{"type":11,"symbol":"+"},{"type":8,"elements":"x"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":8,"elements":"1"},{"type":11,"symbol":"+"},{"type":8,"elements":"x"}]}]}]})" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, diff_power5)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,5),x);");
    ASSERT_TRUE(res.ToStdString(0) == "5*pow(x,4)") << res.ToStdString(0);
    ASSERT_TRUE(res.ToJson(10) ==
        R"({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"5"},{"type":13,"symbol":"·"},{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"x"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"4"}]}]}]}]})" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, diff_cos)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(cos(x),x);");
    ASSERT_TRUE(res.ToStdString(0) == "-sin(x)") << res.ToStdString(0);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":46,"elements":[{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":7,"elements":[{"type":8,"elements":"sin"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, diff_nonsymbol)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x, x+1);"), yutovo_calculator::ParserException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x, 5);"), yutovo_calculator::ParserException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x, sin(x));"), yutovo_calculator::ParserException);
}

TEST_F(CalcTestSymbolicRational, frac_power_mult)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x,(1/2))*pow(x,(1/3));");
    ASSERT_TRUE(res.ToStdString(0) == "pow(x,5/6)") << res.ToStdString(0);
    ASSERT_TRUE(res.ToJson(10) ==
        R"({"type":46,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"x"}]},{"type":10,"elements":[]},{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"5"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"6"}]}]}]}]})" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, expand_diff_squares)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"expand((x+y)*(x-y));");
    ASSERT_TRUE(res.ToStdString(0) == "pow(x,2)-pow(y,2)") << res.ToStdString(0);
    ASSERT_TRUE(res.ToJson(10) ==
        R"({"type":46,"elements":[{"type":7,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"x"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]},{"type":12,"symbol":"-"},{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"y"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]}]}]})" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, log_ratio)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"log(2,4);");
    ASSERT_TRUE(res.ToStdString(0) == "log(4)/log(2)") << res.ToStdString(0);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":46,"elements":[{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"4"},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"2"},{"type":20,"symbol":")"}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, sqrt_half)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sqrt(1/2);");
    ASSERT_TRUE(res.ToStdString(0) == "(1/2)*sqrt(2)") << res.ToStdString(0);
    ASSERT_TRUE(res.ToJson(10) ==
        R"({"type":46,"elements":[{"type":7,"elements":[{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"1"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]},{"type":13,"symbol":"·"},{"type":16,"elements":[{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]}]}]})" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, limit_cot0)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(cot(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, limit_sec_pi2)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(sec(x),x,(pi)/(2));");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, limit_csc0)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(csc(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, limit_sin_x)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(sin(x)/x,x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":46,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"nan\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, limit_exp_m1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs((exp(x)-1)/x,x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, limit_ln_x)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(ln(x)/x,x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":46,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"∞\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, limit_exp_1x)
{
    bool thrown = false;
    try
    {
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(exp(1/x),x,0);");
    }
    catch (...)
    {
        thrown = true;
    }
    ASSERT_TRUE(thrown);
}

TEST_F(CalcTestSymbolicRational, limit_sin_1x)
{
    bool thrown = false;
    try
    {
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(sin(1/x),x,0);");
    }
    catch (...)
    {
        thrown = true;
    }
    ASSERT_TRUE(thrown);
}

TEST_F(CalcTestSymbolicRational, limit_cos_1x)
{
    bool thrown = false;
    try
    {
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(cos(1/x),x,0);");
    }
    catch (...)
    {
        thrown = true;
    }
    ASSERT_TRUE(thrown);
}

TEST_F(CalcTestSymbolicRational, limit_ln_1x)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(ln(1/x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, limit_sqrt_noo)
{
    bool thrown = false;
    try
    {
        parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sqrt(-∞);");
    }
    catch (...)
    {
        thrown = true;
    }
    ASSERT_TRUE(thrown);
}

TEST_F(CalcTestSymbolicRational, inf1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"inf;");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, inf2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"∞;");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, inf3)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(1)/(∞);");
    ASSERT_TRUE(res.ToStdString(10) == "0") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, inf4)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs((1)/(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, inf5)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(exp(x),x,-∞);");
    ASSERT_TRUE(res.ToStdString(10) == "0") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, inf6)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(ln(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, inf7)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(exp(x),x,∞);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, inf8)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs((1)/(x),x,∞);");
    ASSERT_TRUE(res.ToStdString(10) == "0") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, inf9)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs((1)/(x-1),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, inf10)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs((pow(x,2)-1)/(x-1),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, inf11)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-∞;");
    ASSERT_TRUE(res.ToStdString(10) == "-∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, nan1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"nan;");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, nan2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"nan+1;");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, nan3)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x*nan/3;");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, nan4)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(0)/(0);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, nan5)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(∞-∞,x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, nan6)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(0*∞,x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, nan7)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs((∞)/(∞),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, nan8)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(pow(∞,0),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "1") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, nan9)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(pow(0,0),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "1") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, division2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1 / 0;");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":46,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"∞\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, division3)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0 / 0;");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":46,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"nan\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, simplify2)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x+x;");
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 2}, U"simplify(f(x));");
    ASSERT_TRUE(res.ToString(10) == U"2*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, simplify4)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(pow(x+y,2));");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x+y,2)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":46,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"x"},{"type":11,"symbol":"+"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicRational, simplify5)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(pow(x+y,2)+pow(x+y,2));");
    ASSERT_TRUE(res.ToStdString(10) == "2*pow(x+y,2)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"2"},{"type":13,"symbol":"·"},{"type":15,"elements":[{"type":7,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"x"},{"type":11,"symbol":"+"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicRational, power_add_exp1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x+y, z);");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x+y,z)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":46,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"x"},{"type":11,"symbol":"+"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"z"}]}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicRational, power_mul_base1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x*y, z);");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x*y,z)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":46,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"x"},{"type":13,"symbol":"·"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"z"}]}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicRational, power_neg_exp1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x+y, -2);");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x+y,-2)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":46,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"x"},{"type":11,"symbol":"+"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":8,"elements":"2"}]}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicRational, nested_power1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(pow(x+y, 2), z);");
    ASSERT_TRUE(res.ToStdString(10) == "pow(pow(x+y,2),z)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":46,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":15,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"x"},{"type":11,"symbol":"+"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"z"}]}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicRational, sqrt2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sqrt(x);");
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s == "sqrt(x)") << s;
}

TEST_F(CalcTestSymbolicRational, power4)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-(1)/pow(10,100);");
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s == "-1/10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000") << s;
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json == "{\"type\":46,\"elements\":[{\"type\":7,\"elements\":[{\"type\":12,\"symbol\":\"-\"},{\"type\":14,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"1\"}]},{\"type\":10,\"elements\":[]},{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\"}]}]}]}]}") << json;
}

TEST_F(CalcTestSymbolicRational, hyperbolic_sinh)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sinh(0);");
    ASSERT_TRUE(res.ToStdString(0) == "0") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, hyperbolic_cosh)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cosh(0);");
    ASSERT_TRUE(res.ToStdString(0) == "1") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, hyperbolic_tanh)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"tanh(0);");
    ASSERT_TRUE(res.ToStdString(0) == "0") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, hyperbolic_coth0)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(coth(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, hyperbolic_sech)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sech(0);");
    ASSERT_TRUE(res.ToStdString(0) == "1") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, hyperbolic_csch0)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(csch(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, hyperbolic_asinh)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arsinh(0);");
    ASSERT_TRUE(res.ToStdString(0) == "0") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, hyperbolic_acosh)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcosh(1);");
    ASSERT_TRUE(res.ToStdString(0) == "0") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, hyperbolic_atanh)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"artanh(0);");
    ASSERT_TRUE(res.ToStdString(0) == "0") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, hyperbolic_acoth1)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(arcoth(x),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "acoth(1)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, hyperbolic_asech)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arsech(1);");
    ASSERT_TRUE(res.ToStdString(0) == "0") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, hyperbolic_acsch0)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(arcsch(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "acsch(0)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, hyperbolic_symbolic)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sinh(x);");
    ASSERT_TRUE(res.ToStdString(0) == "sinh(x)") << res.ToStdString(0);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcosh(x);");
    ASSERT_TRUE(res.ToStdString(0) == "acosh(x)") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, hyperbolic_alias)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cosech(0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcosech(1);");
    ASSERT_TRUE(res.ToStdString(0) == "log(1+sqrt(2))") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, hyperbolic_inverse_synonyms)
{
    Symbolic<Rational> a = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"asinh(0);");
    Symbolic<Rational> b = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arsinh(0);");
    Symbolic<Rational> c = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcsinh(0);");
    ASSERT_TRUE(a.ToStdString(0) == "0") << a.ToStdString(0);
    ASSERT_TRUE(b.ToStdString(0) == "0") << b.ToStdString(0);
    ASSERT_TRUE(c.ToStdString(0) == "0") << c.ToStdString(0);

    a = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"acosh(1);");
    b = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcosh(1);");
    c = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arccosh(1);");
    ASSERT_TRUE(a.ToStdString(0) == "0") << a.ToStdString(0);
    ASSERT_TRUE(b.ToStdString(0) == "0") << b.ToStdString(0);
    ASSERT_TRUE(c.ToStdString(0) == "0") << c.ToStdString(0);

    a = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"atanh(0);");
    b = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"artanh(0);");
    c = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arctanh(0);");
    ASSERT_TRUE(a.ToStdString(0) == "0") << a.ToStdString(0);
    ASSERT_TRUE(b.ToStdString(0) == "0") << b.ToStdString(0);
    ASSERT_TRUE(c.ToStdString(0) == "0") << c.ToStdString(0);

    a = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"acoth(1);");
    b = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcoth(1);");
    c = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arccoth(1);");
    ASSERT_TRUE(a.ToStdString(10) == "acoth(1)") << a.ToStdString(10);
    ASSERT_TRUE(b.ToStdString(10) == "acoth(1)") << b.ToStdString(10);
    ASSERT_TRUE(c.ToStdString(10) == "acoth(1)") << c.ToStdString(10);

    a = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"asech(1);");
    b = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arsech(1);");
    c = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcsech(1);");
    ASSERT_TRUE(a.ToStdString(0) == "0") << a.ToStdString(0);
    ASSERT_TRUE(b.ToStdString(0) == "0") << b.ToStdString(0);
    ASSERT_TRUE(c.ToStdString(0) == "0") << c.ToStdString(0);

    a = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"acsch(0);");
    b = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcsch(0);");
    c = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arccsch(0);");
    Symbolic<Rational> d = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcosech(0);");
    Symbolic<Rational> e = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arccosech(0);");
    ASSERT_TRUE(a.ToStdString(10) == "acsch(0)") << a.ToStdString(10);
    ASSERT_TRUE(b.ToStdString(10) == "acsch(0)") << b.ToStdString(10);
    ASSERT_TRUE(c.ToStdString(10) == "acsch(0)") << c.ToStdString(10);
    ASSERT_TRUE(d.ToStdString(10) == "acsch(0)") << d.ToStdString(10);
    ASSERT_TRUE(e.ToStdString(10) == "acsch(0)") << e.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, hyperbolic_acosh_nan)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcosh(nan);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, log_x_base)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"log(x,2);");
    ASSERT_TRUE(res.ToStdString(0) == "log(2)/log(x)") << res.ToStdString(0);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":46,"elements":[{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"2"},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, cot_tojson)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cot(x);");
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"cot"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, sec_tojson)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sec(x);");
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"sec"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, nested_functions_brackets)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"csc(coth(x));");
    ASSERT_TRUE(res.ToStdString(0) == "csc(coth(x))") << res.ToStdString(0);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"csc"},{"type":19,"symbol":"("},{"type":8,"elements":"coth"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, ln_russian)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ln(x);");
    ASSERT_TRUE(res.ToStdString(0, Language::Russian) == "ln(x)") << res.ToStdString(0, Language::Russian);
    ASSERT_TRUE(res.ToJson(10, Language::Russian) ==
        R"r({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"ln"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10, Language::Russian);
}

TEST_F(CalcTestSymbolicRational, ln_in_expression_russian)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ln(x)+1;");
    ASSERT_TRUE(res.ToStdString(0, Language::Russian) == "1+ln(x)") << res.ToStdString(0, Language::Russian);
    ASSERT_TRUE(res.ToJson(10, Language::Russian) ==
        R"r({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"1"},{"type":11,"symbol":"+"},{"type":8,"elements":"ln"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10, Language::Russian);
}

TEST_F(CalcTestSymbolicRational, ln_english)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ln(x);");
    ASSERT_TRUE(res.ToStdString(0) == "log(x)") << res.ToStdString(0);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, log_change_of_base_russian)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"log(2,4);");
    ASSERT_TRUE(res.ToStdString(0, Language::Russian) == "ln(4)/ln(2)") << res.ToStdString(0, Language::Russian);
    ASSERT_TRUE(res.ToJson(10, Language::Russian) ==
        R"r({"type":46,"elements":[{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"ln"},{"type":19,"symbol":"("},{"type":8,"elements":"4"},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"ln"},{"type":19,"symbol":"("},{"type":8,"elements":"2"},{"type":20,"symbol":")"}]}]}]})r" ) << res.ToJson(10, Language::Russian);
}

TEST_F(CalcTestSymbolicRational, sin2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(0);");
    ASSERT_TRUE(res.ToStdString(0) == "0") << res.ToStdString(0);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"0"}]}]})"
        ) << json;
}

TEST_F(CalcTestSymbolicRational, sin3)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x);");
    ASSERT_TRUE(res.ToStdString(0) == "sin(x)") << res.ToStdString(0);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"r({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"sin"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r"
        ) << json;
}

TEST_F(CalcTestSymbolicRational, cos2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cos(0);");
    ASSERT_TRUE(res.ToStdString(0) == "1") << res.ToStdString(0);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"1"}]}]})"
        ) << json;
}

TEST_F(CalcTestSymbolicRational, cos3)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cos(x);");
    ASSERT_TRUE(res.ToStdString(0) == "cos(x)") << res.ToStdString(0);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"r({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"cos"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r"
        ) << json;
}

TEST_F(CalcTestSymbolicRational, exp1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"exp(0);");
    ASSERT_TRUE(res.ToStdString(0) == "1") << res.ToStdString(0);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"1"}]}]})"
        ) << json;
}

TEST_F(CalcTestSymbolicRational, exp2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"exp(x);");
    ASSERT_TRUE(res.ToStdString(0) == "exp(x)") << res.ToStdString(0);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"r({"type":46,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"e"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"x"}]}]}]})r"
        ) << json;
}

TEST_F(CalcTestSymbolicRational, ln2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ln(1);");
    ASSERT_TRUE(res.ToStdString(0) == "0") << res.ToStdString(0);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"0"}]}]})"
        ) << json;
}

TEST_F(CalcTestSymbolicRational, ln3)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ln(x);");
    ASSERT_TRUE(res.ToStdString(0) == "log(x)") << res.ToStdString(0);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"r({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r"
        ) << json;
}

TEST_F(CalcTestSymbolicRational, fact1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"5!;");
    ASSERT_TRUE(res.ToStdString(0) == "120") << res.ToStdString(0);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"120"}]}]})"
        ) << json;
}

TEST_F(CalcTestSymbolicRational, fact2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x!;");
    ASSERT_TRUE(res.ToStdString(0) == "gamma(1+x)") << res.ToStdString(0);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"r({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"gamma"},{"type":19,"symbol":"("},{"type":8,"elements":"1"},{"type":11,"symbol":"+"},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r"
        ) << json;
}

TEST_F(CalcTestSymbolicRational, fact3)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0!;");
    ASSERT_TRUE(res.ToStdString(0) == "1") << res.ToStdString(0);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1!;");
    ASSERT_TRUE(res.ToStdString(0) == "1") << res.ToStdString(0);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"10!;");
    ASSERT_TRUE(res.ToStdString(0) == "3628800") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, fact4)
{
    yutovo_calculator::ParserContext parser_context;
#ifdef NDEBUG
    parser_context.Init(1);
#else
    parser_context.Init(100);
#endif
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"100000!;", &parser_context),
        yutovo_calculator::TimeExceedException);
}

TEST_F(CalcTestSymbolicRational, gamma1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"gamma(5);");
    ASSERT_TRUE(res.ToStdString(0) == "24") << res.ToStdString(0);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":46,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"24\"}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicRational, gamma_tojson)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"gamma(x);");
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"gamma"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, min_max1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"min(x,y);");
    ASSERT_TRUE(res.ToStdString(0) == "min(x,y)") << res.ToStdString(0);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"max(x,y);");
    ASSERT_TRUE(res.ToStdString(0) == "max(x,y)") << res.ToStdString(0);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"min(1/2,1/3);");
    ASSERT_TRUE(res.ToStdString(0) == "1/3") << res.ToStdString(0);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"max(1/2,1/3);");
    ASSERT_TRUE(res.ToStdString(0) == "1/2") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, min_max2)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a=5;");
    parser.Parse(LogicalId{0, 0, 2}, U"b=3;");

    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 3}, U"min(a,b);");
    ASSERT_TRUE(res.ToStdString(0) == "3") << res.ToStdString(0);

    res = parser.Parse(LogicalId{0, 0, 4}, U"max(a,b);");
    ASSERT_TRUE(res.ToStdString(0) == "5") << res.ToStdString(0);
}

TEST_F(CalcTestSymbolicRational, derivative_at_point1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x, [x=3]);");
    ASSERT_TRUE(res.ToStdString(10) == "1") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, derivative_at_point2)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x)=pow(x,2);");
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x), [x=3]);");
    ASSERT_TRUE(res.ToStdString(10) == "6") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, derivative_at_point3)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,2), [y=3]);");
    ASSERT_TRUE(res.ToStdString(10) == "0") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, derivative_at_point4)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x,y)=x*y;");
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x,y), [x=3]);");
    ASSERT_TRUE(res.ToString(10) == U"y") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, derivative_at_point5)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x,y)=x*y;");
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x,y), [y=3]);");
    ASSERT_TRUE(res.ToString(10) == U"x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, derivative_at_point6)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(1/x, [x=0]);");
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":46,"elements":[{"type":7,"elements":[{"type":8,"elements":"∞"}]}]})r")
        << res.ToJson(10);
}

TEST_F(CalcTestSymbolicRational, derivative_at_point_multi1)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x*y, [x=2, y=3]);");
    ASSERT_TRUE(res.ToStdString(10) == "3") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, derivative_at_point_multi2)
{
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x*y, [y=3, x=2]);");
    ASSERT_TRUE(res.ToStdString(10) == "2") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicRational, derivative_at_point_multi3)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x,y)=pow(x,2)*y;");
    Symbolic<Rational> res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x,y), [x=2, y=3]);");
    ASSERT_TRUE(res.ToStdString(10) == "12") << res.ToStdString(10);
}

}
