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

TEST_F(CalcTestSymbolicComplex, numbers1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1;");
    ASSERT_TRUE(res.ToStdString(10) == "1") << res.ToStdString(10);

    res = parser.Parse(LogicalId{0, 0, 1}, U"1+2*i;");
    ASSERT_TRUE(res.ToStdString(10) == "1.+2.*i") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, numbers2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"12345678901234567890.123;");
    ASSERT_TRUE(res.ToStdString(10) == "1.235E+19") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, numbers3)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1/3;", 3);
    ASSERT_TRUE(res.ToStdString(10) == "0.333") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, numbers4)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"."), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"..2"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1..2"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.2.3"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2.3.4"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U".1.2"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.2."), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestSymbolicComplex, numbers5)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"z+0.12345678901234567890123456789012345678901234567890+0.98765432109876543210987654321098765432109876543210*i;", 50);
    std::string s = res.ToStdString(50);
    ASSERT_TRUE(s.find("0.123456789012345678901234567890123456789012345678") != std::string::npos) << s;
    ASSERT_TRUE(s.find("0.987654321098765432109876543210987654321098765432") != std::string::npos) << s;
}

TEST_F(CalcTestSymbolicComplex, eval1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x;");
    ASSERT_TRUE(res.ToStdString(10) == "x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, eval2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x+x;");
    ASSERT_TRUE(res.ToString(10) == U"2.*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, eval3)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"y+2*y+2;");
    ASSERT_TRUE(res.ToString(10) == U"2.+3.*y") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, eval4)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"123456789012345678901234567890.123+x;");
    std::string s = res.ToStdString(20);
    ASSERT_TRUE(s == "1.235E+29+x") << s;
}

TEST_F(CalcTestSymbolicComplex, eval5)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x)+2356.78*x;");
    std::string s = res.ToStdString(3);
    ASSERT_TRUE(s == "2.357E+3*x+sin(x)") << s;
}

TEST_F(CalcTestSymbolicComplex, eval6)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x)-2356.78*x;");
    std::string s = res.ToStdString(3);
    ASSERT_TRUE(s == "-2.357E+3*x+sin(x)") << s;
}

TEST_F(CalcTestSymbolicComplex, eval7)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x+(x)/(3);");
    std::string s = res.ToStdString(3);
    ASSERT_TRUE(s == "1.333*x") << s;
}

TEST_F(CalcTestSymbolicComplex, eval8)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"+-x;");
    ASSERT_TRUE(res.ToString(10) == U"-x") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":47,\"elements\":[{\"type\":7,\"elements\":[{\"type\":12,\"symbol\":\"-\"},{\"type\":8,\"elements\":\"x\"}]}]}") << 
        res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, eval9)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"+x;");
    ASSERT_TRUE(res.ToString(10) == U"x") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":47,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"x\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, eval10)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"--x;");
    ASSERT_TRUE(res.ToString(10) == U"x") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":47,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"x\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, eval11)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"++x;");
    ASSERT_TRUE(res.ToString(10) == U"x") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":47,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"x\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, eval12)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x)--2356.78*x;");
    std::string s = res.ToStdString(3);
    ASSERT_TRUE(s == "2.357E+3*x+sin(x)") << s;
}

TEST_F(CalcTestSymbolicComplex, eval13)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x)-+2356.78*x;");
    std::string s = res.ToStdString(3);
    ASSERT_TRUE(s == "-2.357E+3*x+sin(x)") << s;
}

TEST_F(CalcTestSymbolicComplex, eval14)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-sin(x)-2356.78*x;");
    std::string s = res.ToStdString(3);
    ASSERT_TRUE(s == "-2.357E+3*x-sin(x)") << s;
}

TEST_F(CalcTestSymbolicComplex, eval15)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"--+sin(x)--+2356.78*x;");
    std::string s = res.ToStdString(3);
    ASSERT_TRUE(s == "2.357E+3*x+sin(x)") << s;
}

TEST_F(CalcTestSymbolicComplex, eval16)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x+x;");
    ASSERT_TRUE(res.ToStdString(10) == "2.*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, eval17)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"y+2*y+2;");
    ASSERT_TRUE(res.ToStdString(10) == "2.+3.*y") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, addition1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x + 1;");
    ASSERT_TRUE(res.ToStdString(10) == "1.+x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, subtraction1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x - 1;");
    ASSERT_TRUE(res.ToStdString(10) == "-1.+x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, multiplication1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x * 2;");
    ASSERT_TRUE(res.ToStdString(10) == "2.*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, division1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x / 3;", 3);
    ASSERT_TRUE(res.ToStdString(10) == "0.333*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, mixed_arithmetic1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x + 2*x;");
    ASSERT_TRUE(res.ToStdString(10) == "3.*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, mixed_arithmetic2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2*pow(x,2) + 3*x - 5;");
    ASSERT_TRUE(res.ToStdString(10) == "-5.+3.*x+2.*pow(x,2.)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, precision1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1/3;", 10);
    ASSERT_TRUE(res.ToStdString(10) == "0.3333333333") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, precision2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1/3;", 20);
    ASSERT_TRUE(res.ToStdString(10) == "0.33333333333333333333") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, diff1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,2), x);");
    ASSERT_TRUE(res.ToStdString(10) == "2.*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, indefinite_integral1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"indefinite_integral(pow(x,2), x);");
    ASSERT_TRUE(res.ToStdString(10) == "0.333*pow(x,3.)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, indefinite_integral2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"indefinite_integral(x*y, y);");
    ASSERT_TRUE(res.ToStdString(10) == "0.5*x*pow(y,2.)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, indefinite_integral_nonsymbol)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"indefinite_integral(x, 1);"), yutovo_calculator::ParserException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"indefinite_integral(x, x+1);"), yutovo_calculator::ParserException);
}

TEST_F(CalcTestSymbolicComplex, definite_integral1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(2,34,3y,x);");
    ASSERT_TRUE(res.ToStdString(10) == "96.*y") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, definite_integral_j_russian)
{
    parser.SetLocale(Language::Russian);
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,\u03C0,pow(e,j*z),z);", 10);
    ASSERT_TRUE(res.ToStdString(10, Language::Russian) == "2.*j") << res.ToStdString(10, Language::Russian);
}

TEST_F(CalcTestSymbolicComplex, definite_integral_i_english)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,\u03C0,pow(e,i*z),z);", 10);
    ASSERT_TRUE(res.ToStdString(10, Language::English) == "2.*i") << res.ToStdString(10, Language::English);
}

