/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "power.h"
#include "code_row.h"
#include "brackets.h"
#include "../str.h"
#include "../document.h"

namespace yutovo
{

//Power

Power::Power(Element* _parent, bool with_init) :
    MiddleShapeFormula(_parent, with_init)
{
    type = ElementType::POWER;
    UpdateLevel(level);
}

Power::Power(Document* _document, bool with_init) :
    MiddleShapeFormula(_document, with_init)
{
    type = ElementType::POWER;
    UpdateLevel(level);
}

Power::Power(const Power& source) :
    MiddleShapeFormula(source)
{
}

Element* Power::Clone()
{
    return new Power(*this);
}

Element* Power::Create(Element* _parent)
{
    return new Power(_parent);
}

Element* Power::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new Power(parent, false);
    return new Power(document, false);
}

void Power::Draw() const
{
    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, formula_format->bg_selection_color);
    }

    Formula::Draw();
}

bool Power::Remake(bool with_elements)
{
    UpdateLevel(level);

    bool changed = MiddleShapeFormula::Remake(with_elements);

    GetFirst()->rect.Move(0, GetLast()->rect.height);
    GetShape()->rect.SetRect(0, 0, 4, GetLast()->rect.height + GetFirst()->rect.height);
    GetShape()->rect.Move(GetFirst()->rect.width, 0);
    GetLast()->rect.Move(GetFirst()->rect.width + GetShape()->rect.width, 0);

    baseline = GetFirst()->rect.top + GetFirst()->baseline;

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

bool Power::AfterInsert(bool with_undo)
{
    if (!MiddleShapeFormula::AfterInsert(with_undo))
        return false;
    auto first = GetFirst();
    if (first->elements->Count() == 1)
    {
        auto el = first->elements->Get(0);
        if (el->type == ElementType::CODE_ROW && el->elements->Size() > 1)
        {
            //insert brackets
            el->elements->Insert(ElementPtr(new OpenBracket(first, ElementType::OPEN_ROUND_BRACKET)), 0);
            el->elements->Insert(ElementPtr(new CloseBracket(first, ElementType::CLOSE_ROUND_BRACKET)), el->elements->Count());
        }
    }
    else if (first->elements->Count() > 1 && first->elements->Get(0)->type != ElementType::OPEN_ROUND_BRACKET && 
        first->elements->Get(0)->type != ElementType::CLOSE_ROUND_BRACKET)
    {
        //insert brackets
        first->elements->Insert(ElementPtr(new OpenBracket(first, ElementType::OPEN_ROUND_BRACKET)), 0);
        first->elements->Insert(ElementPtr(new CloseBracket(first, ElementType::CLOSE_ROUND_BRACKET)), first->elements->Count());
    }
    return true;
}

void Power::AfterChildInsert(const ElementId child_id, bool with_undo)
{
    //if a close fense was inserted, move elements from parent row into first child until open bracket
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
    if (with_undo)
        document->StoreUndo(parent->id);
    for (int i = pos - 1; i >= open_pos; --i)
    {
        auto _el = parent->elements->Get(i);
        GetFirst()->elements->Move(_el, 0);
    }
}

void Power::UpdateLevel(uint8_t _level)
{
    MiddleShapeFormula::UpdateLevel(_level);
    if (_level >= MAX_LEVEL)
        return;
    if (GetLast())
        GetLast()->UpdateLevel(_level + 1);
}

bool Power::UseSelection()
{
    return true;
}

std::string Power::ToHtml() const
{
    if (!GetFirst() || !GetLast())
        return "";
    std::string s = "<msup>";
    s += GetFirst()->ToHtml();
    s += GetLast()->ToHtml();
    s += "</msup>";
    return s;
}

std::u32string Power::ToText() const
{
    if (!GetFirst() || !GetLast())
        return U"";
    return U"pow(" + GetFirst()->ToText() + U"," + GetLast()->ToText() + U")";
}

void Power::ToParserString(ParserString& str)
{
    if (!GetFirst() || !GetLast())
        return;
    str.Add(id, U"pow(");
    GetFirst()->ToParserString(str);
    str.Add(id, U",");
    GetLast()->ToParserString(str);
    str.Add(id, U")");
}

void Power::AddBase(ElementPtr base)
{
    if (GetFirst()->IsEmpty())
        GetFirst()->elements->Clear();
    GetFirst()->elements->Add(base);
}

void Power::AddExponent(ElementPtr exponent)
{
    if (GetLast()->IsEmpty())
        GetLast()->elements->Clear();
    GetLast()->elements->Add(exponent);
    UpdateLevel(level);
}

CodeRow<>* Power::GetBaseRow() const
{
    return GetFirst();
}

CodeRow<>* Power::GetExponentRow() const
{
    return GetLast();
}

}
