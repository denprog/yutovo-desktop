/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include <condition_variable>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <thread>
#include "mock.h"

namespace yutovo_calc_test
{

using namespace yutovo_calculator;

TEST_F(CalcTestSymbolicReal, numbers1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1;", 3);
    ASSERT_TRUE(res.ToString(10) == U"1.") << res.ToStdString(10);

    res = parser.Parse(LogicalId{0, 0, 1}, U"0.2;", 3);
    ASSERT_TRUE(res.ToString(10) == U"0.2") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, numbers2)
{
    Symbolic res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"123456789012345678901234567890;");
    std::string s = res.ToStdString(30);
    ASSERT_TRUE(s == "123456789012345678901234567890") << s;
}

TEST_F(CalcTestSymbolicReal, numbers4)
{
    Symbolic res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"123456789012345678901234567890.123;");
    std::string s = res.ToStdString(20);
    ASSERT_TRUE(s == "1.235E+29") << s;
}

TEST_F(CalcTestSymbolicReal, numbers5)
{
    Symbolic res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"7890;");
    std::string s = res.ToStdString(3);
    ASSERT_TRUE(s == "7.89E+3") << s;
}

TEST_F(CalcTestSymbolicReal, numbers6)
{
    Symbolic res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-7890;");
    std::string s = res.ToStdString(3);
    ASSERT_TRUE(s == "-7.89E+3") << s;
}

TEST_F(CalcTestSymbolicReal, numbers7)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2.3;", 3);
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s == "2.3") << s;
}

TEST_F(CalcTestSymbolicReal, numbers8)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-2.3;", 3);
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s == "-2.3") << s;
}

TEST_F(CalcTestSymbolicReal, numbers9)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2.3*x;", 3);
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s.find("2.3") != std::string::npos) << s;
}

TEST_F(CalcTestSymbolicReal, numbers10)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"."), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"..2"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1..2"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.2.3"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2.3.4"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U".1.2"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.2."), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestSymbolicReal, eval1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x;");
    ASSERT_TRUE(res.ToString(10) == U"x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, eval2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x+x;");
    ASSERT_TRUE(res.ToString(10) == U"2.*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, eval3)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"y+2*y+2;");
    ASSERT_TRUE(res.ToString(10) == U"2.+3.*y") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, eval4)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"123456789012345678901234567890.123+x;");
    std::string s = res.ToStdString(20);
    ASSERT_TRUE(s == "1.235E+29+x") << s;
}

TEST_F(CalcTestSymbolicReal, eval5)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x)+2356.78*x;");
    std::string s = res.ToStdString(3);
    ASSERT_TRUE(s == "2.357E+3*x+sin(x)") << s;
}

TEST_F(CalcTestSymbolicReal, eval6)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x)-2356.78*x;");
    std::string s = res.ToStdString(3);
    ASSERT_TRUE(s == "-2.357E+3*x+sin(x)") << s;
}

TEST_F(CalcTestSymbolicReal, eval7)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x+(x)/(3);");
    std::string s = res.ToStdString(3);
    ASSERT_TRUE(s == "1.333*x") << s;
    std::string j = res.ToJson(3);
    ASSERT_TRUE(j.find("1333") == std::string::npos) << j;
}

TEST_F(CalcTestSymbolicReal, eval8)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"+-x;");
    ASSERT_TRUE(res.ToString(10) == U"-x") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":12,\"symbol\":\"-\"},{\"type\":8,\"elements\":\"x\"}]}]}") << 
        res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, eval9)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"+x;");
    ASSERT_TRUE(res.ToString(10) == U"x") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"x\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, eval10)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"--x;");
    ASSERT_TRUE(res.ToString(10) == U"x") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"x\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, eval11)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"++x;");
    ASSERT_TRUE(res.ToString(10) == U"x") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"x\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, eval12)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x)--2356.78*x;");
    std::string s = res.ToStdString(3);
    ASSERT_TRUE(s == "2.357E+3*x+sin(x)") << s;
}

TEST_F(CalcTestSymbolicReal, eval13)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x)-+2356.78*x;");
    std::string s = res.ToStdString(3);
    ASSERT_TRUE(s == "-2.357E+3*x+sin(x)") << s;
}

TEST_F(CalcTestSymbolicReal, eval14)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-sin(x)-2356.78*x;");
    std::string s = res.ToStdString(3);
    ASSERT_TRUE(s == "-2.357E+3*x-sin(x)") << s;
}

TEST_F(CalcTestSymbolicReal, eval15)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"--+sin(x)--+2356.78*x;");
    std::string s = res.ToStdString(3);
    ASSERT_TRUE(s == "2.357E+3*x+sin(x)") << s;
}

TEST_F(CalcTestSymbolicReal, unary_plus1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"+x;", 3);
    ASSERT_TRUE(res.ToString(10) == U"x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, unary_minus1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-x;");
    ASSERT_TRUE(res.ToString(10) == U"-x") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":12,\"symbol\":\"-\"},{\"type\":8,\"elements\":\"x\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, addition1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x + 1;");
    ASSERT_TRUE(res.ToString(10) == U"1.+x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, subtraction1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x - 1;");
    ASSERT_TRUE(res.ToString(10) == U"-1.+x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, multiplication1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x * 2;");
    ASSERT_TRUE(res.ToString(10) == U"2.*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, division1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x / 3;");
    ASSERT_TRUE(res.ToString(10) == U"0.333*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, division2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1 / 0;");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"∞\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, division3)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0 / 0;");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"nan\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, inf1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"inf;");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, inf2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"∞;");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, inf3)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(1)/(∞);");
    ASSERT_TRUE(res.ToStdString(10) == "0.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, inf4)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs((1)/(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, inf5)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(exp(x),x,-∞);");
    ASSERT_TRUE(res.ToStdString(10) == "0.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, inf6)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(ln(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, inf7)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(exp(x),x,∞);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, inf8)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs((1)/(x),x,∞);");
    ASSERT_TRUE(res.ToStdString(10) == "0.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, inf9)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs((1)/(x-1),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, inf10)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs((pow(x,2)-1)/(x-1),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, inf11)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-∞;");
    ASSERT_TRUE(res.ToStdString(10) == "-∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, nan1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"nan;");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, nan2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"nan+1;");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, nan3)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x*nan/3;");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, nan4)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(0)/(0);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, nan5)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(∞-∞,x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, nan6)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(0*∞,x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, nan7)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs((∞)/(∞),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, nan8)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(pow(∞,0),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "1.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, nan9)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(pow(0,0),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "1.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, mixed_arithmetic1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x + 2*x;");
    ASSERT_TRUE(res.ToString(10) == U"3.*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, power1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x, 2);");
    ASSERT_TRUE(res.ToString(10) == U"pow(x,2.)") << res.ToStdString(10);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x + 1, 2);");
    ASSERT_TRUE(res.ToString(10) == U"pow(1.+x,2.)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, power2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(1)/(pow(10,100));");
    ASSERT_TRUE(res.ToStdString(10) == "1E-100") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"1\"},{\"type\":13,\"symbol\":\"·\"},{\"type\":15,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"10\"}]},{\"type\":10,\"elements\":[]},{\"type\":7,\"elements\":[{\"type\":7,\"elements\":[{\"type\":12,\"symbol\":\"-\"},{\"type\":8,\"elements\":\"100\"}]}]}]}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, power3)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-(1)/pow(10,100);");
    ASSERT_TRUE(res.ToStdString(10) == "-1E-100") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":12,\"symbol\":\"-\"},{\"type\":8,\"elements\":\"1\"},{\"type\":13,\"symbol\":\"·\"},{\"type\":15,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"10\"}]},{\"type\":10,\"elements\":[]},{\"type\":7,\"elements\":[{\"type\":7,\"elements\":[{\"type\":12,\"symbol\":\"-\"},{\"type\":8,\"elements\":\"100\"}]}]}]}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, sin1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x);");
    ASSERT_TRUE(res.ToString(10) == U"sin(x)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, cos1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cos(x);");
    ASSERT_TRUE(res.ToString(10) == U"cos(x)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, log1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"log(2, 8);");
    ASSERT_TRUE(res.ToStdString(10) == "3.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, log2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"log(x,y);");
    ASSERT_TRUE(res.ToStdString(10, Language::Russian) == "ln(y)/ln(x)") << res.ToStdString(10, Language::Russian);
    ASSERT_TRUE(res.ToJson(10, Language::Russian) ==
        R"r({"type":45,"elements":[{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"ln"},{"type":19,"symbol":"("},{"type":8,"elements":"y"},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"ln"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]}]})r" ) << res.ToJson(10, Language::Russian);
}

TEST_F(CalcTestSymbolicReal, root1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"root(8, 3);");
    ASSERT_TRUE(res.ToString(10) == U"2.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, expand1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"expand(pow(x + 1, 2));");
    ASSERT_TRUE(res.ToString(10) == U"1.+2.*x+pow(x,2.)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, expand2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"expand(pow((pow(x,3)+x+3),2));");
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(!s.empty());
}

TEST_F(CalcTestSymbolicReal, expand3)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"expand(pow((x+1),3));");
    ASSERT_TRUE(res.ToStdString(10) == "1.+3.*x+3.*pow(x,2.)+pow(x,3.)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"1"},{"type":11,"symbol":"+"},{"type":8,"elements":"3"},{"type":13,"symbol":"·"},{"type":8,"elements":"x"},{"type":11,"symbol":"+"},{"type":8,"elements":"3"},{"type":13,"symbol":"·"},{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"x"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]},{"type":11,"symbol":"+"},{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"x"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"3"}]}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, expand4)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"expand((x+y)*(x-y));");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x,2.)-pow(y,2.)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"x"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]},{"type":12,"symbol":"-"},{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"y"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, sin_pi_12)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(sin(pi/12));");
    ASSERT_TRUE(res.ToStdString(10) == "0.259") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json == R"xxx({"type":45,"elements":[{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":8,"elements":"0.25"},{"type":13,"symbol":"·"},{"type":16,"elements":[{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]},{"type":13,"symbol":"·"},{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"1"},{"type":12,"symbol":"-"},{"type":16,"elements":[{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"3"}]}]}]},{"type":20,"symbol":")"}]}]}]})xxx") << json;
}

