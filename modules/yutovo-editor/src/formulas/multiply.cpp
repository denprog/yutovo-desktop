/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "multiply.h"
#include "../document.h"

namespace yutovo
{

//Multiply

Multiply::Multiply(Element* _parent) :
    OnlyShapeFormula(_parent, L'·')
{
    type = ElementType::MULTIPLY;
}

Multiply::Multiply(Document* _document) :
    OnlyShapeFormula(_document, L'·')
{
    type = ElementType::MULTIPLY;
}

Multiply::Multiply(const Multiply& source) :
    OnlyShapeFormula(source)
{
}

Element* Multiply::Clone()
{
    return new Multiply(*this);
}

Element* Multiply::Create(Element* _parent)
{
    return new Multiply(_parent);
}

Element* Multiply::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    Multiply* p = nullptr;
    if (parent)
        p = new Multiply(parent);
    else
        p = new Multiply(document);
    if (!p->SymbolFromJson(value, alloc))
    {
        delete p;
        return nullptr;
    }
    return p;
}

std::u32string Multiply::ToText() const
{
    return U"*";
}

std::string Multiply::ToHtml() const
{
    return "<mo>×</mo>";
}

}
