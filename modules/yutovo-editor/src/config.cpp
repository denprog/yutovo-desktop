/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "config.h"
#include <rapidjson/prettywriter.h>
#include <rapidjson/istreamwrapper.h>
#include "document.h"

#ifdef _MSC_VER
#undef GetObject
#endif

namespace yutovo
{

//Config

void Config::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    //just locale and results for a while
    value.AddMember("language", (int)language, alloc);

    value.AddMember("use_tabs", use_tabs, alloc);
    value.AddMember("tab_spaces", tab_spaces, alloc);
    value.AddMember("scale", scale, alloc);

    rapidjson::Value real_result_config("real_result", alloc);
    real_result_config.SetObject();
    real_result.ToJson(real_result_config, alloc);
    value.AddMember("real_result", real_result_config, alloc);

    rapidjson::Value integer_result_config("integer_result", alloc);
    integer_result_config.SetObject();
    integer_result.ToJson(integer_result_config, alloc);
    value.AddMember("integer_result", integer_result_config, alloc);

    rapidjson::Value rational_result_config("rational_result", alloc);
    rational_result_config.SetObject();
    rational_result.ToJson(rational_result_config, alloc);
    value.AddMember("rational_result", rational_result_config, alloc);

    rapidjson::Value complex_result_config("complex_result", alloc);
    complex_result_config.SetObject();
    complex_result.ToJson(complex_result_config, alloc);
    value.AddMember("complex_result", complex_result_config, alloc);

    rapidjson::Value auto_result_config("auto_result", alloc);
    auto_result_config.SetObject();
    auto_result.ToJson(auto_result_config, alloc);
    value.AddMember("auto_result", auto_result_config, alloc);

    rapidjson::Value include_files_config("include_documents", alloc);
    include_files_config.SetArray();
    include_documents.ToJson(include_files_config, alloc);
    value.AddMember("include_documents", include_files_config, alloc);
}

void Config::ToJson(std::string& json)
{
    rapidjson::Document doc;
    auto& alloc = doc.GetAllocator();
    doc.SetObject();
    ToJson(doc, alloc);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    json = buffer.GetString();
}

void Config::FromJson(const rapidjson::Document& value, rapidjson::Document::AllocatorType& alloc)
{
    //not all the parameters are here for a while
    if (value.HasMember("language") && value["language"].IsInt())
    {
        int val = value["language"].GetInt();
        //a stored value outside the known languages must not reach the parsers, they throw on an unknown language
        if (val >= (int)yutovo_calculator::Language::English && val <= (int)yutovo_calculator::Language::BrazilianPortuguese)
            language = (yutovo_calculator::Language)val;
    }
    
    if (value.HasMember("use_tabs") && value["use_tabs"].IsBool())
        use_tabs = value["use_tabs"].GetBool();
    if (value.HasMember("tab_spaces") && value["tab_spaces"].IsInt())
        tab_spaces = value["tab_spaces"].GetInt();
    if (value.HasMember("scale") && value["scale"].IsFloat())
        scale = value["scale"].GetFloat();
    
    if (value.HasMember("real_result") && value["real_result"].IsObject())
    {
        auto r = value["real_result"].GetObject();
        real_result.FromJson(r, alloc);
    }

    if (value.HasMember("integer_result") && value["integer_result"].IsObject())
    {
        auto r = value["integer_result"].GetObject();
        integer_result.FromJson(r, alloc);
    }

    if (value.HasMember("rational_result") && value["rational_result"].IsObject())
    {
        auto r = value["rational_result"].GetObject();
        rational_result.FromJson(r, alloc);
    }

    if (value.HasMember("complex_result") && value["complex_result"].IsObject())
    {
        auto r = value["complex_result"].GetObject();
        complex_result.FromJson(r, alloc);
    }

    if (value.HasMember("auto_result") && value["auto_result"].IsObject())
    {
        auto r = value["auto_result"].GetObject();
        auto_result.FromJson(r, alloc);
    }
    else
    {
        auto_result.real_result = real_result;
        auto_result.integer_result = integer_result;
        auto_result.rational_result = rational_result;
        auto_result.complex_result = complex_result;
        auto_result.symbolic_real_result = real_result;
        auto_result.symbolic_rational_result = rational_result;
        auto_result.symbolic_complex_result = complex_result;
    }

    if (value.HasMember("include_documents") && value["include_documents"].IsArray())
        include_documents.FromJson(((const rapidjson::Value&)value["include_documents"]).GetArray(), alloc);
}

