/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "paragraph.h"
#include "document.h"
#include "row.h"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace yutovo
{

//Paragraph

Paragraph::Paragraph(Element* parent, bool with_row) :
    Element(parent),
    format(parent->GetParagraphFormat())
{
    type = ElementType::PARAGRAPH;

    if (!format && parent->document)
        document->GetCurrentParagraphFormat(format);
    if (format)
        current_string_format = format->default_string_format;
    else
        current_string_format = parent->GetStringFormat();
    if (!current_string_format)
        document->GetCurrentStringFormat(current_string_format);

    if (with_row)
        AddEmptyElement(); //paragraph has to have at least one row
}

Paragraph::Paragraph(Document* _document, bool with_row) :
    Element(_document)
{
    type = ElementType::PARAGRAPH;

    document->GetCurrentParagraphFormat(format);
    auto el = document->GetElement(caret->GetCaretState().id);
    if (el && el->type == ElementType::LINK)
        current_string_format = el->parent->GetStringFormat();
    else
        document->GetCurrentStringFormat(current_string_format);
    if (with_row)
        AddEmptyElement(); //paragraph has to have at least one row
}

Paragraph::Paragraph(Document* _document, ParagraphFormatPtr _format, bool with_row) :
    Element(_document),
    format(_format)
{
    type = ElementType::PARAGRAPH;

    auto el = document->GetElement(caret->GetCaretState().id);
    if (el && el->type == ElementType::LINK)
        current_string_format = el->parent->GetStringFormat();
    else
        document->GetCurrentStringFormat(current_string_format);
    if (with_row)
        AddEmptyElement(); //paragraph has to have at least one row
}

Element* Paragraph::Clone()
{
    return new Paragraph(*this);
}

Element* Paragraph::Create(Element* parent)
{
    return new Paragraph(parent);
}

void Paragraph::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Element::ToJson(value, alloc);

    rapidjson::Value _format_name(format->name.c_str(), alloc);
    value.AddMember("format_name", _format_name, alloc);
    value.AddMember("format_alignment", (int)format->alignment, alloc);
    if (!marker.empty())
    {
        rapidjson::Value _marker(ToBasicString(marker).c_str(), alloc);
        value.AddMember("marker", _marker, alloc);
        rapidjson::Value _uuid(boost::uuids::to_string(marker_format->id).c_str(), alloc);
        value.AddMember("marker_format_id", _uuid, alloc);
    }
}

