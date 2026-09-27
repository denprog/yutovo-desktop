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

TEST_F(CalcTestComplex, numbers1)
{
    std::string res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1;").ToStdString(3, 3);
    ASSERT_TRUE(res == "1.E+0") << res;
    res = parser.Parse(LogicalId{0, 0, 1}, U"0.2;").ToStdString(3, 3);
    ASSERT_TRUE(res == "0.2E+0") << res;
    res = parser.Parse(LogicalId{0, 0, 1}, U"0.2E+0;").ToStdString(3, 3);
    ASSERT_TRUE(res == "0.2E+0") << res;
}

TEST_F(CalcTestComplex, numbers_invalid_dots)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"."), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"..2"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1..2"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.2.3"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2.3.4"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U".1.2"), yutovo_calculator::SyntaxException);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.2."), yutovo_calculator::SyntaxException);
}

TEST_F(CalcTestComplex, numbers2)
{
    std::string res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1i;").ToStdString(3, 3);
    ASSERT_TRUE(res == "i*1.E+0") << res;
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2+1i;").ToStdString(3, 3);
    ASSERT_TRUE(res == "2.E+0+i*1.E+0") << res;
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"5i;").ToStdString(3, 3);
    ASSERT_TRUE(res == "i*5.E+0") << res;
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"5.678i;").ToStdString(3, 3);
    ASSERT_TRUE(res == "i*5.678E+0") << res;
}

TEST_F(CalcTestComplex, numbers3)
{
    std::string res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(1+2)*i;").ToStdString(3, 3);
    ASSERT_TRUE(res == "i*3.E+0") << res;
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"i*(1+2);").ToStdString(3, 3);
    ASSERT_TRUE(res == "i*3.E+0") << res;
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(1+2)*i;").ToStdString(3, 3);
    ASSERT_TRUE(res == "i*3.E+0") << res;
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2/i;").ToStdString(3, 3);
    ASSERT_TRUE(res == "i*-2.E+0") << res;
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2(i);").ToStdString(3, 3);
    ASSERT_TRUE(res == "i*2.E+0") << res;
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2(2+3i);").ToStdString(3, 3);
    ASSERT_TRUE(res == "4.E+0+i*6.E+0") << res;
}

TEST_F(CalcTestComplex, numbers4)
{
    parser.SetLocale(Language::Russian);
    std::string res;
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(1+2)*i;").ToStdString(3, 3), yutovo_calculator::SyntaxException);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(1+2)*j;").ToStdString(3, 3);
    ASSERT_TRUE(res == "i*3.E+0") << res;
}

TEST_F(CalcTestComplex, scientific1)
{
    std::string res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.23e-5;").ToStdString(3, 3);
    ASSERT_TRUE(res == "1.23E-5") << res;
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-1.23e-5;").ToStdString(3, 3);
    ASSERT_TRUE(res == "-1.23E-5") << res;
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"-1.23e+5;").ToStdString(3, 3);
    ASSERT_TRUE(res == "-1.23E+5") << res;
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1.23e+5;").ToStdString(3, 3);
    ASSERT_TRUE(res == "1.23E+5") << res;
}

