/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "element.h"

namespace yutovo
{

typedef unsigned char uchar;

//Image
class Image : public Element
{
public:
    Image(Element* _parent, const std::string& _image_base64);
    Image(Element* _parent, const std::vector<unsigned char>& _picture);
    Image(Document* _document, const std::string& _image_base64);
    Image(Document* _document, const std::vector<unsigned char>& _picture);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual bool Remake(bool with_elements = false);
    virtual void Rescale() const;

    virtual void UpdateRect(bool with_elements = false);

    virtual void Draw() const;

    virtual bool HasCaretState();
    virtual bool HasLastCaretState();

    virtual std::string ToHtml() const;

private:
    void Base64Encode();
    void Base64Decode();

    void SetBrokenImage();

public:
    std::string image_base64, last_image_base64;
    std::vector<unsigned char> picture; //only PNG format for a while

private:
    mutable Size image_size;
    static const std::string broken_image;
};

}

#endif
