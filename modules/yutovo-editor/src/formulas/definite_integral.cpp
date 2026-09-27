/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "definite_integral.h"
#include "document.h"
#include "code_row.h"
#include "code_string.h"

namespace yutovo
{

//DefiniteIntegral

const std::string DefiniteIntegral::family_name = "DejaVu Serif";

DefiniteIntegral::DefiniteIntegral(Element* _parent, bool with_init) :
    Formula(_parent)
{
    type = ElementType::DEFINITE_INTEGRAL;
    if (with_init)
        Init();
}

DefiniteIntegral::DefiniteIntegral(Document* _document, bool with_init) :
    Formula(_document)
{
    type = ElementType::DEFINITE_INTEGRAL;
    if (with_init)
        Init();
}

DefiniteIntegral::DefiniteIntegral(const DefiniteIntegral& source) :
    Formula(source),
    symbol_str(source.symbol_str)
{
}

Element* DefiniteIntegral::Clone()
{
    return new DefiniteIntegral(*this);
}

Element* DefiniteIntegral::Create(Element* _parent)
{
    return new DefiniteIntegral(_parent);
}

Element* DefiniteIntegral::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new DefiniteIntegral(parent, false);
    return new DefiniteIntegral(document, false);
}

void DefiniteIntegral::Init()
{
    symbol_str = ToBasicString(std::u32string(1, symbol));
    elements->Add(ElementPtr(new CodeRow<>(this))); //lower limit of integration
    elements->Add(ElementPtr(new Shape(this))); //integral symbol
    elements->Add(ElementPtr(new CodeRow<>(this))); //upper limit of integration
    elements->Add(ElementPtr(new CodeRow<>(this))); //integrand
    ElementPtr d(new CodeString(this, U"d")); //differential sign
    d->editable = false;
    elements->Add(d);
    elements->Add(ElementPtr(new CodeRow<>(this, true))); //integration variable

    UpdateLevel(level);
}

bool DefiniteIntegral::AfterFromJson()
{
    if (elements->Count() != 6)
        return false;
    symbol_str = ToBasicString(std::u32string(1, symbol));
    return true;
}

