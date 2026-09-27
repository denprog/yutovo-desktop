/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __MIDDLE_SHAPE_FORMULA_H__
#define __MIDDLE_SHAPE_FORMULA_H__

#include "formula.h"
#include "shape.h"
#include "code_row.h"

namespace yutovo
{

class MiddleShapeFormula : public Formula
{
public:
    MiddleShapeFormula(Element* _parent, bool with_init = true);
    MiddleShapeFormula(Document* _document, bool with_init = true);
    MiddleShapeFormula(const MiddleShapeFormula& source);

    virtual bool AfterFromJson();

    void Init();

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element);

    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);

    virtual bool AfterInsert(bool with_undo);

    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetWordLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetBeginCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetEndCaretState(CaretState& caret_state, Selection* select);

protected:
    CodeRow<>* GetFirst() const;
    Shape* GetShape() const;
    CodeRow<>* GetLast() const;
};

}

#endif
