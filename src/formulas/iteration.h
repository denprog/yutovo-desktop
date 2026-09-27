/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __ITERATION_H__
#define __ITERATION_H__

#include "formula.h"
#include "shape.h"
#include "code_row.h"

namespace yutovo
{

class Iteration : public Formula
{
public:
    Iteration(Element* _parent, char32_t _symbol, bool with_init = true);
    Iteration(Document* _document, char32_t _symbol, bool with_init = true);
    Iteration(const Iteration& source);

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

protected:
    Assignment* GetLower() const;
    CodeRow<>* GetUpper() const;
    CodeRow<>* GetRight() const;
    Shape* GetShape() const;

protected:
    StringFormatPtr format;

    const char32_t symbol;
    std::string symbol_str;
    
    static const std::string family_name;
};

}

#endif
