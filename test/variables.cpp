/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace yutovo_solver;
using namespace std::chrono_literals;

//Changing value of a variable and resolve a dependent expression
TEST_F(VariablesTest, variables1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("4", true);
    document.WaitSolver();
    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.InsertString("5", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>d</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mi>4</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>d</mi>"\
                                "<mo>+</mo>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>9.</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertString("2", true));
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>d</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mi>42</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>d</mi>"\
                                "<mo>+</mo>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>47.</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Insert unknown variable and add it above later
TEST_F(VariablesTest, variables2)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.MoveCaretUp(false));
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>d</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>d</mi>"\
                                "<mo>+</mo>"\
                                "<mi>4</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>9.</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretDown(false);
    for (int i = 0; i < 4; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>d</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>d</mi>"\
                                "<mo>+</mo>"\
                                "<mi>54</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>59.</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Delete a paragraph with a variable
TEST_F(VariablesTest, variables3)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("4", true);
    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.InsertString("5", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>d</mi>"\
                                "<mo>+</mo>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>Unknown identifier</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>d</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mi>4</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>d</mi>"\
                                "<mo>+</mo>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>9.</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Redefine a variable
TEST_F(VariablesTest, variables4)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("4", true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.InsertString("5", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("45", true);
    document.InsertPlus(true);
    document.InsertString("d", true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.WaitTask(document.MoveCaretRight(false));
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.InsertString("7", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=4\n" \
        U"d+5=9.\n" \
        U"d=45+d\n" \
        U"d+7=56."
        ) << ToBasicString(document.ToText());
    
    for (int i = 0; i < 3; ++i)
        document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertString("2", true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"d=42\n" \
        U"d+5=47.\n" \
        U"d=45+d\n" \
        U"d+7=94."
        ) << ToBasicString(document.ToText());
}

//Recalculate after changing a variable
TEST_F(VariablesTest, variables5)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.InsertDivision(true);
    document.InsertString("4", true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=(5)/(4)\n" \
        U"d=1.25"
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=(5)/()\n" \
        U"d=Unknown identifier"
        ) << ToBasicString(document.ToText());
}

//Variable with a long number
TEST_F(VariablesTest, variables6)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("2244444444444", true);
    document.WaitSolver();

    document.WaitTask(document.MoveCaretRight(false));
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=2244444444444\n" \
        U"d=2.244*pow(10,12)"
        ) << ToBasicString(document.ToText());
}

//Insert a paragraph in a variable's value
TEST_F(VariablesTest, variables7)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("2345", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"d=2345\n" \
        U"d=2345."
        ) << ToBasicString(document.ToText());

    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.InsertParagraph(true);
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"d=234\n" \
        U"5\n" \
        U"d=234."
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"d=2345\n" \
        U"d=2345."
        ) << ToBasicString(document.ToText());
}

//Define a variable with an empty placeholder
TEST_F(VariablesTest, errors1)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.WaitTask(document.InsertAssignment(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"d=") << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 2}, start, size));
    ASSERT_TRUE(start == 0 && size == 1);

    document.InsertString("4", true);
    document.WaitTask(document.InsertPlus(true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"d=4+") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 2}, start, size));
    ASSERT_TRUE(start == 0 && size == 2);
}

//Define a variable with unknown variable
TEST_F(VariablesTest, errors2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("4", true);
    document.InsertPlus(true);
    document.InsertString("t", true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"d=4+t") << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(!document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0, 2}, start, size));
}

//Define a variable with unknown variable
TEST_F(VariablesTest, errors3)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("4", true);
    document.InsertPlus(true);
    document.InsertString("t", true);
    document.WaitSolver();
    
    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=4+t\n" \
        U"d+5=Unknown identifier"
        ) << ToBasicString(document.ToText());
    std::this_thread::sleep_for(1s);
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 2}, start, size));
    ASSERT_TRUE(start == 0 && size == 1);
}

