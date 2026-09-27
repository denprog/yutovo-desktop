/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "and.h"
#include "../document.h"

namespace yutovo
{

//And

And::And(Element* _parent) :
    OnlyShapeFormula(_parent, U'∧')
{
    type = ElementType::AND;
}

And::And(Document* _document) :
    OnlyShapeFormula(_document, U'∧')
{
    type = ElementType::AND;
}

And::And(const And& source) :
    OnlyShapeFormula(source)
{
}

Element* And::Clone()
{
    return new And(*this);
}

Element* And::Create(Element* _parent)
{
    return new And(_parent);
}

Element* And::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    And* p = nullptr;
    if (parent)
        p = new And(parent);
    else
        p = new And(document);
    if (!p->SymbolFromJson(value, alloc))
    {
        delete p;
        return nullptr;
    }
    return p;
}

std::string And::ToHtml() const
{
    return "<mo>∧</mo>";
}

}
