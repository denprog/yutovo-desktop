/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "row.h"
#include "str.h"
#include "document.h"
#include "paragraph.h"

namespace yutovo
{

//Row

Row::Row(Document* _document) :
    Element(_document)
{
    type = ElementType::ROW;
}

Row::Row(Document* _document, Element* _parent) :
    Element(_document)
{
    type = ElementType::ROW;
    parent = _parent;
}

Row::Row(Element* _parent, bool with_string) :
    Element(_parent)
{
    type = ElementType::ROW;
    if (with_string)
        AddEmptyElement();
}

Element* Row::Clone()
{
    return new Row(*this);
}

Element* Row::Create(Element* parent)
{
    return new Row(parent);
}

void Row::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Element::ToJson(value, alloc);

    if (format)
    {
        rapidjson::Value _format_name(format->name.c_str(), alloc);
        value.AddMember("format_name", _format_name, alloc);
    }
}

Element* Row::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    ParagraphFormatPtr f;
    if (value.HasMember("format_name") && value["format_name"].IsString())
        f = document->paragraph_formats->GetFormat(value["format_name"].GetString(), document->config.language);

    Row* row = parent ? new Row(parent, false) : new Row(document);
    row->format = f;
    return row;
}

bool Row::Remake(bool with_elements)
{
    if (document->break_remake)
        return false;

    bool changed = Element::Remake(with_elements);
    int cx = 0;
    int left_m = 0, top_m = 0, right_m = 0, bottom_m = 0;

    ParagraphFormatPtr format = parent->GetParagraphFormat();
    if (type == ElementType::ROW && format->alignment == ParagraphFormat::Alignment::Justify && yutovo::GetChildPos(id) != parent->elements->Count() - 1)
    {
        int page_width = ((Text*)parent->parent)->pixel_size.width;
        int line_width = page_width - format->indent_before - format->indent_after;
        std::map<int, int> elements_spaces;
        int all_spaces = 0;
        int w = 0;
        for (int i = 0; i < elements->Count(); ++i)
        {
            auto el = elements->Get(i);
            el->GetMargin(left_m, top_m, right_m, bottom_m);
            if (el->type == ElementType::STRING) //only strings can be stretched
            {
                ((String*)el.get())->SetStretchWidth(0);
                std::u32string s = el->ToText();
                int spaces = std::count_if(s.begin(), s.end(),
                    [](char32_t c)
                    {
                        return c == U' ';
                    });
                all_spaces += spaces;
                elements_spaces[i] = spaces;
                w += el->rect.width + left_m + right_m;
            }
            else
                w += el->rect.width + left_m + right_m;
        }

        if (!elements_spaces.empty() && all_spaces != 0)
        {
            int k = floor((line_width - w) / all_spaces);
            if (k > 0)
            {
                for (auto s : elements_spaces)
                {
                    auto el = elements->Get(s.first);
                    ((String*)el.get())->SetStretchWidth(k * s.second); //stretch the string proportionaly
                }
            }
            for (int i = 0; i < elements->Count(); ++i)
            {
                auto el = elements->Get(i);
                el->GetMargin(left_m, top_m, right_m, bottom_m);
                el->rect.Move(cx + left_m, 0);
                cx += el->rect.width + left_m + right_m;
            }
        }
    }
    else
    {
        for (uint i = 0; i < elements->Count(); ++i)
        {
            auto el = elements->Get(i);
            el->GetMargin(left_m, top_m, right_m, bottom_m);
            el->rect.Move(cx + left_m, 0);
            cx += el->rect.width + left_m + right_m;
        }
    }
    UpdateRect();

    //align the baseline
    baseline = 0;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (el->baseline > baseline)
            baseline = el->baseline;
    }

    for (uint i = 0; i < elements->Count(); ++i)
    {
        const auto& el = elements->Get(i);
        el->rect.Move(el->rect.left, baseline - el->baseline);
    }

    UpdateRect();

    rect.width += right_m;

    //align top and bottom of the line by the margins of the elements
    int max_top_m = 0, max_bottom_m = 0;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        el->GetMargin(left_m, top_m, right_m, bottom_m);
        if (top_m > 0 && el->rect.top - top_m < 0 && max_top_m < top_m - el->rect.top)
            max_top_m = top_m - el->rect.top;
        if (bottom_m > 0 && el->rect.GetBottom() + bottom_m > rect.height && max_bottom_m < bottom_m - (rect.height - el->rect.GetBottom()))
            max_bottom_m = bottom_m - (rect.height - el->rect.GetBottom());
    }

    rect.height += max_top_m + max_bottom_m;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        el->rect.top += max_top_m;
    }

    baseline += max_top_m;

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