//Define a variable with recursion
TEST_F(VariablesTest, errors4)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("4", true);
    document.InsertPlus(true);
    document.InsertString("d", true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=4+d\n" \
        U"d+5=Unknown identifier"
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 2}, start, size));
    ASSERT_TRUE(start == 0 && size == 1);
}

//Check a error position
TEST_F(VariablesTest, errors5)
{
    Start(600);

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::English, true));
    document.InsertCode(false, true);
    document.InsertString("R", true);
    document.InsertAssignment(true);
    document.InsertString("1", true);
    document.InsertString(" ", true);
    document.InsertString("см", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("R", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);
    ASSERT_TRUE(document.ToText() == 
        U"R=1см\n" \
        U"R=Unknown identifier"
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 2}, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 0 && size == 2);
}

//Check error mark
TEST_F(VariablesTest, errors6)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.InsertString("5", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"d+5=Unknown identifier"
        ) << ToBasicString(document.ToText());
    auto el = document.FindByString({0}, U"d");
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(el->id, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 0 && size == 1);
    el = document.FindByString({0}, U"5");
    ASSERT_TRUE(!document.HasErrorMark(el->id, start, size)) << ErrorMarks();
    ASSERT_TRUE(document.HasErrorMark(el->parent->id, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 0 && size == 1);
    ASSERT_TRUE(!document.HasErrorMark(el->parent->parent->id, start, size)) << ErrorMarks();
    ASSERT_TRUE(!document.HasErrorMark(el->parent->parent->parent->id, start, size)) << ErrorMarks();
}

//Rational variables
TEST_F(VariablesTest, variables8)
{
    Start(600);

    document.GetConfig(config);
    ResultType order1[4] = {ResultType::RATIONAL, ResultType::INTEGER, ResultType::REAL, ResultType::COMPLEX};
    std::copy(order1, order1 + 4, config.auto_result.results_order);
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertDivision(true);
    document.InsertString("1", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitSolver();

    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"d=(1)/(3)\n" \
        U"d=(1)/(3)"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();

    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertDivision(true);
    document.InsertString("4", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("5", true);
    document.MoveCaretRight(false);
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"d=(1)/(3)\n" \
        U"d=(4)/(5)\n" \
        U"d=(4)/(5)"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

//Variable with equation
TEST_F(VariablesTest, variables9)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("a", true);
    document.InsertAssignment(true);
    document.InsertString("12", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"a=12=12."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("a", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"a=12=12.\n" \
        U"a=12."
        ) << ToBasicString(document.ToText());
}

//A variable inside a division with a unit
TEST_F(VariablesTest, variables10)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("a", true);
    document.InsertAssignment(true);
    document.InsertString("2", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("a", true);
    document.InsertDivision(true);
    document.InsertString("4", true);
    document.MoveCaretRight(false);
    document.InsertString("m", true);

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"a=2\n" \
        U"(a)/(4)m=5.dm"
        ) << ToBasicString(document.ToText());
}

//A variable inside a division with a unit
TEST_F(VariablesTest, variables11)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("2", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("1", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=2\n" \
        U"d=1\n" \
        U"d=1."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretUp(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=2\n" \
        U"d=2."
        ) << ToBasicString(document.ToText());
}

//Remove a second definition of a variable
TEST_F(VariablesTest, variables12)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("6", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=5\n" \
        U"d=6\n" \
        U"d=6."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretUp(false);
    document.MoveCaretHome(false);
    document.DeleteElements(false, true);
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=5\n" \
        U"d=5."
        ) << ToBasicString(document.ToText());
}

//Remove a second definition of a variable
TEST_F(VariablesTest, variables13)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("b", true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("c", true);
    document.InsertAssignment(true);
    document.InsertString("b", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("c", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"b=5\n" \
        U"c=b\n" \
        U"c=5."
        ) << ToBasicString(document.ToText());

    document.MoveCaretUp(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"\n" \
        U"c=b\n" \
        U"c=Unknown identifier"
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 1, 0, 0, 2, 0}, start, size)) << ErrorMarks();;
    ASSERT_TRUE(start == 0 && size == 1);
    
    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"b=5\n" \
        U"c=b\n" \
        U"c=5."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

//Remove a code block with a variable
TEST_F(VariablesTest, variables14)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("a", true);
    document.InsertAssignment(true);
    document.InsertString("1", true);
    document.WaitSolver();

    document.MoveCaretToDocumentEnd(false);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("b", true);
    document.InsertAssignment(true);
    document.InsertString("2", true);
    document.WaitSolver();

    document.MoveCaretToDocumentEnd(false);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("c", true);
    document.InsertAssignment(true);
    document.InsertString("a", true);
    document.InsertMultiply(true);
    document.InsertString("b", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("c", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"a=1\n" \
        U"b=2\n" \
        U"c=a*b\n" \
        U"c=2."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"a=1\n" \
        U"\n" \
        U"c=a*b\n" \
        U"c=Unknown identifier"
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 2, 0, 0, 0, 0, 0, 2, 2}, start, size)) << ErrorMarks();

    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"a=1\n" \
        U"c=a*b\n" \
        U"c=Unknown identifier"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 1, 0, 0, 0, 0, 0, 2, 2}, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 0 && size == 1);

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"a=1\n" \
        U"b=2\n" \
        U"c=a*b\n" \
        U"c=2."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