TEST_F(CalcTestSymbolicReal, mul_neg_add)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x*(-1+y);");
    ASSERT_TRUE(res.ToStdString(10) == "x*(-1.+y)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json == R"xxx({"type":45,"elements":[{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":8,"elements":"x"},{"type":13,"symbol":"·"},{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"1"},{"type":12,"symbol":"-"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]}]}]})xxx") << json;
}

TEST_F(CalcTestSymbolicReal, simplify1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(x + x);");
    ASSERT_TRUE(res.ToString(10) == U"2.*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, simplify2)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x+x;");
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 2}, U"simplify(f(x));");
    ASSERT_TRUE(res.ToString(10) == U"2.*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, simplify3)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(pow(sin(x),2)+pow(cos(x),2));");
    ASSERT_TRUE(res.ToStdString(10) == "pow(cos(x),2.)+pow(sin(x),2.)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"cos"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]},{"type":11,"symbol":"+"},{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"sin"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, simplify4)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(pow(x+y,2));");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":45,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"x"},{"type":11,"symbol":"+"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicReal, simplify5)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(pow(x+y,2)+pow(x+y,2));");
    ASSERT_TRUE(res.ToStdString(10) == "2.*pow(x+y,2.)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"2"},{"type":13,"symbol":"·"},{"type":15,"elements":[{"type":7,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"x"},{"type":11,"symbol":"+"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicReal, simplify6)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(pow(x-1,2)/(x-1));");
    ASSERT_TRUE(res.ToString(10) == U"-1.+x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, simplify7)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x,2)*pow(x,4);");
    ASSERT_TRUE(res.ToString(10) == U"pow(x,6.)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, simplify8)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(pow(x,5))/(pow(x,2));");
    ASSERT_TRUE(res.ToString(10) == U"pow(x,3.)") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify((pow(x,5))/(pow(x,2)));");
    ASSERT_TRUE(res.ToString(10) == U"pow(x,3.)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, simplify9)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(1-pow(cos(x),2));");
    ASSERT_TRUE(res.ToStdString(10) == "(-(-1.+cos(x)))*(1.+cos(x))") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, simplify10)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(pow((x+y),2)-pow(x,2)-2*x*y);");
    ASSERT_TRUE(res.ToStdString(10) == "pow(y,2.)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, simplify11)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"simplify(pow((x+y),2)+pow((x+y),2));");
    ASSERT_TRUE(res.ToStdString(10) == "2.*pow(x+y,2.)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, simplify_wrong_args)
{
    try
    {
        parser.Parse(LogicalId{0, 0, 1}, U"simplify();", 10);
    }
    catch (SyntaxException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 1}) && ex.ex_id == ParserExceptionCode::WrongArgumentsCount &&
                    ex.pos == 0 && ex.size == 10) << ex.ex_id;
        return;
    }
    ASSERT_FALSE(true);
}

TEST_F(CalcTestSymbolicReal, pi_symbol_output)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"123(ee)/(π);");
    ASSERT_TRUE(res.ToStdString(10) == "123.*ee/π") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, tmp_editor_json)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"123*ee/п;");
    std::cerr << "TEXT: " << res.ToStdString(10) << std::endl;
    std::cerr << "JSON: " << res.ToJson(10) << std::endl;
}

