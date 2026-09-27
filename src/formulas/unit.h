/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __UNIT_H__
#define __UNIT_H__

#include "assignment.h"

namespace yutovo
{

class Unit : public Assignment
{
public:
    Unit(Element* _parent, bool with_init = true);
    Unit(Document* _document, bool with_init = true);
    Unit(const Unit& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);
};

}

#endif
