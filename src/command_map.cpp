/*
 * Yutovo Desktop
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "command_map.h"
#include "document_widget.h"

namespace yutovo
{

//ShortcutsMap

void ShortcutsMap::Init(DocumentPtr _document, QWidget* document_widget)
{
    document = _document;

    //caret moving
    Add(QKeySequence("Left"), "", std::function<void ()>(std::bind(&Document::MoveCaretLeft, document.get(), false, false)));
    Add(QKeySequence("Right"), "", std::function<void ()>(std::bind(&Document::MoveCaretRight, document.get(), false, false)));
    Add(QKeySequence("Up"), "", std::function<void ()>(std::bind(&Document::MoveCaretUp, document.get(), false)));
    Add(QKeySequence("Down"), "", std::function<void ()>(std::bind(&Document::MoveCaretDown, document.get(), false)));

    Add(QKeySequence("Ctrl+Left"), "", std::function<void ()>(std::bind(&Document::MoveCaretWordLeft, document.get(), false)));
    Add(QKeySequence("Ctrl+Right"), "", std::function<void ()>(std::bind(&Document::MoveCaretWordRight, document.get(), false)));

    Add(QKeySequence("Home"), "", std::function<void ()>(std::bind(&Document::MoveCaretHome, document.get(), false)));
    Add(QKeySequence("End"), "", std::function<void ()>(std::bind(&Document::MoveCaretEnd, document.get(), false)));

    Add(QKeySequence("PgUp"), "", std::function<void ()>(std::bind(&Document::MoveCaretPageUp, document.get(), false)));
    Add(QKeySequence("PgDown"), "", std::function<void ()>(std::bind(&Document::MoveCaretPageDown, document.get(), false)));

    Add(QKeySequence("Ctrl+Home"), "", std::function<void ()>(std::bind(&Document::MoveCaretToDocumentBegin, document.get(), false)));
    Add(QKeySequence("Ctrl+End"), "", std::function<void ()>(std::bind(static_cast<uint(Document::*)(bool)>(&Document::MoveCaretToDocumentEnd), 
        document.get(), false)));

    //selection
    Add(QKeySequence("Shift+Left"), "", std::function<void ()>(std::bind(&Document::MoveCaretLeft, document.get(), true, true)));
    Add(QKeySequence("Shift+Right"), "", std::function<void ()>(std::bind(&Document::MoveCaretRight, document.get(), true, true)));
    Add(QKeySequence("Shift+Home"), "", std::function<void ()>(std::bind(&Document::MoveCaretHome, document.get(), true)));
    Add(QKeySequence("Shift+End"), "", std::function<void ()>(std::bind(&Document::MoveCaretEnd, document.get(), true)));
    Add(QKeySequence("Shift+Up"), "", std::function<void ()>(std::bind(&Document::MoveCaretUp, document.get(), true)));
    Add(QKeySequence("Shift+Down"), "", std::function<void ()>(std::bind(&Document::MoveCaretDown, document.get(), true)));
    Add(QKeySequence("Shift+PgUp"), "", std::function<void ()>(std::bind(&Document::MoveCaretPageUp, document.get(), true)));
    Add(QKeySequence("Shift+PgDown"), "", std::function<void ()>(std::bind(&Document::MoveCaretPageDown, document.get(), true)));

    Add(QKeySequence("Shift+Ctrl+Left"), "", std::function<void ()>(std::bind(&Document::MoveCaretWordLeft, document.get(), true)));
    Add(QKeySequence("Shift+Ctrl+Right"), "", std::function<void ()>(std::bind(&Document::MoveCaretWordRight, document.get(), true)));

    Add(QKeySequence("Shift+Ctrl+Home"), "", std::function<void ()>(std::bind(&Document::MoveCaretToDocumentBegin, document.get(), true)));
    Add(QKeySequence("Shift+Ctrl+End"), "", std::function<void ()>(std::bind(static_cast<uint(Document::*)(bool)>(&Document::MoveCaretToDocumentEnd), 
        document.get(), true)));

    Add(QKeySequence("Ctrl+A"), "", std::function<void ()>(std::bind(&Document::SelectAll, document.get())));

    //edit text
    Add(QKeySequence("Delete"), "", std::function<void ()>(std::bind(&Document::DeleteElements, document.get(), false, true)));
    Add(QKeySequence("Backspace"), "", std::function<void ()>(std::bind(&Document::DeleteElements, document.get(), true, true)));
    Add(QKeySequence("Return"), "", std::function<void ()>(std::bind(&Document::InsertParagraph, document.get(), true)));
    
    Add(QKeySequence("Tab"), "", std::function<void ()>(std::bind(static_cast<uint(Document::*)(const std::u32string&, bool)>(&Document::InsertString), 
        document.get(), U"	", true)));

    Add(QKeySequence("Insert"), "", std::function<void ()>(std::bind(&Document::SwitchInsertMode, document.get())));

    Add(QKeySequence("Ctrl+Z"), "", std::function<void ()>(std::bind(&Document::Undo, document.get())));
    Add(QKeySequence("Ctrl+Y"), "", std::function<void ()>(std::bind(&Document::Redo, document.get())));

    //edit code
    Add(QKeySequence("Ctrl+Shift+C"), "\\code", std::function<void ()>(std::bind(&Document::InsertCode, document.get(), false, true)));
    Add(QKeySequence("Ctrl+Shift+D"), "\\div", std::function<void ()>(std::bind(&Document::InsertDivision, document.get(), true)));
    Add(QKeySequence(""), '+', "\\plus", std::function<void ()>(std::bind(&Document::InsertPlus, document.get(), true)), CommandContext::Formula);
    Add(QKeySequence(""), '-', "\\minus", std::function<void ()>(std::bind(&Document::InsertMinus, document.get(), true)), CommandContext::Formula);
    Add(QKeySequence(""), '*', "\\times", std::function<void ()>(std::bind(&Document::InsertMultiply, document.get(), true)), CommandContext::Formula);
    Add(QKeySequence(""), '!', "\\excl", std::function<void ()>(std::bind(&Document::InsertExclamation, document.get(), true)), CommandContext::Formula);
    Add(QKeySequence(""), '&', "\\and", std::function<void ()>(std::bind(&Document::InsertAnd, document.get(), true)), CommandContext::Formula);
    Add(QKeySequence(""), '|', "\\or", std::function<void ()>(std::bind(&Document::InsertOr, document.get(), true)), CommandContext::Formula);
    Add(QKeySequence(""), '^', "\\xor", std::function<void ()>(std::bind(&Document::InsertXor, document.get(), true)), CommandContext::Formula);
    Add(QKeySequence(""), '%', "\\percent", std::function<void ()>(std::bind(&Document::InsertPercent, document.get(), true)), CommandContext::Formula);
    Add(QKeySequence(""), ',', "\\comma", std::function<void ()>(std::bind(&Document::InsertComma, document.get(), true)), CommandContext::Formula);
    Add(QKeySequence("/"), "\\div", std::function<void ()>(std::bind(&Document::InsertDivision, document.get(), true)), CommandContext::Formula);
    Add(QKeySequence("Ctrl+Shift+P"), "\\pow", std::function<void()>(std::bind(&Document::InsertPower, document.get(), true)), CommandContext::Formula);
    Add(QKeySequence("Ctrl+Shift+S"), "\\sub", std::function<void ()>(std::bind(&Document::InsertSubscript, document.get(), true)), CommandContext::Formula);
    Add(QKeySequence("Ctrl+Shift+N"), "\\nth", std::function<void ()>(std::bind(&Document::InsertNthRoot, document.get(), true)));
    Add(QKeySequence("Ctrl+Shift+Q"), "\\sqrt", std::function<void ()>(std::bind(&Document::InsertSquareRoot, document.get(), true)));
    Add(QKeySequence(""), '=', "\\equal", std::function<void ()>(std::bind(&Document::InsertEquation, document.get(), ResultType::AUTO, true)), 
        CommandContext::Formula);
    Add(QKeySequence(""), "\\eq_real", std::function<void ()>(std::bind(&Document::InsertEquation, document.get(), ResultType::REAL, true)), 
        CommandContext::Formula);
    Add(QKeySequence(""), "\\eq_int", std::function<void ()>(std::bind(&Document::InsertEquation, document.get(), ResultType::INTEGER, true)), 
        CommandContext::Formula);
    Add(QKeySequence(""), "\\eq_rat", std::function<void ()>(std::bind(&Document::InsertEquation, document.get(), ResultType::RATIONAL, true)), 
        CommandContext::Formula);
    Add(QKeySequence(""), "\\eq_comp", std::function<void ()>(std::bind(&Document::InsertEquation, document.get(), ResultType::COMPLEX, true)), 
        CommandContext::Formula);
    Add(QKeySequence(""), '(', "\\open_round_bracket", std::function<void ()>(std::bind(&Document::InsertOpenRoundBracket, document.get(), true)), 
        CommandContext::Formula);
    Add(QKeySequence(""), ')', "\\close_round_bracket", std::function<void ()>(std::bind(&Document::InsertCloseRoundBracket, document.get(), true)), 
        CommandContext::Formula);
    Add(QKeySequence(""), '[', "\\open_square_bracket", std::function<void ()>(std::bind(&Document::InsertOpenSquareBracket, document.get(), true)), 
        CommandContext::Formula);
    Add(QKeySequence(""), ']', "\\close_square_bracket", std::function<void ()>(std::bind(&Document::InsertCloseSquareBracket, document.get(), true)), 
        CommandContext::Formula);
    Add(QKeySequence(""), ':', "\\assign", std::function<void ()>(std::bind(&Document::InsertAssignment, document.get(), true)), CommandContext::Formula);
    Add(QKeySequence(""), '~', "\\unit", std::function<void ()>(std::bind(static_cast<uint(Document::*)(bool)>(&Document::InsertUnit), 
        document.get(), true)), CommandContext::Formula);

    //main window
    Add(QKeySequence("Ctrl+Tab"), std::function<void ()>(std::bind(&DocumentWidget::OnNextEditorTab, (DocumentWidget*)document_widget)));
    Add(QKeySequence("Ctrl+]"), std::function<void ()>(std::bind(&DocumentWidget::OnNextEditorTab, (DocumentWidget*)document_widget)));
    Add(QKeySequence("Ctrl+Shift+Tab"), std::function<void ()>(std::bind(&DocumentWidget::OnPrevEditorTab, (DocumentWidget*)document_widget)));
    Add(QKeySequence("Ctrl+["), std::function<void ()>(std::bind(&DocumentWidget::OnPrevEditorTab, (DocumentWidget*)document_widget)));
}

bool ShortcutsMap::Call(const QKeySequence& shortcut, QChar symbol, const EditorState& editor_state)
{
    struct CommandMapsVisitor
    {
        CommandMapsVisitor(const QKeySequence& _shortcut, QChar _symbol, Document* _document, const EditorState& _editor_state, bool& _res) :
            shortcut(_shortcut),
            symbol(_symbol),
            document(_document),
            editor_state(_editor_state),
            res(_res)
        {
        }

        void operator()(CommandMapVoid& m)
        {
            if (m.shortcut == shortcut || (m.symbol != QChar() && m.symbol == symbol))
            {
                switch (m.context)
                {
                case CommandContext::Formula:
                    if (document->GetParentId(editor_state.caret_state.id, ElementType::CODE_BLOCK) == ElementId{})
                        return;
                    break;
                case CommandContext::Text:
                    if (document->GetParentId(editor_state.caret_state.id, ElementType::CODE_BLOCK) != ElementId{})
                        return;
                    break;
                }
                m();
                res = true;
            }
        }

        void operator()(CommandMapString& m)
        {
        }

        const QKeySequence& shortcut;
        QChar symbol;
        Document* document;
        const EditorState& editor_state;
        bool& res;
    };

    for (auto& c : command_maps)
    {
        bool res = false;
        std::visit(CommandMapsVisitor{shortcut, symbol, document.get(), editor_state, res}, c);
        if (res)
            return true;
    }

    return false;
}

void ShortcutsMap::Add(QKeySequence shortcut, std::string command, std::function<void (void)> func, CommandContext context)
{
    command_maps.push_back(CommandMapVoid{shortcut, QChar(), command, context, func});
}

void ShortcutsMap::Add(QKeySequence shortcut, QChar symbol, std::string command, std::function<void (void)> func, CommandContext context)
{
    command_maps.push_back(CommandMapVoid{shortcut, symbol, command, context, func});
}

void ShortcutsMap::Add(QKeySequence shortcut, std::string command, std::function<void (const std::string&)> func, CommandContext context)
{
    command_maps.push_back(CommandMapString{shortcut, QChar(), command, context, func});
}

void ShortcutsMap::Add(QKeySequence shortcut, QChar symbol, std::string command, std::function<void (const std::string&)> func, CommandContext context)
{
    command_maps.push_back(CommandMapString{shortcut, symbol, command, context, func});
}

void ShortcutsMap::Add(QKeySequence shortcut, std::function<void ()> func)
{
    command_maps.push_back(CommandMapVoid{shortcut, QChar(), "", CommandContext::Everywhere, func});
}

}