TEST_F(CalcTestSymbolicReal, power_mul_base1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x*y, z);");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x*y,z)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":45,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"x"},{"type":13,"symbol":"·"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"z"}]}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicReal, power_add_exp1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x+y, z);");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x+y,z)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":45,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"x"},{"type":11,"symbol":"+"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"z"}]}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicReal, power_neg_exp1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x+y, -2);");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x+y,-2.)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":45,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"x"},{"type":11,"symbol":"+"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":8,"elements":"2"}]}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicReal, nested_power1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(pow(x+y, 2), z);");
    ASSERT_TRUE(res.ToStdString(10) == "pow(pow(x+y,2.),z)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":45,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":15,"elements":[{"type":7,"elements":[{"type":19,"symbol":"("},{"type":7,"elements":[{"type":8,"elements":"x"},{"type":11,"symbol":"+"},{"type":8,"elements":"y"}]},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"z"}]}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicReal, derivative1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x, 2), x);");
    ASSERT_TRUE(res.ToString(10) == U"2.*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, derivative_diff_synonym)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"diff(pow(x, 2), x);");
    ASSERT_TRUE(res.ToString(10) == U"2.*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, derivative2)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x+x;");
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 2}, U"derivative(f(x), x);");
    ASSERT_TRUE(res.ToString(10) == U"2.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, derivative3)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=pow(x,2);");
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 2}, U"derivative(f(x), x);");
    ASSERT_TRUE(res.ToString(10) == U"2.*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, derivative4)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(sin(x), x);");
    ASSERT_TRUE(res.ToString(10) == U"cos(x)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, indefinite_integral1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"indefinite_integral(pow(x,2), x);");
    ASSERT_TRUE(res.ToStdString(10) == "0.333*pow(x,3.)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, indefinite_integral2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"indefinite_integral(sin(x), x);");
    ASSERT_TRUE(res.ToStdString(10) == "-cos(x)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, indefinite_integral3)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"indefinite_integral(x*y, y);");
    ASSERT_TRUE(res.ToStdString(10) == "0.5*x*pow(y,2.)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, indefinite_integral_nonsymbol)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"indefinite_integral(x, 1);"), yutovo_calculator::ParserException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"indefinite_integral(x, x+1);"), yutovo_calculator::ParserException);
}

TEST_F(CalcTestSymbolicReal, definite_integral_simplify1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(2,34,3y,x);");
    ASSERT_TRUE(res.ToStdString(10) == "96.*y") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, subs1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(pow(x, 2), x, 5);");
    ASSERT_TRUE(res.ToString(10) == U"25.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, subs2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(pow(yy, 3), yy, 5);");
    ASSERT_TRUE(res.ToStdString(10) == "125.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, subs3)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(pow(x,2)+pow(y,2), x, 3);");
    ASSERT_TRUE(res.ToString(10) == U"9.+pow(y,2.)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"9\"},{\"type\":11,\"symbol\":\"+\"},{\"type\":15,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"y\"}]},{\"type\":10,\"elements\":[]},{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"2\"}]}]}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, subs4)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(log(x,2),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"∞\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, subs5)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(x+y,y,2);");
    ASSERT_TRUE(res.ToStdString(10) == "2.+x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, subs6)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(log(2,x),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "0.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, subs7)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(log(x,2),x,2);");
    ASSERT_TRUE(res.ToStdString(10) == "1.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, definite_integral1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,1,x*y,y);", 10);
    ASSERT_TRUE(res.ToStdString(10) == "0.5*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, definite_integral_nonsymbol)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,1,x,1);"), yutovo_calculator::ParserException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,1,x,x+y);"), yutovo_calculator::ParserException);
}

TEST_F(CalcTestSymbolicReal, definite_integral_inf_divergent)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(1,∞,1/x,x);", 10);
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, subscript1)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x{2};"), SyntaxException);
}

TEST_F(CalcTestSymbolicReal, subscript2)
{
    //text subscript
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x{us};"), SyntaxException);
}

TEST_F(CalcTestSymbolicReal, subscript3)
{
    //multi-character subscript
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x{ab};"), SyntaxException);
}

TEST_F(CalcTestSymbolicReal, subscript4)
{
    //expression with multiple variables, one subscripted
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x{2}+y;"), SyntaxException);
}

TEST_F(CalcTestSymbolicReal, subscript5)
{
    //subscript in implicit multiplication
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2x{2};"), SyntaxException);
}

TEST_F(CalcTestSymbolicReal, subscript6)
{
    //subscript as function argument
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x{2});"), SyntaxException);
}

TEST_F(CalcTestSymbolicReal, infinity_syntax_error)
{
    for (size_t n = 2; n <= 5; ++n)
    {
        std::u32string expr(n, U'∞');
        expr += U';';
        ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, expr), SyntaxException) << "n=" << n;
    }
}

TEST_F(CalcTestSymbolicReal, infinity_in_identifier)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x∞;"), SyntaxException);
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"∞x;"), SyntaxException);
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ab∞;"), SyntaxException);
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"∞ab;"), SyntaxException);
}

TEST_F(CalcTestSymbolicReal, infinity_in_number)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1∞;"), SyntaxException);
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"12.3∞;"), SyntaxException);
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"∞1;"), SyntaxException);
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"∞12.3;"), SyntaxException);
}

TEST_F(CalcTestSymbolicReal, evalf1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"evalf(pi);", 3);
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s == "3.142") << s;
}

TEST_F(CalcTestSymbolicReal, evalf2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"evalf(pi);");
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s == "3.142") << s;
}

TEST_F(CalcTestSymbolicReal, evalf3)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"evalf(pi);", 20);
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s == "3.14159265358979323846") << s;
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"3.14159265358979323846\"}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, evalf4)
{
    Parser<Symbolic<Real>> parser10(10, Language::English);
    Symbolic<Real> res = parser10.Parse(LogicalId{0, 0, 0, 0, 1}, U"evalf(exp(100));");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"2.6881171418"},{"type":13,"symbol":"·"},{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"10"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"43"}]}]}]}]})r" ) << json;
}

TEST_F(CalcTestSymbolicReal, evalf5)
{
    Parser<Symbolic<Real>> parser10(10, Language::English);
    Symbolic<Real> res = parser10.Parse(LogicalId{0, 0, 0, 0, 1}, U"evalf(exp(1000));");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"∞"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, evalf6)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"evalf(x*sqrt(pow(10,9)+1)-x*sqrt(pow(10,9)));", 10);
    ASSERT_TRUE(res.ToStdString(10) == "0.0000158114*x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, precision1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"evalf(pi);", 15);
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s.length() == 17) << s;
    ASSERT_TRUE(s.find("3.141592653589793") != std::string::npos) << s;
}

TEST_F(CalcTestSymbolicReal, precision2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.23456789012345678;", 15);
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s.length() == 17) << s;
    ASSERT_TRUE(s.find("1.234567890123457") != std::string::npos) << s;
}

TEST_F(CalcTestSymbolicReal, member_checks1)
{
    Symbolic res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0;");
    ASSERT_TRUE(res.IsZero());
    ASSERT_TRUE(res.IsNumber());

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"5;");
    ASSERT_FALSE(res.IsZero());
    ASSERT_TRUE(res.IsNumber());

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x;");
    ASSERT_FALSE(res.IsZero());
    ASSERT_FALSE(res.IsNumber());

    //parse without explicit precision uses -1; use explicit precision to check setter
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x;", 10);
    ASSERT_EQ(res.GetPrecision(), 10);
}

TEST_F(CalcTestSymbolicReal, logical_not)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"!x;"), SyntaxException);
}

TEST_F(CalcTestSymbolicReal, logical_and)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x && y;"), SyntaxException);
}

TEST_F(CalcTestSymbolicReal, logical_or)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x || y;"), SyntaxException);
}

TEST_F(CalcTestSymbolicReal, relational_eq)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x == y;"), SyntaxException);
}

TEST_F(CalcTestSymbolicReal, relational_ne)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x != y;"), SyntaxException);
}

TEST_F(CalcTestSymbolicReal, relational_lt)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x < y;"), SyntaxException);
}

TEST_F(CalcTestSymbolicReal, relational_le)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x <= y;"), SyntaxException);
}

TEST_F(CalcTestSymbolicReal, relational_gt)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x > y;"), SyntaxException);
}

TEST_F(CalcTestSymbolicReal, relational_ge)
{
    ASSERT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x >= y;"), SyntaxException);
}

TEST_F(CalcTestSymbolicReal, variables1)
{
    std::vector<std::u32string> dependencies;
    parser.Parse(LogicalId{0, 0, 1}, U"v=555;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"v;") == parser.Parse(LogicalId{0, 0, 2}, U"555;")) <<
        parser.Parse(LogicalId{0, 0, 2}, U"v;").ToStdString(10);
    parser.Parse(LogicalId{0, 0, 3}, U"vp=x;", &dependencies);
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 3, 0}, U"vp+5;", &dependencies) == parser.Parse(LogicalId{0, 0, 3, 0}, U"x+5;")) <<
        parser.Parse(LogicalId{0, 0, 3, 0}, U"vp+5;").ToStdString(10);
    ASSERT_TRUE(std::find(dependencies.begin(), dependencies.end(), U"vp") != dependencies.end());
}

