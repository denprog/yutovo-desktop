/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __MOCK_H__
#define __MOCK_H__

#include <gmock/gmock.h>
#include "parser.h"

namespace yutovo_calc_test
{

using namespace yutovo_calculator;

struct CalcTestInteger : public testing::Test
{
    CalcTestInteger() :
        parser(0, Language::English)
    {
    }

    Parser<Integer> parser;
};

struct CalcTestReal : public testing::Test
{
    CalcTestReal() :
        parser(3, Language::English)
    {
    }

    bool FindUnit(const std::vector<Unit>& units, const Unit& unit)
    {
        return std::find(units.begin(), units.end(), unit) != units.end();
    }

    Parser<Real> parser;
};

struct CalcTestRational : public testing::Test
{
    CalcTestRational() :
        parser(0, Language::English)
    {
    }

    bool FindUnit(const std::vector<Unit>& units, const Unit& unit)
    {
        return std::find(units.begin(), units.end(), unit) != units.end();
    }

    Parser<Rational> parser;
};

struct CalcTestComplex : public testing::Test
{
    CalcTestComplex() :
        parser(3, Language::English)
    {
    }

    bool FindUnit(const std::vector<Unit>& units, const Unit& unit)
    {
        return std::find(units.begin(), units.end(), unit) != units.end();
    }

    std::string ToString(std::vector<Complex>& numbers)
    {
        std::string res;
        for (size_t i = 0; i < numbers.size(); ++i)
        {
            Complex& n = numbers[i];
            res += n.ToStdString(3, 3);
            if (i < numbers.size() - 1)
                res += ",";
        }
        return res;
    }

    Parser<Complex> parser;
};

struct CalcTestArrayReal : public testing::Test
{
    CalcTestArrayReal() :
        parser(0, Language::English)
    {
    }

    Parser<Array<Real>> parser;
};

struct CalcTestSymbolicReal : public testing::Test
{
    CalcTestSymbolicReal() :
        parser(3, Language::English)
    {
    }

    Parser<Symbolic<Real>> parser;
};

struct CalcTestSymbolicRational : public testing::Test
{
    CalcTestSymbolicRational() :
        parser(3, Language::English)
    {
    }

    Parser<Symbolic<Rational>> parser;
};

struct CalcTestSymbolicComplex : public testing::Test
{
    CalcTestSymbolicComplex() :
        parser(3, Language::English)
    {
    }

    Parser<Symbolic<Complex>> parser;
};

}

#endif
