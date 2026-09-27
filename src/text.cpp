/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "text.h"
#include "document.h"
#include "paragraph.h"
#include <assert.h>

namespace yutovo
{

//Text

Text::Text(Document* _document, TextFormatPtr _format, bool with_paragraph) : 
    Block(_document),
    format(_format)
{
    type = ElementType::TEXT;

    id.push_back(0);
    logical_id.push_back(0);

    if (with_paragraph)
        AddElement(ElementPtr(new Paragraph(this))); //text has to have at least one paragraph
}

Element* Text::Clone()
{
    return new Text(*this);
}

Element* Text::Create(Element* parent)
{
    return nullptr;
}

void Text::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    rapidjson::Value _id(ElementIdToString(id).c_str(), alloc);
    value.AddMember("id", _id, alloc);
    value.AddMember("type", (int)type, alloc);

    rapidjson::Value arr(rapidjson::kArrayType);
    for (int i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (!el->visible) //don't write included documents
            continue;
        rapidjson::Value v;
        v.SetObject();
        el->ToJson(v, alloc);
        arr.PushBack(v, alloc);
    }
    value.AddMember("elements", arr, alloc);
}

Element* Text::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    return new Text(document, document->current_text_format, false);
}

void Text::Draw() const
{
    Rect v = window->GetRect();
    window->BeginDrawOutside();
    window->DrawFillRect(v, GetBackgroundColor());
    window->EndDrawOutside();

    v.left += format->left_indent;
    v.top += format->top_indent;
    v.width -= format->right_indent + format->left_indent;
    v.height -= format->bottom_indent + format->top_indent;
    window->SetViewPort(v);

    Block::Draw();

    if (document->config.with_border)
    {
        window->BeginDrawOutside();
        window->DrawRect(Rect(v.left - 1, v.top - 1, v.width + 2, v.height + 2), document->config.page_border_color);
        window->EndDrawOutside();
    }
}

bool Text::Remake(bool with_elements)
{
    Rect v = window->GetRect();
    pixel_size.width = v.width - format->right_indent - format->left_indent;

    if (document->config.pdf) //for pdf export
    {
        v.left += format->left_indent;
        v.top += format->top_indent;
        v.width -= format->right_indent + format->left_indent;
        v.height -= format->bottom_indent + format->top_indent;
        window->SetViewPort(v);
    }

    Block::Remake(with_elements);

    int left_m = 0, top_m = 0, right_m = 0, bottom_m = 0;
    int h = format->top_indent;
    for (int i = 0; i < elements->Count(); ++i) //arrange paragraphs
    {
        if (document->break_remake)
            return false;
        ElementPtr p = elements->Get(i);
        if (!p->visible)
            continue;
        p->GetMargin(left_m, top_m, right_m, bottom_m); //consider the margins
        p->rect.Move(format->left_indent, h + top_m);
        h += p->rect.height + std::round(format->paragraph_spacing * document->config.scale) + bottom_m;
    }

    UpdateRect();

    return false;
}

Color Text::GetBackgroundColor() const
{
    return document->config.page_color;
}

void Text::UpdateRect(bool with_elements)
{
    Block::UpdateRect(with_elements);

    Rect v = window->GetRect();
    pixel_size.width = v.width - format->right_indent - format->left_indent;

    v.width -= format->right_indent + format->left_indent;
    v.height -= format->bottom_indent + format->right_indent;
    if (rect.width < v.width)
        rect.width = v.width;
    if (rect.height < v.height)
        rect.height = v.height;
}

void Text::UpdateDrawRect()
{
    Block::UpdateDrawRect();
    draw_rect.width += format->right_indent + format->left_indent;
    draw_rect.height += format->bottom_indent + format->right_indent;
}

bool Text::GetElementAtCoords(const int x, const int y, const int margin, ElementId& _id)
{
    Rect r = GetAbsoluteRect();
    if (!r.IsPointInside(x, y))
        return false;
    
    //look in the child elements
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr el = elements->Get(i);
        if (!el->visible)
            continue;
        if (el->GetElementAtCoords(x, y, margin, _id))
            return true;
    }
    _id = id;
    return true;
}

bool Text::GetNearestElement(const int x, const int y, ElementId& _id, int& dist)
{
    for (int i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (!el->visible)
            continue;
        el->GetNearestElement(x, y, _id, dist);
    }

    return true;
}

bool Text::GetNearestCaretState(const int x, const int y, CaretState& caret_state)
{
    int min_dist = std::numeric_limits<int>::max();
    ElementPtr el;
    for (int i = 0; i < elements->Count(); ++i) //TODO: binary search
    {
        auto _el = elements->Get(i);
        if (!_el->visible)
            continue;
        Rect r = _el->GetAbsoluteRect();
    	int dist = r.DistToPoint(x, y);
        if (dist < min_dist)
        {
            min_dist = dist;
            el = _el;
        }
    }

    return el->GetNearestCaretState(x, y, caret_state);
}

Rect Text::GetCaretRect(const uint pos) const
{
    return Rect{0, 0, 0, 0};
}

ParagraphFormatPtr Text::GetParagraphFormat()
{
    ParagraphFormatPtr format;
    if (document->GetCurrentParagraphFormat(format))
        return format;
    return nullptr;
}

bool Text::IsEmpty() const
{
    if (elements->Count() != 1)
        return false;
    return elements->Get(0)->IsEmpty();
}

std::string Text::ToHtml() const
{
    return "<body>" + Block::ToHtml() + "</body>";
}

Rect Text::GetAbsoluteRect() const
{
    Rect w = window->GetRect();
    Rect r = Block::GetAbsoluteRect();
    if (r.width < w.width)
        r.width = w.width;
    if (r.height < w.height)
        r.height = w.height;

    Point p = window->GetDocumentPoint();
    if (p.x + w.width > r.width)
        r.width = p.x + w.width;
    r.height += format->bottom_indent + format->right_indent;
    return r;
}

void Text::SetTextFormat(const TextFormat& _format)
{
    format = TextFormats::GetFormat(_format.paging, _format.left_indent, _format.top_indent, _format.right_indent, _format.bottom_indent, 
        _format.paragraph_spacing, _format.size);
    pixel_size = {window->ConvertToPixels(format->size.width), window->ConvertToPixels(format->size.height)};
}

}
