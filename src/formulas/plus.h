/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __PLUS_H__
#define __PLUS_H__

#include "only_shape_formula.h"

namespace yutovo
{

class Plus : public OnlyShapeFormula
{
public:
    Plus(Element* _parent);
    Plus(Document* _document);
    Plus(const Plus& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual std::string ToHtml() const;
};

}

#endif