Element* Paragraph::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    Paragraph* p = nullptr;
    if (parent)
        p = new Paragraph(parent, false);
    else
        p = new Paragraph(document, false);
    
    if (!value.HasMember("format_name") || !value["format_name"].IsString())
        return p;
    auto format_name = value["format_name"].GetString();
    ParagraphFormat::Alignment alignment = ParagraphFormat::Alignment::Left;
    if (value.HasMember("format_alignment") && value["format_alignment"].IsInt())
        alignment = (ParagraphFormat::Alignment)value["format_alignment"].GetInt();

    auto f = document->paragraph_formats->GetFormat(format_name, document->config.language);
    if (f)
    {
        f = document->paragraph_formats->GetFormat(format_name, alignment, f->word_wrap, f->line_spacing, f->indent_before, f->indent_after, 
            f->indent_first_line, f->spacing_before, f->spacing_after, f->default_string_format, document->config.language);
        if (f)
        {
            p->format = f;
            p->current_string_format = f->default_string_format;
        }
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

void Paragraph::Draw() const
{
    auto _id = document->GetFirstVisibleRow(id);
    int pos = elements->GetChildPos(_id);
    for (int i = pos; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (document->IsVisible(el->id))
            el->Draw();
    }
    if (!marker.empty())
    {
        ElementPtr el = elements->Get(0);
        Rect r = GetAbsoluteRect();
        if (!marker_draw_format)
            marker_draw_format = document->string_formats->GetFormat(marker_format, document->config.scale);
        Size s = window->GetTextSize(marker, marker_draw_format);
        int h = std::max(el->rect.height, s.height);
        window->DrawText(ToBasicString(marker), marker_draw_format, 
            Rect{r.left, r.top + (h - s.height) / 2, s.width, s.height}, 
            marker_draw_format->text_color, current_string_format->text_bg_color, false);
    }
}

bool Paragraph::Remake(bool with_elements)
{
    if (document->break_remake)
        return false;
    
    if (format->alignment == ParagraphFormat::Alignment::Justify)
        MakePlain();
    
    bool changed = Element::Remake(format->alignment == ParagraphFormat::Alignment::Justify ? true : with_elements);

    Text* text = type == ElementType::PARAGRAPH ? (Text*)parent : nullptr;
    int left_m = 0, top_m = 0, right_m = 0, bottom_m = 0;
    int page_width = text ? text->pixel_size.width : 0;
    int indent_before = std::round(format->indent_before * document->config.scale);
    int indent_after = std::round(format->indent_after * document->config.scale);
    int m = 0;

    if (!marker.empty())
    {
        if (!marker_draw_format)
            marker_draw_format = document->string_formats->GetFormat(marker_format, document->config.scale);
        Size s(window->GetTextSize(marker, marker_draw_format), document->config.scale);
        m = s.width;
        if (page_width > 0)
            page_width -= m;
    }

    if (format->word_wrap == ParagraphFormat::WordWrap::Normal)
    {
        for (int i = 0; i < elements->Count(); ++i)
        {
            ElementPtr row = elements->Get(i);
            ElementPtr next_row;
            if (i < elements->Count() - 1)
                next_row = elements->Get(i + 1);

            bool b = true;
            //move or split element if it's more then row width
            while (row->rect.width + indent_before + indent_after > page_width)
            {
                if (document->break_remake)
                    return false;
                ElementPtr el = row->elements->Get(row->elements->Count() - 1);
                if (!el)
                    break;
                if (el->Split(page_width - indent_before - indent_after, true))
                    el = row->elements->Get(row->elements->Count() - 1);

                if (row->elements->Count() == 1)
                {
                    b = false;
                    break;
                }

                if (!next_row)
                {
                    next_row.reset(new Row(this));
                    AddElement(next_row);
                    next_row->elements->Clear();
                }

                //move the element in the next row
                next_row->elements->Move(el, 0);
                row->Remake();
                next_row->Remake(true);
                changed = true;
            }

            if (!b)
                continue;

            //move elements above if they are narrower to be placed in the row
            while (next_row && next_row->elements->Count() > 0)
            {
                if (document->break_remake)
                    return false;
                auto el = next_row->elements->Get(0);
                el->GetMargin(left_m, top_m, right_m, bottom_m);
                if (el->rect.width + left_m + right_m >= page_width - row->rect.width - indent_before - indent_after)
                    break;
                
                //move the element from the next row in the current one
                row->elements->Move(next_row->elements->Get(0), row->elements->Count());
                row->Remake(true);
                row->Normalize();
                if (next_row->elements->Count() == 0)
                {
                    elements->RemoveAt(i + 1, 1);
                    --i;
                    next_row.reset();
                }
                else
                {
                    next_row->Remake(true);
                    next_row->Normalize();
                }
                row->UpdateRect();
                changed = true;
            }

            if (next_row)
            {
                //try to split the first element and move it above
                ElementPtr el = next_row->elements->Get(0);
                while (el && el->Split(page_width - row->rect.width - indent_before - indent_after, false))
                {
                    row->elements->Move(next_row->elements->Get(0), row->elements->Count());
                    row->Remake(true);
                    next_row->Remake(true);
                    row->Normalize();
                    next_row->Normalize();
                    el = next_row->elements->Get(0);
                    changed = true;
                }
            }

            if (changed)
            {
                row->Remake();
                if (next_row)
                    next_row->Remake();

                row->Normalize();
                if (next_row)
                    next_row->Normalize();
            }

            UpdateRect();
        }
    }

    int h = 0;
    int w = page_width;
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr row = elements->Get(i);
        row->rect.left = 0;
        if (w < row->rect.width)
            w = row->rect.width;
    }
    int line_spacing = std::round(format->line_spacing * document->config.scale);
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr row = elements->Get(i);
        row->GetMargin(left_m, top_m, right_m, bottom_m); //consider the margins
        //move the row
        switch (format->alignment)
        {
        case ParagraphFormat::Alignment::Left:
        case ParagraphFormat::Alignment::Justify:
            row->rect.Move(m + indent_before + left_m, h + top_m);
            break;
        case ParagraphFormat::Alignment::Right:
            if (row->rect.width > page_width)
                row->rect.Move(m + w - row->rect.width + indent_before + left_m, h + top_m);
            else
                row->rect.Move(m + w - row->rect.width + indent_before - indent_after + left_m, h + top_m);
            break;
        case ParagraphFormat::Alignment::Center:
            row->rect.Move(m + indent_before + (w - row->rect.width) / 2 - indent_after, h + top_m);
            break;
        }
        h += row->rect.height + line_spacing + top_m + bottom_m;
    }

    UpdateRect();

    if (elements->Count() > 0)
        baseline = elements->Get(0)->baseline;

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

