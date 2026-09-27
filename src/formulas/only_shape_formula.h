/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __ONLY_SHAPE_FORMULA_H__
#define __ONLY_SHAPE_FORMULA_H__

#include "formula.h"
#include "shape.h"

namespace yutovo
{

class OnlyShapeFormula : public Formula
{
public:
    OnlyShapeFormula(Element* _parent, char32_t _symbol);
    OnlyShapeFormula(Document* _document, char32_t _symbol);
    OnlyShapeFormula(const OnlyShapeFormula& source);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;
    
    virtual bool AfterInsert(bool with_undo);

    virtual bool GetFirstCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLastCaretState(CaretState& caret_state, Selection* select);

    virtual void UpdateRect(bool with_elements = false);

    virtual StringFormatPtr GetStringFormat() const;

    virtual std::u32string ToText() const;
    virtual void ToParserString(ParserString& str);

protected:
    bool SymbolFromJson(const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

protected:
    Shape *shape;
    char32_t symbol;
};

}

#endif
