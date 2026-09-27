/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __SQUARE_ROOT_H__
#define __SQUARE_ROOT_H__

#include "formula.h"
#include "shape.h"
#include "code_row.h"

namespace yutovo
{

class SquareRoot : public Formula
{
public:
    SquareRoot(Element* _parent, bool with_init = true);
    SquareRoot(Document* _document, bool with_init = true);
    SquareRoot(const SquareRoot& source);

    void Init();

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);
    virtual bool AfterFromJson();

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element);
    
    virtual bool AfterInsert(bool with_undo);
    
    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);

    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);

    virtual bool UseSelection();

    virtual std::string ToHtml() const;
    virtual std::u32string ToText() const;
    virtual void ToParserString(ParserString& str);

protected:
    CodeRow<>* GetLast() const;

protected:
    Element *last = nullptr;
    Shape *shape = nullptr;
};

}

#endif
