/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __CODE_STRING_H__
#define __CODE_STRING_H__

#include "../str.h"

namespace yutovo
{

class CodeString : public String
{
public:
    CodeString(Element* parent);
    CodeString(Element* parent, const std::string str, bool translate = false);
    CodeString(Element* parent, const std::string str, const StringFormatPtr _format, bool translate = false);
    CodeString(Element* parent, const std::u32string str);
    CodeString(Element* parent, const std::u32string str, const StringFormatPtr _format);
    CodeString(Document* _document);
    CodeString(Document* _document, const std::string str, const StringFormatPtr _format);
    CodeString(Document* _document, const std::u32string str, const StringFormatPtr _format);
    CodeString(const String& source);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);
    virtual Element* Create(Element* parent, const std::u32string str, const StringFormatPtr _format);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual bool Remake(bool with_elements = false);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element);
    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);
    virtual bool ChangeStringFormat(const StringFormatPtr format, bool with_undo, ElementId& changed_element);

    virtual void Draw() const;

    virtual void UpdateRect(bool with_elements = false);

    virtual Rect GetCaretRect(const uint pos) const;

    virtual Size GetTextSize(const uint pos) const;

    virtual void GetMargin(int& left, int& top, int& right, int& bottom) const;

    virtual bool IsFormula();

    virtual std::string ToHtml() const;

protected:
    void UpdateGap();
    Notation GetNotation() const;

protected:
    const int empty_rect_width = 6;
    mutable int gap = 0;
    mutable int gap_width = 0;
};

typedef std::shared_ptr<CodeString> CodeStringPtr;

}

#endif
