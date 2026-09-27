/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "code_paragraphs_block.h"
#include "code_paragraph.h"
#include "formula.h"
#include "graph.h"
#include "evaluation_bar.h"
#include "assignment.h"

namespace yutovo
{

//CodeParagraphsBlock

template<typename T>
CodeParagraphsBlock<T>::CodeParagraphsBlock(Document* _document, bool add_empty) :
    Block(_document)
{
    type = ElementType::CODE_PARAGRAPHS_BLOCK;
    paragraph_format = document->paragraph_formats->GetFormat("Code", document->config.language);
    formula_format = document->formula_formats->GetFormat("Formula");
    if (add_empty)
        AddEmptyElement();
}

template<typename T>
CodeParagraphsBlock<T>::CodeParagraphsBlock(Element* parent, bool add_empty) :
    Block(parent)
{
    type = ElementType::CODE_PARAGRAPHS_BLOCK;
    paragraph_format = document->paragraph_formats->GetFormat("Code", document->config.language);
    formula_format = document->formula_formats->GetFormat("Formula");
    if (add_empty)
        AddEmptyElement();
}

template<typename T>
Element* CodeParagraphsBlock<T>::Clone()
{
    return new CodeParagraphsBlock<T>(*this);
}

template<typename T>
Element* CodeParagraphsBlock<T>::Create(Element* parent)
{
    return new CodeParagraphsBlock<T>(parent, true);
}

template<typename T>
void CodeParagraphsBlock<T>::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Block::ToJson(value, alloc);
}

template<typename T>
Element* CodeParagraphsBlock<T>::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
    rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new CodeParagraphsBlock<T>(parent, false);
    return new CodeParagraphsBlock<T>(document, false);
}

template<typename T>
void CodeParagraphsBlock<T>::Draw() const
{
    Element::Draw();
    window->DrawRect(GetAbsoluteRect(), document->config.bg_selection_color);
}

template<typename T>
bool CodeParagraphsBlock<T>::Remake(bool with_elements)
{
    bool changed = Element::Remake(with_elements);

    int left_m = 0, top_m = 0, right_m = 0, bottom_m = 0;
    int h = formula_format->top_margin;
    for (int i = 0; i < elements->Count(); ++i) //arrange paragraphs
    {
        ElementPtr p = elements->Get(i);
        p->GetMargin(left_m, top_m, right_m, bottom_m); //consider the margins
        p->rect.Move(left_m + formula_format->left_margin, h + top_m);
        h = p->rect.GetBottom() + bottom_m;
    }

    Element::UpdateRect(false);

    UpdateRect();
    rect.width += formula_format->right_margin;
    rect.height += formula_format->bottom_margin;

    //align the baseline
    baseline = 0;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (el->baseline > baseline)
            baseline = el->baseline;
    }

    UpdateDrawRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

template<typename T>
ParagraphFormatPtr CodeParagraphsBlock<T>::GetParagraphFormat()
{
    return paragraph_format;
}

template<typename T>
std::string CodeParagraphsBlock<T>::ToHtml() const
{
    if (elements->Count() <= 1)
        return Block::ToHtml();

    std::string html;
    for (int i = 0; i < elements->Count(); ++i)
    {
        const auto& el = elements->Get(i);
        if (!el->IsVisible())
            continue;
        html += "<p>" + el->ToHtml() + "</p>";
    }
    return html;
}

template<typename T>
void CodeParagraphsBlock<T>::AddEmptyElement()
{
    auto* p = new CodeParagraph<>(this, true);

    GraphLine* graph = dynamic_cast<GraphLine*>(parent);
    if (graph)
    {
        StringFormatPtr f = GetStringFormat();
        Color color;
        uint width = 1;
        graph->GetPlotFormat(elements->Count(), color, width);
        p->SetMarker(U"█", document->GetStringFormat(f->family, f->size, f->bold, f->italic, f->underline, f->strikethrough,
            f->subscript, f->superscript, color, f->text_bg_color, f->text_bg_selection_color));
    }
    AddElement(ElementPtr(p));
}

