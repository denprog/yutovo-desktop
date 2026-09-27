/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "element.h"
#include "document.h"
#include "caret.h"
#include "selection.h"
#include <assert.h>
#include <limits>
#include <map>

#ifdef min
#undef min
#endif

#ifdef _MSC_VER
#undef GetObject
#endif

namespace yutovo
{

//Element

Element::Element(Document* _document) :
    parent(nullptr),
    document(_document),
    window(_document->window),
    caret(document->caret),
    selection(&document->selection),
    elements(new Elements(this)),
    logger(document->logger)
{
}

Element::Element(Element* _parent) :
    parent(_parent),
    document(parent ? parent->document : nullptr), //parent == null when pasting from clipboard
    window(document ? document->window : nullptr),
    caret(document ? document->caret : nullptr),
    selection(document ? &document->selection : nullptr),
    elements(new Elements(this))
{
    if (document)
        logger = document->logger;
    else if (parent)
        logger = parent->logger;
}

Element::Element(const Element& source) :
    parent(source.parent),
    document(source.document),
    window(source.window),
    type(source.type),
    id(source.id),
    logical_id(source.logical_id),
    level(source.level),
    editable(source.editable),
    caret(document->caret),
    selection(&document->selection),
    remake_always(source.remake_always),
    can_merge(source.can_merge),
    error_mark(source.error_mark),
    can_move_picture(source.can_move_picture),
    can_resize(source.can_resize),
    visible(source.visible),
    has_caret_hilight(source.has_caret_hilight),
    has_frame_hilight(source.has_frame_hilight),
    logger(source.logger)
{
    elements.reset(source.elements->Clone(this)); //deep copy

#ifdef DEBUG
    to_str = ToText();
#endif
}

Element::~Element()
{
}

void Element::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    rapidjson::Value _id(ElementIdToString(id).c_str(), alloc);
    value.AddMember("id", _id, alloc);
    value.AddMember("type", (int)type, alloc);
    value.AddMember("level", level, alloc);
    elements->ToJson(value, alloc);
}

bool Element::AfterFromJson()
{
    return true;
}

bool Element::Copy(std::vector<ElementPtr>& copy)
{
    uint start, size;
    if (!selection->Has(id, start, size))
        return false;
    if (start == 0 && size == elements->Count())
    {
        copy.push_back(ElementPtr(Clone()));
        return true;
    }
    for (uint i = start; i < start + size; ++i)
        copy.push_back(ElementPtr(elements->Get(i)->Clone()));
    return true;
}

void Element::SetDocument(Document* _document)
{
    document = _document;
    window = _document->window;
    caret = _document->caret;
    logger = _document->logger;
    for (int i = 0; i < elements->Size(); ++i)
        elements->Get(i)->SetDocument(_document);
}

void Element::Draw() const
{
    if (!IsVisible())
        return;
    elements->Draw();

    if (document->caret_hilight_id == id)
    {
        Rect r = GetAbsoluteRect();
        window->DrawRect(r.left, r.top, r.width, r.height, document->config.hilight_color);
    }
    
    int start, size;
    if (document->HasErrorMark(id, start, size))
    {
        int p_start, p_size;
        if (parent && document->HasErrorMark(parent->id, p_start, p_size))
        {
            int p = yutovo::GetChildPos(id);
            if (p >= p_start && p <= p_start + p_size)
            {
                parent->DrawErrorMark(p_start, p_size);
                return;
            }
        }
        DrawErrorMark(start, size);
    }
}

void Element::DrawErrorMark(const int start, const int size) const
{
    Rect r1 = elements->Get(start)->GetAbsoluteRect();
    Rect r2 = elements->Get(start + (size > 0 ? size - 1 : 0))->GetAbsoluteRect();
    window->DrawWavyLine(r1.left, r1.GetBottom() > r2.GetBottom() ? r1.GetBottom() : r2.GetBottom() - 2, 
        r2.left + r2.width - r1.left, 1, document->config.error_marks_color);
}

bool Element::Remake(bool with_elements)
{
    if (document->break_remake || !visible)
        return false;

    bool changed = false;
    if (with_elements)
    {
        for (int i = 0; i < elements->Count(); ++i)
        {
            if (document->break_remake)
                return false;
            auto el = elements->Get(i);
            if (!el->visible)
                continue;
            bool r = el->Remake(true);
            if (!changed && r)
                changed = true;
        }
    }
    for (int i = 0; i < elements->Count(); ++i)
    {
        if (document->break_remake)
            return false;
        auto el = elements->Get(i);
        if (!el->visible)
            continue;
        if (el->remake_always)
        {
            bool r = el->Remake(false);
            if (!changed && r)
                changed = true;
        }
    }
    UpdateRect();
    Solve();
    return changed;
}

void Element::Normalize()
{
    if (!visible)
        return;
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->Normalize();
}

void Element::Rescale() const
{
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->Rescale();
}

void Element::MakePlain()
{
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->MakePlain();
}

void Element::Resize(const int dx, const int dy)
{
    if (parent)
        parent->Resize(dx, dy);
}

void Element::MovePicture(const int dx, const int dy)
{
    if (parent)
        parent->MovePicture(dx, dy);
}

void Element::ZoomPicture(const int pixels)
{
    if (parent)
        parent->ZoomPicture(pixels);
}

bool Element::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    auto c = caret->GetCaretState();
    if (c.IsInsideElement(id))
    {
        int pos = c.GetPosInElement(id);
        for (int i = 0; i < _elements.size(); ++i)
            elements->Insert(_elements[i], pos + i);
        changed_element = id;
        return true;
    }
    else if (c.id == id)
    {
        for (int i = 0; i < _elements.size(); ++i)
            elements->Insert(_elements[i], i);
        changed_element = id;
        return true;
    }
    return false;
}