TEST_F(CalcTestSymbolicComplex, definite_integral_i_spanish)
{
    parser.SetLocale(Language::Spanish);
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,\u03C0,pow(e,i*z),z);", 10);
    ASSERT_TRUE(res.ToStdString(10, Language::Spanish) == "2.*i") << res.ToStdString(10, Language::Spanish);
}

TEST_F(CalcTestSymbolicComplex, definite_integral_i_brazilian_portuguese)
{
    parser.SetLocale(Language::BrazilianPortuguese);
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,\u03C0,pow(e,i*z),z);", 10);
    ASSERT_TRUE(res.ToStdString(10, Language::BrazilianPortuguese) == "2.*i") << res.ToStdString(10, Language::BrazilianPortuguese);
}

TEST_F(CalcTestSymbolicComplex, expand1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"expand(pow((x+1),2));");
    ASSERT_TRUE(res.ToStdString(10) == "1.+2.*x+pow(x,2.)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, subs1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(pow(x,2), x, 5);");
    ASSERT_TRUE(res.ToStdString(10) == "25") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, subs2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(log(x,2),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":47,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"∞\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, subs3)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(log(x,2),x,2);");
    ASSERT_TRUE(res.ToStdString(10) == "1") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, subs4)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(pow(yy,3), yy, 5);");
    ASSERT_TRUE(res.ToStdString(10) == "125") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, subs5)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(pow(z,2)+1,z,2+3*I);");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":12,"symbol":"-"},)"
                R"({"type":8,"elements":"4"},)"
                R"({"type":11,"symbol":"+"},)"
                R"({"type":8,"elements":"12"},)"
                R"({"type":13,"symbol":"·"},)"
                R"({"type":8,"elements":"i"})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicComplex, subscript1)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x{2};"), SyntaxException);
}

TEST_F(CalcTestSymbolicComplex, subscript2)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x{us};"), SyntaxException);
}

TEST_F(CalcTestSymbolicComplex, subscript3)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x{ab};"), SyntaxException);
}

TEST_F(CalcTestSymbolicComplex, subscript4)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x{2}+y;"), SyntaxException);
}

TEST_F(CalcTestSymbolicComplex, subscript5)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2x{2};"), SyntaxException);
}

TEST_F(CalcTestSymbolicComplex, subscript6)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x{2});"), SyntaxException);
}

TEST_F(CalcTestSymbolicComplex, infinity_syntax_error)
{
    for (size_t n = 2; n <= 5; ++n)
    {
        std::u32string expr(n, U'∞');
        expr += U';';
        ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, expr), SyntaxException) << "n=" << n;
    }
}

TEST_F(CalcTestSymbolicComplex, infinity_in_identifier)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x∞;"), SyntaxException);
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"∞x;"), SyntaxException);
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ab∞;"), SyntaxException);
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"∞ab;"), SyntaxException);
}

TEST_F(CalcTestSymbolicComplex, infinity_in_number)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1∞;"), SyntaxException);
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"12.3∞;"), SyntaxException);
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"∞1;"), SyntaxException);
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"∞12.3;"), SyntaxException);
}

TEST_F(CalcTestSymbolicComplex, imaginary1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"3*i;");
    ASSERT_TRUE(res.ToStdString(10) == "3.*i") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, imaginary2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"3-34.5*i;");
    ASSERT_TRUE(res.ToStdString(10) == "3.-34.5*i") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, variables1)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a=2;");
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 2}, U"a*x;");
    ASSERT_TRUE(res.ToStdString(10) == "2.*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, user_functions1)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x+1;");
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 2}, U"f(x);");
    ASSERT_TRUE(res.ToStdString(10) == "1.+x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, unary_plus1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"+x;");
    ASSERT_TRUE(res.ToStdString(10) == "x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, unary_minus1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-x;");
    ASSERT_TRUE(res.ToStdString(10) == "-x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, sin1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x);");
    ASSERT_TRUE(res.ToStdString(10) == "sin(x)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, cos1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cos(x);");
    ASSERT_TRUE(res.ToStdString(10) == "cos(x)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, log1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"log(2, 8);");
    ASSERT_TRUE(res.ToStdString(10) == "3") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, log2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"log(x,y);");
    ASSERT_TRUE(res.ToStdString(10, Language::Russian) == "ln(y)/ln(x)") << res.ToStdString(10, Language::Russian);
    ASSERT_TRUE(res.ToJson(10, Language::Russian) ==
        R"r({"type":47,"elements":[{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"ln"},{"type":19,"symbol":"("},{"type":8,"elements":"y"},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"ln"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]}]})r" ) << res.ToJson(10, Language::Russian);
}

TEST_F(CalcTestSymbolicComplex, root1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"root(8, 3);");
    ASSERT_TRUE(res.ToStdString(10) == "2") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, simplify1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(x+x);");
    ASSERT_TRUE(res.ToStdString(10) == "2.*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, sin_pi_12)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(sin(pi/12));");
    ASSERT_TRUE(res.ToStdString(10) == "0.259") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json == R"xxx({"type":47,"elements":[{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":8,"elements":"0.25"},{"type":13,"symbol":"·"},{"type":16,"elements":[{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]},{"type":13,"symbol":"·"},{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"1"},{"type":12,"symbol":"-"},{"type":16,"elements":[{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"3"}]}]}]},{"type":20,"symbol":")"}]}]}]})xxx") << json;
}

