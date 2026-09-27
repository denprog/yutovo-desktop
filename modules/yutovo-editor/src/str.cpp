/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "str.h"
#include "document.h"
#include "editor_utils.h"
#include <assert.h>
#include <boost/locale.hpp>
#include <boost/lexical_cast.hpp>
#ifdef min
#undef min
#endif
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace yutovo
{

//String

String::String(Element* parent) : 
    Element(parent),
    format(parent->GetStringFormat())
{
    type = ElementType::STRING;
    can_merge = true;

    baseline = parent->window->GetFontAscent(format);

    elements.reset(new StringElements(this));
}

String::String(Element* parent, const std::string _str, bool _translate) : 
    Element(parent),
    format(parent->GetStringFormat()),
    translate(_translate)
{
    type = ElementType::STRING;
    can_merge = true;

    elements.reset(new StringElements(this, ToUtfString(_str)));

#ifdef DEBUG
    to_str = ToText();
#endif
}

String::String(Element* parent, const std::string _str, const StringFormatPtr _format, bool _translate) :
    Element(parent), 
    format(_format),
    translate(_translate)
{
    type = ElementType::STRING;
    can_merge = true;

    elements.reset(new StringElements(this, ToUtfString(_str)));

#ifdef DEBUG
    to_str = ToText();
#endif
}

String::String(Element* parent, const std::u32string _str) :
    Element(parent), 
    format(parent->GetStringFormat())
{
    type = ElementType::STRING;
    can_merge = true;

    elements.reset(new StringElements(this, _str));

#ifdef DEBUG
    to_str = ToText();
#endif
}

String::String(Element* parent, const std::u32string _str, const StringFormatPtr _format) :
    Element(parent), 
    format(_format)
{
    type = ElementType::STRING;
    can_merge = true;

    elements.reset(new StringElements(this, _str));

#ifdef DEBUG
    to_str = ToText();
#endif
}

String::String(Document* _document) :
    Element(_document)
{
    type = ElementType::STRING;
    can_merge = true;

    elements.reset(new StringElements(this));

#ifdef DEBUG
    to_str = ToText();
#endif
}

String::String(Document* _document, const std::u32string _str) :
    Element(_document)
{
    type = ElementType::STRING;
    can_merge = true;

    elements.reset(new StringElements(this, _str));

#ifdef DEBUG
    to_str = ToText();
#endif
}

String::String(Document* _document, const std::string _str, const StringFormatPtr _format) :
    Element(_document), 
    format(_format)
{
    type = ElementType::STRING;
    can_merge = true;

    elements.reset(new StringElements(this, boost::locale::conv::utf_to_utf<char32_t>(_str)));

#ifdef DEBUG
    to_str = ToText();
#endif
}

String::String(Document* _document, const std::u32string _str, const StringFormatPtr _format) :
    Element(_document), 
    format(_format)
{
    type = ElementType::STRING;
    can_merge = true;

    elements.reset(new StringElements(this, _str));

#ifdef DEBUG
    to_str = ToText();
#endif
}

Element* String::Clone()
{
    return new String(*this);
}

bool String::Copy(std::vector<ElementPtr>& copy)
{
    uint start, size;
    if (!selection->Has(id, start, size))
        return false;
    if (start == 0 && size == elements->Count())
    {
        copy.push_back(ElementPtr(Clone()));
        return true;
    }
    ElementPtr s(Create(parent, ((StringElements*)elements.get())->str.substr(start, size), format));
    s->id = id;
    copy.push_back(s);
    return true;
}

void String::SetDocument(Document* _document)
{
    Element::SetDocument(_document);
    ClearCache();
}

Element* String::Create(Element* parent)
{
    return new String(parent);
}

Element* String::Create(Element* parent, const std::u32string _str, const StringFormatPtr _format)
{
    return new String(parent, _str, _format);
}

void String::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Element::ToJson(value, alloc);
    rapidjson::Value _uuid(boost::uuids::to_string(format->id).c_str(), alloc);
    value.AddMember("format_id", _uuid, alloc);
}

Element* String::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (value.HasMember("format_id") && value["format_id"].IsString())
    {
        auto format_id_str = value["format_id"].GetString();
        boost::uuids::uuid format_id;
        try
        {
            format_id = boost::lexical_cast<boost::uuids::uuid>(format_id_str);
        }
        catch (std::bad_cast& ex)
        {
            return nullptr;
        }

        auto f = document->GetStringFormat(format_id);
        if (f)
        {
            if (parent)
                return new String(parent, U"", f);
            return new String(document, U"", f);
        }
    }

    StringFormatPtr f;
    if (parent)
    {
        f = parent->GetStringFormat();
        if (f)
            return new String(parent, U"", f);
    }

    if (document->GetCurrentStringFormat(f))
    {
        if (parent)
            return new String(parent, U"", f);
        return new String(document, U"", f);
    }
    return nullptr;
}

