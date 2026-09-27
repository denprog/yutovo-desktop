/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __EXPORT_H__
#define __EXPORT_H__

#include <vector>
#include <mutex>
#include "ast.h"
#include "custom_unit.h"

namespace yutovo_calculator
{

struct Export
{
public:
    template<typename Number>
    void AddVariable(const VariableNode<Number>& var);

    template<typename Number>
    void AddFunction(const FunctionNode<Number>& func);

    template<typename Number>
    void AddUnit(const CustomUnit<Number>& unit);

    template<typename Number>
    void AddList(const ListNode<Number>& list)
    {
        lists[list.name.name] = list.list;
    }

    template<typename Number>
    void AddString(const StringNode<Number>& str)
    {
        strings[str.name.name] = str.str;
    }

    template<typename Number>
    VariableNode<Number>* FindVariable(const std::u32string& name, const std::u32string& subscript);

    template<typename Number>
    FunctionNode<Number>* FindFunction(const std::u32string& name);

    template<typename Number>
    CustomUnit<Number>* FindUnit(const std::u32string& name, const std::u32string& system);

    template<typename Number>
    void GetUnits(const std::u32string& system, std::vector<CustomUnit<Number>>& units);

    void Clear();

private:
    std::mutex export_mutex;

    std::vector<VariableNode<Integer>> variables_integer;
    std::vector<VariableNode<Real>> variables_real;
    std::vector<VariableNode<Rational>> variables_rational;
    std::vector<VariableNode<Complex>> variables_complex;
    std::vector<VariableNode<Symbolic<Real>>> variables_symbolic;
    std::vector<VariableNode<Symbolic<Rational>>> variables_symbolic_rational;
    std::vector<VariableNode<Symbolic<Complex>>> variables_symbolic_complex;
    std::vector<VariableNode<Array<Real>>> variables_array_real;

    std::vector<FunctionNode<Integer>> functions_integer;
    std::vector<FunctionNode<Real>> functions_real;
    std::vector<FunctionNode<Rational>> functions_rational;
    std::vector<FunctionNode<Complex>> functions_complex;
    std::vector<FunctionNode<Symbolic<Real>>> functions_symbolic;
    std::vector<FunctionNode<Symbolic<Rational>>> functions_symbolic_rational;
    std::vector<FunctionNode<Symbolic<Complex>>> functions_symbolic_complex;
    std::vector<FunctionNode<Array<Real>>> functions_array_real;

    std::vector<CustomUnit<Real>> units_real;
    std::vector<CustomUnit<Rational>> units_rational;
    std::vector<CustomUnit<Array<Real>>> units_array_real;

    std::map<std::u32string, std::vector<std::u32string>> lists;
    std::map<std::u32string, std::u32string> strings;
};

}

#endif