/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __POWER_H__
#define __POWER_H__

#include "middle_shape_formula.h"

namespace yutovo
{

class Power : public MiddleShapeFormula
{
public:
    Power(Element* _parent, bool with_init = true);
    Power(Document* _document, bool with_init = true);
    Power(const Power& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual bool AfterInsert(bool with_undo);
    
    virtual void AfterChildInsert(const ElementId child_id, bool with_undo);

    virtual void UpdateLevel(uint8_t _level);

    virtual bool UseSelection();

    virtual std::string ToHtml() const;
    virtual std::u32string ToText() const;
    virtual void ToParserString(ParserString& str);

    void AddBase(ElementPtr base);
    void AddExponent(ElementPtr exponent);

    CodeRow<>* GetBaseRow() const;
    CodeRow<>* GetExponentRow() const;
};

typedef std::shared_ptr<Power> PowerPtr;

}

#endif
