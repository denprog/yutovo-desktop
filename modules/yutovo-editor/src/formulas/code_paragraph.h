/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __CODE_PARAGRAPH_H__
#define __CODE_PARAGRAPH_H__

#include "../paragraph.h"

namespace yutovo
{

//Group of code rows
template<typename T = void>
class CodeParagraph : public Paragraph
{
public:
    CodeParagraph(Element* _parent, bool with_row = true);
    CodeParagraph(Document* _document, bool with_row = true);
    CodeParagraph(const Paragraph* source);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Normalize();

    virtual void AddEmptyElement();

    virtual bool IsFormula();
    virtual bool IsEmpty() const;

    virtual bool AfterInsert(bool with_undo);

    virtual std::string ToHtml() const;

    virtual ElementPtr GetPlainRow();

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element);
    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);
};

}

#endif
