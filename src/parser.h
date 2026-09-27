/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef PARSER_H
#define PARSER_H

#include "stdafx.h"
#include "integer.h"
#include "rational.h"
#include "real.h"
#include "complex.h"
#include "symbolic.h"
#include "array.h"
#include "ast.h"
#include "script.h"
#include "solver.h"
#include "error_handler.h"
#include "utils.h"

namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace fusion = boost::fusion;

namespace yutovo_calculator
{

//Big numbers parser
template<typename Number>
struct Parser
{
    Parser(const int precision, const Language _language);

    ~Parser()
    {
        if constexpr (is_symbolic_v<Number>)
        {
            if (giac_context && current_giac_context == giac_context.get())
                current_giac_context = nullptr;
        }
    }

    Number Parse(LogicalId id, std::u32string expression, Dependencies* dependencies, AngleMeasure default_angle_measure, AngleMeasure result_angle_measure, 
        Notation default_notation, const int precision = -1, ParserContext* _context = nullptr)
    {
        InitThreadTime();

        if (expression.empty() || expression == U";")
            throw SyntaxException(id, ExpressionExpected, 0, 0);

        std::u32string::iterator iter = expression.begin();
        std::u32string::iterator end = expression.end();
        unicode::space_type space;

        solver.ClearTempVariables();
        solver.ClearCastUnits();
        solver.SetDefaultNotation(default_notation);
        solver.SetParserContext(_context);

        if constexpr (is_symbolic_v<Number>)
        {
            if (!giac_context)
                giac_context = std::make_unique<giac::context>();
            current_giac_context = giac_context.get();
        }

        Script<Number> script(id, expression, &solver);
        ScriptNode<Number> script_node;

        phrase_parse(iter, end, script, space, script_node);
        return solver(script_node, id, default_angle_measure, result_angle_measure, precision, dependencies);
    }

    Number Parse(LogicalId id, std::u32string expression, AngleMeasure default_angle_measure, AngleMeasure result_angle_measure, const int precision = -1, 
        ParserContext* _context = nullptr)
    {
        Dependencies dependencies;
        return Parse(id, expression, &dependencies, default_angle_measure, result_angle_measure, Notation::Decimal, precision, _context);
    }

    Number Parse(LogicalId id, std::string expression, Dependencies* dependencies, AngleMeasure default_angle_measure, AngleMeasure result_angle_measure, 
        const int precision = -1, ParserContext* _context = nullptr)
    {
        return Parse(id, ToUtfString(expression), dependencies, default_angle_measure, result_angle_measure, Notation::Decimal, precision, _context);
    }

    Number Parse(LogicalId id, std::u32string expression, Dependencies* dependencies, AngleMeasure default_angle_measure, AngleMeasure result_angle_measure, 
        const int precision = -1, ParserContext* _context = nullptr)
    {
        return Parse(id, expression, dependencies, default_angle_measure, result_angle_measure, Notation::Decimal, precision, _context);
    }

    Number Parse(LogicalId id, std::u32string expression, ParserContext* _context = nullptr)
    {
        Dependencies dependencies;
        return Parse(id, expression, &dependencies, AngleMeasure::Radian, AngleMeasure::None, Notation::Decimal, -1, _context);
    }

    Number Parse(LogicalId id, std::u32string expression, const int precision, ParserContext* _context = nullptr)
    {
        Dependencies dependencies;
        return Parse(id, expression, &dependencies, AngleMeasure::Radian, AngleMeasure::None, Notation::Decimal, precision, _context);
    }

    Number Parse(LogicalId id, std::u32string expression, Dependencies* dependencies, ParserContext* _context = nullptr)
    {
        return Parse(id, expression, dependencies, AngleMeasure::Radian, AngleMeasure::None, Notation::Decimal, -1, _context);
    }

    Number Parse(LogicalId id, std::u32string expression, Dependencies* dependencies, const int precision, ParserContext* _context = nullptr)
    {
        return Parse(id, expression, dependencies, AngleMeasure::Radian, AngleMeasure::None, Notation::Decimal, precision, _context);
    }

    Number Parse(LogicalId id, std::u32string expression, Dependencies* dependencies, Notation default_notation, ParserContext* _context = nullptr)
    {
        return Parse(id, expression, dependencies, AngleMeasure::Radian, AngleMeasure::None, default_notation, -1, _context);
    }

    Number Parse(LogicalId id, std::string expression, Dependencies* dependencies, Notation default_notation, ParserContext* _context = nullptr)
    {
        return Parse(id, ToUtfString(expression), dependencies, AngleMeasure::Radian, AngleMeasure::None, default_notation, -1, _context);
    }

    Number Parse(LogicalId id, std::string expression, Dependencies* dependencies, ParserContext* _context = nullptr)
    {
        return Parse(id, ToUtfString(expression), dependencies, AngleMeasure::Radian, AngleMeasure::None, -1, _context);
    }

    Number Parse(LogicalId id, std::string expression, Dependencies* dependencies, const int precision, ParserContext* _context = nullptr)
    {
        return Parse(id, ToUtfString(expression), dependencies, AngleMeasure::Radian, AngleMeasure::None, Notation::Decimal, precision, _context);
    }

    Number Parse(LogicalId id, std::u32string expression, Notation default_notation, ParserContext* _context = nullptr)
    {
        Dependencies dependencies;
        return Parse(id, expression, &dependencies, AngleMeasure::Radian, AngleMeasure::None, default_notation, -1, _context);
    }

    Number Parse(LogicalId id, std::u32string expression, Dependencies* dependencies, 
        AngleMeasure default_angle_measure, AngleMeasure result_angle_measure, const int precision, const int res_count, 
        std::vector<Number>& results, ParserContext* _context = nullptr)
    {
        InitThreadTime();

        if (expression.empty() || expression == U";")
            throw SyntaxException(id, ExpressionExpected, 0, 0);
        
        std::u32string::iterator iter = expression.begin();
        std::u32string::iterator end = expression.end();
        unicode::space_type space;

        Script<Complex> script(id, expression, &solver);
        ScriptNode<Complex> script_node;

        phrase_parse(iter, end, script, space, script_node);

        for (int i = 0; i < res_count; ++i)
        {
            solver.res_pos = i;
            Number r = solver(script_node, id, default_angle_measure, result_angle_measure, precision, dependencies);
            if (std::find(results.begin(), results.end(), r) != results.end())
                break;
            results.push_back(r);
        }
        if (results.empty())
        {
            if (_context)
                _context->no_result = true;
            return Number();
        }
        return results[0];
    }

    Number Parse(LogicalId id, std::string expression, Dependencies* dependencies, 
        AngleMeasure default_angle_measure, AngleMeasure result_angle_measure, const int precision, const int res_count, 
        std::vector<Number>& results, ParserContext* _context = nullptr)
    {
        return Parse(id, ToUtfString(expression), dependencies, default_angle_measure, result_angle_measure, precision, res_count, results, _context);
    }

    bool RemoveIdentifier(LogicalId id, const std::u32string& name)
    {
        return solver.RemoveIdentifier(id, name);
    }

    bool RemoveIdentifier(LogicalId id, const std::string& name)
    {
        return RemoveIdentifier(id, ToUtfString(name));
    }

    void RemoveUserIdentifiers()
    {
        solver.RemoveUserIdentifiers();
    }

    Number GetSuitableUnit(LogicalId id, const Number& val)
    {
        InitThreadTime();
        return solver.GetSuitableUnit(id, val, solver.symbols->last_unit_system, solver.symbols->buildin_elements);
    }

    void GetCastUnits(const LogicalId id, const Number& val, std::vector<Unit>& cast_units)
    {
        InitThreadTime();
        solver.GetCastUnits(id, val, cast_units);
    }

    Number CastToUnit(const LogicalId id, const Number& val, const Unit& unit)
    {
        InitThreadTime();
        return solver.CastToUnit(id, val, unit);
    }

    void ClearCastUnits()
    {
        solver.ClearCastUnits();
    }

    void SetLocale(Language _language);

    void ListBuiltinVariables(std::vector<std::u32string>& variables)
    {
        solver.ListBuiltinVariables(variables);
    }

    void ListBuiltinFunctions(std::vector<std::u32string>& functions)
    {
        solver.ListBuiltinFunctions(functions);
    }

    void ListUserVariables(std::vector<std::pair<std::u32string, std::u32string>>& variables)
    {
        solver.ListUserVariables(variables);
    }

    void ListUserFunctions(std::vector<std::u32string>& functions)
    {
        solver.ListUserFunctions(functions);
    }

    void ListBuiltinUnits(std::vector<std::pair<std::u32string, std::u32string>>& units)
    {
        solver.ListBuiltinUnits(units);
    }

