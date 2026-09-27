/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __CODE_BLOCK_H__
#define __CODE_BLOCK_H__

#include "../block.h"
#include "../style.h"

namespace yutovo
{

//Group of code paragraphs
class CodeBlock : public Block
{
public:
    CodeBlock(Document* _document, uint _code_id, bool add_empty, bool list_identifiers);
    CodeBlock(Element* parent, uint _code_id, bool add_empty, bool list_identifiers);
    CodeBlock(Document* _document, Element* parent, uint _code_id, bool add_empty = true);
    CodeBlock(const CodeBlock& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual void UpdateDrawRect();

    virtual Color GetBackgroundColor() const;

    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordLeftCaretState(CaretState& caret_state, Selection* select);

    virtual bool AfterInsert(bool with_undo);

    virtual void GetMargin(int& left, int& top, int& right, int& bottom) const;

    virtual bool HasCaretState();
    virtual bool HasLastCaretState();
    virtual bool CanContinueVerticalMoving();

    virtual StringFormatPtr GetStringFormat() const;
    virtual FormulaFormatPtr GetFormulaFormat() const;
    virtual ParagraphFormatPtr GetParagraphFormat();

    virtual std::string ToHtml() const;

    virtual void AddEmptyElement();

    virtual bool IsFormula();

public:
    uint code_id = 0; //id for unification of code blocks

    CodeFormatPtr code_format;
    ParagraphFormatPtr paragraph_format;
    FormulaFormatPtr formula_format;
};

}

#endif
