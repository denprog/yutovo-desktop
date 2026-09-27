/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "giac_utils.h"
#include "parser_exception.h"
#include "symbolic.h"
#include "real.h"
#include "rational.h"
#include "complex.h"
#include "export.h"
#include <giac/input_lexer.h>
#include <mutex>

namespace yutovo_calculator
{

thread_local giac::context* current_giac_context = nullptr;
std::mutex giac_parsing_mutex;
std::mutex giac_evaluation_mutex;
std::once_flag freeze_static_ids_once;

class GiacSymbolsLock
{
public:
    GiacSymbolsLock()
    {
        giac::lock_syms_mutex();
    }

    ~GiacSymbolsLock()
    {
        giac::unlock_syms_mutex();
    }
};

bool IsClonableSingleLetterIdentifier(const char* name)
{
    if (!name || !name[0] || name[1])
        return false;
    char c = name[0];
    if (c < 'a' || c > 'z')
        return false;
    //keep giac's single-letter constants as shared global objects
    if (c == 'e' || c == 'i')
        return false;
    return true;
}

giac::gen ParseGen(const char* str, const giac::context* ctx)
{
    std::lock_guard<std::mutex> lock(giac_parsing_mutex);
    std::call_once(freeze_static_ids_once, FreezeStaticGiacIdentifiers);
    return CloneSingleLetterIdentifiers(giac::gen(str, ctx));
}

giac::gen ParseGen(const std::string& str, const giac::context* ctx)
{
    return ParseGen(str.c_str(), ctx);
}

giac::gen NumberToGiac(const Real& value)
{
    mpfr_t r;
    mpfr_init2(r, mpfr_get_prec(value.GetNumber()));
    mpfr_set(r, value.GetNumber(), MPFR_RNDN);
    giac::gen result = giac::gen(giac::real_object(r));
    mpfr_clear(r);
    return result;
}

giac::gen NumberToGiac(const Rational& value)
{
    mpq_t q;
    mpq_init(q);
    mpq_set(q, value.GetNumber());
    mpz_t num_z, den_z;
    mpz_init_set(num_z, mpq_numref(q));
    mpz_init_set(den_z, mpq_denref(q));
    mpq_clear(q);
    giac::gen num = giac::gen(num_z);
    giac::gen den = giac::gen(den_z);
    mpz_clear(num_z);
    mpz_clear(den_z);
    return giac::gen(giac::fraction(num, den));
}

giac::gen NumberToGiac(const Complex& value)
{
    giac::gen re = NumberToGiac(value.GetRe());
    giac::gen im = NumberToGiac(value.GetIm());
    return giac::gen(re, im);
}


giac::gen CloneSingleLetterIdentifiers(const giac::gen& g)
{
    if (g.type == giac::_IDNT)
    {
        const char* name = g._IDNTptr->id_name;
        if (IsClonableSingleLetterIdentifier(name))
        {
            giac::identificateur id(name);
            return giac::gen(id);
        }
        return g;
    }
    if (g.type == giac::_SYMB)
    {
        giac::gen new_feuille = CloneSingleLetterIdentifiers(g._SYMBptr->feuille);
        return giac::symbolic(g._SYMBptr->sommet, new_feuille);
    }
    if (g.type == giac::_VECT)
    {
        const giac::vecteur& v = *g._VECTptr;
        giac::vecteur res;
        res.reserve(v.size());
        for (const auto& x : v)
            res.push_back(CloneSingleLetterIdentifiers(x));
        return giac::gen(res, g.subtype);
    }
    if (g.type == giac::_FRAC)
    {
        giac::gen num = CloneSingleLetterIdentifiers(g._FRACptr->num);
        giac::gen den = CloneSingleLetterIdentifiers(g._FRACptr->den);
        return giac::gen(giac::fraction(num, den));
    }
    return g;
}

void FreezeStaticGiacIdentifiers()
{
    GiacSymbolsLock lock;
    //ensure the global single-letter identifiers are registered in syms() before freezing
    for (char c = 'a'; c <= 'z'; ++c)
    {
        std::string name(1, c);
        giac::gen(name, giac::context0);
    }
    giac::sym_string_tab& table = giac::syms();
    for (auto& entry : table)
    {
        giac::gen& g = entry.second;
        if (g.type == giac::_IDNT)
            g.ref_count() = -1;
    }
}

bool IsKnownConstant(const std::string& name)
{
    static const std::set<std::string> known = {"pi", "e", "oo", "inf", "+inf", "-inf", "nan", "undef"};
    return known.find(name) != known.end();
}

bool HasUnknownSymbol(const giac::gen& g)
{
    switch (g.type)
    {
    case giac::_IDNT:
        return !IsKnownConstant(g._IDNTptr->id_name);
    case giac::_SYMB:
        return HasUnknownSymbol(g._SYMBptr->feuille);
    case giac::_CPLX:
        if (!(g._CPLXptr[1] == 0))
            return true;
        return HasUnknownSymbol(g._CPLXptr[0]);
    case giac::_FRAC:
        return HasUnknownSymbol(g._FRACptr->num) || HasUnknownSymbol(g._FRACptr->den);
    case giac::_VECT:
    {
        const giac::vecteur& v = *g._VECTptr;
        for (const auto& x : v)
        {
            if (HasUnknownSymbol(x))
                return true;
        }
        return false;
    }
    default:
        return false;
    }
}

bool IsPower(const giac::gen& g, giac::gen& base, giac::gen& exp)
{
    if (g.type == giac::_SYMB && g._SYMBptr->sommet == giac::at_pow)
    {
        const giac::gen& f = g._SYMBptr->feuille;
        if (f.type == giac::_VECT && f._VECTptr->size() == 2)
        {
            base = (*f._VECTptr)[0];
            exp = (*f._VECTptr)[1];
            return true;
        }
    }
    return false;
}

giac::gen SimplifyPowerDivision(const giac::gen& n, const giac::gen& d, giac::context* ctx)
{
    giac::gen n_base, n_exp, d_base, d_exp;
    bool n_is_pow = IsPower(n, n_base, n_exp);
    bool d_is_pow = IsPower(d, d_base, d_exp);
    if (n_is_pow && d_is_pow && n_base == d_base)
        return giac::pow(n_base, giac::operator_minus(n_exp, d_exp, ctx), ctx);
    if (n_is_pow && !d_is_pow && n_base == d)
        return giac::pow(n_base, giac::operator_minus(n_exp, giac::gen(1), ctx), ctx);
    if (!n_is_pow && d_is_pow && d_base == n)
        return giac::pow(d_base, giac::operator_minus(giac::gen(1), d_exp, ctx), ctx);
    return giac::rdiv(n, d, ctx);
}

std::string GiacToString(const giac::gen& g, giac::context* ctx)
{
    return g.print(ctx);
}

std::string RoundScientificHalfUp(std::string str, int decimals)
{
    size_t e_pos = str.find_first_of("eE");
    if (e_pos == std::string::npos)
        return str;
    std::string mantissa = str.substr(0, e_pos);
    std::string exp_str = str.substr(e_pos);
    bool neg = !mantissa.empty() && mantissa[0] == '-';
    if (neg)
        mantissa = mantissa.substr(1);
    size_t dot = mantissa.find('.');
    if (dot == std::string::npos)
        return str;
    std::string digits = mantissa.substr(0, dot) + mantissa.substr(dot + 1);
    int total_decimals = static_cast<int>(mantissa.size() - dot - 1);
    int drop = total_decimals - decimals;
    if (drop <= 0)
        return str;
    std::string rounded = digits.substr(0, digits.size() - static_cast<size_t>(drop));
    char round_digit = digits[digits.size() - static_cast<size_t>(drop)];
    if (round_digit >= '5')
    {
        int i = static_cast<int>(rounded.size()) - 1;
        bool carry = true;
        while (i >= 0 && carry)
        {
            if (rounded[i] == '9')
                rounded[i] = '0';
            else
            {
                ++rounded[i];
                carry = false;
            }
            --i;
        }
        if (carry)
            rounded.insert(rounded.begin(), '1');
    }

    std::string result = rounded.substr(0, rounded.size() - static_cast<size_t>(decimals)) + "." + rounded.substr(rounded.size() - static_cast<size_t>(decimals));
    while (!result.empty() && result.back() == '0')
        result.pop_back();
    if (!result.empty() && result.back() == '.')
        result.pop_back();
    return (neg ? "-" : "") + result + exp_str;
}

std::string RealNumberStr(const std::string& num_str, int precision, int exp)
{
    std::string raw = num_str;
    if (!raw.empty() && raw.back() == '.')
        raw.pop_back();
    if (raw == "inf" || raw == "+inf" || raw == "+infinity" || raw == "infinity")
        return "∞";
    if (raw == "-inf" || raw == "-infinity")
        return "-∞";
    if (raw == "nan" || raw == "undef")
        return "nan";

    int order = Symbolic<Real>::GetDecimalOrder(raw);
    bool is_integer_string = raw.find('.') == std::string::npos && raw.find_first_of("eE") == std::string::npos;
    bool can_use_double = false;
    double d = 0.0;
    if (precision <= 15)
    {
        try
        {
            size_t idx = 0;
            d = std::stod(raw, &idx);
            if (idx == raw.size())
            {
                //doubles cannot exactly represent very large integers
                if (!is_integer_string || std::fabs(d) <= 9e15)
                    can_use_double = true;
            }
        }
        catch (...)
        {
        }
    }

    std::string str;
    if (can_use_double)
    {
        std::ostringstream oss;
        if (order > exp && exp >= 0)
        {
            oss << std::scientific << std::setprecision(precision) << d;
            str = Symbolic<Real>::FormatScientific(oss.str());
        }
        else
        {
            oss << std::fixed << std::setprecision(precision) << d;
            str = Symbolic<Real>::FormatFixed(oss.str(), false);
            if (d != 0.0 && Symbolic<Real>::IsEffectivelyZeroFixedStr(str))
            {
                oss.str("");
                oss.clear();
                oss << std::scientific << std::setprecision(precision) << d;
                str = Symbolic<Real>::FormatScientific(oss.str());
            }
        }
    }
    else
    {
        Symbolic<Real>::MpfrGuard guard;
        mpfr_set_str(guard.num, raw.c_str(), 10, MPFR_RNDN);
        if (order > exp && exp >= 0)
        {
            str = Symbolic<Real>::FormatScientific(Symbolic<Real>::MpfrFormat("%.*Re", precision, guard.num));
        }
        else
        {
            str = Symbolic<Real>::FormatFixed(Symbolic<Real>::MpfrFormat("%.*Rf", precision, guard.num), false);
            if (mpfr_zero_p(guard.num) == 0 && Symbolic<Real>::IsEffectivelyZeroFixedStr(str))
                str = Symbolic<Real>::FormatScientific(Symbolic<Real>::MpfrFormat("%.*Re", precision, guard.num));
        }
    }
    return str;
}

std::string AddDotIfInteger(const std::string& str)
{
    if (str.empty() || str == "-" || str.find('.') != std::string::npos || str.find('e') != std::string::npos || str.find('E') != std::string::npos ||
        str == "∞" || str == "-∞" || str == "nan")
        return str;
    bool is_integer = std::all_of(str.begin(), str.end(), 
        [](char c)
        {
            return std::isdigit(static_cast<unsigned char>(c)) || c == '-';
        }
        ) && !(str.size() == 1 && str[0] == '-');
    if (is_integer)
    {
        long long v = 0;
        try { v = std::stoll(str); } catch (...) { v = 0; }
        if (std::llabs(v) >= 1000)
            return str;
    }
    return str + ".";
}

std::string StripStandaloneIntegerDot(std::string str)
{
    if (str.empty() || str.back() != '.')
        return str;
    size_t i = 0;
    if (str[0] == '-')
        ++i;
    if (i >= str.size() - 1)
        return str;
    for (; i < str.size() - 1; ++i)
    {
        if (!std::isdigit(static_cast<unsigned char>(str[i])))
            return str;
    }
    str.pop_back();
    return str;
}

std::string StripQuotes(const std::string& name)
{
    std::string n = name;
    while (!n.empty() && (n.front() == '\'' || n.front() == '"'))
        n.erase(n.begin());
    while (!n.empty() && (n.back() == '\'' || n.back() == '"'))
        n.pop_back();
    return n;
}

bool IsNumberString(const std::string& v)
{
    static const std::regex re(R"(-?\d+(\.\d+)?([eE][+-]?\d+)?)");
    return std::regex_match(v, re);
}

bool IsIntegerString(const std::string& v)
{
    static const std::regex re(R"(-?\d+)");
    return std::regex_match(v, re);
}

std::string StripDot(const std::string& v)
{
    if (!v.empty() && v.back() == '.')
        return v.substr(0, v.size() - 1);
    return v;
}

bool IsOne(const std::string& v)
{
    std::string t = StripDot(v);
    return t == "1" || t == "+1";
}

bool IsNumericFactor(const GiacExpression& e)
{
    return e.kind == GiacExpression::Number && (IsNumberString(e.value) || e.value.find('/') != std::string::npos);
}

std::string ReciprocalDecimal(const std::string& den, int precision, int exp)
{
    Symbolic<Real>::MpfrGuard guard;
    mpfr_t den_mpfr, q;
    mpfr_init2(den_mpfr, 512);
    mpfr_init2(q, 512);
    mpfr_set_str(den_mpfr, den.c_str(), 10, MPFR_RNDN);
    mpfr_ui_div(q, 1, den_mpfr, MPFR_RNDN);

    double qd = mpfr_get_d(q, MPFR_RNDN);
    int order = 0;
    if (qd != 0.0 && std::isfinite(qd))
        order = static_cast<int>(std::floor(std::log10(std::fabs(qd)))) + 1;

    std::string result;
    if (order > exp && exp >= 0)
        result = Symbolic<Real>::FormatScientific(Symbolic<Real>::MpfrFormat("%.*Re", precision, q));
    else
        result = Symbolic<Real>::FormatFixed(Symbolic<Real>::MpfrFormat("%.*Rf", precision, q), false);

    if (mpfr_zero_p(q) == 0 && Symbolic<Real>::IsEffectivelyZeroFixedStr(result))
        result = Symbolic<Real>::FormatScientific(Symbolic<Real>::MpfrFormat("%.*Re", precision, q));

    mpfr_clear(den_mpfr);
    mpfr_clear(q);
    return result;
}

bool IsPolynomial(const GiacExpression& e)
{
    switch (e.kind)
    {
    case GiacExpression::Number:
        return true;
    case GiacExpression::Ident:
        return true;
    case GiacExpression::Power:
        return e.args.size() == 2 && IsPolynomial(e.args[0]) && e.args[1].kind == GiacExpression::Number && IsIntegerString(StripDot(e.args[1].value));
    case GiacExpression::Product:
    case GiacExpression::Sum:
        for (const auto& a : e.args)
        {
            if (!IsPolynomial(a))
                return false;
        }
        return true;
    case GiacExpression::Neg:
        return IsPolynomial(e.args[0]);
    case GiacExpression::Root:
        return false;
    default:
        return false;
    }
}

bool IsKnownSymbol(const std::string& name)
{
    static const std::set<std::string> known = {"i", "j", "e", "pi", "oo", "inf", "nan", "undef"};
    return known.find(name) != known.end();
}

bool IsVariableIdent(const std::string& name)
{
    if (name.size() == 1 && std::isalpha(static_cast<unsigned char>(name[0])))
        return !IsKnownSymbol(name);
    return false;
}

int TermDegree(const GiacExpression& e)
{
    switch (e.kind)
    {
    case GiacExpression::Number:
        return 0;
    case GiacExpression::Ident:
        return IsVariableIdent(e.value) ? 1 : 0;
    case GiacExpression::Power:
    {
        int base_deg = TermDegree(e.args[0]);
        if (base_deg == 0)
            return 0;
        if (e.args[1].kind == GiacExpression::Number && IsIntegerString(StripDot(e.args[1].value)))
        {
            int exp_val = std::stoi(StripDot(e.args[1].value));
            return base_deg * exp_val;
        }
        return NON_POLY_DEGREE;
    }
    case GiacExpression::Product:
    case GiacExpression::Sum:
    {
        int d = 0;
        for (const auto& a : e.args)
            d += TermDegree(a);
        return d;
    }
    case GiacExpression::Neg:
        return TermDegree(e.args[0]);
    case GiacExpression::Div:
    {
        int num_deg = TermDegree(e.args[0]);
        int den_deg = TermDegree(e.args[1]);
        if (den_deg == 0)
            return num_deg;
        if (num_deg == 0)
            return NON_POLY_DEGREE;
        return num_deg; //keep polynomial degree for simple denominators
    }
    case GiacExpression::Func:
    case GiacExpression::Sqrt:
    case GiacExpression::Root:
        return NON_POLY_DEGREE;
    default:
        return NON_POLY_DEGREE;
    }
}

bool IsConstantExpr(const GiacExpression& e)
{
    switch (e.kind)
    {
    case GiacExpression::Number:
        return true;
    case GiacExpression::Ident:
        return IsKnownSymbol(e.value);
    case GiacExpression::Func:
    case GiacExpression::Sqrt:
    {
        for (const auto& a : e.args)
        {
            if (!IsConstantExpr(a))
                return false;
        }
        return true;
    }
    case GiacExpression::Power:
    case GiacExpression::Div:
    case GiacExpression::Neg:
        if (e.args.empty())
            return true;
        return IsConstantExpr(e.args[0]) && (e.args.size() < 2 || IsConstantExpr(e.args[1]));
    case GiacExpression::Sum:
    case GiacExpression::Product:
    {
        for (const auto& a : e.args)
            if (!IsConstantExpr(a))
                return false;
        return true;
    }
    case GiacExpression::Root:
        return false;
    default:
        return false;
    }
}

std::string MapSpecialIdent(const std::string& name)
{
    std::string n = StripQuotes(name);
    if (n == "inf" || n == "infinity" || n == "oo")
        return "∞";
    if (n == "undef")
        return "nan";
    if (n == "pi")
        return "π";
    return n;
}

std::string MapFuncName(const std::string& name)
{
    std::string n = StripQuotes(name);
    if (n == "Gamma" || n == "GAMMA")
        return "gamma";
    if (n == "yut_cot")
        return "cot";
    if (n == "yut_sec")
        return "sec";
    if (n == "yut_csc")
        return "csc";
    if (n == "yut_coth")
        return "coth";
    if (n == "yut_sech")
        return "sech";
    if (n == "yut_csch")
        return "csch";
    if (n == "yut_asinh")
        return "asinh";
    if (n == "yut_acosh")
        return "acosh";
    if (n == "yut_atanh")
        return "atanh";
    if (n == "yut_acoth")
        return "acoth";
    if (n == "yut_asech")
        return "asech";
    if (n == "yut_acsch")
        return "acsch";
    return n;
}

std::string EmitString(const GiacExpression& e, const FormatContext& ctx);
std::string EvaluateGiacExpression(const std::string& expr, int precision);

std::string StripTrailingDotZeros(std::string str)
{
    if (str.find('.') == std::string::npos)
        return str;
    while (!str.empty() && str.back() == '0')
        str.pop_back();
    if (!str.empty() && str.back() == '.')
        str.pop_back();
    return str.empty() ? "0" : str;
}

std::string EmitNumber(const std::string& value, const FormatContext& ctx)
{
    if (ctx.mode == FormatContext::Rational)
        return StripTrailingDotZeros(value);
    return AddDotIfInteger(RealNumberStr(value, ctx.precision, ctx.exp));
}

bool NeedsParensForPowerBase(const GiacExpression& base, const GiacExpression& exp)
{
    (void)exp;
    if (base.kind == GiacExpression::Number || base.kind == GiacExpression::Ident ||
        base.kind == GiacExpression::Func || base.kind == GiacExpression::Sqrt)
        return false;
    if (base.kind == GiacExpression::Product || base.kind == GiacExpression::Sum ||
        base.kind == GiacExpression::Power)
        return false;
    return true;
}

bool NeedsParensForPowerExp(const GiacExpression& e)
{
    if (e.kind == GiacExpression::Number || e.kind == GiacExpression::Ident ||
        e.kind == GiacExpression::Func || e.kind == GiacExpression::Sqrt)
        return false;
    if (e.kind == GiacExpression::Neg && e.args[0].kind == GiacExpression::Number)
        return false;
    return true;
}

std::string EmitFuncArgs(const GiacExpression& func, const FormatContext& ctx)
{
    std::string result;
    for (size_t i = 0; i < func.args.size(); ++i)
    {
        if (i > 0)
            result += ",";
        result += EmitString(func.args[i], ctx);
    }
    return result;
}

std::string EmitString(const GiacExpression& e, const FormatContext& ctx)
{
    switch (e.kind)
    {
    case GiacExpression::Number:
        return EmitNumber(e.value, ctx);
    case GiacExpression::Ident:
        return MapSpecialIdent(e.value);
    case GiacExpression::Func:
    {
        std::string name = MapFuncName(e.value);
        if (name == "ln" && ctx.language != Language::Russian)
            name = "log";
        if ((name == "factorial") && e.args.size() == 1)
        {
            //x! is rendered as gamma(1+x)
            std::string arg = EmitString(e.args[0], ctx);
            return "gamma(1.+" + arg + ")";
        }
        //in text output, log/ln of a single numeric argument omits parentheses
        if ((name == "log" || name == "ln") && e.args.size() == 1 && e.args[0].kind == GiacExpression::Number && ctx.mode != FormatContext::Rational)
        {
            return name + EmitString(e.args[0], ctx);
        }
        if (name == "exp" && e.args.size() == 1 && e.args[0].kind == GiacExpression::Number && IsOne(e.args[0].value))
            return "e";
        std::string str = name + "(";
        for (size_t i = 0; i < e.args.size(); ++i)
        {
            if (i > 0)
                str += ",";
            str += EmitString(e.args[i], ctx);
        }
        str += ")";
        return str;
    }
    case GiacExpression::Sqrt:
        return "sqrt(" + EmitString(e.args[0], ctx) + ")";
    case GiacExpression::Root:
    {
        std::string base_str = EmitString(e.args[0], ctx);
        std::string exp_val = EvaluateGiacExpression("1/" + e.args[1].value, ctx.precision);
        return "pow(" + base_str + "," + EmitNumber(exp_val, ctx) + ")";
    }
    case GiacExpression::Power:
    {
        std::string exp_str = EmitString(e.args[1], ctx);
        if (e.args[1].kind == GiacExpression::Number && IsOne(e.args[1].value))
            return EmitString(e.args[0], ctx);
        //preserve explicit right-associative chains as pow(base,exp1**exp2)
        if (e.args[1].kind == GiacExpression::Power)
        {
            const GiacExpression& inner = e.args[1];
            bool inner_simple = (inner.args[0].kind == GiacExpression::Ident || inner.args[0].kind == GiacExpression::Number) &&
                (inner.args[1].kind == GiacExpression::Ident || inner.args[1].kind == GiacExpression::Number);
            if (inner_simple)
            {
                std::string base_str = EmitString(e.args[0], ctx);
                if (NeedsParensForPowerBase(e.args[0], e.args[1]))
                    base_str = "(" + base_str + ")";
                return "pow(" + base_str + "," +
                    EmitString(inner.args[0], ctx) + "**" + EmitString(inner.args[1], ctx) + ")";
            }
        }
        std::string base_str = EmitString(e.args[0], ctx);
        if (NeedsParensForPowerBase(e.args[0], e.args[1]))
            base_str = "(" + base_str + ")";
        if (NeedsParensForPowerExp(e.args[1]))
            exp_str = "(" + exp_str + ")";
        return "pow(" + base_str + "," + exp_str + ")";
    }
    case GiacExpression::Div:
    {
        std::string num = EmitString(e.args[0], ctx);
        std::string den = EmitString(e.args[1], ctx);
        if (e.args[0].kind == GiacExpression::Sum)
            num = "(" + num + ")";
        if (e.args[1].kind == GiacExpression::Sum)
            den = "(" + den + ")";
        return num + "/" + den;
    }
    case GiacExpression::Neg:
    {
        std::string operand = EmitString(e.args[0], ctx);
        if (e.args[0].kind == GiacExpression::Sum || e.args[0].kind == GiacExpression::Div || e.args[0].kind == GiacExpression::Power)
            operand = "(" + operand + ")";
        return "-" + operand;
    }
    case GiacExpression::Sum:
    {
        std::string str = EmitString(e.args[0], ctx);
        for (size_t i = 1; i < e.args.size(); ++i)
        {
            if (e.args[i].kind == GiacExpression::Neg)
                str += "-" + EmitString(e.args[i].args[0], ctx);
            else
                str += "+" + EmitString(e.args[i], ctx);
        }
        return str;
    }
    case GiacExpression::Product:
    {
        auto emit_factor = 
            [&](const GiacExpression& f) -> std::string
            {
                std::string fs = EmitString(f, ctx);
                if (f.kind == GiacExpression::Sum || f.kind == GiacExpression::Div || f.kind == GiacExpression::Neg ||
                    (ctx.mode == FormatContext::Rational && f.kind == GiacExpression::Number && f.value.find('/') != std::string::npos))
                    fs = "(" + fs + ")";
                return fs;
            };
        std::string str = emit_factor(e.args[0]);
        for (size_t i = 1; i < e.args.size(); ++i)
            str += "*" + emit_factor(e.args[i]);
        return str;
    }
    }
    return {};
}

void ReorderFactors(GiacExpression& prod)
{
    std::vector<GiacExpression> numeric;
    std::vector<GiacExpression> other;
    size_t n = prod.args.size();
    numeric.reserve(n);
    other.reserve(n);
    for (size_t i = 0; i < n; ++i)
    {
        if (IsNumericFactor(prod.args[i]))
            numeric.push_back(std::move(prod.args[i]));
        else
            other.push_back(std::move(prod.args[i]));
    }
    prod.args.clear();
    for (auto& x : numeric)
        prod.args.push_back(std::move(x));
    for (auto& x : other)
        prod.args.push_back(std::move(x));
}

std::string TermSortKey(const GiacExpression& e, const FormatContext& ctx)
{
    if (e.kind == GiacExpression::Product)
    {
        //ignore numeric/constant coefficients when ordering terms
        std::string key;
        for (const auto& f : e.args)
        {
            if (IsNumericFactor(f))
                continue;
            if (IsConstantExpr(f))
                continue;
            key += EmitString(f, ctx);
        }
        return key;
    }
    if (e.kind == GiacExpression::Neg)
        return TermSortKey(e.args[0], ctx);
    if (e.kind == GiacExpression::Number)
        return "";
    return EmitString(e, ctx);
}

int FunctionSortRank(const std::string& text)
{
    //canonical function ordering used by SymEngine for same-degree terms
    static const std::vector<std::string> order = 
        {
            "sin", "cos", "tan", "tg", "cot", "ctg", "sec", "csc", "cosec",
            "sinh", "sh", "cosh", "ch", "tanh", "th", "coth", "cth", "sech", "sch", "csch", "cosech",
            "asin", "acos", "atan", "arctg", "arctan", "acot", "arcctg", "arccot", "asec", "arcsec", "acsc", "arccsc", "arccosec",
            "asinh", "arsinh", "arcsinh", "acosh", "arcosh", "arccosh", "atanh", "artanh", "arctanh", "acoth", "arcoth", "arccoth", "asech", "arsech", "arcsech", "acsch", "arcsch", "arcosech"
        };
    std::string name = text;
    size_t paren = name.find('(');
    if (paren != std::string::npos)
        name = name.substr(0, paren);
    //powers of functions are emitted as sinpow(...), cospow(...)
    if (name.size() > 3 && name.substr(name.size() - 3) == "pow")
        name = name.substr(0, name.size() - 3);
    auto it = std::find(order.begin(), order.end(), name);
    if (it != order.end())
        return static_cast<int>(it - order.begin());
    return INT_MAX;
}

void SortTerms(GiacExpression& sum, const FormatContext& ctx)
{
    struct Key
    {
        int degree = 0;
        int func_rank = INT_MAX;
        std::string text;
    };
    std::vector<Key> keys;
    keys.reserve(sum.args.size());
    for (const auto& a : sum.args)
    {
        std::string key = TermSortKey(a, ctx);
        keys.push_back({TermDegree(a), FunctionSortRank(key), std::move(key)});
    }

    std::vector<size_t> idx(sum.args.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::stable_sort(idx.begin(), idx.end(), [&](size_t a, size_t b) {
        if (keys[a].degree != keys[b].degree)
            return keys[a].degree < keys[b].degree;
        if (!ctx.json && keys[a].func_rank != keys[b].func_rank)
            return keys[a].func_rank < keys[b].func_rank;
        if (keys[a].degree == NON_POLY_DEGREE || keys[b].degree == NON_POLY_DEGREE)
            return ctx.json && keys[a].text < keys[b].text;
        return keys[a].text < keys[b].text;
    });

    std::vector<GiacExpression> sorted(sum.args.size());
    for (size_t i = 0; i < idx.size(); ++i)
        sorted[i] = std::move(sum.args[idx[i]]);
    sum.args = std::move(sorted);
}

static std::string RationalStringFromGiac(const giac::gen& g, giac::context* ct, int precision)
{
    try
    {
        Rational r = FromGiac<Rational>(g, precision);
        return ToBasicString(r.ToString());
    }
    catch (...)
    {
        return GiacToString(g, ct);
    }
}

static std::string CoeffStringFromGiac(const giac::gen& g, const FormatContext& ctx)
{
    if (ctx.mode == FormatContext::Rational)
    {
        Rational r = FromGiac<Rational>(g, ctx.precision);
        if (r.IsZero())
            return "0";
        return ToBasicString(r.ToString());
    }

    Real r = FromGiac<Real>(g, ctx.precision);
    if (r.IsZero())
        return "0";
    if (r == 1)
        return "1";
    if (r == -1)
        return "-1";
    if (r.IsInfinity())
        return r.GetSign() ? "-inf" : "inf";
    return ToBasicString(r.ToString(ctx.exp, ctx.precision, false));
}

std::string AddCoeffs(const std::string& a, const std::string& b, const FormatContext& ctx)
{
    giac::context ct;
    giac::decimal_digits(std::max(1, ctx.precision + 1), &ct);
    giac::gen ag(a.c_str(), &ct);
    giac::gen bg(b.c_str(), &ct);
    giac::gen sum = giac::operator_plus(ag, bg, &ct);
    if (ctx.mode == FormatContext::Rational)
        sum = giac::normal(sum, &ct);
    else
        sum = giac::evalf(sum, 1, &ct);
    return CoeffStringFromGiac(sum, ctx);
}

std::string MultiplyCoeffs(const std::string& a, const std::string& b, const FormatContext& ctx)
{
    giac::context ct;
    giac::decimal_digits(std::max(1, ctx.precision + 1), &ct);
    giac::gen ag(a.c_str(), &ct);
    giac::gen bg(b.c_str(), &ct);
    giac::gen prod = giac::operator_times(ag, bg, &ct);
    if (ctx.mode == FormatContext::Rational)
        prod = giac::normal(prod, &ct);
    else
        prod = giac::evalf(prod, 1, &ct);
    return CoeffStringFromGiac(prod, ctx);
}

void CombineLikeTerms(GiacExpression& sum, const FormatContext& ctx)
{
    struct TermParts
    {
        std::string coeff;
        GiacExpression body;
    };

    std::vector<TermParts> parts;
    parts.reserve(sum.args.size());

    for (auto& a : sum.args)
    {
        bool neg = false;
        GiacExpression* cur = &a;
        if (cur->kind == GiacExpression::Neg)
        {
            neg = true;
            cur = &cur->args[0];
        }

        std::string coeff;
        GiacExpression body;
        if (cur->kind == GiacExpression::Number)
        {
            coeff = (neg ? "-" : "") + cur->value;
            body = GiacExpression(GiacExpression::Number, "1");
        }
        else if (cur->kind == GiacExpression::Product)
        {
            std::string num_coeff;
            std::vector<GiacExpression> body_factors;
            for (auto& f : cur->args)
            {
                if (IsNumericFactor(f))
                {
                    if (num_coeff.empty())
                        num_coeff = f.value;
                    else
                        num_coeff = MultiplyCoeffs(num_coeff, f.value, ctx);
                }
                else
                {
                    body_factors.push_back(std::move(f));
                }
            }
            if (num_coeff.empty())
                num_coeff = "1";
            if (neg && (num_coeff.empty() || num_coeff[0] != '-'))
                num_coeff = "-" + num_coeff;
            coeff = num_coeff;
            if (body_factors.empty())
                body = GiacExpression(GiacExpression::Number, "1");
            else if (body_factors.size() == 1)
                body = std::move(body_factors[0]);
            else
                body = GiacExpression(GiacExpression::Product, std::move(body_factors));
        }
        else
        {
            coeff = neg ? "-1" : "1";
            body = std::move(*cur);
        }
        parts.push_back({std::move(coeff), std::move(body)});
    }

    std::vector<std::string> keys;
    std::vector<std::string> coeffs;
    std::vector<GiacExpression> bodies;
    for (auto& p : parts)
    {
        std::string key = EmitString(p.body, ctx);
        auto it = std::find(keys.begin(), keys.end(), key);
        if (it == keys.end())
        {
            keys.push_back(key);
            coeffs.push_back(std::move(p.coeff));
            bodies.push_back(std::move(p.body));
        }
        else
        {
            size_t idx = static_cast<size_t>(it - keys.begin());
            coeffs[idx] = AddCoeffs(coeffs[idx], p.coeff, ctx);
        }
    }

    std::vector<GiacExpression> new_terms;
    for (size_t i = 0; i < bodies.size(); ++i)
    {
        if (coeffs[i] == "0")
            continue;
        if (bodies[i].kind == GiacExpression::Number && bodies[i].value == "1")
        {
            new_terms.emplace_back(GiacExpression::Number, coeffs[i]);
        }
        else if (coeffs[i] == "1")
        {
            new_terms.push_back(std::move(bodies[i]));
        }
        else if (coeffs[i] == "-1")
        {
            new_terms.emplace_back(GiacExpression::Neg, std::move(bodies[i]));
        }
        else
        {
            GiacExpression p(GiacExpression::Product);
            p.args.emplace_back(GiacExpression::Number, coeffs[i]);
            p.args.push_back(std::move(bodies[i]));
            new_terms.push_back(std::move(p));
        }
    }
    sum.args = std::move(new_terms);
}

bool IsConstantPower(const GiacExpression& e)
{
    return e.kind == GiacExpression::Power && e.args[0].kind == GiacExpression::Number && e.args[1].kind == GiacExpression::Number && 
        IsIntegerString(StripDot(e.args[1].value));
}

std::string EvaluateConstantPower(const GiacExpression& base, const GiacExpression& exp, const FormatContext& ctx)
{
    Symbolic<Real>::MpfrGuard guard;
    mpfr_t r;
    mpfr_init2(r, 512);
    mpfr_set_str(guard.num, base.value.c_str(), 10, MPFR_RNDN);
    long e = std::stol(StripDot(exp.value));
    mpfr_pow_si(r, guard.num, e, MPFR_RNDN);

    double rd = mpfr_get_d(r, MPFR_RNDN);
    int order = 0;
    if (rd != 0.0 && std::isfinite(rd))
        order = static_cast<int>(std::floor(std::log10(std::fabs(rd)))) + 1;

    std::string str;
    if (order > ctx.exp && ctx.exp >= 0)
        str = Symbolic<Real>::FormatScientific(Symbolic<Real>::MpfrFormat("%.*Re", ctx.precision, r));
    else
        str = Symbolic<Real>::FormatFixed(Symbolic<Real>::MpfrFormat("%.*Rf", ctx.precision, r), false);

    if (mpfr_zero_p(r) == 0 && Symbolic<Real>::IsEffectivelyZeroFixedStr(str))
        str = Symbolic<Real>::FormatScientific(Symbolic<Real>::MpfrFormat("%.*Re", ctx.precision, r));

    mpfr_clear(r);
    return str;
}

long long IntegerNthRootExact(long long base, int n)
{
    if (n <= 1)
        return base;
    if (base == 0)
        return 0;
    bool negative = base < 0 && (n % 2 == 1);
    unsigned long long abs_base = negative
        ? -static_cast<unsigned long long>(base)
        : static_cast<unsigned long long>(base);
    if (abs_base <= 1)
        return base;
    long double approx = std::pow(static_cast<long double>(abs_base), 1.0L / n);
    long long r = static_cast<long long>(std::llround(approx));
    if (r < 1)
        r = 1;
    auto ipow = [&](long long x) -> __int128
    {
        __int128 res = 1;
        for (int i = 0; i < n; ++i)
            res *= x;
        return res;
    };
    for (long long cand = r - 1; cand <= r + 1; ++cand)
    {
        if (cand <= 0)
            continue;
        __int128 p = ipow(cand);
        if (p == static_cast<__int128>(abs_base))
            return negative ? -cand : cand;
    }
    return 0;
}

bool IsArithmeticConstantExpr(const GiacExpression& e)
{
    switch (e.kind)
    {
    case GiacExpression::Number:
        return true;
    case GiacExpression::Ident:
        return IsKnownSymbol(e.value);
    case GiacExpression::Func:
    case GiacExpression::Sqrt:
        return false;
    case GiacExpression::Power:
    case GiacExpression::Div:
    case GiacExpression::Neg:
        if (e.args.empty())
            return true;
        if (!IsArithmeticConstantExpr(e.args[0]))
            return false;
        return e.args.size() < 2 || IsArithmeticConstantExpr(e.args[1]);
    case GiacExpression::Sum:
    case GiacExpression::Product:
    {
        for (const auto& a : e.args)
            if (!IsArithmeticConstantExpr(a))
                return false;
        return true;
    }
    case GiacExpression::Root:
        return false;
    default:
        return false;
    }
}

std::string EvaluateGiacExpression(const std::string& expr, int precision)
{
    giac::context ct;
    giac::decimal_digits(std::max(1, precision + 1), &ct);
    giac::gen g(expr.c_str(), &ct);
    g = giac::evalf(g, std::max(1, precision), &ct);
    if (g.type == giac::_CPLX)
        return "nan";
    if (g == giac::undef)
        return "nan";
    if (g.type == giac::_SYMB)
    {
        std::string str = g.print(&ct);
        if (str.find("undef") != std::string::npos || str.find("error") != std::string::npos || str.find("Error") != std::string::npos)
            return "nan";
    }
    Real r = FromGiac<Real>(g, precision);
    if (r.IsNaN())
        return "nan";
    if (r.IsZero())
        return "0";
    if (r.IsInfinity())
        return r.GetSign() ? "-inf" : "inf";
    return ToBasicString(r.ToString(precision, precision, false));
}

bool IsInertFunction(const std::string& name)
{
    static const std::set<std::string> inert = {"cot", "sec", "csc", "coth", "sech", "csch", "asinh", "acosh", "atanh", "acoth", "asech", "acsch"};
    return inert.find(name) != inert.end();
}

std::string InertFunctionExpr(const std::string& name, const std::string& arg)
{
    if (name == "cot")
        return "1/tg(" + arg + ")";
    if (name == "sec")
        return "1/cos(" + arg + ")";
    if (name == "csc")
        return "1/sin(" + arg + ")";
    if (name == "coth")
        return "1/tanh(" + arg + ")";
    if (name == "sech")
        return "1/cosh(" + arg + ")";
    if (name == "csch")
        return "1/sinh(" + arg + ")";
    if (name == "asinh")
        return "asinh(" + arg + ")";
    if (name == "acosh")
        return "acosh(" + arg + ")";
    if (name == "atanh")
        return "atanh(" + arg + ")";
    if (name == "acoth")
        return "atanh(1/(" + arg + "))";
    if (name == "asech")
        return "acosh(1/(" + arg + "))";
    if (name == "acsch")
        return "asinh(1/(" + arg + "))";
    return name + "(" + arg + ")";
}

bool IsGiacExpressionZero(const std::string& expr, int precision)
{
    giac::context ct;
    giac::decimal_digits(60, &ct);
    giac::gen g(expr.c_str(), &ct);
    giac::gen n = giac::normal(g, &ct);
    if (giac::is_zero(n, &ct))
        return true;
    giac::gen f = giac::evalf(g, 60, &ct);
    if (f.type == giac::_CPLX)
        return false;
    try
    {
        const Real r = FromGiac<Real>(f, precision);
        if (r.IsNaN())
            return false;
        double d = mpfr_get_d(r.GetNumber(), MPFR_RNDN);
        return std::fabs(d) < 1e-6;
    }
    catch (...)
    {
    }
    return false;
}

std::string ExactInertArg(std::string arg)
{
    for (size_t i = 0; i < arg.size(); )
    {
        if (arg[i] == '.' && (i + 1 >= arg.size() || !std::isdigit(static_cast<unsigned char>(arg[i + 1]))))
            arg.erase(i, 1);
        else
            ++i;
    }
    return arg;
}

bool IsInverseHyperbolicInertSingularity(const std::string& name, const std::string& arg, int precision)
{
    if (name == "atanh" || name == "acoth")
    {
        if (IsGiacExpressionZero("(" + arg + ")-1", precision) || IsGiacExpressionZero("(" + arg + ")+1", precision))
            return true;
    }
    if (name == "asech" || name == "acsch")
    {
        if (IsGiacExpressionZero(arg, precision))
            return true;
    }
    return false;
}

std::optional<GiacExpression> InertFunctionSingularity(const std::string& name, const std::string& arg, int precision)
{
    std::string exact = ExactInertArg(arg);
    if (name == "cot" && IsGiacExpressionZero("sin(" + exact + ")", precision))
        return GiacExpression(GiacExpression::Ident, "inf");
    if (name == "sec" && IsGiacExpressionZero("cos(" + exact + ")", precision))
        return GiacExpression(GiacExpression::Ident, "inf");
    if (name == "csc" && IsGiacExpressionZero("sin(" + exact + ")", precision))
        return GiacExpression(GiacExpression::Ident, "inf");
    if (name == "coth" && IsGiacExpressionZero("tanh(" + exact + ")", precision))
        return GiacExpression(GiacExpression::Ident, "inf");
    if (name == "csch" && IsGiacExpressionZero("sinh(" + exact + ")", precision))
        return GiacExpression(GiacExpression::Ident, "inf");
    return std::nullopt;
}

GiacExpression ExprFromGiacResult(std::string str)
{
    str = Symbolic<Real>::ReplaceAll(str, " ", "");
    str = Symbolic<Real>::ReplaceSqrtSymbol(str);
    if (!str.empty() && str.back() == '.')
        str.pop_back();
    if (str == "inf" || str == "+inf" || str == "+infinity" || str == "infinity")
        return GiacExpression(GiacExpression::Ident, "inf");
    if (str == "-inf" || str == "-infinity")
        return GiacExpression(GiacExpression::Neg, GiacExpression(GiacExpression::Ident, "inf"));
    if (str == "nan" || str == "undef")
        return GiacExpression(GiacExpression::Ident, "nan");
    return GiacExpression(GiacExpression::Number, str);
}

bool ContainsFunctionCall(const GiacExpression& e)
{
    switch (e.kind)
    {
    case GiacExpression::Func:
    case GiacExpression::Sqrt:
    case GiacExpression::Root:
        return true;
    default:
        break;
    }
    for (const auto& a : e.args)
        if (ContainsFunctionCall(a))
            return true;
    return false;
}

bool ContainsImaginaryUnit(const GiacExpression& e)
{
    if (e.kind == GiacExpression::Ident && (e.value == "i" || e.value == "j"))
        return true;
    for (const auto& a : e.args)
    {
        if (ContainsImaginaryUnit(a))
            return true;
    }
    return false;
}

GiacExpression EvaluateArithmeticExpr(GiacExpression e, const FormatContext& ctx)
{
    if (ContainsImaginaryUnit(e))
        return e;
    if (ctx.json && ContainsFunctionCall(e))
        return e;
    if (ctx.mode == FormatContext::Rational)
    {
        std::string expr = EmitString(e, ctx);
        giac::context ct;
        giac::decimal_digits(std::max(1, ctx.precision + 1), &ct);
        giac::gen g(expr.c_str(), &ct);
        g = giac::normal(g, &ct);
        return ExprFromGiacResult(RationalStringFromGiac(g, &ct, ctx.precision));
    }
    std::string expr = EmitString(e, ctx);
    std::string val = EvaluateGiacExpression(expr, ctx.precision);
    return ExprFromGiacResult(val);
}

bool IsNegativeNumberString(const std::string& str)
{
    return !str.empty() && str[0] == '-';
}

GiacExpression NormalizeNegativeProductTerm(GiacExpression e)
{
    if (e.kind == GiacExpression::Product)
    {
        bool negative = false;
        for (auto& f : e.args)
        {
            if (f.kind == GiacExpression::Number && IsNegativeNumberString(f.value))
            {
                f.value = f.value.substr(1);
                negative = !negative;
            }
        }
        if (negative)
            return GiacExpression(GiacExpression::Neg, std::move(e));
    }
    else if (e.kind == GiacExpression::Number && IsNegativeNumberString(e.value))
    {
        std::string v = e.value.substr(1);
        return GiacExpression(GiacExpression::Neg, GiacExpression(GiacExpression::Number, std::move(v)));
    }
    return e;
}

bool HasNegativeConstantTerm(const GiacExpression& e)
{
    if (e.kind != GiacExpression::Sum)
        return false;
    for (const auto& a : e.args)
    {
        if (a.kind == GiacExpression::Number && IsNegativeNumberString(a.value))
            return true;
        if (a.kind == GiacExpression::Neg && a.args[0].kind == GiacExpression::Number)
            return true;
    }
    return false;
}

GiacExpression NegateSum(GiacExpression e)
{
    if (e.kind != GiacExpression::Sum)
        return GiacExpression(GiacExpression::Neg, std::move(e));
    for (auto& a : e.args)
    {
        if (a.kind == GiacExpression::Number)
        {
            if (IsNegativeNumberString(a.value))
                a.value = a.value.substr(1);
            else
                a.value = "-" + a.value;
        }
        else if (a.kind == GiacExpression::Neg && a.args[0].kind == GiacExpression::Number)
        {
            a = std::move(a.args[0]);
        }
        else
        {
            a = GiacExpression(GiacExpression::Neg, std::move(a));
        }
    }
    return e;
}

long long GcdLL(long long a, long long b)
{
    a = std::llabs(a);
    b = std::llabs(b);
    while (b != 0)
    {
        long long t = a % b;
        a = b;
        b = t;
    }
    return a;
}

long long SquarefreePartLL(long long n)
{
    n = std::llabs(n);
    long long result = 1;
    for (long long p = 2; p * p <= n; ++p)
    {
        if (n % p == 0)
        {
            int count = 0;
            while (n % p == 0)
            {
                n /= p;
                ++count;
            }
            if (count % 2 == 1)
                result *= p;
        }
    }
    if (n > 1)
        result *= n;
    return result;
}

std::optional<GiacExpression> TryFactorSqrtSum(const GiacExpression& sum)
{
    if (sum.kind != GiacExpression::Sum || sum.args.size() < 2)
        return std::nullopt;
    std::vector<long long> radicands;
    std::vector<int> signs;
    for (const auto& a : sum.args)
    {
        int sign = 1;
        const GiacExpression* term = &a;
        if (term->kind == GiacExpression::Neg)
        {
            sign = -1;
            term = &term->args[0];
        }
        if (term->kind != GiacExpression::Sqrt || term->args[0].kind != GiacExpression::Number)
            return std::nullopt;
        std::string v = StripDot(term->args[0].value);
        if (!IsIntegerString(v))
            return std::nullopt;
        try
        {
            long long n = std::stoll(v);
            if (n <= 0)
                return std::nullopt;
            radicands.push_back(n);
            signs.push_back(sign);
        }
        catch (...)
        {
            return std::nullopt;
        }
    }
    long long g = radicands[0];
    for (size_t i = 1; i < radicands.size(); ++i)
        g = GcdLL(g, radicands[i]);
    g = SquarefreePartLL(g);
    if (g <= 1)
        return std::nullopt;
    GiacExpression inner(GiacExpression::Sum);
    for (size_t i = 0; i < radicands.size(); ++i)
    {
        long long r = radicands[i] / g;
        GiacExpression term;
        if (r == 1)
            term = GiacExpression(GiacExpression::Number, "1");
        else
            term = GiacExpression(GiacExpression::Sqrt, GiacExpression(GiacExpression::Number, std::to_string(r)));
        if (signs[i] < 0)
            term = GiacExpression(GiacExpression::Neg, std::move(term));
        inner.args.push_back(std::move(term));
    }
    GiacExpression factor(GiacExpression::Sqrt, GiacExpression(GiacExpression::Number, std::to_string(g)));
    return GiacExpression(GiacExpression::Product, std::vector<GiacExpression>{
        std::move(factor), std::move(inner)});
}

GiacExpression Transform(GiacExpression e, const FormatContext& ctx);

GiacExpression TransformChildren(GiacExpression e, const FormatContext& ctx)
{
    for (auto& a : e.args)
        a = Transform(std::move(a), ctx);
    return e;
}

GiacExpression Transform(GiacExpression e, const FormatContext& ctx)
{
    switch (e.kind)
    {
    case GiacExpression::Sum:
    {
        std::vector<GiacExpression> terms;
        for (auto& a : e.args)
        {
            GiacExpression t = Transform(std::move(a), ctx);
            if (t.kind == GiacExpression::Sum)
            {
                for (auto& st : t.args)
                    terms.push_back(std::move(st));
            }
            else
            {
                terms.push_back(std::move(t));
            }
        }
        e.args = std::move(terms);
        if (IsArithmeticConstantExpr(e))
            return EvaluateArithmeticExpr(std::move(e), ctx);
        CombineLikeTerms(e, ctx);
        for (auto& term : e.args)
            term = NormalizeNegativeProductTerm(std::move(term));
        SortTerms(e, ctx);
        if (auto factored = TryFactorSqrtSum(e))
        {
            GiacExpression r = Transform(std::move(*factored), ctx);
            return r;
        }
        return e;
    }
    case GiacExpression::Product:
    {
        std::vector<GiacExpression> factors;
        for (size_t ai = 0; ai < e.args.size(); ++ai)
        {
            GiacExpression f = Transform(std::move(e.args[ai]), ctx);
            if (f.kind == GiacExpression::Product)
            {
                for (auto& sf : f.args)
                    factors.push_back(std::move(sf));
            }
            else
            {
                factors.push_back(std::move(f));
            }
        }
        //normalize a leading Neg(Number) into a negative number coefficient
        for (auto& f : factors)
        {
            if (f.kind == GiacExpression::Neg && f.args[0].kind == GiacExpression::Number)
            {
                std::string v = f.args[0].value;
                if (!v.empty() && v[0] == '-')
                    v = v.substr(1);
                else
                    v = "-" + v;
                f = GiacExpression(GiacExpression::Number, v);
            }
        }
        //merge sqrt/root factors into powers and combine powers with the same base
        for (auto& f : factors)
        {
            if (f.kind == GiacExpression::Sqrt && f.args[0].kind != GiacExpression::Number)
            {
                f = GiacExpression(GiacExpression::Power,
                    std::move(f.args[0]),
                    GiacExpression(GiacExpression::Div,
                        GiacExpression(GiacExpression::Number, "1"),
                        GiacExpression(GiacExpression::Number, "2")));
            }
            else if (f.kind == GiacExpression::Root && f.args[0].kind != GiacExpression::Number)
            {
                f = GiacExpression(GiacExpression::Power,
                    std::move(f.args[0]),
                    GiacExpression(GiacExpression::Div,
                        GiacExpression(GiacExpression::Number, "1"),
                        GiacExpression(GiacExpression::Number, f.args[1].value)));
            }
        }
        {
            //preserve the original factor order while still combining powers that share the same base (SymEngine orders by complexity, not lexicographically)
            std::vector<std::pair<std::string, std::vector<GiacExpression>>> groups;
            std::vector<GiacExpression> others;
            for (auto& f : factors)
            {
                if (f.kind == GiacExpression::Number)
                {
                    others.push_back(std::move(f));
                    continue;
                }
                std::string key = (f.kind == GiacExpression::Power)
                    ? EmitString(f.args[0], ctx)
                    : EmitString(f, ctx);
                auto it = groups.begin();
                for (; it != groups.end(); ++it)
                {
                    if (it->first == key)
                        break;
                }
                if (it == groups.end())
                {
                    groups.emplace_back(std::move(key), std::vector<GiacExpression>{});
                    it = groups.end() - 1;
                }
                it->second.push_back(std::move(f));
            }
            factors.clear();
            for (auto& g : groups)
            {
                if (g.second.size() == 1)
                {
                    factors.push_back(std::move(g.second[0]));
                }
                else
                {
                    GiacExpression base = std::move(g.second[0]);
                    bool base_is_power = base.kind == GiacExpression::Power;
                    GiacExpression real_base = base_is_power ? std::move(base.args[0]) : std::move(base);
                    GiacExpression sum(GiacExpression::Sum);
                    if (base_is_power)
                        sum.args.push_back(std::move(base.args[1]));
                    else
                        sum.args.emplace_back(GiacExpression::Number, "1");
                    for (size_t i = 1; i < g.second.size(); ++i)
                    {
                        GiacExpression& f = g.second[i];
                        if (f.kind == GiacExpression::Power)
                            sum.args.push_back(std::move(f.args[1]));
                        else
                            sum.args.emplace_back(GiacExpression::Number, "1");
                    }
                    factors.emplace_back(GiacExpression::Power,
                        std::move(real_base),
                        Transform(std::move(sum), ctx));
                }
            }
            factors.insert(factors.end(), std::make_move_iterator(others.begin()), std::make_move_iterator(others.end()));
        }
        bool negative = false;
        if (ctx.json)
        {
            //flatten negated products so the minus sign can be hoisted to the outer product
            {
                std::vector<GiacExpression> flat;
                for (auto& f : factors)
                {
                    if (f.kind == GiacExpression::Neg && f.args[0].kind == GiacExpression::Product)
                    {
                        negative = !negative;
                        for (auto& sf : f.args[0].args)
                            flat.push_back(std::move(sf));
                    }
                    else
                    {
                        flat.push_back(std::move(f));
                    }
                }
                factors = std::move(flat);
            }
            for (auto& f : factors)
            {
                if (f.kind == GiacExpression::Number && IsNegativeNumberString(f.value))
                {
                    negative = !negative;
                    f.value = f.value.substr(1);
                }
                else if (f.kind == GiacExpression::Sum && HasNegativeConstantTerm(f))
                {
                    negative = !negative;
                    f = NegateSum(std::move(f));
                }
            }
        }
        e.args = std::move(factors);
        ReorderFactors(e);
        bool is_arith = IsArithmeticConstantExpr(e);
        if (is_arith)
        {
            GiacExpression r = EvaluateArithmeticExpr(std::move(e), ctx);
            if (r.kind == GiacExpression::Number && IsNegativeNumberString(r.value))
            {
                negative = !negative;
                r.value = r.value.substr(1);
            }
            if (negative)
                return GiacExpression(GiacExpression::Neg, std::move(r));
            return r;
        }
        if (negative)
            return GiacExpression(GiacExpression::Neg, std::move(e));
        return e;
    }
    case GiacExpression::Div:
    {
        GiacExpression num = Transform(std::move(e.args[0]), ctx);
        GiacExpression den = Transform(std::move(e.args[1]), ctx);
        if (num.kind == GiacExpression::Neg && num.args[0].kind == GiacExpression::Number)
        {
            GiacExpression inner(GiacExpression::Div, std::move(num.args[0]), std::move(den));
            return Transform(GiacExpression(GiacExpression::Neg, Transform(std::move(inner), ctx)), ctx);
        }
        if (num.kind == GiacExpression::Number && IsNegativeNumberString(num.value))
        {
            GiacExpression n(GiacExpression::Number, num.value.substr(1));
            GiacExpression inner(GiacExpression::Div, std::move(n), std::move(den));
            return Transform(GiacExpression(GiacExpression::Neg, Transform(std::move(inner), ctx)), ctx);
        }
        {
            GiacExpression tmp(GiacExpression::Div, GiacExpression(num), GiacExpression(den));
            if (IsArithmeticConstantExpr(tmp))
                return EvaluateArithmeticExpr(std::move(tmp), ctx);
        }
        if (IsOne(num.value))
            return GiacExpression(GiacExpression::Power, std::move(den), GiacExpression(GiacExpression::Number, "-1"));
        if (IsConstantPower(den))
        {
            std::string den_val = EvaluateConstantPower(den.args[0], den.args[1], ctx);
            GiacExpression tmp(GiacExpression::Div, std::move(num), GiacExpression(GiacExpression::Number, den_val));
            return Transform(std::move(tmp), ctx);
        }
        if (IsNumericFactor(den))
        {
            std::string rec;
            if (ctx.mode == FormatContext::Rational)
                rec = "1/" + den.value;
            else
                rec = ReciprocalDecimal(den.value, ctx.precision, ctx.exp);
            if (num.kind == GiacExpression::Product)
            {
                std::vector<GiacExpression> numeric;
                std::vector<GiacExpression> other;
                for (auto& f : num.args)
                {
                    if (IsNumericFactor(f))
                        numeric.push_back(std::move(f));
                    else
                        other.push_back(std::move(f));
                }
                if (!numeric.empty())
                {
                    GiacExpression num_prod = numeric.size() == 1
                        ? std::move(numeric[0])
                        : GiacExpression(GiacExpression::Product, std::move(numeric));
                    GiacExpression frac(GiacExpression::Div, std::move(num_prod), std::move(den));
                    GiacExpression p(GiacExpression::Product);
                    p.args.push_back(Transform(std::move(frac), ctx));
                    for (auto& o : other)
                        p.args.push_back(std::move(o));
                    return Transform(std::move(p), ctx);
                }
                GiacExpression p(GiacExpression::Product);
                for (auto& o : other)
                    p.args.push_back(std::move(o));
                p.args.emplace_back(GiacExpression::Number, rec);
                return Transform(std::move(p), ctx);
            }
            GiacExpression p(GiacExpression::Product);
            p.args.push_back(std::move(num));
            p.args.emplace_back(GiacExpression::Number, rec);
            return Transform(std::move(p), ctx);
        }
        return GiacExpression(GiacExpression::Div, std::move(num), std::move(den));
    }
    case GiacExpression::Power:
    {
        //integer perfect roots: 8^(1/3) -> 2
        if (e.args[0].kind == GiacExpression::Number &&
            e.args[1].kind == GiacExpression::Div &&
            e.args[1].args.size() == 2 &&
            e.args[1].args[0].kind == GiacExpression::Number &&
            StripDot(e.args[1].args[0].value) == "1" &&
            e.args[1].args[1].kind == GiacExpression::Number &&
            IsIntegerString(StripDot(e.args[1].args[1].value)))
        {
            int n = std::stoi(StripDot(e.args[1].args[1].value));
            if (n > 1)
            {
                try
                {
                    std::string base_str = StripDot(e.args[0].value);
                    size_t pos = 0;
                    long long base_val = std::stoll(base_str, &pos);
                    if (pos == base_str.size())
                    {
                        long long root = IntegerNthRootExact(base_val, n);
                        if (root != 0)
                            return GiacExpression(GiacExpression::Number, std::to_string(root));
                    }
                }
                catch (...)
                {
                }
            }
        }
        //x^(1/n) is rendered as an n-th root in JSON, detect it before the exponent is evaluated to a decimal approximation
        if (ctx.json && e.args[0].kind != GiacExpression::Number && e.args[1].kind == GiacExpression::Div && e.args[1].args.size() == 2 &&
            e.args[1].args[0].kind == GiacExpression::Number && StripDot(e.args[1].args[0].value) == "1" && e.args[1].args[1].kind == GiacExpression::Number &&
            IsIntegerString(StripDot(e.args[1].args[1].value)))
        {
            int n = std::stoi(StripDot(e.args[1].args[1].value));
            if (n > 1)
            {
                GiacExpression base = Transform(std::move(e.args[0]), ctx);
                return GiacExpression(GiacExpression::Root, std::vector<GiacExpression>{
                    std::move(base),
                    GiacExpression(GiacExpression::Number, std::to_string(n))});
            }
        }
        e.args[0] = Transform(std::move(e.args[0]), ctx);
        e.args[1] = Transform(std::move(e.args[1]), ctx);
        //giac flattens (x^y)^z into x^(y*z). Reconstruct the nested power so that left-associative chains are printed as pow(pow(x,y),z)
        if (e.args[1].kind == GiacExpression::Product && e.args[1].args.size() == 2)
        {
            GiacExpression inner(GiacExpression::Power,
                std::move(e.args[0]),
                std::move(e.args[1].args[0]));
            GiacExpression outer(GiacExpression::Power,
                std::move(inner),
                std::move(e.args[1].args[1]));
            return Transform(std::move(outer), ctx);
        }
        //abs(A)^N -> pow(A,N); abs(A)^B outer -> pow(pow(A,N?),B)... the parser turns abspow(A,B) into pow(abs(A),B), if there is an
        //additional outer power N, distribute it to A: pow(pow(A,N),B)
        if (e.args[0].kind == GiacExpression::Power && e.args[0].args[0].kind == GiacExpression::Func && e.args[0].args[0].value == "abs" &&
            e.args[0].args[0].args.size() == 1 && e.args[1].kind == GiacExpression::Number &&
            IsIntegerString(StripDot(e.args[1].value)))
        {
            GiacExpression a = std::move(e.args[0].args[0].args[0]);
            GiacExpression b = std::move(e.args[0].args[1]);
            GiacExpression n = std::move(e.args[1]);
            GiacExpression new_inner(GiacExpression::Power, std::move(a), std::move(n));
            return Transform(GiacExpression(GiacExpression::Power, std::move(new_inner), std::move(b)), ctx);
        }
        //abs(X)^N -> pow(X,N)
        if (e.args[0].kind == GiacExpression::Func && e.args[0].value == "abs" && e.args[0].args.size() == 1 && e.args[1].kind == GiacExpression::Number &&
            IsIntegerString(StripDot(e.args[1].value)))
        {
            GiacExpression base = std::move(e.args[0].args[0]);
            GiacExpression exp = std::move(e.args[1]);
            return Transform(GiacExpression(GiacExpression::Power, std::move(base), std::move(exp)), ctx);
        }
        if (IsConstantPower(e))
        {
            std::string val = EvaluateConstantPower(e.args[0], e.args[1], ctx);
            return GiacExpression(GiacExpression::Number, val);
        }
        if (IsArithmeticConstantExpr(e))
            return EvaluateArithmeticExpr(std::move(e), ctx);
        return e;
    }
    case GiacExpression::Neg:
    {
        e.args[0] = Transform(std::move(e.args[0]), ctx);
        if (e.args[0].kind == GiacExpression::Neg)
            return std::move(e.args[0].args[0]);
        if (IsArithmeticConstantExpr(e))
            return EvaluateArithmeticExpr(std::move(e), ctx);
        if (e.args[0].kind == GiacExpression::Number)
        {
            std::string v = e.args[0].value;
            if (!v.empty() && v[0] == '-')
                v = v.substr(1);
            else
                v = "-" + v;
            return GiacExpression(GiacExpression::Number, v);
        }
        return e;
    }
    case GiacExpression::Func:
    {
        std::string mapped_name = MapFuncName(e.value);
        std::string raw_arg_str;
        bool inert_constant = (e.args.size() == 1 &&
            IsInertFunction(mapped_name) && IsConstantExpr(e.args[0]));
        if (inert_constant)
            raw_arg_str = EmitString(e.args[0], ctx);
        for (auto& a : e.args)
            a = Transform(std::move(a), ctx);
        e.value = std::move(mapped_name);
        if ((e.value == "cos" || e.value == "cosh") && e.args.size() == 1 && e.args[0].kind == GiacExpression::Sum && HasNegativeConstantTerm(e.args[0]))
        {
            e.args[0] = NegateSum(std::move(e.args[0]));
            return Transform(std::move(e), ctx);
        }
        if (e.value == "factorial" && e.args.size() == 1)
        {
            GiacExpression sum(GiacExpression::Sum);
            sum.args.emplace_back(GiacExpression::Number, "1");
            sum.args.push_back(std::move(e.args[0]));
            return GiacExpression(GiacExpression::Func, "gamma", std::vector<GiacExpression>{std::move(sum)});
        }
        if (inert_constant)
        {
            if (IsInverseHyperbolicInertSingularity(e.value, raw_arg_str, ctx.precision))
                return e;
            auto singular = InertFunctionSingularity(e.value, raw_arg_str, ctx.precision);
            if (singular)
                return *singular;
            std::string expr = InertFunctionExpr(e.value, EmitString(e.args[0], ctx));
            if (ctx.mode == FormatContext::Rational)
            {
                giac::context ct;
                giac::decimal_digits(std::max(1, ctx.precision + 1), &ct);
                giac::gen g(expr.c_str(), &ct);
                g = giac::normal(g, &ct);
                return ExprFromGiacResult(RationalStringFromGiac(g, &ct, ctx.precision));
            }
            std::string val = EvaluateGiacExpression(expr, ctx.precision);
            return ExprFromGiacResult(val);
        }
        return e;
    }
    case GiacExpression::Sqrt:
    {
        e.args[0] = Transform(std::move(e.args[0]), ctx);
        return e;
    }
    default:
        return e;
    }
}

GiacExpression BuildFormattedAst(std::string str, const FormatContext& ctx)
{
    str = Symbolic<Real>::ReplaceSqrtSymbol(str);
    str = Symbolic<Real>::ReplacePowerOperator(str);
    str = Symbolic<Real>::ReplaceCaretOperator(str);
    str = Symbolic<Real>::ReplaceAll(str, " ", "");
    str = Symbolic<Real>::ReplaceAll(str, "+infinity", "oo");
    str = Symbolic<Real>::ReplaceAll(str, "-infinity", "-oo");
    str = Symbolic<Real>::ReplaceAll(str, "undef", "nan");

    GiacExpression ast = ExpressionParser(str).Parse();
    return Transform(std::move(ast), ctx);
}

std::string FormatGiacString(std::string str, const FormatContext& ctx)
{
    GiacExpression ast = BuildFormattedAst(std::move(str), ctx);
    std::string result = EmitString(ast, ctx);
    if (ctx.language != Language::Russian)
        result = Symbolic<Real>::ReplaceAll(result, "ln(", "log(");
    return result;
}

std::string JsonComma()
{
    return R"({"type":18,"symbol":","})";
}

//Returns the JSON element for the absolute value of a number, the caller is responsible for emitting a leading minus when the value is negative
std::vector<std::string> JsonNumberRaw(const std::string& value, const FormatContext& ctx)
{
    if (ctx.mode == FormatContext::Rational)
    {
        std::string pos = !value.empty() && value[0] == '-' ? value.substr(1) : value;
        pos = StripTrailingDotZeros(pos);
        size_t slash = pos.find('/');
        if (slash != std::string::npos)
        {
            std::string num = pos.substr(0, slash);
            std::string den = pos.substr(slash + 1);
            return {Symbolic<Real>::JsonDivision(Symbolic<Real>::JsonCodeRow({Symbolic<Real>::JsonCodeString(num)}),
                Symbolic<Real>::JsonCodeRow({Symbolic<Real>::JsonCodeString(den)}))};
        }
        return {Symbolic<Real>::JsonCodeString(pos)};
    }

    std::string formatted = RealNumberStr(value, ctx.precision, ctx.exp);
    if (formatted.find_first_of("eE") != std::string::npos)
        return Symbolic<Real>::JsonScientificElements(formatted);
    std::string pos = !formatted.empty() && formatted[0] == '-' ? formatted.substr(1) : formatted;
    //strip trailing dot from integers in JSON
    if (!pos.empty() && pos.back() == '.')
    {
        bool all_digits = true;
        for (char c : pos)
        {
            if (c == '-')
                continue;
            if (!std::isdigit(static_cast<unsigned char>(c)))
            {
                all_digits = false;
                break;
            }
        }
        if (all_digits)
            pos.pop_back();
    }
    return {Symbolic<Real>::JsonCodeString(pos)};
}

bool IsNegativeNumberValue(const std::string& value)
{
    return !value.empty() && value[0] == '-';
}

std::vector<std::string> EmitJson(const GiacExpression& e, const FormatContext& ctx);
bool IsCodeStringJson(const std::string& str);

std::vector<std::string> EmitJsonChildren(const std::vector<GiacExpression>& args, const FormatContext& ctx)
{
    std::vector<std::string> result;
    for (size_t i = 0; i < args.size(); ++i)
    {
        if (i > 0)
            result.push_back(JsonComma());
        auto child = EmitJson(args[i], ctx);
        result.insert(result.end(), child.begin(), child.end());
    }
    return result;
}

std::vector<std::string> EmitJson(const GiacExpression& e, const FormatContext& ctx)
{
    switch (e.kind)
    {
    case GiacExpression::Number:
    {
        auto raw = JsonNumberRaw(e.value, ctx);
        if (IsNegativeNumberValue(e.value))
        {
            //scientific notation already includes the minus sign in its elements
            if (!raw.empty() && raw[0].find(R"({"type":12,)") == 0)
                return raw;
            std::vector<std::string> res;
            res.reserve(raw.size() + 1);
            res.push_back(Symbolic<Real>::JsonMinus());
            res.insert(res.end(), raw.begin(), raw.end());
            return res;
        }
        return raw;
    }
    case GiacExpression::Ident:
        return {Symbolic<Real>::JsonCodeString(MapSpecialIdent(e.value))};
    case GiacExpression::Func:
    {
        std::string name = MapFuncName(e.value);
        if (ctx.language != Language::Russian && name == "ln")
            name = "log";
        if (name == "exp" && e.args.size() == 1)
        {
            if (e.args[0].kind == GiacExpression::Number && IsOne(e.args[0].value))
                return {Symbolic<Real>::JsonCodeString("e")};
            return {Symbolic<Real>::JsonPower(
                Symbolic<Real>::JsonCodeRow({Symbolic<Real>::JsonCodeString("e")}),
                Symbolic<Real>::JsonCodeRow(EmitJson(e.args[0], ctx)))};
        }
        std::vector<std::string> elems;
        elems.push_back(Symbolic<Real>::JsonCodeString(name));
        elems.push_back(Symbolic<Real>::JsonOpenRoundBracket());
        auto args = EmitJsonChildren(e.args, ctx);
        elems.insert(elems.end(), args.begin(), args.end());
        elems.push_back(Symbolic<Real>::JsonCloseRoundBracket());
        return elems;
    }
    case GiacExpression::Sqrt:
    {
        std::string base_row = Symbolic<Real>::JsonCodeRow(EmitJson(e.args[0], ctx));
        return {Symbolic<Real>::JsonSquareRoot(base_row)};
    }
    case GiacExpression::Root:
    {
        std::string n_row = Symbolic<Real>::JsonCodeRow({Symbolic<Real>::JsonCodeString(e.args[1].value)});
        std::string base_row = Symbolic<Real>::JsonCodeRow(EmitJson(e.args[0], ctx));
        return {Symbolic<Real>::JsonNthRoot(n_row, base_row)};
    }
    case GiacExpression::Power:
    {
        auto base_elems = EmitJson(e.args[0], ctx);
        if (e.args[1].kind == GiacExpression::Number && IsOne(e.args[1].value))
            return base_elems;
        bool base_needs_parens = e.args[0].kind == GiacExpression::Sum || e.args[0].kind == GiacExpression::Product ||
            e.args[0].kind == GiacExpression::Div || e.args[0].kind == GiacExpression::Neg || e.args[0].kind == GiacExpression::Power || 
            e.args[0].kind == GiacExpression::Root;
        std::string base_inner;
        if (base_elems.size() == 1 && !IsCodeStringJson(base_elems[0]))
            base_inner = base_elems[0];
        else
            base_inner = Symbolic<Real>::JsonCodeRow(base_elems);
        std::string base_row;
        if (base_needs_parens)
        {
            base_row = Symbolic<Real>::JsonCodeRow({Symbolic<Real>::JsonOpenRoundBracket(), base_inner, Symbolic<Real>::JsonCloseRoundBracket()});
            if (ctx.in_product)
                base_row = Symbolic<Real>::JsonCodeRow({base_row});
        }
        else
        {
            base_row = base_inner;
        }
        auto exp_elems = EmitJson(e.args[1], ctx);
        std::string exp_row;
        if (exp_elems.size() == 1 && !IsCodeStringJson(exp_elems[0]))
            exp_row = exp_elems[0];
        else
            exp_row = Symbolic<Real>::JsonCodeRow(exp_elems);
        return {Symbolic<Real>::JsonPower(base_row, exp_row)};
    }
    case GiacExpression::Div:
    {
        auto wrap_operand = 
            [&](const GiacExpression& op) -> std::string
            {
                auto inner = EmitJson(op, ctx);
                std::string elem;
                if (inner.size() == 1)
                    elem = inner[0];
                else
                    elem = Symbolic<Real>::JsonCodeRow(inner);
                //avoid nested CodeRow if the operand already produced one
                if (elem.size() >= 10 && elem.substr(0, 10) == R"({"type":7,)")
                    return elem;
                return Symbolic<Real>::JsonCodeRow({elem});
            };
        return {Symbolic<Real>::JsonDivision(wrap_operand(e.args[0]), wrap_operand(e.args[1]))};
    }
    case GiacExpression::Neg:
    {
        std::vector<std::string> res;
        res.push_back(Symbolic<Real>::JsonMinus());
        auto inner = EmitJson(e.args[0], ctx);
        if (inner.size() > 1 && e.args[0].kind != GiacExpression::Product)
            inner = {Symbolic<Real>::JsonCodeRow(inner)};
        res.insert(res.end(), inner.begin(), inner.end());
        return res;
    }
    case GiacExpression::Sum:
    {
        std::vector<std::string> res = EmitJson(e.args[0], ctx);
        for (size_t i = 1; i < e.args.size(); ++i)
        {
            if (e.args[i].kind == GiacExpression::Neg)
            {
                res.push_back(Symbolic<Real>::JsonMinus());
                auto inner = EmitJson(e.args[i].args[0], ctx);
                res.insert(res.end(), inner.begin(), inner.end());
            }
            else
            {
                res.push_back(Symbolic<Real>::JsonPlus());
                auto term = EmitJson(e.args[i], ctx);
                res.insert(res.end(), term.begin(), term.end());
            }
        }
        return res;
    }
    case GiacExpression::Product:
    {
        FormatContext product_ctx = ctx;
        product_ctx.in_product = true;
        auto emit_factor = 
            [&](const GiacExpression& f) -> std::vector<std::string>
            {
                auto elems = EmitJson(f, product_ctx);
                if (f.kind == GiacExpression::Sum)
                {
                    std::string row = Symbolic<Real>::JsonCodeRow(elems);
                    if (!ContainsImaginaryUnit(f))
                        return {Symbolic<Real>::JsonCodeRow({Symbolic<Real>::JsonOpenRoundBracket(), row, Symbolic<Real>::JsonCloseRoundBracket()})};
                    return {row};
                }
                return elems;
            };
        std::vector<std::string> res = emit_factor(e.args[0]);
        for (size_t i = 1; i < e.args.size(); ++i)
        {
            res.push_back(Symbolic<Real>::JsonMultiply());
            auto f = emit_factor(e.args[i]);
            res.insert(res.end(), f.begin(), f.end());
        }
        return res;
    }
    }
    return {};
}

bool IsCodeStringJson(const std::string& str)
{
    return str.size() >= 10 && str.substr(0, 10) == R"({"type":8,)";
}

std::string ExprToJson(const GiacExpression& ast, int result_type, const FormatContext& ctx)
{
    auto elems = EmitJson(ast, ctx);
    if (elems.size() == 1 && !IsCodeStringJson(elems[0]))
        return Symbolic<Real>::JsonResultWrapper(result_type, elems[0]);
    return Symbolic<Real>::JsonResultWrapper(result_type, Symbolic<Real>::JsonCodeRow(elems));
}

bool IsInfiniteLimit(const giac::gen& lim, giac::context* ctx)
{
    if (lim == giac::plus_inf || lim == giac::minus_inf)
        return true;
    if (lim.type == giac::_IDNT)
    {
        std::string str = lim.print(ctx);
        if (str == "undef" || str == "unsigned_inf" || str == "infinity")
            return true;
    }
    return false;
}

bool HasAmbiguousPoleArgument(const giac::gen& expr, const giac::identificateur& var, const giac::gen& value, giac::context* ctx)
{
    if (expr.type == giac::_SYMB)
    {
        const giac::symbolic& str = *expr._SYMBptr;
        bool ambiguous = (str.sommet == giac::at_exp || str.sommet == giac::at_sin || str.sommet == giac::at_cos || str.sommet == giac::at_sinh || 
            str.sommet == giac::at_cosh);
        if (ambiguous)
        {
            giac::gen lim = giac::limit(str.feuille, var, value, 1, ctx);
            if (IsInfiniteLimit(lim, ctx))
                return true;
        }
        return HasAmbiguousPoleArgument(str.feuille, var, value, ctx);
    }
    if (expr.type == giac::_FRAC)
        return HasAmbiguousPoleArgument(expr._FRACptr->num, var, value, ctx) || HasAmbiguousPoleArgument(expr._FRACptr->den, var, value, ctx);
    if (expr.type == giac::_CPLX)
        return HasAmbiguousPoleArgument(expr._CPLXptr[0], var, value, ctx) || HasAmbiguousPoleArgument(expr._CPLXptr[1], var, value, ctx);
    if (expr.type == giac::_VECT)
    {
        const giac::vecteur& v = *expr._VECTptr;
        for (const auto& e : v)
        {
            if (HasAmbiguousPoleArgument(e, var, value, ctx))
                return true;
        }
        return false;
    }
    return false;
}

bool IsExplicitNegativeInfinity(const giac::gen& g)
{
    return g == giac::minus_inf;
}

}
