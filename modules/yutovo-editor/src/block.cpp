/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "block.h"
#include "caret.h"
#include "document.h"
#include "str.h"
#include "paragraph.h"
#include "formulas/code_paragraph.h"
#include "row.h"

namespace yutovo
{

//Block

Block::Block(Element* _parent) :
    Element(_parent)
{
}

Block::Block(Document* _document) :
    Element(_document)
{
}

void Block::Normalize()
{
    Element::Normalize();

    if (elements->Count() == 0)
    {
        AddEmptyElement(); //block has to have at least one paragraph
        CaretState c;
        GetFirstCaretState(c, nullptr);
        caret->SetState(c);
    }
}

bool Block::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    const CaretState before_state = caret->GetCaretState();
    CaretState last;
    auto r = document->FindElementOrParent(caret->GetElement()->id, ElementType::ROW);
    if (r)
        r->GetLastCaretState(last, nullptr);
    else
        caret->GetElement()->GetLastCaretState(last, nullptr);

    if (_elements.size() != 1 || !document->IsParagraph(_elements[0]))
    {
        if (document->IsParagraph(before_state.id) && document->IsRow(_elements[0]))
        {
            ElementPtr el = document->GetElement(before_state.id);
            return el->InsertElements(_elements, insert_mode, with_undo, changed_element);
        }

        std::vector<ElementPtr> _els;
        if (_elements[0]->type == ElementType::TEXT) //insert from Paste
        {
            if (_elements[0]->elements->Count() == 0)
                return false;
            for (int i = 0; i < _elements[0]->elements->Count(); ++i)
                _els.push_back(_elements[0]->elements->Get(i));
        }
        else
        {
            _els = _elements;
        }

        ElementPtr cur = document->GetElement(before_state.id);
        std::vector<ElementPtr> els;
        //insert the paragraphs
        if (document->pasting)
        {
            int i = 0;
            if (document->IsParagraph(_els[0]))
            {
                if (!_els[0]->IsEmpty())
                {
                    ElementPtr row = ((Paragraph*)_els[i].get())->GetPlainRow();
                    els.clear();
                    els.push_back(row);
                    cur = document->GetElement(caret->GetElement()->id);
                    if (!cur->InsertElements(els, insert_mode, with_undo, changed_element))
                        return false;
                    i = 1;

                    if (_els.size() > 1 && _els[1]->IsEmpty())
                    {
                        els.clear();
                        els.push_back(document->CreateParagraph(cur->id));
                        if (!InsertElements(els, insert_mode, with_undo, changed_element))
                            return false;
                    }
                }
            }
            for (; i < _els.size(); ++i)
            {
                els.clear();
                if (document->IsParagraph(_els[i]))
                {
                    cur = document->GetElement(caret->GetElement()->id);
                    if (document->FindParent(cur->id, ElementType::CODE_BLOCK))
                    {
                        ElementPtr paragraph = document->FindParentParagraph(cur->id);
                        int k = elements->GetElementPos(paragraph->id);
                        elements->Insert(ElementPtr(new CodeParagraph<>(this, true)), k + 1);
                        CaretState c;
                        elements->Get(k + 1)->GetFirstCaretState(c, nullptr);
                        caret->SetState(c);
                        if (with_undo)
                            document->StoreUndo(id, k + 1, 1, 0, UndoTask::UndoOperation::DELETE);

                        ElementPtr row = ((Paragraph*)_els[i].get())->GetPlainRow();
                        els.clear();
                        els.push_back(row);
                        cur = document->GetElement(caret->GetElement()->id);
                        if (!cur->InsertElements(els, insert_mode, with_undo, changed_element))
                            return false;
                    }
                    else
                    {
                        Paragraph* p = ((Paragraph*)_els[i].get());
                        p->MakePlain();
                        els.push_back(_els[i]);
                        if (!InsertElements(els, insert_mode, with_undo, changed_element))
                            return false;
                    }
                    continue;
                }
                els.push_back(_els[i]);
                cur = document->GetParent(before_state.id);
                if (!cur || !cur->InsertElements(els, insert_mode, with_undo, changed_element))
                    return false;
            }
        }
        else
        {
            els.push_back(_els[0]);
            if (!cur)
                cur = document->GetParent(before_state.id);
            assert(cur.get() != this);
            if (!cur || !cur->InsertElements(els, insert_mode, with_undo, changed_element))
                return false;
            for (int i = 1; i < _els.size(); ++i)
            {
                if (!document->IsParagraph(_els[i]))
                    return false;
                els.clear();
                _els[i]->MakePlain();
                els.push_back(_els[i]);
                if (!InsertElements(els, insert_mode, with_undo, changed_element))
                    return false;
            }
        }
        changed_element = id;
        return true;
    }