bool Config::FromJson(const std::string& json)
{
    rapidjson::Document doc;
    doc.Parse<0>(json.c_str());
    if (doc.HasParseError() || !doc.IsObject() || doc.ObjectEmpty())
        return false;

    FromJson(doc, doc.GetAllocator());

    if (doc.HasMember("code_block_border_color") && doc["code_block_border_color"].IsInt64())
        code_block_border_color = Color::FromInt(doc["code_block_border_color"].GetInt64());
    if (doc.HasMember("numbers_color") && doc["numbers_color"].IsInt64())
        numbers_color = Color::FromInt(doc["numbers_color"].GetInt64());
    if (doc.HasMember("variables_color") && doc["variables_color"].IsInt64())
        variables_color = Color::FromInt(doc["variables_color"].GetInt64());
    if (doc.HasMember("functions_color") && doc["functions_color"].IsInt64())
        functions_color = Color::FromInt(doc["functions_color"].GetInt64());
    if (doc.HasMember("units_color") && doc["units_color"].IsInt64())
        units_color = Color::FromInt(doc["units_color"].GetInt64());
    if (doc.HasMember("shapes_color") && doc["shapes_color"].IsInt64())
        shapes_color = Color::FromInt(doc["shapes_color"].GetInt64());
    if (doc.HasMember("error_marks_color") && doc["error_marks_color"].IsInt64())
        error_marks_color = Color::FromInt(doc["error_marks_color"].GetInt64());
    if (doc.HasMember("formula_bg_color") && doc["formula_bg_color"].IsInt64())
        formula_bg_color = Color::FromInt(doc["formula_bg_color"].GetInt64());
    if (doc.HasMember("bg_selection_color") && doc["bg_selection_color"].IsInt64())
        bg_selection_color = Color::FromInt(doc["bg_selection_color"].GetInt64());
    
    return true;
}

//Config::RealResultConfig

void Config::RealResultConfig::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    value.AddMember("precision", precision, alloc);
    value.AddMember("exp", exp, alloc);
    value.AddMember("default_angle_measure", (int)default_angle_measure, alloc);
    value.AddMember("result_angle_measure", (int)result_angle_measure, alloc);
    value.AddMember("show_angle_measure", show_angle_measure, alloc);
    if (!unit.IsEmpty())
    {
        auto str = ToBasicString(unit.ToString());
        rapidjson::Value s((boost::locale::conv::utf_to_utf<char>(str)).c_str(), alloc);
        value.AddMember("unit", s, alloc);
    }
}

void Config::RealResultConfig::FromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (value.HasMember("precision") && value["precision"].IsInt())
        precision = value["precision"].GetInt();
    if (value.HasMember("exp") && value["exp"].IsInt())
        exp = value["exp"].GetInt();
    if (value.HasMember("default_angle_measure") && value["default_angle_measure"].IsInt())
        default_angle_measure = (AngleMeasure)value["default_angle_measure"].GetInt();
    if (value.HasMember("result_angle_measure") && value["result_angle_measure"].IsInt())
        result_angle_measure = (AngleMeasure)value["result_angle_measure"].GetInt();
    if (value.HasMember("show_angle_measure") && value["show_angle_measure"].IsBool())
        show_angle_measure = value["show_angle_measure"].GetBool();
    if (value.HasMember("unit") && value["unit"].IsString())
        unit.FromString(ToUtfString(value["unit"].GetString()));
}

