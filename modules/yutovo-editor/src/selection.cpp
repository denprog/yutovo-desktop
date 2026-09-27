/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "selection.h"
#include "document.h"
#include "editor_utils.h"
#include <functional>

#ifdef _MSC_VER
#undef GetObject
#endif

namespace yutovo
{

//ElementSelection

bool ElementSelection::operator==(const ElementSelection& s) const
{
    return element == s.element && start == s.start && size == s.size;
}

bool ElementSelection::operator!=(const ElementSelection& s) const
{
    return element != s.element || start != s.start || size != s.size;
}

bool ElementSelection::operator<(const ElementSelection& s) const
{
    ElementId s_id1 = element->id;
    s_id1.push_back(start);
    ElementId s_id2 = s.element->id;
    s_id2.push_back(s.start);
    if (IsChild(s_id1, s_id2))
        return true;
    else if (s_id1 == s_id2 || IsChild(s_id2, s_id1))
        return false;

    ElementId _id = GetCommonParent(s_id1, s_id2);
    assert(_id.size() > 0 && s_id1.size() > _id.size() && s_id2.size() > _id.size());
    return s_id1[_id.size()] < s_id2[_id.size()]; //get position in common parent
}

bool ElementSelection::IsEmpty() const
{
    return element == nullptr;
}

//ElementSelectionState

bool ElementSelectionState::operator==(const ElementSelectionState& s) const
{
    return id == s.id && start == s.start && size == s.size;
}

bool ElementSelectionState::operator!=(const ElementSelectionState& s) const
{
    return id != s.id || start != s.start || size != s.size;
}

//ElementLogicalSelectionState

void ElementLogicalSelectionState::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    rapidjson::Value obj(rapidjson::kObjectType);
    rapidjson::Value _id(LogicalIdToString(id).c_str(), alloc);
    obj.AddMember("id", _id, alloc);
    obj.AddMember("start", start, alloc);
    obj.AddMember("size", size, alloc);
    value.PushBack(obj, alloc);
}

bool ElementLogicalSelectionState::FromJson(const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.HasMember("id") || !value["id"].IsString())
        return false;
    id = LogicalIdFromString(value["id"].GetString());
    if (!value.HasMember("start") || !value["start"].IsInt())
        return false;
    start = value["start"].GetInt();
    if (!value.HasMember("size") || !value["size"].IsInt())
        return false;
    size = value["size"].GetInt();
    return true;
}

//SelectionState

SelectionState::SelectionState(const ElementId id, uint start, uint size)
{
    Add(id, start, size);
}

bool SelectionState::operator==(const SelectionState& compare) const
{
    if (state.size() != compare.state.size())
        return false;
    for (const ElementSelectionState& s : compare.state)
    {
        auto it = std::find_if(state.begin(), state.end(), 
            [s](auto& t)
            {
                return t.id == s.id && t.start == s.start && t.size == s.size;
            });
        if (it == state.end())
            return false;
    }
    return true;
}

bool SelectionState::operator!=(const SelectionState& compare) const
{
    return !(state == compare.state);
}

void SelectionState::Add(const ElementId id, uint start, uint size)
{
    state.push_back(ElementSelectionState{id, start, size});
}

void SelectionState::Add(const ElementSelectionState& s)
{
    state.push_back(s);
}

void SelectionState::Merge(const SelectionState& s)
{
    for (auto& t : s.state)
        Add(t);
}

ElementId SelectionState::GetCommonElement() const
{
    std::vector<ElementId> ids;
    for (int i = 0; i < state.size(); ++i)
        ids.push_back(state[i].id);
    return GetCommonParent(ids);
}

ElementId SelectionState::GetCommonElement(uint& start, uint& size) const
{
    ElementId parent_id = GetCommonElement();
    auto& first = state[0];
    auto& last = state[state.size() - 1];
    if (first.id == parent_id)
        start = first.start;
    else
        start = yutovo::GetChildPos(parent_id, first.id);
    
    if (last.id == parent_id)
    {
        if (state.size() == 1)
            size = last.size;
        else
            size = last.start + last.size - start;
    }
    else
    {
        int p = yutovo::GetChildPos(parent_id, last.id);
        size = p - start + 1;
    }

    if (parent_id.size() == 2) //for paragraph take its parent
    {
        start = yutovo::GetChildPos(parent_id);
        size = 1;
        parent_id = yutovo::GetParent(parent_id);
    }

    return parent_id;
}