void Paragraph::Normalize()
{
    Element::Normalize();

    if (elements->Count() == 0)
        AddEmptyElement(); //paragraph has to have at least one row

    for (int i = 0; i < elements->Count();)
    {
        auto el = elements->Get(i);
        if (elements->Count() == 1)
            break;
        if (el->elements->Count() == 1 && document->IsString(el->elements->Get(0)) && el->elements->Get(0)->elements->Count() == 0)
        {
            elements->RemoveAt(i, 1);
            document->CaretMoved();
        }
        else
            ++i;
    }

    //try to merge end of a row above and begin of a row below
    for (int i = 0; i < elements->Count() - 1; ++i)
    {
        auto above = elements->Get(i);
        auto below = elements->Get(i + 1);
        auto el1 = above->elements->Get(above->elements->Count() - 1);
        auto el2 = below->elements->Get(0);
        if (document->IsString(el1) && document->IsString(el2))
        {
            auto str = el1->ToText();
            if (str.length() > 0 && str[str.size() - 1] != U' ')
            {
                if (el1->Merge(el2))
                {
                    above->Remake();
                    below->Remake();
                }
            }
        }
    }
}

void Paragraph::Rescale() const
{
    Element::Rescale();
    marker_draw_format.reset();
}

void Paragraph::MakePlain()
{
    ElementPtr row(new Row(parent, false));
    row->id = id;
    row->logical_id = logical_id;
    for (int i = 0; i < elements->Count(); ++i)
    {
        auto r = elements->Get(i);
        for (int j = 0; j < r->elements->Count(); ++j)
        {
            auto _el = r->elements->Get(j);
            row->elements->Add(_el);
        }
    }
    ((Row*)row.get())->format = format;
    row->MakePlain();
    elements->Clear();
    elements->Add(row);
}

bool Paragraph::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    if (_elements.size() == 1 && document->IsRow(_elements[0]))
    {
        if (!Element::InsertElements(_elements, insert_mode, with_undo, changed_element))
            return false;
        Normalize();
        changed_element = id;
        return true;
    }

    auto el = document->GetElement(caret->GetCaretState().id);
    if (el && document->IsRow(el->id))
        return el->InsertElements(_elements, insert_mode, with_undo, changed_element); //insert in the beginning of current row

    if (!parent->InsertElements(_elements, insert_mode, with_undo, changed_element))
        return false;
    //re-solve all the paragraphs below
    for (int i = yutovo::GetChildPos(id); i < parent->elements->Count(); ++i)
        document->AddResolveElement(yutovo::GetChild(parent->logical_id, i));
    return true;
}

