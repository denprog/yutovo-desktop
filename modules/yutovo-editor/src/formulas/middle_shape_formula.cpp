/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "middle_shape_formula.h"
#include "code_row.h"
#include "code_string.h"
#include "../caret.h"
#include "../document.h"

namespace yutovo
{

//MiddleShapeFormula

MiddleShapeFormula::MiddleShapeFormula(Element* _parent, bool with_init) :
    Formula(_parent)
{
    if (with_init)
        Init();
}

MiddleShapeFormula::MiddleShapeFormula(Document* _document, bool with_init) :
    Formula(_document)
{
    if (with_init)
        Init();
}

MiddleShapeFormula::MiddleShapeFormula(const MiddleShapeFormula& source) :
    Formula(source)
{
    if (elements->Count() < 3)
        elements->Add(ElementPtr(new CodeRow<>(this, true)));
}

bool MiddleShapeFormula::AfterFromJson()
{
    if (elements->Count() != 3)
        return false;
    return true;
}

void MiddleShapeFormula::Init()
{
    elements->Add(ElementPtr(new CodeRow<>(this, true)));
    elements->Add(ElementPtr(new Shape(this)));
    elements->Add(ElementPtr(new CodeRow<>(this, true)));
}

bool MiddleShapeFormula::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    if (caret->GetPos() == 1 && caret->GetElement()->id == id)
        return false;
    for (auto el : _elements)
    {
        if (el->type == ElementType::CODE_PARAGRAPH && type != ElementType::ASSIGNMENT && type != ElementType::EQUATION)
            return false;
    }
    return Formula::InsertElements(_elements, insert_mode, with_undo, changed_element);
}

bool MiddleShapeFormula::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    uint start, size;
    if (selection->Has(id, start, size))
    {
        if (start == 0 && size == 1)
        {
            if (with_undo)
                document->StoreUndo(GetFirst()->id);
            GetFirst()->elements->Clear();
            Normalize();
            selection->Remove(id, start, size);
            changed_element = id;
            return true;
        }
        if (start == 2 && size == 1)
        {
            if (with_undo)
                document->StoreUndo(GetLast()->id);
            GetLast()->elements->Clear();
            Normalize();
            selection->Remove(id, start, size);
            changed_element = id;
            return true;
        }
        return false;
    }
    else if (!selection->IsEmpty() || left || caret->GetPos() != 1 || caret->GetElement() != this)
        return false;
    
    if (with_undo)
    {
        if (caret->GetPos() == 1)
            document->StoreUndo(parent->parent->id);
        else
            document->StoreUndo(parent->id);
    }

    //remove this element by deleting its shape
    GetFirst()->UpdateLevel(level);
    int p = parent->elements->GetElementPos(id);
    uint c1 = elements->Get(0)->elements->Count();

    uint c2 = 0;
    if (GetLast())
    {
        GetLast()->UpdateLevel(level);
        c2 = elements->Get(2)->elements->Count();
    }

    caret->SetState(id);
    parent->elements->Move(*elements->Get(0)->elements, p);
    if (GetLast())
        parent->elements->Move(*elements->Get(2)->elements, p + c1);
    CaretState c;
    if (parent->elements->Get(p + c1)->GetFirstCaretState(c, nullptr))
        caret->SetState(c);

    auto t = parent->elements->Get(p + c1 + c2); //for not removing this element until this function ends
    parent->elements->Remove(id);

    //move inner code rows here
    for (int i = 0; i < parent->elements->Count();)
    {
        auto el = (*parent->elements)[i];
        if (el->type != ElementType::CODE_ROW || el->elements->Count() == 0)
        {
            ++i;
            continue;
        }
        for (int j = 0, k = 0; j < el->elements->Count();)
            parent->elements->Move(el->elements->Get(0), i + 1 + k++);
        parent->elements->RemoveAt(i, 1);
    }
    
    parent->Normalize();
    changed_element = parent->id;
    return true;
}

