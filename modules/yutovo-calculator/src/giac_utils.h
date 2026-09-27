/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __GIAC_UTILS_H__
#define __GIAC_UTILS_H__

#include <string>
#include <vector>
#include <set>
#include <memory>
#include <optional>
#include <regex>
#include <cmath>
#include <limits>
#include <cctype>
#include <map>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <mutex>
#include <unistd.h>
#include <fcntl.h>

#ifdef alloca
#undef alloca
#endif

#include <giac/giac.h>
#include <giac/lin.h>
#include <giac/series.h>
#include "math_helper.h"
#include "parser_exception.h"
#include "real.h"
#include "complex.h"
#include "rational.h"
#include "array.h"

namespace yutovo_calculator
{

//Thread-local pointer to the current parser's giac context.
//When set, all Symbolic operations in this thread use this context instead of their own.
extern thread_local giac::context* current_giac_context;

//Global mutex protecting giac string parsing. Giac shares mutable state (e.g. single-letter identifier objects) between threads during parsing.
//After parsing identifiers are cloned so that subsequent operations are thread-local.
extern std::mutex giac_parsing_mutex;

//Global mutex protecting giac integration/evaluation. Giac is not thread-safe for concurrent calls to _integrate, _romberg, evalf and similar operations
//even when each thread uses its own giac::context.
extern std::mutex giac_evaluation_mutex;

struct FormatContext
{
    enum Mode
    {
        Real,
        Rational,
        Complex
    };

    Mode mode = Real;
    int precision = 0;
    int exp = 0;
    Language language = Language::English;
    bool json = false;
    bool in_product = false;
};

struct GiacExpression
{
    enum Kind
    {
        Number,
        Ident,
        Func,
        Sum,
        Product,
        Power,
        Neg,
        Div,
        Sqrt,
        Root
    };

    Kind kind = Number;
    std::string value;
    std::vector<GiacExpression> args;

    GiacExpression() = default;

    GiacExpression(Kind k, std::string v = {}) :
        kind(k),
        value(std::move(v))
    {
    }

    GiacExpression(Kind k, std::vector<GiacExpression> a) :
        kind(k),
        args(std::move(a))
    {
    }

    GiacExpression(Kind k, GiacExpression a) :
        kind(k)
    {
        args.push_back(std::move(a));
    }

    GiacExpression(Kind k, GiacExpression a, GiacExpression b) :
        kind(k)
    {
        args.push_back(std::move(a));
        args.push_back(std::move(b));
    }

    GiacExpression(Kind k, std::string v, std::vector<GiacExpression> a) :
        kind(k),
        value(std::move(v)),
        args(std::move(a))
    {
    }
};

std::string StripQuotes(const std::string& name);

class ExpressionParser
{
public:
    explicit ExpressionParser(const std::string& _str) :
        str(_str),
        pos(0)
    {
    }

    GiacExpression Parse()
    {
        GiacExpression result = ParseAddSub();
        SkipSpaces();
        return result;
    }

private:
    const std::string& str;
    size_t pos;

    void SkipSpaces()
    {
        while (pos < str.size() && std::isspace(static_cast<unsigned char>(str[pos])))
            ++pos;
    }

    char Peek()
    {
        SkipSpaces();
        if (pos >= str.size())
            return '\0';
        return str[pos];
    }

    bool Consume(char c)
    {
        SkipSpaces();
        if (pos < str.size() && str[pos] == c)
        {
            ++pos;
            return true;
        }
        return false;
    }

    GiacExpression ParseAddSub()
    {
        GiacExpression left = ParseMulDiv();
        while (true)
        {
            char c = Peek();
            if (c != '+' && c != '-')
                break;
            ++pos;
            GiacExpression right = ParseMulDiv();
            if (left.kind != GiacExpression::Sum)
            {
                GiacExpression sum(GiacExpression::Sum);
                sum.args.push_back(std::move(left));
                left = std::move(sum);
            }
            if (c == '-')
                left.args.emplace_back(GiacExpression::Neg, std::move(right));
            else
                left.args.push_back(std::move(right));
        }
        return left;
    }