TEST_F(CalcTestComplex, trigonometric1)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"sin(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "i*1.175E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"sin(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.841E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"sin(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "9.154E+0+i*-4.169E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric2)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"cos(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "1.543E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"cos(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.54E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"cos(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "-4.19E+0+i*-9.109E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric3)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"tg(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "i*0.762E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"tg(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "1.557E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"tg(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "-0.00376E+0+i*1.003E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric4)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"ctg(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "i*-1.313E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"ctg(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.642E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"ctg(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "-0.00374E+0+i*-0.997E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric5)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"sec(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.648E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"sec(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "1.851E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"sec(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "-0.0417E+0+i*0.0906E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric6)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"cosec(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "i*-0.851E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"cosec(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "1.188E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"cosec(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.0905E+0+i*0.0412E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric7)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"sh(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "i*0.841E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"sh(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "1.175E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"sh(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "-3.591E+0+i*0.531E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric8)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"ch(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.54E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"ch(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "1.543E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"ch(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "-3.725E+0+i*0.512E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric9)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"th(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "i*1.557E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"th(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.762E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"th(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.965E+0+i*-0.00988E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric10)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"cth(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "i*-0.642E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"cth(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "1.313E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"cth(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "1.036E+0+i*0.0106E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric11)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"sch(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "1.851E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"sch(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.648E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"sch(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "-0.264E+0+i*-0.0362E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric12)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"csch(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "i*-1.188E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"csch(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.851E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"csch(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "-0.273E+0+i*-0.0403E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric13)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arcsin(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Radian && res.ToStdString(3, 3) == "i*0.881E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arcsin(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Radian && res.ToStdString(3, 3) == "1.571E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arcsin(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Radian && res.ToStdString(3, 3) == "0.571E+0+i*1.983E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric14)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arccos(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Radian && res.ToStdString(3, 3) == "1.571E+0+i*-0.881E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arccos(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Radian);
    ASSERT_TRUE(abs(res.GetRe()) <= Real(3, 1e-15f)) << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arccos(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Radian && res.ToStdString(3, 3) == "1.E+0+i*-1.983E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric15)
{
    Complex res;
    EXPECT_THROW(res = parser.Parse(LogicalId{0, 0, 1}, U"arctg(i);"), yutovo_calculator::MathException);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arctg(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Radian && res.ToStdString(3, 3) == "0.785E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arctg(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Radian && res.ToStdString(3, 3) == "1.41E+0+i*0.229E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric16)
{
    Complex res;
    EXPECT_THROW(res = parser.Parse(LogicalId{0, 0, 1}, U"arcctg(i);"), yutovo_calculator::MathException);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arcctg(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Radian && res.ToStdString(3, 3) == "0.785E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arcctg(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Radian && res.ToStdString(3, 3) == "0.161E+0+i*-0.229E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric17)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arcsec(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Radian && res.ToStdString(3, 3) == "1.571E+0+i*0.881E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arcsec(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Radian);
    ASSERT_TRUE(abs(res.GetRe()) <= Real(3, 1e-15f)) << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arcsec(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Radian && res.ToStdString(3, 3) == "1.42E+0+i*0.231E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric18)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arccosec(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Radian && res.ToStdString(3, 3) == "i*-0.881E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arccosec(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Radian && res.ToStdString(3, 3) == "1.571E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arccosec(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::Radian && res.ToStdString(3, 3) == "0.15E+0+i*-0.231E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric19)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arsh(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "i*1.571E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arsh(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.881E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arsh(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "1.969E+0+i*0.965E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric20)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arch(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.881E+0+i*1.571E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arch(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arch(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "1.983E+0+i*1.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric21)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arth(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "i*0.785E+0") << res.ToStdString(3, 3);

    EXPECT_THROW(res = parser.Parse(LogicalId{0, 0, 1}, U"arth(1);"), yutovo_calculator::MathException);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arth(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.147E+0+i*1.339E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric22)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arcth(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "i*-0.785E+0") << res.ToStdString(3, 3);

    EXPECT_THROW(res = parser.Parse(LogicalId{0, 0, 1}, U"arcth(1);"), yutovo_calculator::MathException);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arcth(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.147E+0+i*-0.232E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arcth(0.5);", AngleMeasure::Radian, AngleMeasure::Radian);
    ASSERT_TRUE(res.ToStdString(3, 3) == "0.549E+0+i*1.571E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric23)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arsch(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.881E+0+i*-1.571E+0") << res.ToStdString(3, 3) << (int)res.GetAngleMeasure();

    res = parser.Parse(LogicalId{0, 0, 1}, U"arsch(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arsch(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.231E+0+i*-1.42E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, trigonometric24)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arcsch(i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "i*-1.571E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arcsch(1);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.881E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"arcsch(2+3i);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None && res.ToStdString(3, 3) == "0.157E+0+i*-0.23E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, functions1)
{
    std::string res = parser.Parse(LogicalId{0, 0, 1}, U"ln(i);").ToStdString(3, 3);
    ASSERT_TRUE(res == "i*1.571E+0") << res;
    res = parser.Parse(LogicalId{0, 0, 1}, U"ln(5+i);").ToStdString(3, 3);
    ASSERT_TRUE(res == "1.629E+0+i*0.197E+0") << res;
}

TEST_F(CalcTestComplex, functions2)
{
    std::string res = parser.Parse(LogicalId{0, 0, 1}, U"pow(i,2);").ToStdString(3, 3);
    ASSERT_TRUE(res == "-1.E+0") << res;
    res = parser.Parse(LogicalId{0, 0, 1}, U"pow(2i,3);").ToStdString(3, 3);
    ASSERT_TRUE(res == "i*8.E+0") << res;
}

TEST_F(CalcTestComplex, functions3)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"2exp(1);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "5.437E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, functions4)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"a=5;");
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"pow(2,3)a;", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "40.E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, functions5)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"re(2);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "2.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"re(4+5*i);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "4.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"re(5*i);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "0.E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, functions6)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"im(2);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "0.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"im(4+5*i);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "5.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"im(5*i);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "5.E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, functions7)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"mod(2);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "2.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"mod(-6+3*i);", 3);
    auto r1 = parser.Parse(LogicalId{0, 0, 1}, U"sqrt(45);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == r1.ToStdString(3, 3)) << r.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, functions8)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"arg(5+4i);", 3);
    auto r1 = parser.Parse(LogicalId{0, 0, 1}, U"arctg(4/5);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == r1.ToStdString(3, 3)) << r.ToStdString(3, 3) << " " << r1.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, functions9)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"arcsin(10);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "1.571E+0+i*-2.993E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, functions10)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"arg(10);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "0.E+0") << r.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, functions11)
{
    auto r = parser.Parse(LogicalId{0, 0, 1}, U"abs(2);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == "2.E+0") << r.ToStdString(3, 3);
    r = parser.Parse(LogicalId{0, 0, 1}, U"abs(-6+3*i);", 3);
    auto r1 = parser.Parse(LogicalId{0, 0, 1}, U"sqrt(45);", 3);
    ASSERT_TRUE(r.ToStdString(3, 3) == r1.ToStdString(3, 3)) << r.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, functions12)
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

TEST_F(CalcTestComplex, functions13)
{
    Complex res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"pow(0, 2);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "0.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, functions14)
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

TEST_F(CalcTestComplex, sqrt1)
{
    Dependencies dependencies;
    std::vector<Complex> results;
    parser.Parse(LogicalId{0, 0, 1}, U"sqrt(i);", &dependencies, AngleMeasure::Radian, AngleMeasure::Radian, 3, 10, results);
    std::string res = ToString(results);
    ASSERT_TRUE(res == "0.707E+0+i*0.707E+0,-0.707E+0+i*-0.707E+0") << res;

    results.clear();
    parser.Parse(LogicalId{0, 0, 1}, U"sqrt(3);", &dependencies, AngleMeasure::Radian, AngleMeasure::Radian, 3, 10, results);
    res = ToString(results);
    ASSERT_TRUE(res == "1.732E+0,-1.732E+0") << res;

    results.clear();
    parser.Parse(LogicalId{0, 0, 1}, U"sqrt(2i+3);", &dependencies, AngleMeasure::Radian, AngleMeasure::Radian, 3, 10, results);
    res = ToString(results);
    ASSERT_TRUE(res == "1.817E+0+i*0.55E+0,-1.817E+0+i*-0.55E+0") << res;
}

TEST_F(CalcTestComplex, sqrt2)
{
    Dependencies dependencies;
    std::vector<Complex> results;
    parser.Parse(LogicalId{0, 0, 1}, U"sqrt(sqrt(1+i));", &dependencies, AngleMeasure::Radian, AngleMeasure::Radian, 3, 10, results);
    std::string res = ToString(results);
    ASSERT_TRUE(res == "1.07E+0+i*0.213E+0,0.213E+0+i*-1.07E+0,-1.07E+0+i*-0.213E+0,-0.213E+0+i*1.07E+0") << res;
}

TEST_F(CalcTestComplex, root1)
{
    Dependencies dependencies;
    std::vector<Complex> results;
    parser.Parse(LogicalId{0, 0, 1}, U"root(1+i,3);", &dependencies, AngleMeasure::Radian, AngleMeasure::Radian, 3, 10, results);
    std::string res = ToString(results);
    ASSERT_TRUE(res == "1.084E+0+i*0.291E+0,-0.794E+0+i*0.794E+0,-0.291E+0+i*-1.084E+0") << res;
}

TEST_F(CalcTestComplex, exp1)
{
    Dependencies dependencies;
    std::vector<Complex> results;
    parser.Parse(LogicalId{0, 0, 1}, U"exp(i);", &dependencies, AngleMeasure::Radian, AngleMeasure::Radian, 3, 10, results);
    std::string res = ToString(results);
    ASSERT_TRUE(res == "0.54E+0+i*0.841E+0") << res;
}

TEST_F(CalcTestComplex, variables1)
{
    parser.Parse(LogicalId{0, 0, 1}, U"v=5;");
    std::string s = parser.Parse(LogicalId{0, 0, 2}, U"v;").ToStdString(3, 3);
    ASSERT_TRUE(s == "5.E+0") << s;
}

TEST_F(CalcTestComplex, variables2)
{
    parser.Parse(LogicalId{0, 0, 1}, U"v=5;");
    std::string s = parser.Parse(LogicalId{0, 0, 2}, U"(1/2)v;").ToStdString(3, 3);
    ASSERT_TRUE(s == "2.5E+0") << s;
}

TEST_F(CalcTestComplex, variables3)
{
    parser.Parse(LogicalId{0, 0, 1}, U"y=pow(e,2);");
    std::string s = parser.Parse(LogicalId{0, 0, 2}, U"y;").ToStdString(3, 3);
    ASSERT_TRUE(s == "7.389E+0") << s;
}

TEST_F(CalcTestComplex, variables4)
{
    parser.Parse(LogicalId{0, 0, 2}, U"a=77;");
    parser.Parse(LogicalId{0, 0, 1}, U"a=5;");
    std::string s = parser.Parse(LogicalId{0, 0, 3}, U"a;").ToStdString(3, 3);
    ASSERT_TRUE(s == "77.E+0") << s;
}

TEST_F(CalcTestComplex, variables5)
{
    parser.Parse(LogicalId{0, 0, 1}, U"d=4;");
    std::string s = parser.Parse(LogicalId{0, 0, 2}, U"d+5;").ToStdString(3, 3);
    ASSERT_TRUE(s == "9.E+0") << s;
    parser.Parse(LogicalId{0, 0, 3}, U"d=45+d;");
    s = parser.Parse(LogicalId{0, 0, 4}, U"d+5;").ToStdString(3, 3);
    ASSERT_TRUE(s == "54.E+0") << s;
}

TEST_F(CalcTestComplex, variables6)
{
    parser.Parse(LogicalId{0, 0, 1}, U"a{1}=5;");
    std::string s = parser.Parse(LogicalId{0, 0, 2}, U"a{1};").ToStdString(3, 3);
    ASSERT_TRUE(s == "5.E+0") << s;
    parser.Parse(LogicalId{0, 0, 3}, U"a=7;");
    s = parser.Parse(LogicalId{0, 0, 4}, U"a{1}+a;").ToStdString(3, 3);
    ASSERT_TRUE(s == "12.E+0") << s;
}

TEST_F(CalcTestComplex, variables7)
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
    Complex res = parser.Parse(LogicalId{0, 0, 2}, U"sin;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "2.E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 3}, U"sin(2);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "0.909E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, variables8)
{
    parser.Parse(LogicalId{0, 0, 5}, U"b=4;");
    parser.Parse(LogicalId{0, 0, 3}, U"b=3;");
    parser.Parse(LogicalId{0, 0, 1}, U"b=2;");
    Complex res = parser.Parse(LogicalId{0, 0, 6}, U"b;");
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
}

TEST_F(CalcTestComplex, variables9)
{
    std::vector<std::u32string> dependencies;
    parser.Parse(LogicalId{0, 0, 1}, U"v{12}=555;");
    std::string s = parser.Parse(LogicalId{0, 0, 2}, U"v{12};", &dependencies).ToStdString(3, 3);
    ASSERT_TRUE(s == "555.E+0") << s;
    ASSERT_TRUE(std::find(dependencies.begin(), dependencies.end(), U"v{12}") != dependencies.end());
}

TEST_F(CalcTestComplex, variables10)
{
    yutovo_calculator::ParserContext parser_context;
    parser.Parse(LogicalId{0, 0, 1}, U"v=555;", &parser_context);
    ASSERT_TRUE(parser_context.no_result == true);
    parser.Parse(LogicalId{0, 0, 2}, U"v;", &parser_context);
    ASSERT_TRUE(parser_context.no_result == false);
}

TEST_F(CalcTestComplex, compare1)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"(0<3);").ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"((3+5*i)==(3+5*i));").ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 0, 0, 0, 2, 0}, U"((3+5*i)<>(3+5*i));").ToStdString(3, 3);
    ASSERT_TRUE(s == "0.E+0") << s;
}

TEST_F(CalcTestComplex, compare2)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 0}, U"a=3;");
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(a<10);").ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(a>10);").ToStdString(3, 3);
    ASSERT_TRUE(s == "0.E+0") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(a==3+5*i);").ToStdString(3, 3);
    ASSERT_TRUE(s == "0.E+0") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"(a<>3+5*i);").ToStdString(3, 3);
    ASSERT_TRUE(s == "1.E+0") << s;
}

TEST_F(CalcTestComplex, sum1)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(k=0,(k<=10),k=k+1,t=0,t=t+1);").ToStdString(3, 3);
    ASSERT_TRUE(s == "11.E+0") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(k=1,(k<=10),k=k+1,t=0,t=t+k);").ToStdString(3, 3);
    ASSERT_TRUE(s == "55.E+0") << s;
}

TEST_F(CalcTestComplex, sum2)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(k=0,(k<=10),k=k+1,t1=0,t1=t1+k*i+1);").ToStdString(3, 3);
    ASSERT_TRUE(s == "11.E+0+i*55.E+0") << s;
}

TEST_F(CalcTestComplex, prod1)
{
    std::string s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(k=0,(k<=5),k=k+1,t=1,t=t*(2));").ToStdString(3, 3);
    ASSERT_TRUE(s == "64.E+0") << s;
    s = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"loop(k=1,(k<=3),k=k+1,t=1,t=t*(i+5));").ToStdString(3, 3);
    ASSERT_TRUE(s == "110.E+0+i*74.E+0") << s;
}

TEST_F(CalcTestComplex, errors1)
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

TEST_F(CalcTestComplex, errors2)
{
    try
    {
        parser.Parse(LogicalId{0, 0, 1}, U"2/dj;");
    }
    catch (yutovo_calculator::SyntaxException& ex)
    {
        ASSERT_TRUE((ex.id == LogicalId{0, 0, 1}) && ex.ex_id == ParserExceptionCode::UnknownIdentifier && ex.pos == 2 && ex.size == 2) << ex.ex_id;
        return;
    }
    ASSERT_FALSE(true);
}

TEST_F(CalcTestComplex, errors3)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"(1)/(0);"), yutovo_calculator::MathException);
}