TEST_F(CalcTestSymbolicReal, variables2)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a=5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"a;") == parser.Parse(LogicalId{0, 0, 2}, U"5;")) << parser.Parse(LogicalId{0, 0, 2}, U"a;").ToStdString(10);
    parser.Parse(LogicalId{0, 0, 3}, U"a=7;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"a;") == parser.Parse(LogicalId{0, 0, 4}, U"7;")) << parser.Parse(LogicalId{0, 0, 4}, U"a;").ToStdString(10);
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"a+1;") == parser.Parse(LogicalId{0, 0, 2}, U"6;")) << parser.Parse(LogicalId{0, 0, 2}, U"a+1;").ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, variables3)
{
    parser.Parse(LogicalId{0, 0, 2}, U"a=77;");
    parser.Parse(LogicalId{0, 0, 1}, U"a=5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 3}, U"a;") == parser.Parse(LogicalId{0, 0, 3}, U"77;")) << parser.Parse(LogicalId{0, 0, 3}, U"a;").ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, variables4)
{
    parser.Parse(LogicalId{0, 0, 1}, U"d=4;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"d+5;") == parser.Parse(LogicalId{0, 0, 2}, U"9;")) << parser.Parse(LogicalId{0, 0, 2}, U"d+5;").ToStdString(10);
    parser.Parse(LogicalId{0, 0, 3}, U"d=45+d;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"d+5;") == parser.Parse(LogicalId{0, 0, 4}, U"54;")) << parser.Parse(LogicalId{0, 0, 4}, U"d+5;").ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, variables5)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a=5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"a*x;") == parser.Parse(LogicalId{0, 0, 2}, U"5*x;")) << parser.Parse(LogicalId{0, 0, 2}, U"a*x;").ToStdString(10);
    parser.Parse(LogicalId{0, 0, 3}, U"b=x+1;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"a*b;") == parser.Parse(LogicalId{0, 0, 4}, U"5*(1+x);")) << parser.Parse(LogicalId{0, 0, 4}, U"a*b;").ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, variables7)
{
    //in Symbolic mode, unknown identifiers become symbols
    Symbolic res = parser.Parse(LogicalId{0, 0, 1}, U"sin;");
    ASSERT_TRUE(res.ToStdString(10) == "sin") << res.ToStdString(10);

    parser.Parse(LogicalId{0, 0, 1}, U"sin=2;");
    res = parser.Parse(LogicalId{0, 0, 2}, U"sin;");
    ASSERT_TRUE(res.ToStdString(10) == "2.") << res.ToStdString(10);

    res = parser.Parse(LogicalId{0, 0, 3}, U"sin(2);");
    ASSERT_TRUE(res.ToStdString(10) == "0.909") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, variables8)
{
    parser.Parse(LogicalId{0, 0, 5}, U"b=4;");
    parser.Parse(LogicalId{0, 0, 3}, U"b=3;");
    parser.Parse(LogicalId{0, 0, 1}, U"b=2;");
    Symbolic res = parser.Parse(LogicalId{0, 0, 6}, U"b;");
    ASSERT_TRUE(res.ToStdString(10) == "4.") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 4}, U"b;");
    ASSERT_TRUE(res.ToStdString(10) == "3.") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 2}, U"b;");
    ASSERT_TRUE(res.ToStdString(10) == "2.") << res.ToStdString(10);

    parser.Parse(LogicalId{0, 0, 1}, U"b=5;");
    res = parser.Parse(LogicalId{0, 0, 2}, U"b;");
    ASSERT_TRUE(res.ToStdString(10) == "5.") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 4}, U"b;");
    ASSERT_TRUE(res.ToStdString(10) == "3.") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 6}, U"b;");
    ASSERT_TRUE(res.ToStdString(10) == "4.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, variables10)
{
    yutovo_calculator::ParserContext parser_context;
    parser.Parse(LogicalId{0, 0, 1}, U"v=555;", &parser_context);
    ASSERT_TRUE(parser_context.no_result == true);
    parser.Parse(LogicalId{0, 0, 2}, U"v;", &parser_context);
    ASSERT_TRUE(parser_context.no_result == false);
}

TEST_F(CalcTestSymbolicReal, variables11)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a=1/2;");
    Symbolic res = parser.Parse(LogicalId{0, 0, 2}, U"a+x;");
    ASSERT_TRUE(res.ToStdString(10) == "0.5+x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, user_functions1)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=5;");
    parser.Parse(LogicalId{0, 0, 2}, U"f(x)=x+5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 3}, U"f(2);") == parser.Parse(LogicalId{0, 0, 3}, U"7;")) <<
        parser.Parse(LogicalId{0, 0, 3}, U"f(2);").ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, user_functions2)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"f(2);") == parser.Parse(LogicalId{0, 0, 2}, U"2;")) <<
        parser.Parse(LogicalId{0, 0, 2}, U"f(2);").ToStdString(10);
    parser.Parse(LogicalId{0, 0, 3}, U"f(x)=x+7;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"f(2);") == parser.Parse(LogicalId{0, 0, 4}, U"9;")) <<
        parser.Parse(LogicalId{0, 0, 4}, U"f(2);").ToStdString(10);
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 2}, U"f(3);") == parser.Parse(LogicalId{0, 0, 2}, U"3;")) <<
        parser.Parse(LogicalId{0, 0, 2}, U"f(3);").ToStdString(10);
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 4}, U"f(3);") == parser.Parse(LogicalId{0, 0, 4}, U"10;")) <<
        parser.Parse(LogicalId{0, 0, 4}, U"f(10);").ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, user_functions3)
{
    std::vector<std::u32string> dependencies;
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x*2;");
    parser.Parse(LogicalId{0, 0, 2}, U"f(5);", &dependencies);
    ASSERT_TRUE(std::find(dependencies.begin(), dependencies.end(), U"f") != dependencies.end());
}