bool Element::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    if (selection->IsEmpty())
    {
        if ((left && caret->GetPos() == 0) || (!left && caret->GetPos() == elements->Count()))
            return parent->DeleteElements(left, with_undo, changed_element);
        elements->RemoveAt(left ? caret->GetPos() - 1 : caret->GetPos(), 1);

        changed_element = id;

#ifdef DEBUG
        to_str = ToText();
#endif
        return true;
    }

    uint start, size;
    if (selection->Has(id, start, size))
    {
        if (with_undo)
            document->StoreUndo(id);

        elements->RemoveAt(start, size);
        Normalize();
        CaretState c;
        if (elements->Count() == 0)
        {
            if (GetFirstCaretState(c, nullptr))
                caret->SetState(c);
            else
                caret->SetState(id);
        }
        else
        {
            if (start + size < elements->Count() && elements->Get(start)->GetFirstCaretState(c, nullptr))
                caret->SetState(c);
            else if (start > 0 && start < elements->Count() && elements->Get(start)->GetFirstCaretState(c, nullptr))
                caret->SetState(c);
            else if (start > 0 && GetLastCaretState(c, nullptr))
                caret->SetState(c);
            else if (GetFirstCaretState(c, nullptr))
                caret->SetState(c);
            else
                caret->SetState(id);
        }

        changed_element = id;

#ifdef DEBUG
        to_str = ToText();
#endif
        return true;
    }

#ifdef DEBUG
    to_str = ToText();
#endif
    return false;
}

bool Element::ChangeStringFormat(const StringFormatPtr format, bool with_undo, ElementId& changed_element)
{
    bool changed = false;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (el->ChangeStringFormat(format, with_undo, changed_element) && !changed)
            changed = true;
    }
    return changed;
}

bool Element::ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo, ElementId& changed_element)
{
    if (!parent)
        return false;
    return parent->ChangeParagraphFormat(format, with_undo, changed_element);
}

bool Element::Split(const uint width, bool split_more)
{
    return false;
}

bool Element::SplitAt(const uint pos)
{
    if (elements->Count() <= pos)
        return false;
    ElementPtr part(Create(parent));
    part->elements->Clear();
    int p = parent->elements->GetElementPos(id);
    parent->elements->Insert(part, p + 1);
    for (int i = pos; i < elements->Count();)
        part->elements->Move(elements->Get(i), part->elements->Count());
    return true;
}

bool Element::Merge(const ElementPtr with_element)
{
    return false;
}

bool Element::CanMerge(const ElementPtr with_element)
{
    return can_merge;
}

void Element::UpdateStringFormat(const StringFormatPtr base_format, const StringFormatPtr new_format)
{
    for (int i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (el->type != ElementType::CODE_BLOCK)
            el->UpdateStringFormat(base_format, new_format);
    }
}

bool Element::AfterInsert(bool with_undo)
{
    return false;
}

void Element::AfterChildInsert(const ElementId child_id, bool with_undo)
{
    if (parent)
        parent->AfterChildInsert(child_id, with_undo);
}

void Element::BeforeDelete()
{
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->BeforeDelete();
}

void Element::BeforeReplace()
{
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->BeforeReplace();
}

void Element::AfterReplace()
{
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->AfterReplace();
}

void Element::BeforePaste()
{
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->BeforePaste();
}

void Element::LogicalIdChanged(const LogicalId& last_id)
{
}

bool Element::GetFirstCaretState(CaretState& caret_state, Selection* select)
{
    if (!IsVisible())
        return false;
    return elements->GetFirstCaretState(caret_state, select);
}

bool Element::GetLastCaretState(CaretState& caret_state, Selection* select)
{
    if (!IsVisible())
        return false;
    return elements->GetLastCaretState(caret_state, select);
}

bool Element::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (!IsVisible())
        return false;
    if (caret_state.IsInsideElement(id))
    {
        if (elements->GetLeftCaretState(caret_state, select))
            return true;
    }
    if (parent)
        return parent->GetLeftCaretState(caret_state, select);
    return false;
}

bool Element::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    if (!IsVisible())
        return false;
    if (caret_state.IsInsideElement(id))
    {
        if (elements->GetRightCaretState(caret_state, select))
            return true;
    }
    if (parent)
        return parent->GetRightCaretState(caret_state, select);
    return false;
}

bool Element::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (!IsVisible())
        return false;
    if (y < GetAbsoluteRect().GetBottom())
    {
        if (parent)
            return parent->GetTopCaretState(x, y, caret_state, select);
        return false;
    }

    CaretState next, last;
    if (!GetFirstCaretState(next, select))
        next.SetState(id);
    if (!GetLastCaretState(last, select))
        last.SetState(id);
    
    caret_state = next;
    Rect r = document->GetCaretRect(next);
	int min_dist = r.DistToPoint(x, y);

    while (next != last)
    {
        ElementPtr el = document->GetParent(next.id);
        if (!el->GetRightCaretState(next, select))
            break;
        
        r = document->GetCaretRect(next);
        int dist = r.DistToPoint(x, y);
        if (dist < min_dist)
        {
            min_dist = dist;
            caret_state = next;
        }
    }

    auto el = document->GetElement(caret_state.id);
    if (el && el->CanContinueVerticalMoving())
        el->GetTopCaretState(x, y, caret_state, select);

    return true;
}

bool Element::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (!IsVisible())
        return false;
    if (y > GetAbsoluteRect().top)
    {
        if (parent)
            return parent->GetBottomCaretState(x, y, caret_state, select);
        return false;
    }

    CaretState next, last;
    if (!GetFirstCaretState(next, select))
        next.SetState(id);
    if (!GetLastCaretState(last, select))
        last.SetState(id);
    
    caret_state = next;
    Rect r = document->GetCaretRect(next);
	int min_dist = r.DistToPoint(x, y);

    while (next != last)
    {
        ElementPtr el = document->GetParent(next.id);
        if (!el->GetRightCaretState(next, select))
            break;
        
        Rect r = document->GetCaretRect(next);
        int dist = r.DistToPoint(x, y);
        if (dist < min_dist)
        {
            min_dist = dist;
            caret_state = next;
        }
    }

    auto el = document->GetElement(caret_state.id);
    if (el && el->CanContinueVerticalMoving())
        el->GetBottomCaretState(x, y, caret_state, select);

    return true;
}

bool Element::GetBeginCaretState(CaretState& caret_state, Selection* select)
{
    if (!IsVisible())
        return false;
    if (select && caret_state.IsInsideElement(id))
        select->Add(id, 0, caret_state.GetPos());
    if (parent)
        return parent->GetBeginCaretState(caret_state, select);
    return false;
}

