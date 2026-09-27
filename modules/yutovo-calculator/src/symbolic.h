/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef SYMBOLIC_H
#define SYMBOLIC_H

#include <string>
#include <memory>
#include <regex>
#include <type_traits>
#include <iomanip>
#include <cerrno>
#include <climits>
#include <set>
#include <cmath>
#if !defined(_WIN32) && !defined(HAVE_ALLOCA_H)
#define HAVE_ALLOCA_H
#endif

#ifdef alloca
#undef alloca
#endif

#include <giac/giac.h>
#include <giac/intg.h>
#include <giac/lin.h>
#include <giac/series.h>

#ifdef _WIN32
#undef ulong
#endif

#include "math_helper.h"
#include "parser_exception.h"
#include "utils.h"
#include "giac_utils.h"

namespace yutovo_calculator
{

struct ParserContext;
void CheckBreak(ParserContext* parser_context);

template<typename Number> class Symbolic;
class Rational;
class Complex;

template<class Number>
class Symbolic
{
public:
    Symbolic() :
        precision(3),
        expr(std::make_unique<giac::gen>(0))
    {
    }

    explicit Symbolic(int _precision) :
        precision(_precision),
        expr(std::make_unique<giac::gen>(0))
    {
        giac::decimal_digits(std::max(1, _precision + 1), Context());
    }

    explicit Symbolic(int _precision, int num) :
        precision(_precision),
        expr(std::make_unique<giac::gen>(num))
    {
        giac::decimal_digits(std::max(1, _precision + 1), Context());
    }

    static bool IsNegativeInfinityNumber(const Real& num);

    explicit Symbolic(int _precision, float num) :
        precision(_precision),
        expr(std::make_unique<giac::gen>(static_cast<double>(num)))
    {
        giac::decimal_digits(std::max(1, _precision + 1), Context());
    }

    explicit Symbolic(int _precision, const Number& num) :
        precision(_precision),
        expr(std::make_unique<giac::gen>(ToExpression(num)))
    {
        giac::decimal_digits(std::max(1, _precision + 1), Context());
        if constexpr (std::is_same_v<Number, Real>)
        {
            if (IsNegativeInfinityNumber(num))
                explicit_negative_infinity = true;
        }
    }

    explicit Symbolic(int _precision, const std::u32string& num) :
        precision(_precision),
        expr(std::make_unique<giac::gen>(ToBasicString(num)))
    {
        giac::decimal_digits(std::max(1, _precision + 1), Context());
    }

    explicit Symbolic(int _precision, const std::string& num) :
        precision(_precision),
        expr(std::make_unique<giac::gen>(ToBasicString(ToUtfString(num))))
    {
        giac::decimal_digits(std::max(1, _precision + 1), Context());
    }

    Symbolic(const Symbolic& source) :
        precision(source.precision),
        expr(std::make_unique<giac::gen>(*source.expr)),
        explicit_negative_infinity(source.explicit_negative_infinity)
    {
    }

public:
    typedef Symbolic<Number> value_type;

    Symbolic<Number>& operator=(const Symbolic<Number>& source)
    {
        precision = source.precision;
        *expr = *source.expr;
        explicit_negative_infinity = source.explicit_negative_infinity;
        return *this;
    }

    Symbolic<Number>& operator=(const int num)
    {
        *expr = giac::gen(num);
        return *this;
    }

    Symbolic<Number>& operator=(const double num)
    {
        *expr = giac::gen(num);
        return *this;
    }

    Symbolic<Number>& operator=(const std::u32string& num)
    {
        *expr = ToBasicString(num);
        return *this;
    }

    Symbolic<Number>& operator=(const Number& num)
    {
        *expr = ToExpression(num);
        explicit_negative_infinity = false;
        if constexpr (std::is_same_v<Number, Real>)
        {
            if (IsNegativeInfinityNumber(num))
                explicit_negative_infinity = true;
        }
        return *this;
    }

    Symbolic<Number> operator+() const
    {
        return *this;
    }

    Symbolic<Number> operator-() const
    {
        Symbolic<Number> res(*this);
        *res.expr = -*expr;
        if (*res.expr == giac::minus_inf)
            res.explicit_negative_infinity = true;
        return res;
    }

    friend Symbolic<Number> operator+(const Symbolic<Number>& num1, const Symbolic<Number>& num2)
    {
        Symbolic<Number> res(num1.precision);
        *res.expr = giac::operator_plus(*num1.expr, *num2.expr, num1.Context());
        return res;
    }

    friend Symbolic<Number> operator-(const Symbolic<Number>& num1, const Symbolic<Number>& num2)
    {
        Symbolic<Number> res(num1.precision);
        *res.expr = giac::operator_minus(*num1.expr, *num2.expr, num1.Context());
        return res;
    }

    friend Symbolic<Number> operator*(const Symbolic<Number>& num1, const Symbolic<Number>& num2)
    {
        Symbolic<Number> res(num1.precision);
        *res.expr = giac::operator_times(*num1.expr, *num2.expr, num1.Context());
        return res;
    }

    friend Symbolic<Number> operator/(const Symbolic<Number>& num1, const Symbolic<Number>& num2)
    {
        Symbolic<Number> res(num1.precision);
        *res.expr = SimplifyPowerDivision(*num1.expr, *num2.expr, res.Context());
        return res;
    }

    void operator+=(const Symbolic<Number>& num)
    {
        *expr = giac::operator_plus(*expr, *num.expr, Context());
    }

    void operator-=(const Symbolic<Number>& num)
    {
        *expr = giac::operator_minus(*expr, *num.expr, Context());
    }

    void operator*=(const Symbolic<Number>& num)
    {
        *expr = giac::operator_times(*expr, *num.expr, Context());
    }