std::string Config::RealResultConfig::ToString()
{
    rapidjson::Document json;
    json.SetObject();
    rapidjson::Value c(rapidjson::kObjectType);
    auto& alloc = json.GetAllocator();
    ToJson(c, alloc);
    json.AddMember("RealResultConfig", c, alloc);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);
    return buffer.GetString();
}

//Config::IntegerResultConfig

void Config::IntegerResultConfig::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    value.AddMember("result_notation", (int)result_notation, alloc);
    value.AddMember("default_notation", (int)default_notation, alloc);
    value.AddMember("show_notation", show_notation, alloc);
}

void Config::IntegerResultConfig::FromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (value.HasMember("result_notation") && value["result_notation"].IsInt())
        result_notation = (Notation)value["result_notation"].GetInt();
    if (value.HasMember("default_notation") && value["default_notation"].IsInt())
        default_notation = (Notation)value["default_notation"].GetInt();
    if (value.HasMember("show_notation") && value["show_notation"].IsBool())
        show_notation = value["show_notation"].GetBool();
}

std::string Config::IntegerResultConfig::ToString()
{
    rapidjson::Document json;
    json.SetObject();
    rapidjson::Value c(rapidjson::kObjectType);
    auto& alloc = json.GetAllocator();
    ToJson(c, alloc);
    json.AddMember("IntegerResultConfig", c, alloc);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);
    return buffer.GetString();
}

//Config::RationalResultConfig

void Config::RationalResultConfig::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    value.AddMember("fraction_form", (int)fraction_form, alloc);
    if (!unit.IsEmpty())
    {
        auto str = ToBasicString(unit.ToString());
        rapidjson::Value s((boost::locale::conv::utf_to_utf<char>(str)).c_str(), alloc);
        value.AddMember("unit", s, alloc);
    }
}

void Config::RationalResultConfig::FromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (value.HasMember("fraction_form") && value["fraction_form"].IsInt())
        fraction_form = (FractionForm)value["fraction_form"].GetInt();
    if (value.HasMember("unit") && value["unit"].IsString())
        unit.FromString(ToUtfString(value["unit"].GetString()));
}

std::string Config::RationalResultConfig::ToString()
{
    rapidjson::Document json;
    json.SetObject();
    rapidjson::Value c(rapidjson::kObjectType);
    auto& alloc = json.GetAllocator();
    ToJson(c, alloc);
    json.AddMember("RationalResultConfig", c, alloc);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);
    return buffer.GetString();
}

//Config::ComplexResultConfig

void Config::ComplexResultConfig::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    value.AddMember("precision", precision, alloc);
    value.AddMember("exp", exp, alloc);
    value.AddMember("default_angle_measure", (int)default_angle_measure, alloc);
    value.AddMember("result_angle_measure", (int)result_angle_measure, alloc);
    value.AddMember("show_angle_measure", show_angle_measure, alloc);
    value.AddMember("form", (int)form, alloc);
    value.AddMember("max_count", max_count, alloc);
}

void Config::ComplexResultConfig::FromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (value.HasMember("precision") && value["precision"].IsInt())
        precision = value["precision"].GetInt();
    if (value.HasMember("exp") && value["exp"].IsInt())
        exp = value["exp"].GetInt();
    if (value.HasMember("default_angle_measure") && value["default_angle_measure"].IsInt())
        default_angle_measure = (AngleMeasure)value["default_angle_measure"].GetInt();
    if (value.HasMember("result_angle_measure") && value["result_angle_measure"].IsInt())
        result_angle_measure = (AngleMeasure)value["result_angle_measure"].GetInt();
    if (value.HasMember("show_angle_measure") && value["show_angle_measure"].IsBool())
        show_angle_measure = value["show_angle_measure"].GetBool();
    if (value.HasMember("form") && value["form"].IsInt())
        form = (ComplexForm)value["form"].GetInt();
    if (value.HasMember("max_count") && value["max_count"].IsInt())
        max_count = value["max_count"].GetInt();
}

std::string Config::ComplexResultConfig::ToString()
{
    rapidjson::Document json;
    json.SetObject();
    rapidjson::Value c(rapidjson::kObjectType);
    auto& alloc = json.GetAllocator();
    ToJson(c, alloc);
    json.AddMember("ComplexResultConfig", c, alloc);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);
    return buffer.GetString();
}