TEST_F(CalcTestSymbolicComplex, diff2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x+x, x);");
    ASSERT_TRUE(res.ToStdString(10) == "2") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, diff3)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,2), x);");
    ASSERT_TRUE(res.ToStdString(10) == "2.*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, diff4)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(sin(x), x);");
    ASSERT_TRUE(res.ToStdString(10) == "cos(x)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, variables2)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a=5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"a;") == parser.Parse(LogicalId{0, 0, 2}, U"5;")) << parser.Parse(LogicalId{0, 0, 2}, U"a;").ToStdString(10);
    parser.Parse(LogicalId{0, 0, 3}, U"a=7;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"a;") == parser.Parse(LogicalId{0, 0, 4}, U"7;")) << parser.Parse(LogicalId{0, 0, 4}, U"a;").ToStdString(10);
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"a+1;") == parser.Parse(LogicalId{0, 0, 2}, U"6;")) << parser.Parse(LogicalId{0, 0, 2}, U"a+1;").ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, variables5)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a=5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"a*x;") == parser.Parse(LogicalId{0, 0, 2}, U"5*x;")) << parser.Parse(LogicalId{0, 0, 2}, U"a*x;").ToStdString(10);
    parser.Parse(LogicalId{0, 0, 3}, U"b=x+1;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"a*b;") == parser.Parse(LogicalId{0, 0, 4}, U"5*(1+x);")) << parser.Parse(LogicalId{0, 0, 4}, U"a*b;").ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, long_number_with_symbol)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"12345678901234567890.123*x;");
    ASSERT_TRUE(res.ToStdString(10) == "1.235E+19*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, errors1)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"sin(x;"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestSymbolicComplex, errors2)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"sinx);"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestSymbolicComplex, tojson1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x;");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":8,"elements":"x"})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicComplex, tojson2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1+i;");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":8,"elements":"1"},)"
                R"({"type":11,"symbol":"+"},)"
                R"({"type":8,"elements":"i"})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicComplex, tojson3)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1+2*i;");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":8,"elements":"1"},)"
                R"({"type":11,"symbol":"+"},)"
                R"({"type":8,"elements":"2"},)"
                R"({"type":13,"symbol":"·"},)"
                R"({"type":8,"elements":"i"})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicComplex, tojson4)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.234-2.234*i;");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":8,"elements":"1.234"},)"
                R"({"type":12,"symbol":"-"},)"
                R"({"type":8,"elements":"2.234"},)"
                R"({"type":13,"symbol":"·"},)"
                R"({"type":8,"elements":"i"})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicComplex, tojson5)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-1.234-2.234*i;");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":12,"symbol":"-"},)"
                R"({"type":8,"elements":"1.234"},)"
                R"({"type":12,"symbol":"-"},)"
                R"({"type":8,"elements":"2.234"},)"
                R"({"type":13,"symbol":"·"},)"
                R"({"type":8,"elements":"i"})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicComplex, tojson6)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"evalf(1+2*i);");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":8,"elements":"1"},)"
                R"({"type":11,"symbol":"+"},)"
                R"({"type":8,"elements":"2"},)"
                R"({"type":13,"symbol":"·"},)"
                R"({"type":8,"elements":"i"})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicComplex, tojson7)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"evalf(1-2*i);");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":8,"elements":"1"},)"
                R"({"type":12,"symbol":"-"},)"
                R"({"type":8,"elements":"2"},)"
                R"({"type":13,"symbol":"·"},)"
                R"({"type":8,"elements":"i"})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicComplex, mul_rational_exp)
{
    //x/sqrt(y) produces Mul with y -> -1/2 (Rational, not Integer).
    //this exercises the Mul handler's else branch with a non-Integer exponent.
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x/sqrt(y);");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json.find("\"type\":47") != std::string::npos) << json;
    ASSERT_TRUE(json.find("x") != std::string::npos) << json;
    ASSERT_TRUE(json.find("y") != std::string::npos) << json;
}