void Row::Normalize()
{
    Element::Normalize();

    if (elements->Count() == 0)
    {
        AddEmptyElement(); //insert empty string
        CaretState c;
        if (GetFirstCaretState(c, nullptr))
            caret->SetState(c);
    }
    else
    {
        for (size_t i = 0; i < elements->Count();)
        {
            auto el = (*elements)[i];
            if (document->IsString(el) && el->can_merge)
            {
                if (el->elements->Count() == 0 && elements->Count() > 1)
                {
                    elements->RemoveAt(i, 1); //remove empty strings
                    document->CaretMoved();
                    if (i > 0)
                        --i;
                    continue;
                }
                if (i < elements->Count() - 1)
                {
                    if (el->Merge(elements->Get(i + 1)))
                    {
                        el->UpdateRect(true);
                        document->CaretMoved();
                        continue;
                    }
                }
            }
            ++i;
        }
    }
}

void Row::MakePlain()
{
    for (size_t i = 0; i < elements->Count();)
    {
        auto el = (*elements)[i];
        if (document->IsString(el) && el->can_merge)
        {
            if (i < elements->Count() - 1 && el->Merge(elements->Get(i + 1)))
                continue;
        }
        ++i;
    }
}

bool Row::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    CaretState caret_state = caret->GetCaretState();
    if (!caret_state.IsInsideElement(id) && caret_state.id != id)
        return parent->InsertElements(_elements, insert_mode, with_undo, changed_element);

    for (auto& el : _elements)
    {
        if (document->IsRow(el))
        {
            if (document->pasting && el->type == ElementType::ROW && type == ElementType::ROW)
            {
                Row* r = (Row*)el.get();
                if (r->format)
                    ((Paragraph*)parent)->format = r->format;
            }
            for (int i = 0; i < el->elements->Count(); ++i)
            {
                std::vector<ElementPtr> v;
                v.push_back(el->elements->Get(i));
                if (!InsertElements(v, i == 0 ? insert_mode : true, with_undo, changed_element))
                    return false;
            }
            changed_element = id;
            return true;
        }
        else if (document->IsParagraph(el) || el->type == ElementType::TEXT) //paragraphs can be inserted above
            return parent->InsertElements(_elements, insert_mode, with_undo, changed_element);
    }

    CaretState c;
    ElementPtr el = document->GetParent(caret_state.id);
    if (caret_state.id == id)
    {
        if (with_undo)
            document->StoreUndo(id);
        for (size_t i = 0; i < _elements.size(); ++i)
        {
            auto ins = _elements[i];
            if (insert_mode)
                elements->Insert(ins, i);
            else
                elements->Replace(ins, i);
            if (i == 0 && elements->Get(i)->AfterInsert(with_undo))
            {
                parent->Normalize();
                continue;
            }
            if (elements->Get(i)->GetLastCaretState(c, nullptr))
                caret->SetState(c);
            parent->Normalize();
        }
    }
    else if (el->id == id)
    {
        if (with_undo)
            document->StoreUndo(id);
        for (size_t i = 0; i < _elements.size(); ++i)
        {
            auto ins = _elements[i];
            uint p = caret_state.GetPosInElement(id);
            if (insert_mode || (p + i == elements->Count()))
                elements->Insert(ins, p + i);
            else
                elements->Replace(ins, p + i);
            if (document->pasting)
            {
                if (ins->HasLastCaretState())
                    caret->SetState(id, p + i + 1, true);
                else if (ins->GetLastCaretState(c, nullptr))
                    caret->SetState(c);
                parent->Normalize();
                continue;
            }
            if (i == 0 && elements->Get(p + i)->AfterInsert(with_undo))
            {
                parent->Normalize();
                continue;
            }
            if (elements->Get(p + i)->GetLastCaretState(c, nullptr))
                caret->SetState(c);
            parent->Normalize();
        }
    }
    else
    {
        if (with_undo)
            document->StoreUndo(parent->id);
        for (size_t i = 0; i < _elements.size(); ++i)
        {
            ElementPtr ins(_elements[i]);
            bool b = false;
            uint p = caret_state.GetElementPos(el->id);
            if (el->GetLastCaretState(c, nullptr) && c == caret_state && !el->IsEmpty())
            {
                elements->Insert(ins, p + i + 1);
                if (i == 0 && !document->pasting)
                    b = ins->AfterInsert(with_undo);
                if (!b)
                {
                    if (ins->HasLastCaretState())
                    {
                        caret->SetState(id, p + i + 2, true);
                    }
                    else
                    {
                        if (elements->Get(p + i + 1)->GetLastCaretState(c, nullptr))
                            caret->SetState(c);
                        else if (i > 0 && _elements[i - 1]->parent->GetLastCaretState(c, nullptr))
                            caret->SetState(c);
                    }
                }
                if (elements->Count() > p + i + 2)
                {
                    ElementPtr el1 = elements->Get(p + i + 1);
                    ElementPtr el2 = elements->Get(p + i + 2);
                    el1->Merge(el2);
                }
            }
            else if (el->GetFirstCaretState(c, nullptr) && c == caret_state)
            {
                bool empty = false;
                if (insert_mode)
                {
                    elements->Insert(ins, p + i);
                    if (elements->Get(p + i + 1)->IsEmpty())
                    {
                        if (document->IsString(elements->Get(p + i)) && document->IsString(elements->Get(p + i + 1)))
                            elements->Get(p + i)->can_merge = elements->Get(p + i + 1)->can_merge;
                        b = true;
                        empty = true;
                    }
                }
                else
                {
                    el->SplitAt(caret_state.GetPos() + 1);
                    elements->Replace(ins, p + i);
                }
                if (i == 0 && !document->pasting)
                    b = ins->AfterInsert(with_undo);
                if (!b)
                {
                    if (elements->Get(elements->Count() > p + i + 1 ? p + i + 1 : p + i)->GetFirstCaretState(c, nullptr))
                        caret->SetState(c);
                    else if (document->pasting && ins->GetLastCaretState(c, nullptr))
                        caret->SetState(c);
                }
                if (empty && elements->Count() > p + i + 1)
                    elements->RemoveAt(p + i + 1, 1);
            }
            else
            {
                if (el->SplitAt(caret_state.GetPos()))
                {
                    if (insert_mode)
                        elements->Insert(ins, p + i + 1);
                    else
                    {
                        elements->Get(p + i + 1)->SplitAt(1);
                        elements->Replace(ins, p + i + 1);
                    }
                    if (i == 0 && !document->pasting)
                        b = ins->AfterInsert(with_undo);
                    if (!b)
                    {
                        if (p + i + 1 < elements->Count() - 1 && elements->Get(p + i + 2)->GetFirstCaretState(c, nullptr))
                            caret->SetState(c);
                        else if (GetLastCaretState(c, nullptr))
                            caret->SetState(c);
                    }
                }
            }
        }
        Normalize();
    }

    changed_element = id;

