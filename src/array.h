/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __ARRAY_H__
#define __ARRAY_H__

#include "real.h"
#include <algorithm>

namespace yutovo_calculator
{

struct ParserContext;
void CheckBreak(ParserContext* parser_context);

Real exp(const Real& num);
Real ln(const Real& num);
Real lg(const Real& num);
Real log(const Real& num1, const Real& num2);

Real pow(const Real& num1, const Real& num2);
Real sqr(const Real& num);
Real sqrt(const Real& num);
Real root(const Real& num1, const Real& num2);

Real abs(const Real& num);
Real floor(const Real& num);
Real trunc(const Real& num);
Real ceil(const Real& num);
Real round(const Real& num);
Real integer(const Real& num);
Real fract(const Real &num);
Real fact(const Real &num, ParserContext* parser_context = nullptr);
Real roundoff(const Real& num);

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

Real rad(const Real& num);
Real deg(const Real& num);
Real minute(const Real& num);
Real second(const Real& num);
Real grad(const Real& num);

template<class Number>
class Array
{
public:
    Array()
    {
    }

    Array(int precision)
    {
        numbers.emplace_back(precision);
    }

    Array(int precision, Unit& _unit)
    {
        numbers.emplace_back(precision, _unit);
    }

    Array(int precision, AngleMeasure _angle_measure)
    {
        numbers.emplace_back(precision, _angle_measure);
    }

    Array(int precision, const char* num)
    {
        numbers.emplace_back(precision, num);
    }

    Array(int precision, int num)
    {
        numbers.emplace_back(precision, num);
    }

    Array(int precision, float num)
    {
        numbers.emplace_back(precision, num);
    }

    Array(const std::u32string& num)
    {
        numbers.emplace_back(num);
    }

    Array(const Number& num)
    {
        numbers.emplace_back(num);
    }

    Array(const Array& source) :
        numbers(source.numbers)
    {
    }

public:
    typedef Array<Number> value_type;
    
    Array<Number>& operator=(const Array<Number>& num)
    {
        numbers = num.numbers;
        return *this;
    }

    Array<Number>& operator=(const Number& num)
    {
        numbers.clear();
        numbers.push_back(num);
        return *this;
    }

    Array<Number>& operator=(const int num)
    {
        numbers.clear();
        numbers.emplace_back(num);
        return *this;
    }

    Array<Number>& operator=(const double num)
    {
        numbers.clear();
        numbers.emplace_back(num);
        return *this;
    }

    Array<Number>& operator=(const char* source)
    {
        numbers.clear();
        numbers.emplace_back(source);
        return *this;
    }

    Array<Number>& operator=(const std::u32string& num)
    {
        numbers.clear();
        numbers.emplace_back(num);
        return *this;
    }

    const Number& operator[](size_t index) const
    {
        return numbers[index];
    }

    Array<Number> operator+()
    {
        if (numbers.size() != 1)
            throw MathException(IncorrectOperation);
        return *this;
    }

    Array<Number> operator-()
    {
        if (numbers.size() != 1)
            throw MathException(IncorrectOperation);
        Array<Number> res(*this);
        res.numbers[0] = -res.numbers[0];
        return res;
    }

    friend Array<Number> operator+(const Array<Number>& num1, const Array<Number>& num2)
    {
        if (num1.numbers.size() == 1 && num1 == 0)
            return num2;
        if (num1.numbers.size() != num2.numbers.size())
            throw MathException(IncorrectOperation);
        Array<Number> res(num1);
        for (size_t i = 0; i < num1.numbers.size(); ++i)
            res.numbers[i] += num2.numbers[i];
        return res;
    }

    friend Array<Number> operator+(const Array<Number>& num1, const float num2)
    {
        if (num1.numbers.size() != 1)
            throw MathException(IncorrectOperation);
        Array<Number> res(num1);
        res.numbers[0] += num2;
        return res;
    }

