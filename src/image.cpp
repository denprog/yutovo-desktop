/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "image.h"
#include "document.h"

namespace yutovo
{

//Image

const std::string Image::broken_image = "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";

Image::Image(Element* _parent, const std::string& _image_base64) :
    Element(_parent),
    image_base64(_image_base64)
{
    type = ElementType::IMAGE;
    editable = false;

    Base64Decode();

    image_size.Set(window->GetImageSize(picture), document->config.scale);
    if (image_size.width == 0 || image_size.height == 0)
        SetBrokenImage(); //replace with the broken image
}

Image::Image(Element* _parent, const std::vector<unsigned char>& _picture) :
    Element(_parent),
    picture(_picture)
{
    type = ElementType::IMAGE;
    editable = false;
    image_size.Set(window->GetImageSize(picture), document->config.scale);
    if (image_size.width == 0 || image_size.height == 0)
        SetBrokenImage(); //replace with the broken image
    Base64Encode();
}

Image::Image(Document* _document, const std::string& _image_base64) :
    Element(_document),
    image_base64(_image_base64)
{
    type = ElementType::IMAGE;
    editable = false;

    Base64Decode();

    image_size.Set(window->GetImageSize(picture), document->config.scale);
    if (image_size.width == 0 || image_size.height == 0)
        SetBrokenImage(); //replace with the broken image
}

Image::Image(Document* _document, const std::vector<unsigned char>& _picture) :
    Element(_document),
    picture(_picture)
{
    type = ElementType::IMAGE;
    editable = false;
    image_size.Set(window->GetImageSize(picture), document->config.scale);
    if (image_size.width == 0 || image_size.height == 0)
        SetBrokenImage(); //replace with the broken image
    Base64Encode();
}

Element* Image::Clone()
{
    return new Image(*this);
}

Element* Image::Create(Element* parent)
{
    return new Image(parent, "");
}

void Image::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Element::ToJson(value, alloc);
    rapidjson::Value _image_base64(image_base64.c_str(), alloc);
    value.AddMember("image_base64", _image_base64, alloc);
}

Element* Image::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.HasMember("image_base64") || !value["image_base64"].IsString())
        return nullptr;
    auto image_base64 = value["image_base64"].GetString();
    if (parent)
        return new Image(parent, image_base64);
    return new Image(document, image_base64);
}

bool Image::Remake(bool with_elements)
{
    UpdateRect();

    bool changed = (rect != last_rect);
    last_rect = rect;
    return changed;
}

void Image::Rescale() const
{
    image_size.Set(window->GetImageSize(picture), document->config.scale);
}

void Image::UpdateRect(bool with_elements)
{
    if (document->config.pdf) //for pdf export
        image_size = window->GetImageSize(picture);
    rect.SetSize(image_size.width + 2, image_size.height + 2);
    baseline = image_size.height;
}

void Image::Draw() const
{
    auto r = GetAbsoluteRect();
    window->DrawImage(r.left + 1, r.top + 1, image_size.width, image_size.height, picture);
    if (document->selection.IsSelected(id))
    {
        const auto f = GetStringFormat();
        if (f)
            window->DrawRect(r, f->text_bg_selection_color);
    }
}

bool Image::HasCaretState()
{
    return true;
}

bool Image::HasLastCaretState()
{
    return true;
}

std::string Image::ToHtml() const
{
    return "<img src=\"data:image/png;base64," + (last_image_base64.empty() ? image_base64 : last_image_base64) + "\">";
}

void Image::Base64Encode()
{
    image_base64 = yutovo::Base64Encode(picture);
}

void Image::Base64Decode()
{
    std::vector<int> t(256, -1);
    for (int i = 0; i < 64; ++i)
        t[base64_chars[i]] = i;

    uint32_t val = 0;
    int valb = -8;
    for (uchar c : image_base64)
    {
        if (t[c] == -1)
            break;
        val = (val << 6) + t[c];
        valb += 6;
        if (valb >= 0)
        {
            picture.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
}

void Image::SetBrokenImage()
{
    last_image_base64 = image_base64; //to save real image, not "broken stub"
    image_base64 = broken_image;
    picture.clear();
    Base64Decode();
    image_size.Set(window->GetImageSize(picture), document->config.scale);
}

}