TEST_F(CalcTestSymbolicReal, user_functions4)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x;");
    Symbolic res = parser.Parse(LogicalId{0, 0, 2}, U"f(5);");
    ASSERT_TRUE(res.ToStdString(10) == "5.") << res.ToStdString(10);

    parser.Parse(LogicalId{0, 0, 3}, U"f(x,y)=x+y;");
    res = parser.Parse(LogicalId{0, 0, 4}, U"f(5,4);");
    ASSERT_TRUE(res.ToStdString(10) == "9.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, user_functions5)
{
    parser.Parse(LogicalId{0, 0, 5}, U"p(a)=a+1;");
    parser.Parse(LogicalId{0, 0, 1}, U"p(a)=a+5;");
    Symbolic res = parser.Parse(LogicalId{0, 0, 6}, U"p(5);");
    ASSERT_TRUE(res.ToStdString(10) == "6.") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 2}, U"p(5);");
    ASSERT_TRUE(res.ToStdString(10) == "10.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, user_functions6)
{
    parser.Parse(LogicalId{0, 0, 8}, U"p(a)=a+15;");
    Symbolic res = parser.Parse(LogicalId{0, 0, 9}, U"p(5);");
    ASSERT_TRUE(res.ToStdString(10) == "20.") << res.ToStdString(10);
    parser.Parse(LogicalId{0, 0, 5}, U"p(a)=a+1;");
    parser.Parse(LogicalId{0, 0, 1}, U"p(a)=a+5;");
    res = parser.Parse(LogicalId{0, 0, 2}, U"p(5);");
    ASSERT_TRUE(res.ToStdString(10) == "10.") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 6}, U"p(5);");
    ASSERT_TRUE(res.ToStdString(10) == "6.") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 2}, U"p(5);");
    ASSERT_TRUE(res.ToStdString(10) == "10.") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 9}, U"p(5);");
    ASSERT_TRUE(res.ToStdString(10) == "20.") << res.ToStdString(10);
    parser.Parse(LogicalId{0, 0, 12}, U"p(a)=a+25;");
    res = parser.Parse(LogicalId{0, 0, 9}, U"p(5);");
    ASSERT_TRUE(res.ToStdString(10) == "20.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, user_functions7)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x,y)=pow(x,y);");
    Symbolic res = parser.Parse(LogicalId{0, 0, 2}, U"f(5,2);");
    ASSERT_TRUE(res.ToStdString(10) == "25.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, user_functions8)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x,y)=(x)/(y);");
    Symbolic res = parser.Parse(LogicalId{0, 0, 2}, U"f(8,2);");
    ASSERT_TRUE(res.ToStdString(10) == "4.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, user_functions9)
{
    parser.Parse(LogicalId{0, 0, 1}, U"y=x+1;");
    parser.Parse(LogicalId{0, 0, 2}, U"r=pow(y,2);");
    Symbolic res = parser.Parse(LogicalId{0, 0, 3}, U"r;");
    ASSERT_TRUE(res.ToStdString(10) == "pow(1.+x,2.)") << res.ToStdString(10);
    parser.Parse(LogicalId{0, 0, 4}, U"f(t)=t+r;");
    res = parser.Parse(LogicalId{0, 0, 5}, U"f(3);");
    ASSERT_TRUE(res.ToStdString(10) == "3.+pow(1.+x,2.)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, real1)
{
    Real re(53, 3.5f);
    Symbolic s(53, re);
    ASSERT_TRUE(s.ToStdString(10) == "3.5") << s.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, real2)
{
    Real re(53, 1.5f);
    Unit u(U"m");
    re.SetUnit(u);
    ASSERT_THROW(Symbolic(53, re), ParserException);
}

TEST_F(CalcTestSymbolicReal, errors1)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"sin(x;"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestSymbolicReal, errors2)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"sinx);"), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestSymbolicReal, tojson1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1/x;");
    std::string json = res.ToJson(10);
    std::string expected = 
        "{\"type\":45,\"elements\":["
            "{\"type\":15,\"elements\":["
                "{\"type\":7,\"elements\":["
                    "{\"type\":8,\"elements\":\"x\"}"
                "]},"
                "{\"type\":10,\"elements\":[]},"
                "{\"type\":7,\"elements\":["
                    "{\"type\":12,\"symbol\":\"-\"},"
                    "{\"type\":8,\"elements\":\"1\"}"
                "]}"
            "]}"
        "]}";
    ASSERT_EQ(json, expected) << json;
}

TEST_F(CalcTestSymbolicReal, tojson_exp1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"12345;");
    std::string json = res.ToJson(2);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"1.234\"},{\"type\":13,\"symbol\":\"·\"},{\"type\":15,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"10\"}]},{\"type\":10,\"elements\":[]},{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"4\"}]}]}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, tojson_exp2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"12345;");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"12345\"}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, tojson_exp3)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0.00123;");
    std::string json = res.ToJson(2);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"0.001\"}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, tojson_exp4)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"123.45;");
    std::string json = res.ToJson(2);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"1.235\"},{\"type\":13,\"symbol\":\"·\"},{\"type\":15,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"10\"}]},{\"type\":10,\"elements\":[]},{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"2\"}]}]}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, tojson_exp5)
{
    Parser<Symbolic<Real>> parser15(15, Language::English);
    Symbolic<Real> res = parser15.Parse(LogicalId{0, 0, 0, 0, 1}, U"0.000000000000123;");
    std::string json = res.ToJson(2);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"0.000000000000123\"}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, tojson_exp6)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1234567890123456787*x;");
    std::string json = res.ToJson(2);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"1.235\"},{\"type\":13,\"symbol\":\"·\"},{\"type\":15,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"10\"}]},{\"type\":10,\"elements\":[]},{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"18\"}]}]},{\"type\":13,\"symbol\":\"·\"},{\"type\":8,\"elements\":\"x\"}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, tojson_decimal1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2.3;");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"2.3\"}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, tojson_decimal2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-2.3;");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":12,\"symbol\":\"-\"},{\"type\":8,\"elements\":\"2.3\"}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, sqrt1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sqrt(4);");
    ASSERT_TRUE(res.ToStdString(10) == "2.") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"2\"}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, sqrt2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sqrt(x);");
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s == "sqrt(x)") << s;
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":16,\"elements\":[{\"type\":10,\"elements\":[]},{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"x\"}]}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, pow_operator1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x,3);");
    ASSERT_TRUE(res.ToString(10) == U"pow(x,3.)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":15,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"x\"}]},{\"type\":10,\"elements\":[]},{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"3\"}]}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, derivative_complex1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,3)+2*pow(x,2)-5*x+1, x);");
    ASSERT_TRUE(res.ToString(10) == U"-5.+4.*x+3.*pow(x,2.)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":12,\"symbol\":\"-\"},{\"type\":8,\"elements\":\"5\"},{\"type\":11,\"symbol\":\"+\"},{\"type\":8,\"elements\":\"4\"},{\"type\":13,\"symbol\":\"·\"},{\"type\":8,\"elements\":\"x\"},{\"type\":11,\"symbol\":\"+\"},{\"type\":8,\"elements\":\"3\"},{\"type\":13,\"symbol\":\"·\"},{\"type\":15,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"x\"}]},{\"type\":10,\"elements\":[]},{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"2\"}]}]}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, root2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"root(x, 3);");
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s == "pow(x,0.333)") << s;
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":17,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"3\"}]},{\"type\":10,\"elements\":[]},{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"x\"}]}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, tojson_sin)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x);");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"sin\"},{\"type\":19,\"symbol\":\"(\"},{\"type\":8,\"elements\":\"x\"},{\"type\":20,\"symbol\":\")\"}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, tojson_sincos)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x)+cos(x);");
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"cos\"},{\"type\":19,\"symbol\":\"(\"},{\"type\":8,\"elements\":\"x\"},{\"type\":20,\"symbol\":\")\"},{\"type\":11,\"symbol\":\"+\"},{\"type\":8,\"elements\":\"sin\"},{\"type\":19,\"symbol\":\"(\"},{\"type\":8,\"elements\":\"x\"},{\"type\":20,\"symbol\":\")\"}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, complex_mul_symbol1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2*i;");
    ASSERT_TRUE(res.ToStdString(10) == "2.*i") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"json({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"2"},{"type":13,"symbol":"·"},{"type":8,"elements":"i"}]}]})json" ) << json;
}

TEST_F(CalcTestSymbolicReal, division_nested1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(x+1)/(x-1);");
    ASSERT_TRUE(res.ToStdString(10) == "(1.+x)/(-1.+x)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"1"},{"type":11,"symbol":"+"},{"type":8,"elements":"x"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":8,"elements":"1"},{"type":11,"symbol":"+"},{"type":8,"elements":"x"}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, derivative_power5)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,5),x);");
    ASSERT_TRUE(res.ToStdString(10) == "5.*pow(x,4.)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"5"},{"type":13,"symbol":"·"},{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"x"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"4"}]}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, derivative_cos)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(cos(x),x);");
    ASSERT_TRUE(res.ToStdString(10) == "-sin(x)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":12,"symbol":"-"},{"type":7,"elements":[{"type":8,"elements":"sin"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, derivative_nonsymbol)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x, x+1);"), yutovo_calculator::ParserException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x, 5);"), yutovo_calculator::ParserException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x, sin(x));"), yutovo_calculator::ParserException);
}

TEST_F(CalcTestSymbolicReal, derivative_at_point1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,2), [x=3]);");
    ASSERT_TRUE(res.ToString(10) == U"6.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, derivative_at_point2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(sin(x), [x=0]);");
    ASSERT_TRUE(res.ToString(10) == U"1.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, derivative_at_point3)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=pow(x,2);");
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 2}, U"derivative(f(x), [x=3]);");
    ASSERT_TRUE(res.ToString(10) == U"6.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, derivative_at_point4)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,2), [y=3]);");
    ASSERT_TRUE(res.ToString(10) == U"0.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, derivative_at_point5)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(pow(x,2), [x=3])+1;");
    ASSERT_TRUE(res.ToString(10) == U"7.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, derivative_at_point_error1)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x, [x+1=1]);"), yutovo_calculator::ParserException);
}