bool SelectionState::IsEmpty() const
{
    return state.empty();
}

#ifdef TEST
std::string SelectionState::ToString() const
{
    std::string res;
    for (size_t i = 0; i < state.size(); ++i)
    {
        const ElementSelectionState& s = state[i];
        res += "[{" + ElementIdToString(s.id) + "}," + std::to_string(s.start) + "," + std::to_string(s.size) + "]";
        if (i < state.size() - 1)
            res += ",";
    }
    return res;
}
#endif

//LogicalSelectionState

bool LogicalSelectionState::IsEmpty() const
{
    return state.empty();
}

void LogicalSelectionState::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    for (auto& s : state)
        s.ToJson(value, alloc);
}

bool LogicalSelectionState::FromJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.IsArray())
        return false;
    std::vector<ElementLogicalSelectionState> _state;
    rapidjson::Value::Array arr = value.GetArray();
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
    {
        if (!arr[i].IsObject())
            return false;
        const auto& value = arr[i].GetObject();
        ElementLogicalSelectionState s;
        if (!s.FromJson(value, alloc))
            return false;
        _state.push_back(s);
    }
    state = _state;
    return true;
}

//Selection

Selection::Selection(Document* _document) :
    document(_document)
{
}

bool Selection::operator==(const Selection& s) const
{
    if (selection.size() != s.selection.size())
        return false;
    for (const ElementSelection& s : s.selection)
    {
        auto it = std::find_if(selection.begin(), selection.end(), 
            [s](auto& t)
            {
                return t.element->id == s.element->id;
            });
        if (it == selection.end())
            return false;
        if (*it != s)
            return false;
    }
    return true;
}

bool Selection::operator!=(const Selection& s) const
{
    return !(selection == s.selection);
}

void Selection::Set(SelectionState& state)
{
    selection.clear();
    for (ElementSelectionState& s : state.state)
        Add(s.id, s.start, s.size);
}

void Selection::Set(LogicalSelectionState& state)
{
    selection.clear();
    for (ElementLogicalSelectionState& s : state.state)
    {
        std::vector<ElementPtr> elements;
        document->GetElements(s.id, elements);
        int p = 0;
        int start = s.start;
        int size = s.size;
        for (size_t i = 0; i < elements.size(); ++i)
        {
            ElementPtr _el = elements[i];
            if (_el->type == ElementType::PARAGRAPH)
            {
                for (int k = s.start; k < s.start + s.size; ++k)
                {
                    std::vector<ElementPtr> _elements;
                    LogicalId _id = s.id;
                    _id.push_back(k);
                    document->GetElements(_id, _elements);
                    for (size_t j = 0; j < _elements.size(); ++j)
                    {
                        auto _el = _elements[j];
                        Add(_el->parent->id, GetChildPos(_el->id), 1);
                    }
                }
            }
            else
            {
                int c = (int)_el->elements->Count();
                if (start < c + p)
                {
                    if (size <= c - (start - p))
                    {
                        Add(_el->id, start - p, size);
                        break;
                    }
                    else
                    {
                        Add(_el->id, start - p, c - start + p);
                        size -= c - start + p;
                        start = 0;
                    }
                    p = 0;
                }
                else
                {
                    p += _el->elements->Count();
                }
            }
        }
    }
}

