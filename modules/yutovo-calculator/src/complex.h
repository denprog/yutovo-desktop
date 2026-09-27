/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __COMPLEX_H__
#define __COMPLEX_H__

#include "real.h"
#include <algorithm>

namespace yutovo_calculator
{

struct ParserContext;
void CheckBreak(ParserContext* parser_context);

#ifdef max
#undef max
#endif

extern Real exp(const int precision);
extern Real pi(const int precision);

extern Real rad(const Real& num);
extern Real deg(const Real& num);
extern Real minute(const Real& num);
extern Real second(const Real& num);
extern Real grad(const Real& num);

class Complex
{
public:
    Complex();
    explicit Complex(int precision);
    explicit Complex(int precision, int re);
    explicit Complex(int precision, float re);
    explicit Complex(int precision, const char* re);
    explicit Complex(int precision, int re, int im);
    explicit Complex(int precision, float re, float im);
    explicit Complex(const Real& _re);
    explicit Complex(const Real& _re, const Real& _im);
    Complex(const Complex& source);
    ~Complex();

public:
    Complex& operator=(const Complex& source);
    Complex& operator=(const int num);
    Complex& operator=(const double num);
    Complex& operator=(const std::u32string& source);

    Complex operator+();
    Complex operator-();

    friend Complex operator+(const Complex& num1, const Complex& num2);
    friend Complex operator+(const Complex& num1, const int num2);
    friend Complex operator+(const int num1, const Complex& num2);
    friend Complex operator+(const Complex& num1, const float num2);
    friend Complex operator+(const float num1, const Complex& num2);

    friend Complex operator-(const Complex& num1, const Complex& num2);
    friend Complex operator-(const Complex& num1, const int num2);
    friend Complex operator-(const int num1, const Complex& num2);
    friend Complex operator-(const Complex& num1, const float num2);
    friend Complex operator-(const float num1, const Complex& num2);

    friend Complex operator*(const Complex& num1, const Complex& num2);
    friend Complex operator*(const Complex& num1, const int num2);
    friend Complex operator*(const int num1, const Complex& num2);
    friend Complex operator*(const Complex& num1, const float num2);
    friend Complex operator*(const float num1, const Complex& num2);

    friend Complex operator/(const Complex& num1, const Complex& num2);
    friend Complex operator/(const Complex& num1, const int num2);
    friend Complex operator/(const int num1, const Complex& num2);
    friend Complex operator/(const Complex& num1, const float num2);
    friend Complex operator/(const float num1, const Complex& num2);

    void operator+=(const Complex& num);
    void operator+=(const int num);
    void operator+=(const float num);

    void operator-=(const Complex& num);
    void operator-=(const int num);
    void operator-=(const float num);

    void operator*=(const Complex& num);
    void operator*=(const int num);
    void operator*=(const float num);

    void operator/=(const Complex& num);
    void operator/=(const int num);
    void operator/=(const float num);

public:
    friend bool operator==(const Complex& num1, const Complex& num2);
    friend bool operator==(const Complex& num1, const int num2);
    friend bool operator==(const int num1, const Complex& num2);
    friend bool operator==(const Complex& num1, const float num2);
    friend bool operator==(const float num1, const Complex& num2);

    friend bool operator!=(const Complex& num1, const Complex& num2);
    friend bool operator!=(const Complex& num1, const int num2);
    friend bool operator!=(const int num1, const Complex& num2);
    friend bool operator!=(const Complex& num1, const float num2);
    friend bool operator!=(const float num1, const Complex& num2);

    friend bool operator<(const Complex& num1, const Complex& num2);
    friend bool operator<(const Complex& num1, const int num2);

    friend bool operator>(const Complex& num1, const Complex& num2);
    friend bool operator>(const Complex& num1, const int num2);

    friend bool operator<=(const Complex& num1, const Complex& num2);
    friend bool operator<=(const Complex& num1, const int num2);

    friend bool operator>=(const Complex& num1, const Complex& num2);
    friend bool operator>=(const Complex& num1, const int num2);

public:
    friend Complex exp(const Complex& num, int& res_pos);

    friend Complex ln(const Complex& num, int& res_pos);

    friend Complex lg(const Complex& num, int& res_pos);

    friend Complex log(const Complex& num1, const Complex& num2, int& res_pos);

