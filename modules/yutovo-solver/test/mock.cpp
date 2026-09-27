/*
 * Yutovo Solver
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "mock.h"

namespace yutovo_test
{

void SolverTest::SetUp()
{
    parser_context = std::make_shared<yutovo_calculator::ParserContext>();
    solver = std::make_unique<CalculatorSolver>("document_guid", "solver_guid", parser_context, Language::English, 0, "", false, false);

    //the first Solve call always returns SOLVER_RESTARTED_ERROR
    rapidjson::Document request, reply;
    request.SetObject();
    solver->Solve(request, reply);
}

rapidjson::Document SolverTest::MakeRequest(ResultType result_type, const char* expression)
{
    rapidjson::Document request;
    request.SetObject();
    auto& alloc = request.GetAllocator();
    request.AddMember("result_type", static_cast<int>(result_type), alloc);
    std::string expr_with_semicolon = expression;
    expr_with_semicolon += ";";
    request.AddMember("expression", rapidjson::Value(expr_with_semicolon.c_str(), alloc), alloc);
    request.AddMember("id", rapidjson::Value(rapidjson::kArrayType).Move(), alloc);
    request.AddMember("timestamp", 1, alloc);
    return request;
}

rapidjson::Document SolverTest::MakeAutoRequest(const char* expression)
{
    return MakeRequest(ResultType::AUTO, expression);
}

}
