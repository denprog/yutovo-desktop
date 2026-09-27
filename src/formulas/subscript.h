/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __SUBSCRIPT_H__
#define __SUBSCRIPT_H__

#include "middle_shape_formula.h"

namespace yutovo
{

class Subscript : public MiddleShapeFormula
{
public:
    Subscript(Element* _parent, bool with_init = true);
    Subscript(Document* _document, bool with_init = true);
    Subscript(Document* _document, const std::string& str, const std::string& sub_str);
    Subscript(const Subscript& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual void AfterChildInsert(const ElementId child_id, bool with_undo);

    virtual void UpdateLevel(uint8_t _level);

    virtual std::string ToHtml() const;
    virtual std::u32string ToText() const;
    virtual void ToParserString(ParserString& str);
};

typedef std::shared_ptr<Subscript> SubscriptPtr;

}

#endif