#ifdef DEBUG
    to_str = ToText();
#endif
    return true;
}

bool Row::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    CaretState before_state = caret->GetCaretState();
    if (selection->IsEmpty())
    {
        if (!left)
        {
            if (before_state.GetPos() == 0 && elements->Count() == 1 && document->IsString(elements->Get(0)) && elements->Get(0)->elements->Count() == 0)
            {
                return parent->DeleteElements(left, with_undo, changed_element);
            }
        }

        CaretState first_state, last_state;
        GetFirstCaretState(first_state, nullptr);
        GetLastCaretState(last_state, nullptr);
        if ((left && before_state == first_state) || (!left && before_state == last_state))
            return parent->DeleteElements(left, with_undo, changed_element);

        if (left)
        {
            int p = elements->GetElementPos(document->GetParent(before_state.id)->id);
            if (p > 0)
            {
                auto el = elements->Get(p - 1);
                if (el->CanContinueSelection())
                {
                    CaretState c;
                    if (el->GetLastCaretState(c, nullptr))
                    {
                        caret->SetState(c);
                        el->DeleteElements(left, with_undo, changed_element);
                        changed_element = id;
#ifdef DEBUG
                        to_str = ToText();
#endif
                        return true;
                    }
                }
                else
                {
                    if (with_undo)
                        document->StoreUndo(id);
                    elements->RemoveAt(p - 1, 1);
                    Normalize();
                    changed_element = id;
#ifdef DEBUG
                    to_str = ToText();
#endif
                    return true;
                }
            }
            else
            {
                p = elements->GetElementPos(before_state.id);
                if (p > 0 || before_state == last_state)
                {
                    if (before_state != last_state)
                        --p;
                    auto el = elements->Get(p);
                    if (el->CanContinueSelection())
                    {
                        CaretState c;
                        if (!el->GetLastCaretState(c, nullptr))
                            return false;
                        caret->SetState(c);
                        return el->DeleteElements(left, with_undo, changed_element);
                    }
                    if (with_undo)
                        document->StoreUndo(id);
                    elements->RemoveAt(p, 1);

                    if (elements->Count() == 0)
                        Normalize();
                    changed_element = id;
#ifdef DEBUG
                    to_str = ToText();
#endif
                    return true;
                }
                return parent->DeleteElements(left, with_undo, changed_element);
            }
        }
        else
        {
            int p = elements->GetElementPos(before_state.id);
            if (p == -1)
                p = elements->GetElementPos(document->GetParent(before_state.id)->id);
            if (p < elements->Count())
            {
                auto el = elements->Get(p);
                if (el && document->IsString(el) && el->IsEmpty())
                {
                    if (with_undo)
                        document->StoreUndo(id);
                    elements->RemoveAt(p, 1);
                }
                else
                {
                    if (before_state.GetParent() != id)
                    {
                        ++p;
                        el = elements->Get(p);
                    }
                    if (el && el->CanContinueSelection())
                    {
                        CaretState c;
                        if (el->GetFirstCaretState(c, nullptr))
                        {
                            caret->SetState(c);
                            el->DeleteElements(left, with_undo, changed_element);
                            changed_element = id;
#ifdef DEBUG
                            to_str = ToText();
#endif
                            return true;
                        }
                    }
                    else if (p < elements->Count())
                    {
                        if (with_undo)
                            document->StoreUndo(id);
                        elements->RemoveAt(p, 1);
                    }
                }
            }
            else if (p >= 0)
            {
                if (with_undo)
                    document->StoreUndo(id);
                elements->RemoveAt(p, 1);
            }

            Normalize();
        }

        changed_element = id;

#ifdef DEBUG
        to_str = ToText();
#endif
        return true;
    }
    else
    {
        uint start, size;
        if (selection->Has(id, start, size))
        {
            if (with_undo)
                document->StoreUndo(parent->id);
            elements->RemoveAt(start, size);
            Normalize();
            changed_element = id;
#ifdef DEBUG
            to_str = ToText();
#endif
            return true;
        }
    }

