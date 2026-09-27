/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "equation.h"
#include "result.h"
#include "division.h"
#include "../document.h"

namespace yutovo
{

//Equation

Equation::Equation(Element* _parent) : 
    MiddleShapeFormula(_parent)
{
    type = ElementType::EQUATION;
    remake_always = true;
}

Equation::Equation(Element* _parent, yutovo_solver::ResultType _result_type, bool with_init) :
    MiddleShapeFormula(_parent, with_init),
    result_type(_result_type)
{
    type = ElementType::EQUATION;
    remake_always = true;
}

Equation::Equation(Document* _document, yutovo_solver::ResultType _result_type, bool with_init) :
    MiddleShapeFormula(_document, with_init),
    result_type(_result_type)
{
    type = ElementType::EQUATION;
    remake_always = true;
}

Equation::Equation(const Equation& source) :
    MiddleShapeFormula(source),
    result_type(source.result_type),
    last_expression(source.last_expression),
    dependencies(source.dependencies)
{
    if (GetLast() && GetLast()->elements->Count() > 0)
    {
        auto* r = GetLast()->elements->Get(0).get();
        if (dynamic_cast<ResultRow*>(r))
        {
            result.reset((ResultRow*)r->Clone());
            GetLast()->elements->Replace(result, 0);
        }
    }
}

Element* Equation::Clone()
{
    return new Equation(*this);
}

Element* Equation::Create(Element* _parent)
{
    return new Equation(_parent);
}

void Equation::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    MiddleShapeFormula::ToJson(value, alloc);
    value.AddMember("result_type", (int)result_type, alloc);
}

Element* Equation::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.HasMember("result_type") || !value["result_type"].IsInt())
        return nullptr;
    yutovo_solver::ResultType result_type = (yutovo_solver::ResultType)value["result_type"].GetInt();

    if (parent)
        return new Equation(parent, result_type, false);
    return new Equation(document, result_type, false);
}

bool Equation::AfterFromJson()
{
    auto* r = GetLast()->elements->Get(0).get();
    result.reset((ResultRow*)r->Clone());
    GetLast()->elements->Replace(result, 0);
    return true;
}

void Equation::Draw() const
{
    GetShape()->draw_func = 
        [&](const Rect& r)
        {
            if (document->selection.IsSelected(id))
                window->DrawText("=", draw_string_format, r, document->config.formula_bg_color, document->config.bg_selection_color, false);
            else
                window->DrawText("=", draw_string_format, r, document->config.shapes_color, document->config.formula_bg_color, false);
        };

    MiddleShapeFormula::Draw();

    //if there are two or more formulas in a row, draw a frame for better view
    if (has_frame_hilight)
    {
        Rect r = GetAbsoluteRect();
        window->DrawRect(r.left, r.top, r.width, r.height, document->config.formula_frame_color);
    }
}

void Equation::UpdateRect(bool with_elements)
{
    if (!draw_string_format)
        Rescale();
    Size s = parent->window->GetTextSize(std::u32string(1, '='), draw_string_format);
    GetShape()->rect.SetSize(s.width, s.height * 3 / 4);
    GetShape()->baseline = GetShape()->rect.height / 3 * 2;

    MiddleShapeFormula::UpdateRect(false);
}

bool Equation::Remake(bool with_elements)
{
    bool changed = MiddleShapeFormula::Remake(with_elements);
    if (!with_elements)
        GetLast()->Remake(true);

    UpdateRect();

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

bool Equation::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    if (caret->IsOnElement(GetShape()->id) && !left)
    {
        if (with_undo)
        {
            document->StoreUndo(parent->parent->id);
            with_undo = false;
        }
        elements->RemoveAt(2, 1);
    }
    return MiddleShapeFormula::DeleteElements(left, with_undo, changed_element);
}

bool Equation::AfterInsert(bool with_undo)
{
    int pos = parent->elements->GetElementPos(id);
    if (pos > 0)
        GetFirst()->elements->Clear();
    ready = false;
    for (int i = pos - 1; i >= 0; --i)
    {
        auto el = parent->elements->Get(i);
        if (el->type == ElementType::ASSIGNMENT || el->type == ElementType::EQUATION)
            break;
        GetFirst()->elements->Move(el, 0);
    }
    ready = true;
    caret->SetState(GetShape()->id);
    return true;
}

void Equation::BeforePaste()
{
    GetFirst()->BeforePaste();
    GetShape()->BeforePaste();
}

void Equation::Solve()
{
    MiddleShapeFormula::Solve();

    ParserString str;
    GetFirst()->ToParserString(str);
    if (last_expression.Length() != 0)
        empty = false;
    if (last_expression != str || empty)
        document->AddResolveElement(logical_id);
}