    void ListUserUnits(std::vector<CustomUnit<Number>>& units)
    {
        solver.ListUserUnits(units);
    }

    void ListBuiltinOperations(std::vector<std::u32string>& operations)
    {
        solver.ListBuiltinOperations(operations);
    }

    void ListUserStrings(std::vector<std::u32string>& strings)
    {
        solver.ListUserStrings(strings);
    }

    void SetMaxCastUnitSize(int max_cast_unit_size)
    {
        solver.SetMaxCastUnitSize(max_cast_unit_size);
    }

private:
    void InitUnits()
    {
        solver.ResetBuildinUnits();

        switch (language)
        {
        case Language::English:
            solver.AddBuiltinUnit(Unit(U"m", U"metre", 1)); //meter (length)
            solver.AddBuiltinUnit(Unit(U"kg", U"kilogram", 1)); //kilogram (mass)
            solver.AddBuiltinUnit(Unit(U"s", U"second", 1)); //second (time)
            solver.AddBuiltinUnit(Unit(U"mol", U"mole", 1)); //mole (amount of matter)
            solver.AddBuiltinUnit(Unit(U"A", U"ampere", 1)); //ampere (electric current)
            solver.AddBuiltinUnit(Unit(U"cd", U"candella", 1)); //candella (luminosity)
            solver.AddBuiltinUnit(Unit(U"K", U"kelvin", 1)); //kelvin (temperature)
            solver.AddBuiltinUnit(Unit(U"sr", U"steradian", 1)); //steradian (solid angle)
            solver.AddBuiltinUnit(Unit(U"bit")); //bit (data)
            solver.AddBuiltinUnit(Unit(U"₽", U"ruble", 1)); //russian ruble
            solver.AddBuiltinUnit(Unit(U"$", U"dollar", 1)); //US dollar
            solver.AddBuiltinUnit(Unit(U"€", U"euro", 1)); //euro
            solver.AddBuiltinUnit(Unit(U"¥", U"yuan", 1)); //chinese yuan
            solver.AddBuiltinUnit(Unit(U"₹", U"rupee", 1)); //indian rupee
            solver.AddBuiltinUnit(Unit(U"R$", U"real", 1)); //brazilian real
            break;
        case Language::Russian:
            solver.AddBuiltinUnit(Unit(U"м", U"метр", 1)); //meter (length)
            solver.AddBuiltinUnit(Unit(U"кг", U"килограмм", 1)); //kilogram (mass)
            solver.AddBuiltinUnit(Unit(U"с", U"секунда", 1)); //second (time)
            solver.AddBuiltinUnit(Unit(U"моль")); //mole (amount of matter)
            solver.AddBuiltinUnit(Unit(U"А", U"ампер", 1)); //ampere (electric current)
            solver.AddBuiltinUnit(Unit(U"Кд", U"кандела", 1)); //candella (luminosity)
            solver.AddBuiltinUnit(Unit(U"К", U"кельвин", 1)); //kelvin (temperature)
            solver.AddBuiltinUnit(Unit(U"ср", U"стерадиан", 1)); //steradian (solid angle)
            solver.AddBuiltinUnit(Unit(U"бит")); //bit (data)
            solver.AddBuiltinUnit(Unit(U"₽", U"рубль", 1)); //russian ruble
            solver.AddBuiltinUnit(Unit(U"$", U"доллар", 1)); //US dollar
            solver.AddBuiltinUnit(Unit(U"€", U"евро", 1)); //euro
            solver.AddBuiltinUnit(Unit(U"¥", U"юань", 1)); //chinese yuan
            solver.AddBuiltinUnit(Unit(U"₹", U"рупия", 1)); //indian rupee
            solver.AddBuiltinUnit(Unit(U"R$", U"реал", 1)); //brazilian real
            break;
        case Language::Spanish:
            solver.AddBuiltinUnit(Unit(U"m", U"metro", 1));
            solver.AddBuiltinUnit(Unit(U"kg", U"kilogramo", 1));
            solver.AddBuiltinUnit(Unit(U"s", U"segundo", 1));
            solver.AddBuiltinUnit(Unit(U"mol", U"mol", 1));
            solver.AddBuiltinUnit(Unit(U"A", U"ampère", 1));
            solver.AddBuiltinUnit(Unit(U"cd", U"candela", 1));
            solver.AddBuiltinUnit(Unit(U"K", U"kelvin", 1));
            solver.AddBuiltinUnit(Unit(U"sr", U"esterradiano", 1));
            solver.AddBuiltinUnit(Unit(U"bit"));
            solver.AddBuiltinUnit(Unit(U"₽", U"rublo", 1));
            solver.AddBuiltinUnit(Unit(U"$", U"dólar", 1));
            solver.AddBuiltinUnit(Unit(U"€", U"euro", 1));
            solver.AddBuiltinUnit(Unit(U"¥", U"yuan", 1));
            solver.AddBuiltinUnit(Unit(U"₹", U"rupia", 1));
            solver.AddBuiltinUnit(Unit(U"R$", U"real", 1));
            break;
        case Language::BrazilianPortuguese:
            solver.AddBuiltinUnit(Unit(U"m", U"metro", 1));
            solver.AddBuiltinUnit(Unit(U"kg", U"quilograma", 1));
            solver.AddBuiltinUnit(Unit(U"s", U"segundo", 1));
            solver.AddBuiltinUnit(Unit(U"mol", U"mol", 1));
            solver.AddBuiltinUnit(Unit(U"A", U"ampère", 1));
            solver.AddBuiltinUnit(Unit(U"cd", U"candela", 1));
            solver.AddBuiltinUnit(Unit(U"K", U"kelvin", 1));
            solver.AddBuiltinUnit(Unit(U"sr", U"esterradiano", 1));
            solver.AddBuiltinUnit(Unit(U"bit", U"bit", 1));
            solver.AddBuiltinUnit(Unit(U"₽", U"rublo", 1));
            solver.AddBuiltinUnit(Unit(U"$", U"dólar", 1));
            solver.AddBuiltinUnit(Unit(U"€", U"euro", 1));
            solver.AddBuiltinUnit(Unit(U"¥", U"yuan", 1));
            solver.AddBuiltinUnit(Unit(U"₹", U"rupia", 1));
            solver.AddBuiltinUnit(Unit(U"R$", U"real", 1));
            break;
        default:
            throw ParserException({}, ParserExceptionCode::UnknownLanguage);
        }

        solver.symbols->buildin_elements = true;

        InitSIUnits();
        InitOtherUnits();
        InitRussianUnits();
        InitUsUnits();

        InitBuiltinIdentifiers();

        solver.symbols->buildin_elements = false;
    }

    void InitSIUnits()
    {
        auto it = si_units.find(last_language);
        if (it != si_units.end())
        {
            int p = 0;
            for (auto& expr : it->second)
                solver.RemoveIdentifier(LogicalId{0, -2, 0, 0, p++});
        }

        it = si_units.find(language);
        if (it == si_units.end())
            return;
        int p = 0;
        for (auto& expr : it->second)
            Parse(LogicalId{0, -2, 0, 0, p++}, expr);
    }

    void InitOtherUnits()
    {
        auto it = other_units.find(last_language);
        if (it != other_units.end())
        {
            int p = 0;
            for (auto& expr : it->second)
                solver.RemoveIdentifier(LogicalId{0, -2, 1, 0, p++});
        }

        it = other_units.find(language);
        if (it == other_units.end())
            return;
        int p = 0;
        for (auto& expr : it->second)
            Parse(LogicalId{0, -2, 1, 0, p++}, expr);
    }

    void InitRussianUnits()
    {
        auto it = russian_units.find(last_language);
        if (it != russian_units.end())
        {
            int p = 0;
            for (auto& expr : it->second)
                solver.RemoveIdentifier(LogicalId{0, -2, 2, 0, p++});
        }

        it = russian_units.find(language);
        if (it == russian_units.end())
            return;
        int p = 0;
        for (auto& expr : it->second)
            Parse(LogicalId{0, -2, 2, 0, p++}, expr);
    }

    void InitUsUnits()
    {
        auto it = us_units.find(last_language);
        if (it != us_units.end())
        {
            int p = 0;
            for (auto& expr : it->second)
                solver.RemoveIdentifier(LogicalId{0, -2, 3, 0, p++});
        }

        it = us_units.find(language);
        if (it == us_units.end())
            return;
        int p = 0;
        for (auto& expr : it->second)
            Parse(LogicalId{0, -2, 3, 0, p++}, expr);
    }

    void InitBuiltinIdentifiers()
    {
    }

