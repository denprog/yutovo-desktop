/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef MATHHELPER_H
#define MATHHELPER_H

#include <string>
#include <map>

#define DEFAULT_BASE 10
#define DEFAULT_INCREASE_PRECISION 5
#define DEFAULT_RND MPFR_RNDN

#ifdef DEBUG
#define TRACE_OUTPUT
#endif

namespace yutovo_calculator
{

enum class AngleMeasure
{
	None = -1, 
	Radian = 0, 
	Degree, 
	Grad
};

enum class Notation
{
	None = -1,
	Binary = 0,
	Octal,
	Decimal,
	Hexadecimal
};

enum class FractionForm
{
	None = -1,
    Proper = 0,
    Improper
};

enum class ComplexForm
{
	None = -1,
    Arithmetic = 0,
    Trigonometric,
    Exponential
};

enum class Language
{
	None = 0,
	English = 1,
	Russian,
	Spanish,
	BrazilianPortuguese
};

class Real;

class MathHelper
{
public:
	static int ToBitPrecision(const int precision);
	static int ToDecimalPrecision(const int precision);

	template<class TNumber>
	static TNumber GetMisc(const TNumber& num);
	
private:
	static std::map<int, int> bit_precisions; //the bit precisions cache
	static std::map<int, int> decimal_precisions; //the decimal precisions cache
	static std::map<int, Real> real_miscs;	//the real miscs cache
};

}

#endif
