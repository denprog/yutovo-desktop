/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __LINK_H__
#define __LINK_H__

#include "str.h"

namespace yutovo
{

class Link : public String
{
public:
    Link(Element* parent);
    Link(Element* parent, const std::string _str, const std::string _url, bool _translate = false);
    Link(Element* parent, const std::string _str, const std::string _url, const StringFormatPtr _format, bool _translate = false);
    Link(Element* parent, const std::u32string _str, const std::u32string _url);
    Link(Element* parent, const std::u32string _str, const std::u32string _url, const StringFormatPtr _format);
    Link(Document* _document);
    Link(Document* _document, const std::u32string _str, const std::u32string _url);
    Link(Document* _document, const std::string _str, const std::string _url, const StringFormatPtr _format);
    Link(Document* _document, const std::u32string _str, const std::u32string _url, const StringFormatPtr _format);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);
    virtual Element* Create(Element* parent, const std::u32string _str, const std::u32string _url, const StringFormatPtr _format);
    virtual Element* Create(Element* parent, const std::u32string _str, const StringFormatPtr _format);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual bool Merge(const ElementPtr with_element);
    virtual bool CanMerge(const ElementPtr with_element);

    virtual bool AfterInsert(bool with_undo);

    virtual StringFormatPtr GetStringFormat() const;

    virtual std::string ToHtml() const;

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element);

    void Visit();

public:
    std::u32string url;
};

}

#endif