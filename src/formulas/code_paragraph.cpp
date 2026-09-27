/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "code_paragraph.h"
#include "code_row.h"
#include "code_string.h"
#include "graph.h"
#include "code_paragraphs_block.h"
#include "assignment.h"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace yutovo
{

//CodeParagraph

template<typename T>
CodeParagraph<T>::CodeParagraph(Element* _parent, bool with_row) :
    Paragraph(_parent, false)
{
    type = ElementType::CODE_PARAGRAPH;
    if (with_row)
        AddEmptyElement();
}

template<typename T>
CodeParagraph<T>::CodeParagraph(Document* _document, bool with_row) :
    Paragraph(_document, false)
{
    type = ElementType::CODE_PARAGRAPH;
    format = document->paragraph_formats->GetFormat("Code", document->config.language);
    if (with_row)
        AddEmptyElement();
}

template<typename T>
CodeParagraph<T>::CodeParagraph(const Paragraph* source) :
    Paragraph(source->document, false)
{
    type = ElementType::CODE_PARAGRAPH;
    format = document->paragraph_formats->GetFormat("Code", document->config.language);
    if (source->IsEmpty())
    {
        AddEmptyElement();
    }
    else
    {
        for (int i = 0; i < source->elements->Count(); ++i)
        {
            ElementPtr row(new CodeRow<>(this));
            ElementPtr el = source->elements->Get(i);
            for (int j = 0; j < el->elements->Count(); ++j)
            {
                auto r = el->elements->Get(j);
                if (r->type == ElementType::STRING)
                {
                    //change type of string
                    String* str = (String*)r.get();
                    row->elements->Add(ElementPtr(new CodeString(*str)));
                }
                else
                    row->elements->Add(r);
            }
            elements->Add(row);
        }
    }
}

template<typename T>
Element* CodeParagraph<T>::Clone()
{
    return new CodeParagraph<T>(*this);
}

template<typename T>
Element* CodeParagraph<T>::Create(Element* parent)
{
    return new CodeParagraph<T>(parent, true);
}

template<typename T>
void CodeParagraph<T>::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Paragraph::ToJson(value, alloc);
}

template<typename T>
Element* CodeParagraph<T>::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    CodeParagraph<T>* p = nullptr;
    if (parent)
        p = new CodeParagraph<T>(parent, false);
    else
        p = new CodeParagraph<T>(document, false);
    if (value.HasMember("format_name") && value["format_name"].IsString())
    {
        auto format_name = value["format_name"].GetString();
        auto f = document->paragraph_formats->GetFormat(format_name, document->config.language);
        if (f)
            p->format = f;
    }

    if (value.HasMember("marker") && value["marker"].IsString())
        p->marker = ToUtfString(value["marker"].GetString());
    if (value.HasMember("marker_format_id") && value["marker_format_id"].IsString())
    {
        auto format_id_str = value["marker_format_id"].GetString();
        boost::uuids::uuid format_id;
        try
        {
            format_id = boost::lexical_cast<boost::uuids::uuid>(format_id_str);
        }
        catch (std::bad_cast& ex)
        {
            return nullptr;
        }
        p->marker_format = document->GetStringFormat(format_id);
    }
    return p;
}

template<typename T>
void CodeParagraph<T>::AddEmptyElement()
{
    AddElement(ElementPtr(new CodeRow<>(this, true)));
}

template<typename T>
void CodeParagraph<T>::Normalize()
{
    Paragraph::Normalize();

    for (int i = 1; i < elements->Count();)
    {
        auto el1 = elements->Get(i - 1);
        auto el2 = elements->Get(i);
        if (el1->type == ElementType::CODE_ROW && el2->type == ElementType::CODE_ROW)
        {
            //merge the two rows
            if (!el1->Merge(el2))
                ++i;
        }
        else
            ++i;
    }
}

template<typename T>
bool CodeParagraph<T>::AfterInsert(bool with_undo)
{
    StringFormatPtr f = GetStringFormat();
    ElementPtr graph = document->FindParent(id, ElementType::GRAPH_LINE);
    if (graph)
    {
        Color color;
        uint width = 1;
        ((GraphLine*)graph.get())->GetPlotFormat(yutovo::GetChildPos(id), color, width);
        SetMarker(U"█", document->GetStringFormat(f->family, f->size, f->bold, f->italic, f->underline, f->strikethrough,
            f->subscript, f->superscript, color, f->text_bg_color, f->text_bg_selection_color));
    }
    return true;
}

template<typename T>
bool CodeParagraph<T>::IsFormula()
{
    return true;
}

template<typename T>
bool CodeParagraph<T>::IsEmpty() const
{
    return Paragraph::IsEmpty();
}

template<typename T>
std::string CodeParagraph<T>::ToHtml() const
{
    std::string s = "<math xmlns='http://www.w3.org/1998/Math/MathML'>";
    s += Element::ToHtml();
    s += "</math>";
    return s;
}

template<typename T>
ElementPtr CodeParagraph<T>::GetPlainRow()
{
    ElementPtr row(new CodeRow<>(parent, false));
    for (int i = 0; i < elements->Count(); ++i)
    {
        auto r = elements->Get(i);
        for (int j = 0; j < r->elements->Count(); ++j)
        {
            auto _el = r->elements->Get(j);
            row->elements->Add(_el);
        }
    }
    return row;
}

template<typename T>
bool CodeParagraph<T>::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    return Paragraph::InsertElements(_elements, insert_mode, with_undo, changed_element);
}

template<typename T>
bool CodeParagraph<T>::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    return Paragraph::DeleteElements(left, with_undo, changed_element);
}

//Explicit specializations for Assignment

template<>
void CodeParagraph<Assignment>::AddEmptyElement();

template<>
CodeParagraph<Assignment>::CodeParagraph(Element* _parent, bool with_row) :
    Paragraph(_parent, false)
{
    type = ElementType::CODE_PARAGRAPH_ASSIGNMENT;
    if (with_row)
        AddEmptyElement();
}

template<>
CodeParagraph<Assignment>::CodeParagraph(Document* _document, bool with_row) :
    Paragraph(_document, false)
{
    type = ElementType::CODE_PARAGRAPH_ASSIGNMENT;
    format = document->paragraph_formats->GetFormat("Code", document->config.language);
    if (with_row)
        AddEmptyElement();
}

template<>
CodeParagraph<Assignment>::CodeParagraph(const Paragraph* source) :
    Paragraph(source->document, false)
{
    type = ElementType::CODE_PARAGRAPH_ASSIGNMENT;
    format = document->paragraph_formats->GetFormat("Code", document->config.language);
    AddEmptyElement();
}

template<>
void CodeParagraph<Assignment>::Normalize()
{
    Paragraph::Normalize();
    if (elements->Count() == 0)
    {
        AddEmptyElement();
        return;
    }
    //keep the invariant: each row is a CodeRow<Assignment> with exactly one Assignment
    for (int i = 0; i < elements->Count();)
    {
        auto row = elements->Get(i);
        if (row->type != ElementType::CODE_ROW_ASSIGNMENT)
        {
            elements->RemoveAt(i, 1);
            continue;
        }
        ++i;
    }
    if (elements->Count() == 0)
        AddEmptyElement();
}

template<>
bool CodeParagraph<Assignment>::AfterInsert(bool with_undo)
{
    return false;
}

template<>
ElementPtr CodeParagraph<Assignment>::GetPlainRow()
{
    ElementPtr row(new CodeRow<Assignment>(parent, false));
    for (int i = 0; i < elements->Count(); ++i)
    {
        auto r = elements->Get(i);
        for (int j = 0; j < r->elements->Count(); ++j)
        {
            auto _el = r->elements->Get(j);
            row->elements->Add(_el);
        }
    }
    return row;
}

template<>
bool CodeParagraph<Assignment>::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    if (_elements.size() != 1)
        return false;

    auto el = _elements[0];
    if (document->IsParagraph(el))
        return parent->InsertElements(_elements, insert_mode, with_undo, changed_element);

    if (document->IsRow(el))
    {
        if (el->type != ElementType::CODE_ROW_ASSIGNMENT)
            return false;
        if (!Element::InsertElements(_elements, insert_mode, with_undo, changed_element))
            return false;
        Normalize();
        changed_element = id;
        return true;
    }

    return false;
}

template<>
bool CodeParagraph<Assignment>::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    return parent->DeleteElements(left, with_undo, changed_element);
}

template<>
void CodeParagraph<Assignment>::AddEmptyElement()
{
    AddElement(ElementPtr(new CodeRow<Assignment>(this, true)));
}

template<>
bool CodeParagraph<Assignment>::IsEmpty() const
{
    return elements->Get(0)->IsEmpty();;
}

template class CodeParagraph<void>;
template class CodeParagraph<Assignment>;

}