//Config::AutoResultConfig

void Config::AutoResultConfig::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    value.AddMember("result_auto_advance", result_auto_advance, alloc);

    rapidjson::Value arr(rapidjson::kArrayType);
    for (size_t i = 0; i < std::size(results_order); ++i)
        arr.PushBack((int)results_order[i], alloc);
    value.AddMember("results_order", arr, alloc);

    rapidjson::Value real_config(rapidjson::kObjectType);
    real_result.ToJson(real_config, alloc);
    value.AddMember("real_config", real_config, alloc);

    rapidjson::Value integer_config(rapidjson::kObjectType);
    integer_result.ToJson(integer_config, alloc);
    value.AddMember("integer_config", integer_config, alloc);

    rapidjson::Value rational_config(rapidjson::kObjectType);
    rational_result.ToJson(rational_config, alloc);
    value.AddMember("rational_config", rational_config, alloc);

    rapidjson::Value complex_config(rapidjson::kObjectType);
    complex_result.ToJson(complex_config, alloc);
    value.AddMember("complex_config", complex_config, alloc);

    rapidjson::Value array_real_config(rapidjson::kObjectType);
    array_real_result.ToJson(array_real_config, alloc);
    value.AddMember("array_real_config", array_real_config, alloc);

    rapidjson::Value symbolic_real_config(rapidjson::kObjectType);
    symbolic_real_result.ToJson(symbolic_real_config, alloc);
    value.AddMember("symbolic_real_config", symbolic_real_config, alloc);

    rapidjson::Value symbolic_rational_config(rapidjson::kObjectType);
    symbolic_rational_result.ToJson(symbolic_rational_config, alloc);
    value.AddMember("symbolic_rational_config", symbolic_rational_config, alloc);

    rapidjson::Value symbolic_complex_config(rapidjson::kObjectType);
    symbolic_complex_result.ToJson(symbolic_complex_config, alloc);
    value.AddMember("symbolic_complex_config", symbolic_complex_config, alloc);
}