bool MiddleShapeFormula::AfterInsert(bool with_undo)
{
    if (GetFirst()->elements->Count() != 1 || GetLast()->elements->Count() != 1 || !caret)
        return false;
    
    String* str1 = dynamic_cast<String*>(GetFirst()->elements->Get(0).get());
    String* str2 = dynamic_cast<String*>(GetLast()->elements->Get(0).get());
    if (str1 && str1->elements->Count() == 0)
    {
        int pos = parent->elements->GetElementPos(id);
        ElementPtr el1 = (pos == 0 ? nullptr : parent->elements->Get(pos - 1));
        ElementPtr el2 = (parent->elements->IsLast(id) ? nullptr : parent->elements->Get(pos + 1));

        if (!selection->IsEmpty())
        {
            ElementSelection& select = selection->selection[0];
            uint s = select.start;
            if (s > 0 && document->IsString(select.element))
            {
                select.element->can_merge = true;
                select.element->SplitAt(s);
            }

            //move the selected elements in the GetFirst() element
            GetFirst()->elements->RemoveAt(0, 1);
            for (int i = selection->selection.size() - 1; i >= 0; --i)
            {
                ElementSelection el_s = selection->selection[i];
                for (int j = el_s.size - 1; j >= 0; --j)
                {
                    auto el = document->GetElement(GetChild(el_s.element->id, el_s.start + j));
                    if (el->type == ElementType::CODE_ROW)
                    {
                        for (int k = 0; k < el->elements->Count();)
                            GetFirst()->elements->Move(el->elements->Get(0), GetFirst()->elements->Count());
                    }
                    else
                        GetFirst()->elements->Move(el, 0);
                }
            }
            parent->parent->Normalize(); //merge rows after split
        }
        else
        {
            if (el2 && dynamic_cast<String*>(el2.get()) && str2 && str2->elements->Count() == 0)
            {
                //move the second element in the GetLast() element
                GetLast()->elements->RemoveAt(0, 1);
                GetLast()->elements->Move(document->GetElement(el2->id), 0);
            }
            if (el1 && dynamic_cast<String*>(el1.get()))
            {
                String* str = dynamic_cast<String*>(el1.get());
                if (str->elements->Count() > 0)
                {
                    //move the first element in the upper element
                    GetFirst()->elements->RemoveAt(0, 1);
                    GetFirst()->elements->Move(document->GetElement(el1->id), 0);
                }
            }
            else if (el1 && el1->type == ElementType::CLOSE_ROUND_BRACKET)
            {
                //find open bracket
                int pos = parent->elements->GetElementPos(id);
                int open_pos = -1;
                int close_count = 1;
                for (int i = pos - 2; i >= 0; --i)
                {
                    auto el = parent->elements->Get(i);
                    if (el->type == ElementType::CLOSE_ROUND_BRACKET)
                        ++close_count;
                    else if (el->type == ElementType::OPEN_ROUND_BRACKET)
                    {
                        if (close_count == 1)
                        {
                            open_pos = i;
                            break;
                        }
                        else
                            --close_count;
                    }
                }

                if (open_pos != -1)
                {
                    for (int i = pos - 1; i >= open_pos; --i)
                    {
                        auto _el = parent->elements->Get(i);
                        GetFirst()->elements->Move(_el, 0);
                    }
                }
            }
        }

        selection->Clear();

        CaretState c;
        if (GetFirst()->elements->Get(0)->elements->Count() == 0)
        {
            if (GetFirst()->GetFirstCaretState(c, nullptr))
                caret->SetState(c);
        }
        else
        {
            if (GetLast()->GetFirstCaretState(c, nullptr))
                caret->SetState(c);
        }

        UpdateLevel(level);
        parent->Normalize();
        return true;
    }

    UpdateLevel(level);
    return false;
}