#ifdef DEBUG
    to_str = ToText();
#endif
    return false;
}

bool Row::ChangeStringFormat(const StringFormatPtr format, bool with_undo, ElementId& changed_element)
{
    Element::ChangeStringFormat(format, with_undo, changed_element);
    
    bool changed = false;
    SelectionState s = selection->GetState();
    for (auto& t : s.state)
    {
        if (!IsChild(id, t.id))
            continue;
        ElementPtr el = document->GetElement(t.id);
        if (el->SplitAt(t.start))
            el = elements->Get(elements->GetElementPos(el->id) + 1);
        if (el)
        {
            el->SplitAt(t.size);
            if (el->ChangeStringFormat(format, with_undo, changed_element) && !changed)
                changed = true;
        }
    }

    changed_element = id;

#ifdef DEBUG
    to_str = ToText();
#endif
    return changed;
}

bool Row::GetBeginCaretState(CaretState& caret_state, Selection* select)
{
    if (!select)
        return GetFirstCaretState(caret_state, nullptr);
    if (!GetFirstCaretState(caret_state, nullptr))
        return false;
    CaretState c = caret->GetCaretState();
    int p1 = caret_state.GetPosInElement(id);
    int p2 = c.GetPosInElement(id);
    if (p2 != 0)
        select->Add(id, p1, p2);
    return true;
}