TEST_F(CalcTestSymbolicReal, derivative_at_point_error2)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x, [5=1]);"), yutovo_calculator::ParserException);
}

TEST_F(CalcTestSymbolicReal, derivative_at_point6)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x,y)=x*y;");
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x,y), [x=3]);");
    ASSERT_TRUE(res.ToString(10) == U"y") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, derivative_at_point7)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x,y)=x*y;");
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x,y), [y=3]);");
    ASSERT_TRUE(res.ToString(10) == U"x") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, derivative_at_point8)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x,y)=pow(x,y);");
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x,y), [y=3]);");
    ASSERT_TRUE(res.ToString(10) == U"log(x)*pow(x,3.)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, derivative_at_point9)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(1/x, [x=0]);");
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"∞"}]}]})r")
        << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, derivative_at_point_multi1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x*y, [x=2, y=3]);");
    ASSERT_TRUE(res.ToString(10) == U"3.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, derivative_at_point_multi2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x*y, [y=3, x=2]);");
    ASSERT_TRUE(res.ToString(10) == U"2.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, derivative_at_point_multi3)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x,y)=pow(x,2)*y;");
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x,y), [x=2, y=3]);");
    ASSERT_TRUE(res.ToString(10) == U"12.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, derivative_at_point_multi4)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x,y)=pow(x,2)*y+sin(y);");
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x,y), [x=1, y=3]);");
    ASSERT_TRUE(res.ToString(10) == U"6.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, derivative_mixed_second_order)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"func(x,y)=pow(x,2)*pow(y,2);");
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(derivative(func(x,y),y),x);");
    ASSERT_TRUE(res.ToStdString(10) == "4.*x*y") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, frac_power_mult)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(x,(1/2))*pow(x,(1/3));");
    ASSERT_TRUE(res.ToStdString(10) == "pow(x,0.833)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"x"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"0.833"}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, log_ratio)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"log(2,4);");
    ASSERT_TRUE(res.ToStdString(10) == "2.") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"4"},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"2"},{"type":20,"symbol":")"}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, sqrt_half)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sqrt(1/2);");
    ASSERT_TRUE(res.ToStdString(10) == "0.707") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"0.5"},{"type":13,"symbol":"·"},{"type":16,"elements":[{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"2"}]}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, limit_cot0)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(cot(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, limit_sec_pi2)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(sec(x),x,(pi)/(2));");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, limit_csc0)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(csc(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, limit_sin_x)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(sin(x)/x,x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"nan\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, limit_exp_m1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs((exp(x)-1)/x,x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, limit_ln_x)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(ln(x)/x,x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) == "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"∞\"}]}]}") << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, limit_exp_1x)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(exp(1/x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, limit_sin_1x)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(sin(1/x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, limit_cos_1x)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(cos(1/x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, limit_ln_1x)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(ln(1/x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, limit_sqrt_noo)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sqrt(-∞);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, scientific1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.23e-5;");
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

TEST_F(CalcTestSymbolicReal, hyperbolic_sinh)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sinh(0);");
    ASSERT_TRUE(res.ToStdString(10) == "0.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, hyperbolic_cosh)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cosh(0);");
    ASSERT_TRUE(res.ToStdString(10) == "1.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, hyperbolic_tanh)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"tanh(0);");
    ASSERT_TRUE(res.ToStdString(10) == "0.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, hyperbolic_coth0)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(coth(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, hyperbolic_sech)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sech(0);");
    ASSERT_TRUE(res.ToStdString(10) == "1.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, hyperbolic_csch0)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(csch(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, hyperbolic_asinh)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arsinh(0);");
    ASSERT_TRUE(res.ToStdString(10) == "0.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, hyperbolic_acosh)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcosh(1);");
    ASSERT_TRUE(res.ToStdString(10) == "0.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, hyperbolic_atanh)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"artanh(0);");
    ASSERT_TRUE(res.ToStdString(10) == "0.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, hyperbolic_acoth1)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(arcoth(x),x,1);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, hyperbolic_asech)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arsech(1);");
    ASSERT_TRUE(res.ToStdString(10) == "0.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, hyperbolic_acsch0)
{
    auto res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(arcsch(x),x,0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, hyperbolic_symbolic)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sinh(x);");
    ASSERT_TRUE(res.ToStdString(10) == "sinh(x)") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcosh(x);");
    ASSERT_TRUE(res.ToStdString(10) == "acosh(x)") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, hyperbolic_alias)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cosech(0);");
    ASSERT_TRUE(res.ToStdString(10) == "∞") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcosech(1);");
    ASSERT_TRUE(res.ToStdString(10) == "0.881") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, hyperbolic_inverse_synonyms)
{
    Symbolic<Real> a = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"asinh(0);");
    Symbolic<Real> b = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arsinh(0);");
    Symbolic<Real> c = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcsinh(0);");
    ASSERT_TRUE(a.ToStdString(10) == "0.") << a.ToStdString(10);
    ASSERT_TRUE(b.ToStdString(10) == "0.") << b.ToStdString(10);
    ASSERT_TRUE(c.ToStdString(10) == "0.") << c.ToStdString(10);

    a = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"acosh(1);");
    b = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcosh(1);");
    c = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arccosh(1);");
    ASSERT_TRUE(a.ToStdString(10) == "0.") << a.ToStdString(10);
    ASSERT_TRUE(b.ToStdString(10) == "0.") << b.ToStdString(10);
    ASSERT_TRUE(c.ToStdString(10) == "0.") << c.ToStdString(10);

    a = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"atanh(0);");
    b = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"artanh(0);");
    c = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arctanh(0);");
    ASSERT_TRUE(a.ToStdString(10) == "0.") << a.ToStdString(10);
    ASSERT_TRUE(b.ToStdString(10) == "0.") << b.ToStdString(10);
    ASSERT_TRUE(c.ToStdString(10) == "0.") << c.ToStdString(10);

    a = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(acoth(x),x,1);");
    b = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(arcoth(x),x,1);");
    c = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(arccoth(x),x,1);");
    ASSERT_TRUE(a.ToStdString(10) == "∞") << a.ToStdString(10);
    ASSERT_TRUE(b.ToStdString(10) == "∞") << b.ToStdString(10);
    ASSERT_TRUE(c.ToStdString(10) == "∞") << c.ToStdString(10);

    a = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"asech(1);");
    b = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arsech(1);");
    c = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcsech(1);");
    ASSERT_TRUE(a.ToStdString(10) == "0.") << a.ToStdString(10);
    ASSERT_TRUE(b.ToStdString(10) == "0.") << b.ToStdString(10);
    ASSERT_TRUE(c.ToStdString(10) == "0.") << c.ToStdString(10);

    a = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(acsch(x),x,0);");
    b = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(arcsch(x),x,0);");
    c = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(arccsch(x),x,0);");
    Symbolic<Real> d = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(arcosech(x),x,0);");
    Symbolic<Real> e = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"subs(arccosech(x),x,0);");
    ASSERT_TRUE(a.ToStdString(10) == "∞") << a.ToStdString(10);
    ASSERT_TRUE(b.ToStdString(10) == "∞") << b.ToStdString(10);
    ASSERT_TRUE(c.ToStdString(10) == "∞") << c.ToStdString(10);
    ASSERT_TRUE(d.ToStdString(10) == "∞") << d.ToStdString(10);
    ASSERT_TRUE(e.ToStdString(10) == "∞") << e.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, hyperbolic_acosh_nan)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcosh(nan);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, hyperbolic_asech_nan)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arsech(2);");
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s == "nan" || s == "-nan.") << s;
}

