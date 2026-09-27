/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "exclamation.h"
#include "../document.h"

namespace yutovo
{

//Exclamation

Exclamation::Exclamation(Element* _parent) :
    OnlyShapeFormula(_parent, '!')
{
    type = ElementType::EXCLAMATION;
}

Exclamation::Exclamation(Document* _document) :
    OnlyShapeFormula(_document, '!')
{
    type = ElementType::EXCLAMATION;
}

Exclamation::Exclamation(const Exclamation& source) :
    OnlyShapeFormula(source)
{
}

Element* Exclamation::Clone()
{
    return new Exclamation(*this);
}

Element* Exclamation::Create(Element* _parent)
{
    return new Exclamation(_parent);
}

Element* Exclamation::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    Exclamation* p = nullptr;
    if (parent)
        p = new Exclamation(parent);
    else
        p = new Exclamation(document);
    if (!p->SymbolFromJson(value, alloc))
    {
        delete p;
        return nullptr;
    }
    return p;
}

std::string Exclamation::ToHtml() const
{
    return "<mo>!</mo>";
}

}
