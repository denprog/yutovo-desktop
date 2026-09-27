/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "stdafx.h"
#include "parser.h"

namespace yutovo_calculator
{

Integer bin(const std::u32string& str);
Integer oct(const std::u32string& str);
Integer dec(const std::u32string& str);
Integer hex(const std::u32string& str);

Integer pow(const Integer& num1, const Integer& num2);
Integer reminder(const Integer& num1, const Integer& num2);

Integer abs(const Integer &num);

Integer min(const Integer& num1, const Integer& num2);
Integer max(const Integer& num1, const Integer& num2);

Real pow(const Real& num1, const Real& num2);
Complex pow(const Complex& num1, const Complex& num2, int& res_pos);

Rational pow(const Rational& num1, const Rational& num2);

static Real infinity(const int precision)
{
    return Real(precision, "inf");
}

static Complex infinity(const int precision, const AngleMeasure /*angle_measure*/)
{
    return Complex(precision, "inf");
}

Rational abs(const Rational &num);

Rational min(const Rational& num1, const Rational& num2);
Rational max(const Rational& num1, const Rational& num2);

Real sin(const Real& num);
Real cos(const Real& num);
Real tg(const Real& num);
Real ctg(const Real& num);
Real sec(const Real& num);
Real cosec(const Real& num);
Real arcsin(const Real& num);
Real arccos(const Real& num);
Real arctg(const Real& num);
Real arcctg(const Real& num);
Real arcsec(const Real& num);
Real arccosec(const Real& num);
Real sh(const Real& num);
Real ch(const Real& num);
Real th(const Real& num);
Real cth(const Real& num);
Real sch(const Real& num);
Real csch(const Real& num);
Real arsh(const Real& num);
Real arch(const Real& num);
Real arth(const Real& num);
Real arcth(const Real& num);
Real arsch(const Real& num);
Real arcsch(const Real& num);

Real pi(const int precision);
Real exp(const int precision);

Real exp(const Real& num);

Real ln(const Real& num);
Real lg(const Real& num);
Real log(const Real& num1, const Real& num2);
Real sqrt(const Real& num);
Real root(const Real& num1, const Real& num2);

Real integer(const Real &num);
Real fract(const Real &num);
Real abs(const Real &num);
Real min(const Real& num1, const Real& num2);
Real max(const Real& num1, const Real& num2);
Real ceil(const Real &num);
Real floor(const Real &num);
Real round(const Real &num);
Real trunc(const Real &num);

Real rad(const Real& num);
Real deg(const Real& num);
Real minute(const Real& num);
Real second(const Real& num);
Real grad(const Real& num);

Complex sin(const Complex& num, int& res_pos);
Complex cos(const Complex& num, int& res_pos);
Complex tg(const Complex& num, int& res_pos);
Complex ctg(const Complex& num, int& res_pos);
Complex sec(const Complex& num, int& res_pos);
Complex cosec(const Complex& num, int& res_pos);
Complex arcsin(const Complex& num, int& res_pos);
Complex arccos(const Complex& num, int& res_pos);
Complex arctg(const Complex& num, int& res_pos);
Complex arcctg(const Complex& num, int& res_pos);
Complex arcsec(const Complex& num, int& res_pos);
Complex arccosec(const Complex& num, int& res_pos);
Complex sh(const Complex& num, int& res_pos);
Complex ch(const Complex& num, int& res_pos);
Complex th(const Complex& num, int& res_pos);
Complex cth(const Complex& num, int& res_pos);
Complex sch(const Complex& num, int& res_pos);
Complex csch(const Complex& num, int& res_pos);
Complex arsh(const Complex& num, int& res_pos);
Complex arch(const Complex& num, int& res_pos);
Complex arth(const Complex& num, int& res_pos);
Complex arcth(const Complex& num, int& res_pos);
Complex arsch(const Complex& num, int& res_pos);
Complex arcsch(const Complex& num, int& res_pos);

Complex pi_complex(const int precision, AngleMeasure angle_measure);
Complex exp_complex(const int precision, AngleMeasure angle_measure);

Complex exp(const Complex& num, int& res_pos);

Complex ln(const Complex& num, int& res_pos);
Complex lg(const Complex& num, int& res_pos);
Complex log(const Complex& num1, const Complex& num2, int& res_pos);
Complex sqrt(const Complex& num, int& res_pos);
Complex root(const Complex& num1, const Complex& num2, int& res_pos);

Complex abs(const Complex &num);
Complex min(const Complex& num1, const Complex& num2, int& res_pos);
Complex max(const Complex& num1, const Complex& num2, int& res_pos);

Complex rad(const Complex& num, int& res_pos);
Complex deg(const Complex& num, int& res_pos);
Complex minute(const Complex& num, int& res_pos);
Complex second(const Complex& num, int& res_pos);
Complex grad(const Complex& num, int& res_pos);

Complex re(const Complex& num);
Complex im(const Complex& num);
Complex mod(const Complex& num);
Complex arg(const Complex& num);

Array<Real> pi_array_real(const int precision);
Array<Real> exp_array_real(const int precision);

Array<Real> exp(const Array<Real>& num);

Array<Real> pow(const Array<Real>& num1, const Array<Real>& num2);
Array<Real> ln(const Array<Real>& num);
Array<Real> lg(const Array<Real>& num);
Array<Real> log(const Array<Real>& num1, const Array<Real>& num2);
Array<Real> sqrt(const Array<Real>& num);
Array<Real> root(const Array<Real>& num1, const Array<Real>& num2);

Array<Real> integer(const Array<Real> &num);
Array<Real> fract(const Array<Real> &num);
Array<Real> abs(const Array<Real> &num);
Array<Real> ceil(const Array<Real> &num);
Array<Real> floor(const Array<Real> &num);
Array<Real> round(const Array<Real> &num);
Array<Real> trunc(const Array<Real> &num);

Array<Real> sin(const Array<Real>& num);
Array<Real> cos(const Array<Real>& num);
Array<Real> tg(const Array<Real>& num);
Array<Real> ctg(const Array<Real>& num);
Array<Real> sec(const Array<Real>& num);
Array<Real> cosec(const Array<Real>& num);
Array<Real> arcsin(const Array<Real>& num);
Array<Real> arccos(const Array<Real>& num);
Array<Real> arctg(const Array<Real>& num);
Array<Real> arcctg(const Array<Real>& num);
Array<Real> arcsec(const Array<Real>& num);
Array<Real> arccosec(const Array<Real>& num);
Array<Real> sh(const Array<Real>& num);
Array<Real> ch(const Array<Real>& num);
Array<Real> th(const Array<Real>& num);
Array<Real> cth(const Array<Real>& num);
Array<Real> sch(const Array<Real>& num);
Array<Real> csch(const Array<Real>& num);
Array<Real> arsh(const Array<Real>& num);
Array<Real> arch(const Array<Real>& num);
Array<Real> arth(const Array<Real>& num);
Array<Real> arcth(const Array<Real>& num);
Array<Real> arsch(const Array<Real>& num);
Array<Real> arcsch(const Array<Real>& num);

Array<Real> rad(const Array<Real>& num);
Array<Real> deg(const Array<Real>& num);
Array<Real> minute(const Array<Real>& num);
Array<Real> second(const Array<Real>& num);
Array<Real> grad(const Array<Real>& num);

Array<Real> size(const Array<Real> &num);
Array<Real> min(const Array<Real>& num);
Array<Real> max(const Array<Real>& num);

Real definite_integral_real(const Real& a, const Real& b, const Real& c, const Real& d);
Complex definite_integral_complex(const Complex& a, const Complex& b, const Complex& c, const Complex& d);
Array<Real> definite_integral_array(const Array<Real>& a, const Array<Real>& b, const Array<Real>& c, const Array<Real>& d);

Real definite_integral_real(const Real& a, const Real& b, const Real& c, const Real& d)
{
    throw MathException(IncorrectOperation);
}

Complex definite_integral_complex(const Complex& a, const Complex& b, const Complex& c, const Complex& d)
{
    throw MathException(IncorrectOperation);
}

Array<Real> definite_integral_array(const Array<Real>& a, const Array<Real>& b, const Array<Real>& c, const Array<Real>& d)
{
    throw MathException(IncorrectOperation);
}

Symbolic<Real> evalf(const Symbolic<Real>& num);
Symbolic<Real> evalf(const Symbolic<Real>& num, const Symbolic<Real>& prec);
Symbolic<Real> expand(const Symbolic<Real>& num);
Symbolic<Real> simplify(const Symbolic<Real>& num);
Symbolic<Real> derivative(const Symbolic<Real>& num, const Symbolic<Real>& var);
Symbolic<Real> subs(const Symbolic<Real>& num, const Symbolic<Real>& var, const Symbolic<Real>& value);
Symbolic<Real> definite_integral(const Symbolic<Real>& a, const Symbolic<Real>& b, const Symbolic<Real>& expr, const Symbolic<Real>& var);
Symbolic<Real> indefinite_integral(const Symbolic<Real>& expr, const Symbolic<Real>& var);
Symbolic<Real> pow(const Symbolic<Real>& num1, const Symbolic<Real>& num2);
Symbolic<Real> log(const Symbolic<Real>& num1, const Symbolic<Real>& num2);
Symbolic<Real> root(const Symbolic<Real>& num1, const Symbolic<Real>& num2);
Symbolic<Real> sin(const Symbolic<Real>& num);
Symbolic<Real> cos(const Symbolic<Real>& num);
Symbolic<Real> tg(const Symbolic<Real>& num);
Symbolic<Real> cot(const Symbolic<Real>& num);
Symbolic<Real> sec(const Symbolic<Real>& num);
Symbolic<Real> csc(const Symbolic<Real>& num);
Symbolic<Real> arcsin(const Symbolic<Real>& num);
Symbolic<Real> arccos(const Symbolic<Real>& num);
Symbolic<Real> arctg(const Symbolic<Real>& num);
Symbolic<Real> arcctg(const Symbolic<Real>& num);
Symbolic<Real> arcsec(const Symbolic<Real>& num);
Symbolic<Real> arccsc(const Symbolic<Real>& num);
Symbolic<Real> gamma(const Symbolic<Real>& num);
Symbolic<Real> sinh(const Symbolic<Real>& num);
Symbolic<Real> cosh(const Symbolic<Real>& num);
Symbolic<Real> tanh(const Symbolic<Real>& num);
Symbolic<Real> coth(const Symbolic<Real>& num);
Symbolic<Real> sech(const Symbolic<Real>& num);
Symbolic<Real> csch(const Symbolic<Real>& num);
Symbolic<Real> asinh(const Symbolic<Real>& num);
Symbolic<Real> acosh(const Symbolic<Real>& num);
Symbolic<Real> atanh(const Symbolic<Real>& num);
Symbolic<Real> acoth(const Symbolic<Real>& num);
Symbolic<Real> asech(const Symbolic<Real>& num);
Symbolic<Real> acsch(const Symbolic<Real>& num);
Symbolic<Real> exp(const Symbolic<Real>& num);
Symbolic<Real> sqrt(const Symbolic<Real>& num);
Symbolic<Real> ln(const Symbolic<Real>& num);
Symbolic<Real> min(const Symbolic<Real>& num1, const Symbolic<Real>& num2);
Symbolic<Real> max(const Symbolic<Real>& num1, const Symbolic<Real>& num2);

extern template class Symbolic<Rational>;
extern template class Symbolic<Complex>;

Symbolic<Rational> evalf(const Symbolic<Rational>& num);
Symbolic<Rational> evalf(const Symbolic<Rational>& num, const Symbolic<Rational>& prec);
Symbolic<Rational> expand(const Symbolic<Rational>& num);
Symbolic<Rational> simplify(const Symbolic<Rational>& num);
Symbolic<Rational> derivative(const Symbolic<Rational>& num, const Symbolic<Rational>& var);
Symbolic<Rational> subs(const Symbolic<Rational>& num, const Symbolic<Rational>& var, const Symbolic<Rational>& value);
Symbolic<Rational> definite_integral(const Symbolic<Rational>& a, const Symbolic<Rational>& b, const Symbolic<Rational>& expr, const Symbolic<Rational>& var);
Symbolic<Rational> indefinite_integral(const Symbolic<Rational>& expr, const Symbolic<Rational>& var);
Symbolic<Rational> pow(const Symbolic<Rational>& num1, const Symbolic<Rational>& num2);
Symbolic<Rational> log(const Symbolic<Rational>& num1, const Symbolic<Rational>& num2);
Symbolic<Rational> root(const Symbolic<Rational>& num1, const Symbolic<Rational>& num2);
Symbolic<Rational> sin(const Symbolic<Rational>& num);
Symbolic<Rational> cos(const Symbolic<Rational>& num);
Symbolic<Rational> tg(const Symbolic<Rational>& num);
Symbolic<Rational> cot(const Symbolic<Rational>& num);
Symbolic<Rational> sec(const Symbolic<Rational>& num);
Symbolic<Rational> csc(const Symbolic<Rational>& num);
Symbolic<Rational> arcsin(const Symbolic<Rational>& num);
Symbolic<Rational> arccos(const Symbolic<Rational>& num);
Symbolic<Rational> arctg(const Symbolic<Rational>& num);
Symbolic<Rational> arcctg(const Symbolic<Rational>& num);
Symbolic<Rational> arcsec(const Symbolic<Rational>& num);
Symbolic<Rational> arccsc(const Symbolic<Rational>& num);
Symbolic<Rational> gamma(const Symbolic<Rational>& num);
Symbolic<Rational> sinh(const Symbolic<Rational>& num);
Symbolic<Rational> cosh(const Symbolic<Rational>& num);
Symbolic<Rational> tanh(const Symbolic<Rational>& num);
Symbolic<Rational> coth(const Symbolic<Rational>& num);
Symbolic<Rational> sech(const Symbolic<Rational>& num);
Symbolic<Rational> csch(const Symbolic<Rational>& num);
Symbolic<Rational> asinh(const Symbolic<Rational>& num);
Symbolic<Rational> acosh(const Symbolic<Rational>& num);
Symbolic<Rational> atanh(const Symbolic<Rational>& num);
Symbolic<Rational> acoth(const Symbolic<Rational>& num);
Symbolic<Rational> asech(const Symbolic<Rational>& num);
Symbolic<Rational> acsch(const Symbolic<Rational>& num);
Symbolic<Rational> exp(const Symbolic<Rational>& num);
Symbolic<Rational> sqrt(const Symbolic<Rational>& num);
Symbolic<Rational> ln(const Symbolic<Rational>& num);
Symbolic<Rational> min(const Symbolic<Rational>& num1, const Symbolic<Rational>& num2);
Symbolic<Rational> max(const Symbolic<Rational>& num1, const Symbolic<Rational>& num2);

Symbolic<Complex> evalf(const Symbolic<Complex>& num);
Symbolic<Complex> evalf(const Symbolic<Complex>& num, const Symbolic<Complex>& prec);
Symbolic<Complex> expand(const Symbolic<Complex>& num);
Symbolic<Complex> simplify(const Symbolic<Complex>& num);
Symbolic<Complex> derivative(const Symbolic<Complex>& num, const Symbolic<Complex>& var);
Symbolic<Complex> subs(const Symbolic<Complex>& num, const Symbolic<Complex>& var, const Symbolic<Complex>& value);
Symbolic<Complex> definite_integral(const Symbolic<Complex>& a, const Symbolic<Complex>& b, const Symbolic<Complex>& expr, const Symbolic<Complex>& var);
Symbolic<Complex> indefinite_integral(const Symbolic<Complex>& expr, const Symbolic<Complex>& var);
Symbolic<Complex> pow(const Symbolic<Complex>& num1, const Symbolic<Complex>& num2);
Symbolic<Complex> log(const Symbolic<Complex>& num1, const Symbolic<Complex>& num2);
Symbolic<Complex> root(const Symbolic<Complex>& num1, const Symbolic<Complex>& num2);
Symbolic<Complex> sin(const Symbolic<Complex>& num);
Symbolic<Complex> cos(const Symbolic<Complex>& num);
Symbolic<Complex> tg(const Symbolic<Complex>& num);
Symbolic<Complex> cot(const Symbolic<Complex>& num);
Symbolic<Complex> sec(const Symbolic<Complex>& num);
Symbolic<Complex> csc(const Symbolic<Complex>& num);
Symbolic<Complex> arcsin(const Symbolic<Complex>& num);
Symbolic<Complex> arccos(const Symbolic<Complex>& num);
Symbolic<Complex> arctg(const Symbolic<Complex>& num);
Symbolic<Complex> arcctg(const Symbolic<Complex>& num);
Symbolic<Complex> arcsec(const Symbolic<Complex>& num);
Symbolic<Complex> arccsc(const Symbolic<Complex>& num);
Symbolic<Complex> gamma(const Symbolic<Complex>& num);
Symbolic<Complex> sinh(const Symbolic<Complex>& num);
Symbolic<Complex> cosh(const Symbolic<Complex>& num);
Symbolic<Complex> tanh(const Symbolic<Complex>& num);
Symbolic<Complex> coth(const Symbolic<Complex>& num);
Symbolic<Complex> sech(const Symbolic<Complex>& num);
Symbolic<Complex> csch(const Symbolic<Complex>& num);
Symbolic<Complex> asinh(const Symbolic<Complex>& num);
Symbolic<Complex> acosh(const Symbolic<Complex>& num);
Symbolic<Complex> atanh(const Symbolic<Complex>& num);
Symbolic<Complex> acoth(const Symbolic<Complex>& num);
Symbolic<Complex> asech(const Symbolic<Complex>& num);
Symbolic<Complex> acsch(const Symbolic<Complex>& num);
Symbolic<Complex> exp(const Symbolic<Complex>& num);
Symbolic<Complex> sqrt(const Symbolic<Complex>& num);
Symbolic<Complex> ln(const Symbolic<Complex>& num);
Symbolic<Complex> min(const Symbolic<Complex>& num1, const Symbolic<Complex>& num2);
Symbolic<Complex> max(const Symbolic<Complex>& num1, const Symbolic<Complex>& num2);

template<>
Parser<yutovo_calculator::Integer>::Parser(const int precision, const Language _language) : 
    solver(precision, AngleMeasure::Radian),
    language(_language),
    last_language(_language)
{
    InitThreadTime();
    
    IntegerBinaryFunc binary_func = &pow;
    solver.AddBuiltinFunction(U"pow", binary_func);
    binary_func = &reminder;
    solver.AddBuiltinFunction(U"reminder", binary_func);

    IntegerStringFunc string_func = &bin;
    solver.AddBuiltinFunction(U"bin", string_func);
    string_func = &oct;
    solver.AddBuiltinFunction(U"oct", string_func);
    string_func = &dec;
    solver.AddBuiltinFunction(U"dec", string_func);
    string_func = &hex;
    solver.AddBuiltinFunction(U"hex", string_func);

    IntegerUnaryFunc unary_func = &abs;
    solver.AddBuiltinFunction(U"abs", unary_func);

    binary_func = &min;
    solver.AddBuiltinFunction(U"min", binary_func);
    binary_func = &max;
    solver.AddBuiltinFunction(U"max", binary_func);
}

template<>
Parser<yutovo_calculator::Real>::Parser(const int precision, const Language _language) : 
    solver(precision, AngleMeasure::Radian),
    language(_language),
    last_language(_language)
{
    InitThreadTime();
    
    RealPrecisionVariable var;
    var = &pi;
    solver.AddBuiltinVariable(U"pi", var);
    solver.AddBuiltinVariable(std::u32string(1, 0x03C0).c_str(), var); //π
    var = &exp;
    solver.AddBuiltinVariable(U"e", var);
    var = &infinity;
    solver.AddBuiltinVariable(std::u32string(1, 0x221E).c_str(), var); //∞
    
    RealUnaryFunc unary_func;
    unary_func = &exp;
    solver.AddBuiltinFunction(U"exp", unary_func);
    unary_func = &ln;
    solver.AddBuiltinFunction(U"ln", unary_func);
    unary_func = &lg;
    solver.AddBuiltinFunction(U"lg", unary_func);
    unary_func = &sqrt;
    solver.AddBuiltinFunction(U"sqrt", unary_func);
    unary_func = &integer;
    solver.AddBuiltinFunction(U"integer", unary_func);
    unary_func = &fract;
    solver.AddBuiltinFunction(U"fract", unary_func);
    unary_func = &abs;
    solver.AddBuiltinFunction(U"abs", unary_func);
    unary_func = &ceil;
    solver.AddBuiltinFunction(U"ceil", unary_func);
    unary_func = &floor;
    solver.AddBuiltinFunction(U"floor", unary_func);
    unary_func = &round;
    solver.AddBuiltinFunction(U"round", unary_func);
    unary_func = &trunc;
    solver.AddBuiltinFunction(U"trunc", unary_func);

    unary_func = &rad;
    solver.AddBuiltinFunction(U"rad", unary_func);
    unary_func = &deg;
    solver.AddBuiltinFunction(U"deg", unary_func);
    unary_func = &minute;
    solver.AddBuiltinFunction(U"minute", unary_func);
    unary_func = &second;
    solver.AddBuiltinFunction(U"second", unary_func);
    unary_func = &grad;
    solver.AddBuiltinFunction(U"grad", unary_func);

    RealTrigonometricFunc trigonometric_func;
    trigonometric_func = &sin;
    solver.AddTrigonometricFunction(U"sin", trigonometric_func);
    trigonometric_func = &cos;
    solver.AddTrigonometricFunction(U"cos", trigonometric_func);
    trigonometric_func = &tg;
    solver.AddTrigonometricFunction(U"tg", trigonometric_func);
    solver.AddTrigonometricFunction(U"tan", trigonometric_func);
    trigonometric_func = &ctg;
    solver.AddTrigonometricFunction(U"ctg", trigonometric_func);
    solver.AddTrigonometricFunction(U"cot", trigonometric_func);
    trigonometric_func = &sec;
    solver.AddTrigonometricFunction(U"sec", trigonometric_func);
    trigonometric_func = &cosec;
    solver.AddTrigonometricFunction(U"cosec", trigonometric_func);
    solver.AddTrigonometricFunction(U"csc", trigonometric_func);
    trigonometric_func = &arcsin;
    solver.AddBuiltinFunction(U"arcsin", trigonometric_func);
    trigonometric_func = &arccos;
    solver.AddBuiltinFunction(U"arccos", trigonometric_func);
    trigonometric_func = &arctg;
    solver.AddBuiltinFunction(U"arctg", trigonometric_func);
    solver.AddBuiltinFunction(U"arctan", trigonometric_func);
    trigonometric_func = &arcctg;
    solver.AddBuiltinFunction(U"arcctg", trigonometric_func);
    solver.AddBuiltinFunction(U"arccot", trigonometric_func);
    trigonometric_func = &arcsec;
    solver.AddBuiltinFunction(U"arcsec", trigonometric_func);
    trigonometric_func = &arccosec;
    solver.AddBuiltinFunction(U"arccosec", trigonometric_func);
    solver.AddBuiltinFunction(U"arccsc", trigonometric_func);
    trigonometric_func = &sh;
    solver.AddTrigonometricFunction(U"sh", trigonometric_func);
    solver.AddTrigonometricFunction(U"sinh", trigonometric_func);
    trigonometric_func = &ch;
    solver.AddTrigonometricFunction(U"ch", trigonometric_func);
    solver.AddTrigonometricFunction(U"cosh", trigonometric_func);
    trigonometric_func = &th;
    solver.AddTrigonometricFunction(U"th", trigonometric_func);
    solver.AddTrigonometricFunction(U"tanh", trigonometric_func);
    trigonometric_func = &cth;
    solver.AddTrigonometricFunction(U"cth", trigonometric_func);
    solver.AddTrigonometricFunction(U"coth", trigonometric_func);
    trigonometric_func = &sch;
    solver.AddTrigonometricFunction(U"sch", trigonometric_func);
    solver.AddTrigonometricFunction(U"sech", trigonometric_func);
    trigonometric_func = &csch;
    solver.AddTrigonometricFunction(U"csch", trigonometric_func);
    solver.AddTrigonometricFunction(U"cosech", trigonometric_func);
    trigonometric_func = &arsh;
    solver.AddBuiltinFunction(U"arsh", trigonometric_func);
    solver.AddBuiltinFunction(U"arsinh", trigonometric_func);
    trigonometric_func = &arch;
    solver.AddBuiltinFunction(U"arch", trigonometric_func);
    solver.AddBuiltinFunction(U"arcosh", trigonometric_func);
    trigonometric_func = &arth;
    solver.AddBuiltinFunction(U"arth", trigonometric_func);
    solver.AddBuiltinFunction(U"artanh", trigonometric_func);
    trigonometric_func = &arcth;
    solver.AddBuiltinFunction(U"arcth", trigonometric_func);
    solver.AddBuiltinFunction(U"arcoth", trigonometric_func);
    trigonometric_func = &arsch;
    solver.AddBuiltinFunction(U"arsch", trigonometric_func);
    solver.AddBuiltinFunction(U"arsech", trigonometric_func);
    trigonometric_func = &arcsch;
    solver.AddBuiltinFunction(U"arcsch", trigonometric_func);
    solver.AddBuiltinFunction(U"arcosech", trigonometric_func);

    RealBinaryFunc binary_func = &pow;
    solver.AddBuiltinFunction(U"pow", binary_func);
    binary_func = &log;
    solver.AddBuiltinFunction(U"log", binary_func);
    binary_func = &root;
    solver.AddBuiltinFunction(U"root", binary_func);
    binary_func = &min;
    solver.AddBuiltinFunction(U"min", binary_func);
    binary_func = &max;
    solver.AddBuiltinFunction(U"max", binary_func);

    RealQuaternaryFunc quaternary_func = &definite_integral_real;
    solver.AddBuiltinFunction(U"definite_integral", quaternary_func);

    InitUnits();
    InitPhisicalConstants();
}

template<>
Parser<yutovo_calculator::Rational>::Parser(const int precision, const Language _language) : 
    solver(precision, AngleMeasure::Radian),
    language(_language),
    last_language(_language)
{
    InitThreadTime();

    RationalBinaryFunc binary_func = &pow;
    solver.AddBuiltinFunction(U"pow", binary_func);

    RationalUnaryFunc unary_func = &abs;
    solver.AddBuiltinFunction(U"abs", unary_func);

    binary_func = &min;
    solver.AddBuiltinFunction(U"min", binary_func);
    binary_func = &max;
    solver.AddBuiltinFunction(U"max", binary_func);

    InitUnits();
}

template<>
Parser<yutovo_calculator::Complex>::Parser(const int precision, const Language _language) : 
    solver(precision, AngleMeasure::Radian),
    language(_language),
    last_language(_language)
{
    InitThreadTime();
    
    ComplexPrecisionVariable var;
    var = &pi_complex;
    solver.AddBuiltinVariable(U"pi", var);
    var = &exp_complex;
    solver.AddBuiltinVariable(U"e", var);
    var = &infinity;
    solver.AddBuiltinVariable(std::u32string(1, 0x221E).c_str(), var); //∞
    
    ComplexUnaryFunc unary_func;
    unary_func = &exp;
    solver.AddBuiltinFunction(U"exp", unary_func);
    unary_func = &ln;
    solver.AddBuiltinFunction(U"ln", unary_func);
    unary_func = &lg;
    solver.AddBuiltinFunction(U"lg", unary_func);
    unary_func = &sqrt;
    solver.AddBuiltinFunction(U"sqrt", unary_func);

    unary_func = &rad;
    solver.AddBuiltinFunction(U"rad", unary_func);
    unary_func = &deg;
    solver.AddBuiltinFunction(U"deg", unary_func);
    unary_func = &minute;
    solver.AddBuiltinFunction(U"minute", unary_func);
    unary_func = &second;
    solver.AddBuiltinFunction(U"second", unary_func);
    unary_func = &grad;
    solver.AddBuiltinFunction(U"grad", unary_func);

    ComplexFunc func;
    func = &re;
    solver.AddBuiltinFunction(U"re", func);
    func = &im;
    solver.AddBuiltinFunction(U"im", func);
    func = &mod;
    solver.AddBuiltinFunction(U"mod", func);
    func = &arg;
    solver.AddBuiltinFunction(U"arg", func);
    func = &abs;
    solver.AddBuiltinFunction(U"abs", func);

    unary_func = &sin;
    solver.AddBuiltinFunction(U"sin", unary_func);
    unary_func = &cos;
    solver.AddBuiltinFunction(U"cos", unary_func);
    unary_func = &tg;
    solver.AddBuiltinFunction(U"tg", unary_func);
    unary_func = &ctg;
    solver.AddBuiltinFunction(U"ctg", unary_func);
    unary_func = &sec;
    solver.AddBuiltinFunction(U"sec", unary_func);
    unary_func = &cosec;
    solver.AddBuiltinFunction(U"cosec", unary_func);
    unary_func = &arcsin;
    solver.AddBuiltinFunction(U"arcsin", unary_func);
    unary_func = &arccos;
    solver.AddBuiltinFunction(U"arccos", unary_func);
    unary_func = &arctg;
    solver.AddBuiltinFunction(U"arctg", unary_func);
    unary_func = &arcctg;
    solver.AddBuiltinFunction(U"arcctg", unary_func);
    unary_func = &arcsec;
    solver.AddBuiltinFunction(U"arcsec", unary_func);
    unary_func = &arccosec;
    solver.AddBuiltinFunction(U"arccosec", unary_func);
    unary_func = &sh;
    solver.AddBuiltinFunction(U"sh", unary_func);
    unary_func = &ch;
    solver.AddBuiltinFunction(U"ch", unary_func);
    unary_func = &th;
    solver.AddBuiltinFunction(U"th", unary_func);
    unary_func = &cth;
    solver.AddBuiltinFunction(U"cth", unary_func);
    unary_func = &sch;
    solver.AddBuiltinFunction(U"sch", unary_func);
    unary_func = &csch;
    solver.AddBuiltinFunction(U"csch", unary_func);
    unary_func = &arsh;
    solver.AddBuiltinFunction(U"arsh", unary_func);
    unary_func = &arch;
    solver.AddBuiltinFunction(U"arch", unary_func);
    unary_func = &arth;
    solver.AddBuiltinFunction(U"arth", unary_func);
    unary_func = &arcth;
    solver.AddBuiltinFunction(U"arcth", unary_func);
    unary_func = &arsch;
    solver.AddBuiltinFunction(U"arsch", unary_func);
    unary_func = &arcsch;
    solver.AddBuiltinFunction(U"arcsch", unary_func);

    ComplexBinaryFunc binary_func = &pow;
    solver.AddBuiltinFunction(U"pow", binary_func);
    binary_func = &log;
    solver.AddBuiltinFunction(U"log", binary_func);
    binary_func = &root;
    solver.AddBuiltinFunction(U"root", binary_func);
    binary_func = &min;
    solver.AddBuiltinFunction(U"min", binary_func);
    binary_func = &max;
    solver.AddBuiltinFunction(U"max", binary_func);

    ComplexQuaternaryFunc quaternary_func = &definite_integral_complex;
    solver.AddBuiltinFunction(U"definite_integral", quaternary_func);

    switch (language)
    {
    case Language::Russian:
        solver.im = U"j";
        break;
    default:
        solver.im = U"i";
        break;
    }
}

template<>
Parser<yutovo_calculator::Array<yutovo_calculator::Real>>::Parser(const int precision, const Language _language) : 
    solver(precision, AngleMeasure::Radian),
    language(_language)
{
    InitThreadTime();
    
    ArrayRealPrecisionVariable var;
    var = &pi_array_real;
    solver.AddBuiltinVariable(U"pi", var);
    solver.AddBuiltinVariable(std::u32string(1, 0x03C0).c_str(), var); //π
    var = &exp_array_real;
    solver.AddBuiltinVariable(U"e", var);
    
    ArrayRealUnaryFunc unary_func;
    unary_func = &exp;
    solver.AddBuiltinFunction(U"exp", unary_func);
    unary_func = &ln;
    solver.AddBuiltinFunction(U"ln", unary_func);
    unary_func = &lg;
    solver.AddBuiltinFunction(U"lg", unary_func);
    unary_func = &sqrt;
    solver.AddBuiltinFunction(U"sqrt", unary_func);
    unary_func = &integer;
    solver.AddBuiltinFunction(U"integer", unary_func);
    unary_func = &fract;
    solver.AddBuiltinFunction(U"fract", unary_func);
    unary_func = &abs;
    solver.AddBuiltinFunction(U"abs", unary_func);
    unary_func = &ceil;
    solver.AddBuiltinFunction(U"ceil", unary_func);
    unary_func = &floor;
    solver.AddBuiltinFunction(U"floor", unary_func);
    unary_func = &round;
    solver.AddBuiltinFunction(U"round", unary_func);
    unary_func = &trunc;
    solver.AddBuiltinFunction(U"trunc", unary_func);

    unary_func = &rad;
    solver.AddBuiltinFunction(U"rad", unary_func);
    unary_func = &deg;
    solver.AddBuiltinFunction(U"deg", unary_func);
    unary_func = &minute;
    solver.AddBuiltinFunction(U"minute", unary_func);
    unary_func = &second;
    solver.AddBuiltinFunction(U"second", unary_func);
    unary_func = &grad;
    solver.AddBuiltinFunction(U"grad", unary_func);

    unary_func = &size;
    solver.AddBuiltinFunction(U"size", unary_func);
    unary_func = &min;
    solver.AddBuiltinFunction(U"min", unary_func);
    unary_func = &max;
    solver.AddBuiltinFunction(U"max", unary_func);

    ArrayRealTrigonometricFunc trigonometric_func;
    trigonometric_func = &sin;
    solver.AddTrigonometricFunction(U"sin", trigonometric_func);
    trigonometric_func = &cos;
    solver.AddTrigonometricFunction(U"cos", trigonometric_func);
    trigonometric_func = &tg;
    solver.AddTrigonometricFunction(U"tg", trigonometric_func);
    solver.AddTrigonometricFunction(U"tan", trigonometric_func);
    trigonometric_func = &ctg;
    solver.AddTrigonometricFunction(U"ctg", trigonometric_func);
    solver.AddTrigonometricFunction(U"cot", trigonometric_func);
    trigonometric_func = &sec;
    solver.AddTrigonometricFunction(U"sec", trigonometric_func);
    trigonometric_func = &cosec;
    solver.AddTrigonometricFunction(U"cosec", trigonometric_func);
    solver.AddTrigonometricFunction(U"csc", trigonometric_func);
    trigonometric_func = &arcsin;
    solver.AddBuiltinFunction(U"arcsin", trigonometric_func);
    trigonometric_func = &arccos;
    solver.AddBuiltinFunction(U"arccos", trigonometric_func);
    trigonometric_func = &arctg;
    solver.AddBuiltinFunction(U"arctg", trigonometric_func);
    solver.AddBuiltinFunction(U"arctan", trigonometric_func);
    trigonometric_func = &arcctg;
    solver.AddBuiltinFunction(U"arcctg", trigonometric_func);
    solver.AddBuiltinFunction(U"arccot", trigonometric_func);
    trigonometric_func = &arcsec;
    solver.AddBuiltinFunction(U"arcsec", trigonometric_func);
    trigonometric_func = &arccosec;
    solver.AddBuiltinFunction(U"arccosec", trigonometric_func);
    solver.AddBuiltinFunction(U"arccsc", trigonometric_func);
    trigonometric_func = &sh;
    solver.AddTrigonometricFunction(U"sh", trigonometric_func);
    solver.AddTrigonometricFunction(U"sinh", trigonometric_func);
    trigonometric_func = &ch;
    solver.AddTrigonometricFunction(U"ch", trigonometric_func);
    solver.AddTrigonometricFunction(U"cosh", trigonometric_func);
    trigonometric_func = &th;
    solver.AddTrigonometricFunction(U"th", trigonometric_func);
    solver.AddTrigonometricFunction(U"tanh", trigonometric_func);
    trigonometric_func = &cth;
    solver.AddTrigonometricFunction(U"cth", trigonometric_func);
    solver.AddTrigonometricFunction(U"coth", trigonometric_func);
    trigonometric_func = &sch;
    solver.AddTrigonometricFunction(U"sch", trigonometric_func);
    solver.AddTrigonometricFunction(U"sech", trigonometric_func);
    trigonometric_func = &csch;
    solver.AddTrigonometricFunction(U"csch", trigonometric_func);
    solver.AddTrigonometricFunction(U"cosech", trigonometric_func);
    trigonometric_func = &arsh;
    solver.AddBuiltinFunction(U"arsh", trigonometric_func);
    solver.AddBuiltinFunction(U"arsinh", trigonometric_func);
    trigonometric_func = &arch;
    solver.AddBuiltinFunction(U"arch", trigonometric_func);
    solver.AddBuiltinFunction(U"arcosh", trigonometric_func);
    trigonometric_func = &arth;
    solver.AddBuiltinFunction(U"arth", trigonometric_func);
    solver.AddBuiltinFunction(U"artanh", trigonometric_func);
    trigonometric_func = &arcth;
    solver.AddBuiltinFunction(U"arcth", trigonometric_func);
    solver.AddBuiltinFunction(U"arcoth", trigonometric_func);
    trigonometric_func = &arsch;
    solver.AddBuiltinFunction(U"arsch", trigonometric_func);
    solver.AddBuiltinFunction(U"arsech", trigonometric_func);
    trigonometric_func = &arcsch;
    solver.AddBuiltinFunction(U"arcsch", trigonometric_func);
    solver.AddBuiltinFunction(U"arcosech", trigonometric_func);

    ArrayRealBinaryFunc binary_func = &pow;
    solver.AddBuiltinFunction(U"pow", binary_func);
    binary_func = &log;
    solver.AddBuiltinFunction(U"log", binary_func);
    binary_func = &root;
    solver.AddBuiltinFunction(U"root", binary_func);

    ArrayRealQuaternaryFunc quaternary_func = &definite_integral_array;
    solver.AddBuiltinFunction(U"definite_integral", quaternary_func);

    InitUnits();
    InitPhisicalConstants();
}

template<>
Parser<yutovo_calculator::Symbolic<yutovo_calculator::Real>>::Parser(const int precision, const Language _language) :
    solver(precision, AngleMeasure::Radian),
    language(_language),
    last_language(_language)
{
    InitThreadTime();

    giac_context = std::make_unique<giac::context>();
    current_giac_context = giac_context.get();

    SymbolicUnaryFunc unary_func;
    unary_func = &evalf;
    solver.AddBuiltinFunction(U"evalf", unary_func);
    unary_func = &expand;
    solver.AddBuiltinFunction(U"expand", unary_func);
    unary_func = &simplify;
    solver.AddBuiltinFunction(U"simplify", unary_func);
    unary_func = &sin;
    solver.AddBuiltinFunction(U"sin", unary_func);
    unary_func = &cos;
    solver.AddBuiltinFunction(U"cos", unary_func);
    unary_func = &tg;
    solver.AddBuiltinFunction(U"tg", unary_func);
    solver.AddBuiltinFunction(U"tan", unary_func);
    unary_func = &cot;
    solver.AddBuiltinFunction(U"cot", unary_func);
    solver.AddBuiltinFunction(U"ctg", unary_func);
    unary_func = &sec;
    solver.AddBuiltinFunction(U"sec", unary_func);
    unary_func = &csc;
    solver.AddBuiltinFunction(U"csc", unary_func);
    solver.AddBuiltinFunction(U"cosec", unary_func);
    unary_func = &arcsin;
    solver.AddBuiltinFunction(U"arcsin", unary_func);
    solver.AddBuiltinFunction(U"arcsen", unary_func);
    unary_func = &arccos;
    solver.AddBuiltinFunction(U"arccos", unary_func);
    unary_func = &arctg;
    solver.AddBuiltinFunction(U"arctg", unary_func);
    solver.AddBuiltinFunction(U"arctan", unary_func);
    unary_func = &arcctg;
    solver.AddBuiltinFunction(U"arcctg", unary_func);
    solver.AddBuiltinFunction(U"arccot", unary_func);
    unary_func = &arcsec;
    solver.AddBuiltinFunction(U"arcsec", unary_func);
    unary_func = &arccsc;
    solver.AddBuiltinFunction(U"arccsc", unary_func);
    solver.AddBuiltinFunction(U"arccosec", unary_func);
    unary_func = &gamma;
    solver.AddBuiltinFunction(U"gamma", unary_func);
    unary_func = &exp;
    solver.AddBuiltinFunction(U"exp", unary_func);
    unary_func = &sqrt;
    solver.AddBuiltinFunction(U"sqrt", unary_func);
    unary_func = &ln;
    solver.AddBuiltinFunction(U"ln", unary_func);
    unary_func = &sinh;
    solver.AddBuiltinFunction(U"sinh", unary_func);
    solver.AddBuiltinFunction(U"sh", unary_func);
    unary_func = &cosh;
    solver.AddBuiltinFunction(U"cosh", unary_func);
    solver.AddBuiltinFunction(U"ch", unary_func);
    unary_func = &tanh;
    solver.AddBuiltinFunction(U"tanh", unary_func);
    solver.AddBuiltinFunction(U"th", unary_func);
    unary_func = &coth;
    solver.AddBuiltinFunction(U"coth", unary_func);
    solver.AddBuiltinFunction(U"cth", unary_func);
    unary_func = &sech;
    solver.AddBuiltinFunction(U"sech", unary_func);
    solver.AddBuiltinFunction(U"sch", unary_func);
    unary_func = &csch;
    solver.AddBuiltinFunction(U"csch", unary_func);
    solver.AddBuiltinFunction(U"cosech", unary_func);
    unary_func = &asinh;
    solver.AddBuiltinFunction(U"asinh", unary_func);
    solver.AddBuiltinFunction(U"arsinh", unary_func);
    solver.AddBuiltinFunction(U"arcsinh", unary_func);
    unary_func = &acosh;
    solver.AddBuiltinFunction(U"acosh", unary_func);
    solver.AddBuiltinFunction(U"arcosh", unary_func);
    solver.AddBuiltinFunction(U"arccosh", unary_func);
    unary_func = &atanh;
    solver.AddBuiltinFunction(U"atanh", unary_func);
    solver.AddBuiltinFunction(U"artanh", unary_func);
    solver.AddBuiltinFunction(U"arctanh", unary_func);
    unary_func = &acoth;
    solver.AddBuiltinFunction(U"acoth", unary_func);
    solver.AddBuiltinFunction(U"arcoth", unary_func);
    solver.AddBuiltinFunction(U"arccoth", unary_func);
    unary_func = &asech;
    solver.AddBuiltinFunction(U"asech", unary_func);
    solver.AddBuiltinFunction(U"arsech", unary_func);
    solver.AddBuiltinFunction(U"arcsech", unary_func);
    unary_func = &acsch;
    solver.AddBuiltinFunction(U"acsch", unary_func);
    solver.AddBuiltinFunction(U"arcsch", unary_func);
    solver.AddBuiltinFunction(U"arccsch", unary_func);
    solver.AddBuiltinFunction(U"arcosech", unary_func);
    solver.AddBuiltinFunction(U"arccosech", unary_func);

    SymbolicBinaryFunc binary_func;
    binary_func = &derivative;
    solver.AddBuiltinFunction(U"derivative", binary_func);
    solver.AddBuiltinFunction(U"diff", binary_func);
    binary_func = &indefinite_integral;
    solver.AddBuiltinFunction(U"indefinite_integral", binary_func);
    binary_func = &pow;
    solver.AddBuiltinFunction(U"pow", binary_func);
    binary_func = &log;
    solver.AddBuiltinFunction(U"log", binary_func);
    binary_func = &root;
    solver.AddBuiltinFunction(U"root", binary_func);
    binary_func = &min;
    solver.AddBuiltinFunction(U"min", binary_func);
    binary_func = &max;
    solver.AddBuiltinFunction(U"max", binary_func);

    SymbolicTernaryFunc ternary_func;
    ternary_func = &subs;
    solver.AddBuiltinFunction(U"subs", ternary_func);

    SymbolicQuaternaryFunc quaternary_func;
    quaternary_func = &definite_integral;
    solver.AddBuiltinFunction(U"definite_integral", quaternary_func);

    solver.AddBuiltinIdentifier(U"inf", Symbolic<Real>(precision, "+infinity"));
    solver.AddBuiltinIdentifier(U"infinity", Symbolic<Real>(precision, "+infinity"));
    solver.AddBuiltinIdentifier(std::u32string(1, 0x221E).c_str(), Symbolic<Real>(precision, "+infinity"));
    solver.AddBuiltinIdentifier(U"nan", Symbolic<Real>(precision, "undef"));

    switch (language)
    {
    case Language::Russian:
        solver.im = U"j";
        break;
    default:
        solver.im = U"i";
        break;
    }
}

template<>
void Parser<yutovo_calculator::Integer>::SetLocale(Language _language)
{
    language = _language;
    last_language = language;
}

template<>
void Parser<yutovo_calculator::Real>::SetLocale(Language _language)
{
    language = _language;
    InitUnits();
    InitPhisicalConstants();
    last_language = language;
}

template<>
void Parser<yutovo_calculator::Rational>::SetLocale(Language _language)
{
    language = _language;
    InitUnits();
    last_language = language;
}

template<>
void Parser<yutovo_calculator::Complex>::SetLocale(Language _language)
{
    language = _language;
    switch (language)
    {
    case Language::Russian:
        solver.im = U"j";
        break;
    default:
        solver.im = U"i";
        break;
    }
    last_language = language;
}

template<>
void Parser<yutovo_calculator::Array<Real>>::SetLocale(Language _language)
{
    language = _language;
    InitUnits();
    InitPhisicalConstants();
    last_language = language;
}

template<>
void Parser<yutovo_calculator::Symbolic<yutovo_calculator::Real>>::SetLocale(Language _language)
{
    language = _language;
    switch (language)
    {
    case Language::Russian:
        solver.im = U"j";
        break;
    default:
        solver.im = U"i";
        break;
    }
    last_language = language;
}

template<>
Parser<yutovo_calculator::Symbolic<yutovo_calculator::Rational>>::Parser(const int precision, const Language _language) :
    solver(precision, AngleMeasure::Radian),
    language(_language),
    last_language(_language)
{
    InitThreadTime();

    giac_context = std::make_unique<giac::context>();
    current_giac_context = giac_context.get();

    SymbolicRationalUnaryFunc unary_func;
    unary_func = &expand;
    solver.AddBuiltinFunction(U"expand", unary_func);
    unary_func = &simplify;
    solver.AddBuiltinFunction(U"simplify", unary_func);
    unary_func = &sin;
    solver.AddBuiltinFunction(U"sin", unary_func);
    unary_func = &cos;
    solver.AddBuiltinFunction(U"cos", unary_func);
    unary_func = &tg;
    solver.AddBuiltinFunction(U"tg", unary_func);
    solver.AddBuiltinFunction(U"tan", unary_func);
    unary_func = &cot;
    solver.AddBuiltinFunction(U"cot", unary_func);
    solver.AddBuiltinFunction(U"ctg", unary_func);
    unary_func = &sec;
    solver.AddBuiltinFunction(U"sec", unary_func);
    unary_func = &csc;
    solver.AddBuiltinFunction(U"csc", unary_func);
    solver.AddBuiltinFunction(U"cosec", unary_func);
    unary_func = &arcsin;
    solver.AddBuiltinFunction(U"arcsin", unary_func);
    solver.AddBuiltinFunction(U"arcsen", unary_func);
    unary_func = &arccos;
    solver.AddBuiltinFunction(U"arccos", unary_func);
    unary_func = &arctg;
    solver.AddBuiltinFunction(U"arctg", unary_func);
    solver.AddBuiltinFunction(U"arctan", unary_func);
    unary_func = &arcctg;
    solver.AddBuiltinFunction(U"arcctg", unary_func);
    solver.AddBuiltinFunction(U"arccot", unary_func);
    unary_func = &arcsec;
    solver.AddBuiltinFunction(U"arcsec", unary_func);
    unary_func = &arccsc;
    solver.AddBuiltinFunction(U"arccsc", unary_func);
    solver.AddBuiltinFunction(U"arccosec", unary_func);
    unary_func = &gamma;
    solver.AddBuiltinFunction(U"gamma", unary_func);
    unary_func = &exp;
    solver.AddBuiltinFunction(U"exp", unary_func);
    unary_func = &sqrt;
    solver.AddBuiltinFunction(U"sqrt", unary_func);
    unary_func = &ln;
    solver.AddBuiltinFunction(U"ln", unary_func);
    unary_func = &sinh;
    solver.AddBuiltinFunction(U"sinh", unary_func);
    solver.AddBuiltinFunction(U"sh", unary_func);
    unary_func = &cosh;
    solver.AddBuiltinFunction(U"cosh", unary_func);
    solver.AddBuiltinFunction(U"ch", unary_func);
    unary_func = &tanh;
    solver.AddBuiltinFunction(U"tanh", unary_func);
    solver.AddBuiltinFunction(U"th", unary_func);
    unary_func = &coth;
    solver.AddBuiltinFunction(U"coth", unary_func);
    solver.AddBuiltinFunction(U"cth", unary_func);
    unary_func = &sech;
    solver.AddBuiltinFunction(U"sech", unary_func);
    solver.AddBuiltinFunction(U"sch", unary_func);
    unary_func = &csch;
    solver.AddBuiltinFunction(U"csch", unary_func);
    solver.AddBuiltinFunction(U"cosech", unary_func);
    unary_func = &asinh;
    solver.AddBuiltinFunction(U"asinh", unary_func);
    solver.AddBuiltinFunction(U"arsinh", unary_func);
    solver.AddBuiltinFunction(U"arcsinh", unary_func);
    unary_func = &acosh;
    solver.AddBuiltinFunction(U"acosh", unary_func);
    solver.AddBuiltinFunction(U"arcosh", unary_func);
    solver.AddBuiltinFunction(U"arccosh", unary_func);
    unary_func = &atanh;
    solver.AddBuiltinFunction(U"atanh", unary_func);
    solver.AddBuiltinFunction(U"artanh", unary_func);
    solver.AddBuiltinFunction(U"arctanh", unary_func);
    unary_func = &acoth;
    solver.AddBuiltinFunction(U"acoth", unary_func);
    solver.AddBuiltinFunction(U"arcoth", unary_func);
    solver.AddBuiltinFunction(U"arccoth", unary_func);
    unary_func = &asech;
    solver.AddBuiltinFunction(U"asech", unary_func);
    solver.AddBuiltinFunction(U"arsech", unary_func);
    solver.AddBuiltinFunction(U"arcsech", unary_func);
    unary_func = &acsch;
    solver.AddBuiltinFunction(U"acsch", unary_func);
    solver.AddBuiltinFunction(U"arcsch", unary_func);
    solver.AddBuiltinFunction(U"arccsch", unary_func);
    solver.AddBuiltinFunction(U"arcosech", unary_func);
    solver.AddBuiltinFunction(U"arccosech", unary_func);

    SymbolicRationalBinaryFunc binary_func;
    binary_func = &derivative;
    solver.AddBuiltinFunction(U"derivative", binary_func);
    solver.AddBuiltinFunction(U"diff", binary_func);
    binary_func = &indefinite_integral;
    solver.AddBuiltinFunction(U"indefinite_integral", binary_func);
    binary_func = &pow;
    solver.AddBuiltinFunction(U"pow", binary_func);
    binary_func = &log;
    solver.AddBuiltinFunction(U"log", binary_func);
    binary_func = &root;
    solver.AddBuiltinFunction(U"root", binary_func);
    binary_func = &min;
    solver.AddBuiltinFunction(U"min", binary_func);
    binary_func = &max;
    solver.AddBuiltinFunction(U"max", binary_func);

    SymbolicRationalTernaryFunc ternary_func;
    ternary_func = &subs;
    solver.AddBuiltinFunction(U"subs", ternary_func);

    SymbolicRationalQuaternaryFunc quaternary_func;
    quaternary_func = &definite_integral;
    solver.AddBuiltinFunction(U"definite_integral", quaternary_func);

    solver.AddBuiltinIdentifier(U"inf", Symbolic<Rational>(precision, "+infinity"));
    solver.AddBuiltinIdentifier(U"infinity", Symbolic<Rational>(precision, "+infinity"));
    solver.AddBuiltinIdentifier(std::u32string(1, 0x221E).c_str(), Symbolic<Rational>(precision, "+infinity"));
    solver.AddBuiltinIdentifier(U"nan", Symbolic<Rational>(precision, "undef"));

    switch (language)
    {
    case Language::Russian:
        solver.im = U"j";
        break;
    default:
        solver.im = U"i";
        break;
    }
}

template<>
void Parser<yutovo_calculator::Symbolic<yutovo_calculator::Rational>>::SetLocale(Language _language)
{
    language = _language;
    switch (language)
    {
    case Language::Russian:
        solver.im = U"j";
        break;
    default:
        solver.im = U"i";
        break;
    }
    last_language = language;
}

template<>
Parser<yutovo_calculator::Symbolic<yutovo_calculator::Complex>>::Parser(const int precision, const Language _language) :
    solver(precision, AngleMeasure::Radian),
    language(_language),
    last_language(_language)
{
    InitThreadTime();

    giac_context = std::make_unique<giac::context>();
    current_giac_context = giac_context.get();

    SymbolicComplexUnaryFunc unary_func;
    unary_func = &evalf;
    solver.AddBuiltinFunction(U"evalf", unary_func);
    unary_func = &expand;
    solver.AddBuiltinFunction(U"expand", unary_func);
    unary_func = &simplify;
    solver.AddBuiltinFunction(U"simplify", unary_func);
    unary_func = &sin;
    solver.AddBuiltinFunction(U"sin", unary_func);
    unary_func = &cos;
    solver.AddBuiltinFunction(U"cos", unary_func);
    unary_func = &tg;
    solver.AddBuiltinFunction(U"tg", unary_func);
    solver.AddBuiltinFunction(U"tan", unary_func);
    unary_func = &cot;
    solver.AddBuiltinFunction(U"cot", unary_func);
    solver.AddBuiltinFunction(U"ctg", unary_func);
    unary_func = &sec;
    solver.AddBuiltinFunction(U"sec", unary_func);
    unary_func = &csc;
    solver.AddBuiltinFunction(U"csc", unary_func);
    solver.AddBuiltinFunction(U"cosec", unary_func);
    unary_func = &arcsin;
    solver.AddBuiltinFunction(U"arcsin", unary_func);
    solver.AddBuiltinFunction(U"arcsen", unary_func);
    unary_func = &arccos;
    solver.AddBuiltinFunction(U"arccos", unary_func);
    unary_func = &arctg;
    solver.AddBuiltinFunction(U"arctg", unary_func);
    solver.AddBuiltinFunction(U"arctan", unary_func);
    unary_func = &arcctg;
    solver.AddBuiltinFunction(U"arcctg", unary_func);
    solver.AddBuiltinFunction(U"arccot", unary_func);
    unary_func = &arcsec;
    solver.AddBuiltinFunction(U"arcsec", unary_func);
    unary_func = &arccsc;
    solver.AddBuiltinFunction(U"arccsc", unary_func);
    solver.AddBuiltinFunction(U"arccosec", unary_func);
    unary_func = &gamma;
    solver.AddBuiltinFunction(U"gamma", unary_func);
    unary_func = &exp;
    solver.AddBuiltinFunction(U"exp", unary_func);
    unary_func = &sqrt;
    solver.AddBuiltinFunction(U"sqrt", unary_func);
    unary_func = &ln;
    solver.AddBuiltinFunction(U"ln", unary_func);
    unary_func = &sinh;
    solver.AddBuiltinFunction(U"sinh", unary_func);
    solver.AddBuiltinFunction(U"sh", unary_func);
    unary_func = &cosh;
    solver.AddBuiltinFunction(U"cosh", unary_func);
    solver.AddBuiltinFunction(U"ch", unary_func);
    unary_func = &tanh;
    solver.AddBuiltinFunction(U"tanh", unary_func);
    solver.AddBuiltinFunction(U"th", unary_func);
    unary_func = &coth;
    solver.AddBuiltinFunction(U"coth", unary_func);
    solver.AddBuiltinFunction(U"cth", unary_func);
    unary_func = &sech;
    solver.AddBuiltinFunction(U"sech", unary_func);
    solver.AddBuiltinFunction(U"sch", unary_func);
    unary_func = &csch;
    solver.AddBuiltinFunction(U"csch", unary_func);
    solver.AddBuiltinFunction(U"cosech", unary_func);
    unary_func = &asinh;
    solver.AddBuiltinFunction(U"asinh", unary_func);
    solver.AddBuiltinFunction(U"arsinh", unary_func);
    solver.AddBuiltinFunction(U"arcsinh", unary_func);
    unary_func = &acosh;
    solver.AddBuiltinFunction(U"acosh", unary_func);
    solver.AddBuiltinFunction(U"arcosh", unary_func);
    solver.AddBuiltinFunction(U"arccosh", unary_func);
    unary_func = &atanh;
    solver.AddBuiltinFunction(U"atanh", unary_func);
    solver.AddBuiltinFunction(U"artanh", unary_func);
    solver.AddBuiltinFunction(U"arctanh", unary_func);
    unary_func = &acoth;
    solver.AddBuiltinFunction(U"acoth", unary_func);
    solver.AddBuiltinFunction(U"arcoth", unary_func);
    solver.AddBuiltinFunction(U"arccoth", unary_func);
    unary_func = &asech;
    solver.AddBuiltinFunction(U"asech", unary_func);
    solver.AddBuiltinFunction(U"arsech", unary_func);
    solver.AddBuiltinFunction(U"arcsech", unary_func);
    unary_func = &acsch;
    solver.AddBuiltinFunction(U"acsch", unary_func);
    solver.AddBuiltinFunction(U"arcsch", unary_func);
    solver.AddBuiltinFunction(U"arccsch", unary_func);
    solver.AddBuiltinFunction(U"arcosech", unary_func);
    solver.AddBuiltinFunction(U"arccosech", unary_func);

    SymbolicComplexBinaryFunc binary_func;
    binary_func = &derivative;
    solver.AddBuiltinFunction(U"derivative", binary_func);
    solver.AddBuiltinFunction(U"diff", binary_func);
    binary_func = &indefinite_integral;
    solver.AddBuiltinFunction(U"indefinite_integral", binary_func);
    binary_func = &pow;
    solver.AddBuiltinFunction(U"pow", binary_func);
    binary_func = &log;
    solver.AddBuiltinFunction(U"log", binary_func);
    binary_func = &root;
    solver.AddBuiltinFunction(U"root", binary_func);
    binary_func = &min;
    solver.AddBuiltinFunction(U"min", binary_func);
    binary_func = &max;
    solver.AddBuiltinFunction(U"max", binary_func);

    SymbolicComplexTernaryFunc ternary_func;
    ternary_func = &subs;
    solver.AddBuiltinFunction(U"subs", ternary_func);

    SymbolicComplexQuaternaryFunc quaternary_func;
    quaternary_func = &definite_integral;
    solver.AddBuiltinFunction(U"definite_integral", quaternary_func);

    solver.AddBuiltinIdentifier(U"inf", Symbolic<Complex>(precision, "+infinity"));
    solver.AddBuiltinIdentifier(U"infinity", Symbolic<Complex>(precision, "+infinity"));
    solver.AddBuiltinIdentifier(std::u32string(1, 0x221E).c_str(), Symbolic<Complex>(precision, "+infinity"));
    solver.AddBuiltinIdentifier(U"nan", Symbolic<Complex>(precision, "undef"));

    switch (language)
    {
    case Language::Russian:
        solver.im = U"j";
        break;
    default:
        solver.im = U"i";
        break;
    }
}

template<>
void Parser<yutovo_calculator::Symbolic<yutovo_calculator::Complex>>::SetLocale(Language _language)
{
    language = _language;
    switch (language)
    {
    case Language::Russian:
        solver.im = U"j";
        break;
    default:
        solver.im = U"i";
        break;
    }
    last_language = language;
}

template<>
void Parser<yutovo_calculator::Real>::InitBuiltinIdentifiers()
{
    solver.ResetBuiltinIdentifiers();
    switch (language)
    {
    case Language::English:
    case Language::Spanish:
    case Language::BrazilianPortuguese:
        {
            Real v;
            v = 1;
            v.angle_measure = AngleMeasure::Radian;
            solver.AddBuiltinIdentifier(U"rad", v); //radian (angle)
            v.angle_measure = AngleMeasure::Degree;
            solver.AddBuiltinIdentifier(std::u32string(1, 0x00B0).c_str(), v); //° - degree (angle)
            solver.AddBuiltinIdentifier(U"deg", v); //degree (angle)
            solver.AddBuiltinIdentifier(U"'", v);
            solver.AddBuiltinIdentifier(U"''", v);
            v.angle_measure = AngleMeasure::Grad;
            solver.AddBuiltinIdentifier(U"grad", v); //grad (angle)
        }
        break;
    case Language::Russian:
        {
            Real v;
            v = 1;
            v.angle_measure = AngleMeasure::Radian;
            solver.AddBuiltinIdentifier(U"рад", v); //radian (angle)
            v.angle_measure = AngleMeasure::Degree;
            solver.AddBuiltinIdentifier(std::u32string(1, 0x00B0).c_str(), v); //° - degree (angle)
            solver.AddBuiltinIdentifier(U"'", v);
            solver.AddBuiltinIdentifier(U"''", v);
            v.angle_measure = AngleMeasure::Grad;
            solver.AddBuiltinIdentifier(U"град", v); //grad (angle)
        }
        break;
    default:
        throw ParserException({}, ParserExceptionCode::UnknownLanguage);
    }
}

template<>
void Parser<yutovo_calculator::Array<Real>>::InitBuiltinIdentifiers()
{
    solver.ResetBuiltinIdentifiers();
    switch (language)
    {
    case Language::English:
    case Language::Spanish:
    case Language::BrazilianPortuguese:
        {
            Real v;
            v = 1;
            v.angle_measure = AngleMeasure::Radian;
            solver.AddBuiltinIdentifier(U"rad", v); //radian (angle)
            v.angle_measure = AngleMeasure::Degree;
            solver.AddBuiltinIdentifier(std::u32string(1, 0x00B0).c_str(), v); //° - degree (angle)
            solver.AddBuiltinIdentifier(U"deg", v); //degree (angle)
            solver.AddBuiltinIdentifier(U"'", v);
            solver.AddBuiltinIdentifier(U"''", v);
            v.angle_measure = AngleMeasure::Grad;
            solver.AddBuiltinIdentifier(U"grad", v); //grad (angle)
        }
        break;
    case Language::Russian:
        {
            Real v;
            v = 1;
            v.angle_measure = AngleMeasure::Radian;
            solver.AddBuiltinIdentifier(U"рад", v); //radian (angle)
            v.angle_measure = AngleMeasure::Degree;
            solver.AddBuiltinIdentifier(std::u32string(1, 0x00B0).c_str(), v); //° - degree (angle)
            solver.AddBuiltinIdentifier(U"'", v);
            solver.AddBuiltinIdentifier(U"''", v);
            v.angle_measure = AngleMeasure::Grad;
            solver.AddBuiltinIdentifier(U"град", v); //grad (angle)
        }
        break;
    default:
        throw ParserException({}, ParserExceptionCode::UnknownLanguage);
    }
}

};
