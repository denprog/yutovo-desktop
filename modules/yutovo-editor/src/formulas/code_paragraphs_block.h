/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __CODE_PARAGRAPHS_BLOCK_H__
#define __CODE_PARAGRAPHS_BLOCK_H__

#include "../block.h"

namespace yutovo
{

class Assignment;

template<typename T = void>
class CodeParagraphsBlock : public Block
{
public:
    CodeParagraphsBlock(Document* _document, bool add_empty = true);
    CodeParagraphsBlock(Element* parent, bool add_empty = true);
    CodeParagraphsBlock(const CodeParagraphsBlock& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    ParagraphFormatPtr GetParagraphFormat();

    std::string ToHtml() const;

    virtual void AddEmptyElement();

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element);
    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);

    virtual bool IsFormula();
    virtual bool IsEmpty() const;

public:
    ParagraphFormatPtr paragraph_format;
    FormulaFormatPtr formula_format;
};

}

#endif
