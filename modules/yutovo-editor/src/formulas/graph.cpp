/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "graph.h"
#include "document.h"
#include "formulas/code_block.h"
#include "formulas/code_paragraph.h"
#include "formulas/code_paragraphs_block.h"
#include "code_row.h"
#include "formulas/power.h"
#include "formulas/code_string.h"
#include "formulas/multiply.h"
#include "formulas/minus.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <algorithm>

#ifdef _MSC_VER
#undef GetObject
#endif

namespace yutovo
{

typedef unsigned char uchar;

std::recursive_mutex Graph::mathgl_mutex;

//Graph

Graph::Graph(Element* _parent, bool with_init) :
    Formula(_parent)
{
}

Graph::Graph(Document* _document, bool with_init) :
    Formula(_document)
{
}

Graph::Graph(const Graph& source) :
    Formula(source),
    last_expressions(source.last_expressions),
    dependencies(source.dependencies),
    format(source.format),
    x_left(source.x_left), 
    x_right(source.x_right), 
    y_bottom(source.y_bottom), 
    y_top(source.y_top),
    config(source.config)
{
}

void Graph::Init()
{
    if (elements->Count() == 0)
    {
        elements->Add(ElementPtr(new CodeRow<>(this))); //y up
        elements->Add(ElementPtr(new CodeParagraphsBlock<>(this, true))); //expression
        elements->Add(ElementPtr(new CodeRow<>(this))); //y down
        elements->Add(ElementPtr(new CodeRow<>(this))); //x left
        elements->Add(ElementPtr(new CodeRow<>(this))); //variable
        elements->Add(ElementPtr(new CodeRow<>(this))); //x right
        elements->Add(ElementPtr(new Shape(this))); //graph
    }
    GetShape()->can_resize = true;
    GetShape()->can_move_picture = true;
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->can_merge = false;
    UpdateLevel(level);
    GetShape()->editable = false;
    editable = false;
}

void Graph::Draw() const
{
    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, document->config.bg_selection_color);
    }

    Formula::Draw();
}

bool Graph::Remake(bool with_elements)
{
    if (document->editing)
        moving = false;

    UpdateLevel(level);

    bool changed = Formula::Remake(with_elements);

    int x_left = std::max({GetExpression()->rect.width, GetYTop()->rect.width, GetYBottom()->rect.width});
    GetShape()->rect.SetRect(0, 0, format.size.width, format.size.height);
    GetYTop()->rect.Move(x_left - GetYTop()->rect.width, GetShape()->rect.top);
    GetExpression()->rect.Move(x_left - GetExpression()->rect.width, GetShape()->rect.height / 2 - GetExpression()->rect.height / 2);
    GetYBottom()->rect.Move(x_left - GetYBottom()->rect.width, GetShape()->rect.GetBottom() - GetYBottom()->rect.height);
    if (GetXLeft()->rect.width + GetVariable()->rect.width / 2 > GetShape()->rect.width / 2)
        GetShape()->rect.Move(x_left + 2 + ((GetXLeft()->rect.width + GetVariable()->rect.width / 2) - GetShape()->rect.width / 2), 0);
    else
        GetShape()->rect.Move(x_left + 2, 0);
    GetXLeft()->rect.Move(x_left, GetShape()->rect.GetBottom() + 2);
    GetVariable()->rect.Move(GetShape()->rect.left + 2 + GetShape()->rect.width / 2 - GetVariable()->rect.width / 2, GetShape()->rect.GetBottom() + 2);
    if (GetVariable()->rect.width / 2 + GetXRight()->rect.width > GetShape()->rect.width / 2 - 2)
        GetXRight()->rect.Move(GetVariable()->rect.GetRight() + 2, GetShape()->rect.GetBottom() + 2);
    else
        GetXRight()->rect.Move(GetShape()->rect.GetRight() - GetXRight()->rect.width, GetShape()->rect.GetBottom() + 2);
    baseline = GetShape()->rect.GetBottom() - GetShape()->rect.height / 2;

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

void Graph::Resize(const int dx, const int dy)
{
    if (format.size.width + dx >= 100)
        format.size.width += dx;
    if (format.size.height + dy >= 100)
        format.size.height += dy;
}

void Graph::MovePicture(const int dx, const int dy)
{
    moving = true;
    double tx = (x_right - x_left) / (GetShape()->rect.width - 40);
    double ty = (y_bottom - y_top) / (GetShape()->rect.height - 40);
    double _dx = tx * dx;
    double _dy = ty * dy;
    SetNumber(x_left - _dx, GetXLeft());
    SetNumber(x_right - _dx, GetXRight());
    SetNumber(y_bottom - _dy, GetYBottom());
    SetNumber(y_top - _dy, GetYTop());
    document->AddResolveElement(logical_id);
}

void Graph::ZoomPicture(const int pixels)
{
    moving = true;
    double k = pixels > 0 ? (1 + double(pixels) / 15) : (-(1 / (double(pixels) / 15 - 1)));
    double w = (x_right - x_left) / k;
    double c = (x_right + x_left) / 2;
    SetNumber(c - w / 2, GetXLeft());
    SetNumber(c + w / 2, GetXRight());
    w = (y_top - y_bottom) / k;
    c = (y_top + y_bottom) / 2;
    SetNumber(c - w / 2, GetYBottom());
    SetNumber(c + w / 2, GetYTop());
    document->AddResolveElement(logical_id);
}

bool Graph::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    return false;
}