bool Row::GetEndCaretState(CaretState& caret_state, Selection* select)
{
    if (!select)
        return GetLastCaretState(caret_state, nullptr);
    if (!GetLastCaretState(caret_state, nullptr))
        return false;
    CaretState c = caret->GetCaretState();
    int p1 = c.GetPosInElement(id);
    int p2 = caret_state.GetPosInElement(id);
    ElementSelection s;
    if (IsDirectChild(id, c.id) && !select->Has(GetChild(id, p1), s) && c != caret_state)
        select->Add(id, p1, 1);
    if (caret_state.last_pos && p2 - p1 > 1 && p2 <= elements->Count())
        select->Add(id, p1 + 1, p2 - p1 - 1);
    else if (p2 < elements->Count() && p2 - p1 > 0)
        select->Add(id, p1 + 1, p2 - p1);
    return true;
}

bool Row::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (!select)
        return Element::GetTopCaretState(x, y, caret_state, select);
    
    CaretState next, last;
    if (y < GetAbsoluteRect().GetBottom())
    {
        if (select)
        {
            //select this row from start until caret pos
            if (GetFirstCaretState(next, nullptr))
            {
                if (GetLastCaretState(last, nullptr) && last == next)
                {
                    ElementPtr el = document->GetElement(next.id);
                    if (el)
                    {
                        CaretState c = caret->GetCaretState();
                        el->GetLastCaretState(next, nullptr);
                        caret_state = next;
                        caret->SetState(next, false);
                    }
                }
                else
                {
                    CaretState c;
                    last = caret->GetCaretState();
                    while (next < last)
                    {
                        ElementPtr el = document->GetElement(next.id);
                        if (document->IsString(el))
                        {
                            if (el->GetLastCaretState(c, nullptr) && c == next)
                            {
                                ElementPtr _el = document->GetElement(last.id);
                                if (_el && _el->HasCaretState() && el->GetRightCaretState(c, nullptr) && c == last)
                                    break;
                            }
                        }
                        if (!el || !el->GetRightCaretState(next, select))
                            break;
                    }
                    GetFirstCaretState(next, nullptr);
                    caret_state = next;
                    caret->SetState(next, false);
                }
            }
        }
        return parent->GetTopCaretState(x, y, caret_state, select);
    }

    if (!GetFirstCaretState(next, nullptr))
        next.SetState(id);
    if (!GetLastCaretState(last, nullptr))
        last.SetState(id);

    caret_state = next;
    Rect r = document->GetCaretRect(next);
	int min_dist = r.DistToPoint(x, y);

    CaretState c = caret->GetCaretState();
    if (next == last && !c.IsInsideElement(id) && select)
    {
        select->Add(parent->id);
    }
    else
    {
        while (next != last)
        {
            ElementPtr el = document->GetParent(next.id);
            if (!el->GetRightCaretState(next, nullptr))
                break;
            
            r = document->GetCaretRect(next);
            int dist = r.DistToPoint(x, y);
            if (dist < min_dist)
            {
                min_dist = dist;
                caret_state = next;
            }
        }
    }

    GetLastCaretState(next, nullptr);
    while (caret_state < next)
    {
        ElementPtr el = document->GetParent(next.id);
        if (!el->GetLeftCaretState(next, select))
            break;
    }

    return true;
}