bool Element::GetEndCaretState(CaretState& caret_state, Selection* select)
{
    if (!IsVisible())
        return false;
    if (select && caret_state.IsInsideElement(id))
        select->Add(id, caret_state.GetPos(), elements->Count() - caret_state.GetPos());
    if (parent)
        return parent->GetEndCaretState(caret_state, select);
    return false;
}

bool Element::GetWordLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (!IsVisible())
        return false;
    if (caret_state.IsInsideElement(id))
    {
        if (elements->GetWordLeftCaretState(caret_state, select))
            return true;
    }
    if (parent)
        return parent->GetWordLeftCaretState(caret_state, select);
    return false;
}

bool Element::GetWordRightCaretState(CaretState& caret_state, Selection* select)
{
    if (!IsVisible())
        return false;
    if (caret_state.IsInsideElement(id))
    {
        if (elements->GetWordRightCaretState(caret_state, select))
            return true;
    }
    if (parent)
        return parent->GetWordRightCaretState(caret_state, select);
    return false;
}

bool Element::GetSelectOutCaretState(CaretState& caret_state, Selection* select)
{
    if (!IsVisible())
        return false;
    if (caret_state.IsInsideElement(id))
    {
        if (elements->GetSelectOutCaretState(caret_state, select))
            return true;
    }
    return false;
}

bool Element::HasCaretState()
{
    return false;
}

bool Element::HasLastCaretState()
{
    return false;
}

bool Element::CanContinueSelection()
{
    return false;
}

bool Element::CanContinueVerticalMoving()
{
    return false;
}

bool Element::UseSelection()
{
    return false;
}

Rect Element::GetCaretRect(const uint pos) const
{
    return elements->GetCaretRect(pos);
}

Rect Element::GetCaretRect() const
{
    return parent->GetCaretRect(parent->elements->GetElementPos(id));
}

void Element::DrawCaret(const uint pos) const
{
    elements->DrawCaret(pos);
}

void Element::GetMargin(int& left, int& top, int& right, int& bottom) const
{
    left = top = right = bottom = 0;
}

Color Element::GetBackgroundColor() const
{
    if (parent)
        return parent->GetBackgroundColor();
    return document->config.page_color;
}

bool Element::MouseLButtonHold(const int x, const int y, MouseHoldType& hold_type, ElementId& hold_id)
{
    if (parent)
        return parent->MouseLButtonHold(x, y, hold_type, hold_id);
    return false;
}

bool Element::MouseRButtonHold(const int x, const int y, MouseHoldType& hold_type, ElementId& hold_id)
{
    if (parent)
        return parent->MouseRButtonHold(x, y, hold_type, hold_id);
    return false;
}

std::string Element::ToHtml() const
{
    return elements->ToHtml();
}

std::u32string Element::ToText() const
{
    if (!elements)
        return U"";
    return elements->ToText();
}

void Element::ToParserString(ParserString& str)
{
    if (elements->Count() > 0)
    {
        int start = str.Length();
        for (int i = 0; i < elements->Count(); ++i)
            elements->Get(i)->ToParserString(str);
        str.Annotate(id, start, str.Length());
    }
    else
        str.Add(id, ToText());
}

void Element::UpdateRect(bool with_elements)
{
    if (!IsVisible())
        return;
    if (elements->Count() == 0)
    {
        rect.SetSize(0, 0);
        return;
    }

    if (with_elements)
    {
        for (int i = 0; i < elements->Count(); ++i)
            elements->Get(i)->UpdateRect();
    }
    
    //get rect relatively to the parent
    int right = std::numeric_limits<int>::min();
    int bottom = std::numeric_limits<int>::min();
    for (uint i = 0; i < elements->Count(); ++i)
    {
        Rect& r = elements->Get(i)->rect;
        if (r.GetRight() > right)
            right = r.GetRight();
        if (r.GetBottom() > bottom)
            bottom = r.GetBottom();
    }
    rect.SetSize(right, bottom);
}

bool Element::GetElementAtCoords(const int x, const int y, const int margin, ElementId& _id)
{
    if (!IsVisible())
        return false;
    Rect r = GetAbsoluteRect();
    r.left -= margin;
    r.top -= margin;
    r.width += margin * 2;
    r.height += margin * 2;
    if (!r.IsPointInside(x, y))
        return false;
    else if (elements->Count() == 0)
    {
        _id = id;
        return true;
    }
    
    //look in the child elements
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr el = elements->Get(i);
        if (el->GetElementAtCoords(x, y, margin, _id))
            return true;
        r = el->GetAbsoluteRect();
        if (r.IsPointInside(x, y))
        {
            _id = el->id;
            return true;
        }
    }
    return false;
}

bool Element::GetNearestElement(const int x, const int y, ElementId& _id, int& dist)
{
    if (!IsVisible())
        return false;
    Rect r = GetAbsoluteRect();
    int d = r.DistToPoint(x, y);
    if (d > dist)
        return false;

    dist = d;
    _id = id;

    for (int i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        el->GetNearestElement(x, y, _id, dist);
    }

    return true;
}

bool Element::GetNearestCaretState(const int x, const int y, CaretState& caret_state)
{
    if (!IsVisible())
        return false;
    int dist = std::numeric_limits<int>::max();
    CaretState next, last;
    if (!GetFirstCaretState(next, nullptr) || !GetLastCaretState(last, nullptr))
    {
        if (!parent->GetFirstCaretState(next, nullptr) || !parent->GetLastCaretState(last, nullptr))
        {
            if (parent->HasCaretState())
            {
                caret_state.SetState(yutovo::GetParent(id));
                return true;
            }
        }
    }

    int min_dist = dist;
    Rect r;
    if (HasCaretState())
    {
        next.SetState(id);
        r = document->GetCaretRect(next);
    	dist = r.DistToPoint(x, y);
        if (dist < min_dist)
        {
            min_dist = dist;
            caret_state = next;
        }
    }

    r = document->GetCaretRect(next);
    dist = r.DistToPoint(x, y);
    if (dist < min_dist)
    {
        min_dist = dist;
        caret_state = next;
    }

    caret_state = next;
    while (next != last)
    {
        ElementPtr el = document->GetParent(next.id);
        if (!el->GetRightCaretState(next, nullptr))
            break;
        r = document->GetCaretRect(next);
        dist = r.DistToPoint(x, y);
        if (r.IsPointInside(x, y) || dist < min_dist)
        {
            min_dist = dist;
            caret_state = next;
        }
    }
    return true;
}