bool String::Remake(bool with_elements)
{
    auto& str = ((StringElements*)elements.get())->str;
    if (translate && !str.empty())
    {
        str = window->Translate(id, str);
        if (str == U"")
            return true; //translation is async, it will insert new string later and remake it
        translate = false;
    }

    stretch_width = 0;
    UpdateRect();
    if (with_elements)
    {
        for (int j = yutovo::GetChildPos(id) + 1; j < parent->elements->Count(); ++j)
        {
            ElementPtr el = parent->elements->Get(j);
            if (el->type == ElementType::STRING || el->type == ElementType::CODE_STRING)
            {
                ((String*)el.get())->ClearCache();
                el->Remake(false);
            }
        }
    }

    bool changed = (rect != last_rect);
    last_rect = rect;
    return changed;
}

void String::Normalize()
{
}

void String::Rescale() const
{
    ClearCache();
    if (format)
        draw_format = document->string_formats->GetFormat(format, document->config.scale);
}

void String::UpdateRect(bool with_elements)
{
    if (last_stretch_width != stretch_width)
    {
        last_stretch_width = stretch_width;
        ClearCache();
    }

    if (!draw_format)
        Rescale();

    Size s;
    auto& str = ((StringElements*)elements.get())->str;
    if (type != ElementType::CODE_STRING && stretch_width == 0 && str.find(U'\t') == std::u32string::npos)
        s = window->GetTextSize(str, draw_format);
    else
        s = GetTextSize(elements->Count());
    rect.SetSize(s.width, s.height);
    baseline = window->GetFontAscent(draw_format);
}

bool String::GetElementAtCoords(const int x, const int y, const int margin, ElementId& _id)
{
    Rect r = GetAbsoluteRect();
    if (!r.IsPointInside(x, y))
        return false;
    //find nearest caret state
    CaretState next, last, nearest;
    if (!GetFirstCaretState(next, nullptr) || !GetLastCaretState(last, nullptr))
        return false;

    nearest = next;
    r = document->GetCaretRect(nearest);
	int min_dist = r.DistToPoint(x, y);
    while (next != last)
    {
        ElementPtr el = document->GetParent(next.id);
        if (!el->GetRightCaretState(next, nullptr))
            break;
        r = document->GetCaretRect(next);
        int dist = r.DistToPoint(x, y);
        if (dist < min_dist)
        {
            min_dist = dist;
            nearest = next;
        }
    }
    _id = nearest.id;
    return true;
}

std::string String::ToHtml() const
{
    std::string s = "<span ";
    s += "style=\"font-family:'";
    s += format->family;
    s += "';";
    s += "font-size:";
    s += std::to_string(format->size);
    s += "px;";
    if (format->underline)
        s += "text-decoration: underline;";
    if (format->strikethrough)
        s += "text-decoration: line-through;";
    if (format->text_color != Color::Black())
        s += "color:" + format->text_color.ToString() + ";";
    if (format->text_bg_color != Color::White())
        s += "bgcolor:" + format->text_bg_color.ToString() + ";";
    s += "\">";
    if (format->subscript)
        s += "<sub>";
    if (format->superscript)
        s += "<sup>";
    if (format->bold)
        s += "<strong>";
    if (format->italic)
        s += "<em>";
    s += elements->ToHtml();
    if (format->italic)
        s += "</em>";
    if (format->bold)
        s += "</strong>";
    if (format->subscript)
        s += "</sub>";
    if (format->superscript)
        s += "</sup>";
    s += "</span>";
    return s;
}

void String::ToParserString(ParserString& str)
{
    str.Add(id, elements->ToText());
}

bool String::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    if (!editable)
        return false;
    
    if (!caret->IsInsideElement(id))
        return parent->InsertElements(_elements, insert_mode, with_undo, changed_element);
    
    if (_elements.size() == 1 && _elements[0]->type == type)
    {
        ClearCache();
        String* s = dynamic_cast<String*>(_elements[0].get());
        if (elements->Count() == 0)
        {
            //replace the string and format
            if (caret->GetPos() != 0)
                return false;
            if (with_undo)
                document->StoreUndo(id);
            elements.reset(new StringElements(this, s->elements->ToText()));
            format = s->format;
            Rescale();
            caret->SetState(elements->GetElementId(elements->Count()));
            parent->Normalize();
            auto p = document->FindParent(id, ElementType::PARAGRAPH);
            p->elements->UpdateIds();
            changed_element = id;
#ifdef DEBUG
            to_str = ToText();
#endif
            return true;
        }
        else if (!s->format || (format && *s->format == *format))
        {
            if (with_undo)
                document->StoreUndo(id);
            if (insert_mode)
                elements->Insert(_elements[0], caret->GetPos());
            else
                elements->Replace(_elements[0], caret->GetPos());
            caret->SetState(elements->GetElementId(caret->GetPos() + s->elements->Count()));
            parent->Normalize();
            auto p = document->FindParent(id, ElementType::PARAGRAPH);
            p->elements->UpdateIds();
            changed_element = id;
#ifdef DEBUG
            to_str = ToText();
#endif
            return true;
        }
    }

    return parent->InsertElements(_elements, insert_mode, with_undo, changed_element);
}

