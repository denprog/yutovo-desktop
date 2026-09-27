/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __CARET_H__
#define __CARET_H__

#include <memory>
#include <thread>
#include <mutex>
#include "window.h"
#include "caret_state.h"
#include "element.h"

namespace yutovo
{

class Text;

//Caret of the document
class Caret
{
public:
    Caret(Document* _document);

    void SetState(const CaretState& caret_state, bool update_x_pos = true);
    void SetState(const LogicalCaretState& caret_state, bool update_x_pos = true);
    void SetState(const ElementId id, const uint pos, bool update_x_pos = true);
    void SetState(const ElementId id, bool update_x_pos = true);
    void SetPos(const uint pos, bool update_x_pos = true);

    Element* GetElement() const;
    int GetPos() const;

    CaretState GetCaretState();
    LogicalCaretState GetLogicalCaretState();

    bool IsVisible();
    void SetVisible(bool _visible);

    void Show();
    void Hide();
    void Blink();

    void MoveToDocumentBegin(Selection* selection);
    void MoveToDocumentEnd(Selection* selection);
    void MoveHome(Selection* selection);
    void MoveEnd(Selection* selection);
    void MoveLeft(Selection* selection);
    void MoveRight(Selection* selection);
    void MoveUp(Selection* selection);
    void MoveDown(Selection* selection);
    void MoveWordLeft(Selection* selection);
    void MoveWordRight(Selection* selection);
    void MovePageUp(Selection* selection);
    void MovePageDown(Selection* selection);

    void SelectOut(Selection* selection);

    bool IsInsideElement(const ElementId& id);
    bool IsOnElement(const ElementId& id);

    void UpdateXPos();

    void Update();

    void Reset();

public:
    bool block = false; //don't do any operations with caret
    bool notify = true;
    bool left_up_direction = false; //true: leftward or upward, false: rightward or downward

private:
    ElementPtr element = nullptr;
    int str_pos = -1; //if element is string, then it is position in the element
    bool last_pos = false;

    bool show = false;
    bool visible = true;

    friend class Document;

    Document* document;
    Window* window;

    Rect caret_rect;

    Element* last_x_element = nullptr;
    uint last_x_pos = 0;
};

}

#endif
