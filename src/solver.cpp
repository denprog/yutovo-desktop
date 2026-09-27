/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "solver.h"
#include "document.h"
#include "config.h"
#include "web_socket.h"
#include <chrono>
#ifdef min
#undef min
#endif
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/system/system_error.hpp>

namespace yutovo
{

using namespace std::chrono_literals;
using namespace std::chrono;
using namespace yutovo_solver;

//Solver

Solver::Solver(Document* _document) :
    solver_guid(boost::uuids::to_string(boost::uuids::random_generator()())),
    document(_document),
    logger(Logger::GetInstance(document->config.logs_path + "/yutovo-editor", "yutovo-editor", document->config.log_console, document->config.log_file)),
#ifdef REMOTE_SOLVER
    message_loop(std::thread(&Solver::MessageLoop, this, std::ref(socket), std::ref(tasks), std::ref(next_circle))),
    break_loop(std::thread(&Solver::MessageLoop, this, std::ref(break_socket), std::ref(break_tasks), std::ref(break_next_circle)))
#else
    service_config(logger),
    service_context(&service_config, document->config.logs_path, document->config.log_console, document->config.log_file),
    session(&service_context, document->config.logs_path, document->config.log_console, document->config.log_file),
    message_loop(std::thread(&Solver::MessageLoop, this, std::ref(socket), std::ref(tasks), std::ref(next_circle), std::ref(session))),
    break_loop(std::thread(&Solver::MessageLoop, this, std::ref(break_socket), std::ref(break_tasks), std::ref(break_next_circle), std::ref(session)))
#endif
{
    result_types_seq = {ResultType::REAL, ResultType::INTEGER, ResultType::RATIONAL, ResultType::COMPLEX, ResultType::ARRAY_REAL, ResultType::SYMBOLIC_REAL, ResultType::SYMBOLIC_RATIONAL, ResultType::SYMBOLIC_COMPLEX};
    language = document->config.language;
}

Solver::~Solver()
{
    {
        std::unique_lock<std::mutex> lock(current_solving_mutex);
        if (!current_solving_id.empty())
        {
            std::unique_lock<std::mutex> lock(tasks_mutex);
            break_tasks.emplace_front(nullptr);
            break_tasks.emplace_front(new BreakSolverTask(current_solving_id, document, solver_guid, current_code_id, true, logger)); //first of all break this solving
            break_next_circle = true;
        }
    }

    {
        std::unique_lock<std::mutex> lock(socket_mutex);
        if (socket)
            socket->Close();
    }

    exit = true;
    next_circle = true;
    break_next_circle = true;
    message_loop.join();
    break_loop.join();

    {
        std::unique_lock<std::mutex> lock(socket_mutex);
        if (break_socket)
            break_socket->Close();
    }
}

void Solver::Solve(const LogicalId& id, const std::string& task_guid, const uint code_id, Config::AutoResultConfig& config, bool include_document, 
    const std::u32string& expression, const uint delay)
{
    std::unique_lock<std::mutex> lock(tasks_mutex);
    EraseSolveTasks(id);
    tasks.emplace_back(new AutoSolverTask(id, document, solver_guid, task_guid, code_id, ExpressionType::SOLVE, config, include_document, 
        expression, delay, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::Solve(const LogicalId& id, const std::string& task_guid, const uint code_id, Config::RealResultConfig& config, bool include_document, 
    const std::u32string& expression, const uint delay)
{
    std::unique_lock<std::mutex> lock(tasks_mutex);
    EraseSolveTasks(id);
    tasks.emplace_back(new RealSolverTask(id, document, solver_guid, task_guid, code_id, ExpressionType::SOLVE, config, include_document, 
        expression, delay, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::Solve(const LogicalId& id, const std::string& task_guid, const uint code_id, Config::IntegerResultConfig& config, bool include_document, 
    const std::u32string& expression, const uint delay)
{
    std::unique_lock<std::mutex> lock(tasks_mutex);
    EraseSolveTasks(id);
    tasks.emplace_back(new IntegerSolverTask(id, document, solver_guid, task_guid, code_id, ExpressionType::SOLVE, config, include_document, 
        expression, delay, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::Solve(const LogicalId& id, const std::string& task_guid, const uint code_id, Config::RationalResultConfig& config, bool include_document, 
    const std::u32string& expression, const uint delay)
{
    std::unique_lock<std::mutex> lock(tasks_mutex);
    EraseSolveTasks(id);
    tasks.emplace_back(new RationalSolverTask(id, document, solver_guid, task_guid, code_id, ExpressionType::SOLVE, config, include_document, 
        expression, delay, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::Solve(const LogicalId& id, const std::string& task_guid, const uint code_id, Config::ComplexResultConfig& config, bool include_document, 
    const std::u32string& expression, const uint delay)
{
    std::unique_lock<std::mutex> lock(tasks_mutex);
    EraseSolveTasks(id);
    tasks.emplace_back(new ComplexSolverTask(id, document, solver_guid, task_guid, code_id, ExpressionType::SOLVE, config, include_document, 
        expression, delay, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::Solve(const LogicalId& id, const std::string& task_guid, const uint code_id, Config::ArrayRealResultConfig& config, bool include_document,
    const std::u32string& expression, const uint delay)
{
    std::unique_lock<std::mutex> lock(tasks_mutex);
    EraseSolveTasks(id);
    tasks.emplace_back(new ArrayRealSolverTask(id, document, solver_guid, task_guid, code_id, ExpressionType::SOLVE, config, include_document,
        expression, delay, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::SolveSymbolicReal(const LogicalId& id, const std::string& task_guid, const uint code_id, Config::RealResultConfig& config, bool include_document,
    const std::u32string& expression, const uint delay)
{
    std::unique_lock<std::mutex> lock(tasks_mutex);
    EraseSolveTasks(id);
    tasks.emplace_back(new SymbolicRealSolverTask(id, document, solver_guid, task_guid, code_id, ExpressionType::SOLVE, config, include_document,
        expression, delay, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::SolveSymbolicRational(const LogicalId& id, const std::string& task_guid, const uint code_id, Config::RationalResultConfig& config, bool include_document,
    const std::u32string& expression, const uint delay)
{
    std::unique_lock<std::mutex> lock(tasks_mutex);
    EraseSolveTasks(id);
    tasks.emplace_back(new SymbolicRationalSolverTask(id, document, solver_guid, task_guid, code_id, ExpressionType::SOLVE, config, include_document,
        expression, delay, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::SolveSymbolicComplex(const LogicalId& id, const std::string& task_guid, const uint code_id, Config::ComplexResultConfig& config, bool include_document,
    const std::u32string& expression, const uint delay)
{
    std::unique_lock<std::mutex> lock(tasks_mutex);
    EraseSolveTasks(id);
    tasks.emplace_back(new SymbolicComplexSolverTask(id, document, solver_guid, task_guid, code_id, ExpressionType::SOLVE, config, include_document,
        expression, delay, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::BreakSolving(const LogicalId& id, const std::string& task_guid, const uint code_id, bool wait)
{
    {
        std::unique_lock<std::mutex> task_lock(tasks_mutex);
        //the result row is being deleted, its queued solves are not needed anymore; 
        //the logical id is reused by remakes, so the tasks are matched by the solve guid
        tasks.erase(std::remove_if(tasks.begin(), tasks.end(),
            [&task_guid](SolverTaskPtr& task)
            {
                return task && task->task_guid == task_guid;
            }
            ), tasks.end());
    }

    std::unique_lock<std::mutex> current_lock(current_solving_mutex);
    if (id != current_solving_id)
        return;

    std::unique_lock<std::mutex> task_lock(tasks_mutex);
    break_tasks.emplace_front(nullptr);
    break_tasks.emplace_front(new BreakSolverTask(id, document, solver_guid, code_id, wait, logger)); //first of all break this solving
    break_next_circle = true;
}

void Solver::SetIdentifier(const LogicalId& id, const std::string& task_guid, uint code_id, Config::AutoResultConfig& config, bool include_document, 
    const std::u32string& identifier, const std::u32string& expression, const uint delay)
{
    {
        std::unique_lock<std::mutex> lock(tasks_mutex);
        tasks.erase(std::remove_if(tasks.begin(), tasks.end(), 
            [id](SolverTaskPtr& task)
            {
                return task && task->id == id && task->expression_type == ExpressionType::USER_SYMBOL && (dynamic_cast<AutoSolverTask*>(task.get()));
            }
            ), tasks.end());
    }

    std::unique_lock<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetIdentifierSolverTask(id, document, solver_guid, task_guid, code_id, config, include_document, identifier, expression, 
        delay, logger));
    tasks.emplace_back(new ListIdentifiersSolverTask(document, solver_guid, code_id, logger)); //for syntax highlight
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::RemoveIdentifier(const LogicalId& id, uint code_id, const std::u32string& identifier, const uint delay)
{
    std::vector<std::u32string> id_arr;
    boost::split(id_arr, identifier, boost::is_any_of("()"));

    if (!id_arr.empty())
    {
        std::unique_lock<std::mutex> lock(tasks_mutex);
        tasks.emplace_back(new RemoveIdentifierSolverTask(id, document, solver_guid, code_id, id_arr[0], delay, logger));
        tasks.emplace_back(new ListIdentifiersSolverTask(document, solver_guid, code_id, logger)); //for syntax highlight
        tasks.emplace_back(nullptr);
        next_circle = true;
    }
}

void Solver::RemoveUserIdentifiers()
{
    {
        std::unique_lock<std::mutex> lock(tasks_mutex);
        tasks.erase(std::remove_if(tasks.begin(), tasks.end(), 
            [](SolverTaskPtr& task)
            {
                return task && dynamic_cast<RemoveUserIdentifiersSolverTask*>(task.get());
            }
            ), tasks.end());
    }

    std::unique_lock<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new RemoveUserIdentifiersSolverTask(document, solver_guid, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::ClearExport()
{
    std::unique_lock<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new ClearExportSolverTask(document, solver_guid, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::SetLocale(const yutovo_calculator::Language _language)
{
    language = _language;
    std::unique_lock<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetLocaleSolverTask(document, solver_guid, language, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::ListIdentifiers(uint code_id)
{
    std::unique_lock<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new ListIdentifiersSolverTask(document, solver_guid, code_id, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::RemoveSolver(const uint code_id)
{
    std::unique_lock<std::mutex> lock(tasks_mutex);
    break_tasks.emplace_front(nullptr);
    break_tasks.emplace_front(new RemoveSolverTask(document, solver_guid, code_id, logger)); //first of all break this solving
    break_next_circle = true;
}

#ifdef REMOTE_SOLVER
void Solver::MessageLoop(WebSocketPtr socket_, std::deque<SolverTaskPtr>& tasks_, std::atomic_bool& next_circle_)
#else
void Solver::MessageLoop(WebSocketPtr socket_, std::deque<SolverTaskPtr>& tasks_, std::atomic_bool& next_circle_, yutovo_solver::Session& _session)
#endif
{
    bool connected = false;
    bool connection_error = false;

    {
        std::unique_lock<std::mutex> lock(socket_mutex);
#ifdef EMSCRIPTEN
#ifdef REMOTE_SOLVER
        socket_.reset(new WebSocket(document->config, document->window));
#else
        socket_.reset(new WebSocket(document->config, document->window, _session));
#endif
#else
        try
        {
#ifdef REMOTE_SOLVER
            socket_.reset(new WebSocket(document->config, document->window));
#else
            socket_.reset(new WebSocket(document->config, document->window, _session));
#endif
        }
        catch (boost::system::system_error& ex)
        {
            LOG_ERROR("Error creating socket: {}", ex.code().value());
            throw;
        }
#endif
    }
    if (!socket_->Connect() || !socket_->IsOpen())
    {
        LOG_ERROR("Error connecting to the server: {}:{}", document->config.service_ip, document->config.service_port);
    }
    else
    {
#ifdef REMOTE_SOLVER
        LOG_INFO("Solver connected to the server: {}:{}, solver_guid:{}", document->config.service_ip, document->config.service_port, solver_guid);
#else
        LOG_INFO("Using builtin solver, solver_guid:{}", solver_guid);
#endif
        connected = true;
    }

    time_t now = time(0);
    time_t next = now;
    
    std::vector<SolverTaskPtr> temp_tasks;
    while (!exit)
    {
        bool empty = false;
        {
            std::unique_lock<std::mutex> lock(tasks_mutex);
            empty = tasks_.empty();
        }
        if (empty)
        {
            while (!next_circle_) //wait for tasks
            {
                std::this_thread::sleep_for(10ms);
                if (!socket_->IsOpen())
                {
                    next = time(0);
                    if (next - now >= document->config.reconnect_timeout)
                        break;
                }
                else if (!connected)
                {
#ifdef REMOTE_SOLVER
                    LOG_INFO("Solver connected to the server: {}:{}, solver_guid:{}", document->config.service_ip, document->config.service_port, solver_guid);
#else
                    LOG_INFO("Using builtin solver, solver_guid:{}", solver_guid);
#endif
                    connected = true;
                }
            }
        }
        else
        {
            std::this_thread::sleep_for(10ms);
        }

        next = time(0);

        if (connection_error)
        {
            if (next - now >= document->config.reconnect_timeout)
            {
                now = time(0);
                {
                    std::unique_lock<std::mutex> lock(socket_mutex);
#ifdef REMOTE_SOLVER
                    socket_.reset(new WebSocket(document->config, document->window)); //recreate the socket
#else
                    socket_.reset(new WebSocket(document->config, document->window, _session));
#endif
                }
                if (!socket_->Connect() || !socket_->IsOpen())
                {
                    LOG_ERROR("Error connecting to the server: {}:{}", document->config.service_ip, document->config.service_port);
                }
                else
                {
#ifdef REMOTE_SOLVER
                    LOG_INFO("Solver connected to the server: {}:{}, solver_guid:{}", document->config.service_ip, document->config.service_port, solver_guid);
#else
                    LOG_INFO("Using builtin solver, solver_guid:{}", solver_guid);
#endif
                    document->ReSolveErrors();
                    connection_error = false;
                }
            }
            else
                continue;
        }

        {
            std::unique_lock<std::mutex> lock(tasks_mutex);
            if (temp_tasks.empty())
            {
                while (!tasks_.empty() && tasks_.front() == nullptr)
                    tasks_.pop_front();
                if (tasks_.empty())
                {
                    next_circle_ = false;
                    continue;
                }
                while (!tasks_.empty() && tasks_.front() != nullptr)
                {
                    SolverTaskPtr& t = tasks_.front();
                    uint64_t now_m = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
                    t->delay -= now_m - t->cur_time;
                    if (t->delay <= 0)
                    {
                        temp_tasks.push_back(t);
                        tasks_.pop_front();
                    }
                    else
                    {
                        t->cur_time = now_m;
                        break;
                    }
                }
            }
        }

        Result result;
        int tries = 2;
        for (int i = 0; i < temp_tasks.size();)
        {
            SolverTaskPtr t = temp_tasks[i];
            {
                std::unique_lock<std::mutex> lock(current_solving_mutex);
                current_solving_id = t->id;
                current_code_id = t->code_id;
            }
            bool r = t->Execute(socket_, result);
            {
                std::unique_lock<std::mutex> lock(current_solving_mutex);
                current_solving_id.clear();
                current_code_id = 0;
            }
            if (!r)
            {
                if (result.error.error_code == yutovo_solver::ErrorCode::OPERATION_ERROR)
                {
                    {
                        std::unique_lock<std::mutex> lock(socket_mutex);
#ifdef REMOTE_SOLVER
                        socket_.reset(new WebSocket(document->config, document->window)); //recreate the socket
#else
                        socket_.reset(new WebSocket(document->config, document->window, _session)); //recreate the socket
#endif
                    }
                    if (!socket_->IsOpen())
                    {
                        LOG_ERROR("Error connecting to the server: {}:{}", document->config.service_ip, document->config.service_port);
                    }
                    else
                    {
#ifdef REMOTE_SOLVER
                        LOG_INFO("Solver connected to the server: {}:{}, solver_guid:{}", document->config.service_ip, document->config.service_port, solver_guid);
#else
                        LOG_INFO("Using builtin solver, solver_guid:{}", solver_guid);
#endif
                        document->ReSolveErrors();
                    }
                    if (socket_->IsOpen() && tries-- > 0)
                    {
                        --i; //it's just connected, try once more
                        continue;
                    }
                }
            }

            if (t->expression_type == ExpressionType::USER_SYMBOL)
                document->window->OnIdentifierChanged(t->id);

            if (!result.values.empty() || (result.values.empty() && result.type == yutovo_solver::ResultType::ARRAY_REAL) || 
                result.error.error_code != yutovo_solver::ErrorCode::OK)
            {
                result.guid = t->task_guid;
                document->PutResult(result);
            }
            
            if (result.error.error_code == yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR)
            {
                document->ReSolve(t->id); //re-solve the expression

                std::unique_lock<std::mutex> lock(tasks_mutex);
                tasks.emplace_back(new SetLocaleSolverTask(document, solver_guid, language, logger));
                tasks.emplace_back(new ListIdentifiersSolverTask(document, solver_guid, t->code_id, logger)); //for syntax highlight
                next_circle = true;
            }
            else if (result.error.error_code == yutovo_solver::ErrorCode::TIMEOUT_ERROR)
            {
                std::unique_lock<std::mutex> lock(tasks_mutex);
                break_tasks.emplace_back(new BreakSolverTask(t->id, document, solver_guid, t->code_id, true, logger)); //break the current solving
                break_next_circle = true;
            }

            if (result.error.error_code != yutovo_solver::ErrorCode::OPERATION_ERROR)
                temp_tasks.erase(temp_tasks.begin() + i);
            else
            {
                connection_error = true;
                ++i;
            }
        }
    }
}

void Solver::EraseSolveTasks(const LogicalId id)
{
    tasks.erase(std::remove_if(tasks.begin(), tasks.end(), 
        [id](SolverTaskPtr& task)
        {
            return task && task->id == id && task->expression_type == ExpressionType::SOLVE && 
                (dynamic_cast<RealSolverTask*>(task.get()) || dynamic_cast<IntegerSolverTask*>(task.get()) || 
                dynamic_cast<RationalSolverTask*>(task.get()) || dynamic_cast<AutoSolverTask*>(task.get()) || 
                dynamic_cast<ComplexSolverTask*>(task.get()) || dynamic_cast<ArrayRealSolverTask*>(task.get()) || 
                dynamic_cast<SymbolicRealSolverTask*>(task.get()) || dynamic_cast<SymbolicRationalSolverTask*>(task.get()) || 
                dynamic_cast<SymbolicComplexSolverTask*>(task.get()));
        }
        ), tasks.end());
}

}
