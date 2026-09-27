/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __ROW_H__
#define __ROW_H__

#include "element.h"
#include "str.h"

namespace yutovo
{

class Row : public Element
{
public:
    Row(Document* _document);
    Row(Document* _document, Element* _parent);
    Row(Element* _parent, bool with_string = true);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual bool Remake(bool with_elements = false);
    virtual void Normalize();
    virtual void MakePlain();

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element);
    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);
    virtual bool ChangeStringFormat(const StringFormatPtr format, bool with_undo, ElementId& changed_element);

    virtual bool GetBeginCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetEndCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetWordLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordRightCaretState(CaretState& caret_state, Selection* select);

    virtual bool GetNearestCaretState(const int x, const int y, CaretState& caret_state);

    virtual Rect GetCaretRect(const uint pos) const;
    virtual void DrawCaret(const uint pos) const;

    virtual bool CanContinueSelection();

    virtual void AddEmptyElement();

    virtual bool IsEmpty() const;

public:
    ParagraphFormatPtr format; //for copy-paste
};

typedef std::shared_ptr<Row> RowPtr;

}

#endif
