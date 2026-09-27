/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __ELEMENT_H__
#define __ELEMENT_H__

#include <memory>
#include <fstream>
#include "window.h"
#include "editor_utils.h"
#include "caret_state.h"
#include "parser_string.h"
#include <rapidjson/document.h>
#include "hpdf.h"

namespace yutovo
{

class Document;
class Elements;
class Caret;
class Selection;
class Logger;

#define MAX_LEVEL 3

class Element;

//Base class for all the elements of the document
class Element
{
public:
    Element(Document* _document);
    Element(Element* _parent);
    Element(const Element& source);
    virtual ~Element();

    virtual Element* Clone() = 0;
    virtual bool Copy(std::vector<ElementPtr>& copy);

    virtual void SetDocument(Document* _document);

    virtual Element* Create(Element* parent) = 0;

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    virtual bool AfterFromJson();

    virtual void Draw() const;
    virtual void DrawErrorMark(const int start, const int size) const;
    virtual bool Remake(bool with_elements = false);
    virtual void Normalize();
    virtual void Rescale() const;
    virtual void MakePlain();

    virtual void Resize(const int dx, const int dy);
    virtual void MovePicture(const int dx, const int dy);
    virtual void ZoomPicture(const int pixels);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element);
    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);
    virtual bool ChangeStringFormat(const StringFormatPtr format, bool with_undo, ElementId& changed_element);
    virtual bool ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo, ElementId& changed_element);

    virtual bool Split(const uint width, bool split_more);
    virtual bool SplitAt(const uint pos);
    virtual bool Merge(const ElementPtr with_element);
    virtual bool CanMerge(const ElementPtr with_element);

    virtual void UpdateStringFormat(const StringFormatPtr base_format, const StringFormatPtr new_format);

    virtual bool AfterInsert(bool with_undo);
    virtual void AfterChildInsert(const ElementId child_id, bool with_undo);
    virtual void BeforeDelete();
    virtual void BeforeReplace();
    virtual void AfterReplace();
    virtual void BeforePaste();
    virtual void LogicalIdChanged(const LogicalId& last_id);

    virtual bool GetFirstCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLastCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBeginCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetEndCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetSelectOutCaretState(CaretState& caret_state, Selection* select);

    virtual bool HasCaretState();
    virtual bool HasLastCaretState();
    virtual bool CanContinueSelection();
    virtual bool CanContinueVerticalMoving(); //will pass caret into this element from up and down
    virtual bool UseSelection(); //delete or not selection before inserting

    virtual Rect GetCaretRect(const uint pos) const;
    virtual Rect GetCaretRect() const;
    virtual void DrawCaret(const uint pos) const;

    virtual void GetMargin(int& left, int& top, int& right, int& bottom) const;

    virtual Color GetBackgroundColor() const;

    virtual bool MouseLButtonHold(const int x, const int y, MouseHoldType& hold_type, ElementId& hold_id);
    virtual bool MouseRButtonHold(const int x, const int y, MouseHoldType& hold_type, ElementId& hold_id);

    virtual std::string ToHtml() const;
    virtual std::u32string ToText() const;
    virtual void ToParserString(ParserString& str);

    virtual void UpdateRect(bool with_elements = false);

    virtual bool GetElementAtCoords(const int x, const int y, const int margin, ElementId& _id);
    virtual bool GetNearestElement(const int x, const int y, ElementId& _id, int& dist);
    virtual bool GetNearestCaretState(const int x, const int y, CaretState& caret_state);

    virtual void AddElement(ElementPtr element);
    
    virtual void AddEmptyElement();

    uint GetChildPos(const Element* element);
    Element* GetParent(const int pos);

    Rect GetAbsoluteRect(const Rect& _rect) const;
    virtual Rect GetAbsoluteRect() const;
    Point GetAbsolutePoint(const Point& point) const;

    virtual ParagraphFormatPtr GetParagraphFormat();
    virtual StringFormatPtr GetStringFormat() const;
    virtual FormulaFormatPtr GetFormulaFormat() const;

    virtual void UpdateFormat(StringFormatPtr& _format);

    virtual void UpdateDrawRect();

    virtual void UpdateLevel(uint8_t _level);

    virtual void SetEditable(bool _editable);

    virtual int FindElement(const ElementId from_id, bool forward, const ElementType type);
    virtual void GetElements(ElementType _type, std::vector<ElementId>& _elements);
    virtual void GetLogicalElements(ElementType _type, std::vector<LogicalId>& _elements);
    virtual void GetElementsAbove(const ElementId from_id, ElementType _type, std::vector<ElementId>& _elements);
    virtual void GetElementsBelow(const ElementId from_id, ElementType _type, std::vector<ElementId>& _elements);
    virtual ElementId FindParent(const ElementType _type);

    virtual bool IsFormula();

    virtual bool IsEmpty() const;

    virtual void Solve();
    virtual void ReSolve(bool if_error = false, bool force = false);

    bool IsVisible() const;

public:
    Element* parent = nullptr;

    Document* document = nullptr;
    Window* window = nullptr;

    ElementType type = ElementType::NONE;
    ElementId id; //hierarchic unique id
    LogicalId logical_id; //hierarchic id without rows and splitting elements
    Rect rect; //relative bounding rect
    Rect last_rect; //for determining of necessity of remaking parent
    Rect draw_rect;
    int baseline = 0; //baseline for drawing
    uint8_t level = 1; //level of superscript or subscript
    
    bool editable = true; //changing string or child elements array or splitting element, doesn't affect changing font
    bool can_merge = false; //can be merged with another element of the same type
    bool error_mark = false; //has a error mark
    bool can_move_picture = false; //the picture of the element can be moved usually with mouse
    bool can_resize = false; //can be resized usually with mouse
    bool visible = true;
    bool has_caret_hilight = false; //this element is hilighted when the caret is inside
    bool has_frame_hilight = false; //this element has a colored frame

protected:
    friend class Elements;
    friend class String;
    friend class UndoBase;
    
    CaretPtr caret;
    Selection* selection = nullptr;

    bool remake_always = false;

    Logger* logger = nullptr;

public:
    std::unique_ptr<Elements> elements; //child nodes

#ifdef DEBUG
public:
    std::u32string to_str;
#endif
};

class Elements
{
public:
    Elements(Element* _parent);
    Elements(const Elements& source);

    ElementPtr operator[](const int pos);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    virtual bool FromJson(Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual Elements* Clone(Element* _parent);
    virtual void Clone(std::vector<ElementPtr>& _elements, const uint start, const uint size);
    
    virtual void Draw() const;

    virtual ElementPtr Get(uint pos);
    virtual ElementPtr Get(ElementId id);
    virtual ElementId GetElementId(uint pos);
    virtual int GetElementPos(ElementId id);
    virtual int GetChildPos(ElementId id);
    virtual bool IsFirst(ElementId id);
    virtual bool IsLast(ElementId id);
    virtual ElementId FindUpper(int y);

    virtual void Add(ElementPtr element);
    virtual void Insert(ElementPtr element, const uint pos);
    virtual void Remove(const ElementPtr element);
    virtual void Remove(const ElementId id);
    virtual void RemoveAt(const uint pos, const int size);
    virtual void Move(const ElementPtr element, const uint pos);
    virtual void Move(const Elements& _elements, const uint pos);
    virtual void Replace(ElementPtr element, const uint pos);
    virtual void Replace(const uint pos, const int size, std::vector<ElementPtr>& _elements);
    virtual void ReplaceAll(const Elements& _elements);
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
    virtual void UpdateIds();

protected:
    friend class UndoBase;
    friend class String;
    friend class Link;
    friend class StringFormats;
    
    Element* parent = nullptr;
    CaretPtr caret;
    Selection* selection = nullptr;

private:
    std::vector<ElementPtr> elements;
};

}

#endif