void Equation::ReSolve(bool if_error, bool force)
{
    if (if_error && result && result->last_error_code == yutovo_solver::ErrorCode::OK)
        return;
    
    ParserString str;
    GetFirst()->ToParserString(str);
    if (last_expression == str && !force && !empty)
        return;
    
    empty = false;
    last_expression = str;
    document->RemoveErrorMarks(id);
    UpdateResult(str);
}

bool Equation::Depends(const std::string& identifier)
{
    if (std::find(dependencies.begin(), dependencies.end(), identifier) != dependencies.end())
        return true;
    return false;
}

void Equation::SetResult(const Config::AutoResultConfig& config)
{
    result.reset(new AutoResult(GetLast(), config));
    GetLast()->elements->Clear();
    GetLast()->elements->Add(result);
}

void Equation::SetResult(const Config::RealResultConfig& config)
{
    result.reset(new RealResult(GetLast(), config));
    GetLast()->elements->Clear();
    GetLast()->elements->Add(result);
}

void Equation::SetResult(const Config::IntegerResultConfig& config)
{
    result.reset(new IntegerResult(GetLast(), config));
    GetLast()->elements->Clear();
    GetLast()->elements->Add(result);
}

void Equation::SetResult(const Config::RationalResultConfig& config)
{
    result.reset(new RationalResult(GetLast(), config));
    GetLast()->elements->Clear();
    GetLast()->elements->Add(result);
}

void Equation::SetResult(const Config::ComplexResultConfig& config)
{
    result.reset(new ComplexResult(GetLast(), config));
    GetLast()->elements->Clear();
    GetLast()->elements->Add(result);
}

void Equation::SetSymbolicRealResult(const Config::RealResultConfig& config)
{
    result.reset(new SymbolicRealResult(GetLast(), config));
    GetLast()->elements->Clear();
    GetLast()->elements->Add(result);
}

void Equation::SetSymbolicRationalResult(const Config::RationalResultConfig& config)
{
    result.reset(new SymbolicRationalResult(GetLast(), config));
    GetLast()->elements->Clear();
    GetLast()->elements->Add(result);
}

void Equation::SetSymbolicComplexResult(const Config::ComplexResultConfig& config)
{
    result.reset(new SymbolicComplexResult(GetLast(), config));
    GetLast()->elements->Clear();
    GetLast()->elements->Add(result);
}

bool Equation::SetResult(ResultType _result_type, bool with_undo)
{
    if (result_type == _result_type)
        return false;

    caret->SetState(id, 1, true);

    if (with_undo)
        document->StoreUndo(id);
    
    result_type = _result_type;
    result.reset();
    document->RemoveErrorMarks(id);

    ParserString str;
    GetFirst()->ToParserString(str);
    UpdateResult(str);
    return true;
}

bool Equation::SetConfig(int precision, int exp, AngleMeasure default_angle_measure, AngleMeasure result_angle_measure, bool with_undo)
{
    switch (result->type)
    {
    case ElementType::AUTO_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        AutoResult* r = (AutoResult*)result.get();
        return r->SetConfig(precision, exp, default_angle_measure, result_angle_measure);
    }
    case ElementType::REAL_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        RealResult* r = (RealResult*)result.get();
        return r->SetConfig(precision, exp, default_angle_measure, result_angle_measure);
    }
    case ElementType::COMPLEX_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        ComplexResult* r = (ComplexResult*)result.get();
        return r->SetConfig(precision, exp, default_angle_measure, result_angle_measure);
    }
    case ElementType::ARRAY_REAL_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        ArrayRealResult* r = (ArrayRealResult*)result.get();
        return r->SetConfig(precision, exp, default_angle_measure, result_angle_measure);
    }
    case ElementType::SYMBOLIC_REAL_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        SymbolicRealResult* r = (SymbolicRealResult*)result.get();
        return r->SetConfig(precision, exp, default_angle_measure, result_angle_measure);
    }
    case ElementType::SYMBOLIC_COMPLEX_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        SymbolicComplexResult* r = (SymbolicComplexResult*)result.get();
        return r->SetConfig(precision, exp, default_angle_measure, result_angle_measure);
    }
    default:
        return false;
    }
}

bool Equation::SetConfig(Notation default_notation, Notation result_notation, bool with_undo)
{
    switch (result->type)
    {
    case ElementType::INTEGER_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        IntegerResult* r = (IntegerResult*)result.get();
        return r->SetConfig(default_notation, result_notation);
    }
    case ElementType::AUTO_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        AutoResult* r = (AutoResult*)result.get();
        return r->SetConfig(default_notation, result_notation);
    }
    default:
        return false;
    }
}

bool Equation::SetConfig(FractionForm fraction_form, bool with_undo)
{
    switch (result->type)
    {
    case ElementType::RATIONAL_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        RationalResult* r = (RationalResult*)result.get();
        return r->SetConfig(fraction_form);
    }
    case ElementType::AUTO_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        AutoResult* r = (AutoResult*)result.get();
        return r->SetConfig(fraction_form);
    }
    default:
        return false;
    }
}

