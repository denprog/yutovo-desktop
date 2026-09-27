/*
 * Yutovo Solver
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include <rapidjson/document.h>
#include "mock.h"

namespace yutovo_test
{

TEST_F(SolverTest, solve_real)
{
    auto request = MakeRequest(ResultType::REAL, "1+2");
    rapidjson::Document reply;
    solver->Solve(request, reply);
    ASSERT_FALSE(reply.HasMember("error")) << "Unexpected solver error";
    ASSERT_TRUE(reply.HasMember("result_type"));
    ASSERT_EQ(reply["result_type"].GetInt(), static_cast<int>(ResultType::REAL));
}

TEST_F(SolverTest, solve_integer)
{
    auto request = MakeRequest(ResultType::INTEGER, "5!");
    rapidjson::Document reply;
    solver->Solve(request, reply);
    ASSERT_FALSE(reply.HasMember("error")) << "Unexpected solver error";
    ASSERT_TRUE(reply.HasMember("result_type"));
    ASSERT_EQ(reply["result_type"].GetInt(), static_cast<int>(ResultType::INTEGER));
}

TEST_F(SolverTest, solve_rational)
{
    auto request = MakeRequest(ResultType::RATIONAL, "2/4");
    rapidjson::Document reply;
    solver->Solve(request, reply);
    ASSERT_FALSE(reply.HasMember("error")) << "Unexpected solver error";
    ASSERT_TRUE(reply.HasMember("result_type"));
    ASSERT_EQ(reply["result_type"].GetInt(), static_cast<int>(ResultType::RATIONAL));
}

TEST_F(SolverTest, solve_complex)
{
    auto request = MakeRequest(ResultType::COMPLEX, "2+1i");
    rapidjson::Document reply;
    solver->Solve(request, reply);
    ASSERT_FALSE(reply.HasMember("error")) << "Unexpected solver error";
    ASSERT_TRUE(reply.HasMember("result_type"));
    ASSERT_EQ(reply["result_type"].GetInt(), static_cast<int>(ResultType::COMPLEX));
}

TEST_F(SolverTest, solve_array_real)
{
    auto request = MakeRequest(ResultType::ARRAY_REAL, "[1,2,3]");
    rapidjson::Document reply;
    solver->Solve(request, reply);
    ASSERT_FALSE(reply.HasMember("error")) << "Unexpected solver error";
    ASSERT_TRUE(reply.HasMember("result_type"));
    ASSERT_EQ(reply["result_type"].GetInt(), static_cast<int>(ResultType::ARRAY_REAL));
}

TEST_F(SolverTest, solve_symbolic_real)
{
    auto request = MakeRequest(ResultType::SYMBOLIC_REAL, "x+1");
    rapidjson::Document reply;
    solver->Solve(request, reply);
    ASSERT_FALSE(reply.HasMember("error")) << "Unexpected solver error";
    ASSERT_TRUE(reply.HasMember("result_type"));
    ASSERT_EQ(reply["result_type"].GetInt(), static_cast<int>(ResultType::SYMBOLIC_REAL));
}

TEST_F(SolverTest, solve_symbolic_rational)
{
    auto request = MakeRequest(ResultType::SYMBOLIC_RATIONAL, "x+1/2");
    rapidjson::Document reply;
    solver->Solve(request, reply);
    ASSERT_FALSE(reply.HasMember("error")) << "Unexpected solver error";
    ASSERT_TRUE(reply.HasMember("result_type"));
    ASSERT_EQ(reply["result_type"].GetInt(), static_cast<int>(ResultType::SYMBOLIC_RATIONAL));
}

TEST_F(SolverTest, solve_symbolic_complex)
{
    auto request = MakeRequest(ResultType::SYMBOLIC_COMPLEX, "1+2*i");
    rapidjson::Document reply;
    solver->Solve(request, reply);
    ASSERT_FALSE(reply.HasMember("error")) << "Unexpected solver error";
    ASSERT_TRUE(reply.HasMember("result_type"));
    ASSERT_EQ(reply["result_type"].GetInt(), static_cast<int>(ResultType::SYMBOLIC_COMPLEX));
}

TEST_F(SolverTest, solve_auto_with_results_order)
{
    auto request = MakeRequest(ResultType::AUTO, "x+1");
    auto& alloc = request.GetAllocator();
    rapidjson::Value order(rapidjson::kArrayType);
    order.PushBack(static_cast<int>(ResultType::SYMBOLIC_REAL), alloc);
    request.AddMember("results_order", order, alloc);

    rapidjson::Document reply;
    solver->Solve(request, reply);
    ASSERT_FALSE(reply.HasMember("error")) << "Unexpected solver error";
    ASSERT_TRUE(reply.HasMember("result_type"));
    ASSERT_EQ(reply["result_type"].GetInt(), static_cast<int>(ResultType::SYMBOLIC_REAL));
}

TEST_F(SolverTest, auto_wrong_expression_returns_error_object)
{
    for (const char* expr : {";", "="})
    {
        auto request = MakeAutoRequest(expr);
        rapidjson::Document reply;
        solver->Solve(request, reply);
        ASSERT_TRUE(reply.IsObject()) << "reply for [" << expr << "] is not an object";
        ASSERT_TRUE(reply.HasMember("error")) << "expected parser error for [" << expr << "]";
        ASSERT_EQ(reply["error"]["error_code"].GetInt(), static_cast<int>(ErrorCode::PARSER_ERROR));
    }
}

}