TEST_F(CalcTestComplex, errors4)
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

TEST_F(CalcTestComplex, user_functions1)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=5;");
    parser.Parse(LogicalId{0, 0, 2}, U"f(x)=x+5;");
    ASSERT_TRUE(parser.Parse(LogicalId{0, 0, 3}, U"f(2);") == parser.Parse(LogicalId{0, 0, 3}, U"7;")) << 
        parser.Parse(LogicalId{0, 0, 3}, U"f(2);").ToStdString(3, 3);
}

TEST_F(CalcTestComplex, user_functions2)
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

TEST_F(CalcTestComplex, user_functions3)
{
    std::vector<std::u32string> dependencies;
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x*2;");
    parser.Parse(LogicalId{0, 0, 2}, U"f(5);", &dependencies);
    ASSERT_TRUE(std::find(dependencies.begin(), dependencies.end(), U"f") != dependencies.end());
}

TEST_F(CalcTestComplex, user_functions4)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x)=x;");
    Complex res = parser.Parse(LogicalId{0, 0, 2}, U"f(5);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "5.E+0") << res.ToStdString(3, 3);

    parser.Parse(LogicalId{0, 0, 3}, U"f(x,y)=x+y;");
    res = parser.Parse(LogicalId{0, 0, 4}, U"f(5,4);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "9.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, user_functions5)
{
    parser.Parse(LogicalId{0, 0, 5}, U"p(a)=a+1;");
    parser.Parse(LogicalId{0, 0, 1}, U"p(a)=a+5;");
    Complex res = parser.Parse(LogicalId{0, 0, 6}, U"p(5);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "6.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 2}, U"p(5);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "10.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, user_functions6)
{
    parser.Parse(LogicalId{0, 0, 8}, U"p(a)=a+15;");
    Complex res = parser.Parse(LogicalId{0, 0, 9}, U"p(5);");
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

TEST_F(CalcTestComplex, user_functions7)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x,y)=pow(x,y);");
    Complex res = parser.Parse(LogicalId{0, 0, 2}, U"f(5,2);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "25.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, user_functions8)
{
    parser.Parse(LogicalId{0, 0, 1}, U"f(x,y)=(x)/(y);");
    Complex res = parser.Parse(LogicalId{0, 0, 2}, U"f(8,2);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "4.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, user_functions9)
{
    parser.Parse(LogicalId{0, 0, 1}, U"y=(sin((pi)/(2)))/(3.4);");
    parser.Parse(LogicalId{0, 0, 2}, U"r=sqrt(y);");
    Complex res = parser.Parse(LogicalId{0, 0, 3}, U"r;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "0.542E+0") << res.ToStdString(3, 3);
    ASSERT_TRUE(res.GetBitPrecision() < 1000) << res.GetBitPrecision();
    parser.Parse(LogicalId{0, 0, 4}, U"f(t)=t*cos(r*2);");
    res = parser.Parse(LogicalId{0, 0, 5}, U"f(3.4);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.589E+0") << res.ToStdString(3, 3);
    ASSERT_TRUE(res.GetBitPrecision() < 1000) << res.GetBitPrecision();
}

TEST_F(CalcTestComplex, pow_int)
{
    int res_pos = 0;
    Complex base(64, 2);
    Complex res = pow(base, 0, res_pos);
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.E+0") << res.ToStdString(3, 3);
    res = pow(base, 1, res_pos);
    ASSERT_TRUE(res.ToStdString(3, 3) == "2.E+0") << res.ToStdString(3, 3);
    res = pow(base, 2, res_pos);
    ASSERT_TRUE(res.ToStdString(3, 3) == "4.E+0") << res.ToStdString(3, 3);
    res = pow(base, 3, res_pos);
    ASSERT_TRUE(res.ToStdString(3, 3) == "8.E+0") << res.ToStdString(3, 3);
    res = pow(base, 10, res_pos);
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.024E+3") << res.ToStdString(3, 3);
    res = pow(base, -1, res_pos);
    ASSERT_TRUE(res.ToStdString(3, 3) == "0.5E+0") << res.ToStdString(3, 3);
    res = pow(base, -2, res_pos);
    ASSERT_TRUE(res.ToStdString(3, 3) == "0.25E+0") << res.ToStdString(3, 3);
    res = pow(base, -3, res_pos);
    ASSERT_TRUE(res.ToStdString(3, 3) == "0.125E+0") << res.ToStdString(3, 3);
}


TEST_F(CalcTestComplex, sin_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"sin(pow(10,20));");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None);
    ASSERT_FALSE(res.GetRe() == Real(3, 0)) << res.ToStdString(3, 10);
    ASSERT_TRUE(res.GetIm() == Real(3, 0)) << res.ToStdString(3, 10);
    ASSERT_TRUE(res.ToStdString(3, 3) == "-0.645E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, cos_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"cos(pow(10,20));");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None);
    ASSERT_FALSE(res.GetRe() == Real(3, 1)) << res.ToStdString(3, 10);
    ASSERT_FALSE(res.GetRe() == Real(3, -1)) << res.ToStdString(3, 10);
    ASSERT_TRUE(res.GetIm() == Real(3, 0)) << res.ToStdString(3, 10);
    ASSERT_TRUE(res.ToStdString(3, 3) == "0.764E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, tg_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"tg(pow(10,20));");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None);
    ASSERT_FALSE(res.GetRe() == Real(3, 0)) << res.ToStdString(3, 10);
    ASSERT_TRUE(res.GetIm() == Real(3, 0)) << res.ToStdString(3, 10);
    ASSERT_TRUE(res.ToStdString(3, 3) == "-0.845E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, ctg_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"ctg(pow(10,20));");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None);
    ASSERT_FALSE(res.GetRe() == Real(3, 0)) << res.ToStdString(3, 10);
    ASSERT_TRUE(res.GetIm() == Real(3, 0)) << res.ToStdString(3, 10);
    ASSERT_TRUE(res.ToStdString(3, 3) == "-1.184E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, sec_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"sec(pow(10,20));");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None);
    ASSERT_FALSE(res.GetRe() == Real(3, 1)) << res.ToStdString(3, 10);
    ASSERT_FALSE(res.GetRe() == Real(3, -1)) << res.ToStdString(3, 10);
    ASSERT_TRUE(res.GetIm() == Real(3, 0)) << res.ToStdString(3, 10);
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.309E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, cosec_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"cosec(pow(10,20));");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None);
    ASSERT_FALSE(res.GetRe() == Real(3, 0)) << res.ToStdString(3, 10);
    ASSERT_TRUE(res.GetIm() == Real(3, 0)) << res.ToStdString(3, 10);
    ASSERT_TRUE(res.ToStdString(3, 3) == "-1.55E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, ln_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"ln(pow(10,20));");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None);
    ASSERT_TRUE(res.ToStdString(3, 3) == "46.052E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, lg_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"lg(pow(10,20));");
    ASSERT_TRUE(res.ToStdString(3, 3) == "20.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, log_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"log(2,pow(10,20));");
    ASSERT_TRUE(res.ToStdString(3, 3) == "66.439E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, exp_large_arg)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"exp(pow(10,20));"), yutovo_calculator::MathException);
}

TEST_F(CalcTestComplex, arctg_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arctg(pow(10,20));");
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.571E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, arcctg_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arcctg(pow(10,20));");
    ASSERT_TRUE(res.ToStdString(3, 3) == "0.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, arcsec_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arcsec(pow(10,20));");
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.571E+0+i*5.E-41") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, arccosec_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arccosec(pow(10,20));");
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.E-20+i*-5.E-41") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, th_large_arg)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"th(pow(10,20));"), yutovo_calculator::MathException);
}

TEST_F(CalcTestComplex, cth_large_arg)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"cth(pow(10,20));"), yutovo_calculator::MathException);
}

TEST_F(CalcTestComplex, sch_large_arg)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"sch(pow(10,20));"), yutovo_calculator::MathException);
}

