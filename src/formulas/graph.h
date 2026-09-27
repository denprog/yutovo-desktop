/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __GRAPH_FORMULA_H__
#define __GRAPH_FORMULA_H__

#include "middle_shape_formula.h"
#include "config.h"
#include "code_paragraphs_block.h"
#include <mutex>
#ifdef _MSC_VER
using std::abs;
#define MGL_NO_CMATH_OVERLOADS
#endif
#include "mgl2/mgl.h"

namespace yutovo
{

class Graph : public Formula
{
public:
    Graph(Element* _parent, bool with_init = true);
    Graph(Document* _document, bool with_init = true);
    Graph(const Graph& source);

    void Init();

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);
    virtual void Resize(const int dx, const int dy);

    virtual void MovePicture(const int dx, const int dy);
    virtual void ZoomPicture(const int pixels);

    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);

    virtual void UpdateLevel(uint8_t _level);

    virtual bool AfterInsert(bool with_undo);

    virtual void LogicalIdChanged(const LogicalId& last_id);

    virtual bool Depends(const std::string& identifier);

    virtual void GetImage(std::string& image_base64) const;

protected:
    void SetNumber(const double num, CodeRow<>* el);

public:
    Dependencies dependencies;
    yutovo_solver::ErrorCode last_error_code = yutovo_solver::ErrorCode::OK;
    yutovo_calculator::ParserExceptionCode last_parser_error_code = yutovo_calculator::ParserExceptionCode::None;
    GraphFormat format;
    std::vector<ParserString> last_expressions;

#ifdef TEST
public:
#else
protected:
#endif
    double x_left = -1, x_right = 1, y_bottom = -1, y_top = 1;

    CodeRow<>* GetYTop() const;
    CodeRow<>* GetYBottom() const;
    CodeParagraphsBlock<>* GetExpression() const;
    CodeRow<>* GetXLeft() const;
    CodeRow<>* GetVariable() const;
    CodeRow<>* GetXRight() const;
    Shape* GetShape() const;

    mutable mglGraph graph;
    static std::recursive_mutex mathgl_mutex;  //protects MathGL global state

    Config::ArrayRealResultConfig config;

    bool solving = false;
    bool moving = false;
};

class GraphLine : public Graph
{
public:
    GraphLine(Element* _parent, bool with_init = true);
    GraphLine(Document* _document, bool with_init = true);
    GraphLine(const GraphLine& source);

    void Init();

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual bool AfterFromJson();

    virtual void Solve();

    virtual void ReSolve(bool if_error = false, bool force = false);

    virtual void PutResult(Result& result);

    virtual bool MouseLButtonHold(const int x, const int y, MouseHoldType& hold_type, ElementId& hold_id);

    virtual std::string ToHtml() const;
    virtual std::u32string ToText() const;
    virtual void ToParserString(ParserString& str);

    void GetPlotFormat(const int pos, Color& color, uint& width);
    void GetPlotFormat(PlotFormat& format);
    void SetPlotFormat(const PlotFormat& format);

public:
    struct Plot
    {
        std::string guid;
        PlotFormat format;
        std::vector<double> x, y;
    };

    std::vector<Plot> plots;

protected:
    static const std::vector<Color> default_colors;

    int mouse_l_button_pos = 0;
};

}

#endif