void Element::AddElement(ElementPtr element)
{
    elements->Add(element);
}

void Element::AddEmptyElement()
{
}

uint Element::GetChildPos(const Element* element)
{
    for (size_t i = 0; i < elements->Count(); ++i)
    {
        if (elements->Get(i)->id == element->id)
            return i;
    }
    assert(false);
    return 0;
}

Element* Element::GetParent(const int pos)
{
    if (id.size() <= pos)
        return nullptr;
    Element* res = this;
    for (int i = id.size() - 1; i > pos && res; --i)
        res = res->parent;
    return res;
}

Rect Element::GetAbsoluteRect(const Rect& _rect) const
{
    Rect r = _rect;
    r.left += rect.left;
    r.top += rect.top;

    Element* p = parent;
    while (p)
    {
        r.left += p->rect.left;
        r.top += p->rect.top;
        p = p->parent;
    }
    return r;
}

Rect Element::GetAbsoluteRect() const
{
    Rect r = rect;
    Element* p = parent;
    while (p)
    {
        r.left += p->rect.left;
        r.top += p->rect.top;
        p = p->parent;
    }
    return r;
}

Point Element::GetAbsolutePoint(const Point& point) const
{
    if (parent)
    {
        Point parent_point = parent->GetAbsolutePoint(point);
        Point p = point;
        p.x += parent_point.x + rect.left;
        p.y += parent_point.y + rect.top;
        return p;
    }
    return point;
}

ParagraphFormatPtr Element::GetParagraphFormat()
{
    assert(parent);
    return parent->GetParagraphFormat();
}

StringFormatPtr Element::GetStringFormat() const
{
    if (!parent)
        return nullptr;
    return parent->GetStringFormat();
}

FormulaFormatPtr Element::GetFormulaFormat() const
{
    if (!parent)
    {
        FormulaFormatPtr f;
        if (document->GetCurrentFormulaFormat(f))
            return f;
        return nullptr;
    }
    return parent->GetFormulaFormat();
}

void Element::UpdateFormat(StringFormatPtr& _format)
{
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->UpdateFormat(_format);
}

void Element::UpdateDrawRect()
{
    if (!IsVisible())
        return;

    draw_rect = GetAbsoluteRect();

    if (elements->Count() > 0)
    {
        for (int i = 0; i < elements->Count(); ++i)
            elements->Get(i)->UpdateDrawRect();

        int left = std::numeric_limits<int>::max();
        int top = std::numeric_limits<int>::max();
        int right = std::numeric_limits<int>::min();
        int bottom = std::numeric_limits<int>::min();
        for (uint i = 0; i < elements->Count(); ++i)
        {
            Rect& r = elements->Get(i)->draw_rect;
            if (r.left < left)
                left = r.left;
            if (r.top < top)
                top = r.top;
            if (r.GetRight() > right)
                right = r.GetRight();
            if (r.GetBottom() > bottom)
                bottom = r.GetBottom();
        }

        if (draw_rect.left > left)
            draw_rect.left = left;
        if (draw_rect.top > top)
            draw_rect.top = top;
        if (right - left > draw_rect.width)
            draw_rect.width = right - left;
        if (bottom - top > draw_rect.height)
            draw_rect.height = bottom - top;
    }
    
    int start, size;
    if (has_caret_hilight || has_frame_hilight || document->HasErrorMark(id, start, size))
    {
        draw_rect.left -= 2;
        draw_rect.height += 2;
        draw_rect.width += 4;
    }
}

void Element::UpdateLevel(uint8_t _level)
{
    level = _level;
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->UpdateLevel(_level);
}

void Element::SetEditable(bool _editable)
{
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->SetEditable(_editable);
    editable = _editable;
}

int Element::FindElement(const ElementId from_id, bool forward, const ElementType type)
{
    int pos = elements->GetElementPos(from_id);
    if (pos == -1)
        return -1;
    if (forward)
    {
        for (int i = pos + 1; i < elements->Count(); ++i)
        {
            auto el = elements->Get(i);
            if (el->type == type)
                return i;
        }
    }
    else
    {
        for (int i = pos - 1; i >= 0; --i)
        {
            auto el = elements->Get(i);
            if (el->type == type)
                return i;
        }
    }
    return -1;
}

void Element::GetElements(ElementType _type, std::vector<ElementId>& _elements)
{
    for (int i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (el->type == _type)
            _elements.push_back(el->id);
        el->GetElements(_type, _elements);
    }
}

void Element::GetLogicalElements(ElementType _type, std::vector<LogicalId>& _elements)
{
    std::vector<ElementId> _els;
    GetElements(_type, _els);
    for (auto _el : _els)
        _elements.push_back(document->GetLogicalId(_el));
}

void Element::GetElementsAbove(const ElementId from_id, ElementType _type, std::vector<ElementId>& _elements)
{
    int p = yutovo::GetChildPos(id, from_id);
    for (int i = p - 1; i >= 0; --i)
    {
        auto c = elements->Get(i);
        if (c->type == _type)
            _elements.push_back(c->id);
        c->GetElements(_type, _elements);
    }
}

void Element::GetElementsBelow(const ElementId from_id, ElementType _type, std::vector<ElementId>& _elements)
{
    int p = yutovo::GetChildPos(id, from_id);
    for (int i = p + 1; i < elements->Count(); ++i)
    {
        auto c = elements->Get(i);
        if (c->type == _type)
            _elements.push_back(c->id);
        c->GetElements(_type, _elements);
    }
}

ElementId Element::FindParent(const ElementType _type)
{
    if (type == _type)
        return id;
    if (!parent)
        return ElementId{};
    return parent->FindParent(_type);
}

bool Element::IsFormula()
{
    return false;
}

bool Element::IsEmpty() const
{
    return elements->Count() == 0;
}

void Element::Solve()
{
    if (parent)
        parent->Solve();
}

void Element::ReSolve(bool if_error, bool force)
{
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->ReSolve(if_error, force);
}

bool Element::IsVisible() const
{
    const Element* p = this;
    while (p)
    {
        if (!p->visible)
            return false;
        p = p->parent;
    }
    return true;
}

//Elements

