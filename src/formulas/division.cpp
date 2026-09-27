/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "division.h"
#include "code_row.h"
#include <algorithm>
#include "code_string.h"
#include "power.h"
#include "shape.h"
#include "parser_string.h"
#include "evaluation_bar.h"

namespace yutovo
{

//Division

Division::Division(Element* _parent, bool with_init) : 
    MiddleShapeFormula(_parent, with_init)
{
    type = ElementType::DIVISION;
}

Division::Division(Document* _document, bool with_init) :
    MiddleShapeFormula(_document, with_init)
{
    type = ElementType::DIVISION;
}

Division::Division(const Division& source) :
    MiddleShapeFormula(source)
{
}

Element* Division::Clone()
{
    return new Division(*this);
}

Element* Division::Create(Element* parent)
{
    return new Division(parent);
}

Element* Division::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new Division(parent, false);
    return new Division(document, false);
}

void Division::Draw() const
{
    GetShape()->draw_func = 
        [&](const Rect& r)
        {
            Color c = document->selection.IsSelected(id) ? document->config.formula_bg_color : document->config.shapes_color;
            if (r.height == 0)
                window->DrawLine(r.left, r.top, r.left + r.width, r.top, c);
            else
                window->DrawFillRect(r.left, r.top, r.width, r.height, c);
        };

    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, document->config.bg_selection_color);
    }

    MiddleShapeFormula::Draw();
}

bool Division::Remake(bool with_elements)
{
    bool changed = MiddleShapeFormula::Remake(with_elements);
    
    int w = std::max(GetFirst()->rect.width + 2, GetLast()->rect.width + 2);
    if (w < 200)
        GetShape()->rect.SetRect(0, 0, w, 0);
    else
        GetShape()->rect.SetRect(0, 0, w, w / 200 > 2 ? 2 : w / 200);
    GetFirst()->rect.Move((w - GetFirst()->rect.width) / 2, 0);
    GetShape()->rect.Move(0, GetFirst()->rect.height + GetShape()->rect.height + 4);
    GetLast()->rect.Move((w - GetLast()->rect.width) / 2, GetFirst()->rect.height + GetShape()->rect.height + (GetShape()->rect.height + 4) * 2);
    baseline = GetShape()->rect.GetBottom() - GetShape()->rect.height / 2;

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

bool Division::AfterInsert(bool with_undo)
{
    if (MiddleShapeFormula::AfterInsert(with_undo))
        return true;

    //if the division looks like a derivative, place the caret in the function placeholder
    int order = 0;
    uint func_start = 0;
    if (!GetDerivativeOrderAt(GetFirst(), 0, order, func_start))
        return false;

    int den_order = 0;
    uint var_start = 0;
    if (!GetDerivativeOrderAt(GetLast(), 0, den_order, var_start))
        return false;

    if (func_start < GetFirst()->elements->Count())
    {
        CaretState c;
        if (GetFirst()->elements->Get(func_start)->GetFirstCaretState(c, nullptr))
        {
            caret->SetState(c);
            return true;
        }
    }

    return false;
}

bool Division::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (select)
        return MiddleShapeFormula::GetTopCaretState(x, y, caret_state, select);
    if (GetLast()->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(GetLast()->id))
        return GetLast()->GetTopCaretState(x, y, caret_state, select);
    if (GetShape()->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(GetShape()->id))
        return GetShape()->GetTopCaretState(x, y, caret_state, select);
    if (GetFirst()->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(GetFirst()->id))
        return GetFirst()->GetTopCaretState(x, y, caret_state, select);
    return parent->GetTopCaretState(x, y, caret_state, select);
}

bool Division::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (select)
        return MiddleShapeFormula::GetBottomCaretState(x, y, caret_state, select);
    if (GetFirst()->GetAbsoluteRect().top >= y)
        return GetFirst()->GetBottomCaretState(x, y, caret_state, select);
    if (GetShape()->GetAbsoluteRect().top >= y)
        return GetShape()->GetBottomCaretState(x, y, caret_state, select);
    if (GetLast()->GetAbsoluteRect().top >= y)
        return GetLast()->GetBottomCaretState(x, y, caret_state, select);
    return parent->GetBottomCaretState(x, y, caret_state, select);
}

bool Division::UseSelection()
{
    return true;
}

std::string Division::ToHtml() const
{
    std::string s = "<mfrac>";
    s += GetFirst()->ToHtml();
    s += GetLast()->ToHtml();
    s += "</mfrac>";
    return s;
}

std::u32string Division::ToText() const
{
    if (!GetFirst() || !GetLast())
        return U"";

    ParserString ps;
    if (const_cast<Division*>(this)->BuildDerivativeAtPointParserString(ps, false))
        return ps.Text();
    if (const_cast<Division*>(this)->BuildDerivativeParserString(ps))
        return ps.Text();

    return U"(" + GetFirst()->ToText() + U")/(" + GetLast()->ToText() + U")";
}