    friend Complex pow(const Complex& num1, const int num2, int& res_pos);

    friend Complex pow(const Complex& num1, const Complex& num2, int& res_pos);

    friend Complex sqrt(const Complex& num, int& res_pos);

    friend Complex root(const Complex& num1, const Complex& num2, int& res_pos);

    friend Complex abs(const Complex& num);

    friend Complex min(const Complex& num1, const Complex& num2, int& res_pos);

    friend Complex max(const Complex& num1, const Complex& num2, int& res_pos);

    friend Complex mod(const Complex& num);

    friend Complex fact(const Complex& num, ParserContext* parser_context);

    friend Complex arg(const Complex& num);

    friend Complex re(const Complex& num);

    friend Complex im(const Complex& num);

    friend Complex sin(const Complex& num, int& res_pos);

    friend Complex cos(const Complex& num, int& res_pos);

    friend Complex tg(const Complex& num, int& res_pos);

    friend Complex ctg(const Complex& num, int& res_pos);

    friend Complex sec(const Complex& num, int& res_pos);

    friend Complex cosec(const Complex& num, int& res_pos);

    friend Complex arcsin(const Complex& num, int& res_pos);

    friend Complex arccos(const Complex& num, int& res_pos);

    friend Complex arctg(const Complex& num, int& res_pos);

    friend Complex arcctg(const Complex& num, int& res_pos);

    friend Complex arcsec(const Complex& num, int& res_pos);

    friend Complex arccosec(const Complex& num, int& res_pos);

    friend Complex sh(const Complex& num, int& res_pos);

    friend Complex ch(const Complex& num, int& res_pos);

    friend Complex th(const Complex& num, int& res_pos);

    friend Complex cth(const Complex& num, int& res_pos);

    friend Complex sch(const Complex& num, int& res_pos);

    friend Complex csch(const Complex& num, int& res_pos);

    friend Complex arsh(const Complex& num, int& res_pos);

    friend Complex arch(const Complex& num, int& res_pos);

    friend Complex arth(const Complex& num, int& res_pos);

    friend Complex arcth(const Complex& num, int& res_pos);

    friend Complex arsch(const Complex& num, int& res_pos);

    friend Complex arcsch(const Complex& num, int& res_pos);

    friend Real module(const Complex& num);

    friend Real argument(const Complex& num);

public:
    friend Complex exp_complex(const int precision, AngleMeasure angle_measure);
    friend Complex pi_complex(const int precision, AngleMeasure angle_measure);
    friend Complex j(const int precision);

public:
    Complex ToDegree() const;
    Complex ToRadian() const;
    Complex ToGrad() const;

    friend Complex rad(const Complex& num, int& res_pos);
    friend Complex deg(const Complex& num, int& res_pos);
    friend Complex minute(const Complex& num, int& res_pos);
    friend Complex second(const Complex& num, int& res_pos);
    friend Complex grad(const Complex& num, int& res_pos);

public:
    void GetPolarForm(Real& mod, Real& arg);

public:
    bool IsZero() const
    {
        return re.IsZero() && im.IsZero();
    }

    int GetBitPrecision() const
    {
        return std::max(re.GetBitPrecision(), im.GetBitPrecision());
    }

    int GetPrecision() const;
    void SetPrecision(int prec);

    void SetBitPrecision(const int precision);

    void RaisePrecision();
    void LowerPrecision();
    void LowerPrecision(int prec);

    AngleMeasure GetAngleMeasure();
    void SetAngleMeasure(AngleMeasure angle_measure);

    Real& GetRe() const
    {
        return (Real&)re;
    }

    Real& GetIm() const
    {
        return (Real&)im;
    }

    bool IsReal() const
    {
        return im == 0;
    }

    Complex& Round(int prec);

    bool IsNaN() const
    {
        return re.IsNaN() || im.IsNaN();
    }

    void SetNaN()
    {
        re.SetNaN();
        im.SetNaN();
    }

    bool IsInfinity() const
    {
        return re.IsInfinity() || im.IsInfinity();
    }

public:
    std::u32string ToString() const;
    std::string ToStdString(int exp, int accuracy) const;

    Complex GetNumber();

private:
    Real re, im;

#ifdef TRACE_OUTPUT
    void UpdateNumberStr();

    std::u32string number_str;
#endif
};

}

#endif
