/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __COLUMN_H__
#define __COLUMN_H__

#include "code_row.h"

namespace yutovo
{

class CodeColumn : public CodeRow<>
{
public:
    CodeColumn(Document* _document);
    CodeColumn(Element* parent);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual bool Remake(bool with_elements = false);

    virtual bool GetBeginCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetEndCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);

    virtual void AddEmptyElement();

    virtual std::string ToHtml() const;
    virtual std::u32string ToText() const;
};

}

#endif
