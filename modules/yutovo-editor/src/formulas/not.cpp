/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "not.h"
#include "../document.h"

namespace yutovo
{

//Not

Not::Not(Element* _parent) :
    OnlyShapeFormula(_parent, U'¬')
{
    type = ElementType::NOT;
}

Not::Not(Document* _document) :
    OnlyShapeFormula(_document, U'¬')
{
    type = ElementType::NOT;
}

Not::Not(const Not& source) :
    OnlyShapeFormula(source)
{
}

Element* Not::Clone()
{
    return new Not(*this);
}

Element* Not::Create(Element* _parent)
{
    return new Not(_parent);
}

Element* Not::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    Not* p = nullptr;
    if (parent)
        p = new Not(parent);
    else
        p = new Not(document);
    if (!p->SymbolFromJson(value, alloc))
    {
        delete p;
        return nullptr;
    }
    return p;
}

std::string Not::ToHtml() const
{
    return "<mo>¬</mo>";
}

}