bool Paragraph::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    uint start, size;
    if (selection->Has(id, start, size))
    {
        if (Element::DeleteElements(left, with_undo, changed_element))
        {
            Normalize();
            changed_element = id;
            return true;
        }
    }

    CaretState c = caret->GetCaretState();
    auto row = document->FindParentRow(c.id);
    if (row)
    {
        CaretState first_state, last_state;
        if (!left && row->GetLastCaretState(last_state, nullptr))
        {
            if (c == last_state)
            {
                int p = elements->GetElementPos(row->id);
                if (p < elements->Count() - 1)
                {
                    //delete first element at the next row
                    auto next_row = elements->Get(p + 1);
                    if (next_row->GetFirstCaretState(first_state, nullptr))
                    {
                        caret->SetState(first_state);
                        return caret->GetElement()->DeleteElements(false, with_undo, changed_element);
                    }
                }
            }
        }
        else if (left && row->GetFirstCaretState(first_state, nullptr))
        {
            if (c == first_state)
            {
                int p = elements->GetElementPos(row->id);
                if (p > 0)
                {
                    //delete last element at the previous row
                    auto prev_row = elements->Get(p - 1);
                    if (prev_row->GetLastCaretState(last_state, nullptr))
                    {
                        caret->SetState(last_state);
                        return caret->GetElement()->DeleteElements(true, with_undo, changed_element);
                    }
                }
            }
        }
    }

    return parent->DeleteElements(left, with_undo, changed_element);
}

bool Paragraph::ChangeParagraphFormat(const ParagraphFormatPtr _format, bool with_undo, ElementId& changed_element)
{
    if (*format == *_format)
        return false;
    
    if (with_undo)
        document->StoreUndo(id);
    
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->UpdateStringFormat(format->default_string_format, _format->default_string_format);

    format = _format;
    current_string_format = format->default_string_format;
    changed_element = id;
    document->CaretMoved();
    return true;
}

bool Paragraph::ChangeStringFormat(const StringFormatPtr _format, bool with_undo, ElementId& changed_element)
{
    if (*current_string_format == *_format)
        return false;

    if (with_undo)
        document->StoreUndo(id);

    format->default_string_format = _format;
    current_string_format = _format;
    changed_element = id;
    document->CaretMoved();
    return true;
}

bool Paragraph::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (select && IsEmpty())
    {
        if (parent->elements->IsLast(id))
            return parent->GetLeftCaretState(caret_state, select);
        if (caret->left_up_direction)
        {
            if (caret->IsInsideElement(id))
            {
                if (select->IsSelected(id))
                {
                    if (parent->elements->IsFirst(id))
                        return parent->GetLeftCaretState(caret_state, select);
                    select->Add(id);
                    return parent->GetLeftCaretState(caret_state, nullptr);
                }
                else
                {
                    if (selection->IsSelectionAbove(id))
                    {
                        if (!parent->elements->IsFirst(id))
                        {
                            auto el = parent->elements->Get(parent->elements->GetElementPos(id) - 1);
                            if (!el->IsEmpty())
                                return el->GetLastCaretState(caret_state, nullptr);
                        }
                        return parent->GetLeftCaretState(caret_state, select);
                    }
                    select->Add(id);
                    return parent->GetLeftCaretState(caret_state, nullptr);
                }
            }
            else
            {
                if (select->IsSelected(id))
                {
                    select->Add(id);
                    return GetFirstCaretState(caret_state, nullptr);
                }
                else
                {
                    select->Add(id);
                    if (parent->elements->IsFirst(id))
                        return GetFirstCaretState(caret_state, nullptr);
                    return parent->GetLeftCaretState(caret_state, nullptr);
                }
            }
        }
    }
    return Element::GetLeftCaretState(caret_state, select);
}

