/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "assignment.h"
#include "code_block.h"
#ifdef min
#undef min
#endif
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace yutovo
{

//Assignment

Assignment::Assignment(Element* _parent, bool with_init, bool _auto_solve) :
    MiddleShapeFormula(_parent, with_init),
    auto_solve(_auto_solve)
{
    type = ElementType::ASSIGNMENT;
    guid = boost::uuids::to_string(boost::uuids::random_generator()());
}

Assignment::Assignment(Document* _document, bool with_init) :
    MiddleShapeFormula(_document, with_init)
{
    type = ElementType::ASSIGNMENT;
    guid = boost::uuids::to_string(boost::uuids::random_generator()());
}

Assignment::Assignment(const Assignment& source) :
    MiddleShapeFormula(source),
    auto_solve(source.auto_solve),
    dependencies(source.dependencies),
    guid(source.guid)
{
    if (document->moving_element)
        last_expression = source.last_expression;
}

Element* Assignment::Clone()
{
    return new Assignment(*this);
}

Element* Assignment::Create(Element* _parent)
{
    return new Assignment(_parent);
}

void Assignment::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    MiddleShapeFormula::ToJson(value, alloc);
    value.AddMember("auto_solve", auto_solve, alloc);
}

Element* Assignment::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    bool _auto_solve = true;
    if (value.HasMember("auto_solve") && value["auto_solve"].IsBool())
        _auto_solve = value["auto_solve"].GetBool();
    if (parent)
        return new Assignment(parent, false, _auto_solve);
    return new Assignment(document, false);
}

void Assignment::Draw() const
{
    GetShape()->draw_func = 
        [&](const Rect& r)
        {
            if (document->selection.IsSelected(id))
                window->DrawText(draw_sign, draw_string_format, r, document->config.formula_bg_color, document->config.bg_selection_color, false);
            else
                window->DrawText(draw_sign, draw_string_format, r, document->config.shapes_color, document->config.formula_bg_color, false);
        };

    MiddleShapeFormula::Draw();

    //if there are two or more formulas in a row, draw a frame for better view
    if (has_frame_hilight)
    {
        Rect r = GetAbsoluteRect();
        window->DrawRect(r.left, r.top, r.width, r.height, document->config.formula_frame_color);
    }
}

void Assignment::UpdateRect(bool with_elements)
{
    if (!draw_string_format)
        Rescale();
    Size s = parent->window->GetTextSize(ToUtfString(draw_sign), draw_string_format);
    GetShape()->rect.SetSize(s.width, s.height);
    GetShape()->baseline = GetShape()->rect.height / 3 * 2;

    MiddleShapeFormula::UpdateRect(false);
}

bool Assignment::Remake(bool with_elements)
{
    bool changed = MiddleShapeFormula::Remake(with_elements);

    baseline = 0;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (el->baseline > baseline)
            baseline = el->baseline;
    }

    GetFirst()->rect.Move(0, baseline - GetFirst()->baseline);
    GetShape()->rect.Move(GetFirst()->rect.width, baseline - GetShape()->baseline);
    GetLast()->rect.Move(GetFirst()->rect.width + GetShape()->rect.width, baseline - GetLast()->baseline);

    UpdateRect();

    has_frame_hilight = (parent->type == ElementType::CODE_ROW && parent->parent->type == ElementType::CODE_PARAGRAPH && parent->elements->Count() > 1);

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }

    return changed;
}

bool Assignment::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    if (parent && parent->type == ElementType::CODE_ROW_ASSIGNMENT)
        return false;

    if (auto_solve && caret->GetPos() == 1 && last_identifier != U"")
    {
        auto code = document->FindParent(id, ElementType::CODE_BLOCK);
        if (!code)
            return false;
        document->RemoveIdentifier(logical_id, ((CodeBlock*)code.get())->code_id, last_identifier, document->config.solve_delay);
    }

    return MiddleShapeFormula::DeleteElements(left, with_undo, changed_element);
}

