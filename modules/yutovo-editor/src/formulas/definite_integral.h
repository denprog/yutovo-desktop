/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __DEFINITE_INTEGRAL_H__
#define __DEFINITE_INTEGRAL_H__

#include "formula.h"
#include "shape.h"
#include "code_row.h"

namespace yutovo
{

class CodeString;

class DefiniteIntegral : public Formula
{
public:
    DefiniteIntegral(Element* _parent, bool with_init = true);
    DefiniteIntegral(Document* _document, bool with_init = true);
    DefiniteIntegral(const DefiniteIntegral& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    void Init();

    virtual bool AfterFromJson();

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual void Normalize();

    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);

    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);

    virtual void UpdateLevel(uint8_t _level);

    virtual bool AfterInsert(bool with_undo);

    virtual std::string ToHtml() const;
    virtual std::u32string ToText() const;

    virtual void ToParserString(ParserString& str);

protected:
    CodeRow<>* GetLower() const; //lower limit of integration
    Shape* GetShape() const;
    CodeRow<>* GetUpper() const; //upper limit of integration
    CodeRow<>* GetExpression() const; //integrand
    CodeString* GetD() const; //non-editable "d" string
    CodeRow<>* GetVariable() const; //integration variable

    bool IsOnD(const CaretState& caret_state) const;
    bool IsInsideD(const CaretState& caret_state) const;
    bool SkipDLeft(CaretState& caret_state, Selection* select) const;
    bool SkipDRight(CaretState& caret_state, Selection* select) const;

protected:
    StringFormatPtr format;

    const char32_t symbol = U'∫';
    std::string symbol_str;

    static const std::string family_name;
};

}

#endif
