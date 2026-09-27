/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"
#include "formulas/division.h"
#include "formulas/power.h"
#include "formulas/code_string.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

//Insert an empty derivative and check its initial state
TEST_F(FormulaTest, derivative1)
{
    Start(600);

    document.WaitTask(document.InsertDerivative(U"d", 1, true));
    ASSERT_TRUE(document.ToText() == U"derivative(,)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mfrac>"
                            "<mrow>"
                                "<mi>d</mi>"
                                "<mi></mi>"
                            "</mrow>"
                            "<mrow>"
                                "<mi>d</mi>"
                                "<mi></mi>"
                            "</mrow>"
                        "</mfrac>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") << document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"derivative(,)") << ToBasicString(document.ToText());
}

//Fill function and variable
TEST_F(FormulaTest, derivative2)
{
    Start(600);

    document.WaitTask(document.InsertDerivative(U"d", 1, true));
    FillFunctionAndVariable();
    ASSERT_TRUE(document.ToText() == U"derivative(f,x)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mfrac>"
                            "<mrow>"
                                "<mi>d</mi>"
                                "<mi>f</mi>"
                            "</mrow>"
                            "<mrow>"
                                "<mi>d</mi>"
                                "<mi>x</mi>"
                            "</mrow>"
                        "</mfrac>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") << document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative(f,)") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative(,)") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative(f,)") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative(f,x)") << ToBasicString(document.ToText());
}

//Insert a division inside the numerator
TEST_F(FormulaTest, derivative3)
{
    Start(600);

    document.WaitTask(document.InsertDerivative(U"d", 1, true));
    document.WaitTask(document.InsertString("2pi", true));
    document.WaitTask(document.InsertDivision(true));
    document.WaitTask(document.InsertString("1", true));
    ASSERT_TRUE(document.ToText() == U"derivative((2pi)/(1),)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml().find("<mfrac>") != std::string::npos) << document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative((2pi)/(),)") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative(2pi,)") << ToBasicString(document.ToText());
}

//Select and copy-paste
TEST_F(FormulaTest, derivative4)
{
    Start(600);

    document.WaitTask(document.InsertDerivative(U"d", 1, true));
    document.WaitTask(document.SelectAll());
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.ToText() == U"derivative(,)") << ToBasicString(document.ToText());
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == U"derivative(,)derivative(,)") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative(,)") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative(,)derivative(,)") << ToBasicString(document.ToText());
}

//Replace variable and function by deleting and retyping
TEST_F(FormulaTest, derivative5)
{
    Start(600);

    document.WaitTask(document.InsertDerivative(U"d", 1, true));
    FillFunctionAndVariable();
    ASSERT_TRUE(document.ToText() == U"derivative(f,x)") << ToBasicString(document.ToText());

    //replace the variable
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitTask(document.InsertString("y", true));
    ASSERT_TRUE(document.ToText() == U"derivative(f,y)") << ToBasicString(document.ToText());

    //replace the function
    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitTask(document.InsertString("g", true));
    ASSERT_TRUE(document.ToText() == U"derivative(g,y)") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative(,y)") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative(f,y)") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative(f,)") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative(f,x)") << ToBasicString(document.ToText());
}

//Delete around the formula
TEST_F(FormulaTest, derivative6)
{
    Start(600);

    document.WaitTask(document.InsertDerivative(U"d", 1, true));
    FillFunctionAndVariable();
    ASSERT_TRUE(document.ToText() == U"derivative(f,x)") << ToBasicString(document.ToText());

    //Backspace after the formula deletes it
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative(f,x)") << ToBasicString(document.ToText());

    //Delete before the formula deletes it
    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative(f,x)") << ToBasicString(document.ToText());
}

//Clear the variable and undo
TEST_F(FormulaTest, derivative7)
{
    Start(600);

    document.WaitTask(document.InsertDerivative(U"d", 1, true));
    FillFunctionAndVariable();
    ASSERT_TRUE(document.ToText() == U"derivative(f,x)") << ToBasicString(document.ToText());

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"derivative(f,)") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative(f,x)") << ToBasicString(document.ToText());
}

//Save and load
TEST_F(FormulaTest, derivative8)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.WaitTask(document.InsertDerivative(U"d", 1, true));
    FillFunctionAndVariable();
    ASSERT_TRUE(document.ToText() == U"derivative(f,x)") << ToBasicString(document.ToText());

    document.WaitTask(document.Save("derivative1.yut"));
    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Load("derivative1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToText() == U"derivative(f,x)") << ToBasicString(document.ToText());
}

