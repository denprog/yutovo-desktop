/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __DIVISION_H__
#define __DIVISION_H__

#include "formula.h"
#include "middle_shape_formula.h"
#include "code_row.h"

namespace yutovo
{

class Division : public MiddleShapeFormula
{
public:
    Division(Element* _parent, bool with_init = true);
    Division(Document* _document, bool with_init = true);
    Division(const Division& source);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual bool AfterInsert(bool with_undo);

    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);

    virtual bool UseSelection();

    virtual std::string ToHtml() const;
    virtual std::u32string ToText() const;
    virtual void ToParserString(ParserString& str);

    void AddNumerator(ElementPtr numerator);
    void AddDenomerator(ElementPtr denomerator);

    CodeRow<>* GetNumeratorRow() const;
    CodeRow<>* GetDenominatorRow() const;

    bool IsDerivative() const;

protected:
    struct DiffMarker
    {
        int order = 0;
        std::u32string remaining;
    };

    bool IsDerivativeSymbol(const std::u32string& s) const;
    bool GetDerivativeOrderAt(CodeRow<>* row, uint pos, int& order, uint& content_start);
    bool ParseDerivativeMarker(Element* el, DiffMarker& marker) const;
    bool ParseDerivativeFraction(std::u32string& function_text, std::vector<std::pair<std::u32string, int>>& vars, int& num_order);
    bool BuildDerivativeParserString(ParserString& str);
    bool BuildDerivativeAtPointParserString(ParserString& str, bool include_assignments = true);
};

}

#endif