    void InitPhisicalConstants()
    {
        auto it = phisical_constants.find(last_language);
        if (it != phisical_constants.end())
        {
            int p = 0;
            for (auto& expr : it->second)
                solver.RemoveIdentifier(LogicalId{0, -1, 0, 0, p++});
        }

        it = phisical_constants.find(language);
        if (it == phisical_constants.end())
            return;
        int p = 0;
        for (auto& expr : it->second)
            Parse(LogicalId{0, -1, 0, 0, p++}, expr);
    }

    Solver<Number> solver;

    std::unique_ptr<giac::context> giac_context;

    Language language = Language::English;
    Language last_language = Language::None;

    std::map<Language, std::vector<std::u32string>> si_units = 
        {
            {
                Language::English,
                {
                    //distance
                    U"nm`nanometre`~0.000000001m;",
                    U"mcm`micrometre`~0.000001m;",
                    U"mm`millimetre`~0.001m;",
                    U"cm`centimetre`~0.01m;",
                    U"dm`decimetre`~0.1m;",
                    U"km`kilometre`~1000m;",

                    //time
                    U"ps`picosecond`~0.000000000001s;",
                    U"ns`nanosecond`~0.000000001s;",
                    U"mcs`microsecond`~0.000001s;",
                    U"ms`millisecond`~0.001s;",
                    U"min`minute`~60s;",
                    U"hour~60min;",
                    U"day~24hour;",
                    U"week~7day;",
                    U"year~365day;",

                    //mass
                    U"g`gram`~0.001kg;",
                    U"mcg`microgram`~0.000001kg;",
                    U"mg`milligram`~0.001kg;",
                    U"ton~1000kg;",

                    //electric current
                    U"mcA`microampere`~0.000001A;",
                    U"mA`milliampere`~0.001A;",
                    U"kA`kiloampere`~1000A;",
                    U"MA`megaampere`~1000000A;",

                    //frequency
                    U"Hz`hertz`~1/s;",
                    U"kHz`kilohertz`~1000Hz;",
                    U"MHz`megahertz`~1000kHz;",
                    U"GHz`gigahertz`~1000MHz;",

                    //force
                    U"N`newton`~(kg*m)/(pow(s,2));",
                    U"mcN`micronewton`~0.000001N;",
                    U"mN`millinewton`~0.001N;",
                    U"kN`kilonewton`~1000N;",
                    U"MN`meganewton`~1000kN;",

                    //energy
                    U"J`joule`~N*m;",
                    U"mcJ`microjoule`~0.000001J;",
                    U"mJ`millijoule`~0.001J;",
                    U"kJ`kilojoule`~1000J;",
                    U"MJ`megajoule`~1000000J;",

                    //power
                    U"W`watt`~(J)/(s);",
                    U"mcW`microwatt`~0.000001W;",
                    U"mW`milliwatt`~0.001W;",
                    U"kW`kilowatt`~1000W;",
                    U"MW`megawatt`~1000000W;",

                    //pressure
                    U"Pa`pascal`~(N)/(pow(m,2));",
                    U"mcPa`micropascal`~0.000001Pa;",
                    U"mPa`millipascal`~0.001Pa;",
                    U"kPa`kilopascal`~1000Pa;",
                    U"MPa`megapascal`~1000000Pa;",

                    //luminous intensity
                    U"mccd`microcandela`~0.000001cd;",
                    U"mcd`millicandela`~0.001cd;",
                    U"kcd`kilocandela`~1000cd;",
                    U"Mcd`megacandela`~1000000cd;",

                    //luminous flux
                    U"lm`lumen`~cd*sr;",
                    U"mclm`microlumen`~0.000001lm;",
                    U"mlm`millilumen`~0.001lm;",
                    U"klm`kilolumen`~1000lm;",
                    U"Mlm`megalumen`~1000000lm;",

                    //illuminance
                    U"lx`lux`~lm/pow(m,2);",
                    U"mclx`microlux`~0.000001lx;",
                    U"mlx`millilux`~0.001lx;",
                    U"klx`kilolux`~1000lx;",
                    U"Mlx`megalux`~1000000lx;",

                    //electrical charge
                    U"C`coulomb`~A*s;",
                    U"mcC`microcoulomb`~0.000001C;",
                    U"mC`millicoulomb`~0.001C;",
                    U"kC`kilocoulomb`~1000C;",
                    U"MC`megacoulomb`~1000000C;",

                    //potential
                    U"V`volt`~(J)/(C);",
                    U"pV`pikovolt`~0.000000000001V;",
                    U"nV`nanovolt`~0.000000001V;",
                    U"mcV`microvolt`~0.000001V;",
                    U"mV`millivolt`~0.001V;",
                    U"kV`kilovolt`~1000V;",
                    U"MV`megavolt`~1000000V;",

                    //electrical resistance
                    U"Ohm~V/A;",
                    U"mcOhm`microohm`~0.000001Ohm;",
                    U"mOhm`milliohm`~0.001Ohm;",
                    U"kOhm`kiloohm`~1000Ohm;",
                    U"MOhm`megaohm`~1000000Ohm;",

                    //electrical capacity
                    U"F`farad`~C/V;",
                    U"pF`pikofarad`~0.000000000001F;",
                    U"nF`nanofarad`~0.000000001F;",
                    U"mcF`microfarad`~0.000001F;",
                    U"mF`millifarad`~0.001F;",

                    //magnetic flux
                    U"Wb`weber`~(kg*pow(m,2)/(pow(s,2)*A));",
                    U"mcWb`microweber`~0.000001Wb;",
                    U"mWb`milliweber`~0.001Wb;",
                    U"kWb`kiloweber`~1000Wb;",
                    U"MWb`megaweber`~1000000Wb;",

                    //magnetic field
                    U"T`tesla`~(Wb)/(pow(m,2));",
                    U"mcT`microtesla`~0.000001T;",
                    U"mT`millitesla`~0.001T;",
                    U"kT`kilotesla`~1000T;",
                    U"MT`megatesla`~1000000T;",

                    //electrical inductance
                    U"H`henry`~(kg*pow(m,2))/(pow(s,2)*pow(A,2));",
                    U"pH`pikohenry`~0.000000000001H;",
                    U"nH`nanohenry`~0.000000001H;",
                    U"mcH`microhenry`~0.000001H;",
                    U"mH`millihenry`~0.001H;",

                    //electric conductance
                    U"S`siemens`~(1)/(Ohm);",
                    U"mcS`microsiemens`~0.000001S;",
                    U"mS`millisiemens`~0.001S;",
                    U"kS`kilosiemens`~1000S;",
                    U"MS`megasiemens`~1000000S;",

                    //ionizing radiation
                    U"Gy`gray`~(J)/(kg);",
                    U"mcGy`microgray`~0.000001Gy;",
                    U"mGy`milligray`~0.001Gy;",
                    U"kGy`kilogray`~1000Gy;",
                    U"MGy`megagray`~1000000Gy;"
                }
            },
            {
                Language::Russian,
                {
                    //distance
                    U"нм`нанометр`~0.000000001м;",
                    U"мкм`микрометр`~0.000001м;",
                    U"мм`миллиметр`~0.001м;",
                    U"см`сантиметр`~0.01м;",
                    U"дм`дециметр`~0.1м;",
                    U"км`километр`~1000м;",

                    //time
                    U"пс`пикосекунда`~0.000000000001с;",
                    U"нс`наносекунда`~0.000000001с;",
                    U"мкс`микросекунда`~0.000001с;",
                    U"мс`миллисекунда`~0.001с;",
                    U"мин`минута`~60с;",
                    U"час~60мин;",
                    U"сутки~24час;",
                    U"неделя~7сутки;",
                    U"год~365сутки;",

                    //mass
                    U"г`грамм`~0.001кг;",
                    U"мкг`микрограмм`~0.000001кг;",
                    U"мг`миллиграмм`~0.001кг;",
                    U"тонна~1000кг;",

                    //electric current
                    U"мкА`микроампер`~0.000001А;",
                    U"мА`миллиампер`~0.001А;",
                    U"кА`килоампер`~1000А;",
                    U"МА`мегаампер`~1000000А;",

                    //frequency
                    U"Гц`герц`~1/с;",
                    U"кГц`килогерц`~1000Гц;",
                    U"МГц`мегагерц`~1000кГц;",
                    U"ГГц`гигагерц`~1000МГц;",

                    //force
                    U"Н`ньютон`~кг*м/pow(с,2);",
                    U"мкН`микроньютон`~0.000001Н;",
                    U"мН`миллиньютон`~0.001Н;",
                    U"кН`килоньютон`~1000Н;",
                    U"МН`меганьютон`~1000кН;",

                    //energy
                    U"Дж`джоуль`~Н*м;",
                    U"мкДж`микроджоуль`~0.000001Дж;",
                    U"мДж`миллиджоуль`~0.001Дж;",
                    U"кДж`килоджоуль`~1000Дж;",
                    U"МДж`мегаджоуль`~1000000Дж;",

                    //power
                    U"Вт`ватт`~Дж/с;",
                    U"мкВт`микроватт`~0.000001Вт;",
                    U"мВт`милливатт`~0.001Вт;",
                    U"кВт`киловатт`~1000Вт;",
                    U"МВт`мегаватт`~1000000Вт;",

                    //pressure
                    U"Па`паскаль`~(Н)/pow(м,2);",
                    U"мкПа`микропаскаль`~0.000001Па;",
                    U"мПа`миллипаскаль`~0.001Па;",
                    U"кПа`килопаскаль`~1000Па;",
                    U"МПа`мегапаскаль`~1000000Па;",

                    //luminous intensity
                    U"мкКд`микрокандела`~0.000001Кд;",
                    U"мКд`милликандела`~0.001Кд;",
                    U"кКд`килокандела`~1000Кд;",
                    U"МКд`мегакандела`~1000000Кд;",

                    //luminous flux
                    U"лм`люмен`~Кд*ср;",
                    U"мклм`микролюмен`~0.000001лм;",
                    U"млм`миллилюмен`~0.001лм;",
                    U"клм`килолюмен`~1000лм;",
                    U"Млм`мегалюмен`~1000000лм;",

                    //illuminance
                    U"лк`люкс`~лм/pow(м,2);",
                    U"мклк`микролюкс`~0.000001лк;",
                    U"млк`миллилюкс`~0.001лк;",
                    U"клк`килолюкс`~1000лк;",
                    U"Млк`мегалюкс`~1000000лк;",

                    //electrical charge
                    U"Кл`кулон`~А*с;",
                    U"мкКл`микрокулон`~0.000001Кл;",
                    U"мКл`милликулон`~0.001Кл;",
                    U"кКл`килокулон`~1000Кл;",
                    U"МКл`мегакулон`~1000000Кл;",

                    //potential
                    U"В`вольт`~Дж/Кл;",
                    U"пВ`пиковольт`~0.000000000001В;",
                    U"нВ`нановольт`~0.000000001В;",
                    U"мкВ`микровольт`~0.000001В;",
                    U"мВ`милливольт`~0.001В;",
                    U"кВ`киловольт`~1000В;",
                    U"МВ`мегавольт`~1000000В;",

                    //electrical resistance
                    U"Ом~В/А;",
                    U"мкОм`микроом`~0.000001Ом;",
                    U"мОм`миллиом`~0.001Ом;",
                    U"кОм`килоом`~1000Ом;",
                    U"МОм`мегаом`~1000000Ом;",

                    //electrical capacity
                    U"Ф`фарад`~Кл/В;",
                    U"пФ`пикофарад`~0.000000000001Ф;",
                    U"нФ`нанофарад`~0.000000001Ф;",
                    U"мкФ`микрофарад`~0.000001Ф;",
                    U"мФ`миллифарад`~0.001Ф;",

                    //magnetic flux
                    U"Вб`вебер`~(кг*pow(м,2)/(pow(с,2)*А));",
                    U"мкВб`микровебер`~0.000001Вб;",
                    U"мВб`милливебер`~0.001Вб;",
                    U"кВб`киловебер`~1000Вб;",
                    U"МВб`мегавебер`~1000000Вб;",

                    //magnetic field
                    U"Тл`тесла`~(Вб)/(pow(м,2));",
                    U"мкТл`микротесла`~0.000001Тл;",
                    U"мТл`миллитесла`~0.001Тл;",
                    U"кТл`килотесла`~1000Тл;",
                    U"МТл`мегатесла`~1000000Тл;",

                    //electrical inductance
                    U"Гн`генри`~(кг*pow(м,2))/(pow(с,2)*pow(А,2));",
                    U"пГн`пикогенри`~0.000000000001Гн;",
                    U"нГн`наногенри`~0.000000001Гн;",
                    U"мкГн`микрогенри`~0.000001Гн;",
                    U"мГн`миллигенри`~0.001Гн;",

                    //electric conductance
                    U"См`сименс`~(1)/(Ом);",
                    U"мкСм`микросименс`~0.000001См;",
                    U"мСм`миллисименс`~0.001См;",
                    U"кСм`килосименс`~1000См;",
                    U"МСм`мегасименс`~1000000См;",

                    //ionizing radiation
                    U"Гр`грей`~(Дж)/(кг);",
                    U"мкГр`микрогрей`~0.000001Гр;",
                    U"мГр`миллигрей`~0.001Гр;",
                    U"кГр`килогрей`~1000Гр;",
                    U"МГр`мегагрей`~1000000Гр;",
                }
            },
            {
                Language::Spanish,
                {
                    //distance
                    U"nm`nanómetro`~0.000000001m;",
                    U"mcm`micrómetro`~0.000001m;",
                    U"mm`milímetro`~0.001m;",
                    U"cm`centímetro`~0.01m;",
                    U"dm`decímetro`~0.1m;",
                    U"km`kilómetro`~1000m;",

                    //time
                    U"ps`picosegundo`~0.000000000001s;",
                    U"ns`nanosegundo`~0.000000001s;",
                    U"mcs`microsegundo`~0.000001s;",
                    U"ms`milisegundo`~0.001s;",
                    U"min`minuto`~60s;",
                    U"hora~60min;",
                    U"día~24hora;",
                    U"semana~7día;",
                    U"año~365día;",

                    //mass
                    U"g`gramo`~0.001kg;",
                    U"mcg`microgramo`~0.000001kg;",
                    U"mg`miligramo`~0.001kg;",
                    U"ton`tonelada`~1000kg;",

                    //electric current
                    U"mcA`microamperio`~0.000001A;",
                    U"mA`miliamperio`~0.001A;",
                    U"kA`kiloamperio`~1000A;",
                    U"MA`megaamperio`~1000000A;",

                    //frequency
                    U"Hz`hertz`~1/s;",
                    U"kHz`kilohertz`~1000Hz;",
                    U"MHz`megahertz`~1000kHz;",
                    U"GHz`gigahertz`~1000MHz;",

                    //force
                    U"N`newton`~(kg*m)/(pow(s,2));",
                    U"mcN`micronewton`~0.000001N;",
                    U"mN`milinewton`~0.001N;",
                    U"kN`kilonewton`~1000N;",
                    U"MN`meganewton`~1000000N;",

                    //energy
                    U"J`julio`~N*m;",
                    U"mcJ`microjulio`~0.000001J;",
                    U"mJ`milijulio`~0.001J;",
                    U"kJ`kilojulio`~1000J;",
                    U"MJ`megajulio`~1000000J;",

                    //power
                    U"W`vatio`~(J)/(s);",
                    U"mcW`microvatio`~0.000001W;",
                    U"mW`miliwatio`~0.001W;",
                    U"kW`kilovatio`~1000W;",
                    U"MW`megavatio`~1000000W;",

                    //pressure
                    U"Pa`pascal`~(N)/(pow(m,2));",
                    U"mcPa`micropascal`~0.000001Pa;",
                    U"mPa`milipascal`~0.001Pa;",
                    U"kPa`kilopascal`~1000Pa;",
                    U"MPa`megapascal`~1000000Pa;",

                    //luminous intensity
                    U"mccd`microcandela`~0.000001cd;",
                    U"mcd`milicandela`~0.001cd;",
                    U"kcd`kilocandela`~1000cd;",
                    U"Mcd`megacandela`~1000000cd;",

                    //luminous flux
                    U"lm`lumen`~cd*sr;",
                    U"mclm`microlumen`~0.000001lm;",
                    U"mlm`mililumen`~0.001lm;",
                    U"klm`kilolumen`~1000lm;",
                    U"Mlm`megalumen`~1000000lm;",

                    //illuminance
                    U"lx`lux`~lm/pow(m,2);",
                    U"mclx`microlux`~0.000001lx;",
                    U"mlx`mililux`~0.001lx;",
                    U"klx`kilolux`~1000lx;",
                    U"Mlx`megalux`~1000000lx;",

                    //electric charge
                    U"C`culombio`~A*s;",
                    U"mcC`microculombio`~0.000001C;",
                    U"mC`miliculombio`~0.001C;",
                    U"kC`kiloculombio`~1000C;",
                    U"MC`megaculombio`~1000000C;",

                    //electric potential
                    U"V`voltio`~(J)/(C);",
                    U"pV`picovoltio`~0.000000000001V;",
                    U"nV`nanovoltio`~0.000000001V;",
                    U"mcV`microvoltio`~0.000001V;",
                    U"mV`milivoltio`~0.001V;",
                    U"kV`kilovoltio`~1000V;",
                    U"MV`megavoltio`~1000000V;",

                    //electric resistance
                    U"Ohm`ohmio`~V/A;",
                    U"mcOhm`microohmio`~0.000001Ohm;",
                    U"mOhm`miliohmio`~0.001Ohm;",
                    U"kOhm`kiloohmio`~1000Ohm;",
                    U"MOhm`megaohmio`~1000000Ohm;",

                    //electric capacitance
                    U"F`faradio`~C/V;",
                    U"pF`picofaradio`~0.000000000001F;",
                    U"nF`nanofaradio`~0.000000001F;",
                    U"mcF`microfaradio`~0.000001F;",
                    U"mF`milifaradio`~0.001F;",

                    //magnetic flux
                    U"Wb`weber`~(kg*pow(m,2)/(pow(s,2)*A));",
                    U"mcWb`microweber`~0.000001Wb;",
                    U"mWb`miliweber`~0.001Wb;",
                    U"kWb`kiloweber`~1000Wb;",
                    U"MWb`megaweber`~1000000Wb;",

                    //magnetic field
                    U"T`tesla`~(Wb)/(pow(m,2));",
                    U"mcT`microtesla`~0.000001T;",
                    U"mT`militesla`~0.001T;",
                    U"kT`kilotesla`~1000T;",
                    U"MT`megatesla`~1000000T;",

                    //inductance
                    U"H`henrio`~(kg*pow(m,2))/(pow(s,2)*pow(A,2));",
                    U"pH`picohenrio`~0.000000000001H;",
                    U"nH`nanohenrio`~0.000000001H;",
                    U"mcH`microhenrio`~0.000001H;",
                    U"mH`milihenrio`~0.001H;",

                    //electric conductance
                    U"S`siemensio`~(1)/(Ohm);",
                    U"mcS`microsiemensio`~0.000001S;",
                    U"mS`milisiemensio`~0.001S;",
                    U"kS`kilosiemensio`~1000S;",
                    U"MS`megasiemensio`~1000000S;",

                    //ionizing radiation
                    U"Gy`gray`~(J)/(kg);",
                    U"mcGy`microgray`~0.000001Gy;",
                    U"mGy`miligray`~0.001Gy;",
                    U"kGy`kilogray`~1000Gy;",
                    U"MGy`megagray`~1000000Gy;"
                }
            },
            {
                Language::BrazilianPortuguese,
                {
                    //distance
                    U"nm`nanômetro`~0.000000001m;",
                    U"mcm`micrômetro`~0.000001m;",
                    U"mm`milímetro`~0.001m;",
                    U"cm`centímetro`~0.01m;",
                    U"dm`decímetro`~0.1m;",
                    U"km`quilômetro`~1000m;",

                    //time
                    U"ps`picosegundo`~0.000000000001s;",
                    U"ns`nanosegundo`~0.000000001s;",
                    U"mcs`microsegundo`~0.000001s;",
                    U"ms`milissegundo`~0.001s;",
                    U"min`minuto`~60s;",
                    U"hora~60min;",
                    U"dia~24hora;",
                    U"semana~7dia;",
                    U"ano~365dia;",

                    //mass
                    U"g`grama`~0.001kg;",
                    U"mcg`micrograma`~0.000001kg;",
                    U"mg`miligrama`~0.001kg;",
                    U"ton`tonelada`~1000kg;",

                    //electric current
                    U"mcA`microampère`~0.000001A;",
                    U"mA`miliampère`~0.001A;",
                    U"kA`quiloampère`~1000A;",
                    U"MA`megaampère`~1000000A;",

                    //frequency
                    U"Hz`hertz`~1/s;",
                    U"kHz`quilohertz`~1000Hz;",
                    U"MHz`megahertz`~1000kHz;",
                    U"GHz`gigahertz`~1000MHz;",

                    //force
                    U"N`newton`~(kg*m)/(pow(s,2));",
                    U"mcN`micronewton`~0.000001N;",
                    U"mN`milinewton`~0.001N;",
                    U"kN`quilonewton`~1000N;",
                    U"MN`meganewton`~1000000N;",

                    //energy
                    U"J`joule`~N*m;",
                    U"mcJ`microjoule`~0.000001J;",
                    U"mJ`milijoule`~0.001J;",
                    U"kJ`quilojoule`~1000J;",
                    U"MJ`megajoule`~1000000J;",

                    //power
                    U"W`watt`~(J)/(s);",
                    U"mcW`microwatt`~0.000001W;",
                    U"mW`miliwatt`~0.001W;",
                    U"kW`quilowatt`~1000W;",
                    U"MW`megawatt`~1000000W;",

                    //pressure
                    U"Pa`pascal`~(N)/(pow(m,2));",
                    U"mcPa`micropascal`~0.000001Pa;",
                    U"mPa`milipascal`~0.001Pa;",
                    U"kPa`quilopascal`~1000Pa;",
                    U"MPa`megapascal`~1000000Pa;",

                    //luminous intensity
                    U"mccd`microcandela`~0.000001cd;",
                    U"mcd`milicandela`~0.001cd;",
                    U"kcd`quilocandela`~1000cd;",
                    U"Mcd`megacandela`~1000000cd;",

                    //luminous flux
                    U"lm`lúmen`~cd*sr;",
                    U"mclm`microlúmen`~0.000001lm;",
                    U"mlm`mililúmen`~0.001lm;",
                    U"klm`quilolúmen`~1000lm;",
                    U"Mlm`megalúmen`~1000000lm;",

                    //illuminance
                    U"lx`lux`~lm/pow(m,2);",
                    U"mclx`microlux`~0.000001lx;",
                    U"mlx`mililux`~0.001lx;",
                    U"klx`quilolux`~1000lx;",
                    U"Mlx`megalux`~1000000lx;",

                    //electric charge
                    U"C`coulomb`~A*s;",
                    U"mcC`microcoulomb`~0.000001C;",
                    U"mC`milicoulomb`~0.001C;",
                    U"kC`quilocoulomb`~1000C;",
                    U"MC`megacoulomb`~1000000C;",

                    //electric potential
                    U"V`volt`~(J)/(C);",
                    U"pV`picovolt`~0.000000000001V;",
                    U"nV`nanovolt`~0.000000001V;",
                    U"mcV`microvolt`~0.000001V;",
                    U"mV`milivolt`~0.001V;",
                    U"kV`quilovolt`~1000V;",
                    U"MV`megavolt`~1000000V;",

                    //electric resistance
                    U"Ohm`ohm`~V/A;",
                    U"mcOhm`microohm`~0.000001Ohm;",
                    U"mOhm`miliohm`~0.001Ohm;",
                    U"kOhm`quiloohm`~1000Ohm;",
                    U"MOhm`megaohm`~1000000Ohm;",

                    //electric capacitance
                    U"F`farad`~C/V;",
                    U"pF`picofarad`~0.000000000001F;",
                    U"nF`nanofarad`~0.000000001F;",
                    U"mcF`microfarad`~0.000001F;",
                    U"mF`milifarad`~0.001F;",

                    //magnetic flux
                    U"Wb`weber`~(kg*pow(m,2)/(pow(s,2)*A));",
                    U"mcWb`microweber`~0.000001Wb;",
                    U"mWb`miliweber`~0.001Wb;",
                    U"kWb`quiloweber`~1000Wb;",
                    U"MWb`megaweber`~1000000Wb;",

                    //magnetic field
                    U"T`tesla`~(Wb)/(pow(m,2));",
                    U"mcT`microtesla`~0.000001T;",
                    U"mT`militesla`~0.001T;",
                    U"kT`quilotesla`~1000T;",
                    U"MT`megatesla`~1000000T;",

                    //inductance
                    U"H`henry`~(kg*pow(m,2))/(pow(s,2)*pow(A,2));",
                    U"pH`picohenry`~0.000000000001H;",
                    U"nH`nanohenry`~0.000000001H;",
                    U"mcH`microhenry`~0.000001H;",
                    U"mH`milihenry`~0.001H;",

                    //electric conductance
                    U"S`siemens`~(1)/(Ohm);",
                    U"mcS`microsiemens`~0.000001S;",
                    U"mS`milisiemens`~0.001S;",
                    U"kS`quilosiemens`~1000S;",
                    U"MS`megasiemens`~1000000S;",

                    //ionizing radiation
                    U"Gy`gray`~(J)/(kg);",
                    U"mcGy`microgray`~0.000001Gy;",
                    U"mGy`miligray`~0.001Gy;",
                    U"kGy`quilogray`~1000Gy;",
                    U"MGy`megagray`~1000000Gy;"
                }
            }
        };