void Graph::UpdateLevel(uint8_t _level)
{
    Formula::UpdateLevel(_level);
    if (_level >= MAX_LEVEL)
        return;
    if (GetXLeft())
        GetXLeft()->UpdateLevel(_level + 1);
    if (GetXRight())
        GetXRight()->UpdateLevel(_level + 1);
    if (GetYBottom())
        GetYBottom()->UpdateLevel(_level + 1);
    if (GetYTop())
        GetYTop()->UpdateLevel(_level + 1);
}

bool Graph::AfterInsert(bool with_undo)
{
    CaretState c;
    if (elements->Get(0)->GetFirstCaretState(c, nullptr))
    {
        caret->SetState(c);
        return true;
    }
    return false;
}

void Graph::LogicalIdChanged(const LogicalId& last_id)
{
    solving = false;
}

bool Graph::Depends(const std::string& identifier)
{
    if (std::find(dependencies.begin(), dependencies.end(), identifier) != dependencies.end())
        return true;
    return false;
}

void Graph::GetImage(std::string& image_base64) const
{
    std::lock_guard<std::recursive_mutex> lock(mathgl_mutex);

    Draw();

    const unsigned char* picture = graph.GetRGBA();
    std::vector<unsigned char> rgba(picture, picture + 4 * (graph.GetWidth() * graph.GetHeight()));
    std::vector<unsigned char> png;

    RgbaToPng(rgba, graph.GetWidth(), graph.GetHeight(), png);
    image_base64 = yutovo::Base64Encode(png);
}

void Graph::SetNumber(const double num, CodeRow<>* el)
{
    bool b = document->caret->IsInsideElement(el->id);
    el->elements->Clear();
    std::ostringstream oss;
    oss.precision(3);
    oss << std::scientific << num;
    std::string s = oss.str();
    std::string m = s.substr(0, s.find('e'));
    std::string e = s.substr(s.find('e') + 1);
    int exp = std::stoi(e);
    if (std::abs(exp) < 3)
    {
        std::ostringstream os;
        os.precision(3);
        os << std::fixed << num;
        m = os.str();
        e = "";
    }

    if (m.find('.') != std::string::npos)
        m = m.substr(0, m.find_last_not_of('0') + 1);

    if (m.empty())
    {
        el->AddElement(CodeStringPtr(new CodeString(el, "0")));
        return;
    }
    if (m[0] == '-')
    {
        el->AddElement(ElementPtr(new Minus(el)));
        el->AddElement(CodeStringPtr(new CodeString(el, m.substr(1, m.size() - 1))));
    }
    else
        el->AddElement(CodeStringPtr(new CodeString(el, m)));
    if (!e.empty())
    {
        el->AddElement(ElementPtr(new Multiply(el)));
        PowerPtr p(new Power(el));
        el->AddElement(p);
        p->AddBase(CodeStringPtr(new CodeString(p.get(), "10")));
        if (e[0] == '-')
        {
            e = e.substr(1, e.size() - 1);
            e.erase(0, e.find_first_not_of('0'));
            if (e.empty())
                e = "0";
            p->AddExponent(ElementPtr(new Minus(p.get())));
            p->AddExponent(CodeStringPtr(new CodeString(p.get(), e)));
        }
        else
        {
            if (e[0] == '+')
                e = e.substr(1, e.size() - 1);
            e.erase(0, e.find_first_not_of('0'));
            if (e.empty())
                e = "0";
            p->AddExponent(CodeStringPtr(new CodeString(p.get(), e)));
        }
    }
    if (b)
    {
        CaretState c;
        if (el->GetFirstCaretState(c, nullptr))
        {
            document->caret->SetState(c);
            document->selection.Clear();
        }
    }
}

