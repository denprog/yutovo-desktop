/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef REAL_H
#define REAL_H

#include <string>
#include <mpfr.h>
#include <cmath>
#include "math_helper.h"
#include "parser_exception.h"
#include "unit.h"

namespace yutovo_calculator
{

struct ParserContext;
void CheckBreak(ParserContext* parser_context);

template<typename Number> class Symbolic;

class Complex;

class Real
{
public:
    Real();
    explicit Real(int precision);
    explicit Real(int precision, Unit& _unit);
    explicit Real(int precision, AngleMeasure _angle_measure);
    explicit Real(int precision, const char* num);
    explicit Real(int precision, int num);
    explicit Real(int precision, float num);
    explicit Real(mpfr_srcptr _number);
    Real(const std::u32string& num);
    Real(const Real& source);
    ~Real();

public:
    typedef Real value_type;
    
    Real& operator=(const Real& num);
    Real& operator=(const int num);
    Real& operator=(const double num);
    Real& operator=(const char* source);
    Real& operator=(const std::u32string& num);

    Real operator+();
    Real operator-();

    Real operator++();
    Real operator--();

    friend Real operator+(const Real& num1, const Real& num2);
    friend Real operator+(const Real& num1, const int num2);
    friend Real operator+(const int num1, const Real& num2);
    friend Real operator+(const Real& num1, const float num2);
    friend Real operator+(const float num1, const Real& num2);

    friend Real operator-(const Real& num1, const Real& num2);
    friend Real operator-(const Real& num1, const int num2);
    friend Real operator-(const int num1, const Real& num2);
    friend Real operator-(const Real& num1, const float num2);
    friend Real operator-(const float num1, const Real& num2);

    friend Real operator*(const Real& num1, const Real& num2);
    friend Real operator*(const Real& num1, const int num2);
    friend Real operator*(const int num1, const Real& num2);
    friend Real operator*(const Real& num1, const float num2);
    friend Real operator*(const float num1, const Real& num2);

    friend Real operator/(const Real& num1, const Real& num2);
    friend Real operator/(const Real& num1, const int num2);
    friend Real operator/(const int num1, const Real& num2);
    friend Real operator/(const Real& num1, const float num2);
    friend Real operator/(const float num1, const Real& num2);

    void operator+=(const Real& num);
    void operator+=(const int num);
    void operator+=(const float num);

    void operator-=(const Real& num);
    void operator-=(const int num);
    void operator-=(const float num);

    void operator*=(const Real& num);
    void operator*=(const int num);
    void operator*=(const float num);

    void operator/=(const Real& num);
    void operator/=(const int num);
    void operator/=(const float num);

public:
    operator int() const;

public:
    friend bool operator==(const Real& num1, const Real& num2);
    friend bool operator==(const Real& num1, const int num2);
    friend bool operator==(const int num1, const Real& num2);
    friend bool operator==(const Real& num1, const float num2);
    friend bool operator==(const float num1, const Real& num2);
    friend bool operator==(const Real& num1, const double num2);
    friend bool operator==(const double num1, const Real& num2);
    friend bool operator==(const Real& num1, const char* num2);
    friend bool operator==(const char* num1, const Real& num2);

    friend bool operator!=(const Real& num1, const Real& num2);
    friend bool operator!=(const Real& num1, const int num2);
    friend bool operator!=(const int num1, const Real& num2);
    friend bool operator!=(const Real& num1, const float num2);
    friend bool operator!=(const float num1, const Real& num2);

    friend bool operator>(const Real& num1, const Real& num2);
    friend bool operator>(const Real& num1, const int num2);
    friend bool operator>(const int num1, const Real& num2);
    friend bool operator>(const Real& num1, const float num2);
    friend bool operator>(const float num1, const Real& num2);

    friend bool operator>=(const Real& num1, const Real& num2);
    friend bool operator>=(const Real& num1, const int num2);
    friend bool operator>=(const int num1, const Real& num2);
    friend bool operator>=(const Real& num1, const float num2);
    friend bool operator>=(const float num1, const Real& num2);

    friend bool operator<(const Real& num1, const Real& num2);
    friend bool operator<(const Real& num1, const int num2);
    friend bool operator<(const int num1, const Real& num2);
    friend bool operator<(const Real& num1, const float num2);
    friend bool operator<(const float num1, const Real& num2);

    friend bool operator<=(const Real& num1, const Real& num2);
    friend bool operator<=(const Real& num1, const int num2);
    friend bool operator<=(const int num1, const Real& num2);
    friend bool operator<=(const Real& num1, const float num2);
    friend bool operator<=(const float num1, const Real& num2);

public:
    friend Real exp(const Real& num);

    friend Real ln(const Real& num);
    friend Real lg(const Real& num);
    friend Real log(const Real& num1, const Real& num2);

    friend Real sin(const Real& num);

    friend Real cos(const Real& num);

    friend Real tg(const Real& num);

    friend Real ctg(const Real& num);

    friend Real sec(const Real& num);

    friend Real cosec(const Real& num);

    friend Real arcsin(const Real& num);

    friend Real arccos(const Real& num);

    friend Real arctg(const Real& num);

    friend Real arcctg(const Real& num);

    friend Real arcsec(const Real& num);

    friend Real arccosec(const Real& num);