    ElementPtr insert_element(_elements[0]->Clone());
    ElementPtr el = document->GetParent(before_state.id);
    ElementPtr paragraph = document->FindParentParagraph(el->id);
    ElementPtr new_row, row;
    bool caret_next_row = false;

    if (!paragraph)
    {
        elements->Insert(insert_element, caret->GetPos());
        row = insert_element->elements->Get(0);
    }
    else
    {
        if (with_undo)
            document->StoreUndo(paragraph->id);

        row = document->FindParentRow(el->id);
        int k = elements->GetElementPos(paragraph->id);
        int p = row->elements->GetElementPos(el->id);
        CaretState row_start;
        row->GetFirstCaretState(row_start, nullptr);

        if (caret->GetCaretState() == row_start)
        {
            if (paragraph->elements->GetElementPos(row->id) == 0)
            {
                elements->Insert(insert_element, (document->pasting && paragraph->type == ElementType::PARAGRAPH && row->IsEmpty()) ? k + 1 : k);
            }
            else
            {
                elements->Insert(insert_element, k + 1);
                new_row = insert_element->elements->Get(0); //move elements into this one row, which will be splitted during paragraph formatting
                if (new_row->elements->Count() == 0)
                    new_row->AddEmptyElement();
                for (int i = 0; i < row->elements->Count();) //move all elements at the right side of the row
                    new_row->elements->Move(row->elements->Get(i), new_row->elements->Count());
                caret_next_row = true;
            }
        }
        else
        {
            elements->Insert(insert_element, k + 1);
            caret_next_row = true;
        }

        insert_element->AfterInsert(with_undo);

        if (caret_next_row)
        {
            new_row = insert_element->elements->Get(0); //move elements into this one row, which will be splitted during paragraph formatting
            if (new_row->elements->Count() == 0)
                new_row->AddEmptyElement();

            if (p >= 0)
            {
                if (!el->SplitAt(before_state.GetPos()) && before_state.GetPos() == 0) //try to split current element
                    --p;
                if (before_state.GetPos() != 0 || p >= 0)
                {
                    for (int i = p + 1; i < row->elements->Count();) //move all elements at the right side of the row
                        new_row->elements->Move(row->elements->Get(i), new_row->elements->Count());
                }
            }
            else
            {
                el = document->GetElement(before_state.id);
                if (el)
                {
                    p = row->elements->GetElementPos(el->id);
                    if (p >= 0)
                    {
                        for (int i = p; i < row->elements->Count();) //move all elements at the right side of the row
                            new_row->elements->Move(row->elements->Get(i), new_row->elements->Count());
                    }
                }
            }

            int r_pos = paragraph->elements->GetElementPos(row->id);
            if (r_pos >= 0)
            {
                for (int i = r_pos + 1; i < paragraph->elements->Count();) //move the rest rows of the paragraph
                {
                    ElementPtr r = paragraph->elements->Get(i);
                    for (int j = 0; j < r->elements->Count();)
                        new_row->elements->Move(r->elements->Get(j), new_row->elements->Count());
                    paragraph->elements->RemoveAt(i, 1);
                }
            }
        }
    }

    if (paragraph)
        paragraph->Normalize();
    if (new_row)
        new_row->parent->Normalize();

    if (with_undo)
        document->StoreUndo(id, elements->GetElementPos(insert_element->id), 1, 0, UndoTask::UndoOperation::DELETE);