//Partial derivative symbol
TEST_F(FormulaTest, derivative9)
{
    Start(600);

    document.WaitTask(document.InsertDerivative(U"∂", 1, true));
    FillFunctionAndVariable();
    ASSERT_TRUE(document.ToText() == U"derivative(f,x)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml().find("<mi>\u2202</mi>") != std::string::npos) << document.ToHtml();
}

//Second derivative
TEST_F(FormulaTest, derivative10)
{
    Start(600);

    document.WaitTask(document.InsertDerivative(U"d", 2, true));
    FillFunctionAndVariable();
    ASSERT_TRUE(document.ToText() == U"derivative(derivative(f,x),x)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml().find("<msup>") != std::string::npos) << document.ToHtml();
}

//Partial second derivative
TEST_F(FormulaTest, derivative11)
{
    Start(600);

    document.WaitTask(document.InsertDerivative(U"∂", 2, true));
    FillFunctionAndVariable();
    ASSERT_TRUE(document.ToText() == U"derivative(derivative(f,x),x)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml().find("<mi>\u2202</mi>") != std::string::npos) << document.ToHtml();
    ASSERT_TRUE(document.ToHtml().find("<msup>") != std::string::npos) << document.ToHtml();
}

//Changing the leading symbol removes derivative detection
TEST_F(FormulaTest, derivative12)
{
    Start(600);

    document.WaitTask(document.InsertDerivative(U"d", 1, true));
    FillFunctionAndVariable();
    ASSERT_TRUE(document.ToText() == U"derivative(f,x)") << ToBasicString(document.ToText());

    //move the caret to the numerator's "d" and replace it with "a"
    for (int i = 0; i < 8; ++i)
        document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.InsertString("a", true));
    ASSERT_TRUE(document.ToText() == U"(af)/(dx)") << ToBasicString(document.ToText());
}

//A plain division is not treated as a derivative
TEST_F(FormulaTest, derivative13)
{
    Start(600);

    document.WaitTask(document.InsertDivision(true));
    document.WaitTask(document.InsertString("a", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("b", true));
    ASSERT_TRUE(document.ToText() == U"(a)/(b)") << ToBasicString(document.ToText());
}

//Mixed partial derivative with two variables in the denominator
TEST_F(FormulaTest, derivative14)
{
    Start(600);

    Division* div = CreateMixedDerivativeDivision(U"f", {U"x", U"y"});
    document.WaitTask(document.InsertFormula(div, true));
    ASSERT_TRUE(document.ToText() == U"derivative(derivative(f,y),x)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mfrac>"
                            "<mrow>"
                                "<msup>"
                                    "<mrow>"
                                        "<mi>d</mi>"
                                    "</mrow>"
                                    "<mrow>"
                                        "<mi>2</mi>"
                                    "</mrow>"
                                "</msup>"
                                "<mi>f</mi>"
                            "</mrow>"
                            "<mrow>"
                                "<mi>d</mi>"
                                "<mi>x</mi>"
                                "<mi>d</mi>"
                                "<mi>y</mi>"
                            "</mrow>"
                        "</mfrac>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") << document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative(derivative(f,y),x)") << ToBasicString(document.ToText());
}

//Mixed partial derivative with three variables in the denominator
TEST_F(FormulaTest, derivative15)
{
    Start(600);

    Division* div = CreateMixedDerivativeDivision(U"f(x,y,z)", {U"x", U"y", U"z"});
    document.WaitTask(document.InsertFormula(div, true));
    ASSERT_TRUE(document.ToText() == U"derivative(derivative(derivative(f(x,y,z),z),y),x)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mfrac>"
                            "<mrow>"
                                "<msup>"
                                    "<mrow>"
                                        "<mi>d</mi>"
                                    "</mrow>"
                                    "<mrow>"
                                        "<mi>3</mi>"
                                    "</mrow>"
                                "</msup>"
                                "<mi>f</mi>"
                                "<mi>(</mi>"
                                "<mi>x</mi>"
                                "<mi>,</mi>"
                                "<mi>y</mi>"
                                "<mi>,</mi>"
                                "<mi>z</mi>"
                                "<mi>)</mi>"
                            "</mrow>"
                            "<mrow>"
                                "<mi>d</mi>"
                                "<mi>x</mi>"
                                "<mi>d</mi>"
                                "<mi>y</mi>"
                                "<mi>d</mi>"
                                "<mi>z</mi>"
                            "</mrow>"
                        "</mfrac>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") << document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"derivative(derivative(derivative(f(x,y,z),z),y),x)") << ToBasicString(document.ToText());
}

//Numerator order does not match the number of differentiation operators in the denominator
TEST_F(FormulaTest, derivative16)
{
    Start(600);

    Division* div = CreateMixedDerivativeDivision(1, U"f", {U"x", U"y"});
    document.WaitTask(document.InsertFormula(div, true));
    ASSERT_TRUE(document.ToText() == U"(df)/(dxdy)") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());
}

//Derivative markers merged with function/variables in single CodeStrings
TEST_F(FormulaTest, derivative17)
{
    Start(600);

    Division* div = new Division(&document);
    CodeRow<>* num = div->GetNumeratorRow();
    CodeRow<>* den = div->GetDenominatorRow();

    Power* p = new Power(num);
    CodeRow<>* base = p->GetBaseRow();
    CodeRow<>* exp = p->GetExponentRow();
    base->elements->Clear();
    exp->elements->Clear();

    ElementPtr d(new CodeString(base, U"d"));
    d->can_merge = false;
    base->elements->Add(d);

    ElementPtr n(new CodeString(exp, U"2"));
    n->can_merge = false;
    exp->elements->Add(n);

    div->AddNumerator(ElementPtr(p));

    ElementPtr func(new CodeString(num, U"g(x,y)"));
    func->can_merge = false;
    div->AddNumerator(func);

    ElementPtr den_str(new CodeString(den, U"dxdy"));
    den_str->can_merge = false;
    div->AddDenomerator(den_str);

    document.WaitTask(document.InsertFormula(div, true));
    ASSERT_TRUE(document.ToText() == U"derivative(derivative(g(x,y),y),x)") << ToBasicString(document.ToText());
}

//Derivative at a point with an evaluation bar subscript
TEST_F(FormulaTest, derivative_at_point1)
{
    Start(600);

    document.WaitTask(document.InsertDerivative(U"d", 1, true));
    FillFunctionAndVariable();

    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertEvaluationBarSubscript(true));

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("x", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("2", true));

    ASSERT_TRUE(document.ToText() == U"derivative(f)[x=2]") << ToBasicString(document.ToText());
}

}
