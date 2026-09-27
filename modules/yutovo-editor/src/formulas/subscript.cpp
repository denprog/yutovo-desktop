/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "subscript.h"
#include "code_row.h"
#include "../str.h"
#include "code_string.h"
#include "../document.h"

namespace yutovo
{

//Subscript

Subscript::Subscript(Element* _parent, bool with_init) :
    MiddleShapeFormula(_parent, with_init)
{
    type = ElementType::SUBSCRIPT;
    UpdateLevel(level);
}

Subscript::Subscript(Document* _document, bool with_init) :
    MiddleShapeFormula(_document, with_init)
{
    type = ElementType::SUBSCRIPT;
    UpdateLevel(level);
}

Subscript::Subscript(Document* _document, const std::string& str, const std::string& sub_str) :
    MiddleShapeFormula(_document, true)
{
    type = ElementType::SUBSCRIPT;
    elements->Get(0)->elements->Clear();
    elements->Get(0)->elements->Add(ElementPtr(new CodeString(this, str)));
    elements->Get(2)->elements->Clear();
    elements->Get(2)->elements->Add(ElementPtr(new CodeString(this, sub_str)));
    UpdateLevel(level);
}

Subscript::Subscript(const Subscript& source) :
    MiddleShapeFormula(source)
{
}

Element* Subscript::Clone()
{
    return new Subscript(*this);
}

Element* Subscript::Create(Element* _parent)
{
    return new Subscript(_parent);
}

Element* Subscript::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new Subscript(parent, false);
    return new Subscript(document, false);
}

void Subscript::Draw() const
{
    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, document->config.bg_selection_color);
    }

    Formula::Draw();
}

bool Subscript::Remake(bool with_elements)
{
    UpdateLevel(level);

    bool changed = MiddleShapeFormula::Remake(true);

    GetFirst()->rect.Move(0, 0);
    GetShape()->rect.SetRect(0, 0, 4, GetLast()->rect.height + GetFirst()->rect.height / 2);
    GetShape()->rect.Move(GetFirst()->rect.width, 0);
    GetLast()->rect.Move(GetFirst()->rect.width + GetShape()->rect.width, GetFirst()->rect.height / 2);

    baseline = GetFirst()->rect.top + GetFirst()->baseline;

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

void Subscript::AfterChildInsert(const ElementId child_id, bool with_undo)
{
    //if a close bracket was inserted, move elements from parent row into first child until open bracket
    if (yutovo::GetParent(child_id) != GetFirst()->id)
        return;
    auto el = document->GetElement(child_id);
    if (el->type != ElementType::CLOSE_ROUND_BRACKET)
        return;
    int pos = parent->elements->GetElementPos(id);
    int open_pos = 0;
    for (int i = pos - 1; i >= 0; --i)
    {
        auto c = parent->elements->Get(i);
        if (c->type == ElementType::OPEN_ROUND_BRACKET)
        {
            open_pos = i;
            break;
        }
    }
    for (int i = pos - 1; i >= open_pos; --i)
    {
        auto _el = parent->elements->Get(i);
        GetFirst()->elements->Move(_el, 0);
    }
}

void Subscript::UpdateLevel(uint8_t _level)
{
    MiddleShapeFormula::UpdateLevel(_level);
    if (_level >= MAX_LEVEL)
        return;
    if (GetLast())
        GetLast()->UpdateLevel(_level + 1);
}

std::string Subscript::ToHtml() const
{
    if (!GetFirst() || !GetLast())
        return "";
    std::string s = "<msub>";
    s += GetFirst()->ToHtml();
    s += GetLast()->ToHtml();
    s += "</msub>";
    return s;
}

std::u32string Subscript::ToText() const
{
    if (!GetFirst() || !GetLast())
        return U"";
    auto _first = GetFirst()->ToText();
    auto _last = GetLast()->ToText();
    if (_last == U"bin" || _last == U"oct" || _last == U"dec" || _last == U"hex")
        return _last + U"[" + _first + U"]";
    if (_first == U"log") //TODO: get names of functions from the parser
        return _first + U":" + _last + U",";
    return _first + U"{" + _last + U"}";
}

void Subscript::ToParserString(ParserString& str)
{
    if (!GetFirst() || !GetLast())
        return;
    if (GetFirst()->ToText() == U"log")
    {
        GetFirst()->ToParserString(str);
        str.Add(id, U":");
        GetLast()->ToParserString(str);
        str.Add(id, U",");
    }
    else
    {
        auto _last = GetLast()->ToText();
        if (_last == U"bin" || _last == U"oct" || _last == U"dec" || _last == U"hex")
        {
            GetLast()->ToParserString(str);
            str.Add(id, U"[");
            GetFirst()->ToParserString(str);
            str.Add(id, U"]");
        }
        else
        {
            GetFirst()->ToParserString(str);
            str.Add(id, U"{");
            GetLast()->ToParserString(str);
            str.Add(id, U"}");
        }
    }
}

}