    CaretState after;
    if (document->pasting && insert_element->GetLastCaretState(after, nullptr))
    {
        CaretState c;
        if (caret_next_row && new_row->GetFirstCaretState(c, nullptr))
            caret->SetState(c);
        else if (last == before_state)
            caret->SetState(after);
    }
    else
    {
        if (caret_next_row)
        {
            if (new_row->GetFirstCaretState(after, nullptr))
                document->caret->SetState(after);
        }
        else
        {
            if (row->GetFirstCaretState(after, nullptr))
                document->caret->SetState(after);
        }
    }

    if (new_row)
        new_row->Normalize();
    
    changed_element = id;
    
#ifdef DEBUG
    to_str = ToText();
#endif
    return true;
}

bool Block::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    uint start, size;
    if (selection->Has(id, start, size))
    {
        if (Element::DeleteElements(left, with_undo, changed_element))
            return true;
    }

    CaretState before_state = caret->GetCaretState();
    ElementPtr el = document->GetElement(before_state.id);
    if (!el)
    {
        CaretState last_state;
        el = document->GetElement(caret->GetElement()->id);
        if (el)
        {
            el->GetLastCaretState(last_state, nullptr);
            if (left || before_state != last_state)
            {
                if (!parent)
                    return false;
                return parent->DeleteElements(left, with_undo, changed_element);
            }
        }
        else
        {
            if (!parent)
                return false;
            return parent->DeleteElements(left, with_undo, changed_element);
        }
    }
    int p = 0;
    ElementPtr dest_row;
    if (document->IsParagraph(el))
    {
        p = elements->GetElementPos(el->id) - 1;
    }
    else
    {
        ElementPtr paragraph = document->FindParentParagraph(el->id);

        p = elements->GetElementPos(paragraph->id);
        if (left && (p <= 0 || !elements->Get(p - 1)->visible))
            return false;
        if (!left && p == elements->Count() - 1)
            return false;
        
        if (with_undo)
            document->StoreUndo(id, left ? p - 1 : p, 2, 1);

        //merge current paragraph with the above one
        auto dest_p = left ? elements->Get(p - 1) : elements->Get(p);
        dest_row = dest_p->elements->Get(dest_p->elements->Count() - 1);
        if (dest_row)
        {
            auto source_p = left ? elements->Get(p) : elements->Get(p + 1);
            for (uint i = 0; i < source_p->elements->Count(); ++i)
            {
                auto row = source_p->elements->Get(i);
                for (uint j = 0; j < row->elements->Count();)
                    dest_row->elements->Move(row->elements->Get(0), dest_row->elements->Count());
            }
        }
    }

    elements->RemoveAt(left ? p : p + 1, 1);
    
    if (dest_row)
        dest_row->Normalize();
    
    changed_element = id;

#ifdef DEBUG
    to_str = ToText();
#endif
    return true;
}

bool Block::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    ElementPtr p;
    //find nearest paragraph
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr el = elements->Get(i);
        if (!el->visible)
            continue;
        if (y < el->GetAbsoluteRect().GetBottom() || el->rect.height == 0)
            break;
        p = el;
    }
    if (!p)
    {
        if (!parent)
            return false;
        return parent->GetTopCaretState(x, GetAbsoluteRect().top, caret_state, select);
    }
    return p->GetTopCaretState(x, y, caret_state, select);
}

bool Block::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    ElementPtr p;
    //find nearest paragraph
    for (int i = elements->Count() - 1; i >= 0; --i)
    {
        ElementPtr el = elements->Get(i);
        if (!el->visible)
            continue;
        if (y > el->GetAbsoluteRect().top || el->rect.height == 0)
            break;
        p = el;
    }
    if (!p)
    {
        if (!parent)
            return false;
        return parent->GetBottomCaretState(x, y, caret_state, select);
    }
    return p->GetBottomCaretState(x, y, caret_state, select);
}

void Block::AddEmptyElement()
{
    AddElement(ElementPtr(new Paragraph(this, true)));
}

std::u32string Block::ToText() const
{
    std::u32string t;
    for (int i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (!el->visible)
            continue;
        t += el->ToText();
        if (i < elements->Count() - 1)
            t += U"\n";
    }
    return t;
}

}
