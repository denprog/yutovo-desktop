/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "code_column.h"

namespace yutovo
{

//CodeColumn

CodeColumn::CodeColumn(Document* _document) :
    CodeRow<>(_document)
{
    type = ElementType::CODE_COLUMN;
}

CodeColumn::CodeColumn(Element* parent) : 
    CodeRow<>(parent, false)
{
    type = ElementType::CODE_COLUMN;
}

Element* CodeColumn::Clone()
{
    return new CodeColumn(*this);
}

Element* CodeColumn::Create(Element* parent)
{
    return new CodeColumn(parent);
}

bool CodeColumn::Remake(bool with_elements)
{
    if (document->break_remake)
        return false;

    bool changed = CodeRow::Remake(with_elements);

    ParagraphFormatPtr format = GetParagraphFormat();
    int cy = 0;
    int left_m = 0, top_m = 0, right_m = 0, bottom_m = 0;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        el->GetMargin(left_m, top_m, right_m, bottom_m);
        el->rect.Move(left_m, cy + top_m);
        cy += el->rect.height + top_m + bottom_m + format->line_spacing;
    }
    UpdateRect();

    //align the baseline
    if (elements->Count() == 1)
        baseline = elements->Get(0)->baseline;
    else
        baseline = rect.height / 2;

    rect.height += bottom_m;

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

bool CodeColumn::GetBeginCaretState(CaretState& caret_state, Selection* select)
{
    CaretState c = caret->GetCaretState();
    auto row = document->FindParentRow(c.id);
    if (!row)
        return false;
    if (row->GetFirstCaretState(c, select))
    {
        if (c == caret_state)
            return parent->GetBeginCaretState(caret_state, select);
        caret_state = c;
        return true;
    }
    return false;
}

bool CodeColumn::GetEndCaretState(CaretState& caret_state, Selection* select)
{
    CaretState c = caret->GetCaretState();
    auto row = document->FindParentRow(c.id);
    if (!row)
        return false;
    if (row->GetLastCaretState(c, select))
    {
        if (c == caret_state)
            return parent->GetEndCaretState(caret_state, select);
        caret_state = c;
        return true;
    }
    return false;
}

bool CodeColumn::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    ElementPtr row;
    //find nearest row
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr el = elements->Get(i);
        if (y < el->GetAbsoluteRect().GetBottom() || el->rect.height == 0)
            break;
        row = el;
    }
    if (!row)
        return parent->GetTopCaretState(x, GetAbsoluteRect().top, caret_state, select);
    return row->GetTopCaretState(x, y, caret_state, select);
}

bool CodeColumn::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    ElementPtr row;
    //find nearest row
    for (int i = elements->Count() - 1; i >= 0; --i)
    {
        ElementPtr el = elements->Get(i);
        if (y > el->GetAbsoluteRect().top || el->rect.height == 0)
            break;
        row = el;
    }
    if (!row)
        return parent->GetBottomCaretState(x, GetAbsoluteRect().GetBottom(), caret_state, select);
    return row->GetBottomCaretState(x, y, caret_state, select);
}

void CodeColumn::AddEmptyElement()
{
    elements->Add(ElementPtr(new CodeRow<>(this)));
}

std::string CodeColumn::ToHtml() const
{
    return elements->ToHtml();
}

std::u32string CodeColumn::ToText() const
{
    std::u32string res;
    for (int i = 0; i < elements->Count(); ++i)
    {
        res += elements->Get(i)->ToText();
        if (i < elements->Count() - 1)
            res += U",";
    }
    return res;
}

}