TEST_F(CalcTestSymbolicComplex, tojson8)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"evalf(-1.1-2*i);");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":12,"symbol":"-"},)"
                R"({"type":8,"elements":"1.1"},)"
                R"({"type":12,"symbol":"-"},)"
                R"({"type":8,"elements":"2"},)"
                R"({"type":13,"symbol":"·"},)"
                R"({"type":8,"elements":"i"})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicComplex, sqrt1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sqrt(-1);");
    ASSERT_TRUE(res.ToStdString(10) == "i") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":8,"elements":"i"})"
            R"(]})"
        R"(]})" ) << json;

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sqrt(x);");
    json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
            R"({"type":16,"elements":[)"
                R"({"type":10,"elements":[]},)"
                R"({"type":7,"elements":[)"
                    R"({"type":8,"elements":"x"})"
                R"(]})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicComplex, pow1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x, 3);");
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(res.ToStdString(10) == "pow(x,3.)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
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

TEST_F(CalcTestSymbolicComplex, pow2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1/pow(10,100);");
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s == "1E-100") << s;
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json == "{\"type\":47,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"1\"},{\"type\":13,\"symbol\":\"·\"},{\"type\":15,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"10\"}]},{\"type\":10,\"elements\":[]},{\"type\":7,\"elements\":[{\"type\":7,\"elements\":[{\"type\":12,\"symbol\":\"-\"},{\"type\":8,\"elements\":\"100\"}]}]}]}]}]}") << json;
}

TEST_F(CalcTestSymbolicComplex, pow3)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x,2);");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x,2.)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, evalf1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"evalf(pi);", 3);
    ASSERT_TRUE(res.ToStdString(10) == "3.142") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":8,"elements":"3.142"})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicComplex, diff_complex1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,3)+2*pow(x,2)-5*x+1, x);");
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(res.ToStdString(10) == "-5.+4.*x+3.*pow(x,2.)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
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

TEST_F(CalcTestSymbolicComplex, subs_complex1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(pow(x,2)+pow(y,2), x, 3);");
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(res.ToStdString(10) == "9.+pow(y,2.)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
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

TEST_F(CalcTestSymbolicComplex, tojson_power1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x,(-1));");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
            R"({"type":15,"elements":[)"
                R"({"type":7,"elements":[)"
                    R"({"type":8,"elements":"x"})"
                R"(]},)"
                R"({"type":10,"elements":[]},)"
                R"({"type":7,"elements":[)"
                    R"({"type":12,"symbol":"-"},)"
                    R"({"type":8,"elements":"1"})"
                R"(]})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicComplex, imaginary_variable1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(2+3*i)*x;");
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(res.ToStdString(10) == "(2.+3.*i)*x") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":7,"elements":[)"
                    R"({"type":8,"elements":"2"},)"
                    R"({"type":11,"symbol":"+"},)"
                    R"({"type":8,"elements":"3"},)"
                    R"({"type":13,"symbol":"·"},)"
                    R"({"type":8,"elements":"i"})"
                R"(]},)"
                R"({"type":13,"symbol":"·"},)"
                R"({"type":8,"elements":"x"})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicComplex, tojson_imaginary1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(2+3*i)*x;");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":7,"elements":[)"
                    R"({"type":8,"elements":"2"},)"
                    R"({"type":11,"symbol":"+"},)"
                    R"({"type":8,"elements":"3"},)"
                    R"({"type":13,"symbol":"·"},)"
                    R"({"type":8,"elements":"i"})"
                R"(]},)"
                R"({"type":13,"symbol":"·"},)"
                R"({"type":8,"elements":"x"})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicComplex, root2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"root(8, 3);");
    ASSERT_TRUE(res.ToStdString(10) == "2") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
            R"({"type":7,"elements":[)"
                R"({"type":8,"elements":"2"})"
            R"(]})"
        R"(]})" ) << json;

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"root(x, 3);");
    json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[)"
            R"({"type":17,"elements":[)"
                R"({"type":7,"elements":[)"
                    R"({"type":8,"elements":"3"})"
                R"(]},)"
                R"({"type":10,"elements":[]},)"
                R"({"type":7,"elements":[)"
                    R"({"type":8,"elements":"x"})"
                R"(]})"
            R"(]})"
        R"(]})" ) << json;
}

TEST_F(CalcTestSymbolicComplex, expand3)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"expand(pow((x+1),3));");
    ASSERT_TRUE(res.ToStdString(10) == "1.+3.*x+3.*pow(x,2.)+pow(x,3.)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"1"},{"type":11,"symbol":"+"},{"type":8,"elements":"3"},{"type":13,"symbol":"·"},{"type":8,"elements":"x"},{"type":11,"symbol":"+"},{"type":8,"elements":"3"},{"type":13,"symbol":"·"},{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"x"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]},{"type":11,"symbol":"+"},{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"x"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"3"}]}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, trig_power1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(pow(sin(x),2)+pow(cos(x),2));");
    ASSERT_TRUE(res.ToStdString(10) == "pow(cos(x),2.)+pow(sin(x),2.)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"cos"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]},{"type":11,"symbol":"+"},{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"sin"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, division_nested1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(x+1)/(x-1);");
    ASSERT_TRUE(res.ToStdString(10) == "(1.+x)/(-1.+x)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":47,"elements":[{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"1"},{"type":11,"symbol":"+"},{"type":8,"elements":"x"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":8,"elements":"1"},{"type":11,"symbol":"+"},{"type":8,"elements":"x"}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, diff_power5)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,5),x);");
    ASSERT_TRUE(res.ToStdString(10) == "5.*pow(x,4.)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"5"},{"type":13,"symbol":"·"},{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"x"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"4"}]}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, diff_cos)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(cos(x),x);");
    ASSERT_TRUE(res.ToStdString(10) == "-sin(x)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":7,"elements":[{"type":8,"elements":"sin"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, diff_nonsymbol)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x, x+1);"), yutovo_calculator::ParserException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x, 5);"), yutovo_calculator::ParserException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x, sin(x));"), yutovo_calculator::ParserException);
}

