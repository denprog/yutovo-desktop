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

//Solve an array
TEST_F(SolverArrayRealTest, solver1)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("2.3", true);
    document.InsertComma(true);
    document.InsertString("4.56", true);
    document.InsertCloseSquareBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::ARRAY_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mo>[</mo>"\
                            "<mi>1</mi>"\
                            "<mo>,</mo>"\
                            "<mi>2.3</mi>"\
                            "<mo>,</mo>"\
                            "<mi>4.56</mi>"\
                            "<mo>]</mo>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                "<mo>[</mo>"\
                                "<mi>1.</mi>"\
                                "<mo>,</mo>"\
                                "<mi>2.3</mi>"\
                                "<mo>,</mo>"\
                                "<mi>4.56</mi>"\
                                "<mo>]</mo>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Empty array
TEST_F(SolverArrayRealTest, solver2)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertOpenSquareBracket(true);
    document.InsertCloseSquareBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::ARRAY_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mo>[</mo>"\
                            "<mo>]</mo>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                "<mo>[</mo>"\
                                "<mo>]</mo>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Sum of array
TEST_F(SolverArrayRealTest, solver3)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertSum(true);
    document.InsertString(U"n", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"1", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"5", true);
    document.MoveCaretRight(false);

    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("2", true);
    document.InsertComma(true);
    document.InsertString("3", true);
    document.InsertComma(true);
    document.InsertString("4", true);
    document.InsertComma(true);
    document.InsertString("5", true);
    document.InsertCloseSquareBracket(true);
    document.WaitTask(document.MoveCaretRight(false));

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    ASSERT_TRUE(document.ToText() == U"sum(n=1,5,[1,2,3,4,5])=[5.,10.,15.,20.,25.]") << ToBasicString(document.ToText());
}

//Array as variable
TEST_F(SolverArrayRealTest, solver4)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1.23", true);
    document.InsertComma(true);
    document.InsertString("2", true);
    document.InsertPower(true);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.InsertComma(true);
    document.InsertMinus(true);
    document.InsertString("55", true);
    document.InsertComma(true);
    document.InsertString("4", true);
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();

    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertSum(true);
    document.InsertString(U"n", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"size", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString(U"d", true);
    document.InsertCloseRoundBracket(true);
    document.InsertMinus(true);
    document.InsertString(U"1", true);
    document.MoveCaretRight(false);
    document.InsertString(U"d", true);
    document.InsertSubscript(true);
    document.InsertString(U"n", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    ASSERT_TRUE(document.ToText() == 
        U"d=[1.23,pow(2,3),-55,4]\n"\
        "sum(n=0,size(d)-1,d{n})=-41.77"
        ) << ToBasicString(document.ToText());
}

//Product with arrays
TEST_F(SolverArrayRealTest, solver5)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1.2", true);
    document.InsertComma(true);
    document.InsertString("77", true);
    document.WaitTask(document.InsertCloseSquareBracket(true));

    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("e", true);
    document.InsertAssignment(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("44", true);
    document.InsertComma(true);
    document.InsertString("7", true);
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();

    document.MoveCaretRight(false);
    document.WaitTask(document.InsertParagraph(true));

    document.InsertProduct(true);
    document.InsertString("i", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"size", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString(U"d", true);
    document.InsertCloseRoundBracket(true);
    document.InsertMinus(true);
    document.InsertString(U"1", true);
    document.MoveCaretRight(false);
    document.InsertString(U"d", true);
    document.InsertSubscript(true);
    document.InsertString(U"i", true);
    document.MoveCaretRight(false);
    document.InsertMultiply(true);
    document.InsertString(U"e", true);
    document.InsertSubscript(true);
    document.InsertString(U"i", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    ASSERT_TRUE(document.ToText() == 
        U"d=[1.2,77]\n"\
        U"e=[44,7]\n"\
        "prod(i=0,size(d)-1,d{i}*e{i})=28459.2"
        ) << ToBasicString(document.ToText());
}

//Array with units
TEST_F(SolverArrayRealTest, solver6)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("4м", true);
    document.InsertComma(true);
    document.InsertString("7с", true);
    document.InsertCloseSquareBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::ARRAY_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"[4м,7с]=[4.м,7.с]"
        ) << ToBasicString(document.ToText());
}

//Set precision and exponential threshold
TEST_F(SolverArrayRealTest, solver7)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("123.324324325", true);
    document.InsertComma(true);
    document.InsertString("34.45435345", true);
    document.InsertCloseSquareBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::ARRAY_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"[123.324324325,34.45435345]=[123.324,34.454]"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetPrecision({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 6, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"[123.324324325,34.45435345]=[123.324324,34.454353]"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetExp({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 1, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"[123.324324325,34.45435345]=[1.233243*pow(10,2),3.445435*pow(10,1)]"
        ) << ToBasicString(document.ToText());
}

//Set precision and exponential threshold
TEST_F(SolverArrayRealTest, solver8)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("123.3245324325", true);
    document.InsertComma(true);
    document.InsertString("34.454535345", true);
    document.InsertCloseSquareBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"[123.3245324325,34.454535345]=[123.325,34.455]"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetPrecision({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 6, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"[123.3245324325,34.454535345]=[123.324532,34.454535]"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetExp({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 1, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"[123.3245324325,34.454535345]=[1.233245*pow(10,2),3.445454*pow(10,1)]"
        ) << ToBasicString(document.ToText());
}

//Check correct error
TEST_F(SolverArrayRealTest, errors1)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("2", true);
    document.InsertComma(true);
    document.InsertString("3", true);
    document.InsertCloseSquareBracket(true);
    document.InsertParagraph(true);

    document.WaitTask(document.InsertSum(true));
    document.InsertString("n", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.InsertString("d", true);
    document.InsertSubscript(true);
    document.InsertString("n", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::ARRAY_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);
    ASSERT_TRUE(document.ToText() == 
        U"d=[1,2,3]\n"\
        U"sum(n=1,3,d{n})=Argument is over"
        ) << ToBasicString(document.ToText());
}

}
