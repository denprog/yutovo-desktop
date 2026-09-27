/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "code_string.h"
#include "code_block.h"
#include "result.h"
#include <boost/lexical_cast.hpp>
#ifdef min
#undef min
#endif
#include <boost/uuid/uuid_io.hpp>

namespace yutovo
{

//CodeString

CodeString::CodeString(Element* parent) :
    String(parent)
{
    type = ElementType::CODE_STRING;
    UpdateGap();
}

CodeString::CodeString(Element* parent, const std::string str, bool translate) :
    String(parent, str, translate)
{
    type = ElementType::CODE_STRING;
    UpdateGap();
}

CodeString::CodeString(Element* parent, const std::string str, const StringFormatPtr _format, bool translate) :
    String(parent, str, _format, translate)
{
    type = ElementType::CODE_STRING;
    UpdateGap();
}

CodeString::CodeString(Element* parent, const std::u32string str) :
    String(parent, str)
{
    type = ElementType::CODE_STRING;
    UpdateGap();
}

CodeString::CodeString(Element* parent, const std::u32string str, const StringFormatPtr _format) :
    String(parent, str, _format)
{
    type = ElementType::CODE_STRING;
    UpdateGap();
}

CodeString::CodeString(Document* _document) :
    String(_document)
{
    type = ElementType::CODE_STRING;
    UpdateGap();
}

CodeString::CodeString(Document* _document, const std::string str, const StringFormatPtr _format) :
    String(_document, str, _format)
{
    type = ElementType::CODE_STRING;
    UpdateGap();
}

CodeString::CodeString(Document* _document, const std::u32string str, const StringFormatPtr _format) :
    String(_document, str, _format)
{
    type = ElementType::CODE_STRING;
    UpdateGap();
}

CodeString::CodeString(const String& source) :
    String(source)
{
    type = ElementType::CODE_STRING;
    UpdateGap();
}

Element* CodeString::Clone()
{
    return new CodeString(*this);
}

Element* CodeString::Create(Element* parent)
{
    return new CodeString(parent);
}

Element* CodeString::Create(Element* parent, const std::u32string _str, const StringFormatPtr _format)
{
    return new CodeString(parent, _str, _format);
}

void CodeString::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    String::ToJson(value, alloc);
    value.AddMember("can_merge", can_merge, alloc);
}

Element* CodeString::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    CodeString* r = nullptr;
    bool _can_merge = true;
    if (value.HasMember("can_merge") && value["can_merge"].IsBool())
        _can_merge = value["can_merge"].GetBool();
    if (value.HasMember("format_id") && value["format_id"].IsString())
    {
        auto format_id_str = value["format_id"].GetString();
        boost::uuids::uuid format_id;
        try
        {
            format_id = boost::lexical_cast<boost::uuids::uuid>(format_id_str);
        }
        catch (std::bad_cast& ex)
        {
            return nullptr;
        }

        auto f = document->GetStringFormat(format_id);
        if (f)
        {
            if (parent)
                r = new CodeString(parent, U"", f);
            else
                r = new CodeString(document, "", f);
            r->can_merge = _can_merge;
            return r;
        }
    }

    StringFormatPtr f;
    if (parent)
    {
        f = parent->GetStringFormat();
        if (f)
            r = new CodeString(parent, U"", f);
    }

    if (!r && document->GetCurrentStringFormat(f))
    {
        if (parent)
            r = new CodeString(parent, U"", f);
        else
            r = new CodeString(document, U"", f);
    }
    if (r)
        r->can_merge = _can_merge;
    return r;
}

bool CodeString::Remake(bool with_elements)
{
    UpdateGap();
    return String::Remake(with_elements);
}

bool CodeString::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    gap = 0;
    UpdateGap();

    ElementId _changed_element;
    for (auto& el : _elements)
    {
        if (document->IsString(el))
        {
            String* s = dynamic_cast<String*>(el.get());
            auto _s = s->ToText();
            if (_s == U" " && ToText() == U"")
                return false;
            if (_s.find(U" ") != std::string::npos)
            {
                if (with_undo)
                    document->StoreUndo(parent->id);
                _changed_element = parent->id;
                break;
            }
        }
    }
    bool r = String::InsertElements(_elements, insert_mode, with_undo, changed_element);
    if (!_changed_element.empty() && _changed_element.size() < changed_element.size())
        changed_element = _changed_element;
    return r;
}