bool Row::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (!select)
        return Element::GetBottomCaretState(x, y, caret_state, select);

    CaretState next, last;
    if (y > GetAbsoluteRect().top)
    {
        if (select)
        {
            //select this row from caret pos until the end
            if (GetLastCaretState(last, nullptr))
            {
                if (GetFirstCaretState(next, nullptr) && last == next)
                {
                    if (parent->parent->elements->IsLast(parent->id) && parent->parent->type != ElementType::CODE_BLOCK)
                        return parent->GetBottomCaretState(x, y, caret_state, nullptr);
                    ElementPtr el = document->GetElement(next.id);
                    if (el)
                    {
                        CaretState c = caret->GetCaretState();
                        if (parent->parent->elements->IsLast(parent->id) && parent->parent->type == ElementType::CODE_BLOCK)
                            select->Add(parent->parent->id);
                        else if (parent->parent->elements->IsFirst(parent->id) || c.IsInsideElement(id))
                            select->Add(parent->id);
                        else
                            el->GetLastCaretState(next, nullptr);
                        caret_state = next;
                        caret->SetState(next, false);
                    }
                }
                else
                {
                    next = caret->GetCaretState();
                    while (next < last)
                    {
                        ElementPtr el = document->GetElement(next.id);
                        if (!el || !el->GetRightCaretState(next, select))
                            break;
                    }
                    if (parent && parent->parent && parent->type == ElementType::CODE_PARAGRAPH && parent->parent->elements->IsLast(parent->id) && 
                        !select->IsSelected(parent->parent->id))
                        select->Add(parent->parent->id);
                    caret_state = next;
                    caret->SetState(next, false);
                }
            }
        }
        return parent->GetBottomCaretState(x, y, caret_state, select);
    }

    if (!GetFirstCaretState(next, nullptr))
        next.SetState(id);
    if (!GetLastCaretState(last, nullptr))
        last.SetState(id);

    caret_state = next;
    Rect r = document->GetCaretRect(next);
	int min_dist = r.DistToPoint(x, y);

    while (next != last)
    {
        ElementPtr el = document->GetParent(next.id);
        if (!el->GetRightCaretState(next, nullptr))
            break;
        
        Rect r = document->GetCaretRect(next);
        int dist = r.DistToPoint(x, y);
        if (dist < min_dist)
        {
            min_dist = dist;
            caret_state = next;
        }
    }

    GetFirstCaretState(next, nullptr);
    while (next < caret_state)
    {
        ElementPtr el = document->GetElement(next.id);
        if (!el || !el->GetRightCaretState(next, select))
            break;
    }

    caret_state = next;

    return true;
}

bool Row::GetWordLeftCaretState(CaretState& caret_state, Selection* select)
{
    CaretState c;
    if (elements->Count() > 0 && elements->Get(elements->Count() - 1)->type != ElementType::STRING)
    {
        if (GetLastCaretState(c, nullptr))
        {
            if (c == caret_state)
            {
                int p = elements->Count() - 1;
                if (p < 0)
                {
                    if (!GetFirstCaretState(c, nullptr))
                        return false;
                    caret_state = c;
                    return true;
                }
                auto el = elements->Get(p);
                while (p >= 0 && el->type != ElementType::STRING && !el->HasCaretState())
                    --p;
                if (p < 0)
                {
                    if (!GetFirstCaretState(c, nullptr))
                        return false;
                    caret_state = c;
                    return true;
                }
                if (select)
                    select->Add(id, p, elements->Count() - p);
                caret_state.SetState(id, p);
                return true;
            }
        }
    }
    if (GetFirstCaretState(c, nullptr) && c == caret_state)
    {
        int p = parent->elements->GetChildPos(id);
        if (type == ElementType::ROW && p > 0)
        {
            //move to the previous row
            if (parent->elements->Get(p - 1)->GetLastCaretState(caret_state, select))
                return true;
        }
        else if (!select)
        {
            p = yutovo::GetChildPos(parent->id);
            if (p > 0)
            {
                //move to the previous paragraph
                if (parent->parent->elements->Get(p - 1)->GetLastCaretState(c, nullptr))
                {
                    caret_state = c;
                    return true;
                }
            }
        }
    }
    return Element::GetWordLeftCaretState(caret_state, select);
}