//Insert a variable before its using
TEST_F(VariablesTest, variables15)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("b", true);
    document.InsertAssignment(true);
    document.InsertString("1", true);
    document.WaitSolver();

    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.InsertParagraph(true);
    document.MoveCaretUp(false);
    document.InsertString("b", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"b=Unknown identifier\n" \
        U"b=1"
        ) << ToBasicString(document.ToText());
}

//Insert a variable before its using
TEST_F(VariablesTest, variables16)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("a", true);
    document.InsertAssignment(true);
    document.InsertString("1", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("F", true);
    document.InsertAssignment(true);
    document.InsertString("a", true);
    document.InsertMultiply(true);
    document.InsertString("b", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("F", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"a=1\n" \
        U"F=a*b\n" \
        U"F=Unknown identifier"
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 1, 0, 0, 2, 2}, start, size)) << ErrorMarks();

    document.MoveCaretUp(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.InsertParagraph(true));
    document.MoveCaretUp(false);
    document.InsertString("b", true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"a=1\n" \
        U"b=5\n" \
        U"F=a*b\n" \
        U"F=5."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

//Insert a variable inside a code block before
TEST_F(VariablesTest, variables17)
{
    Start(600);
    
    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.InsertString("h", true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.WaitSolver();

    document.MoveCaretToDocumentEnd(false);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("E", true);
    document.InsertAssignment(true);
    document.InsertString("m", true);
    document.InsertMultiply(true);
    document.InsertString("h", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("E", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"h=5\n" \
        U"E=m*h\n" \
        U"E=Unknown identifier"
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 1, 0, 0, 0, 0, 0, 2, 0}, start, size)) << ErrorMarks();

    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.MoveCaretUp(false));
    document.InsertString("m", true);
    document.InsertAssignment(true);
    document.InsertString("2", true);
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"m=2\n" \
        U"h=5\n" \
        U"E=m*h\n" \
        U"E=10."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

//Move a code block with a variable
TEST_F(VariablesTest, variables18)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("p", true);
    document.InsertAssignment(true);
    document.InsertString("1", true);
    document.WaitSolver();

    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertCode(false, true);
    document.InsertString("L", true);
    document.InsertAssignment(true);
    document.InsertString("p", true);
    document.WaitSolver();
    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("L", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"p=1\n" \
        U"L=p\n" \
        U"L=1."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();

    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertCode(false, true);
    document.InsertString("L", true);
    document.InsertAssignment(true);
    document.InsertString("p", true);
    document.WaitSolver();
    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("L", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"p=1\n" \
        U"L=p\n" \
        U"L=1.\n" \
        U"L=p\n" \
        U"L=1."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"\n" \
        U"p=1\n" \
        U"L=p\n" \
        U"L=1.\n" \
        U"L=p\n" \
        U"L=1."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

//Insert a paragraph over a code block with a variable
TEST_F(VariablesTest, variables19)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("v", true);
    document.InsertAssignment(true);
    document.InsertString("3", true);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("v", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.InsertParagraph(true));
    document.InsertCode(false, true);
    document.InsertString("v", true);
    document.InsertAssignment(true);
    document.InsertString("2", true);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("v", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"v=3\n" \
        U"v=3.\n" \
        U"v=2\n" \
        U"v=2."
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"\n" \
        U"v=3\n" \
        U"v=3.\n" \
        U"v=2\n" \
        U"v=2."
        ) << ToBasicString(document.ToText());
}

//Check error marks
TEST_F(VariablesTest, variables20)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertString("Example 1", true);
    document.InsertParagraph(true);
    document.InsertString("Вычисление", true);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("Р_З", true);
    document.InsertAssignment(true);
    document.InsertString("6370", true);
    document.InsertString(" ", true);
    document.WaitTask(document.InsertString("км", true));
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("v", true);
    document.InsertAssignment(true);
    document.InsertSquareRoot(true);
    document.InsertString("Р_З", true);
    document.InsertMultiply(true);
    document.WaitTask(document.InsertString("g_a", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    
    document.WaitTask(document.MoveCaretEnd(false));
    document.InsertParagraph(true);
    document.InsertString("v", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.InsertParagraph(true);
    document.InsertString("Example 2", true);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("String", true));
    std::this_thread::sleep_for(3s);
    ASSERT_TRUE(document.ToText() == 
        U"Example 1\n" \
        U"Вычисление\n" \
        U"Р_З=6370км\n" \
        U"v=sqrt(Р_З*g_a)\n" \
        U"v=7.904(км)/(с)\n" \
        U"Example 2\n"
        U"String"
        ) << ToBasicString(document.ToText());
    
    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"Example 1\n" \
        U"Вычисление\n" \
        U"Р_З=6370\n" \
        U"v=sqrt(Р_З*g_a)\n" \
        U"v=Units are incompatible\n" \
        U"Example 2\n"
        U"String"
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 2, 0, 0, 1, 0, 0, 2}, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 0 && size == 1);

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.MoveCaretEnd(true);
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"Вычисление\n" \
        U"Р_З=6370\n" \
        U"v=sqrt(Р_З*g_a)\n" \
        U"v=Units are incompatible\n" \
        U"Example 2\n"
        U"String"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 1, 0, 0, 1, 0, 0, 2}, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 0 && size == 1);

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