template<typename T>
bool CodeParagraphsBlock<T>::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    return Block::InsertElements(_elements, insert_mode, with_undo, changed_element);
}

template<typename T>
bool CodeParagraphsBlock<T>::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    return Block::DeleteElements(left, with_undo, changed_element);
}

template<typename T>
bool CodeParagraphsBlock<T>::IsFormula()
{
    return true;
}

template<typename T>
bool CodeParagraphsBlock<T>::IsEmpty() const
{
    return Block::IsEmpty();
}

//Explicit specializations for Assignment

template<>
void CodeParagraphsBlock<Assignment>::AddEmptyElement();
template<>
bool CodeParagraphsBlock<Assignment>::IsEmpty() const;

template<>
CodeParagraphsBlock<Assignment>::CodeParagraphsBlock(Document* _document, bool add_empty) :
    Block(_document)
{
    type = ElementType::CODE_PARAGRAPHS_BLOCK_ASSIGNMENT;
    paragraph_format = document->paragraph_formats->GetFormat("Code", document->config.language);
    formula_format = document->formula_formats->GetFormat("Formula");
    if (add_empty)
        AddEmptyElement();
}

template<>
CodeParagraphsBlock<Assignment>::CodeParagraphsBlock(Element* parent, bool add_empty) :
    Block(parent)
{
    type = ElementType::CODE_PARAGRAPHS_BLOCK_ASSIGNMENT;
    paragraph_format = document->paragraph_formats->GetFormat("Code", document->config.language);
    formula_format = document->formula_formats->GetFormat("Formula");
    if (add_empty)
        AddEmptyElement();
}

template<>
void CodeParagraphsBlock<Assignment>::Draw() const
{
    Element::Draw();
}

template<>
bool CodeParagraphsBlock<Assignment>::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    if (_elements.size() != 1 || !document->IsParagraph(_elements[0]))
        return false;

    int pos = elements->Count();
    auto cur = document->FindParentParagraph(caret->GetCaretState().id);
    if (cur && cur->type == ElementType::CODE_PARAGRAPH_ASSIGNMENT)
        pos = elements->GetElementPos(cur->id) + 1;

    if (with_undo)
        document->StoreUndo(parent->id);

    ElementPtr p(new CodeParagraph<Assignment>(this, true));
    elements->Insert(p, pos);

    CaretState c;
    if (p->GetFirstCaretState(c, nullptr))
        caret->SetState(c);

    changed_element = id;
    return true;
}

template<>
bool CodeParagraphsBlock<Assignment>::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    if (IsEmpty())
        return false;

    uint start, size;
    if (selection->Has(id, start, size))
    {
        if (Element::DeleteElements(left, with_undo, changed_element))
        {
            Normalize();
            changed_element = id;
            return true;
        }
    }

    auto paragraph = document->FindParentParagraph(caret->GetCaretState().id);
    if (!paragraph || paragraph->type != ElementType::CODE_PARAGRAPH_ASSIGNMENT)
        return false;

    int pos = elements->GetElementPos(paragraph->id);
    if (with_undo)
        document->StoreUndo(parent->id);

    elements->RemoveAt(pos, 1);

    CaretState c;
    if (elements->Count() == 0)
        AddEmptyElement();

    if (pos == 0)
    {
        if (GetFirstCaretState(c, nullptr))
            caret->SetState(c);
    }
    else if (pos < elements->Count() - 1)
        caret->SetState(id, pos, true);
    else
        caret->SetState(id, elements->Count() - 1, true);

    changed_element = id;
    return true;
}

template<>
void CodeParagraphsBlock<Assignment>::AddEmptyElement()
{
    AddElement(ElementPtr(new CodeParagraph<Assignment>(this, true)));
}

template<>
bool CodeParagraphsBlock<Assignment>::IsEmpty() const
{
    if (elements->Count() != 1)
        return false;
    return elements->Get(0)->IsEmpty();
}

template class CodeParagraphsBlock<void>;
template class CodeParagraphsBlock<Assignment>;

}