    GiacExpression ParseMulDiv()
    {
        GiacExpression left = ParsePower();
        while (true)
        {
            char c = Peek();
            if (c != '*' && c != '/')
                break;
            ++pos;
            GiacExpression right = ParsePower();
            if (c == '/')
            {
                left = GiacExpression(GiacExpression::Div, std::move(left), std::move(right));
            }
            else
            {
                if (left.kind != GiacExpression::Product)
                {
                    GiacExpression prod(GiacExpression::Product);
                    prod.args.push_back(std::move(left));
                    left = std::move(prod);
                }
                left.args.push_back(std::move(right));
            }
        }
        return left;
    }

    GiacExpression ParsePower()
    {
        GiacExpression left = ParseUnary();
        if (Peek() == '^')
        {
            ++pos;
            GiacExpression right = ParseUnary();
            left = GiacExpression(GiacExpression::Power, std::move(left), std::move(right));
        }
        while (Consume('!'))
        {
            left = GiacExpression(GiacExpression::Func, "factorial", std::vector<GiacExpression>{std::move(left)});
        }
        return left;
    }

    GiacExpression ParseUnary()
    {
        char c = Peek();
        if (c == '+')
        {
            ++pos;
            return ParseUnary();
        }
        if (c == '-')
        {
            ++pos;
            GiacExpression operand = ParseUnary();
            if (operand.kind == GiacExpression::Number)
            {
                if (!operand.value.empty() && operand.value[0] == '-')
                    operand.value = operand.value.substr(1);
                else
                    operand.value = "-" + operand.value;
                return operand;
            }
            return GiacExpression(GiacExpression::Neg, std::move(operand));
        }
        return ParsePrimary();
    }

    GiacExpression ParsePrimary()
    {
        if (Peek() == '(')
        {
            ++pos;
            GiacExpression inside = ParseAddSub();
            Consume(')');
            return inside;
        }
        char c = Peek();
        if (c == '\0')
            return GiacExpression(GiacExpression::Number, "0");
        if (std::isdigit(static_cast<unsigned char>(c)) || c == '.')
            return ParseNumber();
        return ParseIdentOrFunc();
    }

    GiacExpression ParseNumber()
    {
        SkipSpaces();
        size_t start = pos;
        while (pos < str.size() && (std::isdigit(static_cast<unsigned char>(str[pos])) || str[pos] == '.'))
            ++pos;
        if (pos < str.size() && (str[pos] == 'e' || str[pos] == 'E'))
        {
            ++pos;
            if (pos < str.size() && (str[pos] == '+' || str[pos] == '-'))
                ++pos;
            while (pos < str.size() && std::isdigit(static_cast<unsigned char>(str[pos])))
                ++pos;
        }
        return GiacExpression(GiacExpression::Number, str.substr(start, pos - start));
    }

    GiacExpression ParseIdentOrFunc()
    {
        SkipSpaces();
        size_t start = pos;
        while (pos < str.size())
        {
            char c = str[pos];
            if (c == '(' || c == ')' || c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == ',' || c == '!' || std::isspace(static_cast<unsigned char>(c)))
                break;
            ++pos;
        }
        std::string name = StripQuotes(str.substr(start, pos - start));
        if (Consume('('))
        {
            std::vector<GiacExpression> args;
            if (Peek() != ')')
            {
                args.push_back(ParseAddSub());
                while (Consume(','))
                    args.push_back(ParseAddSub());
            }
            Consume(')');

            if (name == "pow" && args.size() == 2)
                return GiacExpression(GiacExpression::Power, std::move(args[0]), std::move(args[1]));
            if (name == "sqrt" && args.size() == 1)
                return GiacExpression(GiacExpression::Sqrt, std::move(args[0]));
            if (args.size() == 2 && name.size() > 3 &&
                name.compare(name.size() - 3, 3, "pow") == 0)
            {
                std::string base_name = name.substr(0, name.size() - 3);
                return GiacExpression(GiacExpression::Power,
                    GiacExpression(GiacExpression::Func, base_name, std::vector<GiacExpression>{std::move(args[0])}),
                    std::move(args[1]));
            }
            return GiacExpression(GiacExpression::Func, name, std::move(args));
        }
        return GiacExpression(GiacExpression::Ident, name);
    }
};

//Saves/restores MPFR default precision and rounding mode around giac calls. giac mutates these globals (e.g. via _integrate), 
//which breaks MPFR calculations.
class GiacMpfrStateGuard
{
public:
    GiacMpfrStateGuard() :
        prec(mpfr_get_default_prec()),
        rounding_mode(mpfr_get_default_rounding_mode())
    {
    }