    std::map<Language, std::vector<std::u32string>> other_units = 
        {
            {
                Language::English,
                {
                    //distance
                    U"ang`angstrom`~0.000000000001m;",

                    //temperature
                    U"°C~K;",
                    
                    //data
                    U"byte~8bit;",
                    U"kbyte~1024byte;",
                    U"Mbyte~1024kbyte;",
                    U"Gbyte~1024Mbyte;",
                    U"Tbyte~1024Gbyte;",

                    //square
                    U"a`ar`~100*pow(m,2);", //ar
                    U"ha`hectar`~100a;", //hectar

                    //volume
                    U"l`litre`~pow(dm,3);", //litre
                    U"ml`millilitre`~0.001l;",
                    U"dl`decilitre`~0.1l;",
                    U"dal~10l;",
                    U"hl`hectolitre`~100l;",

                    //money
                    U"kop~0.01₽;", //kopeck
                    U"dollar~$;",
                    U"¢~0.01$;", //cent
                    U"cent~¢;",
                    U"euro~€;",
                    U"ct~0.01€;", //eurocent
                    U"eurocent~0.01€;",
                    U"yuan~¥;",
                    U"rupee~₹;",
                    U"real~R$;"
                }
            },
            {
                Language::Russian,
                {
                    //distance
                    U"анг`ангстрем`~0.0000000001м;",

                    //temperature
                    U"°C~К;",

                    //data
                    U"байт~8бит;",
                    U"кбайт~1024байт;",
                    U"Мбайт~1024кбайт;",
                    U"Гбайт~1024Мбайт;",
                    U"Тбайт~1024Гбайт;",

                    //square
                    U"ар~100*pow(м,2);", //ar
                    U"га`гектар`~100ар;", //hectar

                    //volume
                    U"л`литр`~pow(дм,3);", //litre
                    U"мл`миллилитр`~0.001л;",
                    U"дл`децилитр`~0.1л;",
                    U"дал~10л;",
                    U"гл`гекталитр`~100л;",

                    //money
                    U"Р~₽;",
                    U"коп~0.01₽;",
                    U"копейка~коп;",
                    U"доллар~$;",
                    U"цент~0.01$;",
                    U"евро~€;",
                    U"евроцент~0.01€;",
                    U"юань~¥;",
                    U"рупия~₹;",
                    U"реал~R$;"
                }
            },
            {
                Language::Spanish,
                {
                    //distance
                    U"ang`ångström`~0.000000000001m;",

                    //temperature
                    U"°C~K;",

                    //data
                    U"byte~8bit;",
                    U"kbyte~1024byte;",
                    U"Mbyte~1024kbyte;",
                    U"Gbyte~1024Mbyte;",
                    U"Tbyte~1024Gbyte;",

                    //square
                    U"a`área`~100*pow(m,2);", // ar
                    U"ha`hectárea`~100a;", // hectar

                    //volume
                    U"l`litro`~pow(dm,3);",
                    U"ml`mililitro`~0.001l;",
                    U"dl`decilitro`~0.1l;",
                    U"dal`decalitro`~10l;",
                    U"hl`hectolitro`~100l;",

                    //money
                    U"kop`kopeck`~0.01₽;",
                    U"dólar~$;",
                    U"¢~0.01$;",
                    U"cent~¢;",
                    U"euro~€;",
                    U"ct~0.01€;",
                    U"eurocent~0.01€;",
                    U"yuan~¥;",
                    U"rupee~₹;",
                    U"real~R$;"
                }
            },
            {
                Language::BrazilianPortuguese,
                {
                    //distance
                    U"ang`ångström`~0.000000000001m;",

                    //temperature
                    U"°C~K;",

                    //data
                    U"byte~8bit;",
                    U"kbyte~1024byte;",
                    U"Mbyte~1024kbyte;",
                    U"Gbyte~1024Mbyte;",
                    U"Tbyte~1024Gbyte;",

                    //square
                    U"a`are`~100*pow(m,2);",
                    U"ha`hectare`~100a;",

                    //volume
                    U"l`litro`~pow(dm,3);",
                    U"ml`mililitro`~0.001l;",
                    U"dl`decilitro`~0.1l;",
                    U"dal`decalitro`~10l;",
                    U"hl`hectolitro`~100l;",

                    //money
                    U"kop`kopeck`~0.01₽;",
                    U"dólar~$;",
                    U"¢~0.01$;",
                    U"cent~¢;",
                    U"euro~€;",
                    U"ct~0.01€;",
                    U"eurocent~0.01€;",
                    U"yuan~¥;",
                    U"rupia~₹;",
                    U"real~R$;"
                }
            }
        };