bool CodeString::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    gap = 0;
    UpdateGap();
    return String::DeleteElements(left, with_undo, changed_element);
}

bool CodeString::ChangeStringFormat(const StringFormatPtr format, bool with_undo, ElementId& changed_element)
{
    gap = 0;
    UpdateGap();
    return String::ChangeStringFormat(format, with_undo, changed_element);
}

void CodeString::Draw() const
{
    std::u32string str = elements->ToText();
    Rect r = GetAbsoluteRect();

    std::pair<int, Color> color1{-1, draw_format->text_color}, color2{-1, Color::Black()};
    if (GetNotation() != Notation::None)
        color1 = std::make_pair(str.length(), document->config.numbers_color);
    else
    {
        auto el = document->FindParent(id, ElementType::CODE_BLOCK);
        assert(el);
        CodeBlock* c = dynamic_cast<CodeBlock*>(el.get());
        uint code_id = c->code_id;

        size_t p = str.find_first_not_of(U"0123456789.");
        if (p == 0)
        {
            auto find_identifier = 
                [&](const std::u32string& s)
                {
                    switch (document->FindIdentifier(code_id, ToBasicString(s)))
                    {
                    case IdentifierType::VARIABLE:
                        color1 = std::make_pair(s.length(), document->config.variables_color);
                        break;
                    case IdentifierType::FUNCTION:
                        color1 = std::make_pair(s.length(), document->config.functions_color);
                        break;
                    case IdentifierType::UNIT:
                        color1 = std::make_pair(s.length(), document->config.units_color);
                        break;
                    default:
                        return false;
                    }
                    return true;
                };

            //may be it is an identifier
            auto s = document->FindParent(id, ElementType::SUBSCRIPT);
            if (!s || !find_identifier(str + s->elements->Get(2)->ToText()))
                find_identifier(str);
        }
        else
        {
            //the first part is a number, after it may be an identifier
            if (p == std::string::npos)
                p = str.length();
            color1 = std::make_pair(p, document->config.numbers_color);
            std::u32string part = str.substr(p);
            switch (document->FindIdentifier(code_id, ToBasicString(part)))
            {
            case IdentifierType::VARIABLE:
                color2 = std::make_pair(part.length(), document->config.variables_color);
                break;
            case IdentifierType::UNIT:
                color2 = std::make_pair(part.length(), document->config.units_color);
                break;
            default:
                color2 = std::make_pair(part.length(), draw_format->text_color);
                break;
            }
        }
    }

    uint start = 0, size = 0;
    if (selection->Has(id, start, size))
    {
        Size s1 = GetTextSize(start);
        Size s2 = GetTextSize(start + size);
        window->DrawFillRect(Rect{r.left + s1.width, r.top, s2.width - s1.width, r.height}, document->config.bg_selection_color);
    }

    StringFormatPtr fmt = draw_format;
    bool has_underline = fmt->underline;
    if (has_underline)
    {
        StringFormat f = *fmt;
        f.underline = false;
        fmt = document->string_formats->GetFormat(f);
    }
    int p = 0;
    for (int i = 0; i < str.length(); ++i)
    {
        if (str[i] == U'\t')
            continue;

        yutovo::Size s(GetTextSize(i + 1));
        auto _ch = str.substr(i, 1);
        int w = document->GetCharWidth(draw_format, _ch[0]);
        if (gap > 0 && s.width - p > w)
            w = s.width - p;
        auto ch = ToBasicString(_ch);
        if (color1.first != -1 && i < color1.first)
        {
            if (size != 0 && i >= start && i < start + size)
                window->DrawText(ch, fmt, Rect{r.left + s.width - w, r.top, w, r.height}, document->config.formula_bg_color, 
                    document->config.bg_selection_color, true);
            else
                window->DrawText(ch, fmt, Rect{r.left + s.width - w, r.top, w, r.height}, color1.second, 
                    document->config.formula_bg_color, true);
        }
        else if (color1.first != -1 && color2.first != -1 && i >= color1.first && i < color1.first + color2.first)
        {
            if (size != 0 && i >= start && i < start + size)
                window->DrawText(ch, fmt, Rect{r.left + s.width - w, r.top, w, r.height}, document->config.formula_bg_color, 
                    document->config.bg_selection_color, true);
            else
                window->DrawText(ch, fmt, Rect{r.left + s.width - w, r.top, w, r.height}, color2.second, 
                    document->config.formula_bg_color, true);
        }
        else if (gap == 0)
        {
            if (size != 0 && i >= start && i < start + size)
                window->DrawText(ch, fmt, Rect{r.left + s.width - w, r.top, w, r.height}, document->config.formula_bg_color, 
                    document->config.bg_selection_color, true);
            else
                window->DrawText(ch, fmt, Rect{r.left + s.width - w, r.top, w, r.height}, draw_format->text_color, 
                    document->config.formula_bg_color, true);
        }
        else
        {
            if (size != 0 && i >= start && i < start + size)
                window->DrawText(ch, fmt, Rect{r.left + s.width - w, r.top, w, r.height}, 
                    document->config.numbers_color, document->config.formula_bg_color, true);
            else
                window->DrawText(ch, fmt, Rect{r.left + s.width - w, r.top, w, r.height}, document->config.numbers_color, 
                    document->config.formula_bg_color, true);
        }

        p = s.width;
    }

    if (has_underline)
    {
        yutovo::Size text_size = GetTextSize(str.length());
        if (text_size.width > 0)
        {
            int ascent = window->GetFontAscent(draw_format);
            window->DrawLine(r.left, r.top + ascent + 1, r.left + text_size.width, r.top + ascent + 1, draw_format->text_color);
        }
    }

    if (elements->Count() == 0)
    {
        uint start = 0, size = 0;
        if (document->selection.Has(parent->id, start, size))
        {
            auto r = GetAbsoluteRect();
            window->DrawRect(r, document->config.bg_selection_color);
            window->DrawFillRect(Rect(r.left + 1, r.top + 1, r.width - 2, r.height - 2), document->config.bg_selection_color);
        }
        else
        {
            auto row = document->FindParentRow(id);
            if (row->parent->type != ElementType::CODE_PARAGRAPH || parent->elements->Count() > 1)
                window->DrawRect(GetAbsoluteRect(), document->config.numbers_color);
        }
    }
}