    ~GiacMpfrStateGuard()
    {
        mpfr_set_default_prec(prec);
        mpfr_set_default_rounding_mode(rounding_mode);
    }

private:
    mpfr_prec_t prec;
    mpfr_rnd_t rounding_mode;
};

//Temporarily redirects stdout and stderr to /dev/null around giac calls, suppressing
//spurious diagnostic output (e.g. numeric integration progress comments).
class GiacOutputGuard
{
public:
    GiacOutputGuard() :
        stdout_copy(-1),
        stderr_copy(-1),
        active(false)
    {
        std::cout.flush();
        std::cerr.flush();
        stdout_copy = dup(STDOUT_FILENO);
        stderr_copy = dup(STDERR_FILENO);
        if (stdout_copy < 0 || stderr_copy < 0)
            return;
        int devnull = open("/dev/null", O_WRONLY);
        if (devnull < 0)
            return;
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
        close(devnull);
        active = true;
    }

    ~GiacOutputGuard()
    {
        if (!active)
            return;
        std::cout.flush();
        std::cerr.flush();
        if (stdout_copy >= 0)
        {
            dup2(stdout_copy, STDOUT_FILENO);
            close(stdout_copy);
        }
        if (stderr_copy >= 0)
        {
            dup2(stderr_copy, STDERR_FILENO);
            close(stderr_copy);
        }
    }

private:
    int stdout_copy;
    int stderr_copy;
    bool active;
};

template<typename Number>
Number FromGiac(const giac::gen& g, int precision);

inline Real RoundRealToDecimalDigits(const Real& value, int decimal_digits)
{
    if (value.IsZero() || value.IsInfinity() || value.IsNaN())
        return value;

    mpfr_srcptr src = value.GetNumber();
    mpfr_prec_t src_prec = mpfr_get_prec(src);

    mpfr_t absx, e, scale, scaled, result_mpfr;
    mpfr_init2(absx, src_prec);
    mpfr_init2(e, src_prec);
    mpfr_init2(scale, src_prec);
    mpfr_init2(scaled, src_prec);

    mpfr_abs(absx, src, MPFR_RNDN);
    mpfr_log10(e, absx, MPFR_RNDN);
    mpfr_floor(e, e);

    mpfr_set_si(scale, decimal_digits - 1, MPFR_RNDN);
    mpfr_sub(scale, scale, e, MPFR_RNDN);
    mpfr_ui_pow(scale, 10, scale, MPFR_RNDN);

    mpfr_mul(scaled, src, scale, MPFR_RNDN);
    mpfr_round(scaled, scaled);
    mpfr_div(scaled, scaled, scale, MPFR_RNDN);

    int target_bits = MathHelper::ToBitPrecision(decimal_digits + 2);
    if (target_bits < 1)
        target_bits = 1;

    mpfr_init2(result_mpfr, target_bits);
    mpfr_set(result_mpfr, scaled, MPFR_RNDN);

    Real result(result_mpfr);
    mpfr_clear(result_mpfr);
    mpfr_clear(scaled);
    mpfr_clear(scale);
    mpfr_clear(e);
    mpfr_clear(absx);
    return result;
}

template<>
inline Real FromGiac<Real>(const giac::gen& g, int precision)
{
    Real result;
    switch (g.type)
    {
    case giac::_REAL:
        result = Real(g._REALptr->inf);
        break;
    case giac::_DOUBLE_:
        result = Real(precision);
        result = g.DOUBLE_val();
        break;
    case giac::_INT_:
        result = Real(precision, g.val);
        break;
    case giac::_ZINT:
    {
        mpfr_t tmp;
        mpfr_init2(tmp, std::max((int)mpfr_get_default_prec(), precision));
        mpfr_set_z(tmp, *g._ZINTptr, MPFR_RNDN);
        result = Real(tmp);
        mpfr_clear(tmp);
        break;
    }
    case giac::_CPLX:
        result = FromGiac<Real>(g._CPLXptr[0], precision);
        break;
    default:
        if (g == giac::plus_inf || g == giac::unsigned_inf)
        {
            result = Real(precision, "inf");
            break;
        }
        if (g == giac::minus_inf)
        {
            result = Real(precision, "-inf");
            break;
        }
        throw MathException(IncorrectOperation);
    }

    return RoundRealToDecimalDigits(result, precision + 1);
}

template<>
inline Complex FromGiac<Complex>(const giac::gen& g, int precision)
{
    if (g.type == giac::_CPLX)
        return Complex(FromGiac<Real>(g._CPLXptr[0], precision), FromGiac<Real>(g._CPLXptr[1], precision));
    return Complex(FromGiac<Real>(g, precision));
}

template<>
inline Rational FromGiac<Rational>(const giac::gen& g, int precision)
{
    if (g.type == giac::_FRAC)
    {
        auto set_mpz = 
            [](mpz_t dest, const giac::gen& src)
            {
                if (src.type == giac::_INT_)
                    mpz_set_si(dest, src.val);
                else if (src.type == giac::_ZINT)
                    mpz_set(dest, *src._ZINTptr);
                else
                    throw MathException(IncorrectOperation);
            };

        mpz_t num, den;
        mpz_init(num);
        mpz_init(den);
        set_mpz(num, g._FRACptr->num);
        set_mpz(den, g._FRACptr->den);
        Rational result(num, den);
        mpz_clear(num);
        mpz_clear(den);
        return result;
    }

    if (g.type == giac::_INT_)
        return Rational(g.val);

    if (g.type == giac::_ZINT)
    {
        mpz_t one;
        mpz_init_set_si(one, 1);
        Rational result(*g._ZINTptr, one);
        mpz_clear(one);
        return result;
    }

    throw MathException(IncorrectOperation);
}

template<>
inline Array<Real> FromGiac<Array<Real>>(const giac::gen& g, int precision)
{
    Array<Real> result;
    if (g.type == giac::_VECT)
    {
        const giac::vecteur& v = *g._VECTptr;
        for (const auto& elem : v)
            result.Add(FromGiac<Real>(elem, precision));
    }
    else
    {
        result.Add(FromGiac<Real>(g, precision));
    }
    return result;
}

inline giac::gen InertCall(const char* name, const giac::gen& arg, giac::context* contextptr)
{
    std::string full = std::string("yut_") + name;
    giac::gen id(full.c_str(), contextptr);
    return giac::symb_of(id, arg);
}

giac::gen ParseGen(const char* str, const giac::context* ctx);
giac::gen ParseGen(const std::string& str, const giac::context* ctx);

//Convert a numeric value to a giac::gen without string round-tripping.
giac::gen NumberToGiac(const Real& value);
giac::gen NumberToGiac(const Rational& value);
giac::gen NumberToGiac(const Complex& value);

//Return a deep copy of g where each single-letter variable identifier (a..d, f..h, j..z) is replaced by a freshly allocated identificateur.
//This avoids sharing giac's global one-letter identifier objects between threads.
giac::gen CloneSingleLetterIdentifiers(const giac::gen& g);

//Freeze ref_count of all identifiers currently in giac's global syms() table to -1.
//This makes the global single-letter identifiers (x, y, z, ...) and any other already
//registered identifiers effectively immutable for reference-counting purposes, so they
//can be copied/destroyed concurrently without racing on ref_count.
void FreezeStaticGiacIdentifiers();

bool HasAmbiguousPoleArgument(const giac::gen& expr, const giac::identificateur& var, const giac::gen& value, giac::context* ctx);
bool IsInfiniteLimit(const giac::gen& lim, giac::context* ctx);
bool HasUnknownSymbol(const giac::gen& g);
bool IsPower(const giac::gen& g, giac::gen& base, giac::gen& exp);
giac::gen SimplifyPowerDivision(const giac::gen& n, const giac::gen& d, giac::context* ctx);
bool IsKnownConstant(const std::string& name);
bool IsExplicitNegativeInfinity(const giac::gen& g);

std::string GiacToString(const giac::gen& g, giac::context* ctx);
std::string RoundScientificHalfUp(std::string str, int decimals);
std::string RealNumberStr(const std::string& num_str, int precision, int exp);
std::string AddDotIfInteger(const std::string& str);
std::string StripStandaloneIntegerDot(std::string str);
std::string StripQuotes(const std::string& name);

bool IsNumberString(const std::string& v);
bool IsIntegerString(const std::string& v);
std::string StripDot(const std::string& v);
bool IsOne(const std::string& v);
bool IsNumericFactor(const GiacExpression& e);
std::string ReciprocalDecimal(const std::string& den, int precision, int exp);
bool IsPolynomial(const GiacExpression& e);
bool IsKnownSymbol(const std::string& name);
bool IsVariableIdent(const std::string& name);
constexpr int NON_POLY_DEGREE = 100;
int TermDegree(const GiacExpression& e);
bool IsConstantExpr(const GiacExpression& e);
std::string MapSpecialIdent(const std::string& name);
std::string MapFuncName(const std::string& name);
std::string EmitString(const GiacExpression& e, const FormatContext& ctx);
std::string EvaluateGiacExpression(const std::string& expr, int precision);
std::string StripTrailingDotZeros(std::string str);
std::string EmitNumber(const std::string& value, const FormatContext& ctx);
bool NeedsParensForPowerBase(const GiacExpression& base, const GiacExpression& exp);
bool NeedsParensForPowerExp(const GiacExpression& e);
std::string EmitFuncArgs(const GiacExpression& func, const FormatContext& ctx);
void ReorderFactors(GiacExpression& prod);
std::string TermSortKey(const GiacExpression& e, const FormatContext& ctx);
int FunctionSortRank(const std::string& text);
void SortTerms(GiacExpression& sum, const FormatContext& ctx);
std::string AddCoeffs(const std::string& a, const std::string& b, const FormatContext& ctx);
std::string MultiplyCoeffs(const std::string& a, const std::string& b, const FormatContext& ctx);
void CombineLikeTerms(GiacExpression& sum, const FormatContext& ctx);
bool IsConstantPower(const GiacExpression& e);
std::string EvaluateConstantPower(const GiacExpression& base, const GiacExpression& exp, const FormatContext& ctx);
long long IntegerNthRootExact(long long base, int n);
bool IsArithmeticConstantExpr(const GiacExpression& e);
bool IsInertFunction(const std::string& name);
std::string InertFunctionExpr(const std::string& name, const std::string& arg);
bool IsGiacExpressionZero(const std::string& expr, int precision);
std::string ExactInertArg(std::string arg);
bool IsInverseHyperbolicInertSingularity(const std::string& name, const std::string& arg, int precision);
std::optional<GiacExpression> InertFunctionSingularity(const std::string& name, const std::string& arg, int precision);
GiacExpression ExprFromGiacResult(std::string str);
bool ContainsFunctionCall(const GiacExpression& e);
bool ContainsImaginaryUnit(const GiacExpression& e);
GiacExpression EvaluateArithmeticExpr(GiacExpression e, const FormatContext& ctx);
bool IsNegativeNumberString(const std::string& str);
GiacExpression NormalizeNegativeProductTerm(GiacExpression e);
bool HasNegativeConstantTerm(const GiacExpression& e);
GiacExpression NegateSum(GiacExpression e);
long long GcdLL(long long a, long long b);
long long SquarefreePartLL(long long n);
std::optional<GiacExpression> TryFactorSqrtSum(const GiacExpression& sum);
GiacExpression Transform(GiacExpression e, const FormatContext& ctx);
GiacExpression TransformChildren(GiacExpression e, const FormatContext& ctx);
GiacExpression BuildFormattedAst(std::string str, const FormatContext& ctx);
std::string FormatGiacString(std::string str, const FormatContext& ctx);

std::string JsonComma();
std::vector<std::string> JsonNumberRaw(const std::string& value, const FormatContext& ctx);
bool IsNegativeNumberValue(const std::string& value);
std::vector<std::string> EmitJson(const GiacExpression& e, const FormatContext& ctx);
bool IsCodeStringJson(const std::string& str);
std::vector<std::string> EmitJsonChildren(const std::vector<GiacExpression>& args, const FormatContext& ctx);
std::string ExprToJson(const GiacExpression& ast, int result_type, const FormatContext& ctx);

}

#endif
