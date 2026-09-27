/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "unit.h"

namespace yutovo
{

//Unit

Unit::Unit(Element* _parent, bool with_init) : 
    Assignment(_parent, with_init, true)
{
    type = ElementType::UNIT;
    solve_sign = U"~";
    draw_sign = "~";
}

Unit::Unit(Document* _document, bool with_init) :
    Assignment(_document, with_init)
{
    type = ElementType::UNIT;
    solve_sign = U"~";
    draw_sign = "~";
}

Unit::Unit(const Unit& source) :
    Assignment(source)
{
    solve_sign = U"~";
    draw_sign = "~";
}

Element* Unit::Clone()
{
    return new Unit(*this);
}

Element* Unit::Create(Element* _parent)
{
    return new Unit(_parent);
}

Element* Unit::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new Unit(parent, false);
    return new Unit(document, false);
}

}
