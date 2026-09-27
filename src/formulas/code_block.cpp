/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "code_block.h"
#include "code_paragraph.h"
#include "formula.h"

namespace yutovo
{

//CodeBlock

CodeBlock::CodeBlock(Document* _document, uint _code_id, bool add_empty, bool list_identifiers) :
    Block(_document),
    code_id(_code_id)
{
    type = ElementType::CODE_BLOCK;
    code_format = document->code_formats->GetFormat("Calculator");
    paragraph_format = document->paragraph_formats->GetFormat("Code", document->config.language);
    formula_format = document->formula_formats->GetFormat("Formula");
    if (add_empty)
        AddEmptyElement(); //code block has to have at least one code paragraph
    
    document->SetLocale(document->config.language, false);
    if (list_identifiers)
        document->ListIdentifiers(code_id);
}

CodeBlock::CodeBlock(Element* parent, uint _code_id, bool add_empty, bool list_identifiers) :
    Block(parent),
    code_id(_code_id)
{
    type = ElementType::CODE_BLOCK;
    code_format = document->code_formats->GetFormat("Calculator");
    paragraph_format = document->paragraph_formats->GetFormat("Code", document->config.language);
    formula_format = document->formula_formats->GetFormat("Formula");
    if (add_empty)
        AddEmptyElement(); //code block has to have at least one code paragraph

    document->SetLocale(document->config.language, false);
    if (list_identifiers)
        document->ListIdentifiers(code_id);
}

CodeBlock::CodeBlock(Document* _document, Element* parent, uint _code_id, bool add_empty) :
    Block(parent),
    code_id(_code_id)
{
    type = ElementType::CODE_BLOCK;
    document = _document;
    window = document->window;
    caret = document->caret;
    selection = &document->selection;
    code_format = document->code_formats->GetFormat("Calculator");
    paragraph_format = document->paragraph_formats->GetFormat("Code", document->config.language);
    formula_format = document->formula_formats->GetFormat("Formula");
    if (add_empty)
        AddEmptyElement(); //code block has to have at least one code paragraph
    
    document->SetLocale(document->config.language, false);
    document->ListIdentifiers(code_id);
}

Element* CodeBlock::Clone()
{
    return new CodeBlock(*this);
}

Element* CodeBlock::Create(Element* parent)
{
    return new CodeBlock(parent, parent->document->cur_code_id, true, true);
}

bool CodeBlock::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    if (caret->IsOnElement(id) && document->pasting)
    {
        std::vector<ElementPtr> _els;
        for (auto& el : _elements)
        {
            if (document->IsRow(el))
            {
                ElementPtr p(new Paragraph(parent, false));
                p->elements->Add(el);
                _els.push_back(p);
            }
            else
                _els.push_back(el);
        }
        return parent->InsertElements(_els, insert_mode, with_undo, changed_element);
    }
    return Block::InsertElements(_elements, insert_mode, with_undo, changed_element);
}

void CodeBlock::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Block::ToJson(value, alloc);
    value.AddMember("code_id", code_id, alloc);
}

Element* CodeBlock::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.HasMember("code_id") || !value["code_id"].IsInt())
        return nullptr;
    auto code_id = value["code_id"].GetInt();
    if (parent)
        return new CodeBlock(parent, code_id, false, false);
    return new CodeBlock(document, code_id, false, false);
}

void CodeBlock::Draw() const
{
    window->DrawFillRect(GetAbsoluteRect(), document->config.formula_bg_color);
    if (document->config.code_block_border)
        window->DrawRect(GetAbsoluteRect(), code_format->border_color);
    Element::Draw();
}

