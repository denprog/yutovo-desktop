/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "sum.h"
#include "document.h"
#include "code_row.h"
#include "assignment.h"

namespace yutovo
{

//Sum

Sum::Sum(Element* _parent, bool with_init) : 
    Iteration(_parent, U'Σ', with_init)
{
    type = ElementType::SUM;
}

Sum::Sum(Document* _document, bool with_init) : 
    Iteration(_document, U'Σ', with_init)
{
    type = ElementType::SUM;
}

Sum::Sum(const Sum& source) :
    Iteration(source)
{
}

Element* Sum::Clone()
{
    return new Sum(*this);
}

Element* Sum::Create(Element* _parent)
{
    return new Sum(_parent);
}

Element* Sum::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new Sum(parent, false);
    return new Sum(document, false);
}

std::u32string Sum::ToText() const
{
    Assignment* lower = GetLower();
    CodeRow<>* upper = GetUpper();
    CodeRow<>* right = GetRight();
    if (!lower || !upper || !right)
        return U"";
    return U"sum(" + lower->ToText() + U"," + upper->ToText() + U"," + right->ToText() + U")";
}

void Sum::ToParserString(ParserString& str)
{
    Assignment* lower = GetLower();
    CodeRow<>* upper = GetUpper();
    CodeRow<>* right = GetRight();
    
    str.Add(id, U"loop(");
    lower->ToParserString(str); //loop variable
    str.Add(id, U",(");
    std::u32string var;
    ElementPtr el = document->FindByType(lower->id, ElementType::ASSIGNMENT);
    if (el)
    {
        //condition
        el = el->elements->Get(0);
        if (el)
        {
            var = el->ToText();
            str.Add(upper->id, var);
            str.Add(upper->id, U"<=");
            str.Add(upper->id, upper->ToText());
        }
    }
    str.Add(id, U"),");
    //increment of the loop variable
    str.Add(upper->id, var);
    str.Add(upper->id, U"=");
    str.Add(upper->id, var);
    str.Add(upper->id, U"+1");
    str.Add(id, U",");
    //result variable
    str.Add(right->id, var + U"_");
    str.Add(right->id, U"=0");
    str.Add(id, U",");
    //result expression
    str.Add(right->id, var + U"_");
    str.Add(right->id, U"=");
    str.Add(right->id, var + U"_");
    str.Add(right->id, U"+");
    right->ToParserString(str);
    str.Add(id, U")");
}

}
