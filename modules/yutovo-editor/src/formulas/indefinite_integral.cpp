/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "indefinite_integral.h"
#include "document.h"
#include "code_row.h"
#include "code_string.h"

namespace yutovo
{

//IndefiniteIntegral

const std::string IndefiniteIntegral::family_name = "DejaVu Serif";

IndefiniteIntegral::IndefiniteIntegral(Element* _parent, bool with_init) :
    Formula(_parent)
{
    type = ElementType::INDEFINITE_INTEGRAL;
    if (with_init)
        Init();
}

IndefiniteIntegral::IndefiniteIntegral(Document* _document, bool with_init) :
    Formula(_document)
{
    type = ElementType::INDEFINITE_INTEGRAL;
    if (with_init)
        Init();
}

IndefiniteIntegral::IndefiniteIntegral(const IndefiniteIntegral& source) :
    Formula(source),
    symbol_str(source.symbol_str)
{
}

Element* IndefiniteIntegral::Clone()
{
    return new IndefiniteIntegral(*this);
}

Element* IndefiniteIntegral::Create(Element* _parent)
{
    return new IndefiniteIntegral(_parent);
}

Element* IndefiniteIntegral::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, 
    rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new IndefiniteIntegral(parent, false);
    return new IndefiniteIntegral(document, false);
}

void IndefiniteIntegral::Init()
{
    symbol_str = ToBasicString(std::u32string(1, symbol));
    elements->Add(ElementPtr(new Shape(this))); //integral symbol
    elements->Add(ElementPtr(new CodeRow<>(this))); //integrand
    ElementPtr d(new CodeString(this, U"d")); //differential sign
    d->editable = false;
    elements->Add(d);
    elements->Add(ElementPtr(new CodeRow<>(this))); //integration variable

    UpdateLevel(level);
}

bool IndefiniteIntegral::AfterFromJson()
{
    if (elements->Count() != 4)
        return false;
    symbol_str = ToBasicString(std::u32string(1, symbol));
    return true;
}