void Selection::Add(const ElementPtr element, uint start, uint size)
{
    if (!element)
        return;

    auto it = std::find_if(selection.begin(), selection.end(), 
        [element](ElementSelection& s)
        {
            return s.element == element;
        });

    if (it == selection.end())
    {
        //remove selections which will be included in the new item
        for (int i = start; i < start + size; ++i)
        {
            ElementSelection s;
            auto el = element->elements->Get(i);
            if (el && HasChild(el->id, s))
                Remove(s.element->id, s.start, s.size);
        }

        bool decomposed = false;
        for (size_t i = 0; i < selection.size(); ++i)
        {
            if (Decompose(selection[i], element->id))
            {
                selection.erase(selection.begin() + i);

                auto it = std::find_if(selection.begin(), selection.end(), 
                    [element](const ElementSelection& s)
                    {
                        return s.element->id == element->id;
                    });
                ElementSelection& cur = *it;

                //substract or add selection
                if (start == cur.start && start + size <= cur.start + cur.size)
                {
                    cur.start += size;
                    cur.size -= size;
                }
                else if (start >= cur.start && start + size == cur.start + cur.size)
                {
                    cur.size -= size;
                }
                else
                {
                    if (start < cur.start)
                        cur.start = start;
                    if (start + size > cur.start + cur.size)
                        cur.size = start + size - cur.start;
                }

                decomposed = true;
                break;
            }
        }

        if (!decomposed)
            selection.emplace_back(ElementSelection{element, start, size}); //add new selection
    }
    else
    {
        //append or substract the selection or add a new one
        if (start < it->start && start + size >= it->start)
        {
            it->size = it->start - start + it->size;
            it->start = start;
        }
        else if (start < it->start && start + size > it->start + it->size)
        {
            it->start = start;
            it->size = size;
        }
        else if (start == it->start && size <= it->size)
        {
            it->start = start + size;
            it->size -= size;
        }
        else if (start > it->start && start < it->start + it->size && start + size == it->start + it->size)
        {
            it->size -= size;
        }
        else if (start > it->start && start + size < it->start + it->size)
        {
            it->size = start - it->start;
            selection.emplace_back(ElementSelection{element, start + size, it->start + it->size - start - size});
        }
        else if (start >= it->start && start <= it->start + it->size && start + size > it->start + it->size)
        {
            it->size = start - it->start + size;
        }
        else
        {
            selection.emplace_back(ElementSelection{element, start, size});
            it = selection.end() - 1;
        }

        if (it->size == 0)
            selection.erase(it);
    }
    
    std::sort(selection.begin(), selection.end());

    if (!document->pasting)
        Optimize();
}

void Selection::Add(const ElementId id, uint start, uint size)
{
    Add(document->GetElement(id), start, size);
}

void Selection::Add(const ElementId id)
{
    ElementId p = GetParent(id);
    Add(document->GetElement(p), GetChildPos(p, id), 1);
}

void Selection::Remove(const ElementId id, uint start, uint size)
{
    for (size_t i = 0; i < selection.size(); ++i)
    {
        ElementSelection& s = selection[i];
        if (s.element->id == id)
            break;
        if (Decompose(s, id))
        {
            selection.erase(selection.begin() + i);
            break;
        }
    }

    auto it = std::find_if(selection.begin(), selection.end(), 
        [id](auto& s)
        {
            return s.element->id == id;
        });
    if (it == selection.end())
        return;
    
    ElementSelection& s = *it;
    if (s.start >= start && s.start + s.size <= start + size)
        selection.erase(it);
    else if (s.start < start && s.start + s.size > start)
        s.size = start - s.start;
    else if (s.start > start && s.start < start + size)
        s.start = start + size;
    else if (s.start == start && s.size >= size)
    {
        s.start = start + size;
        s.size = s.size - size;
    }

    Optimize();
}

void Selection::InsertElement(const ElementId id)
{
    if (selection.empty() || id.empty())
        return;
    ElementId p_id = GetParent(id);
    auto it = std::find_if(selection.begin(), selection.end(), 
        [p_id](auto& s)
        {
            return s.element->id == p_id;
        });
    if (it == selection.end())
        return;
    
    auto& s = *it;
    if (s.start >= GetChildPos(id))
        ++s.start;
}

void Selection::RemoveElement(const ElementId id)
{
    if (selection.empty() || id.empty())
        return;
    ElementId p_id = GetParent(id);
    for (auto& s : selection)
    {
        if (s.element->id == p_id && s.start >= GetChildPos(id))
            --s.start;
    }
}