    friend Real sh(const Real& num);

    friend Real ch(const Real& num);

    friend Real th(const Real& num);

    friend Real cth(const Real& num);

    friend Real sch(const Real& num);

    friend Real csch(const Real& num);

    friend Real arsh(const Real& num);

    friend Real arch(const Real& num);

    friend Real arth(const Real& num);

    friend Real arcth(const Real& num);

    friend Real arsch(const Real& num);

    friend Real arcsch(const Real& num);

    friend Real pow(const Real& num1, const Real& num2);

    friend Real pow(const Real& num1, const int num2);

    //friend Real pow(const Real& num1, const int num2, ParserExpression* pExpression);

    friend Real sqr(const Real& num);

    friend Real sqrt(const Real& num);

    friend Real root(const Real& num1, const Real& num2);

    friend Real abs(const Real& num);

    friend Real min(const Real& num1, const Real& num2);

    friend Real max(const Real& num1, const Real& num2);

    friend Real floor(const Real& num);

    friend Real trunc(const Real& num);

    friend Real ceil(const Real& num);

    friend Real round(const Real& num);

    friend Real integer(const Real& num);

    friend Real fract(const Real &num);

    friend Real fact(const Real &num, ParserContext* parser_context);

    friend Real roundoff(const Real& num);

public:
    friend Real exp(const int precision);
    friend Real pi(const int precision);

public:
    Real ToRadian() const;
    Real ToDegree() const;
    Real ToGrad() const;

    friend void ToCommonAngleMeasure(Real& num1, Real& num2);

    friend Real rad(const Real& num);
    friend Real deg(const Real& num);
    friend Real minute(const Real& num);
    friend Real second(const Real& num);
    friend Real grad(const Real& num);

public:
    bool IsNumber() const
    {
        return mpfr_number_p(number) != 0;
    }

    bool IsZero() const
    {
        return mpfr_zero_p(number) != 0;
    }

    bool IsInfinity() const
    {
        return mpfr_inf_p(number) != 0;
    }

    bool IsInteger() const
    {
        return mpfr_integer_p(number) != 0;
    }

    bool IsNaN() const
    {
        return mpfr_nan_p(number) != 0;
    }

    void SetNaN()
    {
        mpfr_set_nan(number);
    }
    
    bool GetSign() const
    {
        return mpfr_signbit(number);
    }

    int GetExp() const
    {
        if (IsZero() || IsNaN() || IsInfinity())
            return 0;
        return static_cast<int>(std::floor(mpfr_get_exp(number) * 0.3010299956639812));
    }

    AngleMeasure GetAngleMeasure();
    void SetAngleMeasure(AngleMeasure _angle_measure);

    int GetBitPrecision() const
    {
        return mpfr_get_prec(number);
    }

    mpfr_srcptr GetNumber() const
    {
        return number;
    }

    void SetBitPrecision(const int precision)
    {
        //mpfr_prec_round(number, std::max((int)mpfr_get_default_prec(), precision), DEFAULT_RND);
        mpfr_prec_round(number, precision, DEFAULT_RND);

#ifdef TRACE_OUTPUT
        UpdateNumberStr();
#endif
    }

    int GetPrecision() const;
    void SetPrecision(int precision);

    void RaisePrecision();
    void LowerPrecision();
    void LowerPrecision(int prec);

    Real& Round(int prec)
    {
        mpfr_prec_round(number, prec, GMP_RNDN);

#ifdef TRACE_OUTPUT
        UpdateNumberStr();
#endif
        return *this;
    }

public:
    std::u32string ToString() const;
    void ToString(int exp, int accuracy, bool& mantissa_sign, std::u32string& mantissa, bool& exponent_sign, std::u32string& exponent) const;
    void ToString(int exp, int accuracy, bool& mantissa_sign, std::string& mantissa, bool& exponent_sign, std::string& exponent) const;
    std::u32string ToString(int exp, int accuracy, bool with_unit = true) const;
    std::string ToStdString(int exp, int accuracy) const;

    Real GetNumber();

    void SetUnit(const Unit& _unit);

private:
    static void CheckUnit(Real& num);

    void CalcFunc(int (*func)(mpfr_ptr, mpfr_srcptr, mpfr_rnd_t), const Real& num);
    void CalcFunc(int (*func)(mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t), const Real& num1, const Real& num2);
    void CalcFunc(int (*func)(mpfr_ptr, mpfr_srcptr, long, mpfr_rnd_t), const Real& num1, int num2);
    void CalcFunc(int (*func)(mpfr_ptr, long, mpfr_srcptr, mpfr_rnd_t), int num1, const Real& num2);
    void CalcFunc(int (*func)(mpfr_ptr, mpfr_srcptr, double, mpfr_rnd_t), const Real& num1, float num2);
    void CalcFunc(int (*func)(mpfr_ptr, double, mpfr_srcptr, mpfr_rnd_t), float num1, const Real& num2);

public:
    AngleMeasure angle_measure = AngleMeasure::None;
    Unit unit;

private:
    friend class Symbolic<Real>;
    friend class Symbolic<Complex>;

    mpfr_t number;
    std::u32string string_number;

#ifdef TRACE_OUTPUT
    void UpdateNumberStr();

    std::u32string number_str;
#endif
};

}

#endif