TEST_F(CalcTestComplex, csch_large_arg)
{
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 1}, U"csch(pow(10,20));"), yutovo_calculator::MathException);
}

TEST_F(CalcTestComplex, arsh_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arsh(pow(10,20));");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None);
    ASSERT_TRUE(res.ToStdString(3, 3) == "46.745E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, arcth_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arcth(pow(10,20));");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None);
    ASSERT_TRUE(res.ToStdString(3, 3) == "3.942E-21") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, arcsch_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arcsch(pow(10,20));");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None);
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.E-20") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, arcsin_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arcsin(pow(10,20));");
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.571E+0+i*-46.745E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, arccos_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arccos(pow(10,20));");
    ASSERT_TRUE(res.ToStdString(3, 3) == "2.508E-20+i*46.745E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, arch_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"arch(pow(10,20));");
    ASSERT_TRUE(res.ToStdString(3, 3) == "46.745E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, sqrt_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"sqrt(pow(10,20));");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None);
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.E+10") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, root_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"root(pow(10,20),3);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None);
    ASSERT_TRUE(res.ToStdString(3, 3) == "4.642E+6") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, pow_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"pow(10,20);");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None);
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.E+20") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, abs_large_arg)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"abs(-pow(10,20));");
    ASSERT_TRUE(res.GetAngleMeasure() == AngleMeasure::None);
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.E+20") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, fact1)
{
    Complex res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"0!;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1!;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"5!;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "120.E+0") << res.ToStdString(3, 3);
    res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"10!;");
    ASSERT_TRUE(res.ToStdString(3, 3) == "3.629E+6") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, fact2)
{
    yutovo_calculator::ParserContext parser_context;
    parser_context.Init(100);
    EXPECT_THROW(parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1000000!;", &parser_context),
        yutovo_calculator::TimeExceedException);
}