void DefiniteIntegral::Draw() const
{
    GetShape()->draw_func =
        [&](const Rect& r)
        {
            if (format)
            {
                window->DrawText(symbol_str, format, r, document->selection.IsSelected(id) ? document->config.formula_bg_color : document->config.shapes_color,
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

bool DefiniteIntegral::Remake(bool with_elements)
{
    UpdateLevel(level);

    bool changed = Formula::Remake(with_elements);
    CodeRow<>* lower = GetLower();
    Shape* shape = GetShape();
    CodeRow<>* upper = GetUpper();
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

    int max_width = std::max(upper->rect.width, std::max(shape->rect.width, lower->rect.width));

    if (upper->rect.width < max_width)
        upper->rect.Move((max_width - upper->rect.width) / 2, 0);

    if (shape->rect.width < max_width)
        shape->rect.Move((max_width - shape->rect.width) / 2, upper->rect.height + 3);
    else
        shape->rect.Move(0, upper->rect.height + 3);

    if (lower->rect.width < max_width)
        lower->rect.Move((max_width - lower->rect.width) / 2, upper->rect.height + 3 + shape->rect.height + 3);
    else
        lower->rect.Move(0, upper->rect.height + 3 + shape->rect.height + 3);

    baseline = upper->rect.height + 2 + s.height / 2;

    expression->rect.Move(max_width + 2, baseline - expression->baseline);
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

void DefiniteIntegral::Normalize()
{
    Element::Normalize(); //skip Formula::Normalize()

    if (elements->Count() > 6)
        elements->Get(5)->Merge(elements->Get(6));
}

bool DefiniteIntegral::IsOnD(const CaretState& caret_state) const
{
    return caret_state.id == GetD()->id;
}

bool DefiniteIntegral::IsInsideD(const CaretState& caret_state) const
{
    return caret_state.IsInsideElement(GetD()->id);
}

bool DefiniteIntegral::SkipDLeft(CaretState& caret_state, Selection* select) const
{
    CaretState c;
    if (GetExpression()->GetLastCaretState(c, select))
    {
        caret_state = c;
        return true;
    }
    return false;
}

bool DefiniteIntegral::SkipDRight(CaretState& caret_state, Selection* select) const
{
    CaretState c;
    if (GetVariable()->GetFirstCaretState(c, select))
    {
        caret_state = c;
        return true;
    }
    return false;
}

bool DefiniteIntegral::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (!select && (caret->IsOnElement(GetD()->id) || caret->IsInsideElement(GetD()->id)))
        return SkipDLeft(caret_state, select);

    if (select)
    {
        if (caret->IsOnElement(GetLower()->id) || caret->IsOnElement(GetShape()->id) || caret->IsOnElement(GetUpper()->id) ||
            caret->IsOnElement(GetExpression()->id) || caret->IsOnElement(GetD()->id) || caret->IsOnElement(GetVariable()->id) ||
            caret->IsInsideElement(GetLower()->id) || caret->IsInsideElement(GetShape()->id) || caret->IsInsideElement(GetUpper()->id) ||
            caret->IsInsideElement(GetExpression()->id) || caret->IsInsideElement(GetD()->id) || caret->IsInsideElement(GetVariable()->id))
        {
            return false;
        }
    }

    bool res = Formula::GetLeftCaretState(caret_state, select);
    if (!select && res && (IsOnD(caret_state) || IsInsideD(caret_state)))
        return SkipDLeft(caret_state, select);
    return res;
}

bool DefiniteIntegral::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    if (!select && (caret->IsOnElement(GetD()->id) || caret->IsInsideElement(GetD()->id)))
        return SkipDRight(caret_state, select);

    if (select)
    {
        if (caret->IsOnElement(GetLower()->id) || caret->IsOnElement(GetShape()->id) || caret->IsOnElement(GetUpper()->id) ||
            caret->IsOnElement(GetExpression()->id) || caret->IsOnElement(GetD()->id) || caret->IsOnElement(GetVariable()->id) ||
            caret->IsInsideElement(GetLower()->id) || caret->IsInsideElement(GetShape()->id) || caret->IsInsideElement(GetUpper()->id) ||
            caret->IsInsideElement(GetExpression()->id) || caret->IsInsideElement(GetD()->id) || caret->IsInsideElement(GetVariable()->id))
        {
            return false;
        }
    }

    bool res = Formula::GetRightCaretState(caret_state, select);
    if (!select && res && (IsOnD(caret_state) || IsInsideD(caret_state)))
        return SkipDRight(caret_state, select);
    return res;
}

bool DefiniteIntegral::GetWordLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (!select && (caret->IsOnElement(GetD()->id) || caret->IsInsideElement(GetD()->id)))
        return SkipDLeft(caret_state, select);

    if (select)
    {
        if (caret->IsOnElement(GetLower()->id) || caret->IsOnElement(GetShape()->id) || caret->IsOnElement(GetUpper()->id) ||
            caret->IsOnElement(GetExpression()->id) || caret->IsOnElement(GetD()->id) || caret->IsOnElement(GetVariable()->id) ||
            caret->IsInsideElement(GetLower()->id) || caret->IsInsideElement(GetShape()->id) || caret->IsInsideElement(GetUpper()->id) ||
            caret->IsInsideElement(GetExpression()->id) || caret->IsInsideElement(GetD()->id) || caret->IsInsideElement(GetVariable()->id))
        {
            return false;
        }
    }

    bool res = Formula::GetWordLeftCaretState(caret_state, select);
    if (!select && res && (IsOnD(caret_state) || IsInsideD(caret_state)))
        return SkipDLeft(caret_state, select);
    return res;
}

bool DefiniteIntegral::GetWordRightCaretState(CaretState& caret_state, Selection* select)
{
    if (!select && (caret->IsOnElement(GetD()->id) || caret->IsInsideElement(GetD()->id)))
        return SkipDRight(caret_state, select);

    if (select)
    {
        if (caret->IsOnElement(GetLower()->id) || caret->IsOnElement(GetShape()->id) || caret->IsOnElement(GetUpper()->id) ||
            caret->IsOnElement(GetExpression()->id) || caret->IsOnElement(GetD()->id) || caret->IsOnElement(GetVariable()->id) ||
            caret->IsInsideElement(GetLower()->id) || caret->IsInsideElement(GetShape()->id) || caret->IsInsideElement(GetUpper()->id) ||
            caret->IsInsideElement(GetExpression()->id) || caret->IsInsideElement(GetD()->id) || caret->IsInsideElement(GetVariable()->id))
        {
            return false;
        }
    }

    bool res = Formula::GetWordRightCaretState(caret_state, select);
    if (!select && res && (IsOnD(caret_state) || IsInsideD(caret_state)))
        return SkipDRight(caret_state, select);
    return res;
}

bool DefiniteIntegral::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (select)
    {
        select->Add(id);
        caret_state.SetState(parent->id, parent->elements->GetElementPos(id));
        return true;
    }

    CodeRow<>* lower = GetLower();
    Shape* shape = GetShape();
    CodeRow<>* upper = GetUpper();
    if (lower->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(lower->id))
        return lower->GetTopCaretState(x, y, caret_state, select);
    if (shape->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(shape->id))
        return shape->GetTopCaretState(x, y, caret_state, select);
    if (upper->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(upper->id))
        return upper->GetTopCaretState(x, y, caret_state, select);
    return parent->GetTopCaretState(x, y, caret_state, select);
}

bool DefiniteIntegral::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (select)
    {
        select->Add(id);
        caret_state.SetState(parent->id, parent->elements->GetElementPos(id));
        return true;
    }

    CodeRow<>* lower = GetLower();
    Shape* shape = GetShape();
    CodeRow<>* upper = GetUpper();
    if (upper->GetAbsoluteRect().top >= y)
        return upper->GetBottomCaretState(x, y, caret_state, select);
    if (shape->GetAbsoluteRect().top >= y)
        return shape->GetBottomCaretState(x, y, caret_state, select);
    if (lower->GetAbsoluteRect().top >= y)
        return lower->GetBottomCaretState(x, y, caret_state, select);
    return parent->GetBottomCaretState(x, y, caret_state, select);
}

