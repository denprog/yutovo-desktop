/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __CARET_STATE_H__
#define __CARET_STATE_H__

#include <vector>
#include <memory>
#include <rapidjson/document.h>
#include "editor_utils.h"

namespace yutovo
{

class Element;
class Caret;

typedef std::shared_ptr<Element> ElementPtr;
typedef std::shared_ptr<Caret> CaretPtr;

struct CaretState
{
    CaretState() = default;
    CaretState(const std::vector<ElementPtr>& elements);
    CaretState(const ElementId _id);
    CaretState(const ElementId _id, const uint pos);
    CaretState(const ElementId _id, const uint pos, bool _last_pos);
    CaretState(const Element* element, const uint pos);
    CaretState(const Element* element, const uint pos, bool _last_pos);

    bool operator==(const CaretState& c) const;
    bool operator!=(const CaretState& c) const;
    bool operator<(const CaretState& c) const;

    void SetState(ElementPtr element);
    void SetState(const ElementId _id, const ElementId tail_id);
    void SetState(const ElementId _id, const uint pos);
    void SetState(const ElementId _id, const uint pos, bool _last_pos);
    void SetState(const ElementId _id);

    void SetPos(const uint pos);

    uint GetPos() const;
    ElementId GetParent() const;
    int GetPos(const ElementId& _id) const;
    int GetElementPos(const ElementId& _id) const;
    int GetPosInElement(const ElementId& _id) const;
    int GetStatePos(const ElementId& _id) const;
    ElementId GetElementAtPos(const uint pos) const;
    ElementId GetTailId(const uint pos) const;
    bool IsInsideElement(const ElementId& _id) const;

    bool IsEmpty() const;

#ifdef TEST
    std::string ToString() const;
#endif

    ElementId id;
    bool last_pos = false;
};

struct LogicalCaretState
{
    LogicalCaretState() = default;
    LogicalCaretState(const LogicalId _id);

    bool operator==(const LogicalCaretState& c) const;

    bool IsEmpty() const;

    void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    bool FromJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    LogicalId id;
};

}

#endif