void Config::AutoResultConfig::FromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (value.HasMember("result_auto_advance") && value["result_auto_advance"].IsBool())
        result_auto_advance = value["result_auto_advance"].GetBool();
    if (value.HasMember("results_order") && value["results_order"].IsArray())
    {
        const auto& arr = value["results_order"].GetArray();
        //fill unique result types
        rapidjson::SizeType i = 0;
        for (; i < arr.Size() && i < std::size(results_order); ++i)
        {
            if (!arr[i].IsInt())
                return;
            yutovo_solver::ResultType r = (yutovo_solver::ResultType)arr[i].GetInt();
            if (std::find(std::begin(results_order), std::end(results_order), r) == std::end(results_order))
                results_order[i] = (yutovo_solver::ResultType)arr[i].GetInt();
        }
        for (; i < std::size(results_order); ++i)
        {
            if (std::find(std::begin(results_order), std::end(results_order), ResultType::REAL) == std::end(results_order))
                results_order[i] = ResultType::REAL;
            if (std::find(std::begin(results_order), std::end(results_order), ResultType::INTEGER) == std::end(results_order))
                results_order[i] = ResultType::INTEGER;
            if (std::find(std::begin(results_order), std::end(results_order), ResultType::RATIONAL) == std::end(results_order))
                results_order[i] = ResultType::RATIONAL;
            if (std::find(std::begin(results_order), std::end(results_order), ResultType::COMPLEX) == std::end(results_order))
                results_order[i] = ResultType::COMPLEX;
            if (std::find(std::begin(results_order), std::end(results_order), ResultType::ARRAY_REAL) == std::end(results_order))
                results_order[i] = ResultType::ARRAY_REAL;
            if (std::find(std::begin(results_order), std::end(results_order), ResultType::SYMBOLIC_REAL) == std::end(results_order))
                results_order[i] = ResultType::SYMBOLIC_REAL;
            if (std::find(std::begin(results_order), std::end(results_order), ResultType::SYMBOLIC_RATIONAL) == std::end(results_order))
                results_order[i] = ResultType::SYMBOLIC_RATIONAL;
            if (std::find(std::begin(results_order), std::end(results_order), ResultType::SYMBOLIC_COMPLEX) == std::end(results_order))
                results_order[i] = ResultType::SYMBOLIC_COMPLEX;
        }
    }

    if (value.HasMember("real_config") && value["real_config"].IsObject())
    {
        auto obj = value["real_config"].GetObject();
        real_result.FromJson(obj, alloc);
    }
    if (value.HasMember("integer_config") && value["integer_config"].IsObject())
    {
        auto obj = value["integer_config"].GetObject();
        integer_result.FromJson(obj, alloc);
    }
    if (value.HasMember("rational_config") && value["rational_config"].IsObject())
    {
        auto obj = value["rational_config"].GetObject();
        rational_result.FromJson(obj, alloc);
    }
    if (value.HasMember("complex_config") && value["complex_config"].IsObject())
    {
        auto obj = value["complex_config"].GetObject();
        complex_result.FromJson(obj, alloc);
    }
    if (value.HasMember("array_real_config") && value["array_real_config"].IsObject())
    {
        auto obj = value["array_real_config"].GetObject();
        array_real_result.FromJson(obj, alloc);
    }
    if (value.HasMember("symbolic_real_config") && value["symbolic_real_config"].IsObject())
    {
        auto obj = value["symbolic_real_config"].GetObject();
        symbolic_real_result.FromJson(obj, alloc);
    }
    if (value.HasMember("symbolic_rational_config") && value["symbolic_rational_config"].IsObject())
    {
        auto obj = value["symbolic_rational_config"].GetObject();
        symbolic_rational_result.FromJson(obj, alloc);
    }
    if (value.HasMember("symbolic_complex_config") && value["symbolic_complex_config"].IsObject())
    {
        auto obj = value["symbolic_complex_config"].GetObject();
        symbolic_complex_result.FromJson(obj, alloc);
    }
    if (value.HasMember("symbolic_config") && value["symbolic_config"].IsObject())
    {
        auto obj = value["symbolic_config"].GetObject();
        symbolic_real_result.FromJson(obj, alloc);
    }
}

std::string Config::AutoResultConfig::ToString()
{
    rapidjson::Document json;
    json.SetObject();
    rapidjson::Value c(rapidjson::kObjectType);
    auto& alloc = json.GetAllocator();
    ToJson(c, alloc);
    json.AddMember("AutoResultConfig", c, alloc);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);
    return buffer.GetString();
}

//Config::GraphConfig

void Config::GraphConfig::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
}

void Config::GraphConfig::FromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
}

//Config::IncludeDocument

void Config::IncludeDocument::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    rapidjson::Value obj(rapidjson::kObjectType);
    rapidjson::Value _file_name(file_name.c_str(), alloc);
    obj.AddMember("file_name", _file_name, alloc);
    value.PushBack(obj, alloc);
}

bool Config::IncludeDocument::FromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.HasMember("file_name") || !value["file_name"].IsString())
        return false;
    file_name = value["file_name"].GetString();
    return true;
}

//Config::IncludeDocuments

void Config::IncludeDocuments::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    for (auto& f : documents)
        f.ToJson(value, alloc);
}

bool Config::IncludeDocuments::FromJson(const rapidjson::Value::ConstArray& arr, rapidjson::Document::AllocatorType& alloc)
{
    documents.clear();
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
    {
        if (!arr[i].IsObject())
            return false;
        rapidjson::Value::ConstObject value = arr[i].GetObject();
        IncludeDocument s;
        if (!s.FromJson(value, alloc))
            return false;
        auto it = std::find_if(documents.begin(), documents.end(), 
            [s](auto& f)
            {
                return f.file_name == s.file_name;
            });
        if (it == documents.end())
            documents.push_back(s);
    }
    return true;
}

}
