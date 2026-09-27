/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef INTEGER_H
#define INTEGER_H

#include <string>
#include "math_helper.h"
#include "parser_exception.h"

namespace yutovo_calculator
{

struct ParserContext;
void CheckBreak(ParserContext* parser_context);

//Integer number
class Integer
{
public:
    Integer();
    Integer(const int precision, const int num);
    Integer(const int num);
    Integer(const int precision, const double num);
    Integer(const Integer& source);
    Integer(const std::u32string& num, const int base);
    ~Integer();

public:
    typedef Integer value_type;
    
    Integer& operator=(const Integer& source);
    Integer& operator=(const std::u32string& num);
    Integer& operator=(const mpz_t& source);
    
    Integer operator+();
    Integer operator-();
    
    friend Integer operator+(const Integer& num1, const Integer& num2);
    friend Integer operator+(const Integer& num1, const int num2);
    friend Integer operator+(const int num1, const Integer& num2);

    friend Integer operator-(const Integer& num1, const Integer& num2);
    friend Integer operator-(const Integer& num1, const int num2);
    friend Integer operator-(const int num1, const Integer& num2);

    friend Integer operator*(const Integer& num1, const Integer& num2);
    friend Integer operator*(const Integer& num1, const int num2);
    friend Integer operator*(const int num1, const Integer& num2);

    friend Integer operator/(const Integer& num1, const Integer& num2);
    friend Integer operator/(const Integer& num1, const int num2);
    friend Integer operator/(const int num1, const Integer& num2);

    friend Integer operator%(const Integer& num1, const Integer& num2);
    friend Integer operator%(const Integer& num1, const int num2);
    friend Integer operator%(const int num1, const Integer& num2);

    friend Integer operator&(const Integer& num1, const Integer& num2);

    friend Integer operator|(const Integer& num1, const Integer& num2);

    friend Integer operator^(const Integer& num1, const Integer& num2);

    friend Integer operator!(const Integer& num);
    
public:
    void operator+=(const Integer& num);

    void operator-=(const Integer& num);

    void operator*=(const Integer& num);

    void operator/=(const Integer& num);

    void operator%=(const Integer& num);

public:
    void operator=(const int num);
    
    operator int() const;

public:
    friend bool operator==(const Integer& num1, const Integer& num2);
    friend bool operator==(const Integer& num1, const int num2);
    friend bool operator==(const int num1, const Integer& num2);

    friend bool operator!=(const Integer& num1, const Integer& num2);
    friend bool operator!=(const Integer& num1, const int num2);
    friend bool operator!=(const int num1, const Integer& num2);

    friend bool operator>(const Integer& num1, const Integer& num2);
    friend bool operator>(const Integer& num1, const int num2);
    friend bool operator>(const int num1, const Integer& num2);

    friend bool operator>=(const Integer& num1, const Integer& num2);
    friend bool operator>=(const Integer& num1, const int num2);
    friend bool operator>=(const int num1, const Integer& num2);

    friend bool operator<(const Integer& num1, const Integer& num2);
    friend bool operator<(const Integer& num1, const int num2);
    friend bool operator<(const int num1, const Integer& num2);

    friend bool operator<=(const Integer& num1, const Integer& num2);
    friend bool operator<=(const Integer& num1, const int num2);
    friend bool operator<=(const int num1, const Integer& num2);

public:
    friend Integer pow(const Integer& num1, const Integer& num2);

    friend Integer abs(const Integer& num);

    friend Integer min(const Integer& num1, const Integer& num2);

    friend Integer max(const Integer& num1, const Integer& num2);

    friend Integer fact(const Integer& num, ParserContext* parser_context);

    friend Integer reminder(const Integer& num1, const Integer& num2);

    friend Integer bin(const std::u32string& str);
    friend Integer oct(const std::u32string& str);
    friend Integer dec(const std::u32string& str);
    friend Integer hex(const std::u32string& str);

public:
    int GetPrecision() const;
    void SetPrecision(int precision);

public:
    bool IsNan();
    std::u32string ToString(const int base) const;
    static Integer FromString(const std::u32string& str, const int base = 10);
    std::u32string ToString(int exp, int accuracy) const;
    std::string ToStdString(const int base = 10) const;

    bool IsZero() const
    {
        return mpz_sgn(number) == 0;
    }

private:
#ifdef TRACE_OUTPUT
    void UpdateNumberStr();
#endif
    
private:
    mpz_t number;

#ifdef TRACE_OUTPUT
    std::u32string number_str;
#endif
};

}

#endif