    void operator/=(const Symbolic<Number>& num)
    {
        *expr = SimplifyPowerDivision(*expr, *num.expr, Context());
    }

    friend bool operator==(const Symbolic<Number>& num1, const Symbolic<Number>& num2)
    {
        giac::gen derivative = giac::operator_minus(*num1.expr, *num2.expr, num1.Context());
        giac::gen s = giac::simplify(derivative, num1.Context());
        return giac::is_zero(s, num1.Context());
    }

    friend bool operator==(const Symbolic<Number>& num1, const int num2)
    {
        return num1 == Symbolic<Number>(num1.precision, num2);
    }

    friend bool operator==(const int num1, const Symbolic<Number>& num2)
    {
        return num2 == num1;
    }

    friend bool operator!=(const Symbolic<Number>& num1, const Symbolic<Number>& num2)
    {
        return !(num1 == num2);
    }

    friend bool operator!=(const Symbolic<Number>& num1, const int num2)
    {
        return !(num1 == num2);
    }

    friend bool operator!=(const int num1, const Symbolic<Number>& num2)
    {
        return !(num2 == num1);
    }

    friend bool operator>(const Symbolic<Number>& num1, const Symbolic<Number>& num2)
    {
        throw ParserException({}, ParserExceptionCode::IncorrectOperation);
    }

    friend bool operator>(const Symbolic<Number>& num1, const int num2)
    {
        throw ParserException({}, ParserExceptionCode::IncorrectOperation);
    }

    friend bool operator>(const int num1, const Symbolic<Number>& num2)
    {
        throw ParserException({}, ParserExceptionCode::IncorrectOperation);
    }

    friend bool operator>=(const Symbolic<Number>& num1, const Symbolic<Number>& num2)
    {
        throw ParserException({}, ParserExceptionCode::IncorrectOperation);
    }

    friend bool operator>=(const Symbolic<Number>& num1, const int num2)
    {
        throw ParserException({}, ParserExceptionCode::IncorrectOperation);
    }

    friend bool operator>=(const int num1, const Symbolic<Number>& num2)
    {
        throw ParserException({}, ParserExceptionCode::IncorrectOperation);
    }

    friend bool operator<(const Symbolic<Number>& num1, const Symbolic<Number>& num2)
    {
        throw ParserException({}, ParserExceptionCode::IncorrectOperation);
    }

    friend bool operator<(const Symbolic<Number>& num1, const int num2)
    {
        throw ParserException({}, ParserExceptionCode::IncorrectOperation);
    }

    friend bool operator<(const int num1, const Symbolic<Number>& num2)
    {
        throw ParserException({}, ParserExceptionCode::IncorrectOperation);
    }

    friend bool operator<=(const Symbolic<Number>& num1, const Symbolic<Number>& num2)
    {
        throw ParserException({}, ParserExceptionCode::IncorrectOperation);
    }

    friend bool operator<=(const Symbolic<Number>& num1, const int num2)
    {
        throw ParserException({}, ParserExceptionCode::IncorrectOperation);
    }

    friend bool operator<=(const int num1, const Symbolic<Number>& num2)
    {
        throw ParserException({}, ParserExceptionCode::IncorrectOperation);
    }

public:
    friend Symbolic<Number> evalf(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        giac::decimal_digits(std::max(1, num.precision + 10), res.Context());
        giac::gen g = giac::evalf(*num.expr, 1, res.Context());
        if (g.type == giac::_REAL || g.type == giac::_DOUBLE_ || g.type == giac::_CPLX)
        {
            static const giac::gen threshold(1.7976931348623157e308);
            giac::gen abs_g = giac::abs(g, res.Context());
            if (giac::is_greater(abs_g, threshold, res.Context()))
            {
                if (giac::is_positive(-g, res.Context()))
                    g = giac::minus_inf;
                else
                    g = giac::plus_inf;
            }
        }
        *res.expr = g;
        return res;
    }

    friend Symbolic<Number> evalf(const Symbolic<Number>& num, const Symbolic<Number>& prec)
    {
        int p = 0;
        try
        {
            p = static_cast<int>(prec.expr->val);
        }
        catch (...)
        {
            p = num.precision;
        }
        Symbolic<Number> res(p);
        giac::decimal_digits(std::max(1, p + 1), res.Context());
        *res.expr = giac::evalf(*num.expr, 1, res.Context());
        return res;
    }

