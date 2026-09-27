/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "iteration.h"
#include "document.h"
#include "code_row.h"
#include "assignment.h"

namespace yutovo
{

//Iteration

const std::string Iteration::family_name = "DejaVu Serif";

Iteration::Iteration(Element* _parent, char32_t _symbol, bool with_init) : 
    Formula(_parent),
    symbol(_symbol)
{
    if (with_init)
        Init();
}

Iteration::Iteration(Document* _document, char32_t _symbol, bool with_init) : 
    Formula(_document),
    symbol(_symbol)
{
    if (with_init)
        Init();
}

Iteration::Iteration(const Iteration& source) :
    Formula(source),
    symbol(source.symbol),
    symbol_str(source.symbol_str)
{
}

void Iteration::Init()
{
    symbol_str = ToBasicString(std::u32string(1, symbol));
    elements->Add(ElementPtr(new Assignment(this, true, false)));
    elements->Get(0)->editable = false;
    elements->Add(ElementPtr(new Shape(this)));
    elements->Add(ElementPtr(new CodeRow<>(this)));
    elements->Add(ElementPtr(new CodeRow<>(this)));

    UpdateLevel(level);
}

bool Iteration::AfterFromJson()
{
    if (elements->Count() != 4)
        return false;
    symbol_str = ToBasicString(std::u32string(1, symbol));
    return true;
}

void Iteration::Draw() const
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

bool Iteration::Remake(bool with_elements)
{
    UpdateLevel(level);

    bool changed = Formula::Remake(with_elements);
    Assignment* lower = GetLower();
    Shape* shape = GetShape();
    CodeRow<>* upper = GetUpper();
    CodeRow<>* right = GetRight();

    Size s;
    int h = std::max(right->rect.height, std::max(right->baseline, right->rect.height - right->baseline));
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

    right->rect.Move(max_width + 2, baseline - right->baseline);

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

void Iteration::Normalize()
{
    Element::Normalize(); //skip Formula::Normalize()

    if (elements->Count() > 4)
        elements->Get(3)->Merge(elements->Get(4));
}

bool Iteration::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (select)
    {
        if (caret->IsOnElement(GetLower()->id) || caret->IsOnElement(GetShape()->id) || caret->IsOnElement(GetUpper()->id) || caret->IsOnElement(GetRight()->id) || 
            caret->IsInsideElement(GetLower()->id) || caret->IsInsideElement(GetShape()->id) || caret->IsInsideElement(GetUpper()->id) || 
            caret->IsInsideElement(GetRight()->id))
        {
            return false;
        }
    }
    return Formula::GetLeftCaretState(caret_state, select);
}

bool Iteration::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    if (select)
    {
        if (caret->IsOnElement(GetLower()->id) || caret->IsOnElement(GetShape()->id) || caret->IsOnElement(GetUpper()->id) || caret->IsOnElement(GetRight()->id) || 
            caret->IsInsideElement(GetLower()->id) || caret->IsInsideElement(GetShape()->id) || caret->IsInsideElement(GetUpper()->id) || 
            caret->IsInsideElement(GetRight()->id))
        {
            return false;
        }
    }
    return Formula::GetRightCaretState(caret_state, select);
}

bool Iteration::GetWordLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (select)
    {
        if (caret->IsOnElement(GetLower()->id) || caret->IsOnElement(GetShape()->id) || caret->IsOnElement(GetUpper()->id) || caret->IsOnElement(GetRight()->id) || 
            caret->IsInsideElement(GetLower()->id) || caret->IsInsideElement(GetShape()->id) || caret->IsInsideElement(GetUpper()->id) || 
            caret->IsInsideElement(GetRight()->id))
        {
            return false;
        }
    }
    return Formula::GetWordLeftCaretState(caret_state, select);
}

bool Iteration::GetWordRightCaretState(CaretState& caret_state, Selection* select)
{
    if (select)
    {
        if (caret->IsOnElement(GetLower()->id) || caret->IsOnElement(GetShape()->id) || caret->IsOnElement(GetUpper()->id) || caret->IsOnElement(GetRight()->id) || 
            caret->IsInsideElement(GetLower()->id) || caret->IsInsideElement(GetShape()->id) || caret->IsInsideElement(GetUpper()->id) || 
            caret->IsInsideElement(GetRight()->id))
        {
            return false;
        }
    }
    return Formula::GetWordRightCaretState(caret_state, select);
}

bool Iteration::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (select)
    {
        select->Add(id);
        caret_state.SetState(parent->id, parent->elements->GetElementPos(id));
        return true;
    }

    Assignment* lower = GetLower();
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

bool Iteration::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (select)
    {
        select->Add(id);
        caret_state.SetState(parent->id, parent->elements->GetElementPos(id));
        return true;
    }
    
    Assignment* lower = GetLower();
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

bool Iteration::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    if (caret->IsOnElement(GetLower()->id) || caret->IsOnElement(GetUpper()->id) || caret->IsOnElement(GetRight()->id)) //don't delete these elements
        return false;

    uint start, size;
    if (selection->Has(id, start, size))
    {
        if (start == 2 && size == 1)
        {
            if (with_undo)
                document->StoreUndo(GetUpper()->id);
            GetUpper()->elements->Clear();
            Normalize();
            selection->Remove(id, start, size);
            changed_element = id;
            return true;
        }
        if (start == 3 && size == 1)
        {
            if (with_undo)
                document->StoreUndo(GetRight()->id);
            GetRight()->elements->Clear();
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
    Assignment* lower = GetLower();
    CodeRow<>* right = GetRight();
    lower->UpdateLevel(level);
    int p = parent->elements->GetElementPos(id);
    uint c1 = 0;
    caret->SetState(id);
    if (right)
    {
        c1 = right->elements->Count();
        parent->elements->Move(*right->elements, p);
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

void Iteration::UpdateLevel(uint8_t _level)
{
    Formula::UpdateLevel(_level);
    if (_level >= MAX_LEVEL)
        return;
    Assignment* lower = GetLower();
    CodeRow<>* upper = GetUpper();
    if (lower)
        lower->UpdateLevel(_level + 1);
    if (upper)
        upper->UpdateLevel(_level + 1);
}

bool Iteration::AfterInsert(bool with_undo)
{
    Assignment* lower = GetLower();
    if (lower->elements->Count() != 3 || !caret)
        return false;
    CaretState c;
    if (lower->elements->Get(0)->GetFirstCaretState(c, nullptr))
    {
        caret->SetState(c);
        return true;
    }
    return false;
}

std::string Iteration::ToHtml() const
{
    std::string s = 
        "<munderover>" 
            "<mo>" + symbol_str + "</mo>"
            "<mrow>" + GetLower()->ToHtml() + "</mrow>"
            "<mrow>" + GetUpper()->ToHtml() + "</mrow>"
        "</munderover>";
    s += GetRight()->ToHtml();
    return s;
}

Assignment* Iteration::GetLower() const
{
    return (Assignment*)elements->Get(0).get();
}

CodeRow<>* Iteration::GetUpper() const
{
    return (CodeRow<>*)elements->Get(2).get();
}

CodeRow<>* Iteration::GetRight() const
{
    return (CodeRow<>*)elements->Get(3).get();
}

Shape* Iteration::GetShape() const
{
    return (Shape*)elements->Get(1).get();
}

}