//Check error marks
TEST_F(VariablesTest, variables21)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.SetLocale(yutovo_calculator::Language::Russian, true);
    document.InsertString("Example 1", true);
    document.InsertParagraph(true);
    document.InsertString("Вычисление", true);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("Р_З", true);
    document.InsertAssignment(true);
    document.InsertString("6370", true);
    document.InsertString(" ", true);
    document.InsertString("км", true);
    document.WaitSolver();
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("Р_З", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();

    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"Example 1\n" \
        U"Вычисление\n" \
        U"Р_З=6370к\n" \
        U"Р_З=Unknown identifier" \
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 2, 0, 0, 0, 0, 0}, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 2 && size == 1);
}

//Remove a variable and update all the equations below
TEST_F(VariablesTest, variables22)
{
    Start(500);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.InsertString("a", true);
    document.InsertAssignment(true);
    document.InsertString("2", true);
    document.InsertParagraph(true);
    document.InsertString("123a", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);
    ASSERT_TRUE(document.ToText() == 
        U"a=2\n" \
        U"123a=246."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretToDocumentEnd(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.Paste(clipboard_json));
    document.WaitTask(document.Paste(clipboard_json));
    document.WaitTask(document.Paste(clipboard_json));
    document.WaitTask(document.Paste(clipboard_json));

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(4s);
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"123a=Unknown identifier"\
        U"a=2\n" \
        U"123a=246."\
        U"a=2\n" \
        U"123a=246."\
        U"a=2\n" \
        U"123a=246."\
        U"a=2\n" \
        U"123a=246."
        ) << ToBasicString(document.ToText());
}