void CodeString::UpdateRect(bool with_elements)
{
    String::UpdateRect(with_elements);

    if (elements->Count() == 0)
        rect.SetSize(rect.width + empty_rect_width, rect.height);
    if (draw_format->subscript)
        baseline = -window->GetFontAscent(draw_format);
    else if (draw_format->superscript)
        baseline = window->GetFontAscent(draw_format);
    else
        baseline = rect.height / 2;
}

Rect CodeString::GetCaretRect(const uint pos) const
{
    Rect r = String::GetCaretRect(pos);
    if (elements->Count() > 0)
        return r;
    if (document->insert_mode)
        r.left += rect.width / 2;
    return r;
}

Size CodeString::GetTextSize(const uint pos) const
{
    auto it = size_cache.find(pos);
    if (it == size_cache.end())
    {
        auto& str = ((StringElements*)elements.get())->str;
        if (gap == 0 || str.find(U'\t') != std::string::npos)
            return String::GetTextSize(pos);

        auto _str = str.substr(0, pos);
        Size s = window->GetTextSize(_str, draw_format);
        int _gap = gap;
        auto point_pos = str.find(U".");
        if (point_pos != std::string::npos)
        {
            if (point_pos < gap)
                _gap = gap;
            else
            {
                _gap = point_pos % gap;
                if (_gap == 0)
                    _gap = gap;
            }
        }
        else
        {
            _gap = str.length() % gap;
            if (_gap == 0)
                _gap = gap;
        }
        
        for (int i = 0; i < pos; ++i)
        {
            if (str[i] == U'.')
            {
                _gap = gap;
                continue;
            }
            if (_gap == 0)
            {
                s.width += gap_width;
                _gap = gap;
            }
            if (i < str.length() - 1 && str[i + 1] != U'.')
                --_gap;
        }

        if (_gap == 0 && pos < str.length())
            s.width += gap_width;
        size_cache[pos] = s;
        return s;
    }
    return it->second;
}

void CodeString::GetMargin(int& left, int& top, int& right, int& bottom) const
{
    const FormulaFormatPtr f = GetFormulaFormat();
    left = std::round(f->left_margin * document->config.scale);
    top = std::round(f->top_margin * document->config.scale);
    right = std::round(f->right_margin * document->config.scale);
    bottom = std::round(f->bottom_margin * document->config.scale);
}

