/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"
#include "formulas/graph.h"
#include <atomic>
#include <thread>

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

//2D graph
TEST_F(FormulaTest, graphs1)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    std::this_thread::sleep_for(100ms);
    std::string image_base64;
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    ((GraphLine*)el.get())->GetImage(image_base64);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<table>"
                            "<tr>"
                                "<td style=\"height:100%; vertical-align:top;\">"
                                    "<table style=\"height:100%;\">"
                                        "<tr>"
                                            "<td style=\"vertical-align:top;text-align:right;\">"
                                                "<mrow>"
                                                    "<mi></mi>"
                                                "</mrow>"
                                            "</td>"
                                        "</tr>"
                                        "<tr>"
                                            "<td style=\"vertical-align:middle;\">"
                                                "<p>"
                                                    "<span style=\"color: #ff0000;\">█&nbsp;</span>"
                                                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                                                        "<mrow>"
                                                            "<mi></mi>"
                                                        "</mrow>"
                                                    "</math>"
                                                "</p>"
                                            "</td>"
                                        "</tr>"
                                        "<tr>"
                                            "<td style=\"vertical-align:bottom;text-align:right\">"
                                                "<mrow>"
                                                    "<mi></mi>"
                                                "</mrow>"
                                            "</td>"
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
                                            "<td style=\"vertical-align:top;text-align:left;\">"
                                                "<mrow>"
                                                    "<mi></mi>"
                                                "</mrow>"
                                            "</td>"
                                            "<td style=\"vertical-align:top;text-align:center\">"
                                                "<mrow>"
                                                    "<mi></mi>"
                                                "</mrow>"
                                            "</td>"
                                            "<td style=\"vertical-align:top;text-align:right;\">"
                                                "<mrow>"
                                                    "<mi></mi>"
                                                "</mrow>"
                                            "</td>"
                                        "</tr>"
                                    "</table>"
                                "</td>"
                            "</tr>"
                        "</table>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    ((GraphLine*)el.get())->GetImage(image_base64);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<table>"
                            "<tr>"
                                "<td style=\"height:100%; vertical-align:top;\">"
                                    "<table style=\"height:100%;\">"
                                        "<tr>"
                                            "<td style=\"vertical-align:top;text-align:right;\">"
                                                "<mrow>"
                                                    "<mi></mi>"
                                                "</mrow>"
                                            "</td>"
                                        "</tr>"
                                        "<tr>"
                                            "<td style=\"vertical-align:middle;\">"
                                                "<p>"
                                                    "<span style=\"color: #ff0000;\">█&nbsp;</span>"
                                                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                                                        "<mrow>"
                                                            "<mi></mi>"
                                                        "</mrow>"
                                                    "</math>"
                                                "</p>"
                                            "</td>"
                                        "</tr>"
                                        "<tr>"
                                            "<td style=\"vertical-align:bottom;text-align:right\">"
                                                "<mrow>"
                                                    "<mi></mi>"
                                                "</mrow>"
                                            "</td>"
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
                                            "<td style=\"vertical-align:top;text-align:left;\">"
                                                "<mrow>"
                                                    "<mi></mi>"
                                                "</mrow>"
                                            "</td>"
                                            "<td style=\"vertical-align:top;text-align:center\">"
                                                "<mrow>"
                                                    "<mi></mi>"
                                                "</mrow>"
                                            "</td>"
                                            "<td style=\"vertical-align:top;text-align:right;\">"
                                                "<mrow>"
                                                    "<mi></mi>"
                                                "</mrow>"
                                            "</td>"
                                        "</tr>"
                                    "</table>"
                                "</td>"
                            "</tr>"
                        "</table>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, graphs2)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("x", true));
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("5", true));
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("4", true));
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("x", true));
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);
    std::string image_base64;
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    GraphLine* graph = (GraphLine*)el.get();
    graph->GetImage(image_base64);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<table>"
                            "<tr>"
                                "<td style=\"height:100%; vertical-align:top;\">"
                                    "<table style=\"height:100%;\">"
                                        "<tr>"
                                            "<td style=\"vertical-align:top;text-align:right;\">"
                                                "<mrow>"
                                                    "<mi>2</mi>"
                                                "</mrow>"
                                            "</td>"
                                        "</tr>"
                                        "<tr>"
                                            "<td style=\"vertical-align:middle;\">"
                                                "<p>"
                                                    "<span style=\"color: #ff0000;\">█&nbsp;</span>"
                                                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                                                        "<mrow>"
                                                            "<mi>x</mi>"\
                                                            "<mo>+</mo>"\
                                                            "<mi>5</mi>"\
                                                        "</mrow>"
                                                    "</math>"
                                                "</p>"
                                            "</td>"
                                        "</tr>"
                                        "<tr>"
                                            "<td style=\"vertical-align:bottom;text-align:right\">"
                                                "<mrow>"
                                                    "<mo>-</mo>"\
                                                    "<mi>2</mi>"\
                                                "</mrow>"
                                            "</td>"
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
                                            "<td style=\"vertical-align:top;text-align:left;\">"
                                                "<mrow>"
                                                    "<mo>-</mo>"\
                                                    "<mi>4</mi>"\
                                                "</mrow>"
                                            "</td>"
                                            "<td style=\"vertical-align:top;text-align:center\">"
                                                "<mrow>"
                                                    "<mi>x</mi>"
                                                "</mrow>"
                                            "</td>"
                                            "<td style=\"vertical-align:top;text-align:right;\">"
                                                "<mrow>"
                                                    "<mi>4</mi>"
                                                "</mrow>"
                                            "</td>"
                                        "</tr>"
                                    "</table>"
                                "</td>"
                            "</tr>"
                        "</table>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") << 
        document.ToHtml();
    const GraphLine::Plot& plot = graph->plots[0];
    ASSERT_TRUE(graph->y_bottom == -2) << graph->y_bottom;
    ASSERT_TRUE(el->elements->Get(1)->ToText() == U"x+5");
    ASSERT_TRUE(graph->y_top == 2) << graph->y_top;
    ASSERT_TRUE(graph->x_left == -4);
    ASSERT_TRUE(el->elements->Get(4)->ToText() == U"x");
    ASSERT_TRUE(graph->x_right == 4);
    const std::vector<double> _x{-4., -3.98, -3.96};
    std::vector<double> x(plot.x.begin(), std::next(plot.x.begin(), 3));
    ASSERT_TRUE(std::equal(x.begin(), x.end(), _x.begin(), 
        [](double x, double y)
        {
            return std::fabs(x - y) < 0.01;
        })) << x[0] << x[1] << x[2];
    const std::vector<double> _y{1., 1.02, 1.04};
    std::vector<double> y(plot.y.begin(), std::next(plot.y.begin(), 3));
    ASSERT_TRUE(std::equal(y.begin(), y.end(), _y.begin(), 
        [](double x, double y)
        {
            return std::fabs(x - y) < 0.01;
        })) << y[0] << y[1] << y[2];
}

