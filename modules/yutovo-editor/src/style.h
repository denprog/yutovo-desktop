/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __STYLE_H__
#define __STYLE_H__

#include <string>
#include <memory>
#include <vector>
#include <boost/uuid/uuid.hpp>
#include "editor_utils.h"

namespace yutovo
{

typedef std::shared_ptr<Element> ElementPtr;

struct StringFormat
{
    StringFormat() = default;
    StringFormat(const boost::uuids::uuid _id, const std::string _family, uint _size, bool _bold, bool _italic, bool _underline, bool _strikethrough, 
        bool _subscript, bool _superscript, Color _text_color, Color _text_bg_color, Color _text_bg_selection_color);
    StringFormat(const std::string _family, uint _size, bool _bold, bool _italic, bool _underline, bool _strikethrough, bool _subscript, bool _superscript, 
        Color _text_color, Color _text_bg_color, Color _text_bg_selection_color);
 
    bool operator==(const StringFormat& f) const;
    bool operator!=(const StringFormat& f) const;

    void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    bool FromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    std::string ToString();

    void Reset();

    boost::uuids::uuid id; //for serialization

    std::string family;
    uint size = 0;
    bool bold = false;
    bool italic = false;
    bool underline = false;
    bool strikethrough = false;
    bool subscript = false;
    bool superscript = false;
    Color text_color;
    Color text_bg_color;
    Color text_bg_selection_color;
};

typedef std::shared_ptr<StringFormat> StringFormatPtr;

//Collection of string formats
class StringFormats
{
public:
    StringFormatPtr GetFormat(const std::string _family, uint _size, bool _bold, bool _italic, bool _underline, bool _strikethrough, bool _subscript, 
        bool _superscript, Color _text_color, Color _text_bg_color, Color _text_bg_selection_color);
    StringFormatPtr GetFormat(const StringFormat& source);
    StringFormatPtr GetFormat(const StringFormatPtr& source, const float scale);
    StringFormatPtr GetFormat(const boost::uuids::uuid& _id);

    void AddFormats(const StringFormats& source);

    void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc, const std::vector<ElementPtr>& elements);
    bool FromJson(const rapidjson::Value::ConstArray& value, rapidjson::Document::AllocatorType& alloc);

    void Clear()
    {
        string_formats.clear();
    }

private:
    std::vector<StringFormatPtr> string_formats;
};

typedef std::shared_ptr<StringFormats> StringFormatsPtr;

struct ParagraphFormat
{
    enum class Alignment
    {
        Left = 0,
        Right,
        Center,
        Justify
    };

    enum class WordWrap
    {
        None = 0,
        Normal
    };

    ParagraphFormat() = default;
    ParagraphFormat(std::string _name, Alignment _alignment, WordWrap _word_wrap, uint _line_spacing, uint _indent_before, uint _indent_after, 
        uint _indent_first_line, uint _spacing_before, uint _spacing_after, StringFormatPtr _string_format);
    
    bool operator==(const ParagraphFormat& f) const;

    void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    bool FromJson(Document* document, const rapidjson::Value::ConstObject& arr, rapidjson::Document::AllocatorType& alloc);

    std::string ToString();

    std::string name;
    Alignment alignment = Alignment::Left;
    WordWrap word_wrap = WordWrap::Normal;
    uint line_spacing = 0;
    uint indent_before = 0;
    uint indent_after = 0;
    uint indent_first_line = 0;
    uint spacing_before = 0;
    uint spacing_after = 0;

    StringFormatPtr default_string_format;
};

typedef std::shared_ptr<ParagraphFormat> ParagraphFormatPtr;

//Collection of paragraph formats
class ParagraphFormats
{
public:
    ParagraphFormats(StringFormatsPtr _string_formats, const yutovo_calculator::Language language);

    ParagraphFormatPtr GetFormat(std::string _name, ParagraphFormat::Alignment _alignment, ParagraphFormat::WordWrap _word_wrap, uint _line_spacing, 
        uint _indent_before, uint _indent_after, uint _indent_first_line, uint _spacing_before, uint _spacing_after, StringFormatPtr _string_format, 
        const yutovo_calculator::Language language);
    ParagraphFormatPtr GetFormat(const std::string& name, const yutovo_calculator::Language language);
    void GetFormats(std::vector<ParagraphFormatPtr>& formats);