void Assignment::LogicalIdChanged(const LogicalId& last_id)
{
    MiddleShapeFormula::LogicalIdChanged(last_id);
    if (last_id.empty() || last_identifier.empty())
        return;
    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    if (!code)
        return;

    //update the ids in the expression
    ParserString expr;
    GetFirst()->ToParserString(expr);
    expr.Add(id, solve_sign);
    GetLast()->ToParserString(expr);
    last_expression = expr;
            
    //move the identifier in the solver
    document->RemoveIdentifier(last_id, ((CodeBlock*)code.get())->code_id, last_identifier, document->config.solve_delay);
    document->SetIdentifier(logical_id, guid, ((CodeBlock*)code.get())->code_id, document->config.auto_result, !GetParent(1)->visible, 
        GetFirst()->ToText(), last_expression.Text(), document->config.solve_delay);
}

bool Assignment::AfterInsert(bool with_undo)
{
    int pos = parent->elements->GetElementPos(id);
    if (pos > 0)
        GetFirst()->elements->Clear();
    for (int i = pos - 1; i >= 0; --i)
    {
        auto el = parent->elements->Get(i);
        if (el->type == ElementType::ASSIGNMENT || el->type == ElementType::EQUATION)
            break;
        GetFirst()->elements->Move(el, 0);
    }

    CaretState c;
    GetLast()->GetFirstCaretState(c, nullptr);
    caret->SetState(c);
    last_expression.Reset();
    return true;
}

void Assignment::BeforeDelete()
{
    if (!id.empty() && auto_solve && !document->moving_element)
    {
        auto code = document->FindParent(id, ElementType::CODE_BLOCK);
        if (code)
            document->RemoveIdentifier(logical_id, ((CodeBlock*)code.get())->code_id, last_identifier, document->config.solve_delay);
    }
}

void Assignment::Solve()
{
    if (!auto_solve || (parent && parent->type == ElementType::CODE_ROW_ASSIGNMENT))
        return;
    
    MiddleShapeFormula::Solve();

    ParserString str;
    GetFirst()->ToParserString(str);
    str.Add(id, solve_sign);
    GetLast()->ToParserString(str);
    if (last_expression != str)
        document->AddResolveElement(logical_id);
}

void Assignment::ReSolve(bool if_error, bool force)
{
    if (!auto_solve || (parent && parent->type == ElementType::CODE_ROW_ASSIGNMENT))
        return;
    
    document->RemoveErrorMarks(id);
    if (if_error && !last_error)
        return;
    last_expression.Reset();

    ParserString expr;
    GetFirst()->ToParserString(expr);
    expr.Add(id, solve_sign);
    GetLast()->ToParserString(expr);
    if (last_expression != expr)
    {
        auto code = document->FindParent(id, ElementType::CODE_BLOCK);
        if (!code)
            return;
        if (last_identifier != U"")
            document->RemoveIdentifier(logical_id, ((CodeBlock*)code.get())->code_id, last_identifier, document->config.solve_delay);
        document->SetIdentifier(logical_id, guid, ((CodeBlock*)code.get())->code_id, document->config.auto_result, !GetParent(1)->visible, 
            GetFirst()->ToText(), expr.Text(), document->config.solve_delay);
        last_identifier = GetFirst()->ToText();
        last_expression = expr;
    }
}

void Assignment::PutResult(Result& result)
{
    last_error = result.error.error_code != ErrorCode::OK;
    dependencies = result.dependencies;
    Remake(true);
}

bool Assignment::Depends(const std::string& identifier)
{
    if (std::find(dependencies.begin(), dependencies.end(), identifier) != dependencies.end())
        return true;
    return false;
}

std::string Assignment::ToHtml() const
{
    std::string s = GetFirst()->ToHtml();
    s += "<mo>" + ToBasicString(solve_sign) + "</mo>";
    if (GetLast())
        s += GetLast()->ToHtml();
    return s;
}

std::u32string Assignment::ToText() const
{
    std::u32string s;
    if (elements->Count() > 0)
        s = elements->Get(0)->ToText();
    s += solve_sign;
    if (elements->Count() == 3)
        s += elements->Get(2)->ToText();
    return s;
}

void Assignment::ToParserString(ParserString& str)
{
    if (elements->Count() > 0)
        elements->Get(0)->ToParserString(str);
    str.Add(id, solve_sign);
    if (elements->Count() == 3)
        elements->Get(2)->ToParserString(str);
}

}
