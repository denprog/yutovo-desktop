/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include "editor_utils.h"
#include <yutovo-calculator/unit.h>
#include <yutovo-calculator/math_helper.h>
#include <yutovo-logger/logger.h>

namespace yutovo
{

using namespace yutovo_solver;

struct Config
{
    void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    void ToJson(std::string& json);
    void FromJson(const rapidjson::Document& value, rapidjson::Document::AllocatorType& alloc);
    bool FromJson(const std::string& json);

    //solver
    std::string service_ip = "127.0.0.1";
    uint service_port = 8010;
    uint service_timeout = 10000; //milliseconds
    uint reconnect_timeout = 10; //seconds
    uint solve_delay = 2000; //milliseconds

    //document
    bool with_border = true;
    bool code_block_border = true;
    bool pretty_json = true; //for debug purposes
    bool use_tabs = true;
    uint tab_spaces = 4;
    bool draw_whole = false;
    bool auto_prompt = true; //show prompt window when typing
    float scale = 1.0;
    bool pdf = false; //this document will draw a pdf

    //locale
    yutovo_calculator::Language language = yutovo_calculator::Language::English;

    //caret
    bool caret_visible = true;
    bool hilight_caret_element = true; //show rectangle when caret is inside
    uint caret_blink_delay = 500; //milliseconds

    //undo
    uint undo_size = 100; //maximum count of undo groups

    //colors
    Color code_block_border_color = Color::Blue();
    Color numbers_color = Color::Blue();
    Color variables_color = Color::FromHex("#00193e");
    Color functions_color = Color::FromHex("#ff5500");
    Color units_color = Color::FromHex("#005500");
    Color shapes_color = Color::Black();
    Color error_marks_color = Color::Red();
    Color formula_bg_color = Color::White();
    Color formula_frame_color = Color::FromHex("#b8d3ff");
    Color bg_selection_color = Color::Blue();
    Color link_color = Color::Blue();
    Color link_visited_color = Color::FromHex("#ff69b4");
    Color hilight_color = Color::FromHex("#97deff");
    Color page_color = Color::White();
    Color page_border_color = Color::Blue();

    //numbers gaps
    bool use_numbers_gaps = true;
    uint binary_gap = 4;
    uint octal_gap = 3;
    uint decimal_gap = 3;
    uint hexadecimal_gap = 4;

    //graphs
    Size init_size{400, 400};
    uint init_graph_width = 1;
    uint resize_margin_width = 5;

    //logger
    LogLevel log_level = LogLevel::LEVEL_INFO;
    std::string logs_path = ".";
    bool log_console = true;
    bool log_file = true;

    //calculator
    struct RealResultConfig
    {
        bool operator==(const RealResultConfig& other) const
        {
            return precision == other.precision && exp == other.exp && default_angle_measure == other.default_angle_measure &&
                result_angle_measure == other.result_angle_measure && show_angle_measure == other.show_angle_measure;
        }

        void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
        void FromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

        std::string ToString();

        uint precision = 3;
        uint exp = 10;
        AngleMeasure default_angle_measure = AngleMeasure::Radian;
        AngleMeasure result_angle_measure = AngleMeasure::Radian;
        bool show_angle_measure = true;
        yutovo_calculator::Unit unit;
    };

    RealResultConfig real_result;

    struct IntegerResultConfig
    {
        bool operator==(const IntegerResultConfig& other) const
        {
            return default_notation == other.default_notation && result_notation == other.result_notation && show_notation == other.show_notation;
        }

        void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
        void FromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

        std::string ToString();

        Notation result_notation = Notation::Decimal;
        Notation default_notation = Notation::Decimal;
        bool show_notation = true;
    };

    IntegerResultConfig integer_result;

    struct RationalResultConfig
    {
        bool operator==(const RationalResultConfig& other) const
        {
            return fraction_form == other.fraction_form;
        }

        void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
        void FromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

        std::string ToString();

        FractionForm fraction_form = FractionForm::Proper;
        yutovo_calculator::Unit unit;
    };

    RationalResultConfig rational_result;

    struct ComplexResultConfig
    {
        bool operator==(const ComplexResultConfig& other) const
        {
            return precision == other.precision && exp == other.exp && default_angle_measure == other.default_angle_measure && 
                result_angle_measure == other.result_angle_measure && show_angle_measure == other.show_angle_measure && 
                form == other.form && max_count == other.max_count;
        }

        void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
        void FromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

        std::string ToString();

        uint precision = 3;
        uint exp = 10;
        AngleMeasure default_angle_measure = AngleMeasure::Radian;
        AngleMeasure result_angle_measure = AngleMeasure::Radian;
        bool show_angle_measure = true;
        ComplexForm form = ComplexForm::Arithmetic;
        uint max_count = 10;
    };

    ComplexResultConfig complex_result;

    struct ArrayRealResultConfig : RealResultConfig
    {
        bool operator==(const ArrayRealResultConfig& other) const
        {
            return precision == other.precision && exp == other.exp && default_angle_measure == other.default_angle_measure &&
                result_angle_measure == other.result_angle_measure && show_angle_measure == other.show_angle_measure;
        }
    };

    ArrayRealResultConfig array_real_result;

    struct AutoResultConfig
    {
        bool operator==(const AutoResultConfig& other) const
        {
            return result_auto_advance == other.result_auto_advance &&
                std::equal(std::begin(results_order), std::end(results_order), std::begin(other.results_order)) &&
                real_result == other.real_result && integer_result == other.integer_result &&
                rational_result == other.rational_result && complex_result == other.complex_result &&
                array_real_result == other.array_real_result &&
                symbolic_real_result == other.symbolic_real_result && symbolic_rational_result == other.symbolic_rational_result &&
                symbolic_complex_result == other.symbolic_complex_result;
        }

        void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
        void FromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

        std::string ToString();

        bool result_auto_advance = true;
        yutovo_solver::ResultType results_order[8] = {ResultType::REAL, ResultType::INTEGER, ResultType::RATIONAL, ResultType::COMPLEX,
            ResultType::ARRAY_REAL, ResultType::SYMBOLIC_REAL, ResultType::SYMBOLIC_RATIONAL, ResultType::SYMBOLIC_COMPLEX};

        RealResultConfig real_result;
        IntegerResultConfig integer_result;
        RationalResultConfig rational_result;
        ComplexResultConfig complex_result;
        ArrayRealResultConfig array_real_result;
        RealResultConfig symbolic_real_result;
        RationalResultConfig symbolic_rational_result;
        ComplexResultConfig symbolic_complex_result;
    };

    AutoResultConfig auto_result;

    struct GraphConfig
    {
        void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
        void FromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);
    };

    //include documents
    struct IncludeDocument
    {
        bool operator==(const IncludeDocument& other) const
        {
            return file_name == other.file_name;
        }

        void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
        bool FromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

        std::string file_name;
    };

    struct IncludeDocuments
    {
        void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
        bool FromJson(const rapidjson::Value::ConstArray& arr, rapidjson::Document::AllocatorType& alloc);

        std::vector<IncludeDocument> documents;
    };

    IncludeDocuments include_documents;
};

}

#endif
