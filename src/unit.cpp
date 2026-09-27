/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "unit.h"
#include "utils.h"
#include "real.h"
#include "parser_exception.h"
#include <boost/algorithm/string.hpp>

namespace yutovo_calculator
{

//Unit

Unit operator+(const Unit& unit1, const Unit& unit2)
{
    if (unit1 != unit2)
        throw MathException(UnitsAreIncompatible);
    return unit1;
}

Unit operator+(const Unit& unit1, const int)
{
    if (!unit1.IsEmpty())
        throw MathException(UnitsAreIncompatible);
    return Unit();
}

Unit operator+(const int, const Unit& unit2)
{
    if (!unit2.IsEmpty())
        throw MathException(UnitsAreIncompatible);
    return Unit();
}

Unit operator+(const Unit& unit1, const float)
{
    if (!unit1.IsEmpty())
        throw MathException(UnitsAreIncompatible);
    return Unit();
}

Unit operator+(const float, const Unit& unit2)
{
    if (!unit2.IsEmpty())
        throw MathException(UnitsAreIncompatible);
    return Unit();
}

Unit operator-(const Unit& unit1, const Unit& unit2)
{
    if (unit1 != unit2)
        throw MathException(UnitsAreIncompatible);
    return unit1;
}

Unit operator-(const Unit& unit1, const int)
{
    if (!unit1.IsEmpty())
        throw MathException(UnitsAreIncompatible);
    return Unit();
}

Unit operator-(const int, const Unit& unit2)
{
    if (!unit2.IsEmpty())
        throw MathException(UnitsAreIncompatible);
    return Unit();
}

Unit operator-(const Unit& unit1, const float)
{
    if (!unit1.IsEmpty())
        throw MathException(UnitsAreIncompatible);
    return Unit();
}

Unit operator-(const float, const Unit& unit2)
{
    if (!unit2.IsEmpty())
        throw MathException(UnitsAreIncompatible);
    return Unit();
}

Unit operator*(const Unit& unit1, const Unit& unit2)
{
    Unit res;
    Unit _unit2(unit2);
    for (int i = 0; i < unit1.unit.size(); ++i)
    {
        auto& p1 = unit1.unit[i];
        bool f = false;
        for (int j = 0; j < _unit2.unit.size(); ++j)
        {
            auto& p2 = _unit2.unit[j];
            if (p1.first == p2.first)
            {
                if (p1.second != -p2.second)
                    res.unit.push_back({p1.first, p1.second + p2.second});
                _unit2.unit.erase(_unit2.unit.begin() + j);
                f = true;
                break;
            }
        }
        if (!f)
            res.unit.push_back({p1.first, p1.second});
    }
    for (int j = 0; j < _unit2.unit.size(); ++j)
    {
        auto& p2 = _unit2.unit[j];
        res.unit.push_back({p2.first, p2.second});
    }
    return res;
}

Unit operator*(const Unit& unit1, const int)
{
    return unit1;
}

Unit operator*(const int, const Unit& unit2)
{
    return unit2;
}

Unit operator*(const Unit& unit1, const float)
{
    return unit1;
}

Unit operator*(const float, const Unit& unit2)
{
    return unit2;
}

Unit operator/(const Unit& unit1, const Unit& unit2)
{
    Unit res;
    Unit _unit2(unit2);
    for (int i = 0; i < unit1.unit.size(); ++i)
    {
        auto& p1 = unit1.unit[i];
        bool f = false;
        for (int j = 0; j < _unit2.unit.size(); ++j)
        {
            auto& p2 = _unit2.unit[j];
            if (p1.first == p2.first)
            {
                if (p1.second != p2.second)
                    res.unit.push_back({p1.first, p1.second - p2.second});
                _unit2.unit.erase(_unit2.unit.begin() + j);
                f = true;
                break;
            }
        }
        if (!f)
            res.unit.push_back({p1.first, p1.second});
    }
    for (int j = 0; j < _unit2.unit.size(); ++j)
    {
        auto& p2 = _unit2.unit[j];
        res.unit.push_back({p2.first, -p2.second});
    }
    return res;
}

Unit operator/(const int, const Unit& unit2)
{
    Unit res(unit2);
    for (auto& u : res.unit)
    {
        u.second *= -1;
    }
    return res;
}

Unit operator/(const Unit& unit1, const int)
{
    return unit1;
}

Unit operator/(const float, const Unit& unit2)
{
    Unit res(unit2);
    for (auto& u : res.unit)
    {
        u.second *= -1;
    }
    return res;
}

Unit operator/(const Unit& unit1, const float)
{
    return unit1;
}

Unit pow(const Unit& unit1, const Real& val)
{
    if (!val.unit.IsEmpty())
        throw MathException(UnitsAreIncompatible);
    if (unit1.IsEmpty())
        return Unit();
    if (!val.IsInteger())
        throw MathException(UnitsAreIncompatible);
    int p = (int)val;
    Unit res(unit1);
    for (auto& u : res.unit)
        u.second *= p;
    return res;
}

Unit pow(const Unit& unit1, const int val)
{
    Unit res(unit1);
    for (auto& u : res.unit)
    {
        u.second *= val;
    }
    return res;
}

Unit pow(const Unit& unit1, const float)
{
    throw MathException(UnitsAreIncompatible);
}

Unit sqrt(const Unit& unit)
{
    if (unit.IsEmpty())
        return Unit();
    Unit res(unit);
    for (auto& u : res.unit)
    {
        if (u.second % 2 != 0)
            throw MathException(UnitsAreIncompatible);
        u.second /= 2;
    }
    return res;
}

Unit root(const Unit& unit, const Real& val)
{
    if (unit.IsEmpty())
        return Unit();
    Unit res(unit);
    for (auto& u : res.unit)
    {
        if (u.second % val != 0)
            throw MathException(UnitsAreIncompatible);
        u.second /= val;
    }
    return res;
}

bool Unit::operator==(const Unit& other) const
{
    if (system != other.system)
        return false;
    if (unit.size() != other.unit.size())
        return false;
    for (auto& u : other.unit)
    {
        auto it = std::find_if(unit.begin(), unit.end(), 
            [u](auto& p)
            {
                return p.first == u.first && p.second == u.second;
            });
        if (it == unit.end())
            return false;
    }
    return true;
}

bool Unit::operator==(const std::u32string& name) const
{
    return unit.size() == 1 && unit[0].first == name && unit[0].second == 1;
}

bool Unit::operator!=(const Unit& other) const
{
    return !operator==(other);
}

bool Unit::operator<(const Unit& other) const
{
    if (system == other.system)
    {
        if (unit.size() == other.unit.size())
        {
            int p1 = GetPower();
            int p2 = other.GetPower();
            if (p1 == p2)
            {
                std::u32string s1, s2;
                for (auto& u : unit)
                    s1 += u.first;
                for (auto& u : other.unit)
                    s2 += u.first;
                return s1.size() < s2.size();
            }
            return p1 < p2;
        }
        return unit.size() < other.unit.size();
    }
    return system < other.system;
}

int Unit::GetPower() const
{
    if (unit.empty())
        return 0;
    
    int p = unit[0].second;
    int j, k, l = 0;
	for (int i = 0; i < (int)unit.size() - 1; ++i)
	{
		j = ::abs(p);
		k = ::abs(unit[i + 1].second);

		while (k > 0)
		{
			l = j % k;
			j = k;
			k = l;
		}

		p = ::abs(j);
	}

	return p;
}

void Unit::Sort()
{
    std::sort(unit.begin(), unit.end(), 
        [](const auto& p1, const auto& p2)
        {
            if (p1.second == p2.second)
                return p1.first < p2.first;
            return p1.second > p2.second;
        });
}

std::u32string Unit::ToString(bool f) const
{
    if (unit.empty())
        return U"";
    
    std::u32string upper, lower;
    for (auto p : unit)
    {
        auto s = p.first;
        if (::abs(p.second) > 1)
            s += U"^" + ToUtfString(std::to_string(::abs(p.second)));
        if (p.second > 0)
        {
            if (upper.empty())
                upper = s;
            else
                upper += U"*" + s;
        }
        else
        {
            if (lower.empty())
                lower = s;
            else
                lower += U"*" + s;
        }
    }

    if (upper.empty())
        upper = U"1";
    else if (!lower.empty())
        upper = U"(" + upper + U")";
    if (!lower.empty())
    {
        upper += U"/";
        lower = U"(" + lower + U")";
    }

    std::u32string r = f ? U"(" + upper + lower + U")" : upper + lower;
    if (system.empty() || system == U"SI")
        return r;
    return r + U"{" + system + U"}";
}

bool Unit::FromString(const std::u32string& str)
{
    std::u32string _str = str;
    std::u32string _system;
    auto p1 = str.find(U'{');
    if (p1 != std::string::npos)
    {
        auto p2 = _str.find(U'}');
        if (p2 == std::string::npos)
            return false;
        _system = _str.substr(p1 + 1, p2 - p1 - 1);
        _str = _str.substr(0, p1);
    }

    if (_str.length() > 2 && _str[0] == U'(' && _str[1] == U'(' && _str[_str.length() - 1] == U')')
        _str = _str.substr(1, _str.length() - 2);

    std::vector<std::u32string> arr;
    boost::split(arr, _str, boost::is_any_of("/"));

    std::u32string upper, lower;
    if (arr.empty())
        return false;
    upper = arr[0];
    if (upper.empty())
        return false;
    if (arr.size() == 2)
    {
        lower = arr[1];
        if (lower.empty())
            return false;
    }
    
    if (upper[0] == U'(')
    {
        if (upper[upper.length() - 1] != U')')
            return false;
        upper = upper.substr(1, upper.length() - 2);
    }

    std::vector<std::pair<std::u32string, int>> _unit;

    arr.clear();
    if (upper != U"1")
    {
        boost::split(arr, upper, boost::is_any_of("*"));
        for (auto& s : arr)
        {
            std::vector<std::u32string> _arr;
            boost::split(_arr, s, boost::is_any_of("^"));
            if (_arr.empty())
                return false;
            std::u32string name = _arr[0];
            int power = 1;
            if (_arr.size() == 2)
            {
                try
                {
                    power = std::stoi(ToBasicString(_arr[1]));
                }
                catch (std::logic_error& ex)
                {
                    return false;
                }
            }
            _unit.push_back(std::make_pair(name, power));
        }
    }

    if (!lower.empty())
    {
        if (lower[0] == U'(')
        {
            if (lower[lower.length() - 1] != U')')
                return false;
            lower = lower.substr(1, lower.length() - 2);
        }

        arr.clear();
        boost::split(arr, lower, boost::is_any_of("*"));
        for (auto& s : arr)
        {
            std::vector<std::u32string> _arr;
            boost::split(_arr, s, boost::is_any_of("^"));
            if (_arr.empty())
                return false;
            std::u32string name = _arr[0];
            int power = -1;
            if (_arr.size() == 2)
            {
                try
                {
                    power = -std::stoi(ToBasicString(_arr[1]));
                }
                catch (std::logic_error& ex)
                {
                    return false;
                }
            }
            _unit.push_back(std::make_pair(name, power));
        }
    }

    unit = _unit;
    if (_system.empty())
        system = U"SI";
    else
        system = _system;
    description = U"";

    return true;
}

}