TEST_F(FormulaTest, graphs3)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    std::this_thread::sleep_for(100ms);
    
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertString("sin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("x", true));
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"graph_line(2,sin(x),-2,-2,x,2)") << ToBasicString(document.ToText());

    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(1s);
    auto el = document.FindByType({0}, ElementType::GRAPH_LINE);
    GraphLine* graph = (GraphLine*)el.get();
    const GraphLine::Plot& plot = graph->plots[0];
    ASSERT_TRUE(!plot.x.empty());
    ASSERT_TRUE(!plot.y.empty());
}

TEST_F(FormulaTest, graphs4)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("x", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"graph_line(,(x)/(x),,,,)") << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"graph_line(,xx,,,,)") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"graph_line(,(x)/(x),,,,)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"graph_line(,xx,,,,)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, graphs5)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    std::this_thread::sleep_for(100ms);
    
    document.MoveCaretRight(false);
    document.InsertString("sin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToText() == U"graph_line(,sin((x)/()),,,,)") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"graph_line(,sin(x),,,,)") << ToBasicString(document.ToText());
}

//Set graph format
TEST_F(FormulaTest, graphs6)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    std::this_thread::sleep_for(200ms);
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    GraphFormat format;
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    format.size.width = 500;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));
    format.size.width = 600;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));
    el = document.FindByType(ElementId{0}, ElementType::SHAPE);
    ASSERT_TRUE(el->rect.width == 600);

    ASSERT_TRUE(document.CanUndo());
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    el = document.FindByType(ElementId{0}, ElementType::SHAPE);
    ASSERT_TRUE(el->rect.width == 500) << el->rect.width;

    ASSERT_TRUE(document.CanRedo());
    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    el = document.FindByType(ElementId{0}, ElementType::SHAPE);
    ASSERT_TRUE(el->rect.width == 600);
}

