/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "plus.h"
#include "../document.h"

namespace yutovo
{

//Plus

Plus::Plus(Element* _parent) :
    OnlyShapeFormula(_parent, '+')
{
    type = ElementType::PLUS;
}

Plus::Plus(Document* _document) :
    OnlyShapeFormula(_document, '+')
{
    type = ElementType::PLUS;
}

Plus::Plus(const Plus& source) :
    OnlyShapeFormula(source)
{
}

Element* Plus::Clone()
{
    return new Plus(*this);
}

Element* Plus::Create(Element* _parent)
{
    return new Plus(_parent);
}

Element* Plus::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    Plus* p = nullptr;
    if (parent)
        p = new Plus(parent);
    else
        p = new Plus(document);
    if (!p->SymbolFromJson(value, alloc))
    {
        delete p;
        return nullptr;
    }
    return p;
}

std::string Plus::ToHtml() const
{
    return "<mo>+</mo>";
}

}
