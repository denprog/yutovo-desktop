/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __MEASURE_H__
#define __MEASURE_H__

#include <vector>
#include <string>
#include <algorithm>

namespace yutovo_calculator
{

class Real;

class Unit
{
public:
    Unit() :
        system(U"SI")
    {
    }

    Unit(const std::u32string& name) : 
        system(U"SI")
    {
        unit.push_back(std::make_pair(name, 1));
    }

    Unit(const std::u32string& name, const int power) :
        system(U"SI")
    {
        unit.push_back(std::make_pair(name, power));
    }

    Unit(const std::u32string& name, const std::u32string& _description, const int power) :
        description(_description), 
        system(U"SI")
    {
        unit.push_back(std::make_pair(name, power));
    }

    Unit(const std::u32string& numerator, const std::u32string& denomerator) :
        system(U"SI")
    {
        unit.push_back(std::make_pair(numerator, 1));
        unit.push_back(std::make_pair(denomerator, -1));
    }

    friend Unit operator+(const Unit& unit1, const Unit& unit2);
    friend Unit operator+(const Unit& unit1, const int);
    friend Unit operator+(const int, const Unit& unit2);
    friend Unit operator+(const Unit& unit1, const float);
    friend Unit operator+(const float, const Unit& unit2);

    friend Unit operator-(const Unit& unit1, const Unit& unit2);
    friend Unit operator-(const Unit& unit1, const int);
    friend Unit operator-(const int, const Unit& unit2);
    friend Unit operator-(const Unit& unit1, const float);
    friend Unit operator-(const float, const Unit& unit2);

    friend Unit operator*(const Unit& unit1, const Unit& unit2);
    friend Unit operator*(const Unit& unit1, const int);
    friend Unit operator*(const int, const Unit& unit2);
    friend Unit operator*(const Unit& unit1, const float);
    friend Unit operator*(const float, const Unit& unit2);

    friend Unit operator/(const Unit& unit1, const Unit& unit2);
    friend Unit operator/(const int, const Unit& unit2);
    friend Unit operator/(const Unit& unit1, const int);
    friend Unit operator/(const float, const Unit& unit2);
    friend Unit operator/(const Unit& unit1, const float);

    friend Unit pow(const Unit& unit1, const Real& val);
    friend Unit pow(const Unit& unit1, const int val);
    friend Unit pow(const Unit& unit1, const float);

    friend Unit sqrt(const Unit& unit);
    friend Unit root(const Unit& unit, const Real& val);

    bool operator==(const std::u32string& name) const;
    bool operator==(const Unit& other) const;
    bool operator!=(const Unit& other) const;
    bool operator<(const Unit& other) const;

    int GetPower() const;

    bool IsEmpty() const
    {
        return unit.empty();
    }

    void Sort();

    std::u32string ToString(bool f = true) const;
    bool FromString(const std::u32string& str);

public:
    std::vector<std::pair<std::u32string, int>> unit; //unit names with their powers
    std::u32string system;
    std::u32string description;
};

}

#endif