void Division::ToParserString(ParserString& str)
{
    if (BuildDerivativeAtPointParserString(str))
        return;
    if (BuildDerivativeParserString(str))
        return;

    str.Add(id, U"(");
    GetFirst()->ToParserString(str);
    if (GetFirst()->elements->Count() == 1 && document->IsString(GetFirst()->elements->Get(0)->id) && 
        GetLast()->elements->Count() == 1 && document->IsString(GetLast()->elements->Get(0)->id) && 
        GetFirst()->elements->Get(0)->ToText().find_first_not_of(U"0123456789.") == std::string::npos && 
        GetLast()->elements->Get(0)->ToText().find_first_not_of(U"0123456789.") == std::string::npos)
    {
        str.Add(id, U"/");
    }
    else
    {
        str.Add(id, U")/(");
    }
    GetLast()->ToParserString(str);
    str.Add(id, U")");
}

bool Division::IsDerivativeSymbol(const std::u32string& s) const
{
    return s == U"d" || s == U"∂";
}

bool Division::GetDerivativeOrderAt(CodeRow<>* row, uint pos, int& order, uint& content_start)
{
    if (!row || pos >= row->elements->Count())
        return false;

    Element* el = row->elements->Get(pos).get();

    String* str = dynamic_cast<String*>(el);
    if (str)
    {
        if (IsDerivativeSymbol(str->ToText()))
        {
            order = 1;
            content_start = pos + 1;
            return true;
        }
        return false;
    }

    Power* p = dynamic_cast<Power*>(el);
    if (p)
    {
        CodeRow<>* base = p->GetBaseRow();
        CodeRow<>* exp = p->GetExponentRow();
        if (!base || !exp || base->elements->Count() == 0 || exp->elements->Count() == 0)
            return false;

        String* base_str = dynamic_cast<String*>(base->elements->Get(0).get());
        if (!base_str)
            return false;

        std::u32string base_text = base_str->ToText();
        if (base_text.empty() || !IsDerivativeSymbol(base_text.substr(0, 1)))
            return false;

        String* exp_str = dynamic_cast<String*>(exp->elements->Get(0).get());
        if (!exp_str)
            return false;

        std::u32string exp_text = exp_str->ToText();
        if (exp_text.find_first_not_of(U"0123456789") != std::string::npos)
            return false;

        try
        {
            order = std::stoi(ToBasicString(exp_text));
        }
        catch (...)
        {
            return false;
        }

        if (order <= 0)
            return false;

        content_start = pos + 1;
        return true;
    }

    return false;
}

bool Division::ParseDerivativeMarker(Element* el, DiffMarker& marker) const
{
    String* str = dynamic_cast<String*>(el);
    if (str)
    {
        std::u32string text = str->ToText();
        if (!text.empty() && IsDerivativeSymbol(text.substr(0, 1)))
        {
            marker.order = 1;
            marker.remaining = text.substr(1);
            return true;
        }
        return false;
    }

    Power* p = dynamic_cast<Power*>(el);
    if (!p)
        return false;

    CodeRow<>* base = p->GetBaseRow();
    CodeRow<>* exp = p->GetExponentRow();
    if (!base || !exp || base->elements->Count() == 0 || exp->elements->Count() == 0)
        return false;

    String* base_str = dynamic_cast<String*>(base->elements->Get(0).get());
    if (!base_str)
        return false;

    std::u32string base_text = base_str->ToText();
    if (base_text.empty() || !IsDerivativeSymbol(base_text.substr(0, 1)))
        return false;

    String* exp_str = dynamic_cast<String*>(exp->elements->Get(0).get());
    if (!exp_str)
        return false;

    std::u32string exp_text = exp_str->ToText();
    if (exp_text.find_first_not_of(U"0123456789") != std::string::npos)
        return false;

    try
    {
        marker.order = std::stoi(ToBasicString(exp_text));
    }
    catch (...)
    {
        return false;
    }

    if (marker.order <= 0)
        return false;

    marker.remaining = base_text.substr(1);
    return true;
}