bool Equation::SetConfig(ComplexForm complex_form, bool with_undo)
{
    switch (result->type)
    {
    case ElementType::COMPLEX_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        ComplexResult* r = (ComplexResult*)result.get();
        return r->SetConfig(complex_form);
    }
    case ElementType::AUTO_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        AutoResult* r = (AutoResult*)result.get();
        return r->SetConfig(complex_form);
    }
    default:
        return false;
    }
}

bool Equation::SetConfig(int precision, int exp, AngleMeasure default_angle_measure, AngleMeasure result_angle_measure, bool with_undo, 
    ComplexForm form, uint max_count)
{
    switch (result->type)
    {
    case ElementType::AUTO_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        AutoResult* r = (AutoResult*)result.get();
        return r->SetConfig(precision, exp, default_angle_measure, result_angle_measure, form, max_count);
    }
    case ElementType::COMPLEX_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        ComplexResult* r = (ComplexResult*)result.get();
        return r->SetConfig(precision, exp, default_angle_measure, result_angle_measure, form, max_count);
    }
    default:
        return false;
    }
}

bool Equation::SetConfig(const yutovo_calculator::Unit& unit, bool with_undo)
{
    switch (result->type)
    {
    case ElementType::RATIONAL_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        RationalResult* r = (RationalResult*)result.get();
        return r->SetConfig(unit);
    }
    case ElementType::REAL_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        RealResult* r = (RealResult*)result.get();
        return r->SetConfig(unit);
    }
    case ElementType::AUTO_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        AutoResult* r = (AutoResult*)result.get();
        return r->SetConfig(unit);
    }
    default:
        return false;
    }
}

std::string Equation::ToHtml() const
{
    std::string s = GetFirst()->ToHtml();
    s += "<mo>=</mo>";
    if (GetLast())
        s += GetLast()->ToHtml();
    return s;
}

std::u32string Equation::ToText() const
{
    std::u32string s;
    if (elements->Count() > 0)
        s = elements->Get(0)->ToText();
    s += U"=";
    if (elements->Count() == 3)
        s += elements->Get(2)->ToText();
    return s;
}

void Equation::ToParserString(ParserString& str)
{
    if (elements->Count() > 0)
    {
        int start = str.Length();
        elements->Get(0)->ToParserString(str);
        str.Annotate(id, start, str.Length());
    }
}

void Equation::UpdateResult(ParserString& str)
{
    if (!ready)
        return;
    if (GetLast())
    {
        // ResultType effective_type = result_type;
        // if (effective_type != ResultType::AUTO &&
        //     effective_type != ResultType::SYMBOLIC_REAL &&
        //     effective_type != ResultType::SYMBOLIC_RATIONAL &&
        //     effective_type != ResultType::SYMBOLIC_COMPLEX)
        // {
        //     if (elements->Count() > 0 && ContainsDerivative(elements->Get(0).get()))
        //         effective_type = ResultType::SYMBOLIC_REAL;
        // }

        // if (!result || result->type != ResultTypeToElementType(effective_type))
        // {
        //     if (result)
        //     {
        //         bool c = caret->IsInsideElement(result->id);
        //         result.reset();
        //         GetLast()->elements->Clear();
        //         if (c)
        //             caret->SetState(GetShape()->id);
        //     }

        //     switch (effective_type)
        if (!result)
        {
            switch (result_type)
            {
        	case ResultType::REAL:
                result.reset(new RealResult(GetLast()));
                break;
        	case ResultType::INTEGER:
                result.reset(new IntegerResult(GetLast()));
                break;
        	case ResultType::RATIONAL:
                result.reset(new RationalResult(GetLast()));
                break;
        	case ResultType::COMPLEX:
                result.reset(new ComplexResult(GetLast()));
                break;
        	case ResultType::ARRAY_REAL:
                result.reset(new ArrayRealResult(GetLast()));
                break;
            case ResultType::AUTO:
                result.reset(new AutoResult(GetLast()));
                break;
            case ResultType::SYMBOLIC_REAL:
                result.reset(new SymbolicRealResult(GetLast()));
                break;
            case ResultType::SYMBOLIC_RATIONAL:
                result.reset(new SymbolicRationalResult(GetLast()));
                break;
            case ResultType::SYMBOLIC_COMPLEX:
                result.reset(new SymbolicComplexResult(GetLast()));
                break;
            default:
                return;
            }
            GetLast()->elements->Clear();
            GetLast()->elements->Add(result);
        }
        else
        {
            bool c = caret->IsInsideElement(result->id);
            result->Reset();
            GetLast()->elements->Replace(result, 0);
            if (c)
                caret->SetState(GetShape()->id);
        }

        result->Solve(str); //solve the expression in the left part
        document->AddChangedElement(id);
    }
}

}