Elements::Elements(Element* _parent) :
    parent(_parent),
    caret(parent->document ? parent->document->caret : nullptr),
    selection(parent->document ? &parent->document->selection : nullptr)
{
}

Elements::Elements(const Elements& source) :
    parent(source.parent),
    caret(parent->document->caret),
    selection(&parent->document->selection)
{
    for (auto& el : source.elements)
        elements.push_back(ElementPtr(el->Clone())); //deep copy
}

void Elements::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    rapidjson::Value arr(rapidjson::kArrayType);
    for (auto it = elements.begin(); it != elements.end(); ++it)
    {
        auto& el = *it;
        rapidjson::Value v;
        v.SetObject();
        el->ToJson(v, alloc);
        arr.PushBack(v, alloc);
    }
    value.AddMember("elements", arr, alloc);
}

bool Elements::FromJson(Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.HasMember("elements") || !value["elements"].IsArray())
        return false;
    
    rapidjson::Value::ConstArray arr = value["elements"].GetArray();
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
    {
        if (!arr[i].IsObject())
            return false;
        rapidjson::Value::ConstObject value = arr[i].GetObject();
        Element* el = CreateFromJson(parent, document, value, alloc);
        if (!el)
            return false;
        Add(ElementPtr(el));
    }
    return true;
}

ElementPtr Elements::operator[](const int pos)
{
    return elements[pos];
}

Elements* Elements::Clone(Element* _parent)
{
    std::unique_ptr<Elements> res(new Elements(_parent));    
    for (auto el : elements)
        res->Add(ElementPtr(el->Clone()));
    return res.release();
}

void Elements::Clone(std::vector<ElementPtr>& _elements, const uint start, const uint size)
{
    for (int i = start; i < start + size; ++i)
        _elements.push_back(elements[i]);
}

void Elements::Draw() const
{
    for (auto el : elements)
    {
        if (el->IsVisible())
            el->Draw();
    }
}

ElementPtr Elements::Get(uint pos)
{
    if (pos >= elements.size())
        return nullptr;
    return elements[pos];
}

ElementPtr Elements::Get(ElementId id)
{
    auto it = std::find_if(elements.begin(), elements.end(), 
        [id](ElementPtr& el)
        {
            return el->id == id;
        });
    if (it == elements.end())
        return nullptr;
    return *it;
}

ElementId Elements::GetElementId(uint pos)
{
    assert(elements.size() > pos);
    return elements[pos]->id;
}

int Elements::GetElementPos(ElementId id)
{
    for (size_t i = 0; i < elements.size(); ++i)
    {
        if (GetElementId(i) == id)
            return i;
    }
    if (Count() > 0 && elements[elements.size() - 1]->HasLastCaretState())
    {
        if (GetElementId(Count() - 1) == GetPrevPos(id))
            return Count() - 1;
    }
    return -1;
}

//Find child in the id and get its position
int Elements::GetChildPos(ElementId id)
{
    if (parent->id.size() > id.size())
        return -1;
    ElementId _id(id);
    _id.erase(_id.begin() + parent->id.size() + 1, _id.end());

    for (size_t i = 0; i < elements.size(); ++i)
    {
        if (GetElementId(i) == _id)
            return i;
    }
    return -1;
}

bool Elements::IsFirst(ElementId id)
{
    if (elements.empty())
        return false;
    return Get(0)->id == id;
}

bool Elements::IsLast(ElementId id)
{
    if (elements.empty())
        return false;
    return Get(Count() - 1)->id == id;
}

ElementId Elements::FindUpper(int y)
{
    auto it = std::lower_bound(elements.begin(), elements.end(), y, 
        [](const ElementPtr& el, int value)
        {
            return el->GetAbsoluteRect().GetBottom() < value;
        });
    if (it != elements.end())
        return (*it)->id;
    return {};
}

void Elements::Add(ElementPtr element)
{
    elements.push_back(ElementPtr(element));

    element->parent = parent;
    element->document = parent->document;
    element->window = parent->window;
    //set id
    UpdateIds();

#ifdef DEBUG
    parent->to_str = parent->ToText();
#endif
}

void Elements::Insert(ElementPtr element, const uint pos)
{
    CaretState c, last;
    if (caret->IsInsideElement(element->id))
        c = caret->GetCaretState();
    else if (caret->IsOnElement(element->id))
        c = caret->GetCaretState();
    else if (pos == parent->elements->Count() && pos > 0 && parent->elements->Get(pos - 1)->HasCaretState())
        parent->GetLastCaretState(last, nullptr);
    
    bool p = selection->IsSelected(parent->id);
    bool s = false;
    if (element->parent)
    {
        s = selection->IsSelected(element->id);
        if (s)
            selection->Remove(element->parent->id, GetChildPos(element->id), 1);
    }
    ElementSelection p_s;
    if (selection->HasChild(element->id, p_s))
        selection->Remove(p_s.element->id, p_s.start, p_s.size);
    
    for (int i = pos; i < Count(); ++i) //elements after pos will be replaced
        elements[i]->BeforeReplace();
    
    elements.insert(elements.begin() + pos, element);

    element->parent = parent;
    element->document = parent->document;
    element->window = parent->window;

    UpdateIds(); //set id

    for (int i = pos + 1; i < Count(); ++i) //elements after pos were replaced
        elements[i]->AfterReplace();

    selection->InsertElement(element->id); //update selection positions after inserting new element

    //update caret state on the new position of the element
    if (!c.IsEmpty())
    {
        auto _id = GetWithParent(c.id, element->id);
        auto _el = c.last_pos ? parent->document->GetElement(GetParent(_id)) : parent->document->GetElement(_id);
        if (_el)
        {
            CaretState _c;
            if (c.last_pos && _el->GetLastCaretState(_c, nullptr))
                caret->SetState(_c);
            else
                caret->SetState(_id, true);
        }
    }
    else if (!last.IsEmpty())
    {
        if (last == caret->GetCaretState())
        {
            if (element->HasCaretState())
            {
                if (parent->document->pasting)
                    caret->SetState(parent->id, GetChildPos(element->id) + 1, true);
                else
                    caret->SetState(parent->id, GetChildPos(element->id), false);
            }
            else
            {
                if (element->GetFirstCaretState(c, nullptr))
                    caret->SetState(c);
            }
        }
    }
    
    if (p)
        selection->Remove(parent->id, pos, 1);

    if (s)
    {
        if (!selection->IsSelected(element->parent->id))
            selection->Add(element->parent->id, element->parent->elements->GetElementPos(element->id), 1); //move selection onto the placed element
    }
    if (!p_s.IsEmpty())
        selection->Add(GetWithParent(p_s.element->id, element->id), p_s.start, p_s.size);

#ifdef DEBUG
    parent->to_str = parent->ToText();
    element->to_str = element->ToText();
#endif
}