TEST_F(CalcTestComplex, min_max1)
{
    Complex res = parser.Parse(LogicalId{0, 0, 1}, U"min(1,2);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"max(1,2);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "2.E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"min(1+2i,3+4i);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "1.E+0+i*2.E+0") << res.ToStdString(3, 3);

    res = parser.Parse(LogicalId{0, 0, 1}, U"max(1+2i,3+4i);");
    ASSERT_TRUE(res.ToStdString(3, 3) == "3.E+0+i*4.E+0") << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, definite_integral1)
{
    Complex res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,1,x,x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "0.5E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestComplex, definite_integral2)
{
    Complex res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(-1,4,sin(x),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "1.1939459267E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestComplex, definite_integral3)
{
    Complex res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,pi,sin(x),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "2.E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestComplex, definite_integral4)
{
    Complex res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,2,pow(x,2),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "2.6666666667E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestComplex, definite_integral5)
{
    Complex res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,1,pow(x,3),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "0.25E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestComplex, definite_integral6)
{
    Complex res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,pi,cos(x),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "1.5022025553E-16") << res.ToStdString(10, 10);
}

TEST_F(CalcTestComplex, definite_integral7)
{
    Complex res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,10,pow(x,2),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "333.33333333E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestComplex, definite_integral8)
{
    Complex res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"definite_integral(0,1,exp(x),x);", 10);
    ASSERT_TRUE(res.ToStdString(10, 10) == "1.7182818285E+0") << res.ToStdString(10, 10);
}

TEST_F(CalcTestComplex, derivative_at_point1)
{
    Complex res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x*x, [x=3]);");
    ASSERT_TRUE(res == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"6;")) << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, derivative_at_point2)
{
    Complex res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(sin(x), [x=0]);");
    ASSERT_TRUE(res == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"1;")) << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, derivative_at_point3)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x)=x*x;");
    Complex res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x), [x=2]);");
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"4;").ToStdString(3, 3)) << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, derivative_at_point_multi1)
{
    Complex res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x*y, [x=2, y=3]);");
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"3;").ToStdString(3, 3)) << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, derivative_at_point_multi2)
{
    Complex res = parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"derivative(x*y, [y=3, x=2]);");
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"2;").ToStdString(3, 3)) << res.ToStdString(3, 3);
}

TEST_F(CalcTestComplex, derivative_at_point_multi3)
{
    parser.Parse(LogicalId{0, 0, 0, 0, 1}, U"f(x,y)=pow(x,2)*y;");
    Complex res = parser.Parse(LogicalId{0, 0, 0, 0, 2}, U"derivative(f(x,y), [x=2, y=3]);");
    ASSERT_TRUE(res.ToStdString(3, 3) == parser.Parse(LogicalId{0, 0, 0, 0, 3}, U"12;").ToStdString(3, 3)) << res.ToStdString(3, 3);
}

}