bool String::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    if (!editable)
        return false;

    ClearCache();
    uint caret_pos = caret->GetPos();
    if (caret->IsInsideElement(id) && selection->IsEmpty())
    {
        if (caret_pos == 0 && left)
        {
            int p = parent->elements->GetElementPos(id);
            if (p > 0)
            {
                ElementPtr _el = parent->elements->Get(p - 1);
                if (_el->type == type && ((String*)_el.get())->format == format)
                {
                    if (with_undo)
                        document->StoreUndo(parent->id);
                    bool _can_merge = true;
                    if (!_el->can_merge || !can_merge)
                        _can_merge = false;
                    _el->can_merge = true;
                    can_merge = true;
                    bool r = _el->Merge(parent->elements->Get(p));
                    _el->can_merge = _can_merge;
                    changed_element = parent->id;
                    return r;
                }
            }
            return parent->DeleteElements(left, with_undo, changed_element);
        }
        if (caret_pos == elements->Count() && !left)
        {
            int p = parent->elements->GetElementPos(id);
            if (p < parent->elements->Count() - 1)
            {
                ElementPtr _el = parent->elements->Get(p + 1);
                if (_el->type == type && ((String*)_el.get())->format == format)
                {
                    if (with_undo)
                        document->StoreUndo(parent->id);
                    bool _can_merge = true;
                    if (!_el->can_merge || !can_merge)
                        _can_merge = false;
                    _el->can_merge = true;
                    can_merge = true;
                    bool r = Merge(_el);
                    can_merge = _can_merge;
                    changed_element = parent->id;
                    return r;
                }
            }
            return parent->DeleteElements(left, with_undo, changed_element);
        }
    }
    
    std::u32string& str = ((StringElements*)elements.get())->str;

    uint pos = 0;
    uint start, size;
    if (selection->Has(id, start, size))
    {
        if (with_undo)
        {
            if (selection->selection.size() > 1)
                document->StoreUndo(parent->id);
            else
                document->StoreUndo(id);
        }
        elements->RemoveAt(start, size);
        pos = start;
    }
    else if (caret->IsInsideElement(id))
    {
        if (with_undo)
        {
            if (str.size() == 1)
                document->StoreUndo(parent->id);
            else
                document->StoreUndo(id);
        }
        if (left)
        {
            elements->RemoveAt(caret_pos - 1, 1);
            pos = caret_pos - 1;
        }
        else
        {
            elements->RemoveAt(caret_pos, 1);
            pos = caret_pos;
        }
        caret->SetPos(pos, true);
        if (elements->Count() == 0)
            can_merge = true;
    }

    CaretState before_state = caret->GetCaretState();
    auto row = document->FindParentRow(id);
    if (row && !str.empty())
    {
        CaretState first_state, last_state;
        row->GetFirstCaretState(first_state, nullptr);
        row->GetLastCaretState(last_state, nullptr);
        if (before_state == first_state || before_state == last_state)
            parent->parent->Normalize();
        else
            parent->Normalize();
    }

    if (str.empty())
        changed_element = parent->id;
    else
        changed_element = id;

#ifdef DEBUG
    to_str = ToText();
#endif
    return true;
}

bool String::ChangeStringFormat(const StringFormatPtr _format, bool with_undo, ElementId& changed_element)
{
    ClearCache();
    uint start, size;
    if (selection->Has(id, start, size))
    {
        if (*format == *_format)
            return false;
        if (start == 0 && size == elements->Count())
        {
            if (with_undo)
                document->StoreUndo(parent->parent->id);
            //change format of the whole string
            format = _format;
            Rescale();
            parent->Normalize();
            auto p = document->FindParent(id, ElementType::PARAGRAPH);
            p->elements->UpdateIds();
            changed_element = id;
            document->CaretMoved();
            return true;
        }

        if (!editable)
            return false;

        if (with_undo)
            document->StoreUndo(parent->parent->id);
        ElementPtr el = parent->elements->Get(id);
        if (SplitAt(start))
            el = parent->elements->Get(parent->elements->GetElementPos(id) + 1);
        el->SplitAt(size);
        ((String*)el.get())->format = _format;
        el->Rescale();
        document->CaretMoved();

        auto p = document->FindParent(id, ElementType::PARAGRAPH);
        p->elements->UpdateIds();
        
        parent->Normalize();
        changed_element = id;
        return true;
    }
    return false;
}

void String::SetString(const std::u32string& str)
{
    elements.reset(new StringElements(this, str));
    ClearCache();
}