    std::map<Language, std::vector<std::u32string>> russian_units = 
        {
            {
                Language::English,
                {
                    //distance
                    U"tochka{rus}~(254)/(1000)mm;",
                    U"liniya{rus}~10tochka{rus};",
                    U"sotka{rus}~84tochka{rus};",
                    U"dyum{rus}~10liniya{rus};",
                    U"vershok{rus}~(7)/(4)dyum{rus};",
                    U"chetvert{rus}~7dyum{rus};",
                    U"fut{rus}~12dyum{rus};",
                    U"arshin{rus}~28dyum{rus};",
                    U"sazhen{rus}~7fut{rus};",
                    U"versta{rus}~500sazhen{rus};",

                    //mass
                    U"funt{rus}~0.4095124*kg;",
                    U"batman{rus}~10funt{rus};",
                    U"pud{rus}~40funt{rus};",
                    U"bezmen{rus}~(1)/(16)pud{rus};",
                    U"kongar{rus}~40.95kg;",
                    U"pirog{rus}~43mg;",
                    U"zolotnik{rus}~4.266g;",
                    U"dolya{rus}~(1)/(96)zolotnik{rus};",
                    U"pochka{rus}~(1)/(25)zolotnik{rus};",
                    U"lot{rus}~zolotnik{rus};",

                    //time
                    U"s{rus}~s;",
                    U"min{rus}~min;",
                    U"hour{rus}~hour;",

                    //volume
                    U"stakan{rus}~0.273l;",
                    U"polygarnets{rus}~6stakan{rus};",
                    U"garnets{rus}~12stakan{rus};",
                    U"chetveric{rus}~8garnets{rus};",
                    U"osmina{rus}~4chetveric{rus};",
                    U"polosminy{rus}~52.48*l;",
                    U"chet{rus}~64garnets{rus};",
                    U"polovnik{rus}~419.84l;",
                    U"kadka{rus}~2polovnik{rus};",
                    U"shkalic{rus}~61.5ml;",
                    U"charka{rus}~2shkalic{rus};",
                    U"kosushka{rus}~5shkalic{rus};",
                    U"shtof{rus}~10charka{rus};",
                    U"vedro{rus}~4chet{rus};",
                    U"bochka{rus}~40vedro{rus};",
                }
            },
            {
                Language::Russian,
                {
                    //distance
                    U"точка{rus}~(254)/(1000)мм;",
                    U"линия{rus}~10точка{rus};",
                    U"сотка{rus}~84точка{rus};",
                    U"дюйм{rus}~10линия{rus};",
                    U"вершок{rus}~(7)/(4)дюйм{rus};",
                    U"четверть{rus}~7дюйм{rus};",
                    U"фут{rus}~12дюйм{rus};",
                    U"аршин{rus}~28дюйм{rus};",
                    U"сажень{rus}~7фут{rus};",
                    U"верста{rus}~500сажень{rus};",

                    //mass
                    U"фунт{rus}~0.4095124*кг;",
                    U"батман{rus}~10фунт{rus};",
                    U"пуд{rus}~40фунт{rus};",
                    U"безмен{rus}~(1)/(16)пуд{rus};",
                    U"конгарь{rus}~40.95кг;",
                    U"пирог{rus}~43мг;",
                    U"золотник{rus}~4.266г;",
                    U"доля{rus}~(1)/(96)золотник{rus};",
                    U"почка{rus}~(1)/(25)золотник{rus};",
                    U"лот{rus}~золотник{rus};",

                    //time
                    U"с{rus}~с;",
                    U"мин{rus}~мин;",
                    U"час{rus}~час;",

                    //volume
                    U"стакан{rus}~0.273л;",
                    U"полигарнец{rus}~6стакан{rus};",
                    U"гарнец{rus}~12стакан{rus};",
                    U"четверик{rus}~8гарнец{rus};",
                    U"осмина{rus}~4четверик{rus};",
                    U"полосмины{rus}~52.48л;",
                    U"чет{rus}~64гарнец{rus};",
                    U"половник{rus}~419.84л;",
                    U"кадка{rus}~2половник{rus};",
                    U"шкалик{rus}~61.5мл;",
                    U"чарка{rus}~2шкалик{rus};",
                    U"косушка{rus}~5шкалик{rus};",
                    U"штоф{rus}~10чарка{rus};",
                    U"ведро{rus}~4чет{rus};",
                    U"бочка{rus}~40ведро{rus};",
                }
            },
            {
                Language::Spanish,
                {
                    //distance
                    U"punta{rus}~(254)/(1000)mm;",
                    U"línea{rus}~10punta{rus};",
                    U"centena{rus}~84punta{rus};",
                    U"pulgada{rus}~10línea{rus};",
                    U"vershok{rus}~(7)/(4)pulgada{rus};",
                    U"cuarto{rus}~7pulgada{rus};",
                    U"pie{rus}~12pulgada{rus};",
                    U"arshin{rus}~28pulgada{rus};",
                    U"sazhen{rus}~7pie{rus};",
                    U"versta{rus}~500sazhen{rus};",

                    //mass
                    U"libra{rus}~0.4095124*kg;",
                    U"batman{rus}~10libra{rus};",
                    U"pood{rus}~40libra{rus};",
                    U"bezmien{rus}~(1)/(16)pood{rus};",
                    U"kongar{rus}~40.95kg;",
                    U"pirog{rus}~43mg;",
                    U"zolotnik{rus}~4.266g;",
                    U"dole{rus}~(1)/(96)zolotnik{rus};",
                    U"pochka{rus}~(1)/(25)zolotnik{rus};",
                    U"lot{rus}~zolotnik{rus};",

                    //time
                    U"s{rus}~s;",
                    U"min{rus}~min;",
                    U"hora{rus}~hora;",

                    //volume
                    U"vaso{rus}~0.273l;",
                    U"poligarnets{rus}~6vaso{rus};",
                    U"garnets{rus}~12vaso{rus};",
                    U"chetverik{rus}~8garnets{rus};",
                    U"osmina{rus}~4chetverik{rus};",
                    U"polosminy{rus}~52.48l;",
                    U"chet{rus}~64garnets{rus};",
                    U"polovnik{rus}~419.84l;",
                    U"kadka{rus}~2polovnik{rus};",
                    U"shkalik{rus}~61.5ml;",
                    U"charka{rus}~2shkalik{rus};",
                    U"kosushka{rus}~5shkalik{rus};",
                    U"shtof{rus}~10charka{rus};",
                    U"vedro{rus}~4chet{rus};",
                    U"bochka{rus}~40vedro{rus};"
                }
            },
            {
                Language::BrazilianPortuguese,
                {
                    //distance
                    U"ponto{rus}~(254)/(1000)mm;",
                    U"linha{rus}~10ponto{rus};",
                    U"centena{rus}~84ponto{rus};",
                    U"polegada{rus}~10linha{rus};",
                    U"vershok{rus}~(7)/(4)polegada{rus};",
                    U"quarto{rus}~7polegada{rus};",
                    U"pé{rus}~12polegada{rus};",
                    U"arshin{rus}~28polegada{rus};",
                    U"sazhen{rus}~7pé{rus};",
                    U"versta{rus}~500sazhen{rus};",

                    //mass
                    U"libra{rus}~0.4095124*kg;",
                    U"batman{rus}~10libra{rus};",
                    U"pood{rus}~40libra{rus};",
                    U"bezmien{rus}~(1)/(16)pood{rus};",
                    U"kongar{rus}~40.95kg;",
                    U"pirog{rus}~43mg;",
                    U"zolotnik{rus}~4.266g;",
                    U"dole{rus}~(1)/(96)zolotnik{rus};",
                    U"pochka{rus}~(1)/(25)zolotnik{rus};",
                    U"lot{rus}~zolotnik{rus};",

                    //time
                    U"s{rus}~s;",
                    U"min{rus}~min;",
                    U"hora{rus}~hora;",

                    //volume
                    U"copo{rus}~0.273l;",
                    U"poligarnets{rus}~6copo{rus};",
                    U"garnets{rus}~12copo{rus};",
                    U"chetverik{rus}~8garnets{rus};",
                    U"osmina{rus}~4chetverik{rus};",
                    U"polosminy{rus}~52.48l;",
                    U"chet{rus}~64garnets{rus};",
                    U"polovnik{rus}~419.84l;",
                    U"kadka{rus}~2polovnik{rus};",
                    U"shkalik{rus}~61.5ml;",
                    U"charka{rus}~2shkalik{rus};",
                    U"kosushka{rus}~5shkalik{rus};",
                    U"shtof{rus}~10charka{rus};",
                    U"vedro{rus}~4chet{rus};",
                    U"bochka{rus}~40vedro{rus};"
                }
            }
        };