void IndefiniteIntegral::Draw() const
{
    GetShape()->draw_func =
        [&](const Rect& r)
        {
            if (format)
            {
                window->DrawText(symbol_str, format, r, 
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

bool IndefiniteIntegral::Remake(bool with_elements)
{
    UpdateLevel(level);

    bool changed = Formula::Remake(with_elements);
    Shape* shape = GetShape();
    CodeRow<>* expression = GetExpression();
    CodeString* d = GetD();
    CodeRow<>* var = GetVariable();

    Size s;
    int h = std::max(expression->rect.height, std::max(expression->baseline, expression->rect.height - expression->baseline));
    int size = window->GetSymbolSize(symbol, (int)lround(h * 2), family_name, s, baseline);
    if (size != 0)
    {
        shape->rect.SetRect(0, 0, s.width, s.height);
        format = document->GetStringFormat(family_name, size, false, false, false, false, false, false, Color::Black(), Color::White(), Color::Blue());
    }

    baseline = std::max(expression->baseline, var->baseline);
    baseline = std::max(baseline, (s.height + 1) / 2 + 2);

    shape->rect.Move(0, baseline - s.height / 2);
    expression->rect.Move(shape->rect.GetRight() + 2, baseline - expression->baseline);
    d->rect.Move(expression->rect.GetRight() + 2, baseline - d->baseline);
    var->rect.Move(d->rect.GetRight(), baseline - var->baseline);

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

void IndefiniteIntegral::Normalize()
{
    Element::Normalize(); //skip Formula::Normalize()

    if (elements->Count() > 4)
        elements->Get(3)->Merge(elements->Get(4));
}

bool IndefiniteIntegral::IsOnD(const CaretState& caret_state) const
{
    return caret_state.id == GetD()->id;
}

bool IndefiniteIntegral::IsInsideD(const CaretState& caret_state) const
{
    return caret_state.IsInsideElement(GetD()->id);
}

bool IndefiniteIntegral::SkipDLeft(CaretState& caret_state, Selection* select) const
{
    CaretState c;
    if (GetExpression()->GetLastCaretState(c, select))
    {
        caret_state = c;
        return true;
    }
    return false;
}

bool IndefiniteIntegral::SkipDRight(CaretState& caret_state, Selection* select) const
{
    CaretState c;
    if (GetVariable()->GetFirstCaretState(c, select))
    {
        caret_state = c;
        return true;
    }
    return false;
}

bool IndefiniteIntegral::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (!select && (caret->IsOnElement(GetD()->id) || caret->IsInsideElement(GetD()->id)))
        return SkipDLeft(caret_state, select);

    if (select)
    {
        if (caret->IsOnElement(GetShape()->id) || caret->IsOnElement(GetExpression()->id) || caret->IsOnElement(GetD()->id) || 
            caret->IsOnElement(GetVariable()->id) || caret->IsInsideElement(GetShape()->id) || caret->IsInsideElement(GetExpression()->id) || 
            caret->IsInsideElement(GetD()->id) || caret->IsInsideElement(GetVariable()->id))
        {
            return false;
        }
    }

    bool res = Formula::GetLeftCaretState(caret_state, select);
    if (!select && res && (IsOnD(caret_state) || IsInsideD(caret_state)))
        return SkipDLeft(caret_state, select);
    return res;
}

bool IndefiniteIntegral::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    if (!select && (caret->IsOnElement(GetD()->id) || caret->IsInsideElement(GetD()->id)))
        return SkipDRight(caret_state, select);

    if (select)
    {
        if (caret->IsOnElement(GetShape()->id) || caret->IsOnElement(GetExpression()->id) || caret->IsOnElement(GetD()->id) || 
            caret->IsOnElement(GetVariable()->id) || caret->IsInsideElement(GetShape()->id) || caret->IsInsideElement(GetExpression()->id) || 
            caret->IsInsideElement(GetD()->id) || caret->IsInsideElement(GetVariable()->id))
        {
            return false;
        }
    }

    bool res = Formula::GetRightCaretState(caret_state, select);
    if (!select && res && (IsOnD(caret_state) || IsInsideD(caret_state)))
        return SkipDRight(caret_state, select);
    return res;
}

bool IndefiniteIntegral::GetWordLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (!select && (caret->IsOnElement(GetD()->id) || caret->IsInsideElement(GetD()->id)))
        return SkipDLeft(caret_state, select);

    if (select)
    {
        if (caret->IsOnElement(GetShape()->id) || caret->IsOnElement(GetExpression()->id) || caret->IsOnElement(GetD()->id) || 
            caret->IsOnElement(GetVariable()->id) || caret->IsInsideElement(GetShape()->id) || caret->IsInsideElement(GetExpression()->id) || 
            caret->IsInsideElement(GetD()->id) || caret->IsInsideElement(GetVariable()->id))
        {
            return false;
        }
    }

    bool res = Formula::GetWordLeftCaretState(caret_state, select);
    if (!select && res && (IsOnD(caret_state) || IsInsideD(caret_state)))
        return SkipDLeft(caret_state, select);
    return res;
}

bool IndefiniteIntegral::GetWordRightCaretState(CaretState& caret_state, Selection* select)
{
    if (!select && (caret->IsOnElement(GetD()->id) || caret->IsInsideElement(GetD()->id)))
        return SkipDRight(caret_state, select);

    if (select)
    {
        if (caret->IsOnElement(GetShape()->id) || caret->IsOnElement(GetExpression()->id) || caret->IsOnElement(GetD()->id) || 
            caret->IsOnElement(GetVariable()->id) || caret->IsInsideElement(GetShape()->id) || caret->IsInsideElement(GetExpression()->id) || 
            caret->IsInsideElement(GetD()->id) || caret->IsInsideElement(GetVariable()->id))
        {
            return false;
        }
    }

    bool res = Formula::GetWordRightCaretState(caret_state, select);
    if (!select && res && (IsOnD(caret_state) || IsInsideD(caret_state)))
        return SkipDRight(caret_state, select);
    return res;
}

bool IndefiniteIntegral::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    if (caret->IsOnElement(GetExpression()->id) || caret->IsOnElement(GetVariable()->id)) //don't delete these elements
        return false;

    uint start, size;
    if (selection->Has(id, start, size))
    {
        CodeRow<>* row = nullptr;
        if (start == 1 && size == 1)
            row = GetExpression();
        else if (start == 3 && size == 1)
            row = GetVariable();
        if (row)
        {
            if (with_undo)
                document->StoreUndo(row->id);
            row->elements->Clear();
            Normalize();
            selection->Remove(id, start, size);
            changed_element = id;
            return true;
        }
        return false;
    }

    if (!caret->IsOnElement(GetShape()->id))
        return false;

    if (with_undo)
        document->StoreUndo(parent->id);

    //remove this element by deleting its shape
    CodeRow<>* expression = GetExpression();
    int p = parent->elements->GetElementPos(id);
    uint c1 = 0;
    caret->SetState(id);
    if (expression)
    {
        c1 = expression->elements->Count();
        parent->elements->Move(*expression->elements, p);
    }
    CaretState c;
    if (parent->elements->Get(p)->GetFirstCaretState(c, nullptr))
        caret->SetState(c);

    auto t = parent->elements->Get(p + c1); //for not removing this element until this function ends
    parent->elements->Remove(id);
    parent->Normalize();
    changed_element = parent->id;
    return true;
}

bool IndefiniteIntegral::AfterInsert(bool with_undo)
{
    CodeRow<>* expression = GetExpression();
    if (!caret)
        return false;
    CaretState c;
    if (expression->GetFirstCaretState(c, nullptr))
    {
        caret->SetState(c);
        return true;
    }
    return false;
}

std::string IndefiniteIntegral::ToHtml() const
{
    std::string s =
        "<mo>" + symbol_str + "</mo>";
    s += GetExpression()->ToHtml();
    s += GetD()->ToHtml();
    s += GetVariable()->ToHtml();
    return s;
}

std::u32string IndefiniteIntegral::ToText() const
{
    CodeRow<>* expression = GetExpression();
    CodeRow<>* var = GetVariable();
    if (!expression || !var)
        return U"";
    return U"indefinite_integral(" + expression->ToText() + U"," + var->ToText() + U")";
}

void IndefiniteIntegral::ToParserString(ParserString& str)
{
    CodeRow<>* expression = GetExpression();
    CodeRow<>* var = GetVariable();

    str.Add(id, U"indefinite_integral(");
    expression->ToParserString(str); //integrand
    str.Add(id, U",");
    var->ToParserString(str); //integration variable
    str.Add(id, U")");
}

Shape* IndefiniteIntegral::GetShape() const
{
    return (Shape*)elements->Get(0).get();
}

CodeRow<>* IndefiniteIntegral::GetExpression() const
{
    return (CodeRow<>*)elements->Get(1).get();
}

CodeString* IndefiniteIntegral::GetD() const
{
    return (CodeString*)elements->Get(2).get();
}

CodeRow<>* IndefiniteIntegral::GetVariable() const
{
    return (CodeRow<>*)elements->Get(3).get();
}

}