bool String::Split(const uint width, bool split_more)
{
    if (!editable)
        return false;

    ClearCache();

    if (!draw_format)
        Rescale();

    int i = 0;
    std::u32string& str = ((StringElements*)elements.get())->str;
    int len = (int)str.size();

    //fast path for long plain strings: binary search using direct text measurements
    //CodeString overrides GetTextSize to add visual gaps between digits, so it must use the original path
    if (type != ElementType::CODE_STRING && len > 1000 && stretch_width == 0 && str.find(U'\t') == std::u32string::npos)
    {
        //find the largest position p such that the width of the prefix [0, p) fits
        int low = 0, high = len;
        while (low < high)
        {
            int mid = low + (high - low + 1) / 2;
            Size s = window->GetTextSize(str.substr(0, mid), draw_format);
            if (s.width <= (int)width)
                low = mid;
            else
                high = mid - 1;
        }
        int p = low;

        if (split_more)
        {
            //if the next word does not fit, the line is allowed to overflow slightly rather than leaving a very short first line
            size_t pos = str.find(U' ', p);
            if (pos != std::u32string::npos && pos < (size_t)len - 1)
                i = (int)pos;
            else
            {
                //use the last possible space (e.g. a single long word)
                pos = str.rfind(U' ', (size_t)len - 2);
                if (pos != std::u32string::npos && pos > 0)
                    i = (int)pos;
            }
        }
        else
        {
            //find the last space before p (split after it)
            size_t pos = str.rfind(U' ', (size_t)p - 1);
            if (pos != std::u32string::npos && pos > 0)
                i = (int)pos;
        }
    }
    else
    {
        for (int j = 1; j < len - 1; ++j) //at least one character in the splitted string
        {
            if (str[j] == ' ')
            {
                Size s = GetTextSize(j + 1);
                if (s.width <= width)
                    i = j;
                else
                {
                    if (split_more)
                        i = j;
                    break;
                }
            }
        }
    }

    if (i == 0)
        return false;

    uint start, size;
    bool s = selection->Has(id, start, size);
    if (s)
        selection->Remove(id, start, size);

    //create new string and insert it after this one
    ElementPtr el(Create(parent, str.substr(i + 1), format));
    int pos = parent->elements->GetElementPos(id);
    parent->elements->Insert(el, pos + 1);
    str = str.substr(0, i + 1);
    UpdateRect();

    if (caret->IsInsideElement(id))
    {
        //update caret state
        if (caret->GetPos() > i + 1)
            caret->SetState(el->id, caret->GetPos() - i - 1, true);
    }

    if (s)
    {
        if (start >= i + 1)
        {
            //move selection into the new element
            selection->Add(el->id, start - i - 1, size);
        }
        else if (start < str.length())
        {
            //split the selection
            if (str.length() - start > size)
                selection->Add(id, start, size);
            else
                selection->Add(id, start, str.length() - start);
            if (start + size > str.length())
                selection->Add(el->id, 0, size - str.length() + start);
        }
    }
#ifdef DEBUG
    parent->to_str = parent->ToText();
    to_str = ToText();
#endif
    return true;
}

bool String::SplitAt(const uint pos)
{
    if (!editable)
        return false;
    if (pos == 0 || pos >= elements->Count())
        return false;

    ClearCache();

    int cs_pos = -1;
    if (caret->IsInsideElement(id))
        cs_pos = caret->GetPos();
    
    std::u32string& str = ((StringElements*)elements.get())->str;
    ElementPtr el(Create(parent, str.substr(pos), format));
    int p = parent->elements->GetElementPos(id);
    parent->elements->Insert(el, p + 1);
    str = str.substr(0, pos);
    UpdateRect();
    el->UpdateRect();

    uint start, size;
    if (selection->Has(id, start, size))
    {
        if (start >= pos && size <= el->elements->Count())
        {
            selection->Remove(id, start, size);
            selection->Add(el->id, start - pos, size);
        }
        else
            selection->Optimize();
        
        if (cs_pos == pos && start == pos)
            caret->SetState(el->id, 0, true);
        else if (cs_pos == pos && start + size == pos)
            caret->SetState(id, elements->Count(), true);
        else if (cs_pos > (int)pos)
            caret->SetState(el->id, cs_pos - pos, true);
    }
    else if (cs_pos >= (int)pos)
        caret->SetState(el->id, cs_pos - pos, true);

#ifdef DEBUG
    parent->to_str = parent->ToText();
    to_str = ToText();
#endif
    return true;
}

