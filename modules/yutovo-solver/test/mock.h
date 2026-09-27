/*
 * Yutovo Solver
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __MOCK_H__
#define __MOCK_H__

#include <gtest/gtest.h>
#include <yutovo-calculator/parser.h>
#include "service_solver.h"

namespace yutovo_test
{

using namespace yutovo_solver;
using namespace yutovo_calculator;

struct SolverTest : public testing::Test
{
    void SetUp() override;
    
    static rapidjson::Document MakeRequest(ResultType result_type, const char* expression);
    static rapidjson::Document MakeAutoRequest(const char* expression);

    std::shared_ptr<yutovo_calculator::ParserContext> parser_context;
    std::unique_ptr<CalculatorSolver> solver;
};

}

#endif