bool DefiniteIntegral::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    if (caret->IsOnElement(GetLower()->id) || caret->IsOnElement(GetUpper()->id) || caret->IsOnElement(GetExpression()->id) ||
        caret->IsOnElement(GetVariable()->id)) //don't delete these elements
        return false;

    uint start, size;
    if (selection->Has(id, start, size))
    {
        CodeRow<>* row = nullptr;
        if (start == 0 && size == 1)
            row = GetLower();
        else if (start == 2 && size == 1)
            row = GetUpper();
        else if (start == 3 && size == 1)
            row = GetExpression();
        else if (start == 5 && size == 1)
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
    CodeRow<>* lower = GetLower();
    CodeRow<>* expression = GetExpression();
    lower->UpdateLevel(level);
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

void DefiniteIntegral::UpdateLevel(uint8_t _level)
{
    Formula::UpdateLevel(_level);
    if (_level >= MAX_LEVEL)
        return;
    CodeRow<>* lower = GetLower();
    CodeRow<>* upper = GetUpper();
    if (lower)
        lower->UpdateLevel(_level + 1);
    if (upper)
        upper->UpdateLevel(_level + 1);
}

bool DefiniteIntegral::AfterInsert(bool with_undo)
{
    CodeRow<>* lower = GetLower();
    if (!caret)
        return false;
    CaretState c;
    if (lower->GetFirstCaretState(c, nullptr))
    {
        caret->SetState(c);
        return true;
    }
    return false;
}

std::string DefiniteIntegral::ToHtml() const
{
    std::string s =
        "<munderover>"
            "<mo>" + symbol_str + "</mo>"
            "<mrow>" + GetLower()->ToHtml() + "</mrow>"
            "<mrow>" + GetUpper()->ToHtml() + "</mrow>"
        "</munderover>";
    s += GetExpression()->ToHtml();
    s += GetD()->ToHtml();
    s += GetVariable()->ToHtml();
    return s;
}

std::u32string DefiniteIntegral::ToText() const
{
    CodeRow<>* lower = GetLower();
    CodeRow<>* upper = GetUpper();
    CodeRow<>* expression = GetExpression();
    CodeRow<>* var = GetVariable();
    if (!lower || !upper || !expression || !var)
        return U"";
    return U"definite_integral(" + lower->ToText() + U"," + upper->ToText() + U"," + expression->ToText() + U"," + var->ToText() + U")";
}

void DefiniteIntegral::ToParserString(ParserString& str)
{
    CodeRow<>* lower = GetLower();
    CodeRow<>* upper = GetUpper();
    CodeRow<>* expression = GetExpression();
    CodeRow<>* var = GetVariable();

    str.Add(id, U"definite_integral(");
    lower->ToParserString(str); //lower limit
    str.Add(id, U",");
    upper->ToParserString(str); //upper limit
    str.Add(id, U",");
    expression->ToParserString(str); //integrand
    str.Add(id, U",");
    var->ToParserString(str); //integration variable
    str.Add(id, U")");
}

CodeRow<>* DefiniteIntegral::GetLower() const
{
    return (CodeRow<>*)elements->Get(0).get();
}

Shape* DefiniteIntegral::GetShape() const
{
    return (Shape*)elements->Get(1).get();
}

CodeRow<>* DefiniteIntegral::GetUpper() const
{
    return (CodeRow<>*)elements->Get(2).get();
}

CodeRow<>* DefiniteIntegral::GetExpression() const
{
    return (CodeRow<>*)elements->Get(3).get();
}

CodeString* DefiniteIntegral::GetD() const
{
    return (CodeString*)elements->Get(4).get();
}

CodeRow<>* DefiniteIntegral::GetVariable() const
{
    return (CodeRow<>*)elements->Get(5).get();
}

}