    friend Array<Number> operator+(const float num1, const Array<Number>& num2)
    {
        if (num2.numbers.size() != 1)
            throw MathException(IncorrectOperation);
        Array<Number> res(num2);
        res.numbers[0] = num1 + num2.numbers[0];
        return res;
    }

    friend Array<Number> operator-(const Array<Number>& num1, const Array<Number>& num2)
    {
        if (num2.numbers.size() == 1 && num2 == 0)
            return num1;
        if (num1.numbers.size() != num2.numbers.size())
            throw MathException(IncorrectOperation);
        Array<Number> res(num1);
        for (size_t i = 0; i < num1.numbers.size(); ++i)
            res.numbers[i] -= num2.numbers[i];
        return res;
    }

    friend Array<Number> operator-(const Array<Number>& num1, const float num2)
    {
        if (num1.numbers.size() != 1)
            throw MathException(IncorrectOperation);
        Array<Number> res(num1);
        res.numbers[0] -= num2;
        return res;
    }

    friend Array<Number> operator-(const float num1, const Array<Number>& num2)
    {
        if (num2.numbers.size() != 1)
            throw MathException(IncorrectOperation);
        Array<Number> res(num2);
        res.numbers[0] = num1 - num2.numbers[0];
        return res;
    }

    friend Array<Number> operator*(const Array<Number>& num1, const Array<Number>& num2)
    {
        if (num1.numbers.size() == 1 && num1 == 1 && num1.numbers[0].unit.IsEmpty())
            return num2;
        if (num1.numbers.size() != num2.numbers.size())
            throw MathException(IncorrectOperation);
        Array<Number> res(num1);
        for (size_t i = 0; i < num1.numbers.size(); ++i)
            res.numbers[i] *= num2.numbers[i];
        return res;
    }

    friend Array<Number> operator*(const Array<Number>& num1, const float num2)
    {
        if (num1.numbers.size() != 1)
            throw MathException(IncorrectOperation);
        Array<Number> res(num1);
        res.numbers[0] *= num2;
        return res;
    }

    friend Array<Number> operator*(const float num1, const Array<Number>& num2)
    {
        if (num2.numbers.size() != 1)
            throw MathException(IncorrectOperation);
        Array<Number> res(num2);
        res.numbers[0] = num1 * num2.numbers[0];
        return res;
    }

    friend Array<Number> operator/(const Array<Number>& num1, const Array<Number>& num2)
    {
        if (num2.numbers.size() == 1 && num2 == 1 && num2.numbers[0].unit.IsEmpty())
            return num1;
        if (num1.numbers.size() != num2.numbers.size())
            throw MathException(IncorrectOperation);
        Array<Number> res(num1);
        for (size_t i = 0; i < num1.numbers.size(); ++i)
            res.numbers[i] /= num2.numbers[i];
        return res;
    }

    friend Array<Number> operator/(const Array<Number>& num1, const float num2)
    {
        if (num1.numbers.size() != 1)
            throw MathException(IncorrectOperation);
        Array<Number> res(num1);
        res.numbers[0] /= num2;
        return res;
    }

    friend Array<Number> operator/(const float num1, const Array<Number>& num2)
    {
        if (num2.numbers.size() != 1)
            throw MathException(IncorrectOperation);
        Array<Number> res(num2);
        res.numbers[0] = num1 / num2.numbers[0];
        return res;
    }

    void operator+=(const Array<Number>& num)
    {
        *this = *this + num;
    }

    void operator-=(const Array<Number>& num)
    {
        *this = *this - num;
    }

    void operator*=(const Array<Number>& num)
    {
        *this = *this * num;
    }

    void operator/=(const Array<Number>& num)
    {
        *this = *this / num;
    }

public:
    friend bool operator==(const Array<Number>& num1, const Array<Number>& num2)
    {
        if (num1.numbers.size() != num2.numbers.size())
            throw MathException(IncorrectOperation);
        for (size_t i = 0; i < num1.numbers.size(); ++i)
        {
            if (num1.numbers[i] != num2.numbers[i])
                return false;
        }
        return true;
    }

