/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __BRACKETS_H__
#define __BRACKETS_H__

#include "only_shape_formula.h"

namespace yutovo
{

#define BRACES_Y_OFFSET 0.1

class OpenBracket : public OnlyShapeFormula
{
public:
    OpenBracket(Element* _parent, ElementType _type);
    OpenBracket(Document* _document, ElementType _type);
    OpenBracket(const OpenBracket& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual std::string ToHtml() const;

private:
    StringFormatPtr format;
    static const std::string family_name;
};

class CloseBracket : public OnlyShapeFormula
{
public:
    CloseBracket(Element* _parent, ElementType _type);
    CloseBracket(Document* _document, ElementType _type);
    CloseBracket(const CloseBracket& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual std::string ToHtml() const;

private:
    StringFormatPtr format;
    static const std::string family_name;
};

}

#endif