    void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    bool FromJson(Document* document, const rapidjson::Value::ConstArray& arr, rapidjson::Document::AllocatorType& alloc);

private:
    static std::string TranslateName(const std::string& name, const yutovo_calculator::Language language);

private:
    StringFormatsPtr string_formats;
    std::vector<ParagraphFormatPtr> paragraph_formats;
};

typedef std::unique_ptr<ParagraphFormats> ParagraphFormatsPtr;

struct FormulaFormat
{
    FormulaFormat() = default;
    FormulaFormat(const std::string& _name, StringFormatPtr _string_format, uint _inter_spacing, 
        int _left_margin, int _top_margin, int _right_margin, int _bottom_margin, Color _color, Color _bg_color, Color _bg_selection_color);

    bool operator==(const FormulaFormat& f) const;

    std::string name;
    StringFormatPtr string_format;

    int inter_spacing = 0;

    int left_margin = 0;
    int top_margin = 0;
    int right_margin = 0;
    int bottom_margin = 0;

    Color color;
    Color bg_color;
    Color bg_selection_color;
};

typedef std::shared_ptr<FormulaFormat> FormulaFormatPtr;

class FormulaFormats
{
public:
    FormulaFormats(StringFormatsPtr _string_formats);

    FormulaFormatPtr GetFormat(const std::string& name);
    FormulaFormatPtr GetFormat(const std::string& name, StringFormatPtr string_format, uint inter_spacing, 
        int left_margin, int top_margin, int right_margin, int bottom_margin, Color color, Color bg_color, Color bg_selection_color);

private:
    StringFormatsPtr string_formats;
    std::vector<FormulaFormatPtr> formula_formats;
};

typedef std::unique_ptr<FormulaFormats> FormulaFormatsPtr;

struct GraphFormat
{
    void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    bool FromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    Size size{400, 400};
    Color color = Color::Black();
    uint grid_width = 1;
};

struct PlotFormat
{
    void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    bool FromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);
    
    Color color = Color::Red();
    uint width = 1;
};

typedef std::unique_ptr<GraphFormat> GraphFormatPtr;

struct CodeFormat
{
    CodeFormat() = default;
    CodeFormat(const std::string& _name, uint _left_indent, uint _top_indent, uint _right_indent, uint _bottom_indent, 
        uint left_margin, uint top_margin, uint right_margin, uint bottom_margin, uint _paragraph_spacing, Color _border_color);

    bool operator==(const CodeFormat& c) const;

    std::string name;

    uint left_indent;
    uint top_indent;
    uint right_indent;
    uint bottom_indent;

    uint left_margin = 0;
    uint top_margin = 0;
    uint right_margin = 0;
    uint bottom_margin = 0;

    uint paragraph_spacing;

    Color border_color;
};

typedef std::shared_ptr<CodeFormat> CodeFormatPtr;

class CodeFormats
{
public:
    CodeFormats();

    CodeFormatPtr GetFormat(const std::string& name);
    CodeFormatPtr GetFormat(const std::string& name, uint left_indent, uint top_indent, uint right_indent, uint bottom_indent, 
        uint left_margin, uint top_margin, uint right_margin, uint bottom_margin, uint paragraph_spacing, Color border_color);

private:
    std::vector<CodeFormatPtr> code_formats;
};

typedef std::unique_ptr<CodeFormats> CodeFormatsPtr;

struct TextFormat
{
    enum class Paging
    {
        WEB_VIEW = 0 //one infinite page
    };

    bool operator==(const TextFormat& t) const;

    void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    bool FromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    Paging paging = Paging::WEB_VIEW;
    uint left_indent = 20;
    uint top_indent = 20;
    uint right_indent = 20;
    uint bottom_indent = 20;
    uint paragraph_spacing = 10;
    Size size{210, 297}; //in mm, when TextFormat::Paging::PAGINATION, default is A4
};

typedef std::shared_ptr<TextFormat> TextFormatPtr;

class TextFormats
{
public:
    static TextFormatPtr GetFormat(TextFormat::Paging paging, uint left_indent, uint top_indent, uint right_indent, uint bottom_indent, uint paragraph_spacing, 
        Size size);

private:
    static std::vector<TextFormatPtr> text_formats;
};

}

#endif
