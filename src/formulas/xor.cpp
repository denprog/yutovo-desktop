/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "xor.h"
#include "../document.h"

namespace yutovo
{

//Xor

Xor::Xor(Element* _parent) :
    OnlyShapeFormula(_parent, U'⊕')
{
    type = ElementType::XOR;
}

Xor::Xor(Document* _document) :
    OnlyShapeFormula(_document, U'⊕')
{
    type = ElementType::XOR;
}

Xor::Xor(const Xor& source) :
    OnlyShapeFormula(source)
{
}

Element* Xor::Clone()
{
    return new Xor(*this);
}

Element* Xor::Create(Element* _parent)
{
    return new Xor(_parent);
}

Element* Xor::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    Xor* p = nullptr;
    if (parent)
        p = new Xor(parent);
    else
        p = new Xor(document);
    if (!p->SymbolFromJson(value, alloc))
    {
        delete p;
        return nullptr;
    }
    return p;
}

std::string Xor::ToHtml() const
{
    return "<mo>⊕</mo>";
}

}