bool MiddleShapeFormula::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (select)
    {
        if (caret->IsOnElement(GetShape()->id))
            return false;

        CaretState c;
        if ((GetLast()->GetFirstCaretState(c, nullptr) && caret_state == c) || (GetFirst()->GetFirstCaretState(c, nullptr) && caret_state == c))
        {
            caret_state.SetState(id);
            select->Clear();
            select->Add(parent->id, parent->elements->GetChildPos(id), 1);
            return true;
        }
    }
    return Formula::GetLeftCaretState(caret_state, select);
}

bool MiddleShapeFormula::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    if (select)
    {
        if (caret->IsOnElement(GetShape()->id))
            return false;
        
        CaretState c;
        if ((GetLast()->GetLastCaretState(c, nullptr) && caret_state == c) || (GetFirst()->GetLastCaretState(c, nullptr) && caret_state == c))
        {
            caret_state.SetState(parent->id, parent->elements->GetChildPos(id) + 1);
            select->Clear();
            select->Add(parent->id, parent->elements->GetChildPos(id), 1);
            return true;
        }
    }
    return Formula::GetRightCaretState(caret_state, select);
}

bool MiddleShapeFormula::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (select)
    {
        if (caret->IsOnElement(GetShape()->id))
            return false;

        CaretState c;
        if ((GetLast()->GetFirstCaretState(c, nullptr) && caret_state == c) || (GetFirst()->GetFirstCaretState(c, nullptr) && caret_state == c))
        {
            caret_state.SetState(id);
            select->Clear();
            select->Add(parent->id, parent->elements->GetChildPos(id), 1);
            return true;
        }
    }
    return Formula::GetTopCaretState(x, y, caret_state, select);
}

bool MiddleShapeFormula::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (select)
    {
        if (caret->IsOnElement(GetShape()->id))
            return false;
        
        CaretState c;
        if ((GetLast()->GetLastCaretState(c, nullptr) && caret_state == c) || (GetFirst()->GetLastCaretState(c, nullptr) && caret_state == c))
        {
            caret_state.SetState(parent->id, parent->elements->GetChildPos(id) + 1);
            select->Clear();
            select->Add(parent->id, parent->elements->GetChildPos(id), 1);
            return true;
        }
    }
    return Formula::GetBottomCaretState(x, y, caret_state, select);
}

bool MiddleShapeFormula::GetWordLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (select && caret->IsOnElement(GetShape()->id))
        return false;
    return Formula::GetWordLeftCaretState(caret_state, select);
}

bool MiddleShapeFormula::GetWordRightCaretState(CaretState& caret_state, Selection* select)
{
    if (select && caret->IsOnElement(GetShape()->id))
        return false;
    return Formula::GetWordRightCaretState(caret_state, select);
}

bool MiddleShapeFormula::GetBeginCaretState(CaretState& caret_state, Selection* select)
{
    if (select && caret_state.IsInsideElement(id))
    {
        caret_state.SetState(parent->id, parent->elements->GetChildPos(id));
        select->Add(parent->id, parent->elements->GetChildPos(id), 1);
        return true;
    }
    return Formula::GetBeginCaretState(caret_state, select);
}

bool MiddleShapeFormula::GetEndCaretState(CaretState& caret_state, Selection* select)
{
    if (select && caret_state.IsInsideElement(id))
    {
        caret_state.SetState(parent->id, parent->elements->GetChildPos(id) + 1);
        select->Add(parent->id, parent->elements->GetChildPos(id), 1);
        return true;
    }
    return Formula::GetEndCaretState(caret_state, select);
}

CodeRow<>* MiddleShapeFormula::GetFirst() const
{
    return (CodeRow<>*)elements->Get(0).get();
}

Shape* MiddleShapeFormula::GetShape() const
{
    return (Shape*)elements->Get(1).get();
}

CodeRow<>* MiddleShapeFormula::GetLast() const
{
    return (CodeRow<>*)elements->Get(2).get();
}

}
