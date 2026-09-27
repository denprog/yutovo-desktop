/*
 * Yutovo Solver
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include <rapidjson/document.h>
#include "mock.h"
#include "types.h"

namespace yutovo_test
{

TEST_F(SolverTest, parser_exception_becomes_json_error)
{
    auto request = MakeRequest(ResultType::REAL, "sin(1,2)");
    rapidjson::Document reply;
    solver->Solve(request, reply);
    ASSERT_TRUE(reply.HasMember("error")) << "Expected parser error reply";
    const auto& error = reply["error"];
    ASSERT_EQ(error["error_code"].GetInt(), static_cast<int>(ErrorCode::PARSER_ERROR));
    ASSERT_EQ(error["parser_error_code"].GetInt(), static_cast<int>(ParserExceptionCode::WrongArgumentsCount));
    ASSERT_EQ(error["pos"].GetInt(), 0);
}

TEST_F(SolverTest, service_exception_on_missing_id)
{
    auto request = MakeRequest(ResultType::REAL, "1+2");
    request.RemoveMember("id");
    rapidjson::Document reply;
    ASSERT_THROW(solver->Solve(request, reply), ServiceException);
}

TEST_F(SolverTest, service_exception_on_break_solving_missing_id)
{
    rapidjson::Document request, reply;
    request.SetObject();
    request.AddMember("timestamp", 1, request.GetAllocator());
    ASSERT_THROW(solver->BreakSolving(request, reply), ServiceException);
}

}