bool String::Merge(const ElementPtr with_element)
{
    if (!editable || !document->IsString(with_element) || !with_element->can_merge)
        return false;
    //merge two strings if those formats are equal
    String* el = (String*)with_element.get();
    if (*el->format != *format)
        return false;

    ClearCache();

    if (caret->IsInsideElement(with_element->id))
        caret->SetState(id, caret->GetPos() + elements->Count()); //update caret state

    uint start, size = 0;
    if (selection->Has(with_element->id, start, size))
    {
        selection->Remove(with_element->id, start, size);
        selection->Add(id, elements->Count() + start, size);
    }

    elements->Insert(with_element, elements->Count());
    with_element->parent->elements->RemoveAt(with_element->parent->elements->GetElementPos(with_element->id), 1);

#ifdef DEBUG
    to_str = ToText();
#endif
    return true;
}

bool String::CanMerge(const ElementPtr with_element)
{
    if (!editable)
        return false;
    if (!document->IsString(with_element) || with_element->type == ElementType::LINK)
        return false;
    String* el = (String*)with_element.get();
    if (el->format != format)
        return false;
    return can_merge;
}

bool String::AfterInsert(bool with_undo)
{
    if (!caret)
        return false;
    CaretState c = caret->GetCaretState();
    if (document->GetElementType(c.id) == ElementType::LINK)
        format = parent->GetStringFormat(); //get string format from the row
    if (GetLastCaretState(c, nullptr))
        caret->SetState(c);
    return true;
}

void String::BeforeDelete()
{
}

void String::BeforeReplace()
{
}

void String::AfterReplace()
{
}

void String::BeforePaste()
{
}

StringFormatPtr String::GetStringFormat() const
{
    return format;
}

void String::UpdateStringFormat(const StringFormatPtr base_format, const StringFormatPtr new_format)
{
    StringFormat f = *format;
    //change params only the same with the base format
    if (base_format->family == format->family)
        f.family = new_format->family;
    if (base_format->size == format->size)
        f.size = new_format->size;
    if (base_format->bold == format->bold)
        f.bold = new_format->bold;
    if (base_format->italic == format->italic)
        f.italic = new_format->italic;
    if (base_format->underline == format->underline)
        f.underline = new_format->underline;
    if (base_format->strikethrough == format->strikethrough)
        f.strikethrough = new_format->strikethrough;
    if (base_format->text_color == format->text_color)
        f.text_color = new_format->text_color;
    if (base_format->text_bg_color == format->text_bg_color)
        f.text_bg_color = new_format->text_bg_color;
    format = document->GetStringFormat(f.family, f.size, f.bold, f.italic, f.underline, f.strikethrough, f.subscript, f.superscript, f.text_color, f.text_bg_color);
    Rescale();
}

Size String::GetTextSize(const uint pos) const
{
    if (!draw_format)
        Rescale();

    if (size_cache.empty())
    {
        auto& str = ((StringElements*)elements.get())->str;
        int add_space = 0, wide_space = 0;
        
        if (stretch_width != 0)
        {
            int spaces = std::count_if(str.begin(), str.end(),
                [](char32_t c)
                {
                    return c == U' ';
                });
            if (spaces > 0)
                add_space = floor(stretch_width / spaces);
        }

        int tab_pos = -1;
        for (size_t i = 0; i <= str.length(); ++i)
        {
            if (i > 0 && str[i - 1] == U'\t')
            {
                if (tab_size.width == 0)
                    tab_size = window->GetTextSize(std::u32string(document->config.tab_spaces, U' '), draw_format);
                
                if (i == 1)
                {
                    size_cache[i] = tab_size;
                }
                else
                {
                    Size s = size_cache[i - 1];
                    uint w = 0;
                    for (int j = 0; j < yutovo::GetChildPos(id); ++j)
                        w += parent->elements->Get(j)->rect.width;
                    uint p = w;
                    w += s.width;
                    w = (w / tab_size.width + 1) * tab_size.width;
                    s.width = w - p;
                    size_cache[i] = s;
                }
                wide_space = 0;
                tab_pos = i;
                continue;
            }

            if (tab_pos == -1)
            {
                auto _str = str.substr(0, i);
                size_cache[i] = window->GetTextSize(_str, draw_format);
            }
            else
            {
                auto _str = str.substr(tab_pos, i - tab_pos);
                Size s = window->GetTextSize(_str, draw_format);
                s.width += size_cache[tab_pos].width;
                size_cache[i] = s;
            }

            if (add_space > 0 && i > 0 && i < str.length() - 1 && str[i - 1] == U' ')
                wide_space += add_space;
            size_cache[i].width += wide_space;
        }
    }
    return size_cache[pos];
}

bool String::CanContinueSelection()
{
    return true;
}

void String::UpdateDrawRect()
{
    draw_rect = GetAbsoluteRect();
}

void String::UpdateLevel(uint8_t _level)
{
    if (level == _level)
        return;
    level = _level;
    if (!parent || !parent->parent)
        return;
    auto* p = parent;
    while (p && (p->level != 1 || p->type == ElementType::CODE_ROW))
        p = p->parent;
    if (!p)
        return;
    auto f = p->GetStringFormat();
    int s = f->size - (level - p->level) * 2;
    if (s < 8)
        s = 8;
    format = document->GetStringFormat(format->family, s, format->bold, format->italic, format->underline, 
        format->strikethrough, format->subscript, format->superscript, format->text_color, format->text_bg_color);
    Rescale();
}