TEST_F(CalcTestSymbolicComplex, frac_power_mult)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x,(1/2))*pow(x,(1/3));");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x,0.833)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":47,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"x"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"0.833"}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, expand_diff_squares)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"expand((x+y)*(x-y));");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x,2.)-pow(y,2.)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"x"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]},{"type":12,"symbol":"-"},{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"y"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, log_ratio)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"log(2,4);");
    ASSERT_TRUE(res.ToStdString(10) == "2") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":47,"elements":[{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"4"},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"2"},{"type":20,"symbol":")"}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, sqrt_half)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sqrt(1/2);");
    ASSERT_TRUE(res.ToStdString(10) == "0.707") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"0.5"},{"type":13,"symbol":"·"},{"type":16,"elements":[{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, subs_add)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(x+y,y,2);");
    ASSERT_TRUE(res.ToStdString(10) == "2.+x") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"2"},{"type":11,"symbol":"+"},{"type":8,"elements":"x"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, evalf_exp)
{
    Parser<Symbolic<Complex>> parser10(10, Language::English);
    Symbolic<Complex> res = parser10.Parse(LogicalId{0, 0, 0, 0, 1}, U"evalf(exp(100));");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"2.6881171418"},{"type":13,"symbol":"·"},{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"10"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"43"}]}]}]}]})r" ) << json;
}

TEST_F(CalcTestSymbolicComplex, evalf_exp1000)
{
    Parser<Symbolic<Complex>> parser10(10, Language::English);
    Symbolic<Complex> res = parser10.Parse(LogicalId{0, 0, 0, 0, 1}, U"evalf(exp(1000));");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"∞"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, limit_cot0)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(cot(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, limit_sec_pi2)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(sec(x),x,(pi)/(2));");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, limit_csc0)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(csc(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, limit_sin_x)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(sin(x)/x,x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":47,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"nan\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, limit_exp_m1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs((exp(x)-1)/x,x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, limit_ln_x)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(ln(x)/x,x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":47,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"∞\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, limit_exp_1x)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(exp(1/x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, limit_sin_1x)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(sin(1/x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, limit_cos_1x)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(cos(1/x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, limit_ln_1x)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(ln(1/x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, limit_sqrt_noo)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sqrt(-∞);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, inf1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"inf;");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, inf2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"∞;");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, inf3)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(1)/(∞);");
    ASSERT_TRUE(res.ToStdString(10) == "0") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, inf4)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs((1)/(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, inf5)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(exp(x),x,-∞);");
    ASSERT_TRUE(res.ToStdString(10) == "0") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, inf6)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(ln(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, inf7)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(exp(x),x,∞);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, inf8)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs((1)/(x),x,∞);");
    ASSERT_TRUE(res.ToStdString(10) == "0") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, inf9)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs((1)/(x-1),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, inf10)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs((pow(x,2)-1)/(x-1),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, inf11)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-∞;");
    ASSERT_TRUE(res.ToStdString(10) == "-∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, nan1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"nan;");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, nan2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"nan+1;");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, nan3)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x*nan/3;");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, nan4)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(0)/(0);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, nan5)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(∞-∞,x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, nan6)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(0*∞,x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, nan7)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs((∞)/(∞),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, nan8)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(pow(∞,0),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "1") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, nan9)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(pow(0,0),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "1") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, division2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1 / 0;");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":47,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"∞\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, division3)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0 / 0;");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":47,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"nan\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, mul_neg_add)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x*(-1+y);");
    ASSERT_TRUE(res.ToStdString(10) == "x*(-1.+y)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json == R"xxx({"type":47,"elements":[{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":8,"elements":"x"},{"type":13,"symbol":"·"},{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"1"},{"type":12,"symbol":"-"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]}]}]})xxx") << json;
}

TEST_F(CalcTestSymbolicComplex, simplify2)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x+x;");
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 2}, U"simplify(f(x));");
    ASSERT_TRUE(res.ToString(10) == U"2.*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, simplify4)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(pow(x+y,2));");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x+y,2.)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":47,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"x"},{"type":11,"symbol":"+"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicComplex, simplify5)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(pow(x+y,2)+pow(x+y,2));");
    ASSERT_TRUE(res.ToStdString(10) == "2.*pow(x+y,2.)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"2"},{"type":13,"symbol":"·"},{"type":15,"elements":[{"type":7,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"x"},{"type":11,"symbol":"+"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicComplex, power_add_exp1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x+y, z);");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x+y,z)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":47,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"x"},{"type":11,"symbol":"+"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"z"}]}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicComplex, power_mul_base1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x*y, z);");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x*y,z)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":47,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"x"},{"type":13,"symbol":"·"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"z"}]}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicComplex, power_neg_exp1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x+y, -2);");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x+y,-2.)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":47,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"x"},{"type":11,"symbol":"+"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":8,"elements":"2"}]}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicComplex, nested_power1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(pow(x+y, 2), z);");
    ASSERT_TRUE(res.ToStdString(10) == "pow(pow(x+y,2.),z)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":47,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":15,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"x"},{"type":11,"symbol":"+"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"z"}]}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicComplex, power_chain)
{
    //power operator must be parsed by the 'power' rule, not by 'multiply'
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x,pow(y,z));");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x,y**z)") << res.ToStdString(10);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x,pow(y,z));");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x,y**z)") << res.ToStdString(10);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x,pow(y,z));");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x,y**z)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, sqrt2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sqrt(x);");
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s == "sqrt(x)") << s;
}