TEST_F(CalcTestSymbolicReal, hyperbolic_artanh_nan)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"artanh(2);");
    std::string s = res.ToStdString(10);
    ASSERT_TRUE(s == "nan" || s == "-nan.") << s;
}

TEST_F(CalcTestSymbolicReal, hyperbolic_arcoth_nan)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"arcoth(nan);");
    ASSERT_TRUE(res.ToStdString(10) == "nan") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, log_x_base)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"log(x,2);");
    ASSERT_TRUE(res.ToStdString(10) == "log2./log(x)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"2"},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, cot_tojson)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cot(x);");
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"cot"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, sec_tojson)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sec(x);");
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"sec"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, csc_tojson)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"csc(x);");
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"csc"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, sinh_tojson)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sinh(x);");
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"sinh"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, cosh_tojson)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cosh(x);");
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"cosh"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, sin_expression_brackets)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x+1);");
    ASSERT_TRUE(res.ToStdString(10) == "sin(1.+x)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"sin"},{"type":19,"symbol":"("},{"type":8,"elements":"1"},{"type":11,"symbol":"+"},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, nested_functions_brackets)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cot(sec(x));");
    ASSERT_TRUE(res.ToStdString(10) == "cot(sec(x))") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"cot"},{"type":19,"symbol":"("},{"type":8,"elements":"sec"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, ln_russian)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ln(x);");
    ASSERT_TRUE(res.ToStdString(10, Language::Russian) == "ln(x)") << res.ToStdString(10, Language::Russian);
    ASSERT_TRUE(res.ToJson(10, Language::Russian) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"ln"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10, Language::Russian);
}

TEST_F(CalcTestSymbolicReal, ln_in_expression_russian)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ln(x)+1;");
    ASSERT_TRUE(res.ToStdString(10, Language::Russian) == "1.+ln(x)") << res.ToStdString(10, Language::Russian);
    ASSERT_TRUE(res.ToJson(10, Language::Russian) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"1"},{"type":11,"symbol":"+"},{"type":8,"elements":"ln"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10, Language::Russian);
}

TEST_F(CalcTestSymbolicReal, ln_english)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ln(x);");
    ASSERT_TRUE(res.ToStdString(10) == "log(x)") << res.ToStdString(10);
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, log_change_of_base_russian)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"log(2,4);");
    ASSERT_TRUE(res.ToStdString(10, Language::Russian) == "2.") << res.ToStdString(10, Language::Russian);
    ASSERT_TRUE(res.ToJson(10, Language::Russian) ==
        R"r({"type":45,"elements":[{"type":14,"elements":[{"type":7,"elements":[{"type":8,"elements":"ln"},{"type":19,"symbol":"("},{"type":8,"elements":"4"},{"type":20,"symbol":")"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"ln"},{"type":19,"symbol":"("},{"type":8,"elements":"2"},{"type":20,"symbol":")"}]}]}]})r" ) << res.ToJson(10, Language::Russian);
}

TEST_F(CalcTestSymbolicReal, sin2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(0);");
    ASSERT_TRUE(res.ToStdString(10) == "0.") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"0\"}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, sin3)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"sin(x);");
    ASSERT_TRUE(res.ToStdString(10) == "sin(x)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"sin"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, cos2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cos(0);");
    ASSERT_TRUE(res.ToStdString(10) == "1.") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"1\"}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, cos3)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"cos(x);");
    ASSERT_TRUE(res.ToStdString(10) == "cos(x)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"cos"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, exp1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"exp(0);");
    ASSERT_TRUE(res.ToStdString(10) == "1.") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"1\"}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, exp2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"exp(x);");
    ASSERT_TRUE(res.ToStdString(10) == "exp(x)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"r({"type":45,"elements":[{"type":15,"elements":[{"type":7,"elements":[{"type":8,"elements":"e"}]},{"type":10,"elements":[]},{"type":7,"elements":[{"type":8,"elements":"x"}]}]}]})r"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, ln2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ln(1);");
    ASSERT_TRUE(res.ToStdString(10) == "0.") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"0\"}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, ln3)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"ln(x);");
    ASSERT_TRUE(res.ToStdString(10) == "log(x)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"log"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, fact1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"5!;");
    ASSERT_TRUE(res.ToStdString(10) == "120.") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"120\"}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, fact2)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"x!;");
    ASSERT_TRUE(res.ToStdString(10) == "gamma(1.+x)") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"gamma"},{"type":19,"symbol":"("},{"type":8,"elements":"1"},{"type":11,"symbol":"+"},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, fact3)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0!;");
    ASSERT_TRUE(res.ToStdString(10) == "1.") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1!;");
    ASSERT_TRUE(res.ToStdString(10) == "1.") << res.ToStdString(10);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"10!;");
    ASSERT_TRUE(res.ToStdString(10) == "3628800") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, fact4)
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

TEST_F(CalcTestSymbolicReal, gamma1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"gamma(5);");
    ASSERT_TRUE(res.ToStdString(10) == "24.") << res.ToStdString(10);
    std::string json = res.ToJson(10);
    ASSERT_TRUE(json ==
        "{\"type\":45,\"elements\":[{\"type\":7,\"elements\":[{\"type\":8,\"elements\":\"24\"}]}]}"
        ) << json;
}

TEST_F(CalcTestSymbolicReal, gamma_tojson)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"gamma(x);");
    ASSERT_TRUE(res.ToJson(10) ==
        R"r({"type":45,"elements":[{"type":7,"elements":[{"type":8,"elements":"gamma"},{"type":19,"symbol":"("},{"type":8,"elements":"x"},{"type":20,"symbol":")"}]}]})r" ) << res.ToJson(10);
}

