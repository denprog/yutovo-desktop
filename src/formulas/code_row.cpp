/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "code_row.h"
#include "code_string.h"
#include "assignment.h"

namespace yutovo
{

//CodeRow

template<typename T>
CodeRow<T>::CodeRow(Document* _document) :
    Row(_document)
{
    type = ElementType::CODE_ROW;
    can_merge = true;
}

template<typename T>
CodeRow<T>::CodeRow(Element* parent, bool with_string) :
    Row(parent, false)
{
    type = ElementType::CODE_ROW;
    can_merge = true;
    if (with_string)
        AddEmptyElement();
}

template<typename T>
Element* CodeRow<T>::Clone()
{
    return new CodeRow<T>(*this);
}

template<typename T>
Element* CodeRow<T>::Create(Element* parent)
{
    return new CodeRow<T>(parent, true);
}

template<typename T>
void CodeRow<T>::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Row::ToJson(value, alloc);
}

template<typename T>
Element* CodeRow<T>::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new CodeRow<T>(parent, false);
    return new CodeRow<T>(document);
}

template<typename T>
void CodeRow<T>::Normalize()
{
    Row::Normalize();

    for (size_t i = 0; i < elements->Count();)
    {
        auto el = (*elements)[i];
        if (document->IsString(el) && el->editable)
        {
            std::u32string str = el->ToText();
            if (!str.empty())
            {
                //remove lead whitespaces
                size_t pos = str.find_first_not_of(U' ');
                if (pos == std::string::npos)
                {
                    el->elements->RemoveAt(0, str.length());
                    str = ToText();
                }
                else if (pos != 0)
                {
                    el->elements->RemoveAt(0, pos);
                    str = ToText();
                    ElementPtr _el(new CodeString(this, U"", ((CodeString*)el.get())->format));
                    int p = elements->GetElementPos(el->id);
                    elements->Insert(_el, p);
                    _el->can_merge = false;
                }

                for (size_t k = 1; k < str.length();)
                {
                    int j = k;
                    int s = 0;
                    while (j < str.length() && str[j++] == U' ')
                        ++s;
                    if (s > 0)
                    {
                        //split this element
                        if (el->SplitAt(k))
                        {
                            int p = elements->GetElementPos(el->id);
                            auto n = elements->Get(p + 1);
                            n->can_merge = false;
                            n->elements->RemoveAt(0, s);
                            el = n;
                            str = el->ToText();
                            k = 1;
                            continue;
                        }
                    }
                    ++k;
                }
            }
        }
        ++i;
    }
}

template<typename T>
bool CodeRow<T>::Merge(const ElementPtr with_element)
{
    if (!can_merge || !with_element->can_merge || with_element->type != ElementType::CODE_ROW)
        return false;
    //merge the two rows
    for (int j = 0; j < with_element->elements->Count();)
        elements->Move(with_element->elements->Get(0), elements->Count());
    with_element->parent->elements->RemoveAt(yutovo::GetChildPos(with_element->id), 1);
    return true;
}

template<typename T>
bool CodeRow<T>::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    for (auto el : _elements)
    {
        if (el->type == ElementType::CODE_BLOCK)
        {
            std::vector<ElementPtr> ch;
            if (el->elements->Count() == 1)
            {
                if (el->elements->Get(0)->elements->Count() > 0 && ((CodeRow<>*)el->elements->Get(0)->elements->Get(0).get())->IsEmpty())
                    return false;
                for (int i = 0; i < el->elements->Get(0)->elements->Count(); ++i)
                    ch.push_back(el->elements->Get(0)->elements->Get(i));
            }
            else
            {
                for (int i = 0; i < el->elements->Count(); ++i)
                    ch.push_back(el->elements->Get(i));
            }

            if (!Row::InsertElements(ch, insert_mode, with_undo, changed_element))
                return false;
            return true;
        }
        if (el->type == ElementType::CODE_PARAGRAPH &&
            (parent->type != ElementType::CODE_PARAGRAPH && parent->type != ElementType::ASSIGNMENT && parent->type != ElementType::EQUATION))
        {
            return false;
        }
    }
    return Row::InsertElements(_elements, insert_mode, with_undo, changed_element);
}

template<typename T>
bool CodeRow<T>::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    return Row::DeleteElements(left, with_undo, changed_element);
}

template<typename T>
bool CodeRow<T>::GetBeginCaretState(CaretState& caret_state, Selection* select)
{
    CaretState c;
    if (Row::GetBeginCaretState(c, select))
    {
        if (c == caret_state)
            return parent->GetBeginCaretState(caret_state, select);
        caret_state = c;
        return true;
    }
    return false;
}

template<typename T>
bool CodeRow<T>::GetEndCaretState(CaretState& caret_state, Selection* select)
{
    CaretState c;
    if (Row::GetEndCaretState(c, select))
    {
        if (c == caret_state)
            return parent->GetEndCaretState(caret_state, select);
        caret_state = c;
        return true;
    }
    return false;
}

template<typename T>
void CodeRow<T>::AddEmptyElement()
{
    AddElement(ElementPtr(new CodeString(this)));
}

template<typename T>
bool CodeRow<T>::IsFormula()
{
    return true;
}

template<typename T>
bool CodeRow<T>::IsEmpty() const
{
    return Row::IsEmpty();
}

template<typename T>
std::string CodeRow<T>::ToHtml() const
{
    std::string s = "<mrow>";
    s += elements->ToHtml();
    s += "</mrow>";
    return s;
}

//Explicit specializations for Assignment

template<>
void CodeRow<Assignment>::AddEmptyElement();

template<>
CodeRow<Assignment>::CodeRow(Document* _document) :
    Row(_document)
{
    type = ElementType::CODE_ROW_ASSIGNMENT;
    can_merge = true;
}

template<>
CodeRow<Assignment>::CodeRow(Element* parent, bool with_string) :
    Row(parent, false)
{
    type = ElementType::CODE_ROW_ASSIGNMENT;
    can_merge = true;
    if (with_string)
        AddEmptyElement();
}

template<>
void CodeRow<Assignment>::Normalize()
{
    if (elements->Count() == 0)
    {
        AddEmptyElement();
        return;
    }
    Row::Normalize();
    if (elements->Count() == 0)
    {
        AddEmptyElement();
        return;
    }
    //keep the invariant: exactly one Assignment
    if (elements->Count() != 1 || elements->Get(0)->type != ElementType::ASSIGNMENT)
    {
        elements->Clear();
        AddEmptyElement();
    }
}

template<>
bool CodeRow<Assignment>::Merge(const ElementPtr with_element)
{
    return false;
}

template<>
bool CodeRow<Assignment>::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    if (_elements.size() == 1 && document->IsParagraph(_elements[0]))
        return parent->InsertElements(_elements, insert_mode, with_undo, changed_element);
    return false;
}

template<>
bool CodeRow<Assignment>::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    uint start, size;
    if (selection->Has(id, start, size) && start == 0 && size == elements->Count())
    {
        if (with_undo)
            document->StoreUndo(parent->id);
        return parent->DeleteElements(left, with_undo, changed_element);
    }
    return parent->DeleteElements(left, with_undo, changed_element);
}

template<>
void CodeRow<Assignment>::AddEmptyElement()
{
    elements->Clear();
    AddElement(ElementPtr(new Assignment(this, true, false)));
}

template<>
bool CodeRow<Assignment>::IsEmpty() const
{
    return elements->Get(0)->elements->Get(0)->IsEmpty() && elements->Get(0)->elements->Get(2)->IsEmpty();
}

template class CodeRow<void>;
template class CodeRow<Assignment>;

}
