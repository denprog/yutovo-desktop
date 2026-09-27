/*
 * Yutovo Solver
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <memory>
#include <thread>
#include <clocale>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "types.h"
#include "mock.h"

namespace yutovo_test
{

//The real parser does not know simplify, the symbolic parser parses it as its own function and reports a wrong arguments count
TEST_F(SolverTest, auto_simplify_shows_symbolic_parser_error)
{
    auto request = MakeAutoRequest("simplify()");
    rapidjson::Document reply;
    solver->Solve(request, reply);

    ASSERT_TRUE(reply.HasMember("error")) << "Expected a parser error reply";
    const auto& error = reply["error"];
    ASSERT_EQ(error["error_code"].GetInt(), static_cast<int>(ErrorCode::PARSER_ERROR));
    ASSERT_EQ(error["parser_error_code"].GetInt(), static_cast<int>(ParserExceptionCode::WrongArgumentsCount));
    ASSERT_EQ(error["pos"].GetInt(), 0);
}

//sin is a real parser function, so the wrong arguments count error comes from the real parser
TEST_F(SolverTest, auto_wrong_arguments_count_keeps_real_parser_error)
{
    auto request = MakeAutoRequest("sin(1,2)");
    rapidjson::Document reply;
    solver->Solve(request, reply);

    ASSERT_TRUE(reply.HasMember("error")) << "Expected a parser error reply";
    const auto& error = reply["error"];
    ASSERT_EQ(error["error_code"].GetInt(), static_cast<int>(ErrorCode::PARSER_ERROR));
    ASSERT_EQ(error["parser_error_code"].GetInt(), static_cast<int>(ParserExceptionCode::WrongArgumentsCount));
    ASSERT_TRUE(error["pos"].GetInt() == 0);
}

//The real parser parses the operator and stops at the missing operand
TEST_F(SolverTest, auto_incomplete_expression_keeps_real_parser_error)
{
    auto request = MakeAutoRequest("1 +");
    rapidjson::Document reply;
    solver->Solve(request, reply);

    ASSERT_TRUE(reply.HasMember("error")) << "Expected a parser error reply";
    const auto& error = reply["error"];
    ASSERT_EQ(error["error_code"].GetInt(), static_cast<int>(ErrorCode::PARSER_ERROR));
    ASSERT_TRUE(error["pos"].GetInt() == 3);
}

//foo is unknown for both parsers, the real parser is tried first and its error is kept
TEST_F(SolverTest, auto_unknown_function_keeps_real_parser_error)
{
    auto request = MakeAutoRequest("foo(1+2)");
    rapidjson::Document reply;
    solver->Solve(request, reply);

    ASSERT_TRUE(reply.HasMember("error")) << "Expected a parser error reply";
    const auto& error = reply["error"];
    ASSERT_EQ(error["error_code"].GetInt(), static_cast<int>(ErrorCode::PARSER_ERROR));
    ASSERT_EQ(error["parser_error_code"].GetInt(), static_cast<int>(ParserExceptionCode::UnknownIdentifier));
    ASSERT_EQ(error["pos"].GetInt(), 0);
}

//Helper test for the CodeTest.Code53 test in yutovo-editor
TEST_F(SolverTest, auto_symbolic_expand_after_evalf_error)
{
    setlocale(LC_ALL, "");
    std::vector<std::string> expressions =
        {
            "x+2*x+pow(x,2)+3*x+55",
            "evalf(y+3.4*y)",
            "evalf(sqrt(y)+y+3.4*y)",
            "expand(pow(pow(x,3)+x+3,2))",
            "simplify(1-pow(cos(x),2))",
            "simplify(pow(x+y,2)-pow(x,2)-2*x*y)",
            "simplify(pow(x+y,2)+pow(x+y,2))",
            "simplify(x)",
            "simplify(x)"
        };

    for (const auto& expr : expressions)
    {
        auto request = MakeAutoRequest(expr.c_str());
        auto& alloc = request.GetAllocator();
        rapidjson::Value order(rapidjson::kArrayType);
        for (int t : {1,2,3,4,6,7,8,9})
            order.PushBack(t, alloc);
        request.AddMember("results_order", order, alloc);
        rapidjson::Document reply;
        solver->Solve(request, reply);
    }

    {
        auto request = MakeRequest(ResultType::SYMBOLIC_REAL, "expand(pow(pow(x,3)+x+3,2))");
        rapidjson::Document reply;
        solver->Solve(request, reply);
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        reply.Accept(writer);
        ASSERT_FALSE(reply.HasMember("error")) << "Unexpected solver error";
        ASSERT_TRUE(reply.HasMember("result_type"));
        ASSERT_EQ(reply["result_type"].GetInt(), static_cast<int>(ResultType::SYMBOLIC_REAL));
    }
}

//Helper test for the CodeTest.Code53 test in yutovo-editor
TEST_F(SolverTest, thread_auto_symbolic_expand_after_evalf_error)
{
    std::thread t(
        [this]
        {
            for (int i = 0; i < 10; ++i)
            {
                {
                    auto request = MakeAutoRequest("evalf(sqrt((y)/(2))+y+3.4*y)");
                    rapidjson::Document reply;
                    solver->Solve(request, reply);
                }

                {
                    auto request = MakeAutoRequest("expand(pow(pow(x,3)+x+3,2))");
                    rapidjson::Document reply;
                    solver->Solve(request, reply);
                    rapidjson::StringBuffer buffer;
                    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                    reply.Accept(writer);
                    ASSERT_FALSE(reply.HasMember("error")) << "Unexpected solver error";
                    ASSERT_TRUE(reply.HasMember("result_type"));
                }
            }
        });
    t.join();
}

//Helper test for the CodeTest.Code53 test in yutovo-editor
TEST_F(SolverTest, two_solvers_concurrent)
{
    auto solver2 = std::make_unique<CalculatorSolver>("document_guid", "solver_guid-2", parser_context, Language::English, 0, "", false, false);
    {
        rapidjson::Document request, reply;
        request.SetObject();
        solver2->Solve(request, reply);
    }

    std::thread t1(
        [this]
        {
            for (int i = 0; i < 20; ++i)
            {
                auto request = MakeAutoRequest("evalf(sqrt((y)/(2))+y+3.4*y)");
                rapidjson::Document reply;
                solver->Solve(request, reply);
            }
        });

    std::thread t2(
        [&solver2]
        {
            for (int i = 0; i < 20; ++i)
            {
                rapidjson::Document request, reply;
                request.SetObject();
                auto& alloc = request.GetAllocator();
                request.AddMember("result_type", static_cast<int>(ResultType::AUTO), alloc);
                std::string expr = "expand((x^3+x+3)^2);";
                request.AddMember("expression", rapidjson::Value(expr.c_str(), alloc), alloc);
                request.AddMember("id", rapidjson::Value(rapidjson::kArrayType).Move(), alloc);
                request.AddMember("timestamp", 1, alloc);
                solver2->Solve(request, reply);
            }
        });

    t1.join();
    t2.join();
}

//Helper test for the CodeTest.Code53 test in yutovo-editor
TEST_F(SolverTest, one_solver_concurrent)
{
    std::thread t1(
        [this]
        {
            for (int i = 0; i < 20; ++i)
            {
                auto request = MakeAutoRequest("evalf(sqrt((y)/(2))+y+3.4*y)");
                rapidjson::Document reply;
                solver->Solve(request, reply);
            }
        });

    std::thread t2(
        [this]
        {
            for (int i = 0; i < 20; ++i)
            {
                auto request = MakeAutoRequest("expand((x^3+x+3)^2)");
                rapidjson::Document reply;
                solver->Solve(request, reply);
            }
        });

    t1.join();
    t2.join();
}

//Helper test for the CodeTest.Code53 test in yutovo-editor
TEST_F(SolverTest, row0_repro)
{
    for (int i = 0; i < 10; ++i)
    {
        auto request = MakeAutoRequest("x+2*x+pow(x,2)+3*pow(x,55)");
        rapidjson::Document reply;
        solver->Solve(request, reply);
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        reply.Accept(writer);
        ASSERT_FALSE(reply.HasMember("error")) << "Unexpected solver error";
    }
}

//Helper test for the CodeTest.Code53 test in yutovo-editor
TEST_F(SolverTest, row0_semicolon_repro)
{
    for (int i = 0; i < 10; ++i)
    {
        auto request = MakeAutoRequest("x+2*x+pow(x,2)+3*pow(x,55);");
        rapidjson::Document reply;
        solver->Solve(request, reply);
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        reply.Accept(writer);
        ASSERT_FALSE(reply.HasMember("error")) << "Unexpected solver error";
    }
}

//Helper test for the CodeTest.Code53 test in yutovo-editor
TEST_F(SolverTest, row0_concurrent_with_evalf_error)
{
    std::thread t1(
        [this]
        {
            for (int i = 0; i < 20; ++i)
            {
                auto request = MakeAutoRequest("evalf(sqrt((y)/(2))+y+3.4*y)");
                rapidjson::Document reply;
                solver->Solve(request, reply);
            }
        });

    std::thread t2(
        [this]
        {
            for (int i = 0; i < 20; ++i)
            {
                auto request = MakeAutoRequest("x+2*x+pow(x,2)+3*pow(x,55)");
                rapidjson::Document reply;
                solver->Solve(request, reply);
                rapidjson::StringBuffer buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                reply.Accept(writer);
                ASSERT_FALSE(reply.HasMember("error")) << "Unexpected solver error";
            }
        });

    t1.join();
    t2.join();
}

//Helper test for the CodeTest.Code53 test in yutovo-editor
TEST_F(SolverTest, multiple_solvers_same_context)
{
    std::vector<std::unique_ptr<CalculatorSolver>> solvers;
    for (int i = 0; i < 5; ++i)
    {
        auto s = std::make_unique<CalculatorSolver>("document_guid", std::string("solver_guid-") + std::to_string(i), parser_context, 
            Language::English, 0, "", false, false);
        rapidjson::Document request, reply;
        request.SetObject();
        s->Solve(request, reply);
        solvers.push_back(std::move(s));
    }

    std::vector<std::string> expressions =
        {
            "x+2*x+pow(x,2)+3*x+55",
            "evalf(y+3.4*y)",
            "evalf(sqrt(y)+y+3.4*y)",
            "expand(pow(pow(x,3)+x+3,2))",
            "simplify(1-pow(cos(x),2))",
            "simplify(pow(x+y,2)-pow(x,2)-2*x*y)",
            "simplify(pow(x+y,2)+pow(x+y,2))",
            "simplify(x)",
            "simplify(x)"
        };

    for (size_t i = 0; i < expressions.size(); ++i)
    {
        auto request = MakeAutoRequest(expressions[i].c_str());
        rapidjson::Document reply;
        solvers[i % solvers.size()]->Solve(request, reply);
    }
}

TEST_F(SolverTest, pending_break_cancels_next_solve)
{
    rapidjson::Document break_request;
    break_request.SetObject();
    auto& alloc = break_request.GetAllocator();
    rapidjson::Value id(rapidjson::kArrayType);
    id.PushBack(1, alloc);
    break_request.AddMember("id", id, alloc);
    break_request.AddMember("timestamp", 1, alloc);

    rapidjson::Document break_reply;
    solver->BreakSolving(break_request, break_reply);
    ASSERT_FALSE(break_reply.HasMember("error"));

    auto request = MakeRequest(ResultType::REAL, "1+2");
    request.RemoveMember("id");
    rapidjson::Value rid(rapidjson::kArrayType);
    rid.PushBack(1, request.GetAllocator());
    request.AddMember("id", rid, request.GetAllocator());

    rapidjson::Document reply;
    solver->Solve(request, reply);
    ASSERT_TRUE(reply.HasMember("error"));
    ASSERT_EQ(reply["error"]["error_code"].GetInt(), static_cast<int>(ErrorCode::PARSER_ERROR));
    ASSERT_EQ(reply["error"]["parser_error_code"].GetInt(), static_cast<int>(ParserExceptionCode::Break));
}

TEST_F(SolverTest, break_solving_race_condition)
{
    std::atomic<bool> stop{false};
    std::atomic<int> solves{0};

    std::thread worker(
        [this, &stop, &solves]
        {
            while (!stop.load())
            {
                auto request = MakeAutoRequest("expand((x+1)^30)");
                rapidjson::Document reply;
                solver->Solve(request, reply);
                ++solves;
            }
        });

    for (int i = 0; i < 200; ++i)
    {
        rapidjson::Document request, reply;
        request.SetObject();
        auto& alloc = request.GetAllocator();
        request.AddMember("id", rapidjson::Value(rapidjson::kArrayType).Move(), alloc);
        request.AddMember("timestamp", 1, alloc);
        solver->BreakSolving(request, reply);
        std::this_thread::yield();
    }

    stop.store(true);
    worker.join();
    ASSERT_GT(solves.load(), 0);
}

}
