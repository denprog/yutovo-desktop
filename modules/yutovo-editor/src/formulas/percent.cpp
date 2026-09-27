/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "percent.h"

namespace yutovo
{

//Percent

Percent::Percent(Element* _parent) :
    OnlyShapeFormula(_parent, '%')
{
    type = ElementType::PERCENT;
}

Percent::Percent(Document* _document) :
    OnlyShapeFormula(_document, '%')
{
    type = ElementType::PERCENT;
}

Percent::Percent(const Percent& source) :
    OnlyShapeFormula(source)
{
}

Element* Percent::Clone()
{
    return new Percent(*this);
}

Element* Percent::Create(Element* _parent)
{
    return new Percent(_parent);
}

Element* Percent::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    Percent* p = nullptr;
    if (parent)
        p = new Percent(parent);
    else
        p = new Percent(document);
    if (!p->SymbolFromJson(value, alloc))
    {
        delete p;
        return nullptr;
    }
    return p;
}

std::string Percent::ToHtml() const
{
    return "<mo>%</mo>";
}

}
