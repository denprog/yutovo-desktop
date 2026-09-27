/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "editor_state.h"
#include <string>

namespace yutovo
{

//EditorState

bool EditorState::operator==(const EditorState& s)
{
    return caret_state == s.caret_state && selection_state == s.selection_state;
}

bool EditorState::operator!=(const EditorState& s)
{
    return caret_state != s.caret_state || selection_state != s.selection_state;
}

bool EditorState::IsEmpty()
{
    return caret_state.IsEmpty();
}

#ifdef TEST
std::string EditorState::ToString() const
{
    return caret_state.ToString() + " [" + selection_state.ToString() + "]";
}
#endif

//LogicalEditorState

bool LogicalEditorState::IsEmpty()
{
    return caret_state.IsEmpty();
}

}
