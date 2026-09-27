/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __EQUATION_H__
#define __EQUATION_H__

#include "middle_shape_formula.h"
#include "result.h"

namespace yutovo
{

class Equation : public MiddleShapeFormula
{
public:
    Equation(Element* _parent);
    Equation(Element* _parent, yutovo_solver::ResultType _result_type, bool with_init = true);
    Equation(Document* _document, yutovo_solver::ResultType _result_type, bool with_init = true);
    Equation(const Equation& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);
    virtual bool AfterFromJson();

    virtual void Draw() const;

    virtual void UpdateRect(bool with_elements = false);

    virtual bool Remake(bool with_elements = false);

    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);
    
    virtual bool AfterInsert(bool with_undo);
    virtual void BeforePaste();

    virtual void Solve();
    virtual void ReSolve(bool if_error = false, bool force = false);

    virtual bool Depends(const std::string& identifier);

    void SetResult(const Config::AutoResultConfig& config);
    void SetResult(const Config::RealResultConfig& config);
    void SetResult(const Config::IntegerResultConfig& config);
    void SetResult(const Config::RationalResultConfig& config);
    void SetResult(const Config::ComplexResultConfig& config);
    void SetSymbolicRealResult(const Config::RealResultConfig& config);
    void SetSymbolicRationalResult(const Config::RationalResultConfig& config);
    void SetSymbolicComplexResult(const Config::ComplexResultConfig& config);
    bool SetResult(ResultType _result_type, bool with_undo);
    
    bool SetConfig(int precision, int exp, AngleMeasure default_angle_measure, AngleMeasure result_angle_measure, bool with_undo);
    bool SetConfig(Notation default_notation, Notation result_notation, bool with_undo);
    bool SetConfig(FractionForm fraction_form, bool with_undo);
    bool SetConfig(ComplexForm complex_form, bool with_undo);
    bool SetConfig(int precision, int exp, AngleMeasure default_angle_measure, AngleMeasure result_angle_measure, bool with_undo, ComplexForm form, uint max_count);
    bool SetConfig(const yutovo_calculator::Unit& unit, bool with_undo);
    
    virtual std::string ToHtml() const;
    virtual std::u32string ToText() const;
    virtual void ToParserString(ParserString& str);

protected:
    void UpdateResult(ParserString& str);

public:
    yutovo_solver::ResultType result_type;
    Dependencies dependencies;
    ParserString last_expression;

protected:
    ResultPtr result;
    bool ready = true;
    bool empty = true;
};

}

#endif