TEST_F(CalcTestSymbolicComplex, pow4)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-(1)/pow(10,100);");
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s == "-1E-100") << s;
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json == "{\"type\":47,\"elements\":[{\"type\":7,\"elements\":[{\"type\":12,\"symbol\":\"-\"},{\"type\":8,\"elements\":\"1\"},{\"type\":13,\"symbol\":\"·\"},{\"type\":15,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"10\"}]},{\"type\":10,\"elements\":[]},{\"type\":7,\"elements\":[{\"type\":7,\"elements\":[{\"type\":12,\"symbol\":\"-\"},{\"type\":8,\"elements\":\"100\"}]}]}]}]}]}") << json;
}

TEST_F(CalcTestSymbolicComplex, scientific1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.23e-5;");
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s == "1.23E-5" || s == "0.0000123") << s;
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-1.23e-5;");
    s = res.ToStdString(10);
    ASSERT_TRUE(s == "-1.23E-5" || s == "-0.0000123") << s;
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-1.23e+5;");
    ASSERT_TRUE(res.ToStdString(10) == "-123000") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.23e+5;");
    ASSERT_TRUE(res.ToStdString(10) == "123000") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, hyperbolic_sinh)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sinh(0);");
    ASSERT_TRUE(res.ToStdString(10) == "0") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, hyperbolic_cosh)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cosh(0);");
    ASSERT_TRUE(res.ToStdString(10) == "1") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, hyperbolic_tanh)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"tanh(0);");
    ASSERT_TRUE(res.ToStdString(10) == "0") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, hyperbolic_coth0)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(coth(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, hyperbolic_sech)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sech(0);");
    ASSERT_TRUE(res.ToStdString(10) == "1") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, hyperbolic_csch0)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(csch(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, hyperbolic_asinh)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arsinh(0);");
    ASSERT_TRUE(res.ToStdString(10) == "0") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, hyperbolic_acosh)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcosh(1);");
    ASSERT_TRUE(res.ToStdString(10) == "0") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, hyperbolic_atanh)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"artanh(0);");
    ASSERT_TRUE(res.ToStdString(10) == "0") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, hyperbolic_acoth1)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(arcoth(x),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, hyperbolic_asech)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arsech(1);");
    ASSERT_TRUE(res.ToStdString(10) == "0") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, hyperbolic_acsch0)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(arcsch(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, hyperbolic_symbolic)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sinh(x);");
    ASSERT_TRUE(res.ToStdString(10) == "sinh(x)") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcosh(x);");
    ASSERT_TRUE(res.ToStdString(10) == "acosh(x)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, hyperbolic_alias)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cosech(0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcosech(1);");
    ASSERT_TRUE(res.ToStdString(10) == "0.881") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, hyperbolic_inverse_synonyms)
{
    Symbolic<Complex> a = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"asinh(0);");
    Symbolic<Complex> b = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arsinh(0);");
    Symbolic<Complex> c = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcsinh(0);");
    ASSERT_TRUE(a.ToStdString(10) == "0") << a.ToStdString(10);
    ASSERT_TRUE(b.ToStdString(10) == "0") << b.ToStdString(10);
    ASSERT_TRUE(c.ToStdString(10) == "0") << c.ToStdString(10);

    a = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"acosh(1);");
    b = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcosh(1);");
    c = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arccosh(1);");
    ASSERT_TRUE(a.ToStdString(10) == "0") << a.ToStdString(10);
    ASSERT_TRUE(b.ToStdString(10) == "0") << b.ToStdString(10);
    ASSERT_TRUE(c.ToStdString(10) == "0") << c.ToStdString(10);

    a = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"atanh(0);");
    b = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"artanh(0);");
    c = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arctanh(0);");
    ASSERT_TRUE(a.ToStdString(10) == "0") << a.ToStdString(10);
    ASSERT_TRUE(b.ToStdString(10) == "0") << b.ToStdString(10);
    ASSERT_TRUE(c.ToStdString(10) == "0") << c.ToStdString(10);

    a = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(acoth(x),x,1);");
    b = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(arcoth(x),x,1);");
    c = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(arccoth(x),x,1);");
    ASSERT_TRUE(a.ToStdString(10) == "∞") << a.ToStdString(10);
    ASSERT_TRUE(b.ToStdString(10) == "∞") << b.ToStdString(10);
    ASSERT_TRUE(c.ToStdString(10) == "∞") << c.ToStdString(10);

    a = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"asech(1);");
    b = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arsech(1);");
    c = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcsech(1);");
    ASSERT_TRUE(a.ToStdString(10) == "0") << a.ToStdString(10);
    ASSERT_TRUE(b.ToStdString(10) == "0") << b.ToStdString(10);
    ASSERT_TRUE(c.ToStdString(10) == "0") << c.ToStdString(10);

    a = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(acsch(x),x,0);");
    b = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(arcsch(x),x,0);");
    c = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(arccsch(x),x,0);");
    Symbolic<Complex> d = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(arcosech(x),x,0);");
    Symbolic<Complex> e = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(arccosech(x),x,0);");
    ASSERT_TRUE(a.ToStdString(10) == "∞") << a.ToStdString(10);
    ASSERT_TRUE(b.ToStdString(10) == "∞") << b.ToStdString(10);
    ASSERT_TRUE(c.ToStdString(10) == "∞") << c.ToStdString(10);
    ASSERT_TRUE(d.ToStdString(10) == "∞") << d.ToStdString(10);
    ASSERT_TRUE(e.ToStdString(10) == "∞") << e.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, hyperbolic_acosh_nan)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcosh(nan);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, log_x_base)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"log(x,2);");
    ASSERT_TRUE(res.ToStdString(10) == "log2./log(x)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":47,"elements":[{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"2"},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, csc_tojson)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"csc(x);");
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"csc"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, sinh_tojson)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sinh(x);");
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"sinh"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, cos_expression_brackets)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cos(x-1);");
    ASSERT_TRUE(res.ToStdString(10) == "cos(1.-x)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"cos"},{"type":19,"symbol":"("},{"type":8,"elements":"1"},{"type":12,"symbol":"-"},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, ln_russian)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ln(x);");
    ASSERT_TRUE(res.ToStdString(10, Language::Russian) == "ln(x)") << res.ToStdString(10, Language::Russian);
    ASSERT_TRUE(res.ToJson(10, Language::Russian) ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"ln"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10, Language::Russian);
}

