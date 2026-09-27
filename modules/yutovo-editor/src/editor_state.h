/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __EDITOR_STATE_H__
#define __EDITOR_STATE_H__

#include "caret_state.h"
#include "selection.h"

namespace yutovo
{

struct EditorState
{
    bool operator==(const EditorState& s);
    bool operator!=(const EditorState& s);

    bool IsEmpty();
    
#ifdef TEST
    std::string ToString() const;
#endif

    CaretState caret_state;
    SelectionState selection_state;
};

struct LogicalEditorState
{
    bool IsEmpty();
    
    LogicalCaretState caret_state;
    LogicalSelectionState selection_state;
};

}

#endif