bool Row::GetWordRightCaretState(CaretState& caret_state, Selection* select)
{
    CaretState c;
    int p = yutovo::GetChildPos(id, caret_state.id);
    if (p < elements->Count())
    {
        if (elements->Get(p)->HasCaretState() && p < elements->Count() - 1)
        {
            if (select)
                select->Add(id, p, 1);
            if (elements->Get(p + 1)->HasCaretState())
                caret_state.SetState(id, p + 1);
            else
            {
                if (elements->Get(p + 1)->GetFirstCaretState(c, nullptr))
                    caret_state = c;
            }
            return true;
        }
        else if (elements->Get(p)->HasLastCaretState())
        {
            if (select)
                select->Add(id, p, 1);
            caret_state.SetState(id, p + 1, true);
            return true;
        }
        else if (select && p + 1 < elements->Count() && document->IsString(elements->Get(p + 1)))
        {
            if (elements->Get(p + 1)->GetFirstCaretState(c, nullptr) && elements->Get(p + 1)->GetWordRightCaretState(c, select))
            {
                caret_state = c;
                return true;
            }
        }
        else if (p + 2 < elements->Count() && select)
        {
            if (elements->Get(p + 2)->HasLastCaretState())
            {
                select->Add(id, p + 1, 1);
                caret_state.SetState(id, p + 2, true);
                return true;
            }
            if (elements->Get(p + 2)->GetFirstCaretState(c, nullptr))
            {
                select->Add(id, p + 1, 1);
                caret_state = c;
                return true;
            }
        }
        else if (p + 1 < elements->Count())
        {
            if (select && elements->Get(p + 1)->HasLastCaretState())
            {
                select->Add(id, p + 1, 1);
                caret_state.SetState(id, p + 2, true);
                return true;
            }
            if (elements->Get(p + 1)->GetFirstCaretState(c, nullptr) && elements->Get(p + 1)->GetWordRightCaretState(c, select))
            {
                caret_state = c;
                return true;
            }
        }
        else
        {
            if (type == ElementType::ROW && parent->elements->GetChildPos(id) < parent->elements->Count() - 1)
            {
                //move to the next row
                if (parent->elements->Get(yutovo::GetChildPos(id) + 1)->GetFirstCaretState(caret_state, select))
                    return true;
            }
            else if (!select)
            {
                p = yutovo::GetChildPos(parent->id);
                if (p < parent->parent->elements->Count() - 1)
                {
                    //move to the next paragraph
                    if (parent->parent->elements->Get(p + 1)->GetFirstCaretState(c, nullptr))
                    {
                        caret_state = c;
                        return true;
                    }
                }
            }
        }
    }
    if (GetLastCaretState(c, nullptr) && c == caret_state)
    {
        int p = parent->elements->GetChildPos(id);
        if (type == ElementType::ROW && p < parent->elements->Count() - 1)
        {
            //move to the next row
            if (parent->elements->Get(p + 1)->GetFirstCaretState(caret_state, select))
                return true;
        }
        else if (!select)
        {
            p = yutovo::GetChildPos(parent->id);
            if (p < parent->parent->elements->Count() - 1)
            {
                //move to the next paragraph
                if (parent->parent->elements->Get(p + 1)->GetFirstCaretState(c, nullptr))
                {
                    caret_state = c;
                    return true;
                }
            }
        }
    }
    return Element::GetWordRightCaretState(caret_state, select);
}

