/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "minus.h"
#include "../document.h"

namespace yutovo
{

//Minus

Minus::Minus(Element* _parent) :
    OnlyShapeFormula(_parent, '-')
{
    type = ElementType::MINUS;
}

Minus::Minus(Document* _document) :
    OnlyShapeFormula(_document, '-')
{
    type = ElementType::MINUS;
}

Minus::Minus(const Minus& source) :
    OnlyShapeFormula(source)
{
}

Element* Minus::Clone()
{
    return new Minus(*this);
}

Element* Minus::Create(Element* _parent)
{
    return new Minus(_parent);
}

Element* Minus::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    Minus* p = nullptr;
    if (parent)
        p = new Minus(parent);
    else
        p = new Minus(document);
    if (!p->SymbolFromJson(value, alloc))
    {
        delete p;
        return nullptr;
    }
    return p;
}

std::string Minus::ToHtml() const
{
    return "<mo>-</mo>";
}

}