    std::map<Language, std::vector<std::u32string>> us_units = 
        {
            {
                Language::English, 
                {
                    //distance
                    U"inch{us}~25.4mm;",
                    U"foot{us}~12inch{us};",
                    U"yard{us}~3foot{us};",
                    U"mile{us}~1760yard{us};",

                    //mass
                    U"ounce{us}~28.3495g;",
                    U"pound{us}~16ounce{us};",
                    U"stone{us}~14pound{us};",
                    U"hundredweight{us}~100pound{us};",
                    U"ton{us}~2000pound{us};",

                    //volume
                    U"teaspoon{us}~4.92892ml;",
                    U"tablespoon{us}~14.7868ml;",
                    U"fluid_ounce{us}~29.5735ml;",
                    U"cup{us}~236.588ml;",
                    U"pint{us}~16fluid_ounce{us};",
                    U"quart{us}~2pint{us};",
                    U"gallon{us}~4quart{us};",
                    U"dry_pint{us}~550.6ml;",
                    U"dry_quart{us}~1.101l;",
                    U"dry_gallon{us}~4.4048l;",
                    U"bushel{us}~35.2391l;",
                    U"peck{us}~8.8098l;",

                    //abbreviations
                    U"in{us}~inch{us};",
                    U"ft{us}~foot{us};",
                    U"yd{us}~yard{us};",
                    U"mi{us}~mile{us};",
                    U"oz{us}~ounce{us};",
                    U"lb{us}~pound{us};",
                    U"st{us}~stone{us};",
                    U"cwt{us}~hundredweight{us};",
                    U"tsp{us}~teaspoon{us};",
                    U"tbsp{us}~tablespoon{us};",
                    U"fl_oz{us}~fluid_ounce{us};",
                    U"pt{us}~pint{us};",
                    U"qt{us}~quart{us};",
                    U"gal{us}~gallon{us};",
                    U"dry_pt{us}~dry_pint{us};",
                    U"dry_qt{us}~dry_quart{us};",
                    U"dry_gal{us}~dry_gallon{us};",
                    U"pk{us}~peck{us};",
                    U"bu{us}~bushel{us};"
                }
            },
            {
                Language::Russian, 
                {
                    //distance
                    U"дюйм{us}~25.4мм;",
                    U"фут{us}~12дюйм{us};",
                    U"ярд{us}~3фут{us};",
                    U"миля{us}~1760ярд{us};",

                    //mass
                    U"унция{us}~28.3495г;",
                    U"фунт{us}~16унция{us};",
                    U"стоун{us}~14фунт{us};",
                    U"центнер{us}~100фунт{us};",
                    U"тонна{us}~2000фунт{us};",

                    //volume
                    U"чайная_ложка{us}~4.92892мл;",
                    U"столовая_ложка{us}~14.7868мл;",
                    U"жидкая_унция{us}~29.5735мл;",
                    U"чашка{us}~236.588мл;",
                    U"пинта{us}~16жидкая_унция{us};",
                    U"кварта{us}~2пинта{us};",
                    U"галлон{us}~4кварта{us};",
                    U"сухая_пинта{us}~550.6мл;",
                    U"сухая_кварта{us}~1.101л;",
                    U"сухой_галлон{us}~4.4048л;",
                    U"бушель{us}~35.2391л;",
                    U"пек{us}~8.8098л;"
                }
            },
            {
                Language::Spanish, 
                {
                    //distance
                    U"pulgada{us}~25.4mm;",
                    U"pie{us}~12pulgada{us};",
                    U"yarda{us}~3pie{us};",
                    U"milla{us}~1760yarda{us};",

                    //mass
                    U"onza{us}~28.3495g;",
                    U"libra{us}~16onza{us};",
                    U"stone{us}~14libra{us};",
                    U"centena_corta{us}~100libra{us};",
                    U"tonelada_corta{us}~2000libra{us};",

                    //volume
                    U"cucharadita{us}~4.92892ml;",
                    U"cucharada{us}~14.7868ml;",
                    U"onza_líquida{us}~29.5735ml;",
                    U"taza{us}~236.588ml;",
                    U"pinta{us}~16onza_líquida{us};",
                    U"cuarto_de_galón{us}~2pinta{us};",
                    U"galón{us}~4cuarto_de_galón{us};",
                    U"pinta_seca{us}~550.6ml;",
                    U"cuarto_seco{us}~1.101l;",
                    U"galón_seco{us}~4.4048l;",
                    U"bushel{us}~35.2391l;",
                    U"peck{us}~8.8098l;",

                    //abbreviations
                    U"in{us}~pulgada{us};",
                    U"ft{us}~pie{us};",
                    U"yd{us}~yarda{us};",
                    U"mi{us}~milla{us};",
                    U"oz{us}~onza{us};",
                    U"lb{us}~libra{us};",
                    U"st{us}~stone{us};",
                    U"cwt{us}~centena_corta{us};",
                    U"ton{us}~tonelada_corta{us};",
                    U"tsp{us}~cucharadita{us};",
                    U"tbsp{us}~cucharada{us};",
                    U"fl_oz{us}~onza_líquida{us};",
                    U"cup{us}~taza{us};",
                    U"pt{us}~pinta{us};",
                    U"qt{us}~cuarto_de_galón{us};",
                    U"gal{us}~galón{us};",
                    U"dry_pt{us}~pinta_seca{us};",
                    U"dry_quart{us}~cuarto_seco{us};",
                    U"dry_gal{us}~galón_seco{us};",
                    U"pk{us}~peck{us};",
                    U"bu{us}~bushel{us};"
                }
            },
            {
                Language::BrazilianPortuguese,
                {
                    //distance
                    U"polegada{us}~25.4mm;",
                    U"pé{us}~12polegada{us};",
                    U"jarda{us}~3pé{us};",
                    U"milha{us}~1760jarda{us};",

                    //mass
                    U"onça{us}~28.3495g;",
                    U"libra{us}~16onça{us};",
                    U"stone{us}~14libra{us};",
                    U"centena_curta{us}~100libra{us};",
                    U"tonelada_curta{us}~2000libra{us};",

                    //volume
                    U"colher_de_chá{us}~4.92892ml;",
                    U"colher_de_sopa{us}~14.7868ml;",
                    U"onça_líquida{us}~29.5735ml;",
                    U"xícara{us}~236.588ml;",
                    U"pinta{us}~16onça_líquida{us};",
                    U"quarto{us}~2pinta{us};",
                    U"galão{us}~4quarto{us};",
                    U"pinta_seca{us}~550.6ml;",
                    U"quarto_seco{us}~1.101l;",
                    U"galão_seco{us}~4.4048l;",
                    U"bushel{us}~35.2391l;",
                    U"peck{us}~8.8098l;",

                    //abbreviations
                    U"in{us}~polegada{us};",
                    U"ft{us}~pé{us};",
                    U"yd{us}~jarda{us};",
                    U"mi{us}~milha{us};",
                    U"oz{us}~onça{us};",
                    U"lb{us}~libra{us};",
                    U"st{us}~stone{us};",
                    U"cwt{us}~centena_curta{us};",
                    U"ton{us}~tonelada_curta{us};",
                    U"tsp{us}~colher_de_chá{us};",
                    U"tbsp{us}~colher_de_sopa{us};",
                    U"fl_oz{us}~onça_líquida{us};",
                    U"cup{us}~xícara{us};",
                    U"pt{us}~pinta{us};",
                    U"qt{us}~quarto{us};",
                    U"gal{us}~galão{us};",
                    U"dry_pt{us}~pinta_seca{us};",
                    U"dry_quart{us}~quarto_seco{us};",
                    U"dry_gal{us}~galão_seco{us};",
                    U"pk{us}~peck{us};",
                    U"bu{us}~bushel{us};"
                }
            }
        };