//Change user function which is used in the graph
TEST_F(FormulaTest, graphs7)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertAssignment(true));
    document.InsertString("x", true);
    document.InsertMultiply(true);
    document.WaitTask(document.InsertString("2", true));
    document.WaitSolver();

    document.MoveCaretRight(false);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertGraph(true));
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("1", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    auto el = document.FindByType({0}, ElementType::GRAPH_LINE);
    GraphLine* graph = (GraphLine*)el.get();
    const GraphLine::Plot& plot = graph->plots[0];
    const std::vector<double> _y1{-2., -1.99, -1.98};
    std::vector<double> y(plot.y.begin(), std::next(plot.y.begin(), 3));
    ASSERT_TRUE(std::equal(y.begin(), y.end(), _y1.begin(), 
        [](double x, double y)
        {
            return std::fabs(x - y) < 0.01;
        })) << y[0] << y[1] << y[2];

    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertString("0", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    const std::vector<double> _y2{-20., -19.9, -19.8};
    y = std::vector<double>(plot.y.begin(), std::next(plot.y.begin(), 3));
    ASSERT_TRUE(std::equal(y.begin(), y.end(), _y2.begin(), 
        [](double x, double y)
        {
            return std::fabs(x - y) < 0.1;
        })) << y[0] << y[1] << y[2];
}

//Copy-paste a graph after changing its format
TEST_F(FormulaTest, graphs8)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("1", true));
    document.WaitSolver();

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    GraphFormat format;
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    format.size.width = 500;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));

    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.Paste(clipboard_json));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    el = document.FindByType(ElementId{0, 0, 1, 0}, ElementType::GRAPH_LINE);
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    ASSERT_TRUE(format.size.width == 500);
    const std::vector<double> _y{-1., -0.996, -0.992};
    GraphLine* graph = (GraphLine*)el.get();
    const GraphLine::Plot& plot = graph->plots[0];
    std::vector<double> y(plot.y.begin(), std::next(plot.y.begin(), 3));
    ASSERT_TRUE(std::equal(y.begin(), y.end(), _y.begin(), 
        [](double x, double y)
        {
            return std::fabs(x - y) < 0.01;
        })) << y[0] << y[1] << y[2];
}

//Check error marks
TEST_F(FormulaTest, graphs9)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(el->elements->Get(1)->id, start, size)) << ErrorMarks();

    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("1", true));
    document.WaitSolver();

    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertDivision(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.HasErrorMark(el->elements->Get(0)->id, start, size)) << ErrorMarks();

    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("1", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.HasErrorMark(el->elements->Get(1)->elements->Get(0)->elements->Get(0)->id, start, size)) << ErrorMarks();
}