CodeRow<>* Graph::GetYTop() const
{
    return (CodeRow<>*)elements->Get(0).get();
}

CodeParagraphsBlock<>* Graph::GetExpression() const
{
    return (CodeParagraphsBlock<>*)elements->Get(1).get();
}

CodeRow<>* Graph::GetYBottom() const
{
    return (CodeRow<>*)elements->Get(2).get();
}

CodeRow<>* Graph::GetXLeft() const
{
    return (CodeRow<>*)elements->Get(3).get();
}

CodeRow<>* Graph::GetVariable() const
{
    return (CodeRow<>*)elements->Get(4).get();
}

CodeRow<>* Graph::GetXRight() const
{
    return (CodeRow<>*)elements->Get(5).get();
}

Shape* Graph::GetShape() const
{
    return (Shape*)elements->Get(6).get();
}

//GraphLine

const std::vector<Color> GraphLine::default_colors = {Color::Red(), Color::Blue(), Color::Green(), Color::Magenta(), Color::Cian(), Color::Black()};

GraphLine::GraphLine(Element* _parent, bool with_init) : 
    Graph(_parent, with_init)
{
    type = ElementType::GRAPH_LINE;
    if (with_init)
        Init();
}

GraphLine::GraphLine(Document* _document, bool with_init) : 
    Graph(_document, with_init)
{
    type = ElementType::GRAPH_LINE;
    if (with_init)
        Init();
}

GraphLine::GraphLine(const GraphLine& source) : 
    Graph(source),
    plots(source.plots)
{
    Init();
}

void GraphLine::Init()
{
    Graph::Init();

    GetShape()->draw_func = 
        [&](const Rect& r)
        {
            std::lock_guard<std::recursive_mutex> lock(mathgl_mutex);

            graph.NewFrame();
            graph.SetFlagAdv(1, MGL_NO_SCALE_REL);
            graph.SetScaleText(false);
            graph.SetSize(r.width, r.height, false);
            graph.SubPlot(1, 1, 0, "#");
            graph.InPlot(0.05, 0.95, 0.05, 0.95);
            if (r.width < 200 || r.height < 200)
                graph.SetPenDelta(0.5);
            else if (r.width < 300 || r.height < 300)
                graph.SetPenDelta(0.6);
            else if (r.width < 400 || r.height < 400)
                graph.SetPenDelta(0.7);
            else
                graph.SetPenDelta(1.);

            if (solving && !moving)
            {
                graph.SetRanges(-1, 1, -1, 1);
                graph.SetFontSize(level + 1);
                graph.Puts(mglPoint(0, 0), "~");
            }
            else if (last_error_code != yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR && last_error_code != yutovo_solver::ErrorCode::OK && !moving)
            {
                graph.SetRanges(-1, 1, -1, 1);
                graph.SetFontSize(level + 1);
                if (last_parser_error_code != yutovo_calculator::ParserExceptionCode::None)
                    graph.Puts(mglPoint(0, 0), ErrorCodeToString(last_parser_error_code).c_str());
                else
                    graph.Puts(mglPoint(0, 0), ErrorCodeToString(last_error_code).c_str());
            }
            else
            {
                graph.SetRanges(x_left, x_right, y_bottom, y_top);
                graph.SetFontSize(level);
                std::string f = "{" + format.color.ToRGB() + "}";
                graph.Axis("xy", std::string(f + "-1").c_str(), "h-1");
                if (format.grid_width > 0)
                    graph.Grid("xy", std::string("h" + std::to_string(format.grid_width) + f).c_str());
                graph.SetQuality(MGL_DRAW_NORM);
                //graph.SetPenDelta(0.5);
                for (const Plot& p : plots)
                {
                    auto& x = p.x;
                    auto& y = p.y;
                    if (!x.empty() && !y.empty())
                    {
                        mglData x_data(x.size());
                        mglData y_data(y.size());
                        x_data.Set(x);
                        y_data.Set(y);
                        f = "{" + p.format.color.ToRGB() + "}-" + std::to_string(p.format.width);
                        graph.Plot(x_data, y_data, f.c_str());
                    }
                }
            }

            const unsigned char* picture = graph.GetRGBA();
            std::vector<unsigned char> arr(picture, picture + 4 * (graph.GetWidth() * graph.GetHeight()));
            window->DrawImage(r.left + 1, r.top + 1, r.width, r.height, arr);
        };
}