    friend bool operator==(const Array<Number>& num1, const int num2)
    {
        if (num1.numbers.size() != 1)
            throw MathException(IncorrectOperation);
        return num1.numbers[0] == num2;
    }

    friend bool operator!=(const Array<Number>& num1, const Array<Number>& num2)
    {
        return !(operator==(num1, num2));
    }

    friend bool operator!=(const Array<Number>& num1, const int num2)
    {
        return !(operator==(num1, num2));
    }

    friend bool operator>(const Array<Number>& num1, const Array<Number>& num2)
    {
        if (num1.numbers.size() != num2.numbers.size())
            throw MathException(IncorrectComparison);
        for (size_t i = 0; i < num1.numbers.size(); ++i)
        {
            if (num1.numbers[i] <= num2.numbers[i])
                return false;
        }
        return true;
    }

    friend bool operator>=(const Array<Number>& num1, const Array<Number>& num2)
    {
        if (num1.numbers.size() != num2.numbers.size())
            throw MathException(IncorrectComparison);
        for (size_t i = 0; i < num1.numbers.size(); ++i)
        {
            if (num1.numbers[i] < num2.numbers[i])
                return false;
        }
        return true;
    }

    friend bool operator<(const Array<Number>& num1, const Array<Number>& num2)
    {
        if (num1.numbers.size() != num2.numbers.size())
            throw MathException(IncorrectComparison);
        for (size_t i = 0; i < num1.numbers.size(); ++i)
        {
            if (num1.numbers[i] >= num2.numbers[i])
                return false;
        }
        return true;
    }

    friend bool operator<=(const Array<Number>& num1, const Array<Number>& num2)
    {
        if (num1.numbers.size() != num2.numbers.size())
            throw MathException(IncorrectComparison);
        for (size_t i = 0; i < num1.numbers.size(); ++i)
        {
            if (num1.numbers[i] > num2.numbers[i])
                return false;
        }
        return true;
    }

private:
    friend Array<Number> ForEach(Number (*func)(const Number&), const Array<Number>& num)
    {
        Array<Number> r;
        for (auto& n : num.numbers)
            r.numbers.push_back(func(n));
        return r;
    }

    friend Array<Number> ForEach(Number (*func)(const Number&, const Number&), const Array<Number>& num1, const Array<Number>& num2)
    {
        if (num1.numbers.size() != num2.numbers.size())
            throw MathException(IncorrectOperation);
        Array<Number> r;
        for (size_t i = 0; i < num1.numbers.size(); ++i)
            r.numbers.push_back(func(num1.numbers[i], num2.numbers[i]));
        return r;
    }

public:
    friend Array<Number> exp(const Array<Number>& num)
    {
        return ForEach(exp, num);
    }

    friend Array<Number> ln(const Array<Number>& num)
    {
        return ForEach(ln, num);
    }

    friend Array<Number> lg(const Array<Number>& num)
    {
        return ForEach(lg, num);
    }

    friend Array<Number> log(const Array<Number>& num1, const Array<Number>& num2)
    {
        return ForEach(log, num1, num2);
    }

    friend Array<Number> pow(const Array<Number>& num1, const Array<Number>& num2)
    {
        return ForEach(pow, num1, num2);
    }

    friend Array<Number> sqr(const Array<Number>& num)
    {
        return ForEach(sqr, num);
    }

    friend Array<Number> sqrt(const Array<Number>& num)
    {
        return ForEach(sqrt, num);
    }

    friend Array<Number> root(const Array<Number>& num1, const Array<Number>& num2)
    {
        if (num1.numbers.size() != num2.numbers.size())
            throw MathException(IncorrectOperation);
        Array<Number> r;
        for (size_t i = 0; i < num1.numbers.size(); ++i)
            r.numbers.push_back(root(num1.numbers[i], num2.numbers[i]));
        return r;
    }

    friend Array<Number> abs(const Array<Number>& num)
    {
        return ForEach(abs, num);
    }