void Elements::Remove(const ElementPtr element)
{
    parent->elements->RemoveAt(element->parent->elements->GetElementPos(element->id), 1);

#ifdef DEBUG
    parent->to_str = parent->ToText();
#endif
}

void Elements::Remove(const ElementId id)
{
    RemoveAt(GetElementPos(id), 1);
}

void Elements::RemoveAt(const uint pos, const int size)
{
    selection->Remove(parent->id, pos, size);
    int cs_pos = -1;
    if (!parent->id.empty())
    {
        for (int i = pos; i < pos + size; ++i)
        {
            if (caret->IsInsideElement(elements[i]->id) || caret->IsOnElement(elements[i]->id))
                cs_pos = i;
        }
    }

    for (int i = 0; i < size; ++i) //update selection positions before deleting elements
        selection->RemoveElement(GetElementId(pos + i));
    
    for (uint i = pos; i < pos + size; ++i)
        elements[i]->BeforeDelete();

    for (int i = pos + size + 1; i < Count(); ++i) //elements after will be replaced
        elements[i]->BeforeReplace();

    elements.erase(elements.begin() + pos, elements.begin() + pos + size);

    UpdateIds();

    selection->Optimize();

    for (int i = pos + 1; i < Count(); ++i) //elements after were replaced
        elements[i]->AfterReplace();

    if (cs_pos != -1 && Count() > 0)
    {
        if (pos == Count() && elements[pos - 1]->HasLastCaretState())
        {
            caret->SetState(parent->id, pos);
        }
        else
        {
            ElementPtr el = Get(pos < Count() ? pos : pos - 1);
            if (el->HasCaretState())
            {
                caret->SetState(el->id);
            }
            else
            {
                CaretState s;
                if (pos < Count())
                    el->GetFirstCaretState(s, nullptr);
                else
                    el->GetLastCaretState(s, nullptr);
                caret->SetState(s);
            }
        }
    }
    else
        caret->Update();

#ifdef DEBUG
    parent->to_str = parent->ToText();
#endif
}

void Elements::Move(const ElementPtr element, const uint pos)
{
    parent->document->moving_element = true;
    element->BeforeReplace();
    Insert(ElementPtr(element->Clone()), pos);
    Element* old_parent = element->parent;
    old_parent->elements->Remove(element);
    elements[pos]->AfterReplace();
    UpdateIds();
    parent->document->moving_element = false;

#ifdef DEBUG
    parent->to_str = parent->ToText();
    old_parent->to_str = old_parent->ToText();
#endif
}

void Elements::Move(const Elements& _elements, const uint pos)
{
    for (int i = 0, j = 0; i < _elements.Count();)
        Move(_elements.elements[0], pos + j++);
}

void Elements::Replace(ElementPtr element, const uint pos)
{
    bool s = parent->document->selection.IsSelected(elements[pos]->id);
    RemoveAt(pos, 1);
    Insert(element, pos);
    if (s) //return selection
        parent->document->selection.Add(elements[pos]->id);
}

void Elements::Replace(const uint pos, const int size, std::vector<ElementPtr>& _elements)
{
    LogicalSelectionState s = selection->GetLogicalState();
    selection->Clear();

    int cs_pos = -1;
    if (!parent->id.empty())
    {
        for (int i = pos; i < pos + size; ++i)
        {
            if (caret->IsInsideElement(elements[i]->id) || caret->IsOnElement(elements[i]->id))
                cs_pos = i;
        }
    }

    for (uint i = pos; i < pos + size; ++i)
        elements[i]->BeforeDelete();

    for (int i = pos + size + 1; i < Count(); ++i) //elements after will be replaced
        elements[i]->BeforeReplace();

    elements.erase(elements.begin() + pos, elements.begin() + pos + size);

    for (size_t i = 0; i < _elements.size(); ++i)
    {
        auto& el = _elements[i];
        el->parent = parent;
        el->document = parent->document;
        el->window = parent->window;
        elements.insert(elements.begin() + pos + i, el);
    }

    size_t i = 0;
    for (; i < _elements.size(); ++i)
    {
        auto& el = elements[pos + i];
        el->id = parent->id;
        el->id.push_back(i + pos);
        if (el->type != ElementType::ROW)
        {
            el->logical_id = parent->logical_id;
            el->logical_id.push_back(i + pos);
        }
        el->elements->UpdateIds();
    }
    if (size != _elements.size())
    {
        for (; i < elements.size(); ++i)
            elements[pos + i]->elements->UpdateIds();
    }

    selection->Set(s);
    selection->Optimize();

    for (int i = pos + 1; i < Count(); ++i) //elements after were replaced
        elements[i]->AfterReplace();

    if (cs_pos != -1 && Count() > 0)
    {
        if (pos == Count() && elements[pos - 1]->HasLastCaretState())
        {
            caret->SetState(parent->id, pos);
        }
        else
        {
            ElementPtr el = Get(pos < Count() ? pos : pos - 1);
            if (el->HasCaretState())
            {
                caret->SetState(el->id);
            }
            else
            {
                CaretState s;
                if (pos < Count())
                    el->GetFirstCaretState(s, nullptr);
                else
                    el->GetLastCaretState(s, nullptr);
                caret->SetState(s);
            }
        }
    }
    else
        caret->Update();

#ifdef DEBUG
    parent->to_str = parent->ToText();
#endif
}

void Elements::ReplaceAll(const Elements& _elements)
{
    Clear();
    for (auto _el : _elements.elements)
        Add(_el);
}