bool CodeBlock::Remake(bool with_elements)
{
    bool changed = Element::Remake(with_elements);

    int left_m = 0, top_m = 0, right_m = 0, bottom_m = 0;
    int h = code_format->top_indent;
    for (int i = 0; i < elements->Count(); ++i) //arrange paragraphs
    {
        ElementPtr p = elements->Get(i);
        p->GetMargin(left_m, top_m, right_m, bottom_m); //consider the margins
        p->rect.Move(left_m + code_format->left_indent, h + top_m);
        h = p->rect.GetBottom() + bottom_m;
    }

    Element::UpdateRect(false);

    UpdateRect();
    rect.width += code_format->right_indent;
    rect.height += code_format->bottom_indent;

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

void CodeBlock::UpdateDrawRect()
{
    Block::UpdateDrawRect();
    draw_rect.width += 2;
    draw_rect.height += 2;
}

Color CodeBlock::GetBackgroundColor() const
{
    return document->config.formula_bg_color;
}

bool CodeBlock::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (select && select->IsSelected(id))
        caret->SetState(id);
    return Block::GetTopCaretState(x, y, caret_state, select);
}

bool CodeBlock::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (!IsVisible())
        return false;
    if (caret_state.IsInsideElement(id))
    {
        if (elements->GetLeftCaretState(caret_state, select))
            return true;
        if (select)
        {
            ElementSelection s;
            if (!selection->Has(id, s) || s.size != elements->Count())
                select->Add(id);
            caret_state.SetState(parent->id, parent->elements->GetElementPos(id));
            return true;
        }
    }
    if (parent)
        return parent->GetLeftCaretState(caret_state, select);
    return false;
}

bool CodeBlock::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    if (!IsVisible())
        return false;
    if (caret_state.IsInsideElement(id))
    {
        if (elements->GetRightCaretState(caret_state, select))
            return true;
        if (select)
        {
            ElementSelection s;
            if (!selection->Has(id, s) || s.size != elements->Count())
                select->Add(id);
            caret_state.SetState(parent->id, parent->elements->GetElementPos(id) + 1, true);
            return true;
        }
    }
    if (parent)
        return parent->GetRightCaretState(caret_state, select);
    return false;
}

bool CodeBlock::GetWordLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (caret_state.IsInsideElement(id))
    {
        if (caret_state.GetPos() == 0)
        {
            if (select)
                select->Add(id);
            caret_state.SetState(parent->id, parent->elements->GetElementPos(id));
            return true;
        }
    }
    return Block::GetWordLeftCaretState(caret_state, select);
}

bool CodeBlock::AfterInsert(bool with_undo)
{
    auto p = document->FindParent(parent->id, ElementType::CODE_BLOCK);
    if (p)
    {
        //move the child elements outside and remove this code block
        int c = p->elements->Count();
        for (int i = 0, j = 0; i < elements->Count();)
            p->elements->Move(elements->Get(0), c + j++);
        p->elements->RemoveAt(c - 1, 1);
        return true;
    }

    CaretState c;
    if (GetFirstCaretState(c, nullptr))
        caret->SetState(c);
    return true;
}

void CodeBlock::GetMargin(int& left, int& top, int& right, int& bottom) const
{
    left = std::round(code_format->left_margin * document->config.scale);
    top = std::round(code_format->top_margin * document->config.scale);
    right = std::round(code_format->right_margin * document->config.scale);
    bottom = std::round(code_format->bottom_margin * document->config.scale);
}

bool CodeBlock::HasCaretState()
{
    return true;
}

bool CodeBlock::HasLastCaretState()
{
    return true;
}

bool CodeBlock::CanContinueVerticalMoving()
{
    return true;
}

StringFormatPtr CodeBlock::GetStringFormat() const
{
    return formula_format->string_format;
}

FormulaFormatPtr CodeBlock::GetFormulaFormat() const
{
    return formula_format;
}

ParagraphFormatPtr CodeBlock::GetParagraphFormat()
{
    return paragraph_format;
}

std::string CodeBlock::ToHtml() const
{
    if (elements->Count() <= 1)
        return Block::ToHtml();

    std::string html = "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">";
    for (int i = 0; i < elements->Count(); ++i)
    {
        const auto& el = elements->Get(i);
        if (!el->IsVisible())
            continue;
        html += el->ToHtml();
        if (i < elements->Count() - 1)
            html += "<br>";
    }
    html += "</span>";
    return html;
}

void CodeBlock::AddEmptyElement()
{
    AddElement(ElementPtr(new CodeParagraph<>(this, true)));
}

bool CodeBlock::IsFormula()
{
    return true;
}

}
