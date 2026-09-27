/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include <yutovo-calculator/math_helper.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace yutovo_solver;
using namespace std::chrono_literals;

TEST_F(SolverIntegerTest, solver1)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.integer_result.show_notation = false;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("2345", true);
    document.InsertPlus(true);
    document.InsertString("35", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2345</mi>"\
                            "<mo>+</mo>"\
                            "<mi>35</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>2380</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>2345</mi>"\
                        "<mo>+</mo>"\
                        "<mi>35</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 2})) << document.GetEditorState().ToString();
}

TEST_F(SolverIntegerTest, solver2)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.integer_result.show_notation = false;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertMinus(true);
    document.InsertString("23", true);
    document.InsertMultiply(true);
    document.InsertString("355", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mo>-</mo>"\
                            "<mi>23</mi>"\
                            "<mo>×</mo>"\
                            "<mi>355</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mo>-</mo>"\
                                "<mi>8165</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>-</mo>"\
                        "<mi>23</mi>"\
                        "<mo>×</mo>"\
                        "<mi>355</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 3, 3})) << document.GetEditorState().ToString();
}

//Present Auto result as Integer result
TEST_F(SolverIntegerTest, solver3)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.integer_result.show_notation = false;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("2345", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2345</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>2345.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetResultType({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2345</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>2345</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345."
        ) << ToBasicString(document.ToText());
}

//Check result notation
TEST_F(SolverIntegerTest, solver4)
{
    Start(600);
    
    Config config;
    document.GetConfig(config);
    config.integer_result.result_notation = Notation::Binary;
    config.integer_result.show_notation = true;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("2345", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2345=100100101001(bin)"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    config.integer_result.result_notation = Notation::Octal;
    config.integer_result.show_notation = false;
    document.SetConfig(config, true);

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("2345", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2345=100100101001(bin)\n"
        U"2345=4451"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    config.integer_result.result_notation = Notation::Decimal;
    config.integer_result.show_notation = true;
    document.SetConfig(config, true);

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("2345", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2345=100100101001(bin)\n"
        U"2345=4451\n"
        U"2345=2345(dec)"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    config.integer_result.result_notation = Notation::Hexadecimal;
    document.SetConfig(config, true);

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("2345", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2345=100100101001(bin)\n"
        U"2345=4451\n"
        U"2345=2345(dec)\n"
        U"2345=929(hex)"
        ) << ToBasicString(document.ToText());
}

//Set result notation
TEST_F(SolverIntegerTest, solver5)
{
    Start(600);
    
    Config config;
    document.GetConfig(config);
    config.integer_result.result_notation = Notation::Decimal;
    config.integer_result.show_notation = true;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("2345", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345(dec)"
        ) << ToBasicString(document.ToText());

    for (int i = 0; i < 7; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.SetNotation({0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 2}, Notation::Decimal, Notation::Binary, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2345=100100101001(bin)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345(dec)"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    config.solve_delay = 0;
    document.SetConfig(config, true);

    document.Redo();
    document.WaitRedo();
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2345=100100101001(bin)"
        ) << ToBasicString(document.ToText());

    auto _el = document.FindParent({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, ElementType::INTEGER_RESULT);
    yutovo_calculator::Notation n = document.GetDefaultNotation(_el->id);
    document.WaitTask(document.SetNotation({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, n, Notation::Octal, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2345=4451(oct)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetNotation({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, Notation::Decimal, Notation::Hexadecimal, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2345=929(hex)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2345=4451(oct)"
        ) << ToBasicString(document.ToText());
}

//Set result notation
TEST_F(SolverIntegerTest, solver6)
{
    Start(600);
    
    Config config;
    document.GetConfig(config);
    config.integer_result.result_notation = Notation::Decimal;
    config.integer_result.show_notation = true;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("567", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"567=567(dec)"
        ) << ToBasicString(document.ToText());

    auto _el = document.FindParent({0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 3}, ElementType::INTEGER_RESULT);
    yutovo_calculator::Notation n = document.GetDefaultNotation(_el->id);
    document.WaitTask(document.SetNotation({0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 3}, n, Notation::Hexadecimal, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"567=237(hex)"
        ) << ToBasicString(document.ToText());
    
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == 
        U"567"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"567=237(hex)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"567=567(dec)"
        ) << ToBasicString(document.ToText());
}

//Save and load
TEST_F(SolverIntegerTest, solver7)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("234", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    document.Save("solver6_1.yut");

    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.WaitTask(document.Load("solver6_1.yut"));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"234=234(dec)") << ToBasicString(document.ToText());
}

//Factorial
TEST_F(SolverIntegerTest, solver8)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("5", true);
    document.InsertExclamation(true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"5!=120(dec)"
        ) << ToBasicString(document.ToText());
}

//Check default notation
TEST_F(SolverIntegerTest, solver9)
{
    Start(600);
    
    Config config;
    document.GetConfig(config);
    config.integer_result.default_notation = Notation::Binary;
    config.integer_result.result_notation = Notation::Binary;
    config.integer_result.show_notation = true;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("10101", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"10101=10101(bin)"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    config.integer_result.default_notation = Notation::Octal;
    config.integer_result.result_notation = Notation::Decimal;
    config.integer_result.show_notation = false;
    document.WaitTask(document.SetConfig(config, true));

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("6543", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"10101=10101(bin)\n"
        U"6543=3427"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    config.integer_result.default_notation = Notation::Decimal;
    config.integer_result.result_notation = Notation::Hexadecimal;
    config.integer_result.show_notation = true;
    document.WaitTask(document.SetConfig(config, true));

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("6789", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"10101=10101(bin)\n"
        U"6543=3427\n"
        U"6789=1a85(hex)"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    config.integer_result.default_notation = Notation::Hexadecimal;
    config.integer_result.result_notation = Notation::Decimal;
    document.WaitTask(document.SetConfig(config, true));

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("567af", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"10101=10101(bin)\n"
        U"6543=3427\n"
        U"6789=1a85(hex)\n"
        U"567af=354223(dec)"
        ) << ToBasicString(document.ToText());
}

//Set default notation
TEST_F(SolverIntegerTest, solver10)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("567", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"567=567(dec)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetNotation({0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 3}, Notation::Octal, Notation::Hexadecimal, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"567=177(hex)"
        ) << ToBasicString(document.ToText());
    
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == 
        U"567"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"567=177(hex)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"567=567(dec)"
        ) << ToBasicString(document.ToText());
}

//Present Auto result as Integer result
TEST_F(SolverIntegerTest, solver11)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.integer_result.show_notation = true;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("2345", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345."
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetResultType({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, ResultType::INTEGER, true));
    document.WaitSolver();
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345(dec)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345."
        ) << ToBasicString(document.ToText());
}

//Undo of config
TEST_F(SolverIntegerTest, solver12)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("10", true);

    Config config;
    document.GetConfig(config);
    config.integer_result.result_notation = Notation::Binary;
    document.WaitTask(document.SetConfig(config, true));

    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"10=1010(bin)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();

    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"10=10(dec)"
        ) << ToBasicString(document.ToText());
}


//Breaking solving doesn't cause crash
TEST_F(SolverIntegerTest, solving13)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.integer_result.show_notation = false;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("2+2", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();

    LogicalId id = document.GetLogicalId(document.GetEditorState().caret_state.id);

    EXPECT_NO_THROW(document.BreakSolving(id, "", 0));
    EXPECT_NO_THROW(document.BreakSolving(LogicalId(), "", 0));
}

//Solve ids concurrent stress
TEST_F(SolverIntegerTest, solve14)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.integer_result.show_notation = false;
    document.WaitTask(document.SetConfig(config, true));

    for (int i = 0; i < 20; ++i)
    {
        document.InsertCode(false, true);
        document.InsertString("123+456", true);
        document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));

        LogicalId id = document.GetLogicalId(document.GetEditorState().caret_state.id);
        EXPECT_NO_THROW(document.BreakSolving(id, "", 0));

        document.WaitSolver();
        std::this_thread::sleep_for(50ms);

        document.Undo();
        document.WaitUndo();
    }
}

//Logical not
TEST_F(SolverIntegerTest, logical1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertNot(true);
    document.InsertString("4", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"¬4=3(dec)"
        ) << ToBasicString(document.ToText());
}

//Logical and
TEST_F(SolverIntegerTest, logical2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("115", true);
    document.InsertAnd(true);
    document.InsertString("234", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"115∧234=98(dec)"
        ) << ToBasicString(document.ToText());
}

//Logical or
TEST_F(SolverIntegerTest, logical3)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("115", true);
    document.InsertOr(true);
    document.InsertString("234", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"115∨234=251(dec)"
        ) << ToBasicString(document.ToText());
}

//Logical xor
TEST_F(SolverIntegerTest, logical4)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("115", true);
    document.InsertXor(true);
    document.InsertString("234", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"115⊕234=153(dec)"
        ) << ToBasicString(document.ToText());
}

//Solve with notations
TEST_F(SolverIntegerTest, notation1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1010", true);
    document.InsertSubscript(true);
    document.InsertString("bin", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"bin[1010]=10(dec)"
        ) << ToBasicString(document.ToText());
}

//Solve with notations
TEST_F(SolverIntegerTest, notation2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("776", true);
    document.InsertSubscript(true);
    document.InsertString("oct", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"oct[776]=510(dec)"
        ) << ToBasicString(document.ToText());
}

//Solve with notations
TEST_F(SolverIntegerTest, notation3)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("456", true);
    document.InsertSubscript(true);
    document.InsertString("dec", true);
    document.MoveCaretRight(false);
    document.InsertPlus(true);
    document.InsertString("101111", true);
    document.InsertSubscript(true);
    document.InsertString("bin", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"dec[456]+bin[101111]=503(dec)"
        ) << ToBasicString(document.ToText());
}

//Solve with notations
TEST_F(SolverIntegerTest, notation4)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.integer_result.result_notation = Notation::Binary;
    config.integer_result.show_notation = true;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("456f", true);
    document.InsertSubscript(true);
    document.InsertString("hex", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"hex[456f]=100010101101111(bin)"
        ) << ToBasicString(document.ToText());
}

//Solve with notations
TEST_F(SolverIntegerTest, notation5)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillOnce([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("456ft", true);
    document.InsertSubscript(true);
    document.InsertString("hex", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"hex[456ft]=Argument is over"
        ) << ToBasicString(document.ToText());
}

//Error result
TEST_F(SolverIntegerTest, notation6)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillOnce([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertSubscript(true);
    document.InsertString("bin", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"bin[123]=Argument is over"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetResultType({0, 0, 0, 0, 0, 0, 0, 2, 0}) == ResultType::AUTO);
}

}
