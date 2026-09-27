/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "shape.h"
#include "document.h"

namespace yutovo
{

//Shape

Shape::Shape(Element* _parent) : 
    Formula(_parent)
{
    type = ElementType::SHAPE;
}

Shape::Shape(Document* _document) :
    Formula(_document)
{
    type = ElementType::SHAPE;
}

Element* Shape::Clone()
{
    return new Shape(*this);
}

Element* Shape::Create(Element* parent)
{
    return new Shape(parent);
}

Element* Shape::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    return new Shape(parent);
}

void Shape::Draw() const
{
    if (draw_func)
        draw_func(GetAbsoluteRect());
    if (can_resize && caret->IsVisible())
    {
        //draw the resize items
        auto r = GetAbsoluteRect();
        window->DrawRect(r, draw_string_format->text_color);
        int m = document->config.resize_margin_width;
        window->DrawRect(Rect{r.left, r.top, m, m}, draw_string_format->text_color);
        window->DrawRect(Rect{r.GetRight() - m, r.top, m, m}, draw_string_format->text_color);
        window->DrawRect(Rect{r.GetRight() - m, r.GetBottom() - m, m, m}, draw_string_format->text_color);
        window->DrawRect(Rect{r.left, r.GetBottom() - m, m, m}, draw_string_format->text_color);
    }
}

bool Shape::Remake(bool with_elements)
{
    return false;
}

bool Shape::HasCaretState()
{
    return true;
}

bool Shape::GetElementAtCoords(const int x, const int y, const int margin, ElementId& _id)
{
    Rect r = parent->GetAbsoluteRect(GetCaretRect());
    r.left -= margin;
    r.top -= margin;
    r.width += margin * 2;
    r.height += margin * 2;
    if (r.IsPointInside(x, y))
    {
        _id = id;
        return true;
    }
    return false;
}

}
