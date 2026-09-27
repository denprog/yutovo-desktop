/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef RATIONAL_H
#define RATIONAL_H

#include <string>
#include <gmp.h>
#include "math_helper.h"
#include "integer.h"
#include "unit.h"

namespace yutovo_calculator
{

//Rational number
class Rational
{
public:
    Rational();
    Rational(const Rational& source);
    Rational(const int num);
    Rational(const int precision, const double num);
    Rational(const int precision, const int num);
    Rational(const int precision, Unit& _unit);
    Rational(Unit& _unit);
    Rational(const std::u32string& num);
    Rational(mpz_srcptr num, mpz_srcptr den);
    ~Rational();
    
public:
    typedef Rational value_type;

    Rational& operator=(const Rational& source);
    Rational& operator=(const std::u32string& source);
    
    Rational operator+();
    Rational operator-();

    friend Rational operator+(const Rational& num1, const Rational& num2);
    friend Rational operator+(const Rational& num1, const int num2);
    friend Rational operator+(const int num1, const Rational& num2);

    friend Rational operator-(const Rational& num1, const Rational& num2);
    friend Rational operator-(const Rational& num1, const int num2);
    friend Rational operator-(const int num1, const Rational& num2);

    friend Rational operator*(const Rational& num1, const Rational& num2);
    friend Rational operator*(const Rational& num1, const int num2);
    friend Rational operator*(const int num1, const Rational& num2);

    friend Rational operator/(const Rational& num1, const Rational& num2);
    friend Rational operator/(const Rational& num1, const int num2);
    friend Rational operator/(const int num1, const Rational& num2);

    void operator+=(const Rational& num);

    void operator-=(const Rational& num);

    void operator*=(const Rational& num);

    void operator/=(const Rational& num);

public:
    void operator=(const int num);
    void operator=(const double num);
    
    operator int() const;

public:
    friend bool operator==(const Rational& num1, const Rational& num2);
    friend bool operator==(const Rational& num1, const int num2);
    friend bool operator==(const int num1, const Rational& num2);

    friend bool operator!=(const Rational& num1, const Rational& num2);
    friend bool operator!=(const Rational& num1, const int num2);
    friend bool operator!=(const int num1, const Rational& num2);

    friend bool operator>(const Rational& num1, const Rational& num2);
    friend bool operator>(const Rational& num1, const int num2);
    friend bool operator>(const int num1, const Rational& num2);

    friend bool operator>=(const Rational& num1, const Rational& num2);
    friend bool operator>=(const Rational& num1, const int num2);
    friend bool operator>=(const int num1, const Rational& num2);

    friend bool operator<(const Rational& num1, const Rational& num2);
    friend bool operator<(const Rational& num1, const int num2);
    friend bool operator<(const int num1, const Rational& num2);

    friend bool operator<=(const Rational& num1, const Rational& num2);
    friend bool operator<=(const Rational& num1, const int num2);
    friend bool operator<=(const int num1, const Rational& num2);

public:
    friend Rational pow(const Rational& num1, const Rational& num2);
    friend Rational pow(const Rational& num1, const int num2);

    friend Rational abs(const Rational &num);

    friend Rational min(const Rational& num1, const Rational& num2);
    friend Rational max(const Rational& num1, const Rational& num2);

public:
    Integer GetNumerator() const;
    Integer GetDenomerator() const;

    std::u32string ToString(bool with_unit = true) const;
    std::u32string ToString(int, int, bool with_unit = true) const;

    int GetExp() const
    {
        return 0;
    }

    int GetPrecision() const
    {
        return 0;
    }

    void SetPrecision(int precision)
    {
    }

    void SetBitPrecision(const int precision)
    {
    }

    std::string ToStdString() const;

    mpq_srcptr GetNumber() const
    {
        return number;
    }

    void ToProper(Integer& integer, Integer& numerator, Integer& denomerator) const;

    void SetUnit(const Unit& _unit);

    bool IsZero() const
    {
        return mpq_sgn(number) == 0;
    }

public:
    Unit unit;

private:
#ifdef TRACE_OUTPUT
    void UpdateNumberStr();
#endif

private:
    mpq_t number;

#ifdef TRACE_OUTPUT
    std::u32string number_str; //the std::u32string representation of the number for debug purposes
#endif
};

}

#endif