bool Division::ParseDerivativeFraction(std::u32string& function_text, std::vector<std::pair<std::u32string, int>>& vars, int& num_order)
{
    CodeRow<>* num = GetFirst();
    CodeRow<>* den = GetLast();
    if (!num || !den)
        return false;

    //parse the numerator: leading d/∂ marker followed by the function expression
    num_order = 0;
    function_text.clear();
    vars.clear();
    bool num_marker_found = false;
    for (uint i = 0; i < num->elements->Count(); ++i)
    {
        Element* el = num->elements->Get(i).get();
        DiffMarker marker;
        if (!num_marker_found && ParseDerivativeMarker(el, marker))
        {
            num_order = marker.order;
            num_marker_found = true;
            function_text += marker.remaining;
        }
        else if (num_marker_found)
        {
            ParserString ps;
            el->ToParserString(ps);
            function_text += ps.Text();
        }
        else
        {
            //content before the derivative marker means this is an ordinary fraction
            return false;
        }
    }

    if (!num_marker_found)
        return false;

    //parse the denominator: a sequence of d/∂ markers each followed by a variable
    std::u32string cur_var;
    int cur_order = 0;
    bool den_marker_found = false;

    auto process_string =
        [&](const std::u32string& text) -> bool
        {
            for (size_t k = 0; k < text.size();)
            {
                std::u32string ch = text.substr(k, 1);
                if (IsDerivativeSymbol(ch))
                {
                    if (cur_order > 0)
                        vars.emplace_back(cur_var, cur_order);

                    cur_order = 1;
                    cur_var.clear();
                    den_marker_found = true;
                    ++k;
                }
                else if (den_marker_found)
                {
                    cur_var += ch;
                    ++k;
                }
                else
                {
                    //content before the first derivative marker
                    return false;
                }
            }
            return true;
        };

    for (uint i = 0; i < den->elements->Count(); ++i)
    {
        Element* el = den->elements->Get(i).get();
        String* s = dynamic_cast<String*>(el);
        if (s)
        {
            if (!process_string(s->ToText()))
                return false;
            continue;
        }

        DiffMarker marker;
        if (ParseDerivativeMarker(el, marker))
        {
            if (cur_order > 0)
                vars.emplace_back(cur_var, cur_order);

            cur_order = marker.order;
            cur_var = marker.remaining;
            den_marker_found = true;

            //a denominator power such as pow(dx,2) or pow(d,2)x keeps the
            //variable part inside the power base row
            Power* p = dynamic_cast<Power*>(el);
            if (p)
            {
                CodeRow<>* base = p->GetBaseRow();
                for (uint bi = 1; base && bi < base->elements->Count(); ++bi)
                {
                    ParserString ps;
                    base->elements->Get(bi).get()->ToParserString(ps);
                    cur_var += ps.Text();
                }
            }
        }
        else if (den_marker_found)
        {
            ParserString ps;
            el->ToParserString(ps);
            cur_var += ps.Text();
        }
        else
        {
            //denominator does not start with a derivative marker
            return false;
        }
    }

    if (cur_order > 0)
        vars.emplace_back(cur_var, cur_order);

    if (vars.empty())
        return false;

    //Leibniz rule: numerator order must equal the total number of
    //differentiation operators in the denominator
    int total_den_order = 0;
    for (const auto& v : vars)
        total_den_order += v.second;

    if (num_order != total_den_order)
        return false;

    return true;
}

bool Division::BuildDerivativeParserString(ParserString& str)
{
    std::u32string function_text;
    std::vector<std::pair<std::u32string, int>> vars;
    int num_order = 0;
    if (!ParseDerivativeFraction(function_text, vars, num_order))
        return false;

    std::u32string result = function_text;
    //differentiation order is read right-to-left in the denominator
    for (auto it = vars.rbegin(); it != vars.rend(); ++it)
    {
        for (int o = 0; o < it->second; ++o)
            result = U"derivative(" + result + U"," + it->first + U")";
    }

    str.Add(id, result);
    return true;
}

bool Division::BuildDerivativeAtPointParserString(ParserString& str, bool include_assignments)
{
    if (!parent)
        return false;

    int pos = parent->elements->GetElementPos(id);
    if (pos < 0 || pos + 1 >= parent->elements->Count())
        return false;

    ElementPtr next = parent->elements->Get(pos + 1);
    if (!next || next->type != ElementType::EVALUATION_BAR_SUBSCRIPT)
        return false;

    EvaluationBarSubscript* bar = dynamic_cast<EvaluationBarSubscript*>(next.get());
    if (!bar)
        return false;

    std::u32string assignments_text = include_assignments ? bar->ToText() : U"";
    if (include_assignments && assignments_text.empty())
        return false;

    std::u32string function_text;
    std::vector<std::pair<std::u32string, int>> vars;
    int num_order = 0;
    if (!ParseDerivativeFraction(function_text, vars, num_order))
        return false;

    std::u32string result = U"derivative(" + function_text;
    if (include_assignments)
        result += U"," + assignments_text;
    result += U")";

    str.Add(id, result);
    return true;
}

bool Division::IsDerivative() const
{
    ParserString ps;
    return const_cast<Division*>(this)->BuildDerivativeParserString(ps);
}

void Division::AddNumerator(ElementPtr numerator)
{
    if (GetFirst()->IsEmpty())
        GetFirst()->elements->Clear();
    GetFirst()->elements->Add(numerator);
}

void Division::AddDenomerator(ElementPtr denomerator)
{
    if (GetLast()->IsEmpty())
        GetLast()->elements->Clear();
    GetLast()->elements->Add(denomerator);
}

CodeRow<>* Division::GetNumeratorRow() const
{
    return GetFirst();
}

CodeRow<>* Division::GetDenominatorRow() const
{
    return GetLast();
}

}
