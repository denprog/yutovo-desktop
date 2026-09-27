/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "stdafx.h"
#include "rational.h"
#include "utils.h"

namespace yutovo_calculator
{

//Rational

Rational::Rational()
{
    mpq_init(number);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Rational::Rational(const Rational& source)
{
    mpq_init(number);
    mpq_set(number, source.number);
    unit = source.unit;

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Rational::Rational(const int num)
{
    mpq_init(number);
    mpq_set_si(number, num, 1);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Rational::Rational(const int precision, const double num)
{
    mpq_init(number);
    mpq_set_d(number, num);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Rational::Rational(const int precision, const int num)
{
    mpq_init(number);
    mpq_set_si(number, num, 1);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Rational::Rational(const int precision, Unit& _unit) :
    unit(_unit)
{
    mpq_init(number);
    mpq_set_si(number, 1, 1);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Rational::Rational(Unit& _unit)
{
    mpq_init(number);
    mpq_set_si(number, 1, 1);
    unit = _unit;

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Rational::Rational(const std::u32string& num)
{
    mpq_init(number);
    *this = num;

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Rational::Rational(mpz_srcptr num, mpz_srcptr den)
{
    mpq_init(number);
    mpq_set_num(number, num);
    mpq_set_den(number, den);
    mpq_canonicalize(number);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Rational::~Rational()
{
    mpq_clear(number);
}
    
Rational& Rational::operator=(const Rational& source)
{
    if (this == &source)
        return *this;

    mpq_clear(number);
    mpq_init(number);

    mpq_set(number, source.number);

    unit = source.unit;

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif

    return *this;
}

Rational& Rational::operator=(const std::u32string& source)
{
    if (source.empty())
    {
        mpq_set_si(number, 0, 1);
        return *this;
    }

    if (source.find(L'.') != -1)
    {
        std::u32string int_part, fract_part;
        int i = 0;

        while (i < (int)source.length() && source[i] != '.')
        {
            int_part += source[i];
            ++i;
        }

        ++i;

        while (i < (int)source.length())
        {
            fract_part += source[i];
            ++i;
        }

        if (fract_part != U"")
        {
            int n = fract_part.length();
            std::u32string t(U"1");

            for (int j = 0; j < n; ++j)
                t += '0';

            std::u32string combined;
            if (!int_part.empty() && int_part[0] == U'-')
                combined = U"-" + int_part.substr(1) + fract_part;
            else
                combined = int_part + fract_part;
            *this = Rational(combined) / Rational(t);

            return *this;
        }
        else if (!int_part.empty())
        {
            *this = Rational(int_part);
            return *this;
        }
    }

    if (mpq_set_str(number, ToBasicString(source).c_str(), DEFAULT_BASE) != 0)
        throw SyntaxException(SyntaxError);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
    return *this;
}

Rational Rational::operator+()
{
    Rational res(*this);

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Rational Rational::operator-()
{
    Rational res(*this);

    mpq_neg(res.number, number);

    res.unit = unit;

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Rational operator+(const Rational& num1, const Rational& num2)
{
    Rational res;

    mpq_add(res.number, num1.number, num2.number);

    res.unit = num1.unit + num2.unit;

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Rational operator+(const Rational& num1, const int num2)
{
    Rational res;
    Rational _num2(num2);

    mpq_add(res.number, num1.number, _num2.number);

    res.unit = num1.unit + num2;

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Rational operator+(const int num1, const Rational& num2)
{
    Rational res;
    Rational _num1(num1);

    mpq_add(res.number, _num1.number, num2.number);

    res.unit = num1 + num2.unit;

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Rational operator-(const Rational& num1, const Rational& num2)
{
    Rational res;

    mpq_sub(res.number, num1.number, num2.number);

    res.unit = num1.unit - num2.unit;

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Rational operator-(const Rational& num1, const int num2)
{
    Rational res;
    Rational _num2(num2);

    mpq_sub(res.number, num1.number, _num2.number);

    res.unit = num1.unit - num2;

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Rational operator-(const int num1, const Rational& num2)
{
    Rational res;
    Rational _num1(num1);

    mpq_sub(res.number, _num1.number, num2.number);

    res.unit = num1 - num2.unit;

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Rational operator*(const Rational& num1, const Rational& num2)
{
    Rational res;

    mpq_mul(res.number, num1.number, num2.number);

    res.unit = num1.unit * num2.unit;

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Rational operator*(const Rational& num1, const int num2)
{
    Rational res;
    Rational _num2(num2);

    mpq_mul(res.number, num1.number, _num2.number);

    res.unit = num1.unit * num2;

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Rational operator*(const int num1, const Rational& num2)
{
    Rational res;
    Rational _num1(num1);

    mpq_mul(res.number, _num1.number, num2.number);

    res.unit = num1 * num2.unit;

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Rational operator/(const Rational& num1, const Rational& num2)
{
    Rational res;

    if (num2 == 0)
        throw MathException(DivisionByZero);

    mpq_div(res.number, num1.number, num2.number);

    res.unit = num1.unit / num2.unit;

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Rational operator/(const Rational& num1, const int num2)
{
    Rational res;
    Rational _num2(num2);

    if (num2 == 0)
        throw MathException(DivisionByZero);

    mpq_div(res.number, num1.number, _num2.number);

    res.unit = num1.unit / num2;

#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Rational operator/(const int num1, const Rational& num2)
{
    Rational res;
    Rational _num1(num1);

    if (num2 == 0)
        throw MathException(DivisionByZero);

    mpq_div(res.number, _num1.number, num2.number);

    res.unit = num1 / num2.unit;

    return res;
}

void Rational::operator+=(const Rational& num)
{
    *this = *this + num;
}

void Rational::operator-=(const Rational& num)
{
    *this = *this - num;
}

void Rational::operator*=(const Rational& num)
{
    *this = *this * num;
}

void Rational::operator/=(const Rational& num)
{
    *this = *this / num;
}

void Rational::operator=(const int num)
{
    mpq_set_si(number, num, 1);
#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

void Rational::operator=(const double num)
{
    mpq_set_d(number, num);
#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Rational::operator int() const
{
    mpz_t num;
    mpz_init(num);
    mpq_get_num(num, number);
    if (mpz_fits_slong_p(num) == 0)
        throw MathException(ConversionDoesNotFit);
    int res = (int)mpz_get_si(num);
    mpz_clear(num);
    return res;
}

bool operator==(const Rational& num1, const Rational& num2)
{
    return mpq_cmp(num1.number, num2.number) == 0;
}

bool operator==(const Rational& num1, const int num2)
{
    return mpq_cmp_si(num1.number, num2, 1) == 0;
}

bool operator==(const int num1, const Rational& num2)
{
    return mpq_cmp_si(num2.number, num1, 1) == 0;
}

bool operator!=(const Rational& num1, const Rational& num2)
{
    return mpq_cmp(num1.number, num2.number) != 0;
}

bool operator!=(const Rational& num1, const int num2)
{
    return mpq_cmp_si(num1.number, num2, 1) != 0;
}

bool operator!=(const int num1, const Rational& num2)
{
    return mpq_cmp_si(num2.number, num1, 1) != 0;
}

bool operator>(const Rational& num1, const Rational& num2)
{
    return mpq_cmp(num1.number, num2.number) > 0;
}

bool operator>(const Rational& num1, const int num2)
{
    return mpq_cmp_si(num1.number, num2, 1) > 0;
}

bool operator>(const int num1, const Rational& num2)
{
    return mpq_cmp_si(num2.number, num1, 1) < 0;
}

bool operator>=(const Rational& num1, const Rational& num2)
{
    return mpq_cmp(num1.number, num2.number) >= 0;
}

bool operator>=(const Rational& num1, const int num2)
{
    return mpq_cmp_si(num1.number, num2, 1) >= 0;
}

bool operator>=(const int num1, const Rational& num2)
{
    return mpq_cmp_si(num2.number, num1, 1) <= 0;
}

bool operator<(const Rational& num1, const Rational& num2)
{
    return mpq_cmp(num1.number, num2.number) < 0;
}

bool operator<(const Rational& num1, const int num2)
{
    return mpq_cmp_si(num1.number, num2, 1) < 0;
}

bool operator<(const int num1, const Rational& num2)
{
    return mpq_cmp_si(num2.number, num1, 1) > 0;
}

bool operator<=(const Rational& num1, const Rational& num2)
{
    return mpq_cmp(num1.number, num2.number) <= 0;
}

bool operator<=(const Rational& num1, const int num2)
{
    return mpq_cmp_si(num1.number, num2, 1) <= 0;
}

bool operator<=(const int num1, const Rational& num2)
{
    return mpq_cmp_si(num2.number, num1, 1) >= 0;
}

Rational pow(const Rational& num1, const Rational& num2)
{
    if (num2.GetDenomerator().IsZero() || (num2.GetNumerator() % num2.GetDenomerator() != 0))
        throw MathException(ArgumentIsOver);
    if (!num2.unit.IsEmpty())
        throw MathException(UnitsAreIncompatible);
    Integer p = num2.GetNumerator() / num2.GetDenomerator();
    if (abs(p) >= INT_MAX)
        throw MathException(ParserExceptionCode::Overflow);
    return pow(num1, (int)p);
}

Rational pow(const Rational& num1, const int num2)
{
    if (num1.IsZero() && num2 <= 0)
        throw MathException(Overflow);
    Rational res;
    mpz_t num, den;
    mpz_init(num);
    mpz_init(den);
    unsigned long int exp;
    if (num2 > 0)
    {
        exp = static_cast<unsigned long int>(num2);
        mpz_pow_ui(num, mpq_numref(num1.number), exp);
        mpz_pow_ui(den, mpq_denref(num1.number), exp);
    }
    else
    {
        mpq_t tmp;
        mpq_init(tmp);
        mpq_inv(tmp, num1.number);
        exp = static_cast<unsigned long int>(-(static_cast<long long>(num2)));
        mpz_pow_ui(num, mpq_numref(tmp), exp);
        mpz_pow_ui(den, mpq_denref(tmp), exp);
        mpq_clear(tmp);
    }
    mpq_set_num(res.number, num);
    mpq_set_den(res.number, den);
    mpq_canonicalize(res.number);
    mpz_clear(num);
    mpz_clear(den);
    res.unit = pow(num1.unit, num2);
#ifdef TRACE_OUTPUT
    res.UpdateNumberStr();
#endif
    return res;
}

Rational abs(const Rational &num)
{
    if (num < 0)
    {
        Rational r(num);
        return -r;
    }
    return num;
}

Rational min(const Rational& num1, const Rational& num2)
{
    return num1 < num2 ? num1 : num2;
}

Rational max(const Rational& num1, const Rational& num2)
{
    return num1 > num2 ? num1 : num2;
}

Integer Rational::GetNumerator() const
{
    Integer numerator;
    mpz_t numer;
    mpz_init(numer);

    mpq_get_num(numer, number);
    numerator = numer;
    mpz_clear(numer);
    return numerator;
}

Integer Rational::GetDenomerator() const
{
    Integer denomerator;
    mpz_t denom;
    mpz_init(denom);

    mpq_get_den(denom, number);
    denomerator = denom;
    mpz_clear(denom);
    return denomerator;
}

std::u32string Rational::ToString(bool with_unit) const
{
    char* tmp = (char*)malloc(mpz_sizeinbase(mpq_numref(number), 10) + 
        mpz_sizeinbase(mpq_denref(number), 10) + 3);

    char* str = mpq_get_str(tmp, 10, number);
    std::u32string res(ToUtfString(str));
    free(str);

    if (with_unit)
        res += unit.ToString();

    return res;
}

std::u32string Rational::ToString(int, int, bool with_unit) const
{
    return ToString(with_unit);
}

std::string Rational::ToStdString() const
{
    return ToBasicString(ToString());
}

void Rational::ToProper(Integer& integer, Integer& numerator, Integer& denomerator) const
{
    auto n = GetNumerator();
    auto d = GetDenomerator();
    auto i = n / d;
    if (n < 0)
    {
        if (d < 0)
            n = (-n) % (-d);
        else
        {
            n = (-n) % d;
            if (i == 0)
                n = -n;
        }
    }
    else if (d < 0)
    {
        n = n % (-d);
    }
    else
        n = n % d;

    integer = i;
    numerator = n;
    denomerator = d;
}

void Rational::SetUnit(const Unit& _unit)
{
    unit = _unit;
    
#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

#ifdef TRACE_OUTPUT
void Rational::UpdateNumberStr()
{
    number_str = ToString();
}
#endif

}