bool Row::GetNearestCaretState(const int x, const int y, CaretState& caret_state)
{
    int min_dist = std::numeric_limits<int>::max();
    int dist;
    ElementPtr el;
    CaretState next, last;
    for (int i = 0; i < elements->Count(); ++i)
    {
        auto _el = elements->Get(i);
        if (_el->type == ElementType::CODE_BLOCK)
        {
            Rect r = _el->GetAbsoluteRect();
            if (r.IsPointInside(x, y))
            {
                min_dist = 0;
                el = _el;
                break;
            }
        }
        if (!_el->GetFirstCaretState(next, nullptr) || !_el->GetLastCaretState(last, nullptr))
        {
            if (_el->HasCaretState())
            {
                Rect r = _el->GetAbsoluteRect();
                if (r.IsPointInside(x, y))
                {
                    caret_state.SetState(_el->id);
                    return true;
                }
            }
            continue;
        }
        Rect r1 = document->GetCaretRect(next);
        Rect r2 = document->GetCaretRect(last);
        Rect r{r1.left, r1.top, r2.left - r1.left + r2.width, r2.top - r1.top + r2.height};
        if (r.IsPointInside(x, y))
        {
            min_dist = 0;
            el = _el;
            break;
        }
        else
        {
            dist = r1.DistToPoint(x, y);
            if (dist < min_dist)
            {
                min_dist = dist;
                el = _el;
            }
            dist = r2.DistToPoint(x, y);
            if (dist < min_dist)
            {
                min_dist = dist;
                el = _el;
            }
        }
    }

    if (min_dist > 0)
    {
        int dist1 = std::numeric_limits<int>::max(), dist2 = std::numeric_limits<int>::max();
        if (GetFirstCaretState(next, nullptr))
        {
            Rect r = document->GetCaretRect(next);
            dist1 = r.DistToPoint(x, y);
        }
        if (GetLastCaretState(last, nullptr))
        {
            Rect r = document->GetCaretRect(last);
            dist2 = r.DistToPoint(x, y);
        }
        if (dist1 < min_dist && dist1 < dist2)
        {
            if (!el)
            {
                caret_state = next;
                return true;
            }
            if (!el->GetNearestCaretState(x, y, caret_state))
                caret_state = next;
            return true;
        }
        if (dist2 < min_dist)
        {
            caret_state = last;
            return true;
        }
    }

    if (!el)
    {
        if (!GetFirstCaretState(next, nullptr) || !GetLastCaretState(last, nullptr))
            return false;
        Rect r = document->GetCaretRect(next);
        min_dist = r.DistToPoint(x, y);
        r = document->GetCaretRect(last);
        dist = r.DistToPoint(x, y);
        if (dist < min_dist)
            caret_state = last;
        else
            caret_state = next;
        return true;
    }
    return el->GetNearestCaretState(x, y, caret_state);
}

Rect Row::GetCaretRect(const uint pos) const
{
    if (pos > 0 && pos == elements->Count() && elements->Get(pos - 1)->type == ElementType::CODE_BLOCK)
    {
        Rect& rect = elements->Get(pos - 1)->rect;
        return Rect{rect.GetRight() + 1, rect.top - 1, 2, rect.height + 2};
    }
    return Element::GetCaretRect(pos);
}

void Row::DrawCaret(const uint pos) const
{
    // if (parent->document->config.hilight_caret_element)
    //     parent->DrawHilightRect();
    if (pos > 0 && pos == elements->Count() && elements->Get(pos - 1)->type == ElementType::CODE_BLOCK)
    {
        Rect r = GetAbsoluteRect(GetCaretRect(pos));
        window->DrawLine(r.GetRight() - 1, r.top + 1, r.GetRight() - 1, r.GetBottom() - 2, Color::Black());
        return;
    }
    return Element::DrawCaret(pos);
}

bool Row::CanContinueSelection()
{
    return true;
}

void Row::AddEmptyElement()
{
    AddElement(ElementPtr(new String(this)));
}

bool Row::IsEmpty() const
{
    if (elements->Count() != 1)
        return false;
    if (!document->IsString(elements->Get(0)))
        return false;
    return elements->Get(0)->elements->Count() == 0;
}

}
