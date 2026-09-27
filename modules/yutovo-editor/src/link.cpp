/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "link.h"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace yutovo
{

//Link

Link::Link(Element* parent) : 
    String(parent)
{
    type = ElementType::LINK;
    if (document)
    {
        format = document->GetStringFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
            format->subscript, format->superscript, document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
    }
}

Link::Link(Element* parent, const std::string _str, const std::string _url, bool _translate) : 
    String(parent, _str, _translate),
    url(ToUtfString(_url))
{
    type = ElementType::LINK;
    if (!format)
    {
        if (document->GetCurrentStringFormat(format))
        {
            format = document->GetStringFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
                format->subscript, format->superscript, document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
        }
    }
    else
    {
        format = document->GetStringFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
            format->subscript, format->superscript, document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
    }
}

Link::Link(Element* parent, const std::string _str, const std::string _url, const StringFormatPtr _format, bool _translate) : 
    String(parent, _str, _format, _translate),
    url(ToUtfString(_url))
{
    type = ElementType::LINK;
    format = document->GetStringFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
        format->subscript, format->superscript, document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
}

Link::Link(Element* parent, const std::u32string _str, const std::u32string _url) : 
    String(parent, _str),
    url(_url)
{
    type = ElementType::LINK;
    format = document->GetStringFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
        format->subscript, format->superscript, document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
}

Link::Link(Element* parent, const std::u32string _str, const std::u32string _url, const StringFormatPtr _format) : 
    String(parent, _str, _format),
    url(_url)
{
    type = ElementType::LINK;
    format = document->GetStringFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
        format->subscript, format->superscript, document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
}

Link::Link(Document* _document) : 
    String(_document)
{
    type = ElementType::LINK;
    format = document->GetStringFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
        format->subscript, format->superscript, document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
}

Link::Link(Document* _document, const std::u32string _str, const std::u32string _url) :
    String(_document, _str),
    url(_url)
{
    type = ElementType::LINK;
    if (!format)
    {
        if (document->GetCurrentStringFormat(format))
        {
            format = document->GetStringFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
                format->subscript, format->superscript, document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
        }
    }
    else
    {
        format = document->GetStringFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
            format->subscript, format->superscript, document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
    }
}

Link::Link(Document* _document, const std::string _str, const std::string _url, const StringFormatPtr _format) : 
    String(_document, _str, _format),
    url(ToUtfString(_url))
{
    type = ElementType::LINK;
    format = document->GetStringFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
        format->subscript, format->superscript, document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
}

Link::Link(Document* _document, const std::u32string _str, const std::u32string _url, const StringFormatPtr _format): 
    String(_document, _str, _format),
    url(_url)
{
    type = ElementType::LINK;
    format = document->GetStringFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
        format->subscript, format->superscript, document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
}

Element* Link::Clone()
{
    return new Link(*this);
}

Element* Link::Create(Element* parent)
{
    return new Link(parent);
}

Element* Link::Create(Element* parent, const std::u32string _str, const std::u32string _url, const StringFormatPtr _format)
{
    return new Link(parent, _str, _url, _format);
}

Element* Link::Create(Element* parent, const std::u32string _str, const StringFormatPtr _format)
{
    return new Link(parent, _str, U"", _format);
}

void Link::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    String::ToJson(value, alloc);
    rapidjson::Value _url(ToBasicString(url).c_str(), alloc);
    value.AddMember("url", _url, alloc);
}

Element* Link::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.HasMember("url") || !value["url"].IsString())
        return nullptr;
    auto url = value["url"].GetString();
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
                return new Link(parent, "", url, f);
            return new Link(document, "", url, f);
        }
    }
    if (parent)
        return new Link(parent, "", url);
    return new Link(document, U"", ToUtfString(url));
}

bool Link::Merge(const ElementPtr with_element)
{
    if (with_element->type != ElementType::LINK)
        return false;
    return String::Merge(with_element);
}

bool Link::CanMerge(const ElementPtr with_element)
{
    if (!editable || with_element->type != ElementType::LINK)
        return false;
    Link* el = (Link*)with_element.get();
    if (el->format != format)
        return false;
    return can_merge;
}

bool Link::AfterInsert(bool with_undo)
{
    if (!caret)
        return false;
    CaretState c;
    if (GetLastCaretState(c, nullptr))
        caret->SetState(c);
    return true;
}

StringFormatPtr Link::GetStringFormat() const
{
    if (format)
        return format;
    return parent->GetStringFormat();
}

std::string Link::ToHtml() const
{
    std::string s = "<a href=\"" + ToBasicString(url) + "\" ";
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
    if (format->bold)
        s += "<strong>";
    if (format->italic)
        s += "<em>";
    s += elements->ToHtml();
    if (format->italic)
        s += "</em>";
    if (format->bold)
        s += "</strong>";
    s += "</a>";
    return s;
}

bool Link::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    if (_elements.size() == 1)
    {
        if (_elements[0]->type == ElementType::LINK)
        {
            Link* link = (Link*)_elements[0].get();
            if (link->ToText() == U"" || link->url.empty() || (link->ToText() == ToText() && url == link->url))
                return false;

            format = document->GetStringFormat(format->family, format->size, format->bold, format->italic, format->underline, format->strikethrough, 
                format->subscript, format->superscript, document->config.link_color, format->text_bg_color, format->text_bg_selection_color);

            if (with_undo)
                document->StoreUndo(id);
            SetString(link->ToText());
            caret->SetState(id, elements->Count());
            url = link->url; //just change the url
            changed_element = id;
            return true;
        }
        else if (document->IsString(_elements[0]))
        {
            if (caret->IsInsideElement(id) && caret->GetPos() > 0 && caret->GetPos() < elements->Count())
            {
                size_cache.clear();
                String* s = dynamic_cast<String*>(_elements[0].get());
                if (!s->format || (format && s->format->family == format->family && s->format->size == format->size))
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
        }
    }
    return String::InsertElements(_elements, insert_mode, with_undo, changed_element);
}

void Link::Visit()
{
    window->OnLinkClicked(id, url);
    format = document->GetStringFormat(format->family, format->size, format->bold, format->italic, format->underline, format->strikethrough, 
        format->subscript, format->superscript, document->config.link_visited_color, format->text_bg_color, format->text_bg_selection_color);
    document->Redraw(id, false);
}

}
