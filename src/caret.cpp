/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "caret.h"
#include "text.h"
#include "document.h"
#include "editor_utils.h"
#include <chrono>

#ifndef _WIN32
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
#endif

namespace yutovo
{

//Caret

using namespace std::chrono_literals;

Caret::Caret(Document* _document) :
    document(_document),
    window(document->window)
{
}

void Caret::SetState(const CaretState& caret_state, bool update_x_pos)
{
    if (block)
        return;
    if (caret_state.id == document->text->id)
    {
        element = document->text;
        str_pos = -1;
        return;
    }
    auto el = document->GetParent(caret_state.id);
    if (!el)
    {
        Reset();
        return;
    }

    last_pos = false;
    element = el;
    if (document->IsString(element->id))
        str_pos = caret_state.GetPos();
    else
    {
        str_pos = -1;
        int p = caret_state.GetPos();
        if (p == element->elements->Count())
        {
            element = element->elements->Get(p - 1);
            last_pos = true;
        }
        else
        {
            element = element->elements->Get(p);
        }
    }
    if (update_x_pos)
        UpdateXPos();
    
    if (notify)
    {
        document->CaretMoved();
        document->UpdateFormats();
    }
}

void Caret::SetState(const LogicalCaretState& caret_state, bool update_x_pos)
{
    if (block || caret_state.id.empty())
        return;
    ElementId _id = document->GetElementId(caret_state.id, last_pos);
    if (!_id.empty())
        SetState(yutovo::GetParent(_id), yutovo::GetChildPos(_id), update_x_pos);
}

void Caret::SetState(const ElementId id, const uint pos, bool update_x_pos)
{
    if (block)
        return;
    element = document->GetElement(id);
    if (!element)
    {
        Reset();
        return;
    }

    last_pos = false;
    if (document->IsString(element->id))
        str_pos = pos;
    else
    {
        str_pos = -1;
        if (pos == element->elements->Count())
        {
            element = element->elements->Get(pos - 1);
            last_pos = true;
        }
        else
        {
            element = element->elements->Get(pos);
            if (element && !element->HasCaretState())
            {
                CaretState c;
                if (element->GetFirstCaretState(c, nullptr))
                {
                    SetState(c, update_x_pos);
                    return;
                }
            }
        }
    }
    if (update_x_pos)
        UpdateXPos();
    
    if (notify)
    {
        document->CaretMoved();
        document->UpdateFormats();
    }
}

void Caret::SetState(const ElementId id, bool update_x_pos)
{
    if (block)
        return;
    element = document->GetElement(id);
    if (!element)
    {
        Reset();
        return;
    }
    last_pos = false;
    if (document->IsString(element->id))
        str_pos = GetChildPos(id);
    else
        str_pos = -1;

    if (update_x_pos)
        UpdateXPos();
    
    if (notify)
    {
        document->CaretMoved();
        document->UpdateFormats();
    }
}

void Caret::SetPos(const uint pos, bool update_x_pos)
{
    if (block)
        return;
    last_pos = false;
    str_pos = -1;
    if (document->IsString(element->id))
        str_pos = pos;
    else
    {
        if (pos == element->elements->Count())
        {
            element = element->elements->Get(pos - 1);
            last_pos = true;
        }
        else
        {
            element = element->elements->Get(pos);
        }
    }

    if (update_x_pos)
        UpdateXPos();
    
    if (notify)
    {
        document->CaretMoved();
        document->UpdateFormats();
    }
}

Element* Caret::GetElement() const
{
    if (block || !element)
        return nullptr;
    if (element->id == document->text->id)
        return element.get();

    //element may have been orphaned from the tree, its parent row was deleted while caret still holds a shared_ptr to it, 
    //verify it is still the live node in the document tree
    auto el = document->GetElement(element->id);
    if (!el || el.get() != element.get())
        return nullptr;

    if (str_pos >= 0)
        return element.get();
    return document->GetElement(yutovo::GetParent(element->id)).get();
}

int Caret::GetPos() const
{
    if (block)
        return -1;
    if (str_pos >= 0)
        return str_pos;
    if (!element)
        return -1;
    return last_pos ? GetChildPos(element->id) + 1 : GetChildPos(element->id);
}

CaretState Caret::GetCaretState()
{
    Element* el = GetElement();
    if (block || !el)
        return CaretState(ElementId{});
    return CaretState(el, GetPos(), last_pos);
}

LogicalCaretState Caret::GetLogicalCaretState()
{
    Element* el = GetElement();
    if (block || ! el)
        return LogicalCaretState(LogicalId{});
    return LogicalCaretState(document->GetLogicalId(el->id, GetPos()));
}

bool Caret::IsVisible()
{
    return visible;
}

void Caret::SetVisible(bool _visible)
{
    if (block)
        return;
    if (!_visible)
        Hide();
    visible = _visible;
}

void Caret::Show()
{
    if (block || !document->config.caret_visible)
        return;
    auto el = GetElement();
    if (!visible || !el)
        return;
    
    caret_rect = el->GetAbsoluteRect(el->GetCaretRect(GetPos()));
    if (!show)
        window->StoreRect(caret_rect);
    el->DrawCaret(GetPos());
    window->Update(caret_rect);
    show = true;
}

void Caret::Hide()
{
    if (show)
    {
        window->RestoreRect();
        window->Update(caret_rect);
    }
    show = false;
}

void Caret::Blink()
{
    if (show)
        Hide();
    else
        Show();
}

void Caret::MoveToDocumentBegin(Selection* selection)
{
    if (block)
        return;
    
    left_up_direction = true;
    if (!selection)
    {
        CaretState c;
        if (document->text->GetFirstCaretState(c, selection))
        {
            SetState(c);
            UpdateXPos();
        }
        return;
    }

    auto el = GetElement();
    if (!el)
        return;

    auto paragraph = document->FindParent(el->id, ElementType::PARAGRAPH);
    auto row = document->FindElementOrParent(el->id, ElementType::ROW);
    Rect view_port = window->GetViewPort(0);
    Point p = window->GetDocumentPoint();
    Rect cur = document->GetCaretRect(GetCaretState());

    CaretState next, last;
    int paragraph_pos = document->text->elements->GetChildPos(paragraph->id);
    int row_pos = paragraph->elements->GetChildPos(row->id);
    paragraph = document->text->elements->Get(paragraph_pos);
    for (int j = (row_pos == -1 ? paragraph->elements->Count() - 1 : row_pos); j >= 0; --j)
    {
        row = paragraph->elements->Get(j);
        if (j == row_pos)
        {
            CaretState c = GetCaretState();
            int p = GetChildPos(row->id, c.id);
            auto _el = document->GetElement(GetChild(row->id, p));
            if (document->IsString(_el))
                selection->Add(_el->id, 0, c.id[row->id.size() + 1]);
            if (p > 0 && row->IsVisible())
                selection->Add(row->id, 0, p);
        }
        else
            selection->Add(paragraph, j, 1);
    }

    int j = 0;
    for (int i = 0; i < document->text->elements->Count(); ++i, ++j)
    {
        if (document->text->elements->Get(i)->IsVisible())
            break;
    }
    if (j < paragraph_pos)
        selection->Add(document->text, j, paragraph_pos);
    MoveToDocumentBegin(nullptr);
}

void Caret::MoveToDocumentEnd(Selection* selection)
{
    if (block)
        return;

    left_up_direction = false;
    if (!selection)
    {
        CaretState c;
        if (document->text->GetLastCaretState(c, selection))
        {
            SetState(c);
            UpdateXPos();
        }
        return;
    }

    auto el = GetElement();
    if (!el)
        return;

    auto paragraph = document->FindParent(el->id, ElementType::PARAGRAPH);
    auto row = document->FindElementOrParent(el->id, ElementType::ROW);
    int paragraph_pos = document->text->elements->GetChildPos(paragraph->id);
    int row_pos = paragraph->elements->GetChildPos(row->id);
    paragraph = document->text->elements->Get(paragraph_pos);
    for (int j = row_pos; j < paragraph->elements->Count(); ++j)
    {
        row = paragraph->elements->Get(j);
        if (j == row_pos)
        {
            CaretState c = GetCaretState();
            int p = GetChildPos(row->id, c.id);
            if (c.last_pos)
            {
                auto _el = document->GetElement(GetChild(row->id, p - 1));
                selection->Add(_el, c.id[row->id.size()] - 1, _el->elements->Count() - c.id[row->id.size()]);
            }
            else
            {
                auto _el = document->GetElement(GetChild(row->id, p));
                if (document->IsString(_el))
                    selection->Add(_el, c.id[row->id.size() + 1], _el->elements->Count() - c.id[row->id.size() + 1]);
                else
                    selection->Add(_el->id);
                if (row->elements->Count() > p + 1)
                    selection->Add(row->id, p + 1, row->elements->Count() - p - 1);
            }
        }
        else
            selection->Add(paragraph, j, 1);
    }

    selection->Add(document->text, paragraph_pos + 1, document->text->elements->Count() - paragraph_pos - 1);
    MoveToDocumentEnd(nullptr);
}

void Caret::MoveHome(Selection* selection)
{
    if (block)
        return;
    left_up_direction = true;
    CaretState c = GetCaretState();
    auto el = GetElement();
    if (!el)
        return;
    if (el->GetBeginCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveEnd(Selection* selection)
{
    if (block)
        return;
    left_up_direction = false;
    CaretState c = GetCaretState();
    auto el = GetElement();
    if (!el)
        return;
    if (el->GetEndCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveLeft(Selection* selection)
{
    if (block)
        return;
    left_up_direction = true;
    CaretState c = GetCaretState();
    auto el = GetElement();
    if (!el)
        return;
    if (el->GetLeftCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveRight(Selection* selection)
{
    if (block)
        return;
    left_up_direction = false;
    CaretState c = GetCaretState();
    auto el = GetElement();
    if (!el)
        return;
    if (el->GetRightCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveUp(Selection* selection)
{
    if (block)
        return;
    left_up_direction = true;
    auto el = GetElement();
    if (!el)
        return;
    Rect r = el->GetAbsoluteRect(el->GetCaretRect(GetPos()));
    if (last_x_element && (!selection || selection->IsEmpty()))
    {
        //fix x position
        Rect x_rect = last_x_element->GetAbsoluteRect(last_x_element->GetCaretRect(last_x_pos));
        if (last_pos)
            r.left = x_rect.GetRight();
        else
            r.left = x_rect.left;
    }
    CaretState c;
    if (el->GetTopCaretState(r.left, r.top + 2, c, selection))
        SetState(c, false);
}

void Caret::MoveDown(Selection* selection)
{
    if (block)
        return;
    left_up_direction = false;
    auto el = GetElement();
    if (!el)
        return;
    Rect r = el->GetAbsoluteRect(el->GetCaretRect(GetPos()));
    if (last_x_element && (!selection || selection->IsEmpty()))
    {
        //fix x position
        Rect x_rect = last_x_element->GetAbsoluteRect(last_x_element->GetCaretRect(last_x_pos));
        if (last_pos)
            r.left = x_rect.GetRight();
        else
            r.left = x_rect.left;
    }
    CaretState c;
    if (el->GetBottomCaretState(r.left, r.GetBottom() - 2, c, selection))
        SetState(c, false);
}

void Caret::MoveWordLeft(Selection* selection)
{
    if (block)
        return;
    left_up_direction = true;
    CaretState c = GetCaretState();
    auto el = GetElement();
    if (!el)
        return;
    if (el->GetWordLeftCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveWordRight(Selection* selection)
{
    if (block)
        return;
    left_up_direction = false;
    CaretState c = GetCaretState();
    auto el = GetElement();
    if (!el)
        return;
    if (el->GetWordRightCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MovePageUp(Selection* selection)
{
    if (block)
        return;
    left_up_direction = true;
    auto el = GetElement();
    if (!el)
        return;
    auto paragraph = document->FindParent(el->id, ElementType::PARAGRAPH);
    auto row = document->FindElementOrParent(el->id, ElementType::ROW);
    Rect view_port = window->GetViewPort(0);
    Point p = window->GetDocumentPoint();
    Rect cur = document->GetCaretRect(GetCaretState());

    CaretState next, last;
    int paragraph_pos = document->text->elements->GetChildPos(paragraph->id);
    int row_pos = paragraph->elements->GetChildPos(row->id);
    for (int i = paragraph_pos; i >= 0; --i)
    {
        paragraph = document->text->elements->Get(i);
        if (!paragraph->visible)
            continue;
        for (int j = (row_pos == -1 ? paragraph->elements->Count() - 1 : row_pos); j >= 0; --j)
        {
            row = paragraph->elements->Get(j);
            if (selection)
            {
                if (j == row_pos)
                {
                    CaretState c = GetCaretState();
                    int p = GetChildPos(row->id, c.id);
                    auto _el = document->GetElement(GetChild(row->id, p));
                    if (document->IsString(_el))
                        selection->Add(_el->id, 0, c.id[row->id.size() + 1]);
                    if (p > 0)
                        selection->Add(row->id, 0, p);
                }
                else
                    selection->Add(paragraph, j, 1);
            }

            Rect r = row->GetAbsoluteRect();
            if (r.top <= cur.top - view_port.height)
            {
                if (!row->GetFirstCaretState(next, nullptr) || !row->GetLastCaretState(last, nullptr))
                    continue;
                while (next != last)
                {
                    Rect r = document->GetCaretRect(next);
                    if (r.GetRight() > cur.left)
                        break;
                    ElementPtr el = document->GetParent(next.id);
                    if (!el->GetRightCaretState(next, selection))
                        break;
                }

                SetState(next);
                window->MoveDocument(p.x, p.y - view_port.height);
                document->UpdateCaretView();
                return;
            }
        }
        row_pos = -1;
    }

    MoveToDocumentBegin(nullptr);
}

void Caret::MovePageDown(Selection* selection)
{
    if (block)
        return;
    left_up_direction = false;
    auto el = GetElement();
    if (!el)
        return;
    auto paragraph = document->FindParent(el->id, ElementType::PARAGRAPH);
    auto row = document->FindElementOrParent(el->id, ElementType::ROW);
    Rect view_port = window->GetViewPort(0);
    Point p = window->GetDocumentPoint();
    Rect cur = document->GetCaretRect(GetCaretState());

    CaretState next, last;
    int paragraph_pos = document->text->elements->GetChildPos(paragraph->id);
    int row_pos = paragraph->elements->GetChildPos(row->id);
    for (int i = paragraph_pos; i < document->text->elements->Count(); ++i)
    {
        paragraph = document->text->elements->Get(i);
        for (int j = row_pos; j < paragraph->elements->Count(); ++j)
        {
            row = paragraph->elements->Get(j);
            Rect r = row->GetAbsoluteRect();
            if (selection)
            {
                if (j == row_pos && i == paragraph_pos)
                {
                    CaretState c = GetCaretState();
                    if (!c.last_pos)
                    {
                        int p = GetChildPos(row->id, c.id);
                        auto _el = document->GetElement(GetChild(row->id, p));
                        if (document->IsString(_el))
                            selection->Add(_el, c.id[row->id.size() + 1], _el->elements->Count() - c.id[row->id.size() + 1]);
                        else
                            selection->Add(_el->id);
                        if (row->elements->Count() > p + 1)
                            selection->Add(row->id, p + 1, row->elements->Count() - p - 1);
                    }
                }
                else if (r.GetBottom() < cur.top + view_port.height)
                    selection->Add(paragraph, j, 1);
            }

            if (r.GetBottom() >= cur.top + view_port.height)
            {
                if (!row->GetFirstCaretState(next, nullptr) || !row->GetLastCaretState(last, nullptr))
                    continue;
                while (next != last)
                {
                    Rect r = document->GetCaretRect(next);
                    if (r.GetRight() > cur.left)
                        break;
                    ElementPtr el = document->GetParent(next.id);
                    if (!el->GetRightCaretState(next, selection))
                        break;
                }

                SetState(next);
                window->MoveDocument(p.x, p.y + view_port.height);
                document->UpdateCaretView();
                return;
            }
        }
        row_pos = 0;
    }

    MoveToDocumentEnd(nullptr);
}

void Caret::SelectOut(Selection* selection)
{
    if (block)
        return;
    auto el = GetElement();
    if (!el)
        return;
    CaretState c = GetCaretState();
    if (el->GetSelectOutCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

bool Caret::IsInsideElement(const ElementId& id)
{
    if (block)
        return false;
    auto el = GetElement();
    if (!el)
        return false;
    if (document->IsString(el->id))
        return id == el->id || IsChild(id, el->id);
    else if (document->IsRow(el->id) && id == el->id)
        return true;
    else if (last_pos && id == el->id)
        return true;
    return IsChild(id, el->id);
}

bool Caret::IsOnElement(const ElementId& id)
{
    if (block)
        return false;
    auto el = GetElement();
    if (!el)
        return false;
    ElementId _id = el->id;
    _id.push_back(GetPos());
    if (_id == id)
        return true;
    if (last_pos && GetPrevPos(_id) == id)
        return true;
    return false;
}

void Caret::UpdateXPos()
{
    if (block || !element)
        return;
    if (document->IsString(element->id))
    {
        last_x_element = element.get();
        last_x_pos = str_pos;
    }
    else
    {
        last_x_element = element->parent;
        last_x_pos = GetChildPos(element->id);
    }
}

void Caret::Update()
{
    if (!element || block)
        return;
    Element* el = GetElement();
    if (!el)
        return;
    int p = GetPos();
    if (el->elements->Count() <= p)
        return;
    CaretState c(el, p, last_pos);
    SetState(c);
}

void Caret::Reset()
{
    element = nullptr;
    str_pos = -1;
}

}

#ifndef _WIN32
#pragma clang diagnostic pop
#endif