void Elements::Clear()
{
    CaretState c;
    if (parent->caret->IsInsideElement(parent->id))
        c = caret->GetCaretState();

    elements.clear();

    if (!c.IsEmpty())
    {
        //move caret on the nearest parent
        CaretState p_c;
        auto p = parent->parent;
        while (p)
        {
            if (p->GetFirstCaretState(p_c, nullptr) && p_c.id != c.id)
                break;
            p = p->parent;
        }
        caret->SetState(p_c);
    }

#ifdef DEBUG
    parent->to_str = parent->ToText();
#endif
}

uint Elements::Count() const
{
    return elements.size();
}

uint Elements::Size() const
{
    return elements.size();
}

Rect Elements::GetCaretRect(const uint pos) const
{
    Rect r;
    if (pos == Count())
    {
        Rect& rect = elements[pos - 1]->rect;
        r = Rect{rect.GetRight() + 1, rect.top - 1, 2, rect.height + 2}; //for last caret state draw one line
    }
    else
    {
        r = elements[pos]->rect;
        if (parent->document->insert_mode)
        {
            r.left -= 3;
            r.width += 1;
            r.top -= 1;
            r.height += 6;
        }
        else
        {
            r.left -= 3;
            r.width += 6;
            r.top -= 4;
            r.height += 8;
        }
    }
    return r;
}

void Elements::DrawCaret(const uint pos) const
{
    if (pos == Count())
    {
        Rect r = parent->GetAbsoluteRect(GetCaretRect(pos));
        parent->window->DrawLine(r.GetRight() - 1, r.top + 1, r.GetRight() - 1, r.GetBottom() - 2, Color::Black());
    }
    else
    {
        Rect r = parent->GetAbsoluteRect(GetCaretRect(pos));
        if (parent->document->insert_mode) //draw caret of two lines
        {
            parent->window->DrawLine(r.left + 1, r.top + 1, r.left + 1, r.GetBottom() - 2, Color::Black());
            parent->window->DrawLine(r.left + 1, r.GetBottom() - 2, r.GetRight() - 2, r.GetBottom() - 2, Color::Black());
        }
        else //draw caret of a rect
        {
            parent->window->DrawRect(r.left + 1, r.top + 1, r.width - 2, r.height - 2, Color::Black());
        }
    }
}

Rect Elements::GetRect()
{
    int left = std::numeric_limits<int>::max();
    int top = std::numeric_limits<int>::max();
    int right = std::numeric_limits<int>::min();
    int bottom = std::numeric_limits<int>::min();
    for (auto& element : elements)
    {
        if (element->rect.left < left)
            left = element->rect.left;
        if (element->rect.top < top)
            top = element->rect.top;
        if (element->rect.GetRight() > right)
            right = element->rect.GetRight();
        if (element->rect.GetBottom() > bottom)
            bottom = element->rect.GetBottom();
    }
    return Rect{left, top, right - left, bottom - top};
}

Rect Elements::GetRect(const uint pos)
{
    return elements[pos]->rect;
}

bool Elements::GetFirstCaretState(CaretState& caret_state, Selection* select)
{
    if (elements.empty())
        return false;
    if (elements[0]->HasCaretState())
    {
        caret_state.id = GetElementId(0);
        return true;
    }
    for (size_t i = 0; i < elements.size(); ++i)
    {
        if (elements[i]->IsVisible())
            return elements[i]->GetFirstCaretState(caret_state, select);
    }
    return false;
}

bool Elements::GetLastCaretState(CaretState& caret_state, Selection* select)
{
    if (elements.empty())
        return false;
    if (elements[elements.size() - 1]->HasLastCaretState())
    {
        caret_state.SetState(parent->id, Count(), true);
        return true;
    }
    return elements[elements.size() - 1]->GetLastCaretState(caret_state, select);
}

bool Elements::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    int p = GetChildPos(caret_state.id);
    if (p < 0)
    {
        CaretState c;
        if (GetLastCaretState(c, nullptr) && caret_state == c)
            p = Count();
        else
            return false;
    }
    while (p-- > 0)
    {
        if (select)
        {
            if (elements[p]->CanContinueSelection())
            {
                if (elements[p]->GetLastCaretState(caret_state, nullptr))
                {
                    ElementPtr el = parent->document->GetParent(caret_state.id);
                    if (el->GetLeftCaretState(caret_state, select))
                        return true;
                }
            }
            else if (elements[p]->HasCaretState())
            {
                caret_state.SetState(Get(p));
                if (select)
                    select->Add(parent->id, p, 1);
                return true;
            }
        }
        else if (Count() > p + 1 && elements[p + 1]->id != caret_state.id && elements[p + 1]->HasCaretState())
        {
            caret_state.SetState(Get(p + 1));
            if (select)
                select->Add(parent->id, p + 1, 1);
            return true;
        }
        else if (elements[p]->GetLastCaretState(caret_state, select))
            return true;
    }

    if (Count() > 0 && elements[0]->HasCaretState())
    {
        if (caret_state != CaretState(Get(0)->id))
        {
            ElementId last_id = caret_state.id;
            caret_state.SetState(Get(0));
            if (select)
            {
                ElementSelection s;
                if (last_id.size() == caret_state.id.size() || (!selection->Has(elements[0]->id, s) || s.size != elements[0]->elements->Count()))
                    select->Add(parent->id, 0, 1);
            }
            return true;
        }
    }
    return false;
}

