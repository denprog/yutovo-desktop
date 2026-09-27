/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "formula.h"
#include <functional>

namespace yutovo
{

#define ROOT_X_LEFT_OFFSET 2
#define ROOT_Y_OFFSET 4
#define ROOT_X_RIGHT_OFFSET 2

class Shape : public Formula
{
public:
    Shape(Element* _parent);
    Shape(Document* _document);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual bool HasCaretState();

    virtual bool GetElementAtCoords(const int x, const int y, const int margin, ElementId& _id);

public:
    std::function<void(const Rect& rect)> draw_func;
};

typedef std::shared_ptr<Shape> ShapePtr;

}

#endif