TEST_F(CalcTestSymbolicComplex, ln_in_expression_russian)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ln(x)+1;");
    ASSERT_TRUE(res.ToStdString(10, Language::Russian) == "1.+ln(x)") << res.ToStdString(10, Language::Russian);
    ASSERT_TRUE(res.ToJson(10, Language::Russian) ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"1"},{"type":11,"symbol":"+"},{"type":8,"elements":"ln"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10, Language::Russian);
}

TEST_F(CalcTestSymbolicComplex, ln_english)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ln(x);");
    ASSERT_TRUE(res.ToStdString(10) == "log(x)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, log_change_of_base_russian)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"log(2,4);");
    ASSERT_TRUE(res.ToStdString(10, Language::Russian) == "2") << res.ToStdString(10, Language::Russian);
    ASSERT_TRUE(res.ToJson(10, Language::Russian) ==
        R"r({"type":47,"elements":[{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"ln"},{"type":19,"symbol":"("},{"type":8,"elements":"4"},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"ln"},{"type":19,"symbol":"("},{"type":8,"elements":"2"},{"type":20,"symbol":")"}]}]}]})r" ) << res.ToJson(10, Language::Russian);
}

TEST_F(CalcTestSymbolicComplex, imag_unit1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"i;");
    ASSERT_TRUE(res.ToStdString(10) == "i") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"i"}]}]})" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, imag_unit2)
{
    parser.SetLocale(Language::English);
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"i*i;");
    ASSERT_TRUE(res.ToJson(10) == R"({"type":47,"elements":[{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":8,"elements":"1"}]}]})" ) << res.ToJson(10);
    ASSERT_TRUE(res.ToStdString(10) == "-1") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"j*j;");
    ASSERT_TRUE(res.ToStdString(10) == "pow(j,2.)") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"I*I;");
    ASSERT_TRUE(res.ToStdString(10) == "-1") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, imag_unit3)
{
    parser.SetLocale(Language::Russian);
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"i*i;");
    ASSERT_TRUE(res.ToStdString(10) == "-1") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == R"({"type":47,"elements":[{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":8,"elements":"1"}]}]})" ) << res.ToJson(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"j*j;");
    ASSERT_TRUE(res.ToStdString(10) == "-1") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == R"({"type":47,"elements":[{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":8,"elements":"1"}]}]})" ) << res.ToJson(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"I*I;");
    ASSERT_TRUE(res.ToStdString(10) == "-1") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == R"({"type":47,"elements":[{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":8,"elements":"1"}]}]})" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, imag_unit4)
{
    parser.SetLocale(Language::Spanish);
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"i*i;");
    ASSERT_TRUE(res.ToStdString(10) == "-1") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"j*j;");
    ASSERT_TRUE(res.ToStdString(10) == "pow(j,2.)") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"I*I;");
    ASSERT_TRUE(res.ToStdString(10) == "-1") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, imag_unit5)
{
    parser.SetLocale(Language::BrazilianPortuguese);
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"i*i;");
    ASSERT_TRUE(res.ToStdString(10) == "-1") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"j*j;");
    ASSERT_TRUE(res.ToStdString(10) == "pow(j,2.)") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"I*I;");
    ASSERT_TRUE(res.ToStdString(10) == "-1") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, imag_unit_minus_i)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-i;");
    ASSERT_TRUE(res.ToStdString(10) == "-i") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"({"type":47,"elements":[{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":8,"elements":"i"}]}]})" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, complex_1_plus_i)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1+i;");
    ASSERT_TRUE(res.ToStdString(10) == "1.+i") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"1"},{"type":11,"symbol":"+"},{"type":8,"elements":"i"}]}]})" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, complex_1_minus_i)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1-i;");
    ASSERT_TRUE(res.ToStdString(10) == "1.-i") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"1"},{"type":12,"symbol":"-"},{"type":8,"elements":"i"}]}]})" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, complex_2_plus_i)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2+i;");
    ASSERT_TRUE(res.ToStdString(10) == "2.+i") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"2"},{"type":11,"symbol":"+"},{"type":8,"elements":"i"}]}]})" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, complex_2_minus_i)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2-i;");
    ASSERT_TRUE(res.ToStdString(10) == "2.-i") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"2"},{"type":12,"symbol":"-"},{"type":8,"elements":"i"}]}]})" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, sin2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(0);");
    ASSERT_TRUE(res.ToStdString(10) == "0") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"0"}]}]})"
        ) << json;
}

TEST_F(CalcTestSymbolicComplex, sin3)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x);");
    ASSERT_TRUE(res.ToStdString(10) == "sin(x)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"sin"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r"
        ) << json;
}

