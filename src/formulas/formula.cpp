/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "formula.h"
#include "../document.h"

namespace yutovo
{

//Formula

Formula::Formula(Element* _parent) : 
    Element(_parent)
{
    if (document && document->FindCodeBlock(parent->id) == 0)
        document->GetCurrentFormulaFormat(formula_format);
    else
        formula_format = GetFormulaFormat();
    has_caret_hilight = true;
}

Formula::Formula(Document* _document) :
    Element(_document)
{
    document->GetCurrentFormulaFormat(formula_format);
    has_caret_hilight = true;
}

bool Formula::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    return parent->InsertElements(_elements, insert_mode, with_undo, changed_element);
}

bool Formula::ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo, ElementId& changed_element)
{
    return false;
}

bool Formula::ChangeStringFormat(const StringFormatPtr format, bool with_undo, ElementId& changed_element)
{
    if (*formula_format->string_format == *format)
        return false;
    if (with_undo)
        document->StoreUndo(id);
    formula_format = document->formula_formats->GetFormat(formula_format->name, format, formula_format->inter_spacing, 
        formula_format->left_margin, formula_format->top_margin, formula_format->right_margin, formula_format->bottom_margin, 
        formula_format->color, formula_format->bg_color, formula_format->bg_selection_color);
    changed_element = id;
    document->CaretMoved();
    return true;
}

void Formula::Normalize()
{
    Element::Normalize();

    for (int i = 1; i < elements->Count();)
    {
        auto el1 = elements->Get(i - 1);
        auto el2 = elements->Get(i);
        if (el1->type == ElementType::CODE_ROW && el2->type == ElementType::CODE_ROW)
        {
            //merge the two rows
            if (!el1->Merge(el2))
                ++i;
        }
        else
            ++i;
    }
}

void Formula::Rescale() const
{
    Element::Rescale();
    draw_string_format = document->string_formats->GetFormat(formula_format->string_format, document->config.scale);
}

void Formula::UpdateRect(bool with_elements)
{
    Element::UpdateRect(with_elements);
    if (!draw_string_format)
        Rescale();
}

bool Formula::SplitAt(const uint pos)
{
    return false;
}

bool Formula::Merge(const ElementPtr with_element)
{
    return false;
}

void Formula::GetMargin(int& left, int& top, int& right, int& bottom) const
{
    left = std::round(formula_format->left_margin * document->config.scale);
    top = std::round(formula_format->top_margin * document->config.scale);
    right = std::round(formula_format->right_margin * document->config.scale);
    bottom = std::round(formula_format->bottom_margin * document->config.scale);
}

bool Formula::HasCaretState()
{
    return true;
}

bool Formula::HasLastCaretState()
{
    return true;
}

bool Formula::GetElementAtCoords(const int x, const int y, const int margin, ElementId& _id)
{
    //look in the child elements
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr el = elements->Get(i);
        if (el->GetElementAtCoords(x, y, margin, _id))
            return true;
    }
    Rect r = parent->GetAbsoluteRect(GetCaretRect());
    if (r.IsPointInside(x, y))
    {
        _id = id;
        return true;
    }
    return false;
}

StringFormatPtr Formula::GetStringFormat() const
{
    return formula_format->string_format;
}

bool Formula::IsFormula()
{
    return true;
}

}
