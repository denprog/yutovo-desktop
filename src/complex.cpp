/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "complex.h"
#include "utils.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace yutovo_calculator
{

//Complex

Complex::Complex()
{
#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Complex::Complex(int precision) : re(precision), im(precision)
{
#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Complex::Complex(int precision, int re) : re(precision, re), im(precision)
{
#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Complex::Complex(int precision, float re) : re(precision, re), im(precision)
{
#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Complex::Complex(int precision, const char* re) : re(precision, re), im(precision, 0)
{
#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Complex::Complex(int precision, int re, int im) : re(precision, re), im(precision, im)
{
#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Complex::Complex(int precision, float re, float im) : re(precision, re), im(precision, im)
{
#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Complex::Complex(const Real& _re) : re(_re), im(_re.GetBitPrecision(), 0)
{
#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Complex::Complex(const Real& _re, const Real& _im) : re(_re), im(_im)
{
#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Complex::Complex(const Complex& source) : 
    re(source.re),
    im(source.im)
{
#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

Complex::~Complex()
{
}

Complex& Complex::operator=(const Complex& source)
{
    if (this == &source)
        return *this;

    re = source.re;
    im = source.im;

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif

    return *this;
}

Complex& Complex::operator=(const int num)
{
    re = num;
    im = 0;

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif

    return *this;
}

Complex& Complex::operator=(const double num)
{
    re = num;
    im = 0;

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif

    return *this;
}

Complex& Complex::operator=(const std::u32string& source)
{
    re = source;
    im = 0;

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif

    return *this;
}

Complex Complex::operator+()
{
    Complex res(*this);

    return res;
}

Complex Complex::operator-()
{
    Complex res(*this);

    res.re = -res.re;
    res.im = -res.im;

    return res;
}

Complex operator+(const Complex& num1, const Complex& num2)
{
    Complex res(std::max(num1.GetBitPrecision(), num2.GetBitPrecision()));

    res.re = num1.re + num2.re;
    res.im = num1.im + num2.im;

    return res;
}

Complex operator+(const Complex& num1, const int num2)
{
    Complex _num2(num1.GetBitPrecision(), num2);

    return num1 + _num2;
}

Complex operator+(const int num1, const Complex& num2)
{
    Complex _num1(num2.GetBitPrecision(), num1);

    return _num1 + num2;
}

Complex operator+(const Complex& num1, const float num2)
{
    Complex _num2(num1.GetBitPrecision(), num2);

    return num1 + _num2;
}

Complex operator+(const float num1, const Complex& num2)
{
    Complex _num1(num2.GetBitPrecision(), num1);

    return _num1 + num2;
}

Complex operator-(const Complex& num1, const Complex& num2)
{
    Complex res(std::max(num1.GetBitPrecision() + 2, num2.GetBitPrecision() + 2));

    res.re = num1.re - num2.re;
    res.im = num1.im - num2.im;

    return res;
}

Complex operator-(const Complex& num1, const int num2)
{
    Complex _num2(num1.GetBitPrecision() + 2, num2);

    return num1 - _num2;
}

Complex operator-(const int num1, const Complex& num2)
{
    Complex _num1(num2.GetBitPrecision() + 2, num1);

    return _num1 - num2;
}

Complex operator-(const Complex& num1, const float num2)
{
    Complex _num2(num1.GetBitPrecision() + 2, num2);

    return num1 - _num2;
}

Complex operator-(const float num1, const Complex& num2)
{
    Complex _num1(num2.GetBitPrecision(), num1);

    return _num1 - num2;
}

Complex operator*(const Complex& num1, const Complex& num2)
{
    Complex res(std::max(num1.GetBitPrecision(), num2.GetBitPrecision()));

    res.re = num1.re * num2.re - num1.im * num2.im;
    res.im = num1.re * num2.im + num1.im * num2.re;

    return res;
}

Complex operator*(const Complex& num1, const int num2)
{
    Complex _num2(num1.GetBitPrecision(), num2);

    return num1 * _num2;
}

Complex operator*(const int num1, const Complex& num2)
{
    Complex _num1(num2.GetBitPrecision(), num1);

    return _num1 * num2;
}

Complex operator*(const Complex& num1, const float num2)
{
    Complex _num2(num1.GetBitPrecision(), num2);

    return num1 * _num2;
}

Complex operator*(const float num1, const Complex& num2)
{
    Complex _num1(num2.GetBitPrecision(), num1);

    return _num1 * num2;
}

Complex operator/(const Complex& num1, const Complex& num2)
{
    Complex res(std::max(num1.GetBitPrecision(), num2.GetBitPrecision()));

    Real r = num1.re * num2.re + num1.im * num2.im;
    Real n = pow(num2.re, 2) + pow(num2.im, 2);

    res.re = r / n;
    res.im = (num1.im * num2.re - num1.re * num2.im) / n;

    return res;
}

Complex operator/(const Complex& num1, const int num2)
{
    Complex _num2(num1.GetBitPrecision(), num2);

    return num1 / _num2;
}

Complex operator/(const int num1, const Complex& num2)
{
    Complex _num1(num2.GetBitPrecision(), num1);

    return _num1 / num2;
}

Complex operator/(const Complex& num1, const float num2)
{
    Complex _num2(num1.GetBitPrecision(), num2);

    return num1 / _num2;
}

Complex operator/(const float num1, const Complex& num2)
{
    Complex _num1(num2.GetBitPrecision(), num1);

    return _num1 / num2;
}

void Complex::operator+=(const Complex& num)
{
    *this = *this + num;
}

void Complex::operator+=(const int num)
{
    *this = *this + num;
}

void Complex::operator+=(const float num)
{
    *this = *this + num;
}

void Complex::operator-=(const Complex& num)
{
    *this = *this - num;
}

void Complex::operator-=(const int num)
{
    *this = *this - num;
}

void Complex::operator-=(const float num)
{
    *this = *this - num;
}

void Complex::operator*=(const Complex& num)
{
    *this = *this * num;
}

void Complex::operator*=(const int num)
{
    *this = *this * num;
}

void Complex::operator*=(const float num)
{
    *this = *this * num;
}

void Complex::operator/=(const Complex& num)
{
    *this = *this / num;
}

void Complex::operator/=(const int num)
{
    *this = *this / num;
}

void Complex::operator/=(const float num)
{
    *this = *this / num;
}

bool operator==(const Complex& num1, const Complex& num2)
{
    return (num1.re == num2.re) && (num1.im == num2.im);
}

bool operator==(const Complex& num1, const int num2)
{
    Complex _num2(num1.GetBitPrecision(), num2);

    return (num1.re == _num2.re) && (num1.im == _num2.im);
}

bool operator==(const int num1, const Complex& num2)
{
    Complex _num1(num2.GetBitPrecision(), num1);

    return (_num1.re == num2.re) && (_num1.im == num2.im);
}

bool operator==(const Complex& num1, const float num2)
{
    Complex _num2(num1.GetBitPrecision(), num2);

    return (num1.re == _num2.re) && (num1.im == _num2.im);
}

bool operator==(const float num1, const Complex& num2)
{
    Complex _num1(num2.GetBitPrecision(), num1);

    return (_num1.re == num2.re) && (_num1.im == num2.im);
}

bool operator!=(const Complex& num1, const Complex& num2)
{
    return (num1.re != num2.re) || (num1.im != num2.im);
}

bool operator!=(const Complex& num1, const int num2)
{
    Complex _num2(num1.GetBitPrecision(), num2);

    return (num1.re != _num2.re) || (num1.im != _num2.im);
}

bool operator!=(const int num1, const Complex& num2)
{
    Complex _num1(num2.GetBitPrecision(), num1);

    return (_num1.re != num2.re) || (_num1.im != num2.im);
}

bool operator!=(const Complex& num1, const float num2)
{
    Complex _num2(num1.GetBitPrecision(), num2);

    return (num1.re != _num2.re) || (num1.im != _num2.im);
}

bool operator!=(const float num1, const Complex& num2)
{
    Complex _num1(num2.GetBitPrecision(), num1);

    return (_num1.re != num2.re) || (_num1.im != num2.im);
}

bool operator<(const Complex& num1, const Complex& num2)
{
    if (num1.re < num2.re)
        return true;
    if (num1.re == num2.re)
        return num1.im < num2.im;

    return false;
}

bool operator<(const Complex& num1, const int num2)
{
    return num1 < Complex(num1.GetBitPrecision(), num2);
}

bool operator>(const Complex& num1, const Complex& num2)
{
    if (num1.re > num2.re)
        return true;
    if (num1.re == num2.re)
        return num1.im > num2.im;

    return false;
}

bool operator>(const Complex& num1, const int num2)
{
    return num1 > Complex(num1.GetBitPrecision(), num2);
}

bool operator<=(const Complex& num1, const Complex& num2)
{
    return (num1 < num2) || (num1 == num2);
}

bool operator<=(const Complex& num1, const int num2)
{
    return (num1 < num2) || (num1 == num2);
}

bool operator>=(const Complex& num1, const Complex& num2)
{
    return (num1 > num2) || (num1 == num2);
}

bool operator>=(const Complex& num1, const int num2)
{
    return (num1 > num2) || (num1 == num2);
}

Complex exp(const Complex& num, int& res_pos)
{
    Complex res(num.GetBitPrecision() + 1);
    Complex _num(num);
    _num.SetBitPrecision(num.GetBitPrecision() + 1);

    Real exponent = exp(_num.re);

    res.re = exponent * cos(_num.im);
    res.im = exponent * sin(_num.im);

    if (res.re.IsInfinity() || res.re.IsNaN() || res.im.IsInfinity() || res.im.IsNaN())
        throw MathException(Overflow);

    res.SetBitPrecision(num.GetBitPrecision());

    return res;
}

Complex ln(const Complex& num, int& res_pos)
{
    Complex _num(num);

    Real r(_num.GetBitPrecision()), theta(_num.GetBitPrecision());
    ((Complex&)_num).GetPolarForm(r, theta);

    Complex res(ln(r), theta);

    return res;
}

Complex lg(const Complex& num, int& res_pos)
{
    Complex res = ln(num, res_pos) / ln(Complex(num.GetBitPrecision(), 10), res_pos);

    return res;
}

Complex log(const Complex& num1, const Complex& num2, int& res_pos)
{
    Complex res = ln(num2, res_pos) / ln(num1, res_pos);

    return res;
}

Complex pow(const Complex& num1, const int num2, int& res_pos)
{
    if (num1.IsZero() && num2 <= 0)
        throw MathException(Overflow);

    Complex _num1;
    unsigned int _num2;

    if (num2 < 0)
    {
        _num1 = Complex(num1.GetBitPrecision(), 1) / num1;
        _num2 = static_cast<unsigned int>(-(static_cast<long long>(num2)));
    }
    else
    {
        _num1 = num1;
        _num2 = static_cast<unsigned int>(num2);
    }

    Complex res(num1.GetBitPrecision(), 1);

    while (_num2 > 0)
    {
        if (_num2 % 2 == 1)
            res *= _num1;
        _num1 *= _num1;
        _num2 >>= 1;
    }

    return res;
}

Complex pow(const Complex& num1, const Complex& num2, int& res_pos)
{
    if (num1.IsZero() && num2 <= 0)
        throw MathException(Overflow);

    if (num1.IsReal() && num2.IsReal())
    {
        Real re = pow(num1.GetRe(), num2.GetRe());
        return Complex(re, Real(num2.GetBitPrecision(), 0));
    }

    if (num2.IsReal() && num2.GetRe().IsInteger() && num1.GetRe() == 0)
    {
        Real p = num2.GetRe();
        Real r = pow(num1.GetIm(), num2.GetRe());
        if (p % 2 == 0)
            return -Complex(r.GetBitPrecision(), r, 0);
        return Complex(r.GetBitPrecision(), 0, r);
    }

    Complex res = exp(num2 * ln(num1, res_pos), res_pos);

    return res;
}

Complex sqrt(const Complex& num, int& res_pos)
{
    if (num.IsZero())
        return Complex(num.GetBitPrecision(), 0);

	int cur_pos = res_pos % 2;
	res_pos /= 2;

    if (num.GetIm() == 0 && num.GetRe() > 0)
    {
        if (cur_pos == 0)
            return Complex(sqrt(num.GetRe()), Real(num.GetBitPrecision(), 0));
        res_pos -= 2;
        return -Complex(sqrt(num.GetRe()), Real(num.GetBitPrecision(), 0));
    }

    Real modulus(num.GetBitPrecision(), 0);
    Real theta(num.GetBitPrecision(), 0);

    ((Complex&)num).GetPolarForm(modulus, theta);

    Real coeff = sqrt(modulus);
    Real theta2(theta / 2);

    if (cur_pos == 0)
        return Complex(coeff * cos(theta2), coeff * sin(theta2));

    Real pi2 = pi(num.GetBitPrecision()) * 2;
    res_pos -= 2;
    return Complex(coeff * cos((theta + pi2) / 2), coeff * sin((theta + pi2) / 2));
}

Complex root(const Complex& num1, const Complex& num2, int& res_pos)
{
    if (num1.IsZero())
        return Complex(num1.GetBitPrecision(), 0);

    Complex res(std::max(num1.GetBitPrecision(), num2.GetBitPrecision()));

    if (num2.IsReal() && num2.re.IsInteger())
    {
        int _num2 = (int)num2.re;
        Real modulus(num1.GetBitPrecision(), 0);
        Real theta(num1.GetBitPrecision(), 0);
        Real pi2 = pi(num1.GetBitPrecision()) * 2;
		int cur_pos = res_pos % _num2;
		res_pos /= _num2;

        ((Complex&)num1).GetPolarForm(modulus, theta);
        Real coeff = root(modulus, num2.re);
        return Complex(coeff * cos((theta + pi2 * cur_pos) / num2.re), coeff * sin((theta + pi2 * cur_pos) / num2.re));
    }

    return pow(num1, 1 / num2, res_pos);
}

Complex abs(const Complex& num)
{
    Complex res(module(num));

    return res;
}

Complex min(const Complex& num1, const Complex& num2, int& res_pos)
{
    (void)res_pos;
    return num1 < num2 ? num1 : num2;
}

Complex max(const Complex& num1, const Complex& num2, int& res_pos)
{
    (void)res_pos;
    return num1 > num2 ? num1 : num2;
}

Complex mod(const Complex& num)
{
    Complex res(module(num));

    return res;
}

Complex fact(const Complex& num, ParserContext* parser_context)
{
    CheckBreak(parser_context);
    if (!num.IsReal() || !num.re.IsInteger())
        throw MathException(ArgumentIsOver);

    Complex res(fact(num.re, parser_context));

    return res;
}

Complex arg(const Complex& num)
{
    Complex res(argument(num));

    return res;
}

Complex re(const Complex& num)
{
    return Complex(num.GetRe());
}

Complex im(const Complex& num)
{
    return Complex(num.GetIm());
}

Complex sin(const Complex& num, int& res_pos)
{
    Complex j(num.GetBitPrecision(), 0, 1);
    return (exp(j * num, res_pos) - exp(-j * num, res_pos)) * (1 / (2 * j));
}

Complex cos(const Complex& num, int& res_pos)
{
    Complex j(num.GetBitPrecision(), 0, 1);
    return (exp(j * num, res_pos) + exp(-j * num, res_pos)) / 2;
}

Complex tg(const Complex& num, int& res_pos)
{
    Complex j(num.GetBitPrecision(), 0, 1);
    Complex exp1 = exp(j * num, res_pos);
    Complex exp2 = exp(-j * num, res_pos);
    return (exp1 - exp2) * (1 / (j * (exp1 + exp2)));
}

Complex ctg(const Complex& num, int& res_pos)
{
    Complex j(num.GetBitPrecision(), 0, 1);
    Complex exp1 = exp(j * num, res_pos);
    Complex exp2 = exp(-j * num, res_pos);
    return (j * (exp1 + exp2)) * (1 / (exp1 - exp2));
}

Complex sec(const Complex& num, int& res_pos)
{
    return 1 / cos(num, res_pos);
}

Complex cosec(const Complex& num, int& res_pos)
{
    return 1 / sin(num, res_pos);
}

Complex arcsin(const Complex& num, int& res_pos)
{
    Complex j(num.GetBitPrecision(), 0, 1);
    return 1 / j * ln(j * num + sqrt(1 - pow(num, 2, res_pos), res_pos), res_pos);
}

Complex arccos(const Complex& num, int& res_pos)
{
    Complex half_pi(pi(num.GetBitPrecision()) / 2);
    return half_pi - arcsin(num, res_pos);
}

Complex arctg(const Complex& num, int& res_pos)
{
    if (num.GetIm() == 0)
        return Complex(arctg(num.GetRe()));
    Complex j(num.GetBitPrecision(), 0, 1);
    return j * (ln(1 - j * num, res_pos) - ln(1 + j * num, res_pos)) / 2;
}

Complex arcctg(const Complex& num, int& res_pos)
{
    if (num.GetIm() == 0)
        return Complex(arcctg(num.GetRe()));
    Complex j(num.GetBitPrecision(), 0, 1);
    return j * (ln(1 - j / num, res_pos) - ln(1 + j / num, res_pos)) / 2;
}

Complex arcsec(const Complex& num, int& res_pos)
{
    Complex j(num.GetBitPrecision(), 0, 1);

    return arccos(1 / num, res_pos);
}

Complex arccosec(const Complex& num, int& res_pos)
{
    return arcsin(1 / num, res_pos);
}

Complex sh(const Complex& num, int& res_pos)
{
    return (exp(num, res_pos) - exp(-(Complex&)num, res_pos)) / 2;
}

Complex ch(const Complex& num, int& res_pos)
{
    return (exp(num, res_pos) + exp(-(Complex&)num, res_pos)) / 2;
}

Complex th(const Complex& num, int& res_pos)
{
    return (exp(num, res_pos) - exp(-(Complex&)num, res_pos)) / (exp(num, res_pos) + exp(-(Complex&)num, res_pos));
}

Complex cth(const Complex& num, int& res_pos)
{
    return (exp(num, res_pos) + exp(-(Complex&)num, res_pos)) / (exp(num, res_pos) - exp(-(Complex&)num, res_pos));
}

Complex sch(const Complex& num, int& res_pos)
{
    return 1 / ch(num, res_pos);
}

Complex csch(const Complex& num, int& res_pos)
{
    return 1 / sh(num, res_pos);
}

Complex arsh(const Complex& num, int& res_pos)
{
    Complex j(num.GetBitPrecision(), 0, 1);
    Complex r = j * arcsin(-j * num, res_pos);
    r.SetAngleMeasure(AngleMeasure::None);
    return r;
}

Complex arch(const Complex& num, int& res_pos)
{
    return ln(num + sqrt(pow(num, 2, res_pos) - 1, res_pos), res_pos);
}

Complex arth(const Complex& num, int& res_pos)
{
    Complex j(num.GetBitPrecision(), 0, 1);
    Complex r = j * arctg(-j * num, res_pos);
    r.SetAngleMeasure(AngleMeasure::None);
    return r;
}

Complex arcth(const Complex& num, int& res_pos)
{
    Complex r = (ln((num + 1) / (num - 1), res_pos)) / 2;
    r.SetAngleMeasure(AngleMeasure::None);
    return r;
}

Complex arsch(const Complex& num, int& res_pos)
{
    return ln((1 + sqrt(1 - pow(num, 2, res_pos), res_pos)) / num, res_pos);
}

Complex arcsch(const Complex& num, int& res_pos)
{
    return arsh(1 / num, res_pos);
}

Real module(const Complex& num)
{
    return sqrt(pow(num.GetRe(), 2) + pow(num.GetIm(), 2));
}

Real argument(const Complex& num)
{
    Real res(num.GetBitPrecision());

    if (num.re > 0)
    {
        res = arctg(num.im / num.re);
    }
    else if (num.re < 0)
    {
        if (num.im >= 0)
            res = arctg(num.im / num.re) + pi(num.GetBitPrecision());
        else
            res = arctg(num.im / num.re) - pi(num.GetBitPrecision());
    }
    else
    {
        if (num.im > 0)
            res = pi(num.GetBitPrecision()) * Real(num.GetBitPrecision(), 0.5f);
        else if (num.im < 0)
            res = -pi(num.GetBitPrecision()) * Real(num.GetBitPrecision(), 0.5f);
        else
            throw MathException(Overflow);
    }

    return res;
}

Complex exp_complex(const int precision, AngleMeasure angle_measure)
{
    int res_pos = 0;
    Complex v(precision, 1);
    v.SetAngleMeasure(angle_measure);
    return exp(v, res_pos);
}

Complex pi_complex(const int precision, AngleMeasure angle_measure)
{
    return Complex(pi(precision));
}

Complex j(const int precision)
{
    return Complex(precision, 0, 1);
}

Complex Complex::ToGrad() const
{
    return Complex(re.ToGrad(), im.ToGrad());
}

Complex Complex::ToRadian() const
{
    return Complex(re.ToRadian(), im.ToRadian());
}

Complex Complex::ToDegree() const
{
    return Complex(re.ToDegree(), im.ToDegree());
}

Complex rad(const Complex& num, int& res_pos)
{
    return Complex(rad(num.re), rad(num.im));
}

Complex deg(const Complex& num, int& res_pos)
{
    return Complex(deg(num.re), deg(num.im));
}

Complex minute(const Complex& num, int& res_pos)
{
    return Complex(minute(num.re), minute(num.im));
}

Complex second(const Complex& num, int& res_pos)
{
    return Complex(second(num.re), second(num.im));
}

Complex grad(const Complex& num, int& res_pos)
{
    return Complex(grad(num.re), grad(num.im));
}

void Complex::GetPolarForm(Real& mod, Real& arg)
{
    mod = module(*this);
    arg = argument(*this);
}

int Complex::GetPrecision() const
{
    return std::min(re.GetPrecision(), im.GetPrecision());
}

void Complex::SetPrecision(int prec)
{
    re.SetPrecision(prec);
    im.SetPrecision(prec);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

void Complex::SetBitPrecision(const int precision)
{
    re.SetBitPrecision(precision);
    im.SetBitPrecision(precision);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

void Complex::RaisePrecision()
{
    SetPrecision(GetPrecision() + 1);
}

void Complex::LowerPrecision()
{
    SetPrecision(GetPrecision() - 1);
}

void Complex::LowerPrecision(int prec)
{
    re.LowerPrecision(prec);
    im.LowerPrecision(prec);

#ifdef TRACE_OUTPUT
    UpdateNumberStr();
#endif
}

AngleMeasure Complex::GetAngleMeasure()
{
    return re.angle_measure;
}

void Complex::SetAngleMeasure(AngleMeasure angle_measure)
{
    re.angle_measure = angle_measure;
    im.angle_measure = angle_measure;
}

Complex& Complex::Round(int prec)
{
    re.Round(prec);
    im.Round(prec);

    return *this;
}

#ifdef TRACE_OUTPUT
void Complex::UpdateNumberStr()
{
    number_str = ToString();
}
#endif

std::u32string Complex::ToString() const
{
    if (im == 0)
        return re.ToString();
    if (re == 0)
        return U"i*" + im.ToString();
    return re.ToString() + U"+i*" + im.ToString();
}

std::string Complex::ToStdString(int exp, int accuracy) const
{
    if (im == 0)
        return re.ToStdString(exp, accuracy);
    if (re == 0)
        return "i*" + im.ToStdString(exp, accuracy);
    return re.ToStdString(exp, accuracy) + "+i*" + im.ToStdString(exp, accuracy);
}

Complex Complex::GetNumber()
{
    Complex res(*this);
    return res;
}

}
