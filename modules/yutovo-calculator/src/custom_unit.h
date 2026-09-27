/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __CUSTOM_UNIT_H__
#define __CUSTOM_UNIT_H__

namespace yutovo_calculator
{

template<typename Number>
struct CustomUnit
{
    CustomUnit(const LogicalId _id, const std::u32string& _name, const std::u32string& _system, Number _value, bool _buildin) :
        id(_id),
        name(_name),
        system(_system),
        value(_value),
        buildin(_buildin)
    {
        if (system.empty())
            system = U"SI";
    }

    bool Cast(Number& val) const
    {
        Number _val = val;
        Number res_val = val;
        Unit _val_unit = val.unit;
        Unit res_unit = value.unit;
        int power = 0;
        bool f = true;

        while (f)
        {
            for (auto& u : res_unit.unit)
            {
                auto it = std::find_if(_val.unit.unit.begin(), _val.unit.unit.end(), 
                    [u](const auto& p)
                    {
                        if (p.first != u.first)
                            return false;
                        if (p.second > 0 && u.second > 0)
                            return p.second >= u.second;
                        if (p.second < 0 && u.second < 0)
                            return p.second <= u.second;
                        return false;
                    });
                if (it == _val.unit.unit.end())
                {
                    f = false;
                    break;
                }
                it->second -= u.second;
            }
            if (f)
            {
                ++power;
                for (size_t i = 0; i < _val.unit.unit.size(); ++i)
                {
                    auto& p = _val.unit.unit[i];
                    if (p.second == 0)
                        _val.unit.unit.erase(_val.unit.unit.begin() + i--);
                }
                res_val = _val;
            }
        }

        if (power == 0)
        {
            //try to find the backward variant of the unit
            f = true;
            _val = val;
            while (f)
            {
                for (auto& u : res_unit.unit)
                {
                    auto it = std::find_if(_val.unit.unit.begin(), _val.unit.unit.end(), 
                        [u](const auto& p)
                        {
                            if (p.first != u.first)
                                return false;
                            if (p.second < 0 && u.second > 0)
                                return ::abs(p.second) >= u.second;
                            if (p.second > 0 && u.second < 0)
                                return p.second >= ::abs(u.second);
                            return false;
                        });
                    if (it == _val.unit.unit.end())
                    {
                        f = false;
                        break;
                    }
                    it->second += u.second;
                }
                if (f)
                {
                    --power;
                    for (size_t i = 0; i < _val.unit.unit.size(); ++i)
                    {
                        auto& p = _val.unit.unit[i];
                        if (p.second == 0)
                            _val.unit.unit.erase(_val.unit.unit.begin() + i--);
                    }
                    res_val = _val;
                }
            }
        }

        if (power == 0)
            return false;

        res_val.unit.unit.insert(res_val.unit.unit.begin(), std::make_pair(name, power));
        auto u = res_val.unit;
        if (power > 0)
            res_val = res_val / pow(value, power);
        else
            res_val = res_val * pow(value, -power);
        u.system = system;
        u.description = U"";
        res_val.SetUnit(u);
        if (_val_unit == res_val.unit)
            return false;
        val = res_val;
        return true;
    }

    LogicalId id;
    std::u32string name;
    std::u32string system;
    Number value;
    bool buildin;
    std::u32string description;
};

}

#endif