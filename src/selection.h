/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __SELECTION_H__
#define __SELECTION_H__

#include "caret_state.h"
#include <rapidjson/document.h>

namespace yutovo
{

class Document;

struct ElementSelection
{
    bool operator==(const ElementSelection& s) const;
    bool operator!=(const ElementSelection& s) const;
    bool operator<(const ElementSelection& s) const;

    bool IsEmpty() const;

    ElementPtr element;
    uint start = 0;
    uint size = 0;
};

struct ElementSelectionState
{
    bool operator==(const ElementSelectionState& compare) const;
    bool operator!=(const ElementSelectionState& compare) const;

    ElementId id;
    uint start = 0;
    uint size = 0;
};

struct ElementLogicalSelectionState
{
    void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    bool FromJson(const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    LogicalId id;
    uint start = 0;
    uint size = 0;
};

struct SelectionState
{
    SelectionState() = default;
    SelectionState(const ElementId id, uint start, uint size);

    bool operator==(const SelectionState& s) const;
    bool operator!=(const SelectionState& s) const;

    void Add(const ElementId id, uint start, uint size);
    void Add(const ElementSelectionState& s);
    void Merge(const SelectionState& s);
    
    ElementId GetCommonElement() const;
    ElementId GetCommonElement(uint& start, uint& size) const;

    bool IsEmpty() const;

#ifdef TEST
    std::string ToString() const;
#endif

    std::vector<ElementSelectionState> state;
};

struct LogicalSelectionState
{
    bool IsEmpty() const;

    void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    bool FromJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    std::vector<ElementLogicalSelectionState> state;
};

class Selection
{
public:
    Selection(Document* _document);
    
    bool operator==(const Selection& s) const;
    bool operator!=(const Selection& s) const;

public:
    void Set(SelectionState& state);
    void Set(LogicalSelectionState& state);

    void Add(const ElementPtr element, uint start, uint size);
    void Add(const ElementId id, uint start, uint size);
    void Add(const ElementId id);
    void Remove(const ElementId id, uint start, uint size);
    void InsertElement(const ElementId id);
    void RemoveElement(const ElementId id);

    bool Has(const ElementPtr element, uint& start, uint& size) const;
    bool Has(const ElementId id, uint& start, uint& size) const;
    bool Has(const ElementId id, ElementSelection& s) const;
    bool HasChild(const ElementId id, ElementSelection& s) const;
    bool IsSelected(const ElementId id) const;
    bool IsFirstSelected(const ElementId id) const;
    bool IsLastSelected(const ElementId id) const;
    bool IsSelectionAbove(const ElementId id) const;
    bool IsSelectionBelow(const ElementId id) const;

    void Optimize();
    
    void Clear();

    bool IsEmpty() const;

    SelectionState GetState() const;
    LogicalSelectionState GetLogicalState() const;

    CaretState GetFirstCaretState() const;
    CaretState GetLastCaretState() const;

private:
    bool Decompose(ElementSelection s, ElementId until_id);

private:
    Document* document;

public:
    std::vector<ElementSelection> selection;

    bool can_optimize = true;
};

}

#endif