//Remove a code block with a variable and update all the equations below
TEST_F(VariablesTest, variables23)
{
    Start(500);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.SetLocale(yutovo_calculator::Language::Russian, true);
    document.InsertCode(false, true);
    document.InsertString("a", true);
    document.InsertAssignment(true);
    document.InsertString("2", true);
    document.InsertParagraph(true);
    document.InsertString("123a", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"a=2\n" \
        U"123a=246."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretToDocumentEnd(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.Paste(clipboard_json));
    document.WaitTask(document.Paste(clipboard_json));
    document.WaitTask(document.Paste(clipboard_json));
    document.WaitTask(document.Paste(clipboard_json));

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(4s);
    ASSERT_TRUE(document.ToText() == 
        U"a=2\n" \
        U"123a=246."\
        U"a=2\n" \
        U"123a=246."\
        U"a=2\n" \
        U"123a=246."\
        U"a=2\n" \
        U"123a=246."
        ) << ToBasicString(document.ToText());
}

//Insert a string between code blocks with variables and update all the equations below
TEST_F(VariablesTest, variables24)
{
    Start(500);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.SetLocale(yutovo_calculator::Language::Russian, true);
    document.InsertCode(false, true);
    document.InsertString("a", true);
    document.InsertAssignment(true);
    document.InsertString("2", true);
    document.InsertParagraph(true);
    document.InsertString("123a", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"a=2\n" \
        U"123a=246."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretToDocumentEnd(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.Paste(clipboard_json));
    document.WaitTask(document.Paste(clipboard_json));
    document.WaitTask(document.Paste(clipboard_json));
    document.WaitTask(document.Paste(clipboard_json));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    document.WaitTask(document.InsertString("String", true));
    document.WaitSolver();
    std::this_thread::sleep_for(4s);
    ASSERT_TRUE(document.ToText() == 
        U"a=2\n" \
        U"123a=246."\
        U"String"\
        U"a=2\n" \
        U"123a=246."\
        U"a=2\n" \
        U"123a=246."\
        U"a=2\n" \
        U"123a=246."\
        U"a=2\n" \
        U"123a=246."
        ) << ToBasicString(document.ToText());
}

//Check the iteration variable doesn't have value outside of sum
TEST_F(VariablesTest, variables25)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.WaitTask(document.InsertSum(true));
    document.InsertString(U"p", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"10", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString(U"p", true));
    document.WaitTask(document.MoveCaretRight(false));

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"sum(p=0,10,p)=55."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretHome(false);
    for (int i = 0; i < 3; ++i)
        document.MoveCaretRight(false);

    document.WaitTask(document.DeleteElements(false, true));
    document.InsertString(U"t", true);
    for (int i = 0; i < 8; ++i)
        document.WaitTask(document.MoveCaretRight(false));
    
    document.WaitTask(document.DeleteElements(false, true));
    document.InsertString(U"t", true);
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"sum(t=0,10,t)=55."
        ) << ToBasicString(document.ToText());

    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertParagraph(true));

    document.InsertString(U"p", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"sum(t=0,10,t)=55.\n"\
        "p=Unknown identifier"
        ) << ToBasicString(document.ToText());

    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString(U"t", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"sum(t=0,10,t)=55.\n"\
        "p=Unknown identifier\n"\
        "t=Unknown identifier"
        ) << ToBasicString(document.ToText());
}