    friend Symbolic<Number> expand(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = giac::expand(*num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> simplify(const Symbolic<Number>& num)
    {
        if (num.expr)
        {
            try
            {
                giac::gen g;
                if (!HasUnknownSymbol(*num.expr))
                {
                    g = giac::_texpand(*num.expr, num.Context());
                    g = giac::normal(g, num.Context());
                }
                else
                {
                    g = giac::factor(*num.expr, false, num.Context());
                }
                Symbolic<Number> res(num.precision);
                *res.expr = g;
                return res;
            }
            catch (...)
            {
            }
        }
        Symbolic<Number> res(num);
        return res;
    }

    friend Symbolic<Number> derivative(const Symbolic<Number>& num, const Symbolic<Number>& var)
    {
        if (var.expr->type != giac::_IDNT)
            throw ParserException({}, ParserExceptionCode::IncorrectOperation);
        Symbolic<Number> res(num.precision);
        *res.expr = giac::derive(*num.expr, *var.expr, res.Context());
        *res.expr = giac::eval(*res.expr, 1, res.Context());
        return res;
    }

    friend Symbolic<Number> indefinite_integral(const Symbolic<Number>& expr, const Symbolic<Number>& var)
    {
        if (var.expr->type != giac::_IDNT)
            throw ParserException({}, ParserExceptionCode::IncorrectOperation);

        GiacMpfrStateGuard mpfr_guard;
        GiacOutputGuard output_guard;
        Symbolic<Number> res(expr.precision);
        giac::vecteur args;
        args.push_back(*expr.expr);
        args.push_back(*var.expr);
        try
        {
            *res.expr = giac::_integrate(args, res.Context());
        }
        catch (...)
        {
            throw MathException(IncorrectOperation);
        }
        return res;
    }

    friend Symbolic<Number> subs(const Symbolic<Number>& num, const Symbolic<Number>& var, const Symbolic<Number>& value)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = giac::subst(*num.expr, *var.expr, *value.expr, false, res.Context());
        if (*res.expr == giac::undef)
        {
            if (var.expr->type == giac::_IDNT && HasAmbiguousPoleArgument(*num.expr, *var.expr->_IDNTptr, *value.expr, res.Context()))
            {
                if constexpr (std::is_same_v<Number, Rational>)
                    throw MathException(NotImplemented);
                return Symbolic<Number>(num.precision, std::string("nan"));
            }
            return Symbolic<Number>(num.precision, std::string("nan"));
        }
        if (*res.expr == giac::minus_inf && num.expr->type == giac::_SYMB && num.expr->_SYMBptr->sommet == giac::at_ln &&
            giac::is_zero(giac::subst(num.expr->_SYMBptr->feuille, *var.expr, *value.expr, false, res.Context()), res.Context()))
        {
            return Symbolic<Number>(num.precision, std::u32string(U"∞"));
        }
        if constexpr (!std::is_same_v<Number, Rational>)
        {
            if (*res.expr == InertCall("acoth", giac::gen(1), res.Context()) || *res.expr == InertCall("acoth", giac::gen(1.), res.Context()))
                return Symbolic<Number>(num.precision, std::u32string(U"∞"));
            if (*res.expr == InertCall("acoth", giac::gen(-1), res.Context()) || *res.expr == InertCall("acoth", giac::gen(-1.), res.Context()))
                return Symbolic<Number>(num.precision, std::u32string(U"-∞"));
            if (*res.expr == InertCall("acsch", giac::gen(0), res.Context()) || *res.expr == InertCall("acsch", giac::gen(0.), res.Context()))
                return Symbolic<Number>(num.precision, std::u32string(U"∞"));
        }
        return res;
    }

    friend Symbolic<Number> definite_integral(const Symbolic<Number>& a, const Symbolic<Number>& b,
        const Symbolic<Number>& expr, const Symbolic<Number>& var)
    {
        if (var.expr->type != giac::_IDNT)
            throw ParserException({}, ParserExceptionCode::IncorrectOperation);

        GiacMpfrStateGuard mpfr_guard;
        GiacOutputGuard output_guard;
        Symbolic<Number> res(expr.precision);
        try
        {
            Symbolic<Number> antiderivative = indefinite_integral(expr, var);
            Symbolic<Number> upper_value = subs(antiderivative, var, b);
            Symbolic<Number> lower_value = subs(antiderivative, var, a);
            res = upper_value - lower_value;
            *res.expr = giac::simplify(*res.expr, res.Context());
            if constexpr (std::is_same_v<Number, Real> || std::is_same_v<Number, Complex>)
            {
                if (res.IsNumber())
                    *res.expr = giac::evalf(*res.expr, 1, res.Context());
            }
        }
        catch (...)
        {
            throw MathException(IncorrectOperation);
        }
        return res;
    }

    friend Symbolic<Number> min(const Symbolic<Number>& num1, const Symbolic<Number>& num2)
    {
        Symbolic<Number> res(num1.precision);
        *res.expr = giac::min(*num1.expr, *num2.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> max(const Symbolic<Number>& num1, const Symbolic<Number>& num2)
    {
        Symbolic<Number> res(num1.precision);
        *res.expr = giac::max(*num1.expr, *num2.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> sin(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = giac::sin(*num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> cos(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = giac::cos(*num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> tg(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = giac::tan(*num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> cot(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = InertCall("cot", *num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> sec(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = InertCall("sec", *num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> csc(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = InertCall("csc", *num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> arcsin(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = giac::asin(*num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> arccos(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = giac::acos(*num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> arctg(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = giac::atan(*num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> arcctg(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = giac::atan(giac::inv(*num.expr, res.Context()), res.Context());
        return res;
    }

    friend Symbolic<Number> arcsec(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = giac::acos(giac::inv(*num.expr, res.Context()), res.Context());
        return res;
    }

    friend Symbolic<Number> arccsc(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = giac::asin(giac::inv(*num.expr, res.Context()), res.Context());
        return res;
    }

    friend Symbolic<Number> gamma(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = giac::Gamma(*num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> fact(const Symbolic<Number>& num, ParserContext* parser_context)
    {
        if (giac::is_integer(*num.expr) && num.expr->val >= 0)
        {
            long n = num.expr->val;
            Symbolic<Number> res(num.precision);
            *res.expr = giac::gen(1);
            for (long i = 2; i <= n; ++i)
            {
                CheckBreak(parser_context);
                *res.expr = giac::operator_times(*res.expr, giac::gen(static_cast<int>(i)), res.Context());
            }
            return res;
        }
        Symbolic<Number> res(num.precision);
        *res.expr = giac::symbolic(*giac::at_factorial, *num.expr);
        return res;
    }

    friend Symbolic<Number> sinh(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = giac::sinh(*num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> cosh(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = giac::cosh(*num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> tanh(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = giac::tanh(*num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> coth(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = InertCall("coth", *num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> sech(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = InertCall("sech", *num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> csch(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = InertCall("csch", *num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> asinh(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = InertCall("asinh", *num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> acosh(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = InertCall("acosh", *num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> atanh(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = InertCall("atanh", *num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> acoth(const Symbolic<Number>& num)
    {
        if constexpr (!std::is_same_v<Number, Rational>)
        {
            if (num.expr && (*num.expr == giac::gen(1) || *num.expr == giac::gen(1.)))
                return Symbolic<Number>(num.precision, std::u32string(U"∞"));
            if (num.expr && (*num.expr == giac::gen(-1) || *num.expr == giac::gen(-1.)))
                return Symbolic<Number>(num.precision, std::u32string(U"-∞"));
        }
        Symbolic<Number> res(num.precision);
        *res.expr = InertCall("acoth", *num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> asech(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = InertCall("asech", *num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> acsch(const Symbolic<Number>& num)
    {
        if constexpr (!std::is_same_v<Number, Rational>)
        {
            if (num.expr && giac::is_zero(*num.expr, num.Context()))
                return Symbolic<Number>(num.precision, std::u32string(U"∞"));
        }
        Symbolic<Number> res(num.precision);
        *res.expr = InertCall("acsch", *num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> exp(const Symbolic<Number>& num)
    {
        Symbolic<Number> res(num.precision);
        *res.expr = giac::exp(*num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> ln(const Symbolic<Number>& num)
    {
        if (giac::is_zero(*num.expr, num.Context()))
            return Symbolic<Number>(num.precision, std::u32string(U"∞"));
        Symbolic<Number> res(num.precision);
        *res.expr = giac::log(*num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> sqrt(const Symbolic<Number>& num)
    {
        if (num.expr && *num.expr == giac::minus_inf)
        {
            if constexpr (std::is_same_v<Number, Rational>)
                throw MathException(NotImplemented);
            return Symbolic<Number>(num.precision, std::string("nan"));
        }
        Symbolic<Number> res(num.precision);
        *res.expr = giac::sqrt(*num.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> pow(const Symbolic<Number>& num1, const Symbolic<Number>& num2)
    {
        if (giac::is_zero(*num2.expr, num2.Context()))
            return Symbolic<Number>(num1.precision, 1);
        Symbolic<Number> res(num1.precision);
        *res.expr = giac::pow(*num1.expr, *num2.expr, res.Context());
        return res;
    }

    friend Symbolic<Number> log(const Symbolic<Number>& num1, const Symbolic<Number>& num2)
    {
        Symbolic<Number> res(num1.precision);
        *res.expr = giac::rdiv(giac::log(*num2.expr, res.Context()), giac::log(*num1.expr, res.Context()), res.Context());
        return res;
    }

    friend Symbolic<Number> root(const Symbolic<Number>& num1, const Symbolic<Number>& num2)
    {
        Symbolic<Number> res(num1.precision);
        *res.expr = giac::pow(*num1.expr, giac::rdiv(giac::gen(1), *num2.expr, res.Context()), res.Context());
        return res;
    }

public:
    std::u32string ToString(int exp, Language language = Language::English) const;
    std::string ToStdString(int exp, Language language = Language::English) const;
    std::string ToJson(int exp = -1, Language language = Language::English) const;

    int GetPrecision() const
    {
        return precision;
    }

    bool IsZero() const
    {
        return giac::is_zero(*expr, Context());
    }

    bool IsNumber() const
    {
        return expr->is_constant();
    }

public:
    giac::gen ToExpression(const Number& num) const;

    static giac::gen ToBasicString(const std::u32string& s)
    {
        std::string utf8 = yutovo_calculator::ToBasicString(s);
        const giac::context* c = current_giac_context ? current_giac_context : giac::context0;
        // giac uses i for the imaginary unit; uppercase I is our canonical form
        if (utf8 == "I")
            return ParseGen("i", c);
        if (utf8 == "oo" || utf8 == "+oo")
            return giac::plus_inf;
        if (utf8 == "-oo")
            return giac::minus_inf;
        if (utf8 == "nan")
            return giac::undef;
        return ParseGen(utf8, c);
    }

    static std::string ReplacePowerOperator(std::string s)
    {
        size_t pos = 0;
        while ((pos = s.find("**", pos)) != std::string::npos)
        {
            size_t base_end = pos;
            size_t base_start = base_end;
            if (base_start > 0 && s[base_start - 1] == ')')
            {
                if (base_start < 2)
                {
                    ++pos;
                    continue;
                }

                int depth = 1;
                base_start -= 2;
                bool found = false;
                while (true)
                {
                    if (s[base_start] == ')')
                        ++depth;
                    else if (s[base_start] == '(')
                    {
                        --depth;
                        if (depth == 0)
                        {
                            found = true;
                            break;
                        }
                    }

                    if (base_start == 0)
                        break;
                    --base_start;
                }

                if (!found)
                {
                    ++pos;
                    continue;
                }
            }
            else
            {
                while (base_start > 0 && (std::isalnum(static_cast<unsigned char>(s[base_start - 1])) || s[base_start - 1] == '.'))
                    --base_start;
            }

            size_t exp_start = pos + 2;
            size_t exp_end = exp_start;
            if (exp_start < s.size() && s[exp_start] == '(')
            {
                int depth = 1;
                ++exp_end;
                while (exp_end < s.size() && depth > 0)
                {
                    if (s[exp_end] == '(')
                        ++depth;
                    else if (s[exp_end] == ')')
                        --depth;
                    if (depth > 0)
                        ++exp_end;
                }
                ++exp_end;
            }
            else
            {
                while (exp_end < s.size() && (std::isalnum(static_cast<unsigned char>(s[exp_end])) || s[exp_end] == '.'))
                    ++exp_end;
            }

            std::string base = s.substr(base_start, base_end - base_start);
            std::string exp = s.substr(exp_start, exp_end - exp_start);
            auto strip_parens = [](const std::string& str) -> std::string {
                if (str.size() >= 2 && str.front() == '(' && str.back() == ')')
                    return str.substr(1, str.size() - 2);
                return str;
            };
            base = strip_parens(base);
            exp = strip_parens(exp);
            std::string replacement = "pow(" + base + "," + exp + ")";
            s.replace(base_start, exp_end - base_start, replacement);
            pos = base_start + replacement.size();
        }
        return s;
    }

    static std::string ReplaceCaretOperator(std::string s)
    {
        size_t pos = 0;
        while ((pos = s.find('^', pos)) != std::string::npos)
        {
            size_t base_end = pos;
            size_t base_start = base_end;
            if (base_start > 0 && s[base_start - 1] == ')')
            {
                if (base_start < 2)
                {
                    ++pos;
                    continue;
                }
                int depth = 1;
                base_start -= 2;
                while (true)
                {
                    if (s[base_start] == ')')
                        ++depth;
                    else if (s[base_start] == '(')
                    {
                        --depth;
                        if (depth == 0)
                            break;
                    }
                    if (base_start == 0)
                        break;
                    --base_start;
                }
                if (depth != 0)
                {
                    ++pos;
                    continue;
                }
            }
            else
            {
                while (base_start > 0 && (std::isalnum(static_cast<unsigned char>(s[base_start - 1])) || s[base_start - 1] == '.'))
                    --base_start;
            }

            size_t exp_start = pos + 1;
            size_t exp_end = exp_start;
            if (exp_start < s.size() && s[exp_start] == '(')
            {
                int depth = 1;
                ++exp_end;
                while (exp_end < s.size() && depth > 0)
                {
                    if (s[exp_end] == '(')
                        ++depth;
                    else if (s[exp_end] == ')')
                        --depth;
                    if (depth > 0)
                        ++exp_end;
                }
                ++exp_end;
            }
            else
            {
                if (exp_end < s.size() && s[exp_end] == '-')
                    ++exp_end;
                while (exp_end < s.size() && (std::isalnum(static_cast<unsigned char>(s[exp_end])) || s[exp_end] == '.'))
                    ++exp_end;
            }

            std::string base = s.substr(base_start, base_end - base_start);
            std::string exp = s.substr(exp_start, exp_end - exp_start);
            auto strip_parens = [](const std::string& str) -> std::string {
                if (str.size() >= 2 && str.front() == '(' && str.back() == ')')
                    return str.substr(1, str.size() - 2);
                return str;
            };
            base = strip_parens(base);
            exp = strip_parens(exp);
            std::string replacement = "pow(" + base + "," + exp + ")";
            s.replace(base_start, exp_end - base_start, replacement);
            pos = base_start + replacement.size();
        }
        return s;
    }

    static std::string ReplaceFunctionPower(std::string s)
    {
        size_t pos = 0;
        while ((pos = s.find("pow(", pos)) != std::string::npos)
        {
            size_t args_start = pos + 4;
            int depth = 1;
            size_t i = args_start;
            while (i < s.size() && depth > 0)
            {
                if (s[i] == '(')
                    ++depth;
                else if (s[i] == ')')
                    --depth;
                if (depth == 1 && s[i] == ',')
                    break;
                ++i;
            }
            if (i >= s.size() || depth != 1)
            {
                ++pos;
                continue;
            }
            size_t comma = i;
            size_t exp_start = comma + 1;
            depth = 1;
            i = exp_start;
            while (i < s.size() && depth > 0)
            {
                if (s[i] == '(')
                    ++depth;
                else if (s[i] == ')')
                    --depth;
                ++i;
            }
            if (depth != 0)
            {
                ++pos;
                continue;
            }
            size_t pow_end = i;
            std::string base = s.substr(args_start, comma - args_start);
            std::string exp = s.substr(exp_start, i - 1 - exp_start);
            auto strip_parens = [](const std::string& str) -> std::string {
                if (str.size() >= 2 && str.front() == '(' && str.back() == ')')
                    return str.substr(1, str.size() - 2);
                return str;
            };
            std::string base_inner = strip_parens(base);
            size_t func_paren = base_inner.find('(');
            if (func_paren != std::string::npos && func_paren > 0)
            {
                std::string func_name = base_inner.substr(0, func_paren);
                bool all_alpha = !func_name.empty() &&
                    std::all_of(func_name.begin(), func_name.end(), [](char c) { return std::isalpha(static_cast<unsigned char>(c)); });
                if (all_alpha)
                {
                    std::string func_args = base_inner.substr(func_paren + 1, base_inner.size() - func_paren - 2);
                    std::string replacement = func_name + "pow(" + func_args + "," + exp + ")";
                    s.replace(pos, pow_end - pos, replacement);
                    pos = pos + replacement.size();
                    continue;
                }
            }
            pos = pow_end;
        }
        return s;
    }

    static std::string ReplaceSqrtSymbol(std::string s)
    {
        size_t pos = 0;
        while ((pos = s.find("\xE2\x88\x9A", pos)) != std::string::npos) // UTF-8 for √
        {
            size_t arg_start = pos + 3;
            size_t arg_end = arg_start;
            if (arg_start < s.size() && s[arg_start] == '(')
            {
                int depth = 1;
                ++arg_end;
                while (arg_end < s.size() && depth > 0)
                {
                    if (s[arg_end] == '(')
                        ++depth;
                    else if (s[arg_end] == ')')
                        --depth;
                    if (depth > 0)
                        ++arg_end;
                }
                ++arg_end;
            }
            else
            {
                while (arg_end < s.size() && (std::isalnum(static_cast<unsigned char>(s[arg_end])) || s[arg_end] == '.'))
                    ++arg_end;
            }
            std::string arg = s.substr(arg_start, arg_end - arg_start);
            auto strip_parens = [](const std::string& str) -> std::string {
                if (str.size() >= 2 && str.front() == '(' && str.back() == ')')
                    return str.substr(1, str.size() - 2);
                return str;
            };
            arg = strip_parens(arg);
            std::string replacement = "sqrt(" + arg + ")";
            s.replace(pos, arg_end - pos, replacement);
            pos = pos + replacement.size();
        }
        return s;
    }

    static int TermDegree(const std::string& term)
    {
        static const std::set<std::string> known_funcs = {"sin", "cos", "tan", "tg", "cot", "ctg", "sec", "csc", "cosec", "sinh", "sh", "cosh", "ch", "tanh", "th", "coth", "cth", "sech", "sch", "csch", "cosech",
            "asinh", "acosh", "atanh", "acoth", "asech", "acsch", "arsinh", "arcosh", "artanh", "arcoth", "arsech", "arcsch",
            "arcsinh", "arccosh", "arctanh", "arccoth", "arcsech", "arccsch", "arccosech", "arctg", "arcctg", "exp", "ln", "log", "sqrt", "root", "pow",
            "gamma", "factorial", "min", "max", "derivative", "indefinite_integral", "definite_integral", "subs", "expand", "simplify", "evalf"};
        static const std::set<std::string> known_consts = {"pi", "i", "j", "e", "oo", "inf", "nan", "undef"};
        int degree = 0;
        //count explicit powers pow(var,exp)
        std::regex pow_re(R"(pow\(([a-zA-Z_][a-zA-Z0-9_]*),([0-9]+)\.?\))");
        std::sregex_iterator it(term.begin(), term.end(), pow_re);
        std::sregex_iterator end;
        for (; it != end; ++it)
        {
            std::string exp_str = (*it)[2];
            degree += std::stoi(exp_str);
        }
        //count remaining variable identifiers
        std::regex id_re(R"(\b([a-zA-Z_][a-zA-Z0-9_]*)\b)");
        it = std::sregex_iterator(term.begin(), term.end(), id_re);
        for (; it != end; ++it)
        {
            std::string id = (*it)[1];
            if (known_funcs.find(id) != known_funcs.end())
                continue;
            if (known_consts.find(id) != known_consts.end())
                continue;
            //check if already counted in pow(var,exp)
            std::regex already(R"(pow\()" + id + R"(,([0-9]+)\.?\))");
            if (std::regex_search(term, already))
                continue;
            degree += 1;
        }
        return degree;
    }

    static std::string SortExpressionTerms(std::string s)
    {
        //split at top-level + or - (keep sign with term)
        std::vector<std::pair<std::string, int>> terms;
        std::string current;
        int depth = 0;
        for (size_t i = 0; i < s.size(); ++i)
        {
            char c = s[i];
            if (c == '(')
                ++depth;
            else if (c == ')')
                --depth;
            if ((c == '+' || c == '-') && depth == 0 && i > 0)
            {
                terms.emplace_back(current, TermDegree(current));
                current.clear();
            }
            current += c;
        }
        if (terms.empty())
            return s;
        terms.emplace_back(current, TermDegree(current));
        std::stable_sort(terms.begin(), terms.end(), [](const auto& a, const auto& b) {
            return a.second < b.second;
        });
        std::string result = terms[0].first;
        for (size_t i = 1; i < terms.size(); ++i)
            result += "+" + terms[i].first;
        return result;
    }

    static std::string ReplaceUnitImaginary(std::string s)
    {
        auto replace_sub = 
            [](std::string& str, const std::string& from, const std::string& to)
            {
                size_t start_pos = 0;
                while ((start_pos = str.find(from, start_pos)) != std::string::npos)
                {
                    str.replace(start_pos, from.length(), to);
                    start_pos += to.length();
                }
            };
        replace_sub(s, "+1.*i", "+i");
        replace_sub(s, "-1.*i", "-i");
        replace_sub(s, "+1.*j", "+j");
        replace_sub(s, "-1.*j", "-j");
        if (s.size() >= 4 && s.substr(0, 4) == "1.*i")
            s = "i" + s.substr(4);
        if (s.size() >= 5 && s.substr(0, 5) == "-1.*i")
            s = "-i" + s.substr(5);
        if (s.size() >= 4 && s.substr(0, 4) == "1.*j")
            s = "j" + s.substr(4);
        if (s.size() >= 5 && s.substr(0, 5) == "-1.*j")
            s = "-j" + s.substr(5);
        return s;
    }

public:
    static std::string FormatScientific(std::string s)
    {
        size_t e_pos = s.find_first_of("eE");
        if (e_pos == std::string::npos)
            return s;

        std::string mantissa = s.substr(0, e_pos);
        std::string exp_str = s.substr(e_pos + 1);

        //remove trailing zeros from mantissa
        if (mantissa.find('.') != std::string::npos)
        {
            while (!mantissa.empty() && mantissa.back() == '0')
                mantissa.pop_back();
            if (!mantissa.empty() && mantissa.back() == '.')
                mantissa.pop_back();
        }

        //process exponent sign and leading zeros
        bool neg = false;
        if (!exp_str.empty() && exp_str[0] == '-')
        {
            neg = true;
            exp_str = exp_str.substr(1);
        }
        else if (!exp_str.empty() && exp_str[0] == '+')
        {
            exp_str = exp_str.substr(1);
        }
        while (exp_str.size() > 1 && exp_str[0] == '0')
            exp_str.erase(0, 1);

        return mantissa + "E" + (neg ? "-" : "+") + exp_str;
    }

    static std::vector<std::string> JsonScientificElements(const std::string& sci_str)
    {
        size_t e_pos = sci_str.find_first_of("eE");
        if (e_pos == std::string::npos)
            return {JsonCodeString(sci_str)};

        std::string mantissa = sci_str.substr(0, e_pos);
        std::string exp_str = sci_str.substr(e_pos + 1);

        bool neg = false;
        if (!exp_str.empty() && exp_str[0] == '-')
        {
            neg = true;
            exp_str = exp_str.substr(1);
        }
        else if (!exp_str.empty() && exp_str[0] == '+')
        {
            exp_str = exp_str.substr(1);
        }
        while (exp_str.size() > 1 && exp_str[0] == '0')
            exp_str.erase(0, 1);

        std::string power_exp = JsonCodeString(exp_str);
        if (neg)
            power_exp = JsonCodeRow({JsonMinus(), power_exp});

        std::string power = JsonPower(JsonCodeRow({JsonCodeString("10")}), JsonCodeRow({power_exp}));
        std::vector<std::string> result;
        if (!mantissa.empty() && mantissa[0] == '-')
        {
            result.push_back(JsonMinus());
            result.push_back(JsonCodeString(mantissa.substr(1)));
        }
        else
        {
            result.push_back(JsonCodeString(mantissa));
        }
        result.push_back(JsonMultiply());
        result.push_back(power);
        return result;
    }

public:
    static std::string FormatFixed(std::string s, bool keep_trailing_dot)
    {
        if (s.find_first_of("eE") != std::string::npos)
            return s;
        if (s.find('.') != std::string::npos)
        {
            while (!s.empty() && s.back() == '0')
                s.pop_back();
            if (!s.empty() && s.back() == '.')
            {
                if (!keep_trailing_dot)
                    s.pop_back();
            }
        }
        return s;
    }

    static bool IsEffectivelyZeroFixedStr(const std::string& s)
    {
        for (char c : s)
        {
            if (c != '0' && c != '.' && c != '-')
                return false;
        }
        return true;
    }

    static std::string ReplaceAll(std::string str, const std::string& from, const std::string& to)
    {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return str;
    }

    static std::string ReplaceLogWithLn(std::string str, Language language)
    {
        if (language != Language::Russian)
            return str;
        size_t pos = 0;
        while ((pos = str.find("log(", pos)) != std::string::npos)
        {
            size_t end = pos + 4;
            int depth = 1;
            bool has_comma = false;
            while (end < str.size() && depth > 0)
            {
                if (str[end] == '(')
                    ++depth;
                else if (str[end] == ')')
                    --depth;
                else if (str[end] == ',' && depth == 1)
                    has_comma = true;
                if (depth > 0)
                    ++end;
            }
            if (!has_comma)
            {
                str.replace(pos, 4, "ln(");
                pos += 3;
            }
            else
            {
                pos += 4;
            }
        }
        return str;
    }

    struct MpfrGuard
    {
        MpfrGuard()
        {
            mpfr_init2(num, 512);
        }

        ~MpfrGuard()
        {
            mpfr_clear(num);
        }

        mpfr_t num;
    };

    static int GetDecimalOrder(const std::string& num_str)
    {
        size_t e_pos = num_str.find_first_of("eE");
        if (e_pos != std::string::npos)
        {
            const char* start = num_str.c_str() + e_pos + 1;
            char* end = nullptr;
            errno = 0;
            long exp = std::strtol(start, &end, 10);
            if (errno == ERANGE || end == start || *end != '\0')
            {
                if (num_str[e_pos + 1] == '-')
                    return INT_MIN;
                return INT_MAX;
            }
            if (exp > INT_MAX - 1)
                return INT_MAX;
            if (exp < INT_MIN + 1)
                return INT_MIN;
            return static_cast<int>(exp) + 1;
        }
        size_t dot_pos = num_str.find('.');
        if (dot_pos != std::string::npos)
            return static_cast<int>(dot_pos);
        return static_cast<int>(num_str.length());
    }

    static std::string ReplaceRationalNumbers(const std::string& expr_str, int exp, int precision)
    {
        std::regex re(R"((\d+)/(\d+))");
        std::string result;
        size_t pos = 0;
        std::sregex_iterator it(expr_str.begin(), expr_str.end(), re);
        std::sregex_iterator end;
        for (; it != end; ++it)
        {
            std::smatch match = *it;
            result += expr_str.substr(pos, match.position() - pos);
            std::string num_str = match[1];
            std::string den_str = match[2];

            if (exp <= precision && num_str == "1")
            {
                result += match[0];
            }
            else
            {
                mpfr_t num, den, q;
                mpfr_init2(num, 512);
                mpfr_init2(den, 512);
                mpfr_init2(q, 512);
                mpfr_set_str(num, num_str.c_str(), 10, MPFR_RNDN);
                mpfr_set_str(den, den_str.c_str(), 10, MPFR_RNDN);
                mpfr_div(q, num, den, MPFR_RNDN);
                std::string s = MpfrFormat("%.*Rf", precision, q);
                s = FormatFixed(s, false);
                if (mpfr_zero_p(q) == 0 && IsEffectivelyZeroFixedStr(s))
                {
                    s = FormatScientific(MpfrFormat("%.*Re", precision, q));
                }
                result += s;
                mpfr_clear(num);
                mpfr_clear(den);
                mpfr_clear(q);
            }

            pos = match.position() + match.length();
        }
        result += expr_str.substr(pos);
        return result;
    }

    static std::string RemoveNumberParentheses(const std::string& expr_str)
    {
        std::regex re(R"(\((\d+(?:\.\d+)?(?:[eE][+-]?\d+)?)\))");
        return std::regex_replace(expr_str, re, "$1");
    }

    static std::string FormatNumberInExpression(const std::string& expr_str, int exp, int precision)
    {
        std::regex re(R"((\d+(\.\d+)?([eE][+-]?\d+)?))");
        std::string result;
        size_t pos = 0;
        std::sregex_iterator it(expr_str.begin(), expr_str.end(), re);
        std::sregex_iterator end;
        for (; it != end; ++it)
        {
            std::smatch match = *it;
            result += expr_str.substr(pos, match.position() - pos);
            std::string num_str = match[0];

            MpfrGuard guard;
            int order = GetDecimalOrder(num_str);
            mpfr_set_str(guard.num, num_str.c_str(), 10, MPFR_RNDN);
            if (order > exp || num_str.find_first_of("eE") != std::string::npos)
            {
                result += FormatScientific(MpfrFormat("%.*Re", precision, guard.num));
            }
            else
            {
                result += FormatFixed(MpfrFormat("%.*Rf", precision, guard.num), false);
            }

            pos = match.position() + match.length();
        }
        result += expr_str.substr(pos);
        return result;
    }

    static std::string ReplaceImaginaryUnit(const std::string& s, char imag_unit)
    {
        std::string result;
        for (size_t i = 0; i < s.size(); ++i)
        {
            if (s[i] == 'i')
            {
                bool boundary_before = (i == 0) ||
                    (!std::isalnum(static_cast<unsigned char>(s[i - 1])) && s[i - 1] != '.' && s[i - 1] != '_');
                bool boundary_after = (i + 1 == s.size()) ||
                    (!std::isalnum(static_cast<unsigned char>(s[i + 1])) && s[i + 1] != '.' && s[i + 1] != '_');
                if (boundary_before && boundary_after)
                {
                    result += imag_unit;
                    continue;
                }
            }
            result += s[i];
        }
        return result;
    }

public:
    static std::string JsonEscape(const std::string& s)
    {
        std::string r;
        for (char c : s)
        {
            if (c == '\\' || c == '"')
                r += '\\';
            r += c;
        }
        return r;
    }

    static std::string JsonCodeString(const std::string& text)
    {
        return std::string(R"({"type":8,"elements":")") + JsonEscape(text) + R"("})";
    }

    static std::string ImaginaryUnitName(Language language)
    {
        return language == Language::Russian ? "j" : "i";
    }

    static std::string JsonCodeRow(const std::vector<std::string>& items)
    {
        std::string s = R"({"type":7,"elements":[)";
        for (size_t i = 0; i < items.size(); ++i)
        {
            if (i > 0)
                s += ",";
            s += items[i];
        }
        s += "]}";
        return s;
    }

    static std::string JsonPlus()
    {
        return R"({"type":11,"symbol":"+"})";
    }

    static std::string JsonMinus()
    {
        return R"({"type":12,"symbol":"-"})";
    }

    static std::string JsonMultiply()
    {
        return R"({"type":13,"symbol":"·"})";
    }

    static std::string JsonShape()
    {
        return R"({"type":10,"elements":[]})";
    }

    static std::string JsonOpenRoundBracket()
    {
        return R"({"type":19,"symbol":"("})";
    }

    static std::string JsonCloseRoundBracket()
    {
        return R"xx({"type":20,"symbol":")"})xx";
    }

    static std::string JsonPower(const std::string& base, const std::string& exp)
    {
        return R"({"type":15,"elements":[)" + base + "," + JsonShape() + "," + exp + "]}";
    }

    static std::string JsonSquareRoot(const std::string& base)
    {
        return R"({"type":16,"elements":[{"type":10,"elements":[]},)" + base + "]}";
    }

    static std::string JsonNthRoot(const std::string& n, const std::string& base)
    {
        return R"({"type":17,"elements":[)" + n + "," + JsonShape() + "," + base + "]}";
    }

    static std::string JsonDivision(const std::string& num, const std::string& den)
    {
        return R"({"type":14,"elements":[)" + num + "," + JsonShape() + "," + den + "]}";
    }

public:
    static std::string JsonResultWrapper(int type, const std::string& content)
    {
        return R"({"type":)" + std::to_string(type) + R"(,"elements":[)" + content + "]}";
    }

    static std::string RemoveInsignificantPoint(const std::string& s)
    {
        if (s.size() < 2 || s.back() != '.')
            return s;
        for (size_t i = 0; i < s.size() - 1; ++i)
        {
            if (i == 0 && s[i] == '-')
                continue;
            if (!std::isdigit(static_cast<unsigned char>(s[i])))
                return s;
        }
        return s.substr(0, s.size() - 1);
    }

    static std::string MpfrFormat(const char* fmt, int precision, mpfr_srcptr value)
    {
        char* buf = nullptr;
        int n = mpfr_asprintf(&buf, fmt, precision, value);
        if (n < 0 || !buf)
            return {};
        std::string result(buf);
        mpfr_free_str(buf);
        return result;
    }

    giac::context* Context() const
    {
        return current_giac_context ? current_giac_context : &context;
    }

private:
    int precision = 0;
    mutable giac::context context;
    std::unique_ptr<giac::gen> expr;
    bool explicit_negative_infinity = false;
};

template<typename T>
struct is_symbolic : std::false_type {};

template<typename T>
struct is_symbolic<Symbolic<T>> : std::true_type {};

template<typename T>
inline constexpr bool is_symbolic_v = is_symbolic<T>::value;

}

#endif