Element* GraphLine::Clone()
{
    return new GraphLine(*this);
}

Element* GraphLine::Create(Element* _parent)
{
    return new GraphLine(_parent);
}

void GraphLine::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Formula::ToJson(value, alloc);
    format.ToJson(value, alloc);
    rapidjson::Value arr(rapidjson::kArrayType);
    for (auto& p : plots)
    {
        rapidjson::Value v;
        v.SetObject();
        p.format.ToJson(v, alloc);
        arr.PushBack(v, alloc);
    }
    value.AddMember("plots", arr, alloc);
}

Element* GraphLine::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    GraphFormat f;
    f.FromJson(value, alloc);
    GraphLine* el = parent ? new GraphLine(parent, false) : new GraphLine(document, false);
    el->format = f;

    if (value.HasMember("plots") && value["plots"].IsArray())
    {
        rapidjson::Value::ConstArray arr = value["plots"].GetArray();
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
        {
            std::string guid = boost::uuids::to_string(boost::uuids::random_generator()());
            Plot p{guid};
            if (arr[i].IsObject())
            {
                rapidjson::Value::ConstObject v = arr[i].GetObject();
                p.format.FromJson(v, alloc);
            }
            el->plots.push_back(p);
        }
    }

    return el;
}

bool GraphLine::AfterFromJson()
{
    if (elements->Count() < 7)
        elements->Clear();
    else if (elements->Get(1)->type != ElementType::CODE_PARAGRAPHS_BLOCK)
    {
        //this is old type of graph, transform it
        ElementPtr el = elements->Get(1);
        elements->RemoveAt(1, 1);
        ElementPtr b(new CodeParagraphsBlock<>(this, false));
        elements->Insert(b, 1);
        b->elements->Clear();

        CodeParagraph<>* p = new CodeParagraph<>(b.get(), true);
        p->elements->Add(el);
        StringFormatPtr f = GetStringFormat();
        Color color;
        uint width = 1;
        GetPlotFormat(0, color, width);
        p->SetMarker(U"█", document->GetStringFormat(f->family, f->size, f->bold, f->italic, f->underline, f->strikethrough, 
            f->subscript, f->superscript, color, f->text_bg_color, f->text_bg_selection_color));
        b->elements->Add(ElementPtr(p));
    }
    else
    {
        StringFormatPtr f = GetStringFormat();
        Color color;
        uint width = 1;
        for (int i = 0; i < elements->Get(1)->elements->Count(); ++i)
        {
            CodeParagraph<>* p = (CodeParagraph<>*)elements->Get(1)->elements->Get(i).get();
            GetPlotFormat(i, color, width);
            p->SetMarker(U"█", document->GetStringFormat(f->family, f->size, f->bold, f->italic, f->underline, f->strikethrough, 
                f->subscript, f->superscript, color, f->text_bg_color, f->text_bg_selection_color));
        }
    }

    Init();
    last_expressions.clear();
    return true;
}

void GraphLine::Solve()
{
    if (rect.width == 0)
        return;
        
    std::vector<ParserString> expressions;
    for (int i = 0; i < GetExpression()->elements->Count(); ++i)
    {
        ParserString str;
        ElementPtr el = GetExpression()->elements->Get(i);
        ElementId& _id = el->id;
        str.Add(_id, U"graph_line(");
        el->ToParserString(str);
        str.Add(_id, U",");
        GetVariable()->ToParserString(str);
        str.Add(_id, U",");
        GetXLeft()->ToParserString(str);
        str.Add(_id, U",");
        GetXRight()->ToParserString(str);
        str.Add(_id, U",");
        GetYBottom()->ToParserString(str);
        str.Add(_id, U",");
        GetYTop()->ToParserString(str);
        str.Add(_id, U",");
        str.Add(_id, ToUtfString(std::to_string(rect.width)));
        str.Add(_id, U")");
        expressions.push_back(str);
    }

    if (last_expressions != expressions)
    {
        last_expressions = expressions;
        document->AddResolveElement(logical_id);
    }
}

