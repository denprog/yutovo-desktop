/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "stdafx.h"
#include "math_helper.h"
#include <algorithm>
#include "integer.h"
#include "real.h"
#include "rational.h"

namespace yutovo_calculator
{

std::map<int, int> MathHelper::bit_precisions;
std::map<int, int> MathHelper::decimal_precisions;
std::map<int, Real> MathHelper::real_miscs;

struct Widen
{
	wchar_t operator()(char c)
	{
  		return std::use_facet<std::ctype<char> >(std::locale()).widen(c);
	}
};

//Converts the decimal precision to the bit precision of the mpfr.
int MathHelper::ToBitPrecision(const int precision)
{
	if (bit_precisions[precision] != 0)
		return bit_precisions[precision];
	
	std::string str = "1";
	for (int i = 0; i < precision; ++i)
		str += "0";
	
	mpz_t number;
	mpz_init(number);
	mpz_set_str(number, str.c_str(), 10);
	int res = mpz_sizeinbase(number, 2);
	bit_precisions[precision] = res;
	mpz_clear(number);
	
	return res;
}

//Converts the bit precision to the decimal precision.
int MathHelper::ToDecimalPrecision(const int precision)
{
	if (decimal_precisions[precision] != 0)
		return decimal_precisions[precision];
	
	std::string str = "1";
	for (int i = 0; i < precision; ++i)
		str += "1";
	
	mpz_t number;
	mpz_init(number);
	mpz_set_str(number, str.c_str(), 2);
	int res = mpz_sizeinbase(number, 10);
	decimal_precisions[precision] = res;
	mpz_clear(number);
	
	return res;
}

//Gets the calculation error, which presents a number 0.00001, where the number of zeros after the point is the precision.
template<>
Real MathHelper::GetMisc(const Real& num)
{
	std::map<int, Real>::iterator iter = real_miscs.find(num.GetBitPrecision());

	if (iter == real_miscs.end())
	{
		Real misc = pow(Real(num.GetBitPrecision(), (float)0.1), Real(num.GetBitPrecision(), (int)(num.GetPrecision() - 2)));
		real_miscs[num.GetBitPrecision()] = misc;

		return misc;
	}

	return iter->second;
}

//Gets a misc of Integer number.
template<>
Integer MathHelper::GetMisc(const Integer& num)
{
	return Integer(0);
}

//Gets a misc of Rational number.
template<>
Rational MathHelper::GetMisc(const Rational& num)
{
	return Rational(0);
}
}