bool Paragraph::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    if (select && IsEmpty())
    {
        if (parent->elements->IsLast(id))
            return false;
        if (!caret->left_up_direction)
        {
            select->Add(id);
            return parent->GetRightCaretState(caret_state, nullptr);
        }
    }
    return Element::GetRightCaretState(caret_state, select);
}

bool Paragraph::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    ElementPtr row;
    //find nearest row
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr el = elements->Get(i);
        if (y < el->GetAbsoluteRect().GetBottom() || el->rect.height == 0)
            break;
        row = el;
    }
    if (!row)
        return parent->GetTopCaretState(x, GetAbsoluteRect().top, caret_state, select);
    return row->GetTopCaretState(x, y, caret_state, select);
}

bool Paragraph::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    ElementPtr row;
    //find nearest row
    for (int i = elements->Count() - 1; i >= 0; --i)
    {
        ElementPtr el = elements->Get(i);
        if (y > el->GetAbsoluteRect().top || el->rect.height == 0)
            break;
        row = el;
    }
    if (!row)
        return parent->GetBottomCaretState(x, GetAbsoluteRect().GetBottom(), caret_state, select);
    return row->GetBottomCaretState(x, y, caret_state, select);
}

bool Paragraph::GetWordLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (select && IsEmpty())
        return GetLeftCaretState(caret_state, select);
    return Element::GetWordLeftCaretState(caret_state, select);
}

bool Paragraph::GetWordRightCaretState(CaretState& caret_state, Selection* select)
{
    if (select && IsEmpty())
        return GetRightCaretState(caret_state, select);
    return Element::GetWordRightCaretState(caret_state, select);
}

bool Paragraph::CanContinueSelection()
{
    return true;
}

bool Paragraph::GetNearestCaretState(const int x, const int y, CaretState& caret_state)
{
    int min_dist = std::numeric_limits<int>::max();
    ElementPtr el;
    for (int i = 0; i < elements->Count(); ++i) //TODO: binary search
    {
        auto _el = elements->Get(i);
        Rect r = _el->GetAbsoluteRect();
    	int dist = r.DistToPoint(x, y);
        if (dist < min_dist)
        {
            min_dist = dist;
            el = _el;
        }
    }

    if (!el)
        return false;
    return el->GetNearestCaretState(x, y, caret_state);
}

void Paragraph::AddEmptyElement()
{
    AddElement(ElementPtr(new Row(this)));
}

bool Paragraph::IsEmpty() const
{
    if (elements->Count() != 1)
        return false;
    return elements->Get(0)->IsEmpty();
}

ParagraphFormatPtr Paragraph::GetParagraphFormat()
{
    return format;
}

StringFormatPtr Paragraph::GetStringFormat() const
{
    return current_string_format;
}

std::string Paragraph::ToHtml() const
{
    std::string r = "<p";
    switch (format->alignment)
    {
    case ParagraphFormat::Alignment::Left:
        break;
    case ParagraphFormat::Alignment::Right:
        r += " align=\"right\"";
        break;
    case ParagraphFormat::Alignment::Center:
        r += " align=\"center\"";
        break;
    case ParagraphFormat::Alignment::Justify:
        r += " align=\"justify\"";
        break;
    }
    return r + ">" + Element::ToHtml() + "</p>";
}

ElementPtr Paragraph::GetPlainRow()
{
    ElementPtr row(new Row(parent, false));
    for (int i = 0; i < elements->Count(); ++i)
    {
        auto r = elements->Get(i);
        for (int j = 0; j < r->elements->Count(); ++j)
        {
            auto _el = r->elements->Get(j);
            row->elements->Add(_el);
        }
    }
    ((Row*)row.get())->format = format;
    return row;
}

void Paragraph::SetMarker(const std::u32string& _marker, const StringFormatPtr& _marker_format)
{
    marker = _marker;
    marker_format = _marker_format;
}

}