//Check redrawing graph after undo
TEST_F(FormulaTest, graphs10)
{
    Start(600);

    document.InsertString("Graph:", true);
    document.WaitTask(document.InsertParagraph(true));

    document.WaitTask(document.InsertGraph(true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("1", true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertParagraph(true));

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();

    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    const std::vector<double> _y{-1., -0.996, -0.992};
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    GraphLine* graph = (GraphLine*)el.get();
    const GraphLine::Plot& plot = graph->plots[0];
    std::vector<double> y(plot.y.begin(), std::next(plot.y.begin(), 3));
    ASSERT_TRUE(std::equal(y.begin(), y.end(), _y.begin(), 
        [](double x, double y)
        {
            return std::fabs(x - y) < 0.01;
        })) << y[0] << y[1] << y[2];
}

//Zooming a graph
TEST_F(FormulaTest, graphs11)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    document.WaitSolver();
    std::this_thread::sleep_for(200ms);
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    GraphFormat format;
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    format.size.width = 400;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));
    format.size.width = 400;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));

    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("2", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    auto r = el->GetAbsoluteRect();
    ASSERT_TRUE(document.MouseWheel(r.left + r.width / 2, r.top + r.height / 2, Point{0, 15}, Point{0, 15}));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(el->elements->Get(0)->ToText() == U"1.") << ToBasicString(el->elements->Get(0)->ToText());
    ASSERT_TRUE(el->elements->Get(2)->ToText() == U"-1.") << ToBasicString(el->elements->Get(2)->ToText());
    ASSERT_TRUE(el->elements->Get(3)->ToText() == U"-1.");
    ASSERT_TRUE(el->elements->Get(5)->ToText() == U"1.");

    ASSERT_TRUE(document.MouseWheel(r.left + r.width / 2, r.top + r.height / 2, Point{0, 15}, Point{0, 15}));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(el->elements->Get(0)->ToText() == U"0.5");
    ASSERT_TRUE(el->elements->Get(2)->ToText() == U"-0.5");
    ASSERT_TRUE(el->elements->Get(3)->ToText() == U"-0.5");
    ASSERT_TRUE(el->elements->Get(5)->ToText() == U"0.5");

    ASSERT_TRUE(document.MouseWheel(r.left + r.width / 2, r.top + r.height / 2, Point{0, 15}, Point{0, 15}));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(el->elements->Get(0)->ToText() == U"0.25");
    ASSERT_TRUE(el->elements->Get(2)->ToText() == U"-0.25");
    ASSERT_TRUE(el->elements->Get(3)->ToText() == U"-0.25");
    ASSERT_TRUE(el->elements->Get(5)->ToText() == U"0.25");

    ASSERT_TRUE(document.MouseWheel(r.left + r.width / 2, r.top + r.height / 2, Point{0, -15}, Point{0, -15}));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(el->elements->Get(0)->ToText() == U"0.5");
    ASSERT_TRUE(el->elements->Get(2)->ToText() == U"-0.5");
    ASSERT_TRUE(el->elements->Get(3)->ToText() == U"-0.5");
    ASSERT_TRUE(el->elements->Get(5)->ToText() == U"0.5");

    ASSERT_TRUE(document.MouseWheel(r.left + r.width / 2, r.top + r.height / 2, Point{0, -15}, Point{0, -15}));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(el->elements->Get(0)->ToText() == U"1.");
    ASSERT_TRUE(el->elements->Get(2)->ToText() == U"-1.");
    ASSERT_TRUE(el->elements->Get(3)->ToText() == U"-1.");
    ASSERT_TRUE(el->elements->Get(5)->ToText() == U"1.");
}

//Zooming a graph
TEST_F(FormulaTest, graphs12)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    document.WaitSolver();
    std::this_thread::sleep_for(200ms);
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    GraphFormat format;
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    format.size.width = 400;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));
    format.size.width = 400;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));

    document.InsertString("2000", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("2000", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("2000", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("2000", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    auto r = el->GetAbsoluteRect();
    ASSERT_TRUE(document.MouseWheel(r.left + r.width / 2, r.top + r.height / 2, Point{0, -15}, Point{0, -15}));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(el->elements->Get(0)->ToText() == U"4.*pow(10,3)") << ToBasicString(el->elements->Get(0)->ToText());
    ASSERT_TRUE(el->elements->Get(2)->ToText() == U"-4.*pow(10,3)") << ToBasicString(el->elements->Get(2)->ToText());
    ASSERT_TRUE(el->elements->Get(3)->ToText() == U"-4.*pow(10,3)");
    ASSERT_TRUE(el->elements->Get(5)->ToText() == U"4.*pow(10,3)");
}

//Check the ln function
TEST_F(FormulaTest, graphs13)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    auto el = document.FindByType({0}, ElementType::GRAPH_LINE);
    GraphLine* graph = (GraphLine*)el.get();
    document.WaitTask(document.InsertString("10", true));
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("ln", true));
    document.InsertOpenRoundBracket(true);
    document.WaitTask(document.InsertString("yy", true));
    document.WaitTask(document.InsertCloseRoundBracket(true));
    std::this_thread::sleep_for(1s);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertMinus(false));
    document.WaitTask(document.InsertString("10", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertMinus(false));
    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("yy", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(4s);

    const GraphLine::Plot& plot = graph->plots[0];
    auto it = std::find_if(plot.x.begin(), plot.x.end(), 
        [](auto& x)
        {
            return x > -0.01 && x < 0.01;
        });
    const std::vector<double> _y1{std::nan(""), -4.615, -3.74};
    int p = static_cast<int>(it - plot.x.begin());
    std::vector<double> y(plot.y.begin() + p, std::next(plot.y.begin() + p, 3));
    ASSERT_TRUE(std::equal(y.begin(), y.end(), _y1.begin(), 
        [](double x, double y)
        {
            return std::fabs(x - y) < 0.1 || (std::isnan(x) && std::isnan(y));
        })) << y[0] << y[1] << y[2];
}

//Graph of two functions
TEST_F(FormulaTest, graphs14)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    GraphFormat format;
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    format.size.width = 500;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));

    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.InsertParagraph(true);
    document.InsertMinus(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("1", true));
    document.WaitSolver();
    std::this_thread::sleep_for(4s);

    const std::vector<double> _y1{-1., -0.996, -0.992};
    GraphLine* graph = (GraphLine*)el.get();
    ASSERT_TRUE(graph->plots.size() >= 1);
    GraphLine::Plot& plot = graph->plots[0];
    ASSERT_TRUE(plot.y.size() > 3);
    std::vector<double> y1(plot.y.begin(), std::next(plot.y.begin(), 3));
    ASSERT_TRUE(std::equal(y1.begin(), y1.end(), _y1.begin(), 
        [](double x, double y)
        {
            return std::fabs(x - y) < 0.01;
        })) << y1[0] << y1[1] << y1[2];

    const std::vector<double> _y2{1., 0.996, 0.992};
    ASSERT_TRUE(graph->plots.size() == 2);
    plot = graph->plots[1];
    ASSERT_TRUE(plot.y.size() > 3);
    std::vector<double> y2(plot.y.begin(), std::next(plot.y.begin(), 3));
    ASSERT_TRUE(std::equal(y2.begin(), y2.end(), _y2.begin(), 
        [](double x, double y)
        {
            return std::fabs(x - y) < 0.01;
        })) << y2[0] << y2[1] << y2[2];
}

