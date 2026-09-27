/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "caret_state.h"
#include "element.h"
#include "editor_utils.h"
#include <algorithm>

namespace yutovo
{

//CaretState

CaretState::CaretState(const std::vector<ElementPtr>& elements) :
    id(elements[0]->id)
{
}

CaretState::CaretState(const ElementId _id) :
    id(_id)
{
}

CaretState::CaretState(const Element* element, const uint pos)
{
    assert(element);
    id = element->id;
    id.push_back(pos);
}

CaretState::CaretState(const Element* element, const uint pos, bool _last_pos) :
    CaretState(element, pos)
{
    last_pos = _last_pos;
}

CaretState::CaretState(const ElementId _id, const uint pos) :
    id(_id)
{
    id.push_back(pos);
}

CaretState::CaretState(const ElementId _id, const uint pos, bool _last_pos) :
    id(_id),
    last_pos(_last_pos)
{
    id.push_back(pos);
}

bool CaretState::operator==(const CaretState& c) const
{
    return id == c.id;
}

bool CaretState::operator!=(const CaretState& c) const
{
    return id != c.id;
}

bool CaretState::operator<(const CaretState& c) const
{
    return yutovo::IsLess(id, c.id);
}

void CaretState::SetState(ElementPtr element)
{
    id = element->id;
}

void CaretState::SetState(const ElementId _id, const ElementId tail_id)
{
    id = _id;
    for (auto i : tail_id)
        id.push_back(i);
}

void CaretState::SetState(const ElementId _id, const uint pos)
{
    id = _id;
    id.push_back(pos);
}

void CaretState::SetState(const ElementId _id, const uint pos, bool _last_pos)
{
    id = _id;
    id.push_back(pos);
    last_pos = _last_pos;
}

void CaretState::SetState(const ElementId _id)
{
    id = _id;
}

void CaretState::SetPos(const uint pos)
{
    id[id.size() - 1] = pos;
}

uint CaretState::GetPos() const
{
    return id[id.size() - 1];
}

ElementId CaretState::GetParent() const
{
    ElementId _id(id);
    _id.erase(_id.end() - 1);
    return _id;
}

int CaretState::GetPos(const ElementId& _id) const
{
    if (IsInsideElement(_id))
        return GetPos();
    return -1;
}

int CaretState::GetElementPos(const ElementId& _id) const
{
    if (id.size() < _id.size() - 1)
        return -1;
    return id[_id.size() - 1];
}

int CaretState::GetPosInElement(const ElementId& _id) const
{
    if (id.size() <= _id.size())
        return -1;
    return id[_id.size()];
}

int CaretState::GetStatePos(const ElementId& _id) const
{
    if (id.size() < _id.size())
        return -1;
    size_t i = 0;
    for (; i < _id.size(); ++i)
    {
        if (_id[i] != id[i])
            return -1;
    }
    return i - 1;
}

ElementId CaretState::GetElementAtPos(const uint pos) const
{
    ElementId _id(id);
    _id.erase(_id.begin() + pos);
    return _id;
}

ElementId CaretState::GetTailId(const uint pos) const
{
    ElementId _id(id);
    _id.erase(_id.begin(), _id.begin() + pos);
    return _id;
}

bool CaretState::IsInsideElement(const ElementId& _id) const
{
    if (id.size() <= _id.size() || _id.empty())
        return false;
    for (size_t i = 0; i < _id.size(); ++i)
    {
        if (_id[i] != id[i])
            return false;
    }
    return true;
}

bool CaretState::IsEmpty() const
{
    return id.empty();
}

#ifdef TEST
std::string CaretState::ToString() const
{
    return ElementIdToString(id);
}
#endif

//LogicalCaretState

LogicalCaretState::LogicalCaretState(const LogicalId _id) :
    id(_id)
{
}

bool LogicalCaretState::operator==(const LogicalCaretState& c) const
{
    return id == c.id;
}

bool LogicalCaretState::IsEmpty() const
{
    return id.empty();
}

void LogicalCaretState::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    rapidjson::Value _id(LogicalIdToString(id).c_str(), alloc);
    value.AddMember("id", _id, alloc);
}

bool LogicalCaretState::FromJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.HasMember("id") || !value["id"].IsString())
        return false;
    id = LogicalIdFromString(value["id"].GetString());
    return true;
}

}