TEST_F(CalcTestSymbolicComplex, cos2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cos(0);");
    ASSERT_TRUE(res.ToStdString(10) == "1") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"1"}]}]})"
        ) << json;
}

TEST_F(CalcTestSymbolicComplex, cos3)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cos(x);");
    ASSERT_TRUE(res.ToStdString(10) == "cos(x)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"cos"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r"
        ) << json;
}

TEST_F(CalcTestSymbolicComplex, exp1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"exp(0);");
    ASSERT_TRUE(res.ToStdString(10) == "1") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"1"}]}]})"
        ) << json;
}

TEST_F(CalcTestSymbolicComplex, exp2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"exp(x);");
    ASSERT_TRUE(res.ToStdString(10) == "exp(x)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"r({"type":47,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"e"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"x"}]}]}]})r"
        ) << json;
}

TEST_F(CalcTestSymbolicComplex, ln2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ln(1);");
    ASSERT_TRUE(res.ToStdString(10) == "0") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"0"}]}]})"
        ) << json;
}

TEST_F(CalcTestSymbolicComplex, ln3)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ln(x);");
    ASSERT_TRUE(res.ToStdString(10) == "log(x)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r"
        ) << json;
}

TEST_F(CalcTestSymbolicComplex, fact1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"5!;");
    ASSERT_TRUE(res.ToStdString(10) == "120") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"120"}]}]})"
        ) << json;
}

TEST_F(CalcTestSymbolicComplex, fact2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x!;");
    ASSERT_TRUE(res.ToStdString(10) == "gamma(1.+x)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"gamma"},{"type":19,"symbol":"("},{"type":8,"elements":"1"},{"type":11,"symbol":"+"},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r"
        ) << json;
}

TEST_F(CalcTestSymbolicComplex, fact3)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0!;");
    ASSERT_TRUE(res.ToStdString(10) == "1") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1!;");
    ASSERT_TRUE(res.ToStdString(10) == "1") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"10!;");
    ASSERT_TRUE(res.ToStdString(10) == "3628800") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, fact4)
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

TEST_F(CalcTestSymbolicComplex, gamma1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"gamma(5);");
    ASSERT_TRUE(res.ToStdString(10) == "24") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":47,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"24\"}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicComplex, gamma_tojson)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"gamma(x);");
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"gamma"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, min_max1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"min(x,y);");
    ASSERT_TRUE(res.ToStdString(10) == "min(x,y)") << res.ToStdString(10);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"max(x,y);");
    ASSERT_TRUE(res.ToStdString(10) == "max(x,y)") << res.ToStdString(10);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"min(1,2);");
    ASSERT_TRUE(res.ToStdString(10) == "1") << res.ToStdString(10);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"max(1,2);");
    ASSERT_TRUE(res.ToStdString(10) == "2") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, min_max2)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a=5;");
    parser.Parse(LogicalId{0, 0, 2}, U"b=3;");

    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 3}, U"min(a,b);");
    ASSERT_TRUE(res.ToStdString(10) == "3") << res.ToStdString(10);

    res = parser.Parse(LogicalId{0, 0, 4}, U"max(a,b);");
    ASSERT_TRUE(res.ToStdString(10) == "5") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, exp_derivative_e_base)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(e, 2*i*x), x);");
    std::string text = res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(text == "2.*i*exp(2.*i*x)") << text;
    ASSERT_TRUE(json ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"2"},{"type":13,"symbol":"·"},{"type":8,"elements":"i"},{"type":13,"symbol":"·"},{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"e"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"},{"type":13,"symbol":"·"},{"type":8,"elements":"i"},{"type":13,"symbol":"·"},{"type":8,"elements":"x"}]}]}]}]})r"
        ) << json;
}

TEST_F(CalcTestSymbolicComplex, e_times_x)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"e*x;");
    std::string text = res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(text == "e*x") << text;
    ASSERT_TRUE(json ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"e"},{"type":13,"symbol":"·"},{"type":8,"elements":"x"}]}]})r"
        ) << json;
}

TEST_F(CalcTestSymbolicComplex, derivative_at_point1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,2), [x=3]);");
    ASSERT_TRUE(res.ToStdString(10) == "6") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, derivative_at_point2)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x)=pow(x,2);");
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x), [x=3]);");
    ASSERT_TRUE(res.ToStdString(10) == "6") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, derivative_at_point3)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x,y)=x*y;");
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x,y), [x=3]);");
    ASSERT_TRUE(res.ToString(10) == U"y") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, derivative_at_point4)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x,y)=x*y;");
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x,y), [y=3]);");
    ASSERT_TRUE(res.ToString(10) == U"x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, derivative_at_point5)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(1/x, [x=0]);");
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":47,"elements":[{"type":7,"elements":[{"type":8,"elements":"∞"}]}]})r")
        << res.ToJson(10);
}

TEST_F(CalcTestSymbolicComplex, derivative_at_point_multi1)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x*y, [x=2, y=3]);");
    ASSERT_TRUE(res.ToStdString(10) == "3") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, derivative_at_point_multi2)
{
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x*y, [y=3, x=2]);");
    ASSERT_TRUE(res.ToStdString(10) == "2") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicComplex, derivative_at_point_multi3)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x,y)=pow(x,2)*y;");
    Symbolic<Complex> res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x,y), [x=2, y=3]);");
    ASSERT_TRUE(res.ToStdString(10) == "12") << res.ToStdString(10);
}

}
