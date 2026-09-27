/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "product.h"
#include "document.h"
#include "code_row.h"
#include "assignment.h"

namespace yutovo
{

//Product

Product::Product(Element* _parent, bool with_init) : 
    Iteration(_parent, U'∏', with_init)
{
    type = ElementType::PRODUCT;
}

Product::Product(Document* _document, bool with_init) : 
    Iteration(_document, U'∏', with_init)
{
    type = ElementType::PRODUCT;
}

Product::Product(const Product& source) :
    Iteration(source)
{
}

Element* Product::Clone()
{
    return new Product(*this);
}

Element* Product::Create(Element* _parent)
{
    return new Product(_parent);
}

Element* Product::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new Product(parent, false);
    return new Product(document, false);
}

std::u32string Product::ToText() const
{
    Assignment* lower = GetLower();
    CodeRow<>* upper = GetUpper();
    CodeRow<>* right = GetRight();
    if (!lower || !upper || !right)
        return U"";
    return U"prod(" + lower->ToText() + U"," + upper->ToText() + U"," + right->ToText() + U")";
}

void Product::ToParserString(ParserString& str)
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
    str.Add(right->id, U"=1");
    str.Add(id, U",");
    //result expression
    str.Add(right->id, var + U"_");
    str.Add(right->id, U"=");
    str.Add(right->id, var + U"_");
    str.Add(right->id, U"*(");
    right->ToParserString(str);
    str.Add(id, U"))");
}

}