    friend Array<Number> min(const Array<Number>& num)
    {
        if (num.numbers.empty())
            throw MathException(IncorrectOperation);

        Number r = num.numbers[0];
        for (size_t i = 1; i < num.numbers.size(); ++i)
        {
            if (num.numbers[i] < r)
                r = num.numbers[i];
        }

        Array<Number> res;
        res.numbers.push_back(r);
        return res;
    }

    friend Array<Number> max(const Array<Number>& num)
    {
        if (num.numbers.empty())
            throw MathException(IncorrectOperation);

        Number r = num.numbers[0];
        for (size_t i = 1; i < num.numbers.size(); ++i)
        {
            if (num.numbers[i] > r)
                r = num.numbers[i];
        }

        Array<Number> res;
        res.numbers.push_back(r);
        return res;
    }

    friend Array<Number> floor(const Array<Number>& num)
    {
        return ForEach(floor, num);
    }

    friend Array<Number> trunc(const Array<Number>& num)
    {
        return ForEach(trunc, num);
    }

    friend Array<Number> ceil(const Array<Number>& num)
    {
        return ForEach(ceil, num);
    }

    friend Array<Number> round(const Array<Number>& num)
    {
        return ForEach(round, num);
    }

    friend Array<Number> integer(const Array<Number>& num)
    {
        return ForEach(integer, num);
    }

    friend Array<Number> fract(const Array<Number> &num)
    {
        return ForEach(fract, num);
    }

    friend Array<Number> fact(const Array<Number> &num, ParserContext* parser_context)
    {
        Array<Number> r;
        for (auto& n : num.numbers)
        {
            CheckBreak(parser_context);
            r.numbers.push_back(fact(n, parser_context));
        }
        return r;
    }

    friend Array<Number> roundoff(const Array<Number>& num)
    {
        return ForEach(roundoff, num);
    }

    friend Array<Number> sin(const Array<Number>& num)
    {
        return ForEach(sin, num);
    }

    friend Array<Number> cos(const Array<Number>& num)
    {
        return ForEach(cos, num);
    }

    friend Array<Number> tg(const Array<Number>& num)
    {
        return ForEach(tg, num);
    }

    friend Array<Number> ctg(const Array<Number>& num)
    {
        return ForEach(ctg, num);
    }

    friend Array<Number> sec(const Array<Number>& num)
    {
        return ForEach(sec, num);
    }

    friend Array<Number> cosec(const Array<Number>& num)
    {
        return ForEach(cosec, num);
    }

    friend Array<Number> arcsin(const Array<Number>& num)
    {
        return ForEach(arcsin, num);
    }

    friend Array<Number> arccos(const Array<Number>& num)
    {
        return ForEach(arccos, num);
    }

    friend Array<Number> arctg(const Array<Number>& num)
    {
        return ForEach(arctg, num);
    }

    friend Array<Number> arcctg(const Array<Number>& num)
    {
        return ForEach(arcctg, num);
    }

    friend Array<Number> arcsec(const Array<Number>& num)
    {
        return ForEach(arcsec, num);
    }

    friend Array<Number> arccosec(const Array<Number>& num)
    {
        return ForEach(arccosec, num);
    }

    friend Array<Number> sh(const Array<Number>& num)
    {
        return ForEach(sh, num);
    }

    friend Array<Number> ch(const Array<Number>& num)
    {
        return ForEach(ch, num);
    }

    friend Array<Number> th(const Array<Number>& num)
    {
        return ForEach(th, num);
    }

    friend Array<Number> cth(const Array<Number>& num)
    {
        return ForEach(cth, num);
    }

    friend Array<Number> sch(const Array<Number>& num)
    {
        return ForEach(sch, num);
    }

    friend Array<Number> csch(const Array<Number>& num)
    {
        return ForEach(csch, num);
    }

    friend Array<Number> arsh(const Array<Number>& num)
    {
        return ForEach(arsh, num);
    }

    friend Array<Number> arch(const Array<Number>& num)
    {
        return ForEach(arch, num);
    }