void GraphLine::ReSolve(bool if_error, bool force)
{
    if (if_error)
        return;

    document->RemoveErrorMarks(id);
    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    assert(code);

    //plots count must be count of the expressions
    while (plots.size() < GetExpression()->elements->Count())
    {
        std::string guid = boost::uuids::to_string(boost::uuids::random_generator()());
        plots.push_back(Plot{guid, default_colors[plots.size() % 4]});
    }
    if (plots.size() > GetExpression()->elements->Count())
        plots.resize(GetExpression()->elements->Count());

    if (solving)
    {
        for (size_t i = 0; i < plots.size(); ++i)
            document->BreakSolving(GetExpression()->elements->Get(i)->logical_id, plots[i].guid, ((CodeBlock*)code.get())->code_id, false);
    }

    solving = true;
    for (const Plot& p : plots)
    {
        for (size_t i = 0; i < plots.size() && i < last_expressions.size(); ++i)
        {
            document->Solve(GetExpression()->elements->Get(i)->logical_id, plots[i].guid, ((CodeBlock*)code.get())->code_id, config, 
                !GetParent(1)->visible, last_expressions[i].Text(), 
                (!moving && last_error_code != yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR) ? document->config.solve_delay : 0);
        }
    }
    document->AddChangedElement(id);
}

void GraphLine::PutResult(Result& result)
{
    solving = false;

    last_error_code = result.error.error_code;
    dependencies = result.dependencies;
    if (result.error.error_code == yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        last_expressions.clear();
        return;
    }

    auto it = std::find_if(plots.begin(), plots.end(), 
        [result](const Plot& p)
        {
            return p.guid == result.guid;
        });
    if (it == plots.end())
        return;

    Plot& plot = *it;

    if (result.error.error_code != yutovo_solver::ErrorCode::OK)
    {
        //put error message
        size_t i = std::distance(plots.begin(), it);
        last_parser_error_code = result.error.parser_error_code;
        if (i < last_expressions.size())
        {
            ElementId err_id = last_expressions[i].GetElement(result.error.pos, result.error.size);
            if (!err_id.empty())
            {
                auto el = document->GetElement(err_id);
                if (el)
                {
                    document->RemoveErrorMarks(parent->parent->id);
                    document->AddErrorMark(err_id, 0, el->elements->Count());
                    document->Redraw(err_id, false);
                }
            }
        }
    }
    else
        document->RemoveErrorMarks(id);

    std::locale::global(std::locale::classic());

    auto to_double = 
        [](Value& v)
        {
            double r;
            try
            {
                r = std::stod(v.value["mantissa"] + "E" + v.value["exponent"]);
            }
            catch (const std::exception& ex)
            {
                r = std::numeric_limits<double>::quiet_NaN();
            }
            return r;
        };

    plot.x.clear();
    plot.y.clear();

    if (result.values.size() < 4)
    {
        document->Redraw(id, false);
        return;
    }

    //the first items are the bounds of the graph
    x_left = to_double(result.values[0]);
    x_right = to_double(result.values[1]);
    y_bottom = to_double(result.values[2]);
    y_top = to_double(result.values[3]);

    //next items are the coords of the points
    for (size_t i = 4, j = 5; i < result.values.size() && j < result.values.size(); i += 2, j += 2)
    {
        plot.x.push_back(to_double(result.values[i]));
        plot.y.push_back(to_double(result.values[j]));
    }

    document->Redraw(id, false);
}

bool GraphLine::MouseLButtonHold(const int x, const int y, MouseHoldType& hold_type, ElementId& hold_id)
{
    for (int i = 0; i < elements->Get(1)->elements->Count(); ++i)
    {
        ElementPtr el = elements->Get(1)->elements->Get(i);
        if (el->elements->Count() == 0)
            return false;
        Rect r1 = el->GetAbsoluteRect();
        Rect r2 = el->elements->Get(0)->GetAbsoluteRect();
        Rect r{r1.left, r1.top, r2.left - r1.left, r2.GetBottom() - r1.top};
        if (r.IsPointInside(x, y))
        {
            mouse_l_button_pos = i;
            hold_type = MouseHoldType::PLOT_FORMAT_DIALOG;
            hold_id = id;
            return true;
        }
    }
    return false;
}