TEST_F(CalcTestSymbolicReal, min_max1)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"min(x,y);");
    ASSERT_TRUE(res.ToStdString(10) == "min(x,y)") << res.ToStdString(10);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"max(x,y);");
    ASSERT_TRUE(res.ToStdString(10) == "max(x,y)") << res.ToStdString(10);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"min(2,3);");
    ASSERT_TRUE(res.ToStdString(10) == "2.") << res.ToStdString(10);

    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"max(2,3);");
    ASSERT_TRUE(res.ToStdString(10) == "3.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, min_max2)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a=5;");
    parser.Parse(LogicalId{0, 0, 2}, U"b=3;");

    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 3}, U"min(a,b);");
    ASSERT_TRUE(res.ToStdString(10) == "3.") << res.ToStdString(10);

    res = parser.Parse(LogicalId{0, 0, 4}, U"max(a,b);");
    ASSERT_TRUE(res.ToStdString(10) == "5.") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, threads_variables)
{
    constexpr int threads_count = 50;
    constexpr int iterations = 20;

    auto format_number =
        [](double v)
        {
            std::ostringstream s;
            s << std::setprecision(10) << std::fixed << v;
            std::string str = s.str();
            if (str.find('.') != std::string::npos)
            {
                str.erase(str.find_last_not_of('0') + 1);
                if (str.back() == '.')
                    str.pop_back();
            }
            return str;
        };

    std::vector<std::string> expected1(threads_count);
    std::vector<std::string> expected2(threads_count);
    for (int i = 0; i < threads_count; ++i)
    {
        double a = 2.3 + i;
        double b = 3.4 + i;
        double c = 6.7 + i;
        double d = 0.5 + i;
        double e = 1.5 + i;
        double f = 2.5 + i;
        expected1[i] = format_number(a + b * c + d + e * f) + "+sin(pow(x,2.))";

        double g = 0.25 + i;
        double h = 1.25 + i;
        double j = 2.25 + i;
        expected2[i] = format_number(g + h * j) + "+cos(pow(y,2.))";
    }

    std::vector<std::vector<std::string>> actual1(iterations, std::vector<std::string>(threads_count));
    std::vector<std::vector<std::string>> actual2(iterations, std::vector<std::string>(threads_count));
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
                Parser<Symbolic<Real>> p(3, Language::English);
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

                    p.Parse(LogicalId{0, 0, 1}, U"a=" + ToUtfString(std::to_string(2.3 + i)) + U";", 3);
                    p.Parse(LogicalId{0, 0, 2}, U"b=" + ToUtfString(std::to_string(3.4 + i)) + U";", 3);
                    p.Parse(LogicalId{0, 0, 3}, U"c=" + ToUtfString(std::to_string(6.7 + i)) + U";", 3);
                    p.Parse(LogicalId{0, 0, 4}, U"d=" + ToUtfString(std::to_string(0.5 + i)) + U";", 3);
                    p.Parse(LogicalId{0, 0, 5}, U"e=" + ToUtfString(std::to_string(1.5 + i)) + U";", 3);
                    p.Parse(LogicalId{0, 0, 6}, U"f=" + ToUtfString(std::to_string(2.5 + i)) + U";", 3);
                    Symbolic<Real> res1 = p.Parse(LogicalId{0, 0, 7}, U"a+b*c+d+e*f+sin(pow(x,2));", 10);
                    actual1[iter][i] = res1.ToStdString(10);

                    p.Parse(LogicalId{0, 0, 8}, U"g=" + ToUtfString(std::to_string(0.25 + i)) + U";", 3);
                    p.Parse(LogicalId{0, 0, 9}, U"h=" + ToUtfString(std::to_string(1.25 + i)) + U";", 3);
                    p.Parse(LogicalId{0, 0, 10}, U"j=" + ToUtfString(std::to_string(2.25 + i)) + U";", 3);
                    Symbolic<Real> res2 = p.Parse(LogicalId{0, 0, 11}, U"g+h*j+cos(pow(y,2));", 10);
                    actual2[iter][i] = res2.ToStdString(10);

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
        }
        actual1[iter].clear();
        actual2[iter].clear();
    }

    {
        std::lock_guard<std::mutex> lock(m);
        current_iter = iterations;
    }
    cv_start.notify_all();

    for (auto& t : threads)
        t.join();
}

TEST_F(CalcTestSymbolicReal, definite_integral_user_function)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"iff(t)=5*pow(e,-2t);");
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"definite_integral(0,1,iff(t),t);", 10);
    ASSERT_TRUE(res.ToStdString(10) == "2.1616617919") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, definite_integral_constant_with_e)
{
    Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,1,5*pow(e,-2t),t);", 10);
    ASSERT_TRUE(res.ToStdString(10) == "2.1616617919") << res.ToStdString(10);
}

TEST_F(CalcTestSymbolicReal, all_symbolic_functions)
{
    struct Expect
    {
        const char* name;
        const char* alias;
        const char* numeric_expr;
        const char* numeric_result;
        const char* sym_result;
        const char* derivative_result;
    };
    const Expect funcs[] = 
        {
            //direct numeric, alias, symbolic form, derivative
            {"sin", nullptr, "sin(0)", "0.", "sin(x)", "cos(x)"},
            {"cos", nullptr, "cos(0)", "1.", "cos(x)", "-sin(x)"},
            {"tg", "tan", "tg(0)", "0.", "tan(x)", "1.+pow(tan(x),2.)"},
            {"cot", "ctg", "subs(cot(x),x,0)", "∞", "cot(x)", "diff(cot(x),x)"},
            {"sec", nullptr, "subs(sec(x),x,0)", "1.", "sec(x)", "diff(sec(x),x)"},
            {"csc", "cosec", "subs(csc(x),x,1)", "1.1883951058", "csc(x)", "diff(csc(x),x)"},
            {"arcsin", nullptr, "arcsin(0)", "0.", "asin(x)", "pow(sqrt(1.-pow(x,2.)),-1.)"},
            {"arccos", nullptr, "arccos(1)", "0.", "acos(x)", "-(pow(sqrt(1.-pow(x,2.)),-1.))"},
            {"arctg", "arctan", "arctg(0)", "0.", "atan(x)", "pow(1.+pow(x,2.),-1.)"},
            {"arcctg", "arccot", "subs(arcctg(x),x,1)", "0.7853981634", "atan(pow(x,-1.))", "-(pow(pow(x,2.),-1.))/(pow(pow(x,-1.),2.)+1.)"},
            {"arcsec", nullptr, "arcsec(1)", "0.", "acos(pow(x,-1.))", "(pow(pow(x,2.),-1.)/sqrt(-1.+pow(x,2.))/abs(x))*pow(x,2.)"},
            {"arccsc", "arccosec", "arccsc(1)", "1.5707963268", "asin(pow(x,-1.))", "(-(pow(pow(x,2.),-1.))/sqrt(-1.+pow(x,2.))/abs(x))*pow(x,2.)"},
            {"sinh", "sh", "sinh(0)", "0.", "sinh(x)", "cosh(x)"},
            {"cosh", "ch", "cosh(0)", "1.", "cosh(x)", "sinh(x)"},
            {"tanh", "th", "tanh(0)", "0.", "tanh(x)", "1.-pow(tanh(x),2.)"},
            {"coth", "cth", "subs(coth(x),x,1)", "1.3130352855", "coth(x)", "diff(coth(x),x)"},
            {"sech", "sch", "sech(0)", "1.", "sech(x)", "diff(sech(x),x)"},
            {"csch", "cosech", "subs(csch(x),x,1)", "0.8509181282", "csch(x)", "diff(csch(x),x)"},
            {"asinh", "arsinh", "asinh(0)", "0.", "asinh(x)", "diff(asinh(x),x)"},
            {"acosh", "arcosh", "acosh(1)", "0.", "acosh(x)", "diff(acosh(x),x)"},
            {"atanh", "artanh", "atanh(0)", "0.", "atanh(x)", "diff(atanh(x),x)"},
            {"acoth", "arcoth", "subs(acoth(x),x,2)", "0.5493061443", "acoth(x)", "diff(acoth(x),x)"},
            {"asech", "arsech", "asech(1)", "0.", "asech(x)", "diff(asech(x),x)"},
            {"acsch", "arcsch", "acsch(1)", "0.881373587", "acsch(x)", "diff(acsch(x),x)"},
        };

    for (const auto& f : funcs)
    {
        SCOPED_TRACE(f.name);
        std::u32string name = std::u32string(f.name, f.name + strlen(f.name));

        //numeric variant
        {
            Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1},
                std::u32string(f.numeric_expr, f.numeric_expr + strlen(f.numeric_expr)) + U";", 10);
            ASSERT_TRUE(res.ToStdString(10) == f.numeric_result) << res.ToStdString(10);
        }

        //symbolic variant
        {
            Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, name + U"(x);", 10);
            ASSERT_TRUE(res.ToStdString(10) == f.sym_result) << res.ToStdString(10);
        }

        //derivative variant
        {
            Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(" + name + U"(x),x);", 10);
            ASSERT_TRUE(res.ToStdString(10) == f.derivative_result) << res.ToStdString(10);
        }

        //alias variant
        if (f.alias)
        {
            std::u32string alias = std::u32string(f.alias, f.alias + strlen(f.alias));
            Symbolic<Real> res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, alias + U"(x);", 10);
            ASSERT_TRUE(res.ToStdString(10) == f.sym_result) << res.ToStdString(10);
        }
    }
}

}