//Recalculate after changing a variable with a subscript
TEST_F(VariablesTest, variables26)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("ф", true);
    document.InsertSubscript(true);
    document.InsertString("1", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.InsertParagraph(true);
    document.InsertString("ф", true);
    document.InsertSubscript(true);
    document.InsertString("1", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"ф{1}=5\n" \
        U"ф{1}=5."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitTask(document.InsertString("7", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"ф{1}=7\n" \
        U"ф{1}=7."
        ) << ToBasicString(document.ToText());
}

//Check no result
TEST_F(VariablesTest, variables27)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("a", true);
    document.WaitTask(document.InsertAssignment(true));
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"a=5=Expression expected"
        ) << ToBasicString(document.ToText());
}

//Check variables after merging paragraphs
TEST_F(VariablesTest, variables28)
{
    Start(600);

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.InsertString("k", true);
    document.WaitTask(document.InsertAssignment(true));
    document.WaitTask(document.InsertString("1м", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.InsertParagraph(true);
    document.InsertString("k", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"k=1м\n"\
        U"k=1.м"
        ) << ToBasicString(document.ToText());

    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.DeleteElements(false, true));

    ElementPtr el = document.FindByString({0, 0, 0}, U"1.");
    yutovo_calculator::Unit unit(U"см");
    document.WaitTask(document.SetUnit(el->parent->parent->id, unit, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"k=1мk=100.см"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"k=1мk=1.м"
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"k=1мk=100.см"
        ) << ToBasicString(document.ToText());
}

//Check variables after merging paragraphs
TEST_F(VariablesTest, variables29)
{
    Start(600);

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.InsertString("k", true);
    document.WaitTask(document.InsertAssignment(true));
    document.WaitTask(document.InsertString("1м", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.InsertParagraph(true);
    document.InsertString("k", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"k=1м\n"\
        U"k=1.м"
        ) << ToBasicString(document.ToText());

    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == 
        U"k=1мk=1.м"
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == 
        U"k=1мk"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"k=1мk=1.м"
        ) << ToBasicString(document.ToText());
}

//Check variables with subscript
TEST_F(VariablesTest, variables30)
{
    Start(600);

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.InsertString("q", true);
    document.InsertSubscript(true);
    document.InsertString("e", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertAssignment(true));
    document.WaitTask(document.InsertString("1м", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.InsertParagraph(true);
    document.InsertString("q", true);
    document.InsertSubscript(true);
    document.InsertString("e", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"q{e}=1м\n"\
        U"q{e}=1.м"
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretUp(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"{e}=1м\n"\
        U"q{e}=Unknown identifier"
        ) << ToBasicString(document.ToText());
}

//Recalculate after changing a variable
TEST_F(VariablesTest, variables31)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(1ms);

    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("d", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"d=5\n" \
        U"d=5."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretUp(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(false));
    document.InsertString("1", true);
    document.WaitTask(document.InsertString("2", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"d12=5\n" \
        U"d=Unknown identifier"
        ) << ToBasicString(document.ToText());
}

//Code should not recalculate because of changing a string above its code block
TEST_F(VariablesTest, variables32)
{
    Start(600);

    document.InsertString("String", true);
    document.WaitTask(document.InsertParagraph(true));
    
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("123", true));
    document.WaitSolver();
    document.WaitTask(document.InsertParagraph(true));

    document.InsertString("d", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"String\n"\
        U"d=123\n"\
        U"d=123."
        ) << ToBasicString(document.ToText());

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretRight(true);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.DeleteElements(false, true));
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == 
        U"String\n"\
        U"d=123\n"\
        U"d=123."
        ) << ToBasicString(document.ToText());
}

//Code should not recalculate because of reformating paragraph of a variable
TEST_F(VariablesTest, variables33)
{
    Start(500);

    int width = 500;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.Load("../../test/tests/variables33.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(4s);

    width = 450;
    document.WaitTask(document.Resize(450, 700));
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"Yutovo es una calculadora poderosa v=234con la visualización y edición gráfica habitual de operaciones matemáticas "
        "dentro de un editor de texto. Con él, puede hacer varios cálculos v=234.combinando cálculos y texto en un solo documento, a saber:"
        ) << ToBasicString(document.ToText());
}

}