bool Selection::Has(const ElementPtr element, uint& start, uint& size) const
{
    return Has(element->id, start, size);
}

bool Selection::Has(const ElementId id, uint& start, uint& size) const
{
    ElementSelection s;
    if (!Has(id, s))
        return false;
    start = s.start;
    size = s.size;
    return true;
}

bool Selection::Has(const ElementId id, ElementSelection& s) const
{
    auto it = std::find_if(selection.begin(), selection.end(), 
        [id](auto& s)
        {
            if (s.element->id == id)
                return true;
            for (int i = s.start; i < s.start + s.size; ++i)
            {
                ElementId child_id = GetChild(s.element->id, i);
                if (child_id == id || IsChild(child_id, id))
                    return true;
            }
            return false;
        });
    if (it == selection.end())
        return false;
    if (it->element->id == id)
        s = *it;
    else
    {
        //parent element is selected, so select the whole this element
        auto el = document->GetElement(id);
        if (!el)
            return false;
        s.start = 0;
        s.size = el->elements->Count();
    }
    return true;
}

bool Selection::HasChild(const ElementId id, ElementSelection& s) const
{
    auto it = std::find_if(selection.begin(), selection.end(), 
        [id](auto& s)
        {
            for (int i = s.start; i < s.start + s.size; ++i)
            {
                if (IsChild(id, s.element->id) || id == s.element->id)
                    return true;
            }
            return false;
        });
    if (it == selection.end())
        return false;
    s = *it;
    return true;
}

bool Selection::IsSelected(const ElementId id) const
{
    auto it = std::find_if(selection.begin(), selection.end(), 
        [id](const ElementSelection& s)
        {
            for (int i = s.start; i < s.start + s.size; ++i)
            {
                ElementId ch = GetChild(s.element->id, i);
                if (ch == id || IsChild(ch, id))
                    return true;
            }
            return false;
        });
    return it != selection.end();
}

bool Selection::IsFirstSelected(const ElementId id) const
{
    if (selection.empty())
        return false;
    auto el = selection[0].element;
    if (el->id == id || IsChild(el->id, id))
        return true;
    return false;
}

bool Selection::IsLastSelected(const ElementId id) const
{
    if (selection.empty())
        return false;
    auto el = selection[selection.size() - 1].element;
    if (el->id == id || IsChild(el->id, id))
        return true;
    return false;
}

bool Selection::IsSelectionAbove(const ElementId id) const
{
    if (selection.empty())
        return false;
    auto el = document->GetElement(id);
    while (el && el->parent)
    {
        int p = el->parent->elements->GetElementPos(id);
        if (p > 0)
        {
            auto _el = el->parent->elements->Get(p - 1);
            return IsSelected(_el->id);
        }
        el = document->GetElement(el->parent->id);
    }
    return false;
}

bool Selection::IsSelectionBelow(const ElementId id) const
{
    if (selection.empty())
        return false;
    auto el = document->GetElement(id);
    while (el && el->parent)
    {
        int p = el->parent->elements->GetElementPos(id);
        if (p < el->parent->elements->Count() - 1)
        {
            auto _el = el->parent->elements->Get(p + 1);
            return IsSelected(_el->id);
        }
        el = document->GetElement(el->parent->id);
    }
    return false;
}

