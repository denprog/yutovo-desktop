/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "stdafx.h"
#include "real.h"
#include "utils.h"

#ifdef max
#undef max
#endif

namespace yutovo_calculator
{

//Real

Real pi(const int precision);

Real::Real()
{
    mpfr_init2(number, mpfr_get_default_prec());
    mpfr_set_si(number, 0, GMP_RNDN);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Real::Real(int precision)
{
    mpfr_init2(number, std::max((int)mpfr_get_default_prec(), precision));
    mpfr_set_si(number, 0, GMP_RNDN);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Real::Real(int precision, Unit& _unit) :
    unit(_unit)
{
    mpfr_init2(number, std::max((int)mpfr_get_default_prec(), precision));
    mpfr_set_si(number, 1, GMP_RNDN);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Real::Real(int precision, AngleMeasure _angle_measure) :
    angle_measure(_angle_measure)
{
    mpfr_init2(number, std::max((int)mpfr_get_default_prec(), precision));
    mpfr_set_si(number, 0, GMP_RNDN);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Real::Real(int precision, const char* num)
{
    mpfr_init2(number, std::max((int)mpfr_get_default_prec(), precision));
    if (mpfr_set_str(number, num, DEFAULT_BASE, MPFR_RNDZ) != 0)
        throw SyntaxException(SyntaxError);
    string_number = ToUtfString(num);
    //mpfr_prec_round(number, precision / 2, GMP_RNDN);
    //addPrecision = (int)strchr(num, '.');
    //if (!addPrecision)
    //	addPrecision = strlen(num);
    //else
    //	addPrecision -= (int)num;

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Real::Real(int precision, int num)
{
    mpfr_init2(number, std::max((int)mpfr_get_default_prec(), precision));
    mpfr_set_si(number, num, GMP_RNDN);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Real::Real(int precision, float num)
{
    mpfr_init2(number, std::max((int)mpfr_get_default_prec(), precision));
    mpfr_set_d(number, num, GMP_RNDN);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Real::Real(mpfr_srcptr _number)
{
    mpfr_init2(number, mpfr_get_prec(_number));
    mpfr_set(number, _number, MPFR_RNDN);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Real::Real(const std::u32string& num)
{
    string_number = num;
    mpfr_init2(number, std::max((int)mpfr_get_default_prec(), MathHelper::ToBitPrecision(num.length() * 2)) + 1);
    if (mpfr_set_str(number, ToBasicString(num).c_str(), DEFAULT_BASE, MPFR_RNDA) != 0)
        throw SyntaxException(SyntaxError);
    SetPrecision(GetPrecision() + GetExp() - 1);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Real::Real(const Real& source) :
    string_number(source.string_number),
    angle_measure(source.angle_measure),
    unit(source.unit)
{
    mpfr_init2(number, source.GetBitPrecision());
    mpfr_set(number, source.number, GMP_RNDN);
    
#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Real::~Real()
{
    mpfr_clear(number);
}

Real& Real::operator=(const Real& source)
{
    if (this == &source)
        return *this;

    mpfr_clear(number);
    mpfr_init2(number, source.GetBitPrecision());
    mpfr_set(number, source.number, GMP_RNDN);
    string_number = source.string_number;
    angle_measure = source.angle_measure;
    unit = source.unit;

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif

    return *this;
}

Real& Real::operator=(const int num)
{
    mpfr_set_si(number, num, GMP_RNDN);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif

    return *this;
}

Real& Real::operator=(const double num)
{
    mpfr_set_d(number, num, GMP_RNDN);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif

    return *this;
}

Real& Real::operator=(const char* num)
{
    mpfr_t t;
    mpfr_init2(t, GetBitPrecision());

    if (mpfr_set_str(t, num, DEFAULT_BASE, GMP_RNDN) == 0)
    {
        mpfr_set(number, t, GMP_RNDN);
        mpfr_clear(t);
    }
    else
    {
        mpfr_clear(t);
    }

    string_number = ToUtfString(num);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif

    return *this;
}

Real& Real::operator=(const std::u32string& num)
{
    mpfr_t t;

    if ((int)num.length() + 1 > GetPrecision())
    {
        SetBitPrecision(MathHelper::ToBitPrecision(num.length() * 2));
        mpfr_init2(t, GetBitPrecision());
    }
    else
        mpfr_init2(t, GetBitPrecision());

    if (mpfr_set_str(t, ToBasicString(num).c_str(), DEFAULT_BASE, GMP_RNDN) == 0)
    {
        mpfr_set(number, t, GMP_RNDN);
        mpfr_clear(t);
    }
    else
    {
        mpfr_clear(t);
    }

    string_number = num;

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif

    return *this;
}

Real Real::operator+()
{
    return *this;
}

Real Real::operator-()
{
    Real res(GetBitPrecision(), angle_measure);

    mpfr_neg(res.number, number, GMP_RNDN);

    res.unit = unit;

    return res;
}

Real Real::operator++()
{
    *this += 1;

    return *this;
}

Real Real::operator--()
{
    *this -= 1;

    return *this;
}

Real operator+(const Real& num1, const Real& num2)
{
    Real _num1 = num1;
    Real _num2 = num2;
    ToCommonAngleMeasure(_num1, _num2);
    Real res(std::max(_num1.GetBitPrecision(), _num2.GetBitPrecision()), _num1.angle_measure);
    res.CalcFunc(mpfr_add, _num1, _num2);
    res.unit = num1.unit + num2.unit;
    return res;
}

Real operator+(const Real& num1, const int num2)
{
    bool convert = num1.angle_measure != AngleMeasure::None;
    Real _num1 = convert ? num1.ToRadian() : num1;
    Real res(_num1.GetBitPrecision(), convert ? AngleMeasure::Radian : AngleMeasure::None);
    res.CalcFunc(mpfr_add_si, _num1, num2);
    res.unit = num1.unit + num2;
    return res;
}

Real operator+(const int num1, const Real& num2)
{
    bool convert = num2.angle_measure != AngleMeasure::None;
    Real _num2 = convert ? num2.ToRadian() : num2;
    Real res(_num2.GetBitPrecision(), convert ? AngleMeasure::Radian : AngleMeasure::None);
    res.CalcFunc(mpfr_add_si, _num2, num1);
    res.unit = num1 + num2.unit;
    return res;
}

Real operator+(const Real& num1, const float num2)
{
    bool convert = num1.angle_measure != AngleMeasure::None;
    Real _num1 = convert ? num1.ToRadian() : num1;
    Real res(_num1.GetBitPrecision(), convert ? AngleMeasure::Radian : AngleMeasure::None);
    res.CalcFunc(mpfr_add_d, _num1, num2);
    res.unit = num1.unit + num2;
    return res;
}

Real operator+(const float num1, const Real& num2)
{
    bool convert = num2.angle_measure != AngleMeasure::None;
    Real _num2 = convert ? num2.ToRadian() : num2;
    Real res(_num2.GetBitPrecision(), convert ? AngleMeasure::Radian : AngleMeasure::None);
    res.CalcFunc(mpfr_add_d, _num2, num1);
    res.unit = num1 + num2.unit;
    return res;
}

Real operator-(const Real& num1, const Real& num2)
{
    Real _num1 = num1;
    Real _num2 = num2;
    ToCommonAngleMeasure(_num1, _num2);
    Real res(std::max(_num1.GetBitPrecision(), _num2.GetBitPrecision()), _num1.angle_measure);
    res.CalcFunc(mpfr_sub, _num1, _num2);
    res.unit = num1.unit - num2.unit;
#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real operator-(const Real& num1, const int num2)
{
    bool convert = num1.angle_measure != AngleMeasure::None;
    Real _num1 = convert ? num1.ToRadian() : num1;
    Real res(_num1.GetBitPrecision(), convert ? AngleMeasure::Radian : AngleMeasure::None);
    res.CalcFunc(mpfr_sub_si, _num1, num2);
    res.unit = num1.unit - num2;
    return res;
}

Real operator-(const int num1, const Real& num2)
{
    bool convert = num2.angle_measure != AngleMeasure::None;
    Real _num2 = convert ? num2.ToRadian() : num2;
    Real res(_num2.GetBitPrecision(), convert ? AngleMeasure::Radian : AngleMeasure::None);
    res.CalcFunc(mpfr_si_sub, num1, _num2);
    res.unit = num1 - num2.unit;
    return res;
}

Real operator-(const Real& num1, const float num2)
{
    bool convert = num1.angle_measure != AngleMeasure::None;
    Real _num1 = convert ? num1.ToRadian() : num1;
    Real res(_num1.GetBitPrecision(), convert ? AngleMeasure::Radian : AngleMeasure::None);
    res.CalcFunc(mpfr_sub_d, _num1, num2);
    res.unit = num1.unit - num2;
    return res;
}

Real operator-(const float num1, const Real& num2)
{
    bool convert = num2.angle_measure != AngleMeasure::None;
    Real _num2 = convert ? num2.ToRadian() : num2;
    Real res(_num2.GetBitPrecision(), convert ? AngleMeasure::Radian : AngleMeasure::None);
    res.CalcFunc(mpfr_d_sub, num1, _num2);
    res.unit = num1 - num2.unit;
    return res;
}

Real operator*(const Real& num1, const Real& num2)
{
    Real _num1 = num1;
    Real _num2 = num2;
    ToCommonAngleMeasure(_num1, _num2);
    mpfr_prec_t cur_precision = std::max(_num1.GetBitPrecision(), _num2.GetBitPrecision());
    Real res(std::max(_num1.GetBitPrecision() + 2, _num2.GetBitPrecision() + 2), _num1.angle_measure);
    res.CalcFunc(mpfr_mul, _num1, _num2);
    mpfr_prec_round(res.number, cur_precision, DEFAULT_RND);
    res.unit = num1.unit * num2.unit;
#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real operator*(const Real& num1, const int num2)
{
    Real res(num1.GetBitPrecision(), num1.angle_measure);
    res.CalcFunc(mpfr_mul_si, num1, num2);
    res.unit = num1.unit * num2;
#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real operator*(const int num1, const Real& num2)
{
    Real res(num2.GetBitPrecision(), num2.angle_measure);
    res.CalcFunc(mpfr_mul_si, num2, num1);
    res.unit = num1 * num2.unit;
#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real operator*(const Real& num1, const float num2)
{
    Real res(num1.GetBitPrecision(), num1.angle_measure);
    res.CalcFunc(mpfr_mul_d, num1, num2);
    res.unit = num1.unit * num2;
#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real operator*(const float num1, const Real& num2)
{
    Real res(num2.GetBitPrecision(), num2.angle_measure);
    res.CalcFunc(mpfr_mul_d, num2, num1);
    res.unit = num1 * num2.unit;
#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real operator/(const Real& num1, const Real& num2)
{
    Real _num1 = num1;
    Real _num2 = num2;
    ToCommonAngleMeasure(_num1, _num2);
    Real res(std::max(_num1.GetBitPrecision(), _num2.GetBitPrecision()), _num1.angle_measure);
    res.CalcFunc(mpfr_div, _num1, _num2);
    if (res.IsInfinity() || res.IsNaN())
        throw MathException(DivisionByZero);
    res.unit = num1.unit / num2.unit;
#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real operator/(const Real& num1, const int num2)
{
    Real res(num1.GetBitPrecision(), num1.angle_measure);
    res.CalcFunc(mpfr_div_si, num1, num2);
    if (res.IsInfinity() || res.IsNaN())
        throw MathException(Overflow);
    res.unit = num1.unit / num2;
    return res;
}

Real operator/(const int num1, const Real& num2)
{
    Real res(num2.GetBitPrecision(), num2.angle_measure);
    res.CalcFunc(mpfr_si_div, num1, num2);
    if (res.IsInfinity() || res.IsNaN())
        throw MathException(Overflow);
    res.unit = num1 / num2.unit;
    return res;
}

Real operator/(const Real& num1, const float num2)
{
    Real res(num1.GetBitPrecision(), num1.angle_measure);
    res.CalcFunc(mpfr_div_d, num1, num2);
    if (res.IsInfinity() || res.IsNaN())
        throw MathException(Overflow);
    res.unit = num1.unit / num2;
    return res;
}

Real operator/(const float num1, const Real& num2)
{
    Real res(num2.GetBitPrecision(), num2.angle_measure);
    res.CalcFunc(mpfr_d_div, num1, num2);
    if (res.IsInfinity() || res.IsNaN())
        throw MathException(Overflow);
    res.unit = num1 / num2.unit;
    return res;
}

void Real::operator+=(const Real& num)
{
    *this = *this + num;
}

void Real::operator+=(const int num)
{
    *this = *this + num;
}

void Real::operator+=(const float num)
{
    *this = *this + num;
}

void Real::operator-=(const Real& num)
{
    *this = *this - num;
}

void Real::operator-=(const int num)
{
    *this = *this - num;
}

void Real::operator-=(const float num)
{
    *this = *this - num;
}

void Real::operator*=(const Real& num)
{
    *this = *this * num;
}

void Real::operator*=(const int num)
{
    *this = *this * num;
}

void Real::operator*=(const float num)
{
    *this = *this * num;
}

void Real::operator/=(const Real& num)
{
    *this = *this / num;
}

void Real::operator/=(const int num)
{
    *this = *this / num;
}

void Real::operator/=(const float num)
{
    *this = *this / num;
}

Real::operator int() const
{
    if (IsNaN())
        throw MathException(Overflow);
    if (IsInfinity())
        throw MathException(Overflow);
    if (!IsInteger())
        throw MathException(ConversionDoesNotFit);
    if (mpfr_fits_sint_p(number, DEFAULT_RND) == 0)
        throw MathException(ConversionDoesNotFit);
    return mpfr_get_si(number, DEFAULT_RND);
}

bool operator==(const Real& num1, const Real& num2)
{
    if (num1.IsNaN() || num2.IsNaN())
        throw MathException(Overflow);

    return mpfr_equal_p(num1.number, num2.number) != 0;
}

bool operator==(const Real& num1, const int num2)
{
    if (num1.IsNaN())
        throw MathException(Overflow);

    return mpfr_cmp_si(num1.number, num2) == 0;
}

bool operator==(const int num1, const Real& num2)
{
    if (num2.IsNaN())
        throw MathException(Overflow);

    return mpfr_cmp_si(num2.number, num1) == 0;
}

bool operator==(const Real& num1, const float num2)
{
    if (num1.IsNaN())
        throw MathException(Overflow);

    return mpfr_cmp_d(num1.number, num2) == 0;
}

bool operator==(const float num1, const Real& num2)
{
    if (num2.IsNaN())
        throw MathException(Overflow);

    return mpfr_cmp_d(num2.number, num1) == 0;
}

bool operator==(const Real& num1, const double num2)
{
    if (num1.IsNaN())
        throw MathException(Overflow);

    return mpfr_cmp_d(num1.number, num2) == 0;
}

bool operator==(const double num1, const Real& num2)
{
    if (num2.IsNaN())
        throw MathException(Overflow);

    return mpfr_cmp_d(num2.number, num1) == 0;
}

bool operator==(const Real& num1, const char* num2)
{
    return num1.ToStdString(3, 3) == num2;
}

bool operator==(const char* num1, const Real& num2)
{
    return num2 == num1;
}

bool operator!=(const Real& num1, const Real& num2)
{
    if (num1.IsNaN() || num2.IsNaN())
        throw MathException(Overflow);

    return mpfr_equal_p(num1.number, num2.number) == 0;
}

bool operator!=(const Real& num1, const int num2)
{
    if (num1.IsNaN())
        throw MathException(Overflow);

    return mpfr_cmp_si(num1.number, num2) != 0;
}

bool operator!=(const int num1, const Real& num2)
{
    if (num2.IsNaN())
        throw MathException(Overflow);

    return mpfr_cmp_si(num2.number, num1) != 0;
}

bool operator!=(const Real& num1, const float num2)
{
    if (num1.IsNaN())
        throw MathException(Overflow);

    return mpfr_cmp_d(num1.number, num2) != 0;
}

bool operator!=(const float num1, const Real& num2)
{
    if (num2.IsNaN())
        throw MathException(Overflow);

    return mpfr_cmp_d(num2.number, num1) != 0;
}

bool operator>(const Real& num1, const Real& num2)
{
    if (num1.IsNaN() || num2.IsNaN())
        throw MathException(Overflow);

    return mpfr_greater_p(num1.number, num2.number) != 0;
}

bool operator>(const Real& num1, const int num2)
{
    if (num1.IsNaN())
        throw MathException(Overflow);

    return mpfr_cmp_si(num1.number, num2) > 0;
}

bool operator>(const int num1, const Real& num2)
{
    Real _num1(num2.GetBitPrecision(), num1);

    return _num1 > num2;
}

bool operator>(const Real& num1, const float num2)
{
    if (num1.IsNaN())
        throw MathException(Overflow);

    return mpfr_cmp_d(num1.number, num2) > 0;
}

bool operator>(const float num1, const Real& num2)
{
    Real _num1(num2.GetBitPrecision(), num1);

    return _num1 > num2;
}

bool operator>=(const Real& num1, const Real& num2)
{
    if (num1.IsNaN() || num2.IsNaN())
        throw MathException(Overflow);

    return mpfr_greaterequal_p(num1.number, num2.number) != 0;
}

bool operator>=(const Real& num1, const int num2)
{
    return (num1 > num2) || (num1 == num2);
}

bool operator>=(const int num1, const Real& num2)
{
    return (num1 > num2) || (num1 == num2);
}

bool operator>=(const Real& num1, const float num2)
{
    return (num1 > num2) || (num1 == num2);
}

bool operator>=(const float num1, const Real& num2)
{
    return (num1 > num2) || (num1 == num2);
}

bool operator<(const Real& num1, const Real& num2)
{
    if (num1.IsNaN() || num2.IsNaN())
        throw MathException(Overflow);

    return mpfr_less_p(num1.number, num2.number) != 0;
}

bool operator<(const Real& num1, const int num2)
{
    if (num1.IsNaN())
        throw MathException(Overflow);

    return mpfr_cmp_si(num1.number, num2) < 0;
}

bool operator<(const int num1, const Real& num2)
{
    Real _num1(num2.GetBitPrecision(), num1);

    return _num1 < num2;
}

bool operator<(const Real& num1, const float num2)
{
    if (num1.IsNaN())
        throw MathException(Overflow);

    return mpfr_cmp_d(num1.number, num2) < 0;
}

bool operator<(const float num1, const Real& num2)
{
    Real _num1(num2.GetBitPrecision(), num1);

    return _num1 < num2;
}

bool operator<=(const Real& num1, const Real& num2)
{
    if (num1.IsNaN() || num2.IsNaN())
        throw MathException(Overflow);

    return mpfr_lessequal_p(num1.number, num2.number) != 0;
}

bool operator<=(const Real& num1, const int num2)
{
    return (num1 < num2) || (num1 == num2);
}

bool operator<=(const int num1, const Real& num2)
{
    return (num1 < num2) || (num1 == num2);
}

bool operator<=(const Real& num1, const float num2)
{
    return (num1 < num2) || (num1 == num2);
}

bool operator<=(const float num1, const Real& num2)
{
    return (num1 < num2) || (num1 == num2);
}

Real exp(const Real& num)
{
    Real res(num.GetBitPrecision() * 2);
    res.CalcFunc(mpfr_exp, num);
    return res;
}

Real ln(const Real& num)
{
    Real res(num.GetBitPrecision() * 2);
    res.CalcFunc(mpfr_log, num);
    return res;
}

Real lg(const Real& num)
{
    return ln(num) / ln(Real(num.GetBitPrecision(), 10));
}

Real log(const Real& num1, const Real& num2)
{
    return ln(num2) / ln(num1);
}

Real sin(const Real& num)
{
    assert(num.angle_measure != AngleMeasure::None);

    Real res(num.GetBitPrecision());
    Real _num = num;

    Real::CheckUnit(_num);

    if (_num.angle_measure != AngleMeasure::Radian)
    {
        _num = _num.ToRadian();
        res.CalcFunc(mpfr_sin, _num);
        return res;
    }

    Real _pi = pi(_num.GetBitPrecision());
    Real misc = MathHelper::GetMisc<Real>(_num);
    Real r(num.GetBitPrecision());
    r.CalcFunc(mpfr_remainder, _num, _pi * 2);
    r = abs(r);
    if (r <= misc || abs(r - _pi) <= misc)
        res = 0;
    else
        res.CalcFunc(mpfr_sin, _num);

    return res;
}

Real cos(const Real& num)
{
    assert(num.angle_measure != AngleMeasure::None);

    Real res(num.GetBitPrecision());
    Real _num = num;

    Real::CheckUnit(_num);

    if (_num.angle_measure != AngleMeasure::Radian)
    {
        _num = _num.ToRadian();
        res.CalcFunc(mpfr_cos, _num);
        return res;
    }

    Real _pi = pi(_num.GetBitPrecision());
    Real misc = MathHelper::GetMisc<Real>(_num);
    Real r(num.GetBitPrecision());
    r.CalcFunc(mpfr_remainder, _num, _pi * 2);
    r = abs(r);
    if (r <= misc)
        res = 1;
    else if (abs(r - _pi) <= misc)
        res = -1;
    else if (abs(r - _pi / 2) <= misc)
        res = 0;
    else
        res.CalcFunc(mpfr_cos, _num);

    return res;
}

Real tg(const Real& num)
{
    assert(num.angle_measure != AngleMeasure::None);

    Real res(num.GetBitPrecision());
    Real _num = num;

    Real::CheckUnit(_num);

    if (_num.angle_measure != AngleMeasure::Radian)
        _num = _num.ToRadian();

    res.CalcFunc(mpfr_tan, _num);

    if (res.IsInfinity() || res.IsNaN())
        throw MathException(Overflow);

    return res;
}

Real ctg(const Real& num)
{
    assert(num.angle_measure != AngleMeasure::None);

    Real res(num.GetBitPrecision());
    Real _num = num;

    Real::CheckUnit(_num);

    if (_num.angle_measure != AngleMeasure::Radian)
        _num = _num.ToRadian();

    res.CalcFunc(mpfr_cot, _num);

    if (res.IsInfinity() || res.IsNaN())
        throw MathException(Overflow);

    return res;
}

Real sec(const Real& num)
{
    Real res(num.GetBitPrecision());
    Real _num = num;

    Real::CheckUnit(_num);

    if (_num.angle_measure != AngleMeasure::Radian)
    {
        _num = _num.ToRadian();
    }

    res.CalcFunc(mpfr_sec, _num);
    return res;
}

Real cosec(const Real& num)
{
    Real res(num.GetBitPrecision());
    Real _num = num;

    Real::CheckUnit(_num);

    if (_num.angle_measure != AngleMeasure::Radian)
    {
        _num = _num.ToRadian();
    }

    res.CalcFunc(mpfr_csc, _num);
    return res;
}

Real arcsin(const Real& num)
{
    Real res(num.GetBitPrecision());

    if (abs(num) > 1)
        throw MathException(Overflow);

    res.CalcFunc(mpfr_asin, num);
    res.angle_measure = AngleMeasure::Radian;
    return res;
}

Real arccos(const Real& num)
{
    Real res(num.GetBitPrecision());

    if (abs(num) > 1)
        throw MathException(Overflow);

    res.CalcFunc(mpfr_acos, num);
    res.angle_measure = AngleMeasure::Radian;
    return res;
}

Real arctg(const Real& num)
{
    Real res(num.GetBitPrecision());

    res.CalcFunc(mpfr_atan, num);
    res.angle_measure = AngleMeasure::Radian;
    return res;
}

Real arcctg(const Real& num)
{
    Real _pi = pi(num.GetBitPrecision());
    return _pi / 2 - arctg(num);
}

Real arcsec(const Real& num)
{
    return arccos(1 / num);
}

Real arccosec(const Real& num)
{
    return arcsin(1 / num);
}

Real sh(const Real& num)
{
    Real res(num.GetBitPrecision());

    if (num.angle_measure != AngleMeasure::Radian)
    {
        Real _num = num.ToRadian();
        res.CalcFunc(mpfr_sinh, _num);
        return res;
    }

    res.CalcFunc(mpfr_sinh, num);

    if (res.IsInfinity() || res.IsNaN())
        throw MathException(Overflow);

    return res;
}

Real ch(const Real& num)
{
    Real res(num.GetBitPrecision());

    if (num.angle_measure != AngleMeasure::Radian)
    {
        Real _num = num.ToRadian();
        res.CalcFunc(mpfr_cosh, _num);
        return res;
    }

    res.CalcFunc(mpfr_cosh, num);

    if (res.IsInfinity() || res.IsNaN())
        throw MathException(Overflow);

    return res;
}

Real th(const Real& num)
{
    Real res(num.GetBitPrecision());

    if (num.angle_measure != AngleMeasure::Radian)
    {
        Real _num = num.ToRadian();
        res.CalcFunc(mpfr_tanh, _num);
        return res;
    }

    res.CalcFunc(mpfr_tanh, num);

    if (res.IsInfinity() || res.IsNaN())
        throw MathException(Overflow);

    return res;
}

Real cth(const Real& num)
{
    Real res(num.GetBitPrecision());

    if (num.angle_measure != AngleMeasure::Radian)
    {
        Real _num = num.ToRadian();
        res.CalcFunc(mpfr_coth, _num);
        return res;
    }

    res.CalcFunc(mpfr_coth, num);

    if (res.IsInfinity() || res.IsNaN())
        throw MathException(Overflow);

    return res;
}

Real sch(const Real& num)
{
    Real res(num.GetBitPrecision());

    if (num.angle_measure != AngleMeasure::Radian)
    {
        Real _num = num.ToRadian();
        
        mpfr_sech(res.number, _num.number, DEFAULT_RND);

        return res;
    }

    mpfr_sech(res.number, num.number, DEFAULT_RND);
    return res;
}

Real csch(const Real& num)
{
    Real res(num.GetBitPrecision());

    if (num.angle_measure != AngleMeasure::Radian)
    {
        Real _num = num.ToRadian();

        mpfr_csch(res.number, _num.number, DEFAULT_RND);
        return res;
    }

    mpfr_csch(res.number, num.number, DEFAULT_RND);
    return res;
}

Real arsh(const Real& num)
{
    Real res(num.GetBitPrecision());

    mpfr_asinh(res.number, num.number, DEFAULT_RND);

    if (res.IsInfinity() || res.IsNaN())
        throw MathException(Overflow);

    res.angle_measure = AngleMeasure::Radian;
    return res;
}

Real arch(const Real& num)
{
    Real res(num.GetBitPrecision());

    mpfr_acosh(res.number, num.number, DEFAULT_RND);

    if (res.IsInfinity() || res.IsNaN())
        throw MathException(Overflow);

    res.angle_measure = AngleMeasure::Radian;
    return res;
}

Real arth(const Real& num)
{
    Real res(num.GetBitPrecision());

    mpfr_atanh(res.number, num.number, DEFAULT_RND);

    if (res.IsInfinity() || res.IsNaN())
        throw MathException(Overflow);

    res.angle_measure = AngleMeasure::Radian;
    return res;
}

Real arcth(const Real& num)
{
    Real res(num.GetBitPrecision());

    //res = (ln((num + 1) / (num - 1))) / 2;
    res = arth(1 / num);

    res.angle_measure = AngleMeasure::Radian;
    return res;
}

Real arsch(const Real& num)
{
    Real res(num.GetBitPrecision());

    //res = ln(sqrt((1 / num) - 1) * sqrt((1 / num) + 1) + (1 / num));
    res = arch(1 / num);

    res.angle_measure = AngleMeasure::Radian;
    return res;
}

Real arcsch(const Real& num)
{
    Real res(num.GetBitPrecision());

    //res = ln(sqrt(1 + 1 / sqr(num)) + 1 / num);
    res = arsh(1 / num);

    res.angle_measure = AngleMeasure::Radian;
    return res;
}

Real pow(const Real& num1, const Real& num2)
{
    if (num1.IsZero() && num2 <= 0)
        throw MathException(Overflow);

    Real res(num1.GetBitPrecision() + num2.GetBitPrecision() + 64);

    res.CalcFunc(mpfr_pow, num1, num2);

    if (res.IsInfinity() || res.IsNaN())
        throw MathException(Overflow);

    res.Round(std::max(num1.GetBitPrecision(), num2.GetBitPrecision()));

    res.unit = pow(num1.unit, num2);

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real pow(const Real& num1, const int num2)
{
    if (num1.IsZero() && num2 <= 0)
        throw MathException(Overflow);
    
    Real res(num1.GetBitPrecision() + sizeof(int) * 8);

    res.CalcFunc(mpfr_pow_si, num1, num2);

    res.unit = pow(num1.unit, num2);

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real sqr(const Real& num)
{
    Real res(num.GetBitPrecision() * 2);

    res.CalcFunc(mpfr_sqr, num);

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real sqrt(const Real& num)
{
    Real res(num.GetBitPrecision() + 64);

    res.CalcFunc(mpfr_sqrt, num);
    res.unit = sqrt(num.unit);

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real root(const Real& num1, const Real& num2)
{
    if (num1.unit.IsEmpty())
    {
        Real res(pow(num1, 1 / num2));

#ifdef TRACE_OUTPUT
        res.UpdateNumberStr();
#endif
        return res;
    }

    Unit u = num1.unit;
    Real _num1 = num1;
    _num1.unit.unit.clear();
    Real res(pow(_num1, 1 / num2));

    res.unit = root(u, num2);

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real abs(const Real& num)
{
    Real res(num.GetBitPrecision(), num.angle_measure);

    mpfr_abs(res.number, num.number, DEFAULT_RND);

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real min(const Real& num1, const Real& num2)
{
    return num1 < num2 ? num1 : num2;
}

Real max(const Real& num1, const Real& num2)
{
    return num1 > num2 ? num1 : num2;
}

Real floor(const Real& num)
{
    Real res(num.GetBitPrecision(), num.angle_measure);

    mpfr_floor(res.number, num.number);

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real trunc(const Real& num)
{
    Real res(num.GetBitPrecision(), num.angle_measure);

    mpfr_trunc(res.number, num.number);

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real ceil(const Real& num)
{
    Real res(num.GetBitPrecision(), num.angle_measure);

    mpfr_ceil(res.number, num.number);

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real round(const Real& num)
{
    Real res(num.GetBitPrecision(), num.angle_measure);

    mpfr_round(res.number, num.number);

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real integer(const Real& num)
{
    return trunc(num);
}

Real fract(const Real &num)
{
    Real res(num.GetBitPrecision(), num.angle_measure);

    mpfr_frac(res.number, num.number, DEFAULT_RND);

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real fact(const Real& num, ParserContext* parser_context)
{
    CheckBreak(parser_context);
    Real res(num.GetBitPrecision(), num.angle_measure);

    if (!num.IsInteger())
        throw MathException(ArgumentIsOver);

    int n = (int)num;
    if (n < 0)
        throw MathException(ArgumentIsOver);

    mpfr_set_ui(res.number, 1, DEFAULT_RND);
    for (unsigned long i = 2; i <= (unsigned long)n; ++i)
    {
        CheckBreak(parser_context);
        mpfr_mul_ui(res.number, res.number, i, DEFAULT_RND);
    }

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real roundoff(const Real& num)
{
    Real res(num.GetBitPrecision(), num.angle_measure);

    mpfr_round(res.number, num.number);

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real exp(const int precision)
{
    return exp(Real(precision, 1));
}

Real pi(const int precision)
{
    Real res(precision, AngleMeasure::Radian);

    mpfr_const_pi(res.number, DEFAULT_RND);

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Real Real::ToRadian() const
{
    Real res(GetBitPrecision());

    switch (angle_measure)
    {
    case AngleMeasure::Radian:
        return *this;
    case AngleMeasure::Degree:
        res = *this;
        res.angle_measure = AngleMeasure::None;
        res *= pi(GetBitPrecision()) / 180;
        break;
    case AngleMeasure::Grad:
        res = *this;
        res.angle_measure = AngleMeasure::None;
        res *= pi(GetBitPrecision()) / 180 * (float)0.9;
        break;
    default:
        res = *this;
        break;
    }

    res.angle_measure = AngleMeasure::Radian;
    return res;
}

Real Real::ToDegree() const
{
    Real res(GetBitPrecision());

    switch (angle_measure)
    {
    case AngleMeasure::Radian:
        res = *this;
        res.angle_measure = AngleMeasure::None;
        res *= 180 / pi(GetBitPrecision());
        break;
    case AngleMeasure::Degree:
        return *this;
    case AngleMeasure::Grad:
        res = *this;
        res.angle_measure = AngleMeasure::None;
        res *= (float)0.9;
        break;
    default:
        res = *this;
        break;
    }

    res.angle_measure = AngleMeasure::Degree;
    return res;
}

Real Real::ToGrad() const
{
    Real res(GetBitPrecision());

    switch (angle_measure)
    {
    case AngleMeasure::Radian:
        res = *this;
        res.angle_measure = AngleMeasure::None;
        res *= 180 / pi(GetBitPrecision()) * (float)0.9;
        break;
    case AngleMeasure::Degree:
        res = *this / (float)0.9;
        break;
    case AngleMeasure::Grad:
        return *this;
    default:
        res = *this;
        break;
    }

    res.angle_measure = AngleMeasure::Grad;
    return res;
}

void ToCommonAngleMeasure(Real& num1, Real& num2)
{
    if (num1.angle_measure == num2.angle_measure)
        return;
    switch (num1.angle_measure)
    {
    case AngleMeasure::Radian:
        num2 = num2.ToRadian();
        break;
    case AngleMeasure::Degree:
        num2 = num2.ToDegree();
        break;
    case AngleMeasure::Grad:
        num2 = num2.ToGrad();
        break;
    case AngleMeasure::None:
    {
        switch (num2.angle_measure)
        {
        case AngleMeasure::Radian:
            num1 = num1.ToRadian();
            break;
        case AngleMeasure::Degree:
            num1 = num1.ToDegree();
            break;
        case AngleMeasure::Grad:
            num1 = num1.ToGrad();
            break;
        case AngleMeasure::None:
            break;
        }
    }
    }
}

Real rad(const Real& num)
{
    return num.ToRadian();
}

Real deg(const Real& num)
{
    return num.ToDegree();
}

Real minute(const Real& num)
{
    return num.ToDegree() / 60;
}

Real second(const Real& num)
{
    return num.ToDegree() / 3600;
}

Real grad(const Real& num)
{
    return num.ToGrad();
}

AngleMeasure Real::GetAngleMeasure()
{
    return angle_measure;
}

void Real::SetAngleMeasure(AngleMeasure _angle_measure)
{
    angle_measure = _angle_measure;
}

int Real::GetPrecision() const
{
    return MathHelper::ToDecimalPrecision(GetBitPrecision());
}

void Real::SetPrecision(int precision)
{
    if (string_number.empty())
        SetBitPrecision(std::max((int)mpfr_get_default_prec(), MathHelper::ToBitPrecision(precision + 2)));
    else
    {
        SetBitPrecision(std::max((int)mpfr_get_default_prec(), MathHelper::ToBitPrecision(precision + 2)));
        //renew the number because of not being precios getting by std::u32string
        mpfr_t tmp;
        mpfr_init2(tmp, mpfr_get_prec(number));
        if (mpfr_set_str(tmp, ToBasicString(string_number).c_str(), DEFAULT_BASE, MPFR_RNDA) == 0)
            mpfr_set(number, tmp, MPFR_RNDA);
        mpfr_clear(tmp);
    }
    //SetBitPrecision(MathHelper::ToBitPrecision(precision + addPrecision + 1));

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

void Real::RaisePrecision()
{
    SetPrecision(GetPrecision() + 1);
}

void Real::LowerPrecision()
{
    SetPrecision(GetPrecision() - 1);
}

void Real::LowerPrecision(int prec)
{
    if (GetBitPrecision() <= MathHelper::ToBitPrecision(prec))
        return;

    SetPrecision(prec);
}

std::u32string Real::ToString() const
{
    if (IsInfinity())
        return GetSign() ? U"-∞" : U"∞";
    if (IsZero())
        return U"0.";

    std::ostringstream s;
    mp_exp_t exp;

    char* c_str = mpfr_get_str(NULL, &exp, DEFAULT_BASE, 0, number, DEFAULT_RND);

    --exp;

    if (c_str == NULL)
        return U"";

    if (mpfr_number_p(number))
    {
        if (c_str[0] != '-')
            s << std::string(c_str).insert(1, 1, '.');
        else
            s << std::string(c_str).insert(2, 1, '.');
    }
    else
        s << std::string(c_str);

    mpfr_free_str(c_str);

    if (IsNumber() && !IsZero())
    {
        s << 'e';

        if (exp < 0)
            s << '-';
        else
            s << '+';

        if (std::abs(exp) < 10)
            s << '0';

        s << std::abs(exp);
    }

    return ToUtfString(s.str().c_str());
}

void Real::ToString(int exp, int accuracy, bool& mantissa_sign, std::u32string& mantissa, bool& exponent_sign, std::u32string& exponent) const
{
    char* num_str;
    mp_exp_t num_exp;
    char buf[20];

    mantissa_sign = GetSign();
    if (IsZero())
        mantissa_sign = false;
    exponent_sign = false;

    assert(accuracy >= 1);

    if (IsNaN() || IsInfinity())
        return;

    num_str = mpfr_get_str(NULL, &num_exp, 10, accuracy + 1, number, DEFAULT_RND);

    if (num_exp >= 0 && exp >= num_exp)
    {
        mpfr_free_str(num_str);
        num_str = mpfr_get_str(NULL, &num_exp, 10, num_exp + accuracy, number, DEFAULT_RND);
    }
    else if (exp > ::abs(num_exp))
    {
        mpfr_free_str(num_str);
        num_str = mpfr_get_str(NULL, &num_exp, 10, accuracy, number, DEFAULT_RND);
    }

    mantissa = ToUtfString(num_str);
    mpfr_free_str(num_str);

    if (mantissa_sign)
        mantissa.erase(0, 1);

    if (*this >= 1 || *this <= -1)
    {
        if (num_exp > exp)
        {
            mantissa.insert(1, 1, '.');

            --num_exp;
#ifdef _WIN32
            _itoa_s(num_exp, buf, 10);
#else
            sprintf(buf, "%d", (int)num_exp);
#endif
            exponent = ToUtfString(buf);
        }
        else
        {
            mantissa.insert(num_exp, 1, '.');
            exponent = U"";
        }
    }
    else
    {
        if (IsZero())
        {
            mantissa = U"0.";
            exponent = U"";
            return;
        }

        int i = ::abs(num_exp);

        if (num_exp <= 0)
        {
            while (i < (int)mantissa.size() - 1 && mantissa[i] == '0')
                ++i;

            if (i >= exp)
            {
                mantissa.insert(1, 1, '.');

                --num_exp;
                if (num_exp < 0)
                    exponent_sign = true;

                if (num_exp != 0)
                {
#ifdef _WIN32
                    _itoa_s(::abs(num_exp), buf, 10);
#else
#ifdef EMSCRIPTEN
                    sprintf(buf, "%ld", ::abs(num_exp));
#else
                    sprintf(buf, "%d", ::abs(num_exp));
#endif
#endif
                    exponent = ToUtfString(buf);
                }
                else
                    exponent = U"";
            }
            else
            {
                mantissa.insert(0, 1, '0');
                mantissa.insert(1, 1, '.');

                if (num_exp < 0)
                    mantissa.insert(2, ::abs(num_exp), '0');

                exponent = U"";
            }
        }
        else
        {
            mantissa.insert(1, 1, '.');

            --num_exp;
            if (num_exp < 0)
                exponent_sign = true;

            if (num_exp != 0)
            {
#ifdef _WIN32
                _itoa_s(::abs(num_exp), buf, 10);
#else
#ifdef EMSCRIPTEN
                sprintf(buf, "%ld", ::abs(num_exp));
#else
                sprintf(buf, "%d", ::abs(num_exp));
#endif
#endif
                exponent = ToUtfString(buf);
            }
            else
                exponent = U"";
        }
    }

    for (int i = mantissa.size() - 1; i > 0; --i)
    {
        if (mantissa[i] == '.')
            break;
        if (mantissa[i] == '0')
            mantissa.erase(i, 1);
        else
            break;
    }
}

void Real::ToString(int exp, int accuracy, bool& mantissa_sign, std::string& mantissa, bool& exponent_sign, std::string& exponent) const
{
    std::u32string m, e;
    ToString(exp, accuracy, mantissa_sign, m, exponent_sign, e);
    mantissa = ToBasicString(m);
    exponent = ToBasicString(e);
}

std::u32string Real::ToString(int exp, int accuracy, bool with_unit) const
{
    if (IsNaN())
        return U"NAN";
    if (IsInfinity())
    {
        std::u32string res = GetSign() ? U"-∞" : U"∞";
        if (with_unit)
            res += unit.ToString();
        return res;
    }
    bool mantissa_sign;
    std::u32string mantissa;
    bool exponent_sign;
    std::u32string exponent;
    
    ToString(exp, accuracy, mantissa_sign, mantissa, exponent_sign, exponent);
    
    std::u32string res;
    res += mantissa_sign ? U"-" : U"";
    res += mantissa;
    res += U"E";
    res += exponent_sign ? U"-" : U"+";
    res += exponent.empty() ? U"0" : exponent;
    if (with_unit)
        res += unit.ToString();
    
    return res;
}

std::string Real::ToStdString(int exp, int accuracy) const
{
    auto r = ToString(exp, accuracy);
    return ToBasicString(r);
}

Real Real::GetNumber()
{
    Real res(*this);

    return res;
}

void Real::SetUnit(const Unit& _unit)
{
    unit = _unit;
    
#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

void Real::CheckUnit(Real& num)
{
    if (!num.unit.IsEmpty())
    {
        if (num.unit.unit.size() != 1 || num.unit.GetPower() != 1)
            throw MathException(UnitsAreIncompatible);
    }
}

void Real::CalcFunc(int (*func)(mpfr_ptr, mpfr_srcptr, mpfr_rnd_t), const Real& num)
{
    mpfr_prec_t cur_precision = GetBitPrecision();
    SetBitPrecision(GetBitPrecision() + 64);
    func(number, num.number, DEFAULT_RND);
    mpfr_prec_round(number, cur_precision, DEFAULT_RND);
}

void Real::CalcFunc(int (*func)(mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t), const Real& num1, const Real& num2)
{
    mpfr_prec_t cur_precision = GetBitPrecision();
    SetBitPrecision(GetBitPrecision() + 64);
    func(number, num1.number, num2.number, DEFAULT_RND);
    mpfr_prec_round(number, cur_precision, DEFAULT_RND);
}

void Real::CalcFunc(int (*func)(mpfr_ptr, mpfr_srcptr, long, mpfr_rnd_t), const Real& num1, int num2)
{
    mpfr_prec_t cur_precision = GetBitPrecision();
    SetBitPrecision(GetBitPrecision() + 64);
    func(number, num1.number, num2, DEFAULT_RND);
    mpfr_prec_round(number, cur_precision, DEFAULT_RND);
}

void Real::CalcFunc(int (*func)(mpfr_ptr, long, mpfr_srcptr, mpfr_rnd_t), int num1, const Real& num2)
{
    mpfr_prec_t cur_precision = GetBitPrecision();
    SetBitPrecision(GetBitPrecision() + 64);
    func(number, num1, num2.number, DEFAULT_RND);
    mpfr_prec_round(number, cur_precision, DEFAULT_RND);
}

void Real::CalcFunc(int (*func)(mpfr_ptr, mpfr_srcptr, double, mpfr_rnd_t), const Real& num1, float num2)
{
    mpfr_prec_t cur_precision = GetBitPrecision();
    SetBitPrecision(GetBitPrecision() + 64);
    func(number, num1.number, num2, DEFAULT_RND);
    mpfr_prec_round(number, cur_precision, DEFAULT_RND);
}

void Real::CalcFunc(int (*func)(mpfr_ptr, double, mpfr_srcptr, mpfr_rnd_t), float num1, const Real& num2)
{
    mpfr_prec_t cur_precision = GetBitPrecision();
    SetBitPrecision(GetBitPrecision() + 64);
    func(number, num1, num2.number, DEFAULT_RND);
    mpfr_prec_round(number, cur_precision, DEFAULT_RND);
}

//int Real::GetPrecision() const
//{
//	return MathHelper::ToDecimalPrecision(GetBitPrecision());
//}

//void Real::SetPrecision(int precision)
//{
//	//'precision' in this case means digits count after the point, so we need to add the leading digit
//	SetBitPrecision(MathHelper::ToBitPrecision(precision + 1));

//#ifdef TRACE_OUTPUT
//	UpdateNumberStr();
//#endif
//}

//std::u32string Real::ToString() const
//{
//	char buf[100];
//	
//	gmp_sprintf(buf, "%.Fe", number);
//	int p = mpf_get_prec(number);
//	
//	return std::u32string(buf);
//	
//	//std::ostringstream s;
//	//mp_exp_t exp;

//	//char* c_str = mpf_get_str(NULL, &exp, DEFAULT_BASE, 0, number);

//	//--exp;

//	//if (c_str == NULL)
//	//	return "";

//	////if (mpf_number_p(number))
//	////{
//	////	if (c_str[0] != '-')
//	////		s << std::u32string(c_str).insert(1, 1, '.');
//	////	else
//	////		s << std::u32string(c_str).insert(2, 1, '.');
//	////}
//	////else
//	//	s << std::u32string(c_str);

//	////mpf_free_str(c_str);
//	//free_function(c_str);

//	////if (IsNumber() && !IsZero())
//	//{
//	//	s << 'e';

//	//	if (exp < 0)
//	//		s << '-';
//	//	else
//	//		s << '+';

//	//	if (std::abs(exp) < 10)
//	//		s << '0';

//	//	s << std::abs(exp);
//	//}

//	//std::u32string res = s.str();

//	//return res;
//}

#ifdef TRACE_OUTPUT
void Real::UpdateNumberStr()
{
    number_str = ToString(3, 3);
}
#endif

}