std::string GraphLine::ToHtml() const
{
    std::string expr;
    for (int i = 0; i < GetExpression()->elements->Count(); ++i)
    {
        const auto& el = GetExpression()->elements->Get(i);
        if (!el->IsVisible())
            continue;
        expr += "<p><span style=\"color: " + plots[i].format.color.ToHex() + ";\">█&nbsp;</span>" + el->ToHtml() + "</p>";
    }

    std::string image_base64;
    GetImage(image_base64);
    std::string s =
        "<table>"
            "<tr>"
                "<td style=\"height:100%; vertical-align:top;\">"
                    "<table style=\"height:100%;\">"
                        "<tr>"
                            "<td style=\"vertical-align:top;text-align:right;\">" + GetYTop()->ToHtml() + "</td>"
                        "</tr>"
                        "<tr>"
                            "<td style=\"vertical-align:middle;\">" + expr + "</td>"
                        "</tr>"
                        "<tr>"
                            "<td style=\"vertical-align:bottom;text-align:right\">" + GetYBottom()->ToHtml() + "</td>"
                        "</tr>"
                    "</table>"
                "</td>"
                "<td>"
                    "<img src=\"data:image/png;base64," + image_base64 + "\">"
                "</td>"
            "</tr>"
            "<tr>"
                "<td>"
                "</td>"
                "<td style=\"vertical-align:top;\">"
                    "<table style=\"width:100%;\">"
                        "<tr>"
                            "<td style=\"vertical-align:top;text-align:left;\">" + GetXLeft()->ToHtml() + "</td>"
                            "<td style=\"vertical-align:top;text-align:center\">" + GetVariable()->ToHtml() + "</td>"
                            "<td style=\"vertical-align:top;text-align:right;\">" + GetXRight()->ToHtml() + "</td>"
                        "</tr>"
                    "</table>"
                "</td>"
            "</tr>"
       "</table>";
    return s;
}

std::u32string GraphLine::ToText() const
{
    std::u32string s = U"graph_line(";
    for (int i = 0; i < elements->Count(); ++i)
    {
        s += elements->Get(i)->ToText();
        if (i < elements->Count() - 2)
            s += U",";
    }
    s += U")";
    return s;
}

void GraphLine::ToParserString(ParserString& str)
{
    str.Add(id, U"graph_line(");
    if (elements->Count() > 0)
        elements->Get(0)->ToParserString(str);
    str.Add(id, U",");
    if (elements->Count() < 7)
    {
        str.Add(id, U")");
        return;
    }
    elements->Get(2)->ToParserString(str);
    str.Add(id, U",");
    elements->Get(3)->ToParserString(str);
    str.Add(id, U",");
    elements->Get(4)->ToParserString(str);
    str.Add(id, U",");
    elements->Get(5)->ToParserString(str);
    str.Add(id, U",");
    elements->Get(6)->ToParserString(str);
    str.Add(id, U")");

    if (elements->Count() > 0)
    {
        int start = str.Length();
        elements->Get(0)->ToParserString(str);
        str.Annotate(id, start, str.Length());
    }
}

void GraphLine::GetPlotFormat(const int pos, Color& color, uint& width)
{
    while (plots.size() < pos + 1)
    {
        std::string guid = boost::uuids::to_string(boost::uuids::random_generator()());
        plots.push_back(Plot{guid, default_colors[plots.size() % default_colors.size()]});
    }
    color = plots[pos].format.color;
    width = plots[pos].format.width;
}

void GraphLine::GetPlotFormat(PlotFormat& format)
{
    Color color;
    uint width = 1;
    GetPlotFormat(mouse_l_button_pos, color, width);
    format.color = color;
    format.width = width;
}

void GraphLine::SetPlotFormat(const PlotFormat& format)
{
    if (mouse_l_button_pos >= plots.size())
        return;
    plots[mouse_l_button_pos].format.color = format.color;
    plots[mouse_l_button_pos].format.width = format.width;

    ElementPtr p = elements->Get(1)->elements->Get(mouse_l_button_pos);
    if (!p)
        return;
    StringFormatPtr f = GetStringFormat();
    ((CodeParagraph<>*)p.get())->SetMarker(U"█", document->GetStringFormat(f->family, f->size, f->bold, f->italic, f->underline, f->strikethrough, 
        f->subscript, f->superscript, format.color, f->text_bg_color, f->text_bg_selection_color));
}

}