void Selection::Optimize()
{
    bool optimize = true;
    while (optimize)
    {
        optimize = false;

        //make parent selections if child selections have full size
        for (size_t i = 0; i < selection.size(); ++i)
        {
            ElementSelection& s1 = selection[i];
            if (s1.start == 0 && s1.size == s1.element->elements->Count())
            {
                if (!s1.element->parent)
                    continue;
                int pos = s1.element->parent->elements->GetChildPos(s1.element->id);
                auto p = document->GetElement(s1.element->parent->id);
                selection.erase(selection.begin() + i);
                auto it = std::find_if(selection.begin(), selection.end(), 
                    [p, pos](ElementSelection& s)
                    {
                        return s.element == p && s.start <= pos && s.start + s.size > pos;
                    });
                if (it == selection.end())
                {
                    ElementSelection s{p, (uint)pos, 1};
                    selection.emplace_back(s);
                }
                optimize = true;
                break;
            }
        }

        if (optimize)
        {
            std::sort(selection.begin(), selection.end());
            continue;
        }
        
        //try to merge selections
        for (size_t i = 0; i < selection.size(); ++i)
        {
            ElementSelection& s1 = selection[i];
            for (size_t j = i + 1; j < selection.size(); ++j)
            {
                ElementSelection& s2 = selection[j];
                if (s1.element->id == s2.element->id && s1.start + s1.size == s2.start)
                {
                    s1.size += s2.size;
                    selection.erase(selection.begin() + j);
                    optimize = true;
                    break;
                }
            }
            if (optimize)
                break;
        }

        if (!optimize)
        {
            //remove selections which are inside another selections
            for (size_t i = 0; i < selection.size(); ++i)
            {
                ElementSelection& s1 = selection[i];
                for (size_t j = 0; j < selection.size(); ++j)
                {
                    if (j == i)
                        continue;
                    ElementSelection& s2 = selection[j];
                    if (IsChild(s1.element->id, s2.element->id))
                    {
                        for (int k = s1.start; k < s1.start + s1.size; ++k)
                        {
                            auto child_id = GetChild(s1.element->id, k);
                            if (IsChild(child_id, s2.element->id) || child_id == s2.element->id)
                            {
                                selection.erase(selection.begin() + j);
                                optimize = true;
                                break;
                            }
                        }
                        if (optimize)
                            break;
                    }
                }
                if (optimize)
                    break;
            }
        }

        selection.erase(std::unique(selection.begin(), selection.end()), selection.end()); //remove dublicates

        if (selection.size() == 1)
        {
            auto& s = selection[0];
            if (s.IsEmpty() || (s.element->elements->Count() > 0 && s.size == 0))
                selection.clear();
        }
    }
}

void Selection::Clear()
{
    selection.clear();
}

bool Selection::IsEmpty() const
{
    return selection.empty();
}

SelectionState Selection::GetState() const
{
    SelectionState state;
    for (auto& s : selection)
        state.state.push_back(ElementSelectionState{s.element->id, s.start, s.size});
    return state;
}

LogicalSelectionState Selection::GetLogicalState() const
{
    LogicalSelectionState state;
    for (auto& s : selection)
    {
        ElementId _id = s.element->id;
        if (_id.size() == 1) //paragraphs of text
        {
            LogicalId logical_id = document->GetLogicalId(_id);
            state.state.push_back(ElementLogicalSelectionState{logical_id, s.start, s.size});
        }
        else if (_id.size() == 2) //rows of paragraph
        {
            for (int i = s.start; i < s.start + s.size; ++i)
            {
                auto row = document->GetElement(GetChild(_id, i));
                for (int j = 0; j < row->elements->Count(); ++j)
                {
                    auto _el = row->elements->Get(j);
                    if (_el->elements->Count() > 0)
                    {
                        LogicalId start = document->GetLogicalId(_el->id, 0);
                        LogicalId end = document->GetLogicalId(_el->id, _el->elements->Count() - 1);
                        state.state.push_back(ElementLogicalSelectionState{_el->logical_id, (uint)GetChildPos(start), 
                            uint(GetChildPos(end) - GetChildPos(start)) + 1});
                    }
                    else
                    {
                        state.state.push_back(ElementLogicalSelectionState{yutovo::GetParent(_el->logical_id), 
                            (uint)yutovo::GetChildPos(_el->logical_id), 1});
                    }
                }
            }
        }
        else if (_id.size() == 3) //elements of row
        {
            LogicalId _logical_id;
            for (int j = s.start; j < s.start + s.size; ++j)
            {
                auto s_id = _id;
                s_id.push_back(j);
                LogicalId logical_id = document->GetLogicalId(s_id);
                std::vector<ElementPtr> _elements;
                document->GetElements(logical_id, _elements);
                if (_elements.size() == 1)
                    state.state.push_back(ElementLogicalSelectionState{yutovo::GetParent(logical_id), (uint)yutovo::GetChildPos(logical_id), 1});
                else
                {
                    for (auto _el : _elements)
                    {
                        if (IsChild(_id, _el->id))
                        {
                            state.state.push_back(ElementLogicalSelectionState{logical_id, 0, _el->elements->Count()});
                            break;
                        }
                    }
                }
            }
        }
        else //others
        {
            _id.push_back(s.start);
            LogicalId logical_id = document->GetLogicalId(_id);
            state.state.push_back(ElementLogicalSelectionState{yutovo::GetParent(logical_id), (uint)yutovo::GetChildPos(logical_id), s.size});
        }
    }

    //Normalize
    for (int i = 1; i < state.state.size();)
    {
        auto& s1 = state.state[i - 1];
        auto& s2 = state.state[i];
        if (s1.id == s2.id)
        {
            s1.size += s2.size;
            state.state.erase(state.state.begin() + i);
        }
        else
            ++i;
    }
    return state;
}