void String::SetEditable(bool _editable)
{
    editable = _editable;
}

void String::GetElements(ElementType _type, std::vector<ElementId>& _elements)
{
    if (type == _type)
        _elements.push_back(id);
}

void String::GetElementsBelow(const ElementId from_id, ElementType _type, std::vector<ElementId>& _elements)
{
}

bool String::GetNearestElement(const int x, const int y, ElementId& _id, int& dist)
{
    Rect r = GetAbsoluteRect();
    int d = r.DistToPoint(x, y);
    if (d < dist)
    {
        _id = id;
        dist = d;
        return true;
    }
    return false;
}

void String::ReSolve(bool if_error, bool force)
{
}

void String::ClearCache() const
{
    size_cache.clear();
    tab_size.Set(0, 0);
}

void String::SetStretchWidth(float val)
{
    stretch_width = val;
    last_stretch_width = val;
    ClearCache();
    UpdateRect();
}

//StringElements

StringElements::StringElements(Element* parent) :
    Elements(parent)
{
}

StringElements::StringElements(Element* parent, const std::u32string& _str) :
    Elements(parent),
    str(_str)
{
}

void StringElements::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    rapidjson::Value _str(ToBasicString(str).c_str(), alloc);
    value.AddMember("elements", _str, alloc);
}

bool StringElements::FromJson(Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.HasMember("elements") || !value["elements"].IsString())
        return false;
    str = ToUtfString(value["elements"].GetString());
    return true;
}

Elements* StringElements::Clone(Element* _parent)
{
    return new StringElements(_parent, str);
}

void StringElements::Draw() const
{
    String* p = (String*)parent;
    uint start = 0, size = 0;
    Rect r = p->GetAbsoluteRect();
    //draw the string by symbols
    if (parent->document->selection.Has(parent->id, start, size))
    {
        yutovo::Size s1(p->GetTextSize(start));
        yutovo::Size s2(p->GetTextSize(start + size));
        parent->window->DrawFillRect(Rect{r.left + s1.width, r.top, s2.width - s1.width, r.height}, p->draw_format->text_bg_selection_color);
    }
    if (p->document->config.pdf) //for pdf export
    {
        p->window->DrawText(ToBasicString(str), p->draw_format, r, p->draw_format->text_color, p->draw_format->text_bg_color, true);
    }
    else
    {
        StringFormatPtr fmt = p->draw_format;
        bool has_underline = fmt->underline;
        if (has_underline)
        {
            StringFormat f = *fmt;
            f.underline = false;
            fmt = p->document->string_formats->GetFormat(f);
        }
        for (int i = 0; i < str.length(); ++i)
        {
            if (str[i] == U'\t')
                continue;
            auto ch = str.substr(i, 1);
            yutovo::Size s(p->GetTextSize(i + 1));
            int w = p->document->GetCharWidth(p->draw_format, ch[0]);
            std::string sub = ToBasicString(ch);
            if (i >= start && i < start + size)
            {
                p->window->DrawText(sub, fmt, Rect{r.left + s.width - w, r.top, w, r.height}, 
                    p->draw_format->text_bg_color, p->draw_format->text_bg_selection_color, true);
            }
            else
            {
                p->window->DrawText(sub, fmt, Rect{r.left + s.width - w, r.top, w, r.height}, 
                    p->draw_format->text_color, p->draw_format->text_bg_color, true);
            }
        }

        if (has_underline)
        {
            yutovo::Size text_size = p->GetTextSize(str.length());
            if (text_size.width > 0)
            {
                int ascent = p->window->GetFontAscent(p->draw_format);
                p->window->DrawLine(r.left, r.top + ascent + 1, r.left + text_size.width, r.top + ascent + 1, p->draw_format->text_color);
            }
        }
    }
}

ElementPtr StringElements::Get(uint pos)
{
    if (pos > str.length())
        return nullptr;
    return parent->document->GetElement(parent->id);
}

ElementId StringElements::GetElementId(uint pos)
{
    assert(pos < str.length() + 1); //empty string has one caret pos
    ElementId id = parent->id;
    id.push_back(pos);
    return id;
}

void StringElements::Add(ElementPtr element)
{
}

void StringElements::Insert(ElementPtr element, const uint pos)
{
    assert(parent->document->IsString(element));
    assert(str.length() >= pos);
    std::u32string s = dynamic_cast<String*>(element.get())->ToText();

    uint start, size;
    if (selection->Has(parent->id, start, size))
    {
        selection->Remove(parent->id, start, size);
        str.insert(pos, s);
        selection->Add(parent->id, start, size);
    }
    else
        str.insert(pos, s);

    if (selection->Has(element, start, size))
    {
        selection->Add(parent->id, Count(), size);
        selection->Remove(element->id, start, size);
    }

#ifdef DEBUG
    parent->to_str = parent->ToText();
#endif
}