    std::map<Language, std::vector<std::u32string>> phisical_constants = 
        {
            {
                Language::English,
                {
                    U"c`speed of light`=299792458(m/s);",
                    U"G`gravitational constant`=6.674301515151515*pow(10,-11)*(pow(m,3)/(kg*pow(s,2)));",
                    U"h`Planck constant`=6.62607015*pow(10,-34)*(J*s);",
                    U"e_c`electron charge`=-1.602176634*pow(10,-19)*C;",
                    U"e_m`mass of an electron`=9.1093837015*pow(10,-31)*kg;",
                    U"p_c`proton charge`=1.602176634*pow(10,-19)*C;",
                    U"p_m`mass of an proton`=1.67262192595*pow(10,-27)*kg;",
                    U"n_c`neutron charge`=0.*C;",
                    U"n_m`mass of an neutron`=1.67492749804*pow(10,-27)*kg;",
                    U"k`Boltzmann constant`=1.380649*pow(10,-23)*(J/K);",
                    U"g_a`Gravitational acceleration`=9.80665(m/pow(s,2));",
                    U"μ{0}`vacuum permeability`=4*pi*pow(10,-7)*(H/m);",
                    U"ε{0}`vacuum permittivity`=1/(μ{0}*pow(c,2));"
                }
            },
            {
                Language::Russian,
                {
                    U"ск_св`скорость света`=299792458(м/с);",
                    U"G`гравитационная постоянная`=6.674301515151515*pow(10,-11)*(pow(м,3)/(кг*pow(с,2)));",
                    U"h`постоянная Планка`=6.62607015*pow(10,-34)*(Дж*с);",
                    U"e_c`заряд электрона`=-1.602176634*pow(10,-19)*Кл;",
                    U"e_m`масса электрона`=9.1093837015*pow(10,-31)*кг;",
                    U"p_c`заряд протона`=1.602176634*pow(10,-19)*Кл;",
                    U"p_m`масса протона`=1.67262192595*pow(10,-27)*кг;",
                    U"n_c`заряд нейтрона`=0.*Кл;",
                    U"n_m`масса нейтрона`=1.67492749804*pow(10,-27)*кг;",
                    U"k`постоянная Больцмана`=1.380649*pow(10,-23)*(Дж/К);",
                    U"g_a`ускорение свободного падения`=9.80665(м/pow(с,2));",
                    U"μ{0}`магнитная постоянная`=4*pi*pow(10,-7)*(Гн/м);",
                    U"ε{0}`электрическая постоянная`=1/(μ{0}*pow(ск_св,2));"
                }
            },
            {
                Language::Spanish,
                {
                    U"v_l`velocidad de la luz`=299792458(m/s);",
                    U"G`constante gravitacional`=6.674301515151515*pow(10,-11)*(pow(m,3)/(kg*pow(s,2)));",
                    U"h`constante de Planck`=6.62607015*pow(10,-34)*(J*s);",
                    U"e_c`carga del electrón`=-1.602176634*pow(10,-19)*C;",
                    U"e_m`masa del electrón`=9.1093837015*pow(10,-31)*kg;",
                    U"p_c`carga del protón`=1.602176634*pow(10,-19)*C;",
                    U"p_m`masa del protón`=1.67262192595*pow(10,-27)*kg;",
                    U"n_c`carga del neutron`=0.*C;",
                    U"n_m`masa del neutrón`=1.67492749804*pow(10,-27)*kg;",
                    U"k`constante de Boltzmann`=1.380649*pow(10,-23)*(J/K);",
                    U"a_g`aceleración de la gravedad`=9.80665(m/pow(s,2));",
                    U"μ{0}`constante magnética`=4*pi*pow(10,-7)*(H/m);",
                    U"ε{0}`constante elétrica`=1/(μ{0}*pow(v_l,2));"
                }
            },
            {
                Language::BrazilianPortuguese,
                {
                    U"v_l`velocidade da luz`=299792458(m/s);",
                    U"G`constante gravitacional`=6.674301515151515*pow(10,-11)*(pow(m,3)/(kg*pow(s,2)));",
                    U"h`constante de Planck`=6.62607015*pow(10,-34)*(J*s);",
                    U"e_c`carga do elétron`=-1.602176634*pow(10,-19)*C;",
                    U"e_m`massa do elétron`=9.1093837015*pow(10,-31)*kg;",
                    U"p_c`carga do próton`=1.602176634*pow(10,-19)*C;",
                    U"p_m`massa do próton`=1.67262192595*pow(10,-27)*kg;",
                    U"n_c`carga do nêutron`=0.*C;",
                    U"n_m`massa do nêutron`=1.67492749804*pow(10,-27)*kg;",
                    U"k`constante de Boltzmann`=1.380649*pow(10,-23)*(J/K);",
                    U"a_g`aceleração da gravidade`=9.80665(m/pow(s,2));",
                    U"μ{0}`permeabilidade do vácuo`=4*pi*pow(10,-7)*(H/m);",
                    U"ε{0}`permissividade do vácuo`=1/(μ{0}*pow(v_l,2));"
                }
            }
        };
};

};

#endif
