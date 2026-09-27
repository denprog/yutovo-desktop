/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "brackets.h"
#include "../document.h"

namespace yutovo
{

//OpenBracket

const std::string OpenBracket::family_name = "Arial";

OpenBracket::OpenBracket(Element* _parent, ElementType _type) : 
    OnlyShapeFormula(_parent, _type == ElementType::OPEN_ROUND_BRACKET ? '(' : '[')
{
    assert(_type == ElementType::OPEN_ROUND_BRACKET || _type == ElementType::OPEN_SQUARE_BRACKET);
    type = _type;
    remake_always = true;
    formula_format = document->formula_formats->GetFormat("Formula");
}

OpenBracket::OpenBracket(Document* _document, ElementType _type) :
    OnlyShapeFormula(_document, _type == ElementType::OPEN_ROUND_BRACKET ? '(' : '[')
{
    assert(_type == ElementType::OPEN_ROUND_BRACKET || _type == ElementType::OPEN_SQUARE_BRACKET);
    type = _type;
    remake_always = true;
    formula_format = document->formula_formats->GetFormat("Formula");
}

OpenBracket::OpenBracket(const OpenBracket& source) :
    OnlyShapeFormula(source)
{
    remake_always = true;
}

Element* OpenBracket::Clone()
{
    return new OpenBracket(*this);
}

Element* OpenBracket::Create(Element* _parent)
{
    return new OpenBracket(_parent, type);
}

Element* OpenBracket::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    ElementType type = ElementType::OPEN_ROUND_BRACKET;
    if (value.HasMember("type") && value["type"].IsInt())
        type = (ElementType)value["type"].GetInt();

    OpenBracket* p = nullptr;
    if (parent)
        p = new OpenBracket(parent, type);
    else
        p = new OpenBracket(document, type);
    if (!p->SymbolFromJson(value, alloc))
    {
        delete p;
        return nullptr;
    }
    return p;
}

void OpenBracket::Draw() const
{
    shape->draw_func = 
        [&](const Rect& r)
        {
            if (format)
            {
                window->DrawText(ToBasicString(ToText()), format, r, 
                    document->selection.IsSelected(id) ? document->config.formula_bg_color : document->config.shapes_color, 
                    document->selection.IsSelected(id) ? document->config.bg_selection_color : document->config.formula_bg_color, false);
            }
        };

    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        parent->window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, formula_format->bg_selection_color);
    }

    Formula::Draw();
}

bool OpenBracket::Remake(bool with_elements)
{
    bool changed = OnlyShapeFormula::Remake(with_elements);

    auto empty_brace = 
        [&]()
        {
            Size s = window->GetTextSize(U" ", GetStringFormat());
            int size = window->GetSymbolSize(symbol, (int)lround(s.height * 1.5), family_name, s, baseline);
            baseline = s.height * 2 / 3;
            rect.SetRect(0, 0, s.width, s.height);
            shape->rect = rect;
            format = document->GetStringFormat(family_name, size, false, false, false, false, false, false, Color::Black(), 
                Color::White(), Color::Blue());
        };

    if (parent->elements->IsLast(id) || 
        (type == ElementType::OPEN_ROUND_BRACKET && parent->elements->Count() == 2 && parent->elements->Get(0)->type == ElementType::CLOSE_ROUND_BRACKET) ||
        (type == ElementType::OPEN_SQUARE_BRACKET && parent->elements->Count() == 2 && parent->elements->Get(0)->type == ElementType::CLOSE_SQUARE_BRACKET))
    {
        empty_brace();
        return changed;
    }
    
    //set rect of the shape by the next elements until close bracket
    int pos = parent->elements->GetElementPos(id);
    int close_pos = -1;
    int open_count = 0;
    for (int i = pos + 1; i < parent->elements->Count(); ++i)
    {
        auto el = parent->elements->Get(i);
        if ((type == ElementType::OPEN_ROUND_BRACKET && el->type == ElementType::OPEN_ROUND_BRACKET) || 
            (type == ElementType::OPEN_SQUARE_BRACKET && el->type == ElementType::OPEN_SQUARE_BRACKET))
        {
            ++open_count;
        }
        else if ((type == ElementType::OPEN_ROUND_BRACKET && el->type == ElementType::CLOSE_ROUND_BRACKET) || 
            (type == ElementType::OPEN_SQUARE_BRACKET && el->type == ElementType::CLOSE_SQUARE_BRACKET))
        {
            if (open_count == 0)
            {
                close_pos = i;
                break;
            }
            else
                --open_count;
        }
    }

    if (close_pos == pos + 1)
    {
        empty_brace();
        return true;
    }

    for (int i = pos + 1; i < close_pos; ++i)
    {
        auto el = parent->elements->Get(i);
        el->Remake(with_elements);
    }

    int max_height = 0;
    for (int i = pos + 1; i < (close_pos == -1 ? parent->elements->Count() : close_pos); ++i)
    {
        auto el = parent->elements->Get(i);
        if (el->rect.height > max_height)
            max_height = el->rect.height;
    }

    Size s;
    int size = window->GetSymbolSize(symbol, (int)lround(max_height * 1.5), family_name, s, baseline);
    baseline = s.height * 2 / 3;
    if (size != 0)
    {
        rect.SetRect(0, 0, s.width, s.height);
        shape->rect = rect;
        format = document->GetStringFormat(family_name, size, false, false, false, false, false, false, Color::Black(), 
            Color::White(), Color::Blue());
    }

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

std::string OpenBracket::ToHtml() const
{
    return "<mo>" + ToBasicString(ToText()) + "</mo>";
}

//CloseBracket

const std::string CloseBracket::family_name = "Arial";

CloseBracket::CloseBracket(Element* _parent, ElementType _type) : 
    OnlyShapeFormula(_parent, _type == ElementType::CLOSE_ROUND_BRACKET ? ')' : ']')
{
    assert(_type == ElementType::CLOSE_ROUND_BRACKET || _type == ElementType::CLOSE_SQUARE_BRACKET);
    type = _type;
    remake_always = true;
    formula_format = document->formula_formats->GetFormat("Formula");
}

CloseBracket::CloseBracket(Document* _document, ElementType _type) :
    OnlyShapeFormula(_document, _type == ElementType::CLOSE_ROUND_BRACKET ? ')' : ']')
{
    assert(_type == ElementType::CLOSE_ROUND_BRACKET || _type == ElementType::CLOSE_SQUARE_BRACKET);
    type = _type;
    remake_always = true;
    formula_format = document->formula_formats->GetFormat("Formula");
}

CloseBracket::CloseBracket(const CloseBracket& source) :
    OnlyShapeFormula(source)
{
    remake_always = true;
}

Element* CloseBracket::Clone()
{
    return new CloseBracket(*this);
}

Element* CloseBracket::Create(Element* _parent)
{
    return new CloseBracket(_parent, type);
}

Element* CloseBracket::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    ElementType type = ElementType::CLOSE_ROUND_BRACKET;
    if (value.HasMember("type") && value["type"].IsInt())
        type = (ElementType)value["type"].GetInt();

    CloseBracket* p = nullptr;
    if (parent)
        p = new CloseBracket(parent, type);
    else
        p = new CloseBracket(document, type);
    if (!p->SymbolFromJson(value, alloc))
    {
        delete p;
        return nullptr;
    }
    return p;
}