void StringElements::Remove(const ElementPtr element)
{
    assert(false);
}

void StringElements::RemoveAt(const uint pos, const int size)
{
    assert(str.length() >= pos + size);
    int p = -1;
    if (caret->IsInsideElement(parent->id))
    {
        p = caret->GetPos();
        if (p >= pos + size)
            caret->SetPos(p - size);
    }

    str.erase(str.begin() + pos, str.begin() + pos + size);
    selection->Remove(parent->id, pos, size);

#ifdef DEBUG
    parent->to_str = parent->ToText();
#endif
}

void StringElements::Replace(ElementPtr element, const uint pos)
{
    assert(parent->document->IsString(element));
    assert(str.length() >= pos);
    std::u32string s = element->ToText();

    uint start, size;
    if (selection->Has(parent->id, start, size))
    {
        selection->Remove(parent->id, start, size);
        str.insert(pos, s);
        selection->Add(parent->id, start, size);
    }
    else
        str.replace(pos, 1, s);

    if (selection->Has(element, start, size))
    {
        selection->Add(parent->id, Count(), size);
        selection->Remove(element->id, start, size);
    }

#ifdef DEBUG
    parent->to_str = parent->ToText();
#endif
}

void StringElements::Clear()
{
    str = U"";

#ifdef DEBUG
    parent->to_str = parent->ToText();
#endif
}

uint StringElements::Count() const
{
    return str.length();
}

uint StringElements::Size() const
{
    return 0;
}

Rect StringElements::GetCaretRect(const uint pos) const
{
    yutovo::Size s = ((String*)parent)->GetTextSize(pos);
    if (parent->document->insert_mode)
        return Rect(s.width, 0, 1, s.height);
    if (str.empty())
    {
        if (parent->type == ElementType::CODE_STRING)
            return Rect(parent->rect.left, parent->rect.top, parent->rect.width - 2, parent->rect.height - 2);
        return Rect(s.width, 0, 4, s.height);
    }
    if (pos == str.length())
        return Rect(s.width, 0, 1, s.height);
    yutovo::Size s2 = parent->window->GetTextSize(std::u32string(1, str[pos]), ((String*)parent)->draw_format);
    return Rect(s.width, 0, s2.width, s.height);
}

void StringElements::DrawCaret(const uint pos) const
{
    Rect r = parent->GetAbsoluteRect(GetCaretRect(pos));
    if (Count() == 0)
    {
        if (parent->document->insert_mode)
            parent->window->DrawLine(r.left + parent->rect.width / 2, r.top, r.left + parent->rect.width / 2, r.GetBottom() - 1, Color::Black());
        else
            parent->window->DrawRect(r.left, r.top, r.width - 2, r.height - 2, Color::Black());
    }
    else
    {
        if (parent->document->insert_mode || pos == str.length())
            parent->window->DrawLine(r.left, r.top, r.left, r.GetBottom() - 1, Color::Black());
        else
            parent->window->DrawRect(r.left, r.top, r.width - 1, r.height - 1, Color::Black());
    }
}

Rect StringElements::GetRect()
{
    yutovo::Size s = ((String*)parent)->GetTextSize(str.length());
    return Rect{0, 0, s.width, s.height};
}

Rect StringElements::GetRect(const uint pos)
{
    String* p = (String*)parent;
    yutovo::Size s1 = p->GetTextSize(pos);
    yutovo::Size s2 = p->GetTextSize(pos + 1);
    return Rect{s1.width, p->rect.top, s2.width - s1.width, s2.height};
}

bool StringElements::GetFirstCaretState(CaretState& caret_state, Selection* select)
{
    if (select && caret_state.IsInsideElement(parent->id))
        select->Add(parent->id, 0, caret_state.GetPos());
    caret_state.id = GetElementId(0);
    return true;
}

bool StringElements::GetLastCaretState(CaretState& caret_state, Selection* select)
{
    if (select && caret_state.IsInsideElement(parent->id))
        select->Add(parent->id, caret_state.GetPos(), str.length() - caret_state.GetPos());
    caret_state.id = GetElementId(str.length());
    return true;
}

bool StringElements::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    uint pos = caret_state.GetPos();
    if (pos == 0 || pos > str.length())
        return false;
    caret_state.SetState(GetElementId(pos - 1));
    if (select)
        select->Add(parent->id, pos - 1, 1);
    return true;
}

bool StringElements::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    uint pos = caret_state.GetPos();
    if (pos >= str.length())
        return false;
    caret_state.SetState(GetElementId(pos + 1));
    if (select)
        select->Add(parent->id, pos, 1);
    return true;
}

