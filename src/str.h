/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __STRING_H__
#define __STRING_H__

#include <string>
#include "element.h"
#include "document.h"
#include "style.h"

namespace yutovo
{

class String : public Element
{
public:
    String(Element* parent);
    String(Element* parent, const std::string _str, bool _translate = false);
    String(Element* parent, const std::string _str, const StringFormatPtr _format, bool _translate = false);
    String(Element* parent, const std::u32string _str);
    String(Element* parent, const std::u32string _str, const StringFormatPtr _format);
    String(Document* _document);
    String(Document* _document, const std::u32string _str);
    String(Document* _document, const std::string _str, const StringFormatPtr _format);
    String(Document* _document, const std::u32string _str, const StringFormatPtr _format);

    virtual Element* Clone();
    virtual bool Copy(std::vector<ElementPtr>& copy);

    virtual void SetDocument(Document* _document);

    virtual Element* Create(Element* parent);
    virtual Element* Create(Element* parent, const std::u32string _str, const StringFormatPtr _format);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual bool Remake(bool with_elements = false);
    virtual void Normalize();
    virtual void Rescale() const;

    virtual void UpdateRect(bool with_elements = false);

    virtual bool GetElementAtCoords(const int x, const int y, const int margin, ElementId& _id);

    virtual std::string ToHtml() const;
    virtual void ToParserString(ParserString& str);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element);
    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);
    virtual bool ChangeStringFormat(const StringFormatPtr format, bool with_undo, ElementId& changed_element);

    virtual void SetString(const std::u32string& str);

    virtual bool Split(const uint width, bool split_more);
    virtual bool SplitAt(const uint pos);
    virtual bool Merge(const ElementPtr with_element);
    virtual bool CanMerge(const ElementPtr with_element);

    virtual bool AfterInsert(bool with_undo);
    virtual void BeforeDelete();
    virtual void BeforeReplace();
    virtual void AfterReplace();
    virtual void BeforePaste();

    virtual StringFormatPtr GetStringFormat() const;
    virtual void UpdateStringFormat(const StringFormatPtr base_format, const StringFormatPtr new_format);

    virtual Size GetTextSize(const uint pos) const;

    virtual bool CanContinueSelection();

    virtual void UpdateDrawRect();

    virtual void UpdateLevel(uint8_t _level);

    virtual void SetEditable(bool _editable);

    virtual void GetElements(ElementType _type, std::vector<ElementId>& _elements);
    virtual void GetElementsBelow(const ElementId from_id, ElementType _type, std::vector<ElementId>& _elements);

    virtual bool GetNearestElement(const int x, const int y, ElementId& _id, int& dist);

    virtual void ReSolve(bool if_error = false, bool force = false);

    virtual void ClearCache() const;

    void SetStretchWidth(float val);

public:
    StringFormatPtr format;
    mutable StringFormatPtr draw_format;
    bool translate = false;

protected:
    friend class StringElements;

    mutable std::map<uint, Size> size_cache; //cache of string sizes
    float stretch_width = 0; //stretch spaces when align is justify
    float last_stretch_width = 0;
    mutable Size tab_size;
};

class StringElements : public Elements
{
public:
    StringElements(Element* parent);
    StringElements(Element* parent, const std::u32string& _str);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    virtual bool FromJson(Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual Elements* Clone(Element* _parent);

    virtual void Draw() const;

    virtual ElementPtr Get(uint pos);
    virtual ElementId GetElementId(uint pos);
    
    virtual void Add(ElementPtr element);
    virtual void Insert(ElementPtr element, const uint pos);
    virtual void Remove(const ElementPtr element);
    virtual void RemoveAt(const uint pos, const int size);
    virtual void Replace(ElementPtr element, const uint pos);
    virtual void Clear();

    virtual uint Count() const;
    virtual uint Size() const;

    virtual Rect GetCaretRect(const uint pos) const;
    virtual void DrawCaret(const uint pos) const;

    virtual Rect GetRect();
    virtual Rect GetRect(const uint pos);

    virtual bool GetFirstCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLastCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetSelectOutCaretState(CaretState& caret_state, Selection* select);

    virtual std::string ToHtml() const;
    virtual std::u32string ToText() const;

protected:
    static bool IsOpenDelimiter(char32_t ch);
    static bool IsCloseDelimiter(char32_t ch);
    static bool IsDelimiter(char32_t ch);
    static bool IsSpace(char32_t ch);

private:
    friend class String;
    friend class CodeString;

    std::u32string str;
};

}

#endif
