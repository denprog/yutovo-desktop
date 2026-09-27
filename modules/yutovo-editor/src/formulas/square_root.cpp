/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "square_root.h"
#include "code_row.h"
#include "code_paragraph.h"
#include "../str.h"
#include "../document.h"

namespace yutovo
{

//SquareRoot

SquareRoot::SquareRoot(Element* _parent, bool with_init) :
    Formula(_parent)
{
    type = ElementType::SQUARE_ROOT;
    if (with_init)
        Init();
}

SquareRoot::SquareRoot(Document* _document, bool with_init) :
    Formula(_document)
{
    type = ElementType::SQUARE_ROOT;
    if (with_init)
        Init();
}

SquareRoot::SquareRoot(const SquareRoot& source) :
    Formula(source),
    shape((Shape*)elements->Get(0).get()),
    last(elements->Get(1).get())
{
}

void SquareRoot::Init()
{
    type = ElementType::SQUARE_ROOT;
    shape = new Shape(this);
    last = new CodeRow<>(this, true);
    elements->Add(ElementPtr(shape));
    elements->Add(ElementPtr(last));
}

Element* SquareRoot::Clone()
{
    return new SquareRoot(*this);
}

Element* SquareRoot::Create(Element* _parent)
{
    return new SquareRoot(_parent);
}

Element* SquareRoot::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new SquareRoot(parent, false);
    return new SquareRoot(document, false);
}

bool SquareRoot::AfterFromJson()
{
    if (elements->Count() != 2)
        return false;
    shape = (Shape*)elements->Get(0).get();
    last = (CodeRow<>*)elements->Get(1).get();
    return true;
}

void SquareRoot::Draw() const
{
    shape->draw_func = 
        [&](const Rect& r)
        {
            std::list<Point> path;
            path.push_back(Point{(int)lround(r.left + r.width * 0.722), r.GetBottom()});
            path.push_back(Point{(int)lround(r.left + r.width * 0.281), (int)lround(r.top + r.height * 0.495)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.061), (int)lround(r.top + r.height * 0.544)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.075), (int)lround(r.top + r.height * 0.510)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.343), (int)lround(r.top + r.height * 0.429)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.703), (int)lround(r.top + r.height * 0.869)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.934), r.top});
            path.push_back(Point{r.GetRight() + GetLast()->rect.width, r.top + 1});
            path.push_back(Point{r.GetRight() + GetLast()->rect.width, (int)lround(r.top + r.height * 0.01 + 1)});
            path.push_back(Point{r.GetRight(), (int)lround(r.top + r.height * 0.01 + 1)});
            window->DrawFillPath(path, document->selection.IsSelected(id) ? document->config.formula_bg_color : document->config.shapes_color);
        };
    
    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, document->config.bg_selection_color);
    }

    Formula::Draw();
}

bool SquareRoot::Remake(bool with_elements)
{
    bool changed = Formula::Remake(with_elements);

    //recalc shape rect
    shape->rect.SetRect(0, 4, round(last->rect.height * 5 / 11), last->rect.height + ROOT_Y_OFFSET * 2 + 4);

    //recalc item's rect
    int x_offset = ROOT_X_LEFT_OFFSET + shape->rect.width;
    int y_offset = 0;
    if (shape->rect.height > last->rect.height)
        y_offset = ROOT_Y_OFFSET + round((shape->rect.height - last->rect.height) / 2);
    else
        y_offset = ROOT_Y_OFFSET + round(shape->rect.height * 0.01);
    last->rect.Move(x_offset, y_offset + 4);
    baseline = last->rect.top + last->baseline;

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

bool SquareRoot::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    if (caret->GetPos() == 0 && caret->GetElement()->id == id)
        return false;
    return Formula::InsertElements(_elements, insert_mode, with_undo, changed_element);
}

bool SquareRoot::AfterInsert(bool with_undo)
{
    if (!selection->IsEmpty())
    {
        ElementSelection& select = selection->selection[0];
        if (select.start > 0 && document->IsString(select.element))
            select.element->SplitAt(select.start);

        //move the selected elements in the GetLast() element
        GetLast()->elements->RemoveAt(0, 1);
        for (int i = selection->selection.size() - 1; i >= 0; --i)
        {
            ElementSelection el_s = selection->selection[i];
            for (int j = el_s.size - 1; j >= 0; --j)
                GetLast()->elements->Move(document->GetElement(GetChild(el_s.element->id, el_s.start + j)), 0);
        }

        selection->Clear();
        CaretState c;
        if (GetLast()->GetFirstCaretState(c, nullptr))
            caret->SetState(c);

        UpdateLevel(level);
        parent->parent->Normalize();
        return true;
    }

    CaretState c;
    last->GetFirstCaretState(c, nullptr);
    caret->SetState(c);
    return true;
}

bool SquareRoot::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    uint start, size;
    if (selection->Has(id, start, size))
    {
        if (start == 1 && size == 1)
        {
            if (with_undo)
                document->StoreUndo(id);
            last->elements->Clear();
            Normalize();
            caret->SetState(last->elements->Get(0)->id);
            selection->Clear();
            changed_element = id;
            return true;
        }
        return false;
    }
    else if (!selection->IsEmpty() || left || caret->GetPos() != 0)
        return false;
    
    if (with_undo)
        document->StoreUndo(parent->id);

    //remove this element by deleting its shape
    int p = parent->elements->GetElementPos(id);
    uint c1 = elements->Get(1)->elements->Count();

    caret->SetState(id);
    parent->elements->Move(*elements->Get(1)->elements, p);
    CaretState c;
    if (parent->elements->Get(p)->GetFirstCaretState(c, nullptr))
        caret->SetState(c);

    auto t = parent->elements->Get(p + c1); //for not removing this element until this function ends
    parent->elements->Remove(id);
    parent->Normalize();
    changed_element = id;
    return true;
}

bool SquareRoot::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (select && caret->IsOnElement(shape->id))
        return false;
    return Formula::GetLeftCaretState(caret_state, select);
}

bool SquareRoot::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    if (select && caret->IsOnElement(shape->id))
        return false;
    return Formula::GetRightCaretState(caret_state, select);
}

bool SquareRoot::UseSelection()
{
    return true;
}

std::string SquareRoot::ToHtml() const
{
    std::string s = "<msqrt>";
    s += GetLast()->ToHtml();
    s += "</msqrt>";
    return s;
}

std::u32string SquareRoot::ToText() const
{
    if (!GetLast())
        return U"";
    return U"sqrt(" + GetLast()->ToText() + U")";
}

void SquareRoot::ToParserString(ParserString& str)
{
    str.Add(id, U"sqrt(");
    GetLast()->ToParserString(str);
    str.Add(id, U")");
}

CodeRow<>* SquareRoot::GetLast() const
{
    return (CodeRow<>*)elements->Get(1).get();
}

}