void CloseBracket::Draw() const
{
    shape->draw_func = 
        [&](const Rect& r)
        {
            if (format)
            {
                window->DrawText(ToBasicString(ToText()), format, r, 
                    document->selection.IsSelected(id) ? document->config.formula_bg_color : document->config.shapes_color, 
                    document->selection.IsSelected(id) ? document->config.bg_selection_color : document->config.formula_bg_color, false);
            }
        };

    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        parent->window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, document->config.bg_selection_color);
    }

    Formula::Draw();
}

bool CloseBracket::Remake(bool with_elements)
{
    bool changed = OnlyShapeFormula::Remake(with_elements);

    auto empty_brace = 
        [&]()
        {
            Size s = window->GetTextSize(U" ", GetStringFormat());
            int size = window->GetSymbolSize(symbol, (int)lround(s.height * 1.5), family_name, s, baseline);
            baseline = s.height * 2 / 3;
            rect.SetRect(0, 0, s.width, s.height);
            shape->rect = rect;
            format = document->GetStringFormat(family_name, size, false, false, false, false, false, false, Color::Black(), 
                Color::White(), Color::Blue());
        };

    if (parent->elements->IsFirst(id) || 
        (type == ElementType::CLOSE_ROUND_BRACKET && parent->elements->Count() == 2 && parent->elements->Get(0)->type == ElementType::OPEN_ROUND_BRACKET) ||
        (type == ElementType::CLOSE_SQUARE_BRACKET && parent->elements->Count() == 2 && parent->elements->Get(0)->type == ElementType::OPEN_SQUARE_BRACKET))
    {
        empty_brace();
        return changed;
    }
    
    //set rect of the shape by the previous elements until corresponsing open bracket
    int pos = parent->elements->GetElementPos(id);
    int open_pos = -1;
    int close_count = 0;
    for (int i = pos - 1; i >= 0; --i)
    {
        auto el = parent->elements->Get(i);
        if ((type == ElementType::CLOSE_ROUND_BRACKET && el->type == ElementType::CLOSE_ROUND_BRACKET) || 
            (type == ElementType::CLOSE_SQUARE_BRACKET && el->type == ElementType::CLOSE_SQUARE_BRACKET))
        {
            ++close_count;
        }
        else if ((type == ElementType::CLOSE_ROUND_BRACKET && el->type == ElementType::OPEN_ROUND_BRACKET) || 
            (type == ElementType::CLOSE_SQUARE_BRACKET && el->type == ElementType::OPEN_SQUARE_BRACKET))
        {
            if (close_count == 0)
            {
                open_pos = i;
                break;
            }
            else
                --close_count;
        }
    }

    if (open_pos == pos - 1)
    {
        empty_brace();
        return true;
    }
    
    int max_height = 0;
    for (int i = (open_pos == -1 ? 0 : open_pos + 1); i < pos; ++i)
    {
        auto el = parent->elements->Get(i);
        if (el->rect.height > max_height)
            max_height = el->rect.height;
    }

    Size s;
    int size = window->GetSymbolSize(symbol, (int)lround(max_height * 1.5), family_name, s, baseline);
    baseline = s.height * 2 / 3;
    if (size != 0)
    {
        rect.SetRect(0, 0, s.width, s.height);
        shape->rect = rect;
        format = document->GetStringFormat(family_name, size, false, false, false, false, false, false, Color::Black(), 
            Color::White(), Color::Blue());
    }

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

std::string CloseBracket::ToHtml() const
{
    return "<mo>" + ToBasicString(ToText()) + "</mo>";
}

}