    friend Array<Number> arth(const Array<Number>& num)
    {
        return ForEach(arth, num);
    }

    friend Array<Number> arcth(const Array<Number>& num)
    {
        return ForEach(arcth, num);
    }

    friend Array<Number> arsch(const Array<Number>& num)
    {
        return ForEach(arsch, num);
    }

    friend Array<Number> arcsch(const Array<Number>& num)
    {
        return ForEach(arcsch, num);
    }

public:
    friend Array<Number> exp_array_real(const int precision)
    {
        Array<Number> r;
        r.numbers.push_back(Number(exp(precision)));
        return r;
    }

    friend Array<Number> pi_array_real(const int precision)
    {
        Array<Number> r;
        r.numbers.push_back(Number(pi(precision)));
        return r;
    }

    friend Array<Number> size(const Array<Number>& num)
    {
        Array<Number> r;
        r.numbers.push_back(Number(num.numbers.empty() ? (int)mpfr_get_default_prec() : num.numbers[0].GetPrecision(), (int)num.numbers.size()));
        return r;
    }

public:
    void Add(const Number& number)
    {
        numbers.push_back(number);
    }

    void Add(const Array<Number>& number)
    {
        numbers.insert(numbers.end(), number.numbers.begin(), number.numbers.end());
    }

    void Insert(const int pos, const Array<Number>& number)
    {
        numbers.insert(numbers.begin() + pos, number.numbers.begin(), number.numbers.end());
    }
    
    void Clear()
    {
        numbers.clear();
    }

    Number Get(const int pos) const
    {
        if (pos >= numbers.size() || pos < 0)
            throw SyntaxException(ArgumentIsOver);
        return numbers[pos];
    }

    int Size() const
    {
        return numbers.size();
    }

    Array<Number> ToRadian() const
    {
        Array<Number> r;
        for (auto& n : numbers)
            r.numbers.push_back(n.ToRadian());
        return r;
    }

    Array<Number> ToDegree() const
    {
        Array<Number> r;
        for (auto& n : numbers)
            r.numbers.push_back(n.ToDegree());
        return r;
    }

    Array<Number> ToGrad() const
    {
        Array<Number> r;
        for (auto& n : numbers)
            r.numbers.push_back(n.ToGrad());
        return r;
    }

    friend Array<Number> rad(const Array<Number>& num)
    {
        return ForEach(rad, num);
    }

    friend Array<Number> deg(const Array<Number>& num)
    {
        return ForEach(deg, num);
    }
    
    friend Array<Number> minute(const Array<Number>& num)
    {
        return ForEach(minute, num);
    }
    
    friend Array<Number> second(const Array<Number>& num)
    {
        return ForEach(second, num);
    }
    
    friend Array<Number> grad(const Array<Number>& num)
    {
        return ForEach(grad, num);
    }
    
    AngleMeasure GetAngleMeasure()
    {
        if (numbers.empty())
            return AngleMeasure::None;
        return numbers[0].angle_measure;
    }

    void SetAngleMeasure(AngleMeasure angle_measure)
    {
        for (auto& n : numbers)
            n.angle_measure = angle_measure;
    }

    bool IsNaN() const
    {
        for (auto& n : numbers)
        {
            if (n.IsNaN())
                return true;
        }
        return false;
    }

public:
    std::u32string ToString() const
    {
        std::u32string res = U"[";
        for (size_t i = 0; i < numbers.size(); ++i)
        {
            res += numbers[i].ToString();
            if (i < numbers.size() - 1)
                res += U",";
        }
        res += U"]";
        return res;
    }

    std::string ToStdString(int exp, int accuracy) const
    {
        std::string res = "[";
        for (size_t i = 0; i < numbers.size(); ++i)
        {
            res += numbers[i].ToStdString(exp, accuracy);
            if (i < numbers.size() - 1)
                res += ",";
        }
        res += "]";
        return res;
    }

private:
    std::vector<Number> numbers;
};

}

#endif