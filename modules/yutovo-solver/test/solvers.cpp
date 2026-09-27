/*
 * Yutovo Solver
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include <yutovo-logger/logger.h>
#include <ctime>
#include "service_config.h"
#include "service_context.h"
#include "service_solver.h"
#include "types.h"

namespace yutovo_test
{

using namespace yutovo_solver;
using namespace yutovo;

struct SolversTest : public testing::Test
{
    Logger* logger = nullptr;
    ServiceConfig config;
    ServiceContext context;

    SolversTest() :
        logger(Logger::GetInstance("", "test", false, false)),
        config(logger),
        context(&config, "", false, false)
    {
    }
};

TEST_F(SolversTest, get_same_solver_returns_same_instance)
{
    auto solver1 = context.solvers.GetSolver("doc_lifecycle", "solver_g1", 1, SolverType::CALCULATOR);
    ASSERT_NE(solver1, nullptr);
    auto solver2 = context.solvers.GetSolver("doc_lifecycle", "solver_g1", 1, SolverType::CALCULATOR);
    ASSERT_EQ(solver1.get(), solver2.get());
}

TEST_F(SolversTest, get_different_code_id_returns_different_instance)
{
    auto solver1 = context.solvers.GetSolver("doc_lifecycle", "solver_g2", 1, SolverType::CALCULATOR);
    auto solver2 = context.solvers.GetSolver("doc_lifecycle", "solver_g2", 2, SolverType::CALCULATOR);
    ASSERT_NE(solver1.get(), solver2.get());
}

TEST_F(SolversTest, remove_solver_creates_new_instance)
{
    auto solver1 = context.solvers.GetSolver("doc_lifecycle", "solver_g3", 1, SolverType::CALCULATOR);
    ASSERT_TRUE(context.solvers.RemoveSolver("solver_g3", 1));
    auto solver2 = context.solvers.GetSolver("doc_lifecycle", "solver_g3", 1, SolverType::CALCULATOR);
    ASSERT_NE(solver1.get(), solver2.get());
}

TEST_F(SolversTest, remove_timeouted_removes_idle_solver)
{
    auto solver1 = context.solvers.GetSolver("doc_lifecycle", "solver_g4", 1, SolverType::CALCULATOR);
    config.solver_idle_timeout = 1;
    solver1->idle_time = time(nullptr) - 10;
    context.solvers.RemoveTimeouted();
    auto solver2 = context.solvers.GetSolver("doc_lifecycle", "solver_g4", 1, SolverType::CALCULATOR);
    ASSERT_NE(solver1.get(), solver2.get());
}

}