CaretState Selection::GetFirstCaretState() const
{
    assert(!selection.empty());
    SelectionState s = GetState();
    ElementSelectionState el_s = s.state[0];
    auto el = document->GetElement(el_s.id);
    CaretState c;
    if (document->IsString(el))
        return CaretState(el->id, el_s.start);
    if (document->IsRow(el))
    {
        if (el_s.start == 0 && el->GetFirstCaretState(c, nullptr))
            return c;
        if (document->IsString(el->elements->Get(el_s.start)))
            return CaretState(el->elements->Get(el_s.start)->id, 0);
        return CaretState(el_s.id, el_s.start);
    }
    if (el->type == ElementType::TEXT || document->IsParagraph(el))
    {
        auto _el = el->elements->Get(el_s.start);
        if (_el->GetFirstCaretState(c, nullptr))
            return c;
    }
    if (el->elements->Get(el_s.start)->GetFirstCaretState(c, nullptr))
        return c;
    if (el->GetFirstCaretState(c, nullptr))
        return c;
    return CaretState(el_s.id, el_s.start);
}

CaretState Selection::GetLastCaretState() const
{
    assert(!selection.empty());
    SelectionState s = GetState();
    auto& el_s = s.state[s.state.size() - 1];
    auto el = document->GetElement(el_s.id);
    CaretState c;
    if (el_s.start + el_s.size == el->elements->Count())
    {
        if (el->GetLastCaretState(c, nullptr))
            return c;
    }
    else if (document->IsRow(el))
    {
        el = el->elements->Get(el_s.start + el_s.size);
        if (el->HasCaretState())
            return CaretState(el_s.id, el_s.start + el_s.size);
        if (el->GetFirstCaretState(c, nullptr))
            return c;
    }
    else if (document->IsString(el))
    {
        return CaretState(el_s.id, el_s.start + el_s.size);
    }
    if (el->elements->Get(el_s.start + el_s.size - 1)->GetLastCaretState(c, nullptr))
        return c;
    return CaretState(el_s.id, el_s.start + el_s.size, el_s.start + el_s.size == el->elements->Count() ? true : false);
}

bool Selection::Decompose(ElementSelection s, ElementId until_id)
{
    if (s.element->id == until_id)
    {
        selection.emplace_back(ElementSelection{s.element, 0, s.element->elements->Count()});
        return true;
    }
    if (!IsChild(s.element->id, until_id))
        return false;
    for (int i = s.start; i < s.start + s.size; ++i)
    {
        auto el = s.element->elements->Get(i);
        if (!el)
            return false;
        if (IsChild(el->id, until_id) || el->id == until_id)
        {
            if (Decompose(ElementSelection{el, 0, el->elements->Count()}, until_id))
            {
                for (int j = i + 1; j < s.start + s.size; ++j)
                {
                    auto _el = s.element->elements->Get(j);
                    if (!_el)
                        return false;
                    selection.emplace_back(ElementSelection{_el, 0, _el->elements->Count()});
                }
                return true;
            }
            return false;
        }
        else
            selection.emplace_back(ElementSelection{el, 0, el->elements->Count()});
    }
    return false;
}

}