bool CodeString::IsFormula()
{
    return true;
}

std::string CodeString::ToHtml() const
{
    std::string s = "<mi>";
    if (elements->Count() > 0)
        s += elements->ToHtml();
    s += "</mi>";
    return s;
}

void CodeString::UpdateGap()
{
    int _gap = gap;
    Config& config = document->config;
    if (!config.use_numbers_gaps)
    {
        _gap = 0;
    }
    else
    {
        auto& str = ((StringElements*)elements.get())->str;
        if (parent && parent->parent && 
            ((parent->parent->type == ElementType::SUBSCRIPT && parent->parent->elements->Count() == 3) || 
            (parent->parent->type == ElementType::INTEGER_RESULT)))
        {
            Notation notation = Notation::None;
            if (parent->parent->type == ElementType::INTEGER_RESULT)
                notation = ((IntegerResult*)parent->parent)->config.result_notation;
            else
            {
                auto last = parent->parent->elements->Get(2)->ToText();
                if (last == U"bin")
                    notation = Notation::Binary;
                else if (last == U"oct")
                    notation = Notation::Octal;
                else if (last == U"hex")
                    notation = Notation::Hexadecimal;
                else
                    notation = Notation::Decimal;
            }

            switch (notation)
            {
            case Notation::None:
                break;
            case Notation::Binary:
                if (str.find_first_not_of(U"01") == std::string::npos)
                    _gap = document->config.binary_gap;
                else
                    _gap = 0;
                break;
            case Notation::Decimal:
                if (str.find_first_not_of(U"0123456789") == std::string::npos)
                    _gap = document->config.decimal_gap;
                else
                    _gap = 0;
                break;
            case Notation::Octal:
                if (str.find_first_not_of(U"01234567") == std::string::npos)
                    _gap = document->config.octal_gap;
                else
                    _gap = 0;
                break;
            case Notation::Hexadecimal:
                if (str.find_first_not_of(U"0123456789abcdefABCDEF") == std::string::npos)
                    _gap = document->config.hexadecimal_gap;
                else
                    _gap = 0;
                break;
            }
        }
        else
        {
            if (str.find_first_not_of(U"0123456789.") == std::string::npos)
            {
                if (std::count(str.begin(), str.end(), '.') > 1)
                    _gap = 0;
                else
                    _gap = document->config.decimal_gap;
            }
            else
                _gap = 0;
        }
    }

    if (gap != _gap)
    {
        if (!draw_format)
            Rescale();
        Size s = window->GetTextSize(U" ", draw_format);
        gap_width = s.width / 2;
        gap = _gap;
        size_cache.clear();
    }
}

Notation CodeString::GetNotation() const
{
    auto& str = ((StringElements*)elements.get())->str;
    if (parent && parent->parent && 
        ((parent->parent->type == ElementType::SUBSCRIPT && parent->parent->elements->Count() == 3) || 
        (parent->parent->type == ElementType::INTEGER_RESULT)))
    {
        Notation notation = Notation::None;
        if (parent->parent->type == ElementType::INTEGER_RESULT)
            notation = ((IntegerResult*)parent->parent)->config.result_notation;
        else
        {
            auto last = parent->parent->elements->Get(2)->ToText();
            if (last == U"bin")
                notation = Notation::Binary;
            else if (last == U"oct")
                notation = Notation::Octal;
            else if (last == U"hex")
                notation = Notation::Hexadecimal;
            else
                notation = Notation::Decimal;
        }

        switch (notation)
        {
        case Notation::None:
            break;
        case Notation::Binary:
            if (str.find_first_not_of(U"01") == std::string::npos)
                return Notation::Binary;
            break;
        case Notation::Decimal:
            if (str.find_first_not_of(U"0123456789") == std::string::npos)
                return Notation::Decimal;
            break;
        case Notation::Octal:
            if (str.find_first_not_of(U"01234567") == std::string::npos)
                return Notation::Octal;
            break;
        case Notation::Hexadecimal:
            if (str.find_first_not_of(U"0123456789abcdefABCDEF") == std::string::npos)
                return Notation::Hexadecimal;
            break;
        }
    }
    else
    {
        if (str.find_first_not_of(U"0123456789.") == std::string::npos)
            return Notation::Decimal;
    }
    return Notation::None;
}

}
