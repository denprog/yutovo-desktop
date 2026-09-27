/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "parser_string.h"

namespace yutovo
{

//ParserString

ParserString& ParserString::operator=(const ParserString& str)
{
    text = str.text;
    annotates = str.annotates;
    return *this;
}

bool ParserString::operator==(const ParserString& str) const
{
    return text == str.text;
}

bool ParserString::operator!=(const ParserString& str) const
{
    return text != str.text;
}

void ParserString::Add(const ElementId id, const std::u32string& _text)
{
    if (_text.empty())
        return;
    int start = text.length();
    text += _text;
    annotates.emplace_back(StringAnnotate{id, start, (int)_text.length()});
}

void ParserString::Add(const ParserString& str)
{
    text += str.text;
    annotates.insert(annotates.end(), str.annotates.begin(), str.annotates.end());
}

void ParserString::Reset()
{
    text = U"";
    annotates.clear();
}

void ParserString::Annotate(const ElementId id, const int start, const int size)
{
    annotates.emplace_back(StringAnnotate{id, start, size});
}

std::u32string& ParserString::Text()
{
    return text;
}

int ParserString::Length()
{
    return text.length();
}

ElementId ParserString::GetElement(const int pos, const int size)
{
    if (text.empty() && pos == 0 && annotates.size() == 1 && annotates[0].pos == 0)
        return annotates[0].id;

    ElementId res;
    int s = text.length() + 1;
    if (size > 0)
    {
        //find ElementId at the pos with the same size
        for (auto it = annotates.begin(); it != annotates.end(); ++it)
        {
            auto& a = *it;
            if (pos == a.pos && a.size == size)
                return a.id;
        }
    }
    //find ElementId at the pos with minimal size
    for (auto it = annotates.begin(); it != annotates.end(); ++it)
    {
        auto& a = *it;
        if (pos >= a.pos && pos < a.pos + a.size)
        {
            if (s > a.size)
            {
                res = a.id;
                s = a.size;
            }
        }
    }

    return res;
}

}
