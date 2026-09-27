/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __SOLVER_H__
#define __SOLVER_H__

#include "solver_task.h"
#include <queue>
#include <thread>
#include <mutex>
#include <yutovo-solver/types.h>
#include <yutovo-calculator/math_helper.h>

namespace yutovo
{

class Document;

class Solver
{
public:
    Solver(Document* _document);
    ~Solver();

    void Solve(const LogicalId& id, const std::string& task_guid, const uint code_id, Config::AutoResultConfig& config, bool include_document, 
        const std::u32string& expression, const uint delay);
    void Solve(const LogicalId& id, const std::string& task_guid, const uint code_id, Config::RealResultConfig& config, bool include_document, 
        const std::u32string& expression, const uint delay);
    void Solve(const LogicalId& id, const std::string& task_guid, const uint code_id, Config::IntegerResultConfig& config, bool include_document, 
        const std::u32string& expression, const uint delay);
    void Solve(const LogicalId& id, const std::string& task_guid, const uint code_id, Config::RationalResultConfig& config, bool include_document, 
        const std::u32string& expression, const uint delay);
    void Solve(const LogicalId& id, const std::string& task_guid, const uint code_id, Config::ComplexResultConfig& config, bool include_document, 
        const std::u32string& expression, const uint delay);
    void Solve(const LogicalId& id, const std::string& task_guid, const uint code_id, Config::ArrayRealResultConfig& config, bool include_document,
        const std::u32string& expression, const uint delay);
    void SolveSymbolicReal(const LogicalId& id, const std::string& task_guid, const uint code_id, Config::RealResultConfig& config, bool include_document,
        const std::u32string& expression, const uint delay);
    void SolveSymbolicRational(const LogicalId& id, const std::string& task_guid, const uint code_id, Config::RationalResultConfig& config, bool include_document,
        const std::u32string& expression, const uint delay);
    void SolveSymbolicComplex(const LogicalId& id, const std::string& task_guid, const uint code_id, Config::ComplexResultConfig& config, bool include_document,
        const std::u32string& expression, const uint delay);

    void BreakSolving(const LogicalId& id, const std::string& task_guid, const uint code_id, bool wait);

    void SetIdentifier(const LogicalId& id, const std::string& task_guid, uint code_id, Config::AutoResultConfig& config, bool include_document, 
        const std::u32string& identifier, const std::u32string& expression, const uint delay);
    void RemoveIdentifier(const LogicalId& id, uint code_id, const std::u32string& identifier, const uint delay);
    void RemoveUserIdentifiers();
    void ClearExport();
    void SetLocale(const yutovo_calculator::Language _language);
    void ListIdentifiers(uint code_id);

    void RemoveSolver(const uint code_id);

private:
#ifdef REMOTE_SOLVER
    void MessageLoop(WebSocketPtr socket_, std::deque<SolverTaskPtr>& tasks_, std::atomic_bool& next_circle_);
#else
    void MessageLoop(WebSocketPtr socket_, std::deque<SolverTaskPtr>& tasks_, std::atomic_bool& next_circle_, yutovo_solver::Session& _session);
#endif

    void EraseSolveTasks(const LogicalId id);

public:
    std::string solver_guid;
    
private:
    Document* document;
    yutovo_calculator::Language language = yutovo_calculator::Language::English;

    std::mutex socket_mutex;
    WebSocketPtr socket, break_socket;

    std::deque<SolverTaskPtr> tasks;
    std::deque<SolverTaskPtr> break_tasks;

    std::mutex current_solving_mutex;
    LogicalId current_solving_id;
    uint current_code_id = 0;

    std::vector<yutovo_solver::ResultType> result_types_seq;

    Logger* logger;

    bool exit = false;

    std::mutex tasks_mutex;
    std::atomic_bool next_circle, break_next_circle;

    std::thread message_loop;
    std::thread break_loop;

#ifndef REMOTE_SOLVER
    yutovo_solver::ServiceConfig service_config;
    yutovo_solver::ServiceContext service_context;
    yutovo_solver::Session session;
#endif
};

}

#endif