bool Elements::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    int p = GetChildPos(caret_state.id);
    if (p < 0)
        return false;
    
    if (parent->document->GetParent(caret_state.id)->id == parent->id) //try to enter into this element
    {
        if ((select && elements[p]->CanContinueSelection()) || !select)
        {
            if (elements[p]->GetFirstCaretState(caret_state, select))
            {
                if (select)
                    select->Add(parent->id, p, 1);
                return true;
            }
        }
    }

    while (++p < Count()) //find next nearest caret state
    {
        if (elements[p]->HasCaretState())
        {
            if (select)
            {
                if (elements[p - 1]->HasCaretState())
                {
                    if (!elements[p]->CanContinueSelection() || !elements[p]->GetFirstCaretState(caret_state, nullptr))
                        caret_state.SetState(parent->id, p);
                }
                else if (Count() > p + 1)
                {
                    if (!elements[p + 1]->CanContinueSelection() || !elements[p + 1]->GetFirstCaretState(caret_state, nullptr))
                        caret_state.SetState(parent->id, caret->IsOnElement(elements[p]->id) ? p : p + 1);
                }
                else
                    caret_state.SetState(parent->id, elements[p - 1]->HasCaretState() ? p : p + 1);
                select->Add(parent->id, elements[p - 1]->HasCaretState() ? p - 1 : p, 1);
            }
            else
                caret_state.SetState(Get(p));
            return true;
        }
        if (select)
        {
            if (elements[p - 1]->HasCaretState())
            {
                select->Add(parent->id, p - 1, 1);
                if (elements[p]->HasCaretState())
                {
                    caret_state.SetState(Get(p));
                    return true;
                }
                else if (elements[p]->GetFirstCaretState(caret_state, nullptr))
                    return true;
            }
            else if (elements[p]->CanContinueSelection())
            {
                if (elements[p]->GetFirstCaretState(caret_state, nullptr))
                {
                    ElementPtr el = parent->document->GetParent(caret_state.id);
                    if (el->GetRightCaretState(caret_state, select))
                        return true;
                }
            }
        }
        else if (elements[p]->GetFirstCaretState(caret_state, select))
        {
            if (select)
                select->Add(parent->id, p, 1);
            return true;
        }
    }

    if (elements[p - 1]->HasLastCaretState())
    {
        ElementId last_id = caret_state.id;
        caret_state.SetState(parent->id, p);
        if (select)
        {
            ElementSelection s;
            if (last_id.size() == caret_state.id.size() || (!selection->Has(elements[p - 1]->id, s) || s.size != elements[p - 1]->elements->Count()))
                select->Add(parent->id, p - 1, 1);
        }
        return true;
    }
    return false;
}

bool Elements::GetWordLeftCaretState(CaretState& caret_state, Selection* select)
{
    int p = GetChildPos(caret_state.id);
    if (p < 0)
        return false;
    while (p-- > 0)
    {
        if (elements[p]->type == ElementType::CODE_BLOCK)
        {
            caret_state.SetState(Get(p));
            if (select)
                select->Add(parent->id, p, 1);
            return true;
        }
        if (select)
        {
            if (elements[p]->CanContinueSelection())
            {
                if (elements[p]->GetLastCaretState(caret_state, select))
                {
                    ElementPtr el = parent->document->GetParent(caret_state.id);
                    if (el->GetWordLeftCaretState(caret_state, select))
                        return true;
                }
            }
        }
        else
        {
            if (elements[p]->GetLastCaretState(caret_state, select))
            {
                if (elements[p]->GetWordLeftCaretState(caret_state, select))
                    return true;
            }
        }
        if (elements[p]->HasCaretState())
        {
            caret_state.SetState(Get(p));
            if (select)
                select->Add(parent->id, p, 1);
            return true;
        }
    }
    return false;
}

bool Elements::GetWordRightCaretState(CaretState& caret_state, Selection* select)
{
    int p = GetChildPos(caret_state.id);
    if (p < 0)
        return false;
    while (++p < Count())
    {
        if (elements[p]->HasCaretState())
        {
            caret_state.SetState(Get(p));
            if (select)
                select->Add(parent->id, p, 1);
            return true;
        }
        if (select)
        {
            if (elements[p - 1]->HasCaretState())
                select->Add(parent->id, p - 1, 1);
            if (elements[p]->CanContinueSelection())
            {
                if (elements[p]->GetFirstCaretState(caret_state, select))
                {
                    ElementPtr el = parent->document->GetParent(caret_state.id);
                    if (el->GetWordRightCaretState(caret_state, select))
                        return true;
                }
            }
        }
        else
        {
            if (elements[p]->GetFirstCaretState(caret_state, select))
            {
                if (elements[p]->GetWordRightCaretState(caret_state, select))
                   return true;
            }
        }
    }
    return false;
}

bool Elements::GetSelectOutCaretState(CaretState& caret_state, Selection* select)
{
    if (!select)
        return false;
    select->Add(parent->id, 0, Count());
    caret_state.SetState(parent->id, Count());
    return true;
}

std::string Elements::ToHtml() const
{
    std::string html;
    for (auto it = elements.begin(); it != elements.end(); ++it)
    {
        auto& el = *it;
        if (!el->IsVisible())
            continue;
        html += el->ToHtml();
    }
    return html;
}

std::u32string Elements::ToText() const
{
    std::u32string t;
    for (auto it = elements.begin(); it != elements.end(); ++it)
        t += (*it)->ToText();
    return t;
}

void Elements::UpdateIds()
{
    for (size_t i = 0; i < elements.size(); ++i)
    {
        auto& el = elements[i];
        if (el->parent->id.empty())
        {
            el->id.clear();
            el->logical_id.clear();
            el->elements->UpdateIds();
            continue;
        }
        LogicalId last_id = el->logical_id;
        el->id = el->parent->id;
        el->id.push_back(i);

        if (el->type == ElementType::PARAGRAPH)
        {
            el->logical_id = el->parent->logical_id;
            el->logical_id.push_back(i);
        }
        else if (el->type == ElementType::ROW)
        {
        }
        else if (el->parent->type == ElementType::ROW)
        {
            if (el->parent->parent->elements)
            {
                el->logical_id = el->parent->parent->logical_id;

                int p = el->parent->parent->elements->GetChildPos(el->parent->id);
                if (p > 0 && i == 0)
                {
                    auto row = el->parent->parent->elements->Get(p - 1);
                    if (row->elements->Count() > 0)
                    {
                        auto last = row->elements->Get(row->elements->Count() - 1);
                        if (last && last->CanMerge(el))
                            el->logical_id = last->logical_id;
                        else
                            el->logical_id.push_back(yutovo::GetChildPos(last->logical_id) + 1);
                    }
                }
                else if (i > 0)
                {
                    auto prev = elements[i - 1];
                    if (prev->CanMerge(el))
                        el->logical_id = prev->logical_id;
                    else
                        el->logical_id.push_back(yutovo::GetChildPos(prev->logical_id) + 1);
                }
                else
                {
                    el->logical_id.push_back(i);
                }
            }
        }
        else
        {
            el->logical_id = el->parent->logical_id;
            el->logical_id.push_back(i);
        }

        el->elements->UpdateIds();

        if (!last_id.empty() && last_id != el->logical_id)
            el->LogicalIdChanged(last_id);
    }
}

}