//Load old graph
TEST_F(FormulaTest, graphs15)
{
    Start(600);

    document.Load("../../test/tests/old_graph1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    GraphFormat format;
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    format.size.width = 500;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));

    document.WaitSolver();
    std::this_thread::sleep_for(1s);

    const std::vector<double> _y{-1., -0.996, -0.992};
    GraphLine* graph = (GraphLine*)el.get();
    ASSERT_TRUE(graph->plots.size() == 1);
    GraphLine::Plot& plot = graph->plots[0];
    std::vector<double> y(plot.y.begin(), std::next(plot.y.begin(), 3));
    ASSERT_TRUE(std::equal(y.begin(), y.end(), _y.begin(), 
        [](double x, double y)
        {
            return std::fabs(x - y) < 0.01;
        })) << y[0] << y[1] << y[2];
}

//Graph moving
TEST_F(FormulaTest, graphs16)
{
    Start(600);

    document.Load("../../test/tests/graph_moving.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    auto r = el->GetAbsoluteRect();
    MouseHoldType hold_type;
    ElementId hold_id;
    ASSERT_TRUE(document.MouseLButtonDown(r.left + r.width / 2, r.top + r.height / 2, hold_type, hold_id));
    ASSERT_TRUE(document.MouseMove(r.left + r.width / 2 + 10, r.top + r.height / 2 + 10));
    document.WaitSolver();
    document.MouseLButtonUp(r.left + r.width / 2 + 10, r.top + r.height / 2 + 10);
    std::this_thread::sleep_for(2s);

    GraphLine* graph = (GraphLine*)el.get();
    ASSERT_TRUE(graph->plots.size() == 2);
    GraphLine::Plot& plot = graph->plots[0];
    ASSERT_TRUE(plot.x.size() > 0 && plot.y.size() > 0);
    plot = graph->plots[1];
    ASSERT_TRUE(plot.x.size() > 0 && plot.y.size() > 0);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Check of Undo
TEST_F(FormulaTest, graphs17)
{
    Start(600);

    document.Load("../../test/tests/graph_moving.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.ToText() == U"graph_line(10,sin(x)*5\npow(x,2),-9.393,-14.306,x,11.694)") << ToBasicString(document.ToText());
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"graph_line(10,pow(x,2),-9.393,-14.306,x,11.694)") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"graph_line(10,sin(x)*5\npow(x,2),-9.393,-14.306,x,11.694)") << ToBasicString(document.ToText());
}

//Check plot format
TEST_F(FormulaTest, graphs18)
{
    Start(600);

    document.Load("../../test/tests/graph_moving.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    auto p1 = el->elements->Get(1)->elements->Get(0);
    auto p2 = el->elements->Get(1)->elements->Get(1);
    Rect r = p1->GetAbsoluteRect();
    MouseHoldType hold_type;
    ElementId id;
    ASSERT_TRUE(document.MouseLButtonDown(r.left + 5, r.top + 5, hold_type, id));
    ASSERT_TRUE(hold_type == MouseHoldType::PLOT_FORMAT_DIALOG && id == el->id);
    r = p2->GetAbsoluteRect();
    ASSERT_TRUE(document.MouseLButtonDown(r.left + 5, r.top + 5, hold_type, id));
    ASSERT_TRUE(hold_type == MouseHoldType::PLOT_FORMAT_DIALOG && id == el->id);

    PlotFormat format;
    ASSERT_TRUE(document.GetPlotFormat(id, format));
    format.width = 2;
    format.color = Color::Blue();
    document.WaitTask(document.SetPlotFormat(id, format, true));

    format = PlotFormat{};
    ASSERT_TRUE(document.GetPlotFormat(id, format));
    ASSERT_TRUE(format.width == 2 && format.color == Color::Blue());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);

    ASSERT_TRUE(document.GetPlotFormat(id, format));
    ASSERT_TRUE(format.width == 1 && format.color == Color::Red());
}

//Graph should not recalculate because of reformating paragraph
TEST_F(FormulaTest, graphs19)
{
    Start(700);

    int width = 700;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.Load("../../test/tests/graphs19.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(4s);

    std::string image_base64_1;
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    ((GraphLine*)el.get())->GetImage(image_base64_1);

    width = 800;
    document.WaitTask(document.Resize(width, 700));
    std::this_thread::sleep_for(200ms);
    std::string image_base64_2;
    ((GraphLine*)el.get())->GetImage(image_base64_2);
    ASSERT_TRUE(image_base64_1 == image_base64_2);

    width = 300;
    document.WaitTask(document.Resize(width, 700));
    std::this_thread::sleep_for(200ms);
    ((GraphLine*)el.get())->GetImage(image_base64_2);
    ASSERT_TRUE(image_base64_1 == image_base64_2);
}

//Insert two independent graphs into the same document, concurrently render both graphs from different mglGraph instances
TEST_F(FormulaTest, graphs20)
{
    Start(600);

    document.config.solve_delay = 1000000;
    
    document.WaitTask(document.InsertGraph(true));
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertGraph(true));
    std::this_thread::sleep_for(200ms);

    auto elements = FindAllByType(document.GetElement({0}), ElementType::GRAPH_LINE);
    ASSERT_EQ(elements.size(), 2u);
    GraphLine* graph1 = (GraphLine*)elements[0].get();
    GraphLine* graph2 = (GraphLine*)elements[1].get();

    //MathGL uses global shared state, so this would crash without the mutex
    std::atomic<bool> stop{false};
    std::atomic<int> counter{0};
    auto render = 
        [&](GraphLine* g)
        {
            while (!stop)
            {
                std::string image_base64;
                g->GetImage(image_base64);
                ++counter;
            }
        };

    std::thread t1(render, graph1);
    std::thread t2(render, graph2);
    std::thread t3(render, graph1);
    std::thread t4(render, graph2);

    std::this_thread::sleep_for(2s);
    stop = true;
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    EXPECT_GT(counter, 0);
}

//Copy a graph from a code block as a PNG image and paste it into normal text
TEST_F(FormulaTest, graphs21)
{
    Start(600);

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly(
        [&](const std::vector<unsigned char>& image)
        {
            return GetImageSizeMock(image);
        });

    document.InsertCode(false, false);
    document.WaitTask(document.InsertGraph(true));
    std::this_thread::sleep_for(100ms);

    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("x", true));
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("x", true));
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("1", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    //get the graph element and extract its PNG image
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    ASSERT_TRUE(el != nullptr);
    std::vector<unsigned char> png;
    ASSERT_TRUE(document.GetGraphImage(el->id, png));
    ASSERT_FALSE(png.empty());
    ASSERT_TRUE(png.size() > 8);
    ASSERT_TRUE(png[0] == 0x89 && png[1] == 'P' && png[2] == 'N' && png[3] == 'G');

    //move caret out of the graph fields and create a new normal paragraph after the code block
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(200ms);

    //paste the PNG image into the normal text paragraph
    document.WaitTask(document.PasteImage(png));
    std::this_thread::sleep_for(500ms);
    ASSERT_TRUE(document.ToHtml().find("<img src=\"data:image/png;base64,") != std::string::npos) << document.ToHtml();

    auto el2 = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    ASSERT_TRUE(el2 != nullptr);
    auto img = document.FindByType(ElementId{0}, ElementType::IMAGE);
    ASSERT_TRUE(img != nullptr);
}

}
