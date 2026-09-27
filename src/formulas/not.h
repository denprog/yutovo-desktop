/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __NOT_FORMULA_H__
#define __NOT_FORMULA_H__

#include "only_shape_formula.h"

namespace yutovo
{

class Not : public OnlyShapeFormula
{
public:
    Not(Element* _parent);
    Not(Document* _document);
    Not(const Not& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual std::string ToHtml() const;
};

}

#endif