bool StringElements::GetWordLeftCaretState(CaretState& caret_state, Selection* select)
{
    uint pos = caret_state.GetPos();
    if (pos == 0 || pos > str.length())
        return false;
    for (int i = pos - 1; i > 0; --i)
    {
        if (IsSpace(str[i]))
        {
            while (IsSpace(str[i]))
                --i;
            ++i;
        }
        if ((IsOpenDelimiter(str[i - 1]) || IsCloseDelimiter(str[i]) || IsOpenDelimiter(str[i])) || 
            (IsCloseDelimiter(str[i - 1]) || IsDelimiter(str[i - 1]) || str[i] == U'.' || str[i] == U',') && !(str[i - 1] == U'.' || str[i - 1] == U','))
        {
            caret_state.SetState(GetElementId(i));
            if (select)
                select->Add(parent->id, i, pos - i);
            return true;
        }
    }
    if (select)
    {
        if (select->IsSelected(parent->parent->parent->id))
            select->Add(parent->parent->parent->id);
        else
            select->Add(parent->id, 0, pos);
    }
    return GetFirstCaretState(caret_state, nullptr);
}

bool StringElements::GetWordRightCaretState(CaretState& caret_state, Selection* select)
{
    uint pos = caret_state.GetPos();
    if (pos >= str.length())
        return false;
    if (pos == str.length() - 1 && !str.empty() && str[str.length() - 1] == ' ')
    {
        if (select)
            select->Add(parent->id, pos, 1);
        return parent->parent->GetWordRightCaretState(caret_state, select);
    }
    for (int i = pos + 1; i < str.length(); ++i)
    {
        while (IsSpace(str[i]))
            ++i;
        if ((IsCloseDelimiter(str[i]) || IsCloseDelimiter(str[i - 1])) ||
            (IsOpenDelimiter(str[i - 1]) || IsDelimiter(str[i - 1]) || str[i] == U'.' || str[i] == U',') && 
            !(str[i - 1] == U'.' || str[i - 1] == U','))
        {
            caret_state.SetState(GetElementId(i));
            if (select)
                select->Add(parent->id, pos, i - pos);
            return true;
        }
    }
    if (select)
    {
        if (pos == 0 && parent->parent->elements->Count() == 1 && parent->parent->parent->elements->Count() == 1)
            select->Add(parent->parent->parent->id);
        else
            select->Add(parent->id, pos, Count() - pos);
    }
    return GetLastCaretState(caret_state, nullptr);
}

bool StringElements::GetSelectOutCaretState(CaretState& caret_state, Selection* select)
{
    if (!select || !caret_state.IsInsideElement(parent->id))
        return false;
    
    static constexpr char32_t delims[] = U" \n\t\v\f\r!\"#$%&\'()*+,-./[\\]^`{|}~";
    auto is_delim = 
        [](char32_t ch)
        {
            for (char32_t d : delims)
            {
                if (ch == d)
                    return true;
            }
            return false;
        };

    int p = caret_state.GetPos() - 1;
    while (p >= 0)
    {
        char32_t ch = str[p];
        if (IsOpenDelimiter(ch) || IsCloseDelimiter(ch) || IsDelimiter(ch))
            break;
        --p;
    }
    int s1 = p++ + 1;
    while (p < (int)str.length())
    {
        char32_t ch = str[p];
        if (IsOpenDelimiter(ch) || IsCloseDelimiter(ch) || IsDelimiter(ch))
            break;
        ++p;
    }
    if (p - s1 <= 0)
        return false;
    select->Add(parent->id, s1, p - s1);
    caret_state.SetState(parent->id, p);
    return true;
}

std::string StringElements::ToHtml() const
{
    return ToBasicString(str);
}

std::u32string StringElements::ToText() const
{
    return str;
}

bool StringElements::IsOpenDelimiter(char32_t ch)
{
    static constexpr char32_t delims[] = U"«([{";
    for (char32_t d : delims)
    {
        if (ch == d)
            return true;
    }
    return false;
}

bool StringElements::IsCloseDelimiter(char32_t ch)
{
    static constexpr char32_t delims[] = U"»)]}";
    for (char32_t d : delims)
    {
        if (ch == d)
            return true;
    }
    return false;
}

bool StringElements::IsDelimiter(char32_t ch)
{
    static constexpr char32_t delims[] = U" \n\t\v\f\r!\"#$%&\'*+,-./\\^`|~";
    for (char32_t d : delims)
    {
        if (ch == d)
            return true;
    }
    return false;
}

bool StringElements::IsSpace(char32_t ch)
{
#ifdef _WIN32
    std::u32string s(1, ch);
    std::wstring w = boost::locale::conv::utf_to_utf<wchar_t>(s);
    WORD type;
    return GetStringTypeW(CT_CTYPE1, &w[0], 1, &type) && (type & C1_SPACE);
#else
    return std::isspace(ch);
#endif
}

}
