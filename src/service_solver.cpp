/*
 * Yutovo Solver
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "service_solver.h"
#include "service_config.h"
#include <yutovo-calculator/integer.h>
#include <giac/global.h>
#include <atomic>
#include <chrono>
#include <thread>

#ifdef YUTOVO_SOLVER_WORKER
#include "solver_process.h"
#endif

#ifdef _MSC_VER
#undef GetObject
#endif

namespace yutovo_solver
{

//Solver

Solver::Solver(const std::string& _document_guid, const std::string& _solver_guid, const yutovo_calculator::Language _language) :
    document_guid(_document_guid), 
    solver_guid(_solver_guid),
    locale{_language}
{
}

void Solver::SetMaxTime(const uint64_t _max_time)
{
    max_time = _max_time;
}

void Solver::ReplyOk(rapidjson::Document& reply)
{
    rapidjson::Value ok;
    ok.SetObject();
    auto& alloc = reply.GetAllocator();
    ok.AddMember("error_code", (int)ErrorCode::OK, alloc);
    reply.AddMember("result", ok, alloc);
}

void Solver::ReplyError(const ErrorCode error_code, rapidjson::Document& reply)
{
    rapidjson::Value error;
    error.SetObject();
    auto& alloc = reply.GetAllocator();
    error.AddMember("error_code", (int)error_code, alloc);
    reply.AddMember("error", error, alloc);
}

void Solver::ReplyError(const yutovo_calculator::ParserException& ex, rapidjson::Document& reply)
{
    rapidjson::Value error;
    error.SetObject();
    auto& alloc = reply.GetAllocator();
    error.AddMember("id", LogicalIdToValue(reply, ex.id), alloc);
    error.AddMember("error_code", (int)ErrorCode::PARSER_ERROR, alloc);
    error.AddMember("parser_error_code", ex.ex_id, alloc);
    error.AddMember("pos", ex.pos, alloc);
    error.AddMember("size", ex.size, alloc);
    error.AddMember("line", ex.line, alloc);
    rapidjson::Value s((boost::locale::conv::utf_to_utf<char>(ex.description)).c_str(), alloc);
    error.AddMember("description", s, alloc);
    reply.AddMember("error", error, alloc);
}

void Solver::AddUnit(rapidjson::Value& reply, const Unit& unit, rapidjson::Document::AllocatorType& alloc)
{
    if (unit.IsEmpty())
        return;

    rapidjson::Value _unit;
    _unit.SetObject();
    rapidjson::Value d(rapidjson::kArrayType);
    for (auto& u : unit.unit)
    {
        rapidjson::Value _u;
        _u.SetObject();
        rapidjson::Value s((boost::locale::conv::utf_to_utf<char>(u.first)).c_str(), alloc);
        _u.AddMember("name", s, alloc);
        if (u.second != 1)
            _u.AddMember("power", u.second, alloc);
        d.PushBack(_u, alloc);
    }
    if (unit.system != U"")
    {
        rapidjson::Value s((boost::locale::conv::utf_to_utf<char>(unit.system)).c_str(), alloc);
        _unit.AddMember("system", s, alloc);
    }
    _unit.AddMember("value", d, alloc);
    reply.AddMember("unit", _unit, alloc);
}

void Solver::AddCastUnits(rapidjson::Document& reply, const std::vector<Unit>& cast_units)
{
    if (cast_units.empty())
        return;
    
    //sort the cast units by those systems
    std::map<std::u32string, std::vector<Unit>> system_units;
    for (const Unit& unit : cast_units)
    {
        if (unit.system == U"")
            system_units[U"SI"].push_back(unit);
        else
            system_units[unit.system].push_back(unit);
    }

    //make json arrays
    auto& alloc = reply.GetAllocator();
    rapidjson::Value systems(rapidjson::kArrayType);
    for (auto& s : system_units)
    {
        rapidjson::Value system;
        system.SetObject();
        system.AddMember("system", rapidjson::Value((boost::locale::conv::utf_to_utf<char>(s.first)).c_str(), alloc), alloc);

        rapidjson::Value units(rapidjson::kArrayType);
        for (auto& unit : s.second)
        {
            rapidjson::Value unit_array(rapidjson::kArrayType);
            for (auto& u : unit.unit)
            {
                rapidjson::Value _u;
                _u.SetObject();
                rapidjson::Value s((boost::locale::conv::utf_to_utf<char>(u.first)).c_str(), alloc);
                _u.AddMember("name", s, alloc);
                if (u.second != 1)
                    _u.AddMember("power", u.second, alloc);
                unit_array.PushBack(_u, alloc);
            }
            units.PushBack(unit_array, alloc);
        }
        system.AddMember("units", units, alloc);

        systems.PushBack(system, alloc);
    }

    reply.AddMember("cast_units", systems, alloc);
}

void Solver::AddDependencies(rapidjson::Document& reply, const std::vector<std::u32string>* dependencies)
{
    if (dependencies->empty())
        return;
    
    auto& alloc = reply.GetAllocator();
    rapidjson::Value d(rapidjson::kArrayType);
    for (auto& str : *dependencies)
    {
        rapidjson::Value s((boost::locale::conv::utf_to_utf<char>(str)).c_str(), alloc);
        d.PushBack(s, alloc);
    }
    while (reply.HasMember("dependencies")) //remove the previous dependencies added by former tries
        reply.RemoveMember("dependencies");
    reply.AddMember("dependencies", d, alloc);
}

bool Solver::GetLogicalId(const rapidjson::Document& request, LogicalId& id)
{
    if (!request.HasMember("id") || !request["id"].IsArray())
        return false;
    rapidjson::GenericArray arr = request["id"].GetArray();
    id.clear();
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
    {
        if (!arr[i].IsInt())
            return false;
        id.push_back(arr[i].GetInt());
    }
    return true;
}

bool Solver::GetTimestamp(const rapidjson::Document& request, uint64_t& time_stamp)
{
    if (!request.HasMember("timestamp") || !request["timestamp"].IsUint64())
        return false;
    time_stamp = request["timestamp"].GetUint64();
    return true;
}

rapidjson::Value Solver::LogicalIdToValue(rapidjson::Document& reply, const LogicalId& id)
{
    auto& alloc = reply.GetAllocator();
    rapidjson::Value d(rapidjson::kArrayType);
    for (int i : id)
        d.PushBack(i, alloc);
    return d;
}

bool Solver::GetUnit(const rapidjson::Document& request, Unit& unit)
{
    if (!request.HasMember("unit") || !request["unit"].IsObject())
        return false;
    
    const auto& _unit = request["unit"].GetObject();
    if (!_unit.HasMember("value") || !_unit["value"].IsArray())
        return false;
    if (_unit.HasMember("system"))
        unit.system = ToUtfString(_unit["system"].GetString());
    rapidjson::GenericArray arr = _unit["value"].GetArray();
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
    {
        if (!arr[i].IsObject())
            return false;
        const auto& u = arr[i].GetObject();
        std::u32string name;
        int power = 1;
        if (!u.HasMember("name") || !u["name"].IsString())
            return false;
        name = ToUtfString(u["name"].GetString());
        if (u.HasMember("power") && u["power"].IsInt())
            power = u["power"].GetInt();
        unit.unit.push_back(std::make_pair(name, power));
    }
    return true;
}

//CalculatorSolver

CalculatorSolver::CalculatorSolver(const std::string& _document_guid, const std::string& _solver_guid, ParserContextPtr _parser_context, 
    const yutovo_calculator::Language _language, uint64_t _max_time, const std::string& _logs_path, bool _log_console, bool _log_file) :
    Solver(_document_guid, _solver_guid, _language),
    real_parser(0, _language),
    integer_parser(0, _language),
    rational_parser(0, _language),
    complex_parser(0, _language),
    array_real_parser(0, _language),
    symbolic_real_parser(10, _language),
    symbolic_rational_parser(10, _language),
    symbolic_complex_parser(10, _language),
    parser_context(_parser_context),
    logger(Logger::GetInstance(_logs_path + "/yutovo-solver", "calculator-solver", _log_console, _log_file))
{
    max_time = _max_time;
#ifdef YUTOVO_SOLVER_WORKER
    //a null parser context means proxy mode: the parsers live in the calculator worker process
    if (!parser_context)
        process = SolverProcess::GetProcess(_document_guid, _max_time, _logs_path, _log_console, _log_file);
#endif
    logger->Info("Calculator Solver started: {}", solver_guid);
}

CalculatorSolver::~CalculatorSolver()
{
#ifdef YUTOVO_SOLVER_WORKER
    if (process)
        process->SendRemoveSolver(solver_guid);
#endif
    logger->Info("Calculator Solver finished: {}", solver_guid);
}

void CalculatorSolver::SetMaxTime(const uint64_t _max_time)
{
    max_time = _max_time;
#ifdef YUTOVO_SOLVER_WORKER
    if (process)
        process->SetMaxTime(_max_time);
#endif
}

void CalculatorSolver::Solve(const rapidjson::Document& request, rapidjson::Document& reply)
{
    idle_time = time(nullptr);

#ifdef YUTOVO_SOLVER_WORKER
    //proxy mode: the parsers live in the calculator worker process
    if (!parser_context)
    {
        if (process)
            process->SendAction(solver_guid, "solve", request, reply, (int)locale.language);
        else
            ReplyError(ErrorCode::SOLVER_ERROR, reply);
        return;
    }
#endif

    reply.SetObject();
    if (just_started)
    {
        just_started = false;
        ReplyError(ErrorCode::SOLVER_RESTARTED_ERROR, reply);
        return;
    }

    if (!request.HasMember("result_type") || !request["result_type"].IsInt() ||
        !request.HasMember("expression") || !request["expression"].IsString())
    {
        logger->Error("result_type error");
        ReplyError(ErrorCode::NO_FIELD_ERROR, reply);
        return;
    }

    {
        std::lock_guard<std::mutex> lock(solving_id_lock);
        if (!GetLogicalId(request, solving_id))
        {
            logger->Error("id error");
            throw ServiceException{ErrorCode::NO_FIELD_ERROR};
        }
    }

    solving_time_stamp = 0;
    if (!GetTimestamp(request, solving_time_stamp))
    {
        logger->Error("result_type error");
        ReplyError(ErrorCode::NO_FIELD_ERROR, reply);
        return;
    }

    //clear the giac interrupt flags that could be left from a previous break
    giac::ctrl_c = false;
    giac::interrupted = false;

    {
        //check for breaking the solving before it's started
        std::lock_guard<std::mutex> l(break_lock);
        auto it = break_solvings.find(solving_id);
        if (it != break_solvings.end())
        {
            if (it->second >= solving_time_stamp)
            {
                break_solvings.erase(it);
                ReplyError(ParserException(solving_id, ParserExceptionCode::Break, -1, -1), reply);
                return;
            }
        }
    }

    std::vector<std::u32string> dependencies;
    ResultType result_type = (ResultType)request["result_type"].GetInt();
    ExpressionType expression_type = ExpressionType::NONE;
    if (request.HasMember("expression_type") && request["expression_type"].IsInt())
        expression_type = (ExpressionType)request["expression_type"].GetInt();

    //on the web there is no worker process to kill, a stuck giac evaluation is interrupted by the giac flags instead
    SolveTimeoutWatchdog timeout_watchdog(max_time);

    switch (result_type)
    {
    case ResultType::AUTO:
        {
            std::vector<ResultType> results_order;
            bool exit_on_success = true;
            if (expression_type == ExpressionType::USER_SYMBOL)
                exit_on_success = false;
            if (!request.HasMember("results_order") || !request["results_order"].IsArray())
            {
                results_order.push_back(ResultType::REAL);
                results_order.push_back(ResultType::INTEGER);
                results_order.push_back(ResultType::RATIONAL);
                results_order.push_back(ResultType::COMPLEX);
                results_order.push_back(ResultType::ARRAY_REAL);
                results_order.push_back(ResultType::SYMBOLIC_REAL);
                results_order.push_back(ResultType::SYMBOLIC_RATIONAL);
                results_order.push_back(ResultType::SYMBOLIC_COMPLEX);
                exit_on_success = false;
            }
            else
            {
                const rapidjson::Value& r = request["results_order"];
                for (rapidjson::SizeType i = 0; i < r.Size(); i++)
                {
                    if (!r[i].IsInt())
                    {
                        logger->Error("results_order error");
                        ReplyError(ErrorCode::FIELD_ERROR, reply);
                        return;
                    }
                    results_order.push_back((ResultType)r[i].GetInt());
                }
            }

            //try all the parsers in the requered order until one of them parses
            rapidjson::Document error_reply;
            error_reply.CopyFrom(reply, error_reply.GetAllocator());
            rapidjson::Document first_reply;
            rapidjson::Document user_symbol_reply;
            bool user_symbol_success = false;
            bool error = false;
            yutovo_calculator::ParserException last_exception(yutovo_calculator::LogicalId{}, yutovo_calculator::ParserExceptionCode::None, -1, 0);

            std::lock_guard<std::mutex> lock(parsers_lock);
            for (size_t i = 0; i < results_order.size(); ++i)
            {
                ResultType t = results_order[i];
                rapidjson::Document r;
                r.SetObject();
                try
                {
                    switch (t)
                    {
                    case ResultType::REAL:
                        SolveReal(request, r, &dependencies);
                        reply.CopyFrom(r, reply.GetAllocator());
                        if (exit_on_success)
                            return;
                        if (expression_type == ExpressionType::USER_SYMBOL && !user_symbol_success)
                        {
                            user_symbol_reply.CopyFrom(reply, reply.GetAllocator());
                            user_symbol_success = true;
                        }
                        break;
                    case ResultType::INTEGER:
                        SolveInteger(request, r, &dependencies);
                        reply.CopyFrom(r, reply.GetAllocator());
                        if (exit_on_success)
                            return;
                        if (expression_type == ExpressionType::USER_SYMBOL && !user_symbol_success)
                        {
                            user_symbol_reply.CopyFrom(reply, reply.GetAllocator());
                            user_symbol_success = true;
                        }
                        break;
                    case ResultType::RATIONAL:
                        SolveRational(request, r, &dependencies);
                        reply.CopyFrom(r, reply.GetAllocator());
                        if (exit_on_success)
                            return;
                        if (expression_type == ExpressionType::USER_SYMBOL && !user_symbol_success)
                        {
                            user_symbol_reply.CopyFrom(reply, reply.GetAllocator());
                            user_symbol_success = true;
                        }
                        break;
                    case ResultType::COMPLEX:
                        SolveComplex(request, r, &dependencies);
                        reply.CopyFrom(r, reply.GetAllocator());
                        if (exit_on_success)
                            return;
                        if (expression_type == ExpressionType::USER_SYMBOL && !user_symbol_success)
                        {
                            user_symbol_reply.CopyFrom(reply, reply.GetAllocator());
                            user_symbol_success = true;
                        }
                        break;
                    case ResultType::ARRAY_REAL:
                        SolveArrayReal(request, r, &dependencies);
                        reply.CopyFrom(r, reply.GetAllocator());
                        if (exit_on_success)
                            return;
                        if (expression_type == ExpressionType::USER_SYMBOL && !user_symbol_success)
                        {
                            user_symbol_reply.CopyFrom(reply, reply.GetAllocator());
                            user_symbol_success = true;
                        }
                        break;
                    case ResultType::SYMBOLIC_REAL:
                        SolveSymbolicReal(request, r, &dependencies);
                        reply.CopyFrom(r, reply.GetAllocator());
                        if (exit_on_success)
                            return;
                        if (expression_type == ExpressionType::USER_SYMBOL && !user_symbol_success)
                        {
                            user_symbol_reply.CopyFrom(reply, reply.GetAllocator());
                            user_symbol_success = true;
                        }
                        break;
                    case ResultType::SYMBOLIC_RATIONAL:
                        SolveSymbolicRational(request, r, &dependencies);
                        reply.CopyFrom(r, reply.GetAllocator());
                        if (exit_on_success)
                            return;
                        if (expression_type == ExpressionType::USER_SYMBOL && !user_symbol_success)
                        {
                            user_symbol_reply.CopyFrom(reply, reply.GetAllocator());
                            user_symbol_success = true;
                        }
                        break;
                    case ResultType::SYMBOLIC_COMPLEX:
                        SolveSymbolicComplex(request, r, &dependencies);
                        reply.CopyFrom(r, reply.GetAllocator());
                        if (exit_on_success)
                            return;
                        if (expression_type == ExpressionType::USER_SYMBOL && !user_symbol_success)
                        {
                            user_symbol_reply.CopyFrom(reply, reply.GetAllocator());
                            user_symbol_success = true;
                        }
                        break;
                    case ResultType::AUTO:
                    case ResultType::NONE:
                        ReplyError(ErrorCode::OPERATION_ERROR, reply);
                        reply.CopyFrom(error_reply, reply.GetAllocator());
                        return;
                    }
                    if (i == 0)
                        first_reply.CopyFrom(reply, first_reply.GetAllocator());
                }
                catch (yutovo_calculator::BreakException& ex)
                {
                    logger->Error("Break exception");
                    ReplyError(ex, error_reply);
                    reply.CopyFrom(error_reply, reply.GetAllocator());
                    return;
                }
                catch (yutovo_calculator::TimeExceedException& ex)
                {
                    logger->Error("Time exceed exception");
                    ReplyError(ex, error_reply);
                    reply.CopyFrom(error_reply, reply.GetAllocator());
                    return;
                }
                catch (yutovo_calculator::ParserException& ex)
                {
                    logger->Error("Parser exception: {}", (int)ex.ex_id);
                    //take an exception whose position is further away
                    if (last_exception.ex_id == yutovo_calculator::ParserExceptionCode::None || 
                        (ex.pos + ex.size > last_exception.pos + last_exception.size))
                    {
                        if (error_reply.HasMember("error"))
                            error_reply.RemoveMember("error");
                        ReplyError(ex, error_reply);
                        AddDependencies(error_reply, &dependencies);
                        if (!exit_on_success)
                            reply.CopyFrom(error_reply, reply.GetAllocator());
                        error = true;
                        last_exception = ex;
                    }
                }
                catch (ServiceException& ex)
                {
                    if (i == 0)
                    {
                        ReplyError(ex.error_code, error_reply);
                        AddDependencies(error_reply, &dependencies);
                        if (!exit_on_success)
                            reply.CopyFrom(error_reply, reply.GetAllocator());
                        error = true;
                    }
                }
            }

            if (expression_type == ExpressionType::USER_SYMBOL && user_symbol_success)
                reply.CopyFrom(user_symbol_reply, reply.GetAllocator());
            else if (exit_on_success)
            {
                //none of the parsers has parsed
                reply.CopyFrom(error_reply, reply.GetAllocator());
            }
            else if (!error)
            {
                reply.CopyFrom(first_reply, reply.GetAllocator());
            }
        }
        break;
    case ResultType::REAL:
    case ResultType::INTEGER:
    case ResultType::RATIONAL:
    case ResultType::COMPLEX:
    case ResultType::ARRAY_REAL:
    case ResultType::SYMBOLIC_REAL:
    case ResultType::SYMBOLIC_RATIONAL:
    case ResultType::SYMBOLIC_COMPLEX:
        try
        {
            std::lock_guard<std::mutex> lock(parsers_lock);
            switch (result_type)
            {
            case ResultType::REAL:
                SolveReal(request, reply, &dependencies);
                break;
            case ResultType::INTEGER:
                SolveInteger(request, reply, &dependencies);
                break;
            case ResultType::RATIONAL:
                SolveRational(request, reply, &dependencies);
                break;
            case ResultType::COMPLEX:
                SolveComplex(request, reply, &dependencies);
                break;
            case ResultType::ARRAY_REAL:
                SolveArrayReal(request, reply, &dependencies);
                break;
            case ResultType::SYMBOLIC_REAL:
                SolveSymbolicReal(request, reply, &dependencies);
                break;
            case ResultType::SYMBOLIC_RATIONAL:
                SolveSymbolicRational(request, reply, &dependencies);
                break;
            case ResultType::SYMBOLIC_COMPLEX:
                SolveSymbolicComplex(request, reply, &dependencies);
                break;
            case ResultType::AUTO:
            case ResultType::NONE:
                ReplyError(ErrorCode::OPERATION_ERROR, reply);
                return;
            }
        }
        catch (yutovo_calculator::ParserException& ex)
        {
            ReplyError(ex, reply);
            AddDependencies(reply, &dependencies);
        }
        catch (ServiceException& ex)
        {
            ReplyError(ex.error_code, reply);
            AddDependencies(reply, &dependencies);
        }
        break;
    case ResultType::NONE:
        ReplyError(ErrorCode::OPERATION_ERROR, reply);
        return;
    }

    //an evaluation interrupted by the deadline is reported as a time exceed, like a killed worker process
    if (timeout_watchdog.fired.load() && (reply.HasMember("error") || reply.ObjectEmpty()))
    {
        logger->Error("Solve interrupted by the time watchdog");
        ReplyError(ParserException(solving_id, ParserExceptionCode::TimeExceed, -1, -1), reply);
    }
}

void CalculatorSolver::BreakSolving(const rapidjson::Document& request, rapidjson::Document& reply)
{
    LogicalId id;
    if (!GetLogicalId(request, id))
    {
        logger->Error("id error");
        throw ServiceException{ErrorCode::NO_FIELD_ERROR};
    }

    uint64_t time_stamp = 0;
    if (!GetTimestamp(request, time_stamp))
    {
        logger->Error("result_type error");
        ReplyError(ErrorCode::NO_FIELD_ERROR, reply);
        return;
    }

    bool wait = true;
    if (request.HasMember("wait") && request["wait"].IsBool())
        wait = request["wait"].GetBool();

#ifdef YUTOVO_SOLVER_WORKER
    //proxy mode: forward the break to the calculator worker process
    if (!parser_context)
    {
        if (process)
            process->SendBreak(solver_guid, request, wait);
        ReplyOk(reply);
        return;
    }
#endif

    reply.SetObject();

    {
        std::lock_guard<std::mutex> lock(solving_id_lock);
        if (id == solving_id && time_stamp >= solving_time_stamp)
        {
#ifndef YUTOVO_SOLVER_TEST_NO_INTERRUPT
            //the test build of the worker cannot stop itself, so the parent kill path can be tested
            parser_context->break_solving = true; //break the current solving
            giac::ctrl_c = true; //softly interrupt the giac evaluation loops
            giac::interrupted = true;
#endif
        }
        else if (wait)
        {
            //delay breaking the future solving
            std::lock_guard<std::mutex> l(break_lock);
            break_solvings[id] = time_stamp;
        }
    }

    ReplyOk(reply);
}

void CalculatorSolver::RemoveIdentifier(const rapidjson::Document& request, rapidjson::Document& reply)
{
    idle_time = time(nullptr);

#ifdef YUTOVO_SOLVER_WORKER
    if (!parser_context)
    {
        if (process)
            process->SendAction(solver_guid, "remove_identifier", request, reply, (int)locale.language);
        else
            ReplyError(ErrorCode::SOLVER_ERROR, reply);
        return;
    }
#endif

    if (!request.HasMember("expression") || !request["expression"].IsString())
    {
        logger->Error("expression error");
        ReplyError(ErrorCode::NO_FIELD_ERROR, reply);
        return;
    }

    LogicalId id;
    if (!GetLogicalId(request, id))
        return;
    std::string identifier = request["expression"].GetString();

    try
    {
        std::lock_guard<std::mutex> lock(parsers_lock);
        real_parser.RemoveIdentifier(id, identifier);
        integer_parser.RemoveIdentifier(id, identifier);
        rational_parser.RemoveIdentifier(id, identifier);
        complex_parser.RemoveIdentifier(id, identifier);
        array_real_parser.RemoveIdentifier(id, identifier);
        symbolic_real_parser.RemoveIdentifier(id, identifier);
    }
    catch (yutovo_calculator::ParserException ex)
    {
        ReplyError(ex, reply);
        return;
    }
}

void CalculatorSolver::RemoveUserIdentifiers(const rapidjson::Document& request, rapidjson::Document& reply)
{
    idle_time = time(nullptr);

#ifdef YUTOVO_SOLVER_WORKER
    if (!parser_context)
    {
        if (process)
            process->SendAction(solver_guid, "remove_user_identifiers", request, reply, (int)locale.language);
        else
            ReplyError(ErrorCode::SOLVER_ERROR, reply);
        return;
    }
#endif

    try
    {
        std::lock_guard<std::mutex> lock(parsers_lock);
        real_parser.RemoveUserIdentifiers();
        integer_parser.RemoveUserIdentifiers();
        rational_parser.RemoveUserIdentifiers();
        complex_parser.RemoveUserIdentifiers();
        array_real_parser.RemoveUserIdentifiers();
        symbolic_real_parser.RemoveUserIdentifiers();
    }
    catch (yutovo_calculator::ParserException ex)
    {
        ReplyError(ex, reply);
    }
}

void CalculatorSolver::ListIdentifiers(const rapidjson::Document& request, rapidjson::Document& reply)
{
    idle_time = time(nullptr);

#ifdef YUTOVO_SOLVER_WORKER
    if (!parser_context)
    {
        if (process)
            process->SendAction(solver_guid, "list_identifiers", request, reply, (int)locale.language);
        else
            ReplyError(ErrorCode::SOLVER_ERROR, reply);
        return;
    }
#endif

    auto& alloc = reply.GetAllocator();

    std::lock_guard<std::mutex> lock(parsers_lock);
    //collect all the identifiers from all the parsers
    std::vector<std::u32string> builtin_functions, user_functions, builtin_variables, builtin_operations, user_strings;
    real_parser.ListBuiltinVariables(builtin_variables);
    integer_parser.ListBuiltinVariables(builtin_variables);
    rational_parser.ListBuiltinVariables(builtin_variables);
    complex_parser.ListBuiltinVariables(builtin_variables);
    array_real_parser.ListBuiltinVariables(builtin_variables);
    symbolic_real_parser.ListBuiltinVariables(builtin_variables);

    std::vector<std::pair<std::u32string, std::u32string>> user_variables;
    real_parser.ListUserVariables(user_variables);
    integer_parser.ListUserVariables(user_variables);
    rational_parser.ListUserVariables(user_variables);
    complex_parser.ListUserVariables(user_variables);
    array_real_parser.ListUserVariables(user_variables);
    symbolic_real_parser.ListUserVariables(user_variables);

    std::sort(builtin_variables.begin(), builtin_variables.end());
    builtin_variables.erase(std::unique(builtin_variables.begin(), builtin_variables.end()), builtin_variables.end());

    std::sort(user_variables.begin(), user_variables.end());
    user_variables.erase(std::unique(user_variables.begin(), user_variables.end()), user_variables.end());

    rapidjson::Value variables_arr(rapidjson::kArrayType);
    for (auto& u : builtin_variables)
    {
        rapidjson::Value var;
        var.SetObject();
        var.AddMember("name", rapidjson::Value((boost::locale::conv::utf_to_utf<char>(u)).c_str(), alloc), alloc);
        variables_arr.PushBack(var, alloc);
    }

    for (auto& u : user_variables)
    {
        rapidjson::Value var;
        var.SetObject();
        var.AddMember("name", rapidjson::Value((boost::locale::conv::utf_to_utf<char>(u.first)).c_str(), alloc), alloc);
        if (!u.second.empty())
            var.AddMember("description", rapidjson::Value((boost::locale::conv::utf_to_utf<char>(u.second)).c_str(), alloc), alloc);
        variables_arr.PushBack(var, alloc);
    }

    reply.AddMember("Variables", variables_arr, alloc);

    real_parser.ListBuiltinFunctions(builtin_functions);
    integer_parser.ListBuiltinFunctions(builtin_functions);
    rational_parser.ListBuiltinFunctions(builtin_functions);
    complex_parser.ListBuiltinFunctions(builtin_functions);
    array_real_parser.ListBuiltinFunctions(builtin_functions);
    symbolic_real_parser.ListBuiltinFunctions(builtin_functions);

    //remove duplicates
    std::sort(builtin_functions.begin(), builtin_functions.end());
    builtin_functions.erase(std::unique(builtin_functions.begin(), builtin_functions.end()), builtin_functions.end());

    rapidjson::Value functions_arr(rapidjson::kArrayType);
    for (auto& f : builtin_functions)
    {
        rapidjson::Value func;
        func.SetObject();
        func.AddMember("name", rapidjson::Value((boost::locale::conv::utf_to_utf<char>(f)).c_str(), alloc), alloc);
        functions_arr.PushBack(func, alloc);
    }

    real_parser.ListUserFunctions(user_functions);
    integer_parser.ListUserFunctions(user_functions);
    rational_parser.ListUserFunctions(user_functions);
    complex_parser.ListUserFunctions(user_functions);
    array_real_parser.ListUserFunctions(user_functions);
    symbolic_real_parser.ListUserFunctions(user_functions);

    //remove duplicates
    std::sort(user_functions.begin(), user_functions.end());
    user_functions.erase(std::unique(user_functions.begin(), user_functions.end()), user_functions.end());

    for (auto& f : user_functions)
    {
        rapidjson::Value func;
        func.SetObject();
        func.AddMember("name", rapidjson::Value((boost::locale::conv::utf_to_utf<char>(f)).c_str(), alloc), alloc);
        functions_arr.PushBack(func, alloc);
    }

    reply.AddMember("Functions", functions_arr, alloc);

    real_parser.ListBuiltinOperations(builtin_operations);
    integer_parser.ListBuiltinOperations(builtin_operations);
    rational_parser.ListBuiltinOperations(builtin_operations);
    complex_parser.ListBuiltinOperations(builtin_operations);
    array_real_parser.ListBuiltinOperations(builtin_operations);
    symbolic_real_parser.ListBuiltinOperations(builtin_operations);

    //remove duplicates
    std::sort(builtin_operations.begin(), builtin_operations.end());
    builtin_operations.erase(std::unique(builtin_operations.begin(), builtin_operations.end()), builtin_operations.end());

    rapidjson::Value operations_arr(rapidjson::kArrayType);
    for (auto& f : builtin_operations)
    {
        rapidjson::Value op;
        op.SetObject();
        op.AddMember("name", rapidjson::Value((boost::locale::conv::utf_to_utf<char>(f)).c_str(), alloc), alloc);
        operations_arr.PushBack(op, alloc);
    }

    reply.AddMember("Operations", operations_arr, alloc);

    real_parser.ListUserStrings(user_strings);
    integer_parser.ListUserStrings(user_strings);
    rational_parser.ListUserStrings(user_strings);
    complex_parser.ListUserStrings(user_strings);
    array_real_parser.ListUserStrings(user_strings);
    symbolic_real_parser.ListUserStrings(user_strings);

    //remove duplicates
    std::sort(user_strings.begin(), user_strings.end());
    user_strings.erase(std::unique(user_strings.begin(), user_strings.end()), user_strings.end());

    rapidjson::Value strings_arr(rapidjson::kArrayType);
    for (auto& f : user_strings)
    {
        rapidjson::Value str;
        str.SetObject();
        str.AddMember("name", rapidjson::Value((boost::locale::conv::utf_to_utf<char>(f)).c_str(), alloc), alloc);
        strings_arr.PushBack(str, alloc);
    }

    reply.AddMember("Strings", strings_arr, alloc);

    struct UnitsCategory
    {
        UnitsCategory(const std::u32string& _category) : 
            category(_category)
        {
        }

        std::u32string category;
        std::vector<std::pair<std::u32string, std::u32string>> units;
    };

    struct Units
    {
        Units(const std::u32string& _system) : 
            system(_system)
        {
        }

        void AddCategory(const std::u32string& category)
        {
            categories.push_back(UnitsCategory(category));
        }

        std::u32string system;
        std::vector<UnitsCategory> categories;
    };

    static std::map<std::u32string, std::string> physical_values = 
        {
            {U"(m)", "length"},
            {U"(kg)", "mass"},
            {U"(s)", "time"},
            {U"(mol)", "amount of matter"},
            {U"(cd)", "luminosity"},
            {U"(K)", "temperature"},
            {U"(m^2)", "square"},
            {U"(m^3)", "volume"},
            {U"(1/(s))", "frequency"},
            {U"((kg*m)/(s^2))", "force"},
            {U"((kg*m^2)/(s^2))", "energy"},
            {U"((kg)/(m*s^2))", "pressure"},
            {U"((kg*m^2)/(s^3))", "power"},
            {U"(cd*sr)", "luminous flux"},
            {U"((cd*sr)/(m^2))", "illuminance"},
            {U"(A*s)", "electrical charge"},
            {U"(A)", "electric current"},
            {U"((kg*m^2)/(s^3*A))", "electrical potential"},
            {U"((kg*m^2)/(s^3*A^2))", "electrical resistance"},
            {U"((A^2*s^4)/(kg*m^2))", "electrical capacity"},
            {U"((kg*m^2)/(s^2*A^2))", "electrical inductance"},
            {U"((s^3*A^2)/(kg*m^2))", "electrical conductance"},
            {U"((kg)/(s^2*A))", "magnetic field"},
            {U"((kg*m^2)/(s^2*A))", "magnetic flux"},
            {U"((m^2)/(s^2))", "ionizing radiation"},

            {U"(м)", "length"},
            {U"(кг)", "mass"},
            {U"(с)", "time"},
            {U"(моль)", "amount of matter"},
            {U"(Кд)", "luminosity"},
            {U"(К)", "temperature"},
            {U"(м^2)", "square"},
            {U"(м^3)", "volume"},
            {U"(1/(с))", "frequency"},
            {U"((кг*м)/(с^2))", "force"},
            {U"((кг*м^2)/(с^2))", "energy"},
            {U"((кг)/(м*с^2))", "pressure"},
            {U"((кг*м^2)/(с^3))", "power"},
            {U"(Кд*ср)", "luminous flux"},
            {U"((Кд*ср)/(м^2))", "illuminance"},
            {U"(А*с)", "electrical charge"},
            {U"(А)", "electric current"},
            {U"((кг*м^2)/(с^3*А))", "electrical potential"},
            {U"((кг*м^2)/(с^3*А^2))", "electrical resistance"},
            {U"((А^2*с^4)/(кг*м^2))", "electrical capacity"},
            {U"((кг*м^2)/(с^2*А^2))", "electrical inductance"},
            {U"((с^3*А^2)/(кг*м^2))", "electrical conductance"},
            {U"((кг)/(с^2*А))", "magnetic field"},
            {U"((кг*м^2)/(с^2*А))", "magnetic flux"},
            {U"((м^2)/(с^2))", "ionizing radiation"}
        };
    
    static std::map<std::u32string, std::string> others_values = 
        {
            {U"(bit)", "information"},
            {U"", "rest"},

            {U"(бит)", "information"}
        };

    std::vector<Units> units;
    //set the right order
    units.push_back(Units(U"SI"));
    units[0].AddCategory(U"length");
    units[0].AddCategory(U"mass");
    units[0].AddCategory(U"time");
    units[0].AddCategory(U"amount of matter");
    units[0].AddCategory(U"luminosity");
    units[0].AddCategory(U"temperature");
    units[0].AddCategory(U"square");
    units[0].AddCategory(U"volume");
    units[0].AddCategory(U"frequency");
    units[0].AddCategory(U"force");
    units[0].AddCategory(U"energy");
    units[0].AddCategory(U"pressure");
    units[0].AddCategory(U"power");
    units[0].AddCategory(U"luminous flux");
    units[0].AddCategory(U"illuminance");
    units[0].AddCategory(U"electrical charge");
    units[0].AddCategory(U"electric current");
    units[0].AddCategory(U"electrical potential");
    units[0].AddCategory(U"electrical resistance");
    units[0].AddCategory(U"electrical capacity");
    units[0].AddCategory(U"electrical inductance");
    units[0].AddCategory(U"electrical conductance");
    units[0].AddCategory(U"magnetic field");
    units[0].AddCategory(U"magnetic flux");
    units[0].AddCategory(U"ionizing radiation");

    units.push_back(Units(U"rus"));
    units[1].AddCategory(U"length");
    units[1].AddCategory(U"mass");
    units[1].AddCategory(U"time");
    units[1].AddCategory(U"volume");

    units.push_back(Units(U"us"));
    units[2].AddCategory(U"length");
    units[2].AddCategory(U"mass");
    units[2].AddCategory(U"time");
    units[2].AddCategory(U"volume");

    units.push_back(Units(U"Others"));
    units[3].AddCategory(U"information");
    units[3].AddCategory(U"rest");

    std::vector<CustomUnit<yutovo_calculator::Real>> real_units;
    real_parser.ListUserUnits(real_units);

    for (auto& unit : real_units)
    {
        std::u32string unit_str = unit.value.unit.ToString();
        //search in physical
        auto it = physical_values.find(unit_str);
        if (it != physical_values.end())
        {
            auto u_it = std::find_if(units.begin(), units.end(), 
                [system = unit.system](auto& u)
                {
                    return u.system == system;
                });
            if (u_it != units.end())
            {
                std::vector<UnitsCategory>& categories = u_it->categories;
                auto c_it = std::find_if(categories.begin(), categories.end(), 
                    [c = ToUtfString(it->second)](auto& cat)
                    {
                        return cat.category == c;
                    });
                if (c_it != categories.end())
                    c_it->units.push_back(std::make_pair(unit.name, unit.description));
                else
                {
                    UnitsCategory c(ToUtfString(it->second));
                    c.units.push_back(std::make_pair(unit.name, unit.description));
                    u_it->categories.push_back(c);
                }
            }
            continue;
        }

        //search in others
        it = others_values.find(unit_str);
        if (it != others_values.end())
        {
            auto u_it = std::find_if(units.begin(), units.end(), 
                [](auto& u)
                {
                    return u.system == U"Others";
                });
            if (u_it != units.end())
            {
                std::vector<UnitsCategory>& categories = u_it->categories;
                auto c_it = std::find_if(categories.begin(), categories.end(), 
                    [c = ToUtfString(it->second)](auto& cat)
                    {
                        return cat.category == c;
                    });
                if (c_it != categories.end())
                    c_it->units.push_back(std::make_pair(unit.name, unit.description));
                else
                {
                    UnitsCategory c(ToUtfString(it->second));
                    c.units.push_back(std::make_pair(unit.name, unit.description));
                    u_it->categories.push_back(c);
                }
            }
            continue;
        }

        //add to rest
        auto u_it = std::find_if(units.begin(), units.end(), 
            [](auto& u)
            {
                return u.system == U"Others";
            });
        if (u_it != units.end())
        {
            std::vector<UnitsCategory>& categories = u_it->categories;
            auto c_it = std::find_if(categories.begin(), categories.end(), 
                [](auto& cat)
                {
                    return cat.category == U"rest";
                });
            if (c_it != categories.end())
                c_it->units.push_back(std::make_pair(unit.name, unit.description));
            else
            {
                UnitsCategory c(U"rest");
                c.units.push_back(std::make_pair(unit.name, unit.description));
                u_it->categories.push_back(c);
            }
        }
    }

    rapidjson::Value units_arr(rapidjson::kArrayType);
    for (auto& system : units)
    {
        rapidjson::Value system_arr(rapidjson::kArrayType);
        for (auto& category : system.categories)
        {
            rapidjson::Value physical_arr(rapidjson::kArrayType);
            for (auto& unit : category.units)
            {
                rapidjson::Value u;
                u.SetObject();
                u.AddMember("name", rapidjson::Value((boost::locale::conv::utf_to_utf<char>(unit.first)).c_str(), alloc), alloc);
                if (!unit.second.empty())
                    u.AddMember("description", rapidjson::Value((boost::locale::conv::utf_to_utf<char>(unit.second)).c_str(), alloc), alloc);
                physical_arr.PushBack(u, alloc);
            }

            rapidjson::Value p;
            p.SetObject();
            rapidjson::Value m;
            m.SetString((boost::locale::conv::utf_to_utf<char>(category.category)).c_str(), alloc);
            p.AddMember(m, physical_arr, alloc);
            system_arr.PushBack(p, alloc);
        }

        rapidjson::Value p;
        p.SetObject();
        rapidjson::Value m;
        auto s = system.system;
        if (s == U"rus")
            s = U"Russian";
        else if (s == U"us")
            s = U"American";
        m.SetString((boost::locale::conv::utf_to_utf<char>(s)).c_str(), alloc);
        p.AddMember(m, system_arr, alloc);
        units_arr.PushBack(p, alloc);
    }

    reply.AddMember("Units", units_arr, alloc);
}

bool CalculatorSolver::SetLocale(const rapidjson::Document& request, rapidjson::Document& reply)
{
    idle_time = time(nullptr);

#ifdef YUTOVO_SOLVER_WORKER
    if (!parser_context)
    {
        if (request.HasMember("language") && request["language"].IsInt())
            locale.language = (Language)request["language"].GetInt();
        if (process)
            process->SendAction(solver_guid, "set_locale", request, reply, (int)locale.language);
        else
            ReplyError(ErrorCode::SOLVER_ERROR, reply);
        return !reply.HasMember("error");
    }
#endif

    if (!request.HasMember("language") || !request["language"].IsInt())
    {
        logger->Error("language error");
        ReplyError(ErrorCode::NO_FIELD_ERROR, reply);
        return false;
    }

    Language new_language = (Language)request["language"].GetInt();
    if (new_language == locale.language)
        return true; //the parsers are created with the current language, skip the heavy rebuild
    locale.language = new_language;

    try
    {
        std::lock_guard<std::mutex> lock(parsers_lock);
        real_parser.SetLocale(locale.language);
        integer_parser.SetLocale(locale.language);
        rational_parser.SetLocale(locale.language);
        complex_parser.SetLocale(locale.language);
        array_real_parser.SetLocale(locale.language);
        symbolic_real_parser.SetLocale(locale.language);
        symbolic_rational_parser.SetLocale(locale.language);
        symbolic_complex_parser.SetLocale(locale.language);
    }
    catch (yutovo_calculator::ParserException ex)
    {
        ReplyError(ex, reply);
        return false;
    }

    return true;
}

void CalculatorSolver::SolveReal(const rapidjson::Document& request, rapidjson::Document& reply, std::vector<std::u32string>* dependencies)
{
    std::string expression = request["expression"].GetString();
    
    int precision = 3;
    if (request.HasMember("real_precision") && request["real_precision"].IsInt())
        precision = request["real_precision"].GetInt();
    if (precision <= 0)
        precision = 3;

    int exponent_size = 3;
    if (request.HasMember("real_exponent_size") && request["real_exponent_size"].IsInt())
        exponent_size = request["real_exponent_size"].GetInt();

    AngleMeasure default_angle_measure = AngleMeasure::None;
    if (request.HasMember("real_default_angle_measure") && request["real_default_angle_measure"].IsInt())
        default_angle_measure = (AngleMeasure)request["real_default_angle_measure"].GetInt();

    AngleMeasure result_angle_measure = AngleMeasure::None;
    if (request.HasMember("real_result_angle_measure") && request["real_result_angle_measure"].IsInt())
        result_angle_measure = (AngleMeasure)request["real_result_angle_measure"].GetInt();

    bool include_document = false;
    if (request.HasMember("include_document") && request["include_document"].IsBool())
        include_document = request["include_document"].GetBool();

    auto& alloc = reply.GetAllocator();

    //solving
    parser_context->Init(max_time);
    parser_context->no_result = false;
    parser_context->include_document = include_document;
    Real si_res = real_parser.Parse(solving_id, expression, dependencies, default_angle_measure, result_angle_measure, precision, parser_context.get());
    if (parser_context->no_result)
    {
        AddDependencies(reply, dependencies);
        reply.AddMember("result_type", (int)ResultType::NONE, alloc);
        return;
    }

    Real res;
    Unit unit;
    if (GetUnit(request, unit))
        res = real_parser.CastToUnit(solving_id, si_res, unit);
    else
        res = real_parser.GetSuitableUnit(solving_id, si_res);

    bool mantissa_sign;
    std::string mantissa;
    bool exponent_sign;
    std::string exponent;
    res.ToString(exponent_size, precision, mantissa_sign, mantissa, exponent_sign, exponent);
    if (mantissa_sign)
        mantissa.insert(mantissa.begin(), '-');
    
    rapidjson::Value m(rapidjson::kStringType);
    reply.AddMember("result_type", (int)ResultType::REAL, alloc);
    m.SetString(mantissa.c_str(), mantissa.size(), alloc);
    reply.AddMember("mantissa", m, alloc);
    if (exponent_sign)
        exponent.insert(exponent.begin(), '-');
    if (exponent != "")
    {
        rapidjson::Value e(rapidjson::kStringType);
        e.SetString(exponent.c_str(), exponent.size(), alloc);
        reply.AddMember("exponent", e, alloc);
    }

    AddUnit(reply, res.unit, reply.GetAllocator());

    if (!si_res.unit.IsEmpty())
    {
        std::vector<Unit> cast_units;
        real_parser.GetCastUnits(solving_id, si_res, cast_units);
        AddCastUnits(reply, cast_units);
    }

    if (res.angle_measure != AngleMeasure::None)
        reply.AddMember("angle_measure", (int)res.angle_measure, alloc);
    AddDependencies(reply, dependencies);
}

void CalculatorSolver::SolveInteger(const rapidjson::Document& request, rapidjson::Document& reply, std::vector<std::u32string>* dependencies)
{
    std::string expression = request["expression"].GetString();
    Notation default_notation = Notation::Decimal;

    if (request.HasMember("integer_default_notation") && request["integer_default_notation"].IsInt())
        default_notation = (Notation)request["integer_default_notation"].GetInt();

    auto& alloc = reply.GetAllocator();

    parser_context->Init(max_time);
    parser_context->no_result = false;
    yutovo_calculator::Integer res = integer_parser.Parse(solving_id, expression, dependencies, default_notation, parser_context.get());
    if (parser_context->no_result)
    {
        AddDependencies(reply, dependencies);
        reply.AddMember("result_type", (int)ResultType::NONE, alloc);
        return;
    }

    reply.AddMember("result_type", (int)ResultType::INTEGER, alloc);
    std::string s;
    if (request.HasMember("integer_result_notation") && request["integer_result_notation"].IsInt())
    {
        Notation notation = (Notation)request["integer_result_notation"].GetInt();
        if ((int)notation < 0 || notation > Notation::Hexadecimal)
            notation = Notation::Decimal;
        switch (notation)
        {
        case Notation::Binary:
            s = res.ToStdString(2);
            break;
        case Notation::Octal:
            s = res.ToStdString(8);
            break;
        case Notation::Decimal:
            s = res.ToStdString(10);
            break;
        case Notation::Hexadecimal:
            s = res.ToStdString(16);
            break;
        case Notation::None:
            return;
        }
        reply.AddMember("notation", (int)notation, alloc);
    }
    else
    {
        s = res.ToStdString();
        reply.AddMember("notation", (int)Notation::Decimal, alloc);
    }
    
    rapidjson::Value val(rapidjson::kStringType);
    val.SetString(s.c_str(), s.size(), alloc);
    reply.AddMember("value", val, alloc);

    AddDependencies(reply, dependencies);
}

void CalculatorSolver::SolveRational(const rapidjson::Document& request, rapidjson::Document& reply, std::vector<std::u32string>* dependencies)
{
    std::string expression = request["expression"].GetString();

    auto& alloc = reply.GetAllocator();

    parser_context->Init(max_time);
    parser_context->no_result = false;
    Rational si_res = rational_parser.Parse(solving_id, expression, dependencies, parser_context.get());
    if (parser_context->no_result)
    {
        AddDependencies(reply, dependencies);
        reply.AddMember("result_type", (int)ResultType::NONE, alloc);
        return;
    }

    Rational res;
    Unit unit;
    if (GetUnit(request, unit))
        res = rational_parser.CastToUnit(solving_id, si_res, unit);
    else
        res = rational_parser.GetSuitableUnit(solving_id, si_res);

    reply.AddMember("result_type", (int)ResultType::RATIONAL, alloc);
    FractionForm form = FractionForm::Improper;
    if (request.HasMember("fraction_form") && request["fraction_form"].IsInt())
        form = (FractionForm)request["fraction_form"].GetInt();

    if (form == FractionForm::Proper)
    {
        yutovo_calculator::Integer i, n, d;
        res.ToProper(i, n, d);

        if (i != 0)
        {
            std::string integer = i.ToStdString();
            rapidjson::Value _i(rapidjson::kStringType);
            _i.SetString(integer.c_str(), integer.size(), alloc);
            reply.AddMember("integer", _i, alloc);
        }

        std::string numerator = n.ToStdString();
        std::string denomerator = d.ToStdString();
        rapidjson::Value _n(rapidjson::kStringType);
        _n.SetString(numerator.c_str(), numerator.size(), alloc);
        reply.AddMember("numerator", _n, alloc);
        rapidjson::Value _d(rapidjson::kStringType);
        _d.SetString(denomerator.c_str(), denomerator.size(), alloc);
        reply.AddMember("denomerator", _d, alloc);
    }
    else
    {
        std::string numerator = res.GetNumerator().ToStdString();
        std::string denomerator = res.GetDenomerator().ToStdString();
        rapidjson::Value n(rapidjson::kStringType);
        n.SetString(numerator.c_str(), numerator.size(), alloc);
        reply.AddMember("numerator", n, alloc);
        rapidjson::Value d(rapidjson::kStringType);
        d.SetString(denomerator.c_str(), denomerator.size(), alloc);
        reply.AddMember("denomerator", d, alloc);
    }

    AddUnit(reply, res.unit, reply.GetAllocator());

    if (!si_res.unit.IsEmpty())
    {
        std::vector<Unit> cast_units;
        rational_parser.GetCastUnits(solving_id, si_res, cast_units);
        AddCastUnits(reply, cast_units);
    }

    AddDependencies(reply, dependencies);
}

void CalculatorSolver::SolveComplex(const rapidjson::Document& request, rapidjson::Document& reply, std::vector<std::u32string>* dependencies)
{
    std::string expression = request["expression"].GetString();
    
    int precision = 3;
    if (request.HasMember("complex_precision") && request["complex_precision"].IsInt())
        precision = request["complex_precision"].GetInt();
    if (precision <= 0)
        precision = 3;

    int exponent_size = 3;
    if (request.HasMember("complex_exponent_size") && request["complex_exponent_size"].IsInt())
        exponent_size = request["complex_exponent_size"].GetInt();

    AngleMeasure default_angle_measure = AngleMeasure::None;
    if (request.HasMember("complex_default_angle_measure") && request["complex_default_angle_measure"].IsInt())
        default_angle_measure = (AngleMeasure)request["complex_default_angle_measure"].GetInt();

    AngleMeasure result_angle_measure = AngleMeasure::None;
    if (request.HasMember("complex_result_angle_measure") && request["complex_result_angle_measure"].IsInt())
        result_angle_measure = (AngleMeasure)request["complex_result_angle_measure"].GetInt();
    
    ComplexForm form = ComplexForm::Arithmetic;
    if (request.HasMember("complex_form") && request["complex_form"].IsInt())
        form = (ComplexForm)request["complex_form"].GetInt();
    
    int max_count = 10;
    if (request.HasMember("complex_max_count") && request["complex_max_count"].IsInt())
        max_count = request["complex_max_count"].GetInt();

    auto& alloc = reply.GetAllocator();

    //solving
    std::vector<Complex> results;
    parser_context->Init(max_time);
    parser_context->no_result = false;
    complex_parser.Parse(solving_id, expression, dependencies, default_angle_measure, result_angle_measure, precision, max_count, results, 
        parser_context.get());
    if (parser_context->no_result)
    {
        AddDependencies(reply, dependencies);
        reply.AddMember("result_type", (int)ResultType::NONE, alloc);
        return;
    }

    reply.AddMember("result_type", (int)ResultType::COMPLEX, alloc);

    rapidjson::Value results_arr(rapidjson::kArrayType);
    for (Complex& r : results)
    {
        rapidjson::Value res;
        res.SetObject();
        if (form == ComplexForm::Trigonometric || form == ComplexForm::Exponential)
        {
            rapidjson::Value m(rapidjson::kObjectType);
            AddReal(reply, m, module(r), exponent_size, precision);
            res.AddMember("module", m, alloc);

            rapidjson::Value a(rapidjson::kObjectType);
            AddReal(reply, a, argument(r), exponent_size, precision);
            res.AddMember("argument", a, alloc);
        }
        else
        {
            if (r.GetRe() != 0 || (r.GetRe() == 0 && r.GetIm() == 0))
            {
                rapidjson::Value re(rapidjson::kObjectType);
                AddReal(reply, re, r.GetRe(), exponent_size, precision);
                res.AddMember("re", re, alloc);
            }

            if (r.GetIm() != 0)
            {
                rapidjson::Value im(rapidjson::kObjectType);
                AddReal(reply, im, r.GetIm(), exponent_size, precision);
                res.AddMember("im", im, alloc);
            }
        }

        if (r.GetAngleMeasure() != AngleMeasure::None)
            res.AddMember("angle_measure", (int)r.GetAngleMeasure(), alloc);

        results_arr.PushBack(res, alloc);
    }

    reply.AddMember("results", results_arr, alloc);

    AddDependencies(reply, dependencies);
}

void CalculatorSolver::SolveArrayReal(const rapidjson::Document& request, rapidjson::Document& reply, std::vector<std::u32string>* dependencies)
{
    std::string expression = request["expression"].GetString();
    
    int precision = 3;
    if (request.HasMember("real_precision") && request["real_precision"].IsInt())
        precision = request["real_precision"].GetInt();
    if (precision <= 0)
        precision = 3;

    int exponent_size = 3;
    if (request.HasMember("real_exponent_size") && request["real_exponent_size"].IsInt())
        exponent_size = request["real_exponent_size"].GetInt();

    AngleMeasure default_angle_measure = AngleMeasure::None;
    if (request.HasMember("real_default_angle_measure") && request["real_default_angle_measure"].IsInt())
        default_angle_measure = (AngleMeasure)request["real_default_angle_measure"].GetInt();

    AngleMeasure result_angle_measure = AngleMeasure::None;
    if (request.HasMember("real_result_angle_measure") && request["real_result_angle_measure"].IsInt())
        result_angle_measure = (AngleMeasure)request["real_result_angle_measure"].GetInt();

    bool include_document = false;
    if (request.HasMember("include_document") && request["include_document"].IsBool())
        include_document = request["include_document"].GetBool();

    auto& alloc = reply.GetAllocator();

    //solving
    parser_context->Init(max_time);
    parser_context->no_result = false;
    parser_context->include_document = include_document;
    Array<Real> si_res = array_real_parser.Parse(solving_id, expression, dependencies, default_angle_measure, result_angle_measure, precision, 
        parser_context.get());
    if (parser_context->no_result)
    {
        AddDependencies(reply, dependencies);
        reply.AddMember("result_type", (int)ResultType::NONE, alloc);
        return;
    }

    bool mantissa_sign;
    std::string mantissa;
    bool exponent_sign;
    std::string exponent;

    reply.AddMember("result_type", (int)ResultType::ARRAY_REAL, alloc);

    rapidjson::Value r_arr(rapidjson::kArrayType);

    for (int i = 0; i < si_res.Size(); ++i)
    {
        Real r = si_res.Get(i);

        real_parser.ClearCastUnits();
        Unit unit;
        if (GetUnit(request, unit))
            r = real_parser.CastToUnit(solving_id, r, unit);
        else
            r = real_parser.GetSuitableUnit(solving_id, r);

        r.ToString(exponent_size, precision, mantissa_sign, mantissa, exponent_sign, exponent);
        if (mantissa_sign)
            mantissa.insert(mantissa.begin(), '-');
        
        rapidjson::Value obj(rapidjson::kObjectType);

        rapidjson::Value m(rapidjson::kStringType);
        m.SetString(mantissa.c_str(), mantissa.size(), alloc);
        obj.AddMember("mantissa", m, alloc);
        if (exponent_sign)
            exponent.insert(exponent.begin(), '-');
        if (exponent != "")
        {
            rapidjson::Value e(rapidjson::kStringType);
            e.SetString(exponent.c_str(), exponent.size(), alloc);
            obj.AddMember("exponent", e, alloc);
        }
        if (r.GetAngleMeasure() != AngleMeasure::None)
            obj.AddMember("angle_measure", (int)r.GetAngleMeasure(), alloc);
        
        AddUnit(obj, r.unit, reply.GetAllocator());
        
        r_arr.PushBack(obj, alloc);
    }

    rapidjson::Value r_obj(rapidjson::kObjectType);
    reply.AddMember("results", r_arr, alloc);

    AddDependencies(reply, dependencies);
}

void CalculatorSolver::SolveSymbolicReal(const rapidjson::Document& request, rapidjson::Document& reply, std::vector<std::u32string>* dependencies)
{
    std::string expression = request["expression"].GetString();

    int precision = 3;
    if (request.HasMember("real_precision") && request["real_precision"].IsInt())
        precision = request["real_precision"].GetInt();
    if (precision <= 0)
        precision = 3;

    int exponent = 3;
    if (request.HasMember("real_exponent_size") && request["real_exponent_size"].IsInt())
        exponent = request["real_exponent_size"].GetInt();

    auto& alloc = reply.GetAllocator();

    parser_context->Init(max_time);
    parser_context->no_result = false;
    Symbolic<Real> res = symbolic_real_parser.Parse(solving_id, expression, dependencies, precision, parser_context.get());
    if (parser_context->no_result)
    {
        AddDependencies(reply, dependencies);
        reply.AddMember("result_type", (int)ResultType::NONE, alloc);
        return;
    }

    reply.AddMember("result_type", (int)ResultType::SYMBOLIC_REAL, alloc);

    std::string json_value = res.ToJson(exponent, locale.language);
    if (!json_value.empty())
    {
        rapidjson::Value j(rapidjson::kStringType);
        j.SetString(json_value.c_str(), json_value.size(), alloc);
        reply.AddMember("json", j, alloc);
    }

    AddDependencies(reply, dependencies);
}

void CalculatorSolver::SolveSymbolicRational(const rapidjson::Document& request, rapidjson::Document& reply, std::vector<std::u32string>* dependencies)
{
    std::string expression = request["expression"].GetString();

    auto& alloc = reply.GetAllocator();

    parser_context->Init(max_time);
    parser_context->no_result = false;
    Symbolic<Rational> res = symbolic_rational_parser.Parse(solving_id, expression, dependencies, 0, parser_context.get());
    if (parser_context->no_result)
    {
        AddDependencies(reply, dependencies);
        reply.AddMember("result_type", (int)ResultType::NONE, alloc);
        return;
    }

    reply.AddMember("result_type", (int)ResultType::SYMBOLIC_RATIONAL, alloc);

    std::string json_value = res.ToJson(0, locale.language);
    if (!json_value.empty())
    {
        rapidjson::Value j(rapidjson::kStringType);
        j.SetString(json_value.c_str(), json_value.size(), alloc);
        reply.AddMember("json", j, alloc);
    }

    AddDependencies(reply, dependencies);
}

void CalculatorSolver::SolveSymbolicComplex(const rapidjson::Document& request, rapidjson::Document& reply, std::vector<std::u32string>* dependencies)
{
    std::string expression = request["expression"].GetString();

    int precision = 10;
    if (request.HasMember("complex_precision") && request["complex_precision"].IsInt())
        precision = request["complex_precision"].GetInt();
    if (precision <= 0)
        precision = 10;

    int exponent = 10;
    if (request.HasMember("complex_exponent_size") && request["complex_exponent_size"].IsInt())
        exponent = request["complex_exponent_size"].GetInt();

    auto& alloc = reply.GetAllocator();

    parser_context->Init(max_time);
    parser_context->no_result = false;
    Symbolic<Complex> res = symbolic_complex_parser.Parse(solving_id, expression, dependencies, precision, parser_context.get());
    if (parser_context->no_result)
    {
        AddDependencies(reply, dependencies);
        reply.AddMember("result_type", (int)ResultType::NONE, alloc);
        return;
    }

    reply.AddMember("result_type", (int)ResultType::SYMBOLIC_COMPLEX, alloc);

    std::string json_value = res.ToJson(exponent, locale.language);
    if (!json_value.empty())
    {
        rapidjson::Value j(rapidjson::kStringType);
        j.SetString(json_value.c_str(), json_value.size(), alloc);
        reply.AddMember("json", j, alloc);
    }

    AddDependencies(reply, dependencies);
}

void CalculatorSolver::AddReal(rapidjson::Document& reply, rapidjson::Value& obj, const Real& value, const int exponent_size, const int precision)
{
    bool mantissa_sign;
    std::string mantissa;
    bool exponent_sign;
    std::string exponent;
    value.ToString(exponent_size, precision, mantissa_sign, mantissa, exponent_sign, exponent);
    if (mantissa_sign)
        mantissa.insert(mantissa.begin(), '-');
    
    auto& alloc = reply.GetAllocator();
    rapidjson::Value m(rapidjson::kStringType);
    m.SetString(mantissa.c_str(), mantissa.size(), alloc);

    obj.AddMember("mantissa", m, alloc);
    if (exponent_sign)
        exponent.insert(exponent.begin(), '-');
    if (exponent != "")
    {
        rapidjson::Value e(rapidjson::kStringType);
        e.SetString(exponent.c_str(), exponent.size(), alloc);
        obj.AddMember("exponent", e, alloc);
    }
}

//PythonSolver

PythonSolver::PythonSolver(const std::string& _document_guid, const std::string& _solver_guid, const yutovo_calculator::Language _language, 
    uint64_t _max_time, const std::string& _logs_path, bool _log_console, bool _log_file) :
    Solver(_document_guid, _solver_guid, _language),
    logger(Logger::GetInstance(_logs_path + "/yutovo-solver", "python-solver", _log_console, _log_file))
{
}

PythonSolver::~PythonSolver()
{
    logger->Info("Python Solver finished: {}", solver_guid);
}

void PythonSolver::Solve(const rapidjson::Document& request, rapidjson::Document& reply)
{
    throw ServiceException{ErrorCode::NOT_IMPLEMENTED};
}

void PythonSolver::BreakSolving(const rapidjson::Document& request, rapidjson::Document& reply)
{
    throw ServiceException{ErrorCode::NOT_IMPLEMENTED};
}

void PythonSolver::RemoveIdentifier(const rapidjson::Document& request, rapidjson::Document& reply)
{
    throw ServiceException{ErrorCode::NOT_IMPLEMENTED};
}

void PythonSolver::RemoveUserIdentifiers(const rapidjson::Document& request, rapidjson::Document& reply)
{
    throw ServiceException{ErrorCode::NOT_IMPLEMENTED};
}

void PythonSolver::ListIdentifiers(const rapidjson::Document& request, rapidjson::Document& reply)
{
    throw ServiceException{ErrorCode::NOT_IMPLEMENTED};
}

bool PythonSolver::SetLocale(const rapidjson::Document& request, rapidjson::Document& reply)
{
    throw ServiceException{ErrorCode::NOT_IMPLEMENTED};
}

//Solvers

std::map<std::string, ParserContextPtr> Solvers::parser_contexts;

Solvers::Solvers(ServiceConfig* _service_config, const std::string& _logs_path, bool _log_console, bool _log_file) :
    service_config(_service_config),
    logs_path(_logs_path), 
    log_console(_log_console), 
    log_file(_log_file)
{
}

SolverPtr Solvers::GetSolver(const std::string& document_guid, const std::string& solver_guid, const int code_id, SolverType solver_type)
{
    SolverLocale locale;

    std::lock_guard<std::mutex> lock(solvers_mutex);
    auto it = solvers.find(solver_guid);
    if (it != solvers.end())
    {
        auto it_c = it->second.find(code_id);
        if (it_c != it->second.end())
            return it_c->second;
    }
    
    auto it_l = solvers_locales.find(solver_guid);
    if (it_l != solvers_locales.end())
        locale = it_l->second;

    ParserContextPtr parser_context;
    //on desktop the parsers live in a separate worker process, so a proxy solver gets no parser context
#ifndef YUTOVO_SOLVER_WORKER
    auto it_p = parser_contexts.find(document_guid);
    if (it_p != parser_contexts.end())
        parser_context = it_p->second;
    else
    {
        parser_context.reset(new yutovo_calculator::ParserContext());
        parser_contexts[document_guid] = parser_context;
    }
#endif
    
    switch (solver_type)
    {
    case SolverType::CALCULATOR:
        {
            std::string solver_id = solver_guid + "-" + std::to_string(code_id);
            SolverPtr solver(new CalculatorSolver(document_guid, solver_id, parser_context, locale.language, service_config->max_time, 
                logs_path, log_console, log_file));
            if (it == solvers.end())
            {
                std::map<int, SolverPtr> m;
                m[code_id] = solver;
                solvers[solver_guid] = m;
            }
            else
            {
                it->second[code_id] = solver;
            }
            return solver;
        }
    case SolverType::PYTHON:
    case SolverType::NONE:
        break;
    }

    return nullptr;
}

bool Solvers::RemoveSolver(const std::string& solver_guid, const int code_id)
{
    std::lock_guard<std::mutex> lock(solvers_mutex);
    auto it = solvers.find(solver_guid);
    if (it != solvers.end())
    {
        auto it_c = it->second.find(code_id);
        if (it_c != it->second.end())
        {
            it->second.erase(it_c);
            return true;
        }
    }
    return false;
}

void Solvers::SetLocale(const std::string& solver_guid, const yutovo_calculator::Language language, 
    const rapidjson::Document& request, rapidjson::Document& reply)
{
    //an out of range language would throw from the parser constructors when a solver is created
    if (language < yutovo_calculator::Language::English || language > yutovo_calculator::Language::BrazilianPortuguese)
        return;

    std::lock_guard<std::mutex> lock(solvers_mutex);
    auto it = solvers.find(solver_guid);
    if (it != solvers.end())
    {
        for (auto& [code_id, solver] : it->second)
            solver->SetLocale(request, reply);
    }

    solvers_locales[solver_guid] = SolverLocale{language};
}

void Solvers::RemoveUserIdentifiers(const std::string& solver_guid, const rapidjson::Document& request, rapidjson::Document& reply)
{
    std::lock_guard<std::mutex> lock(solvers_mutex);
    auto it = solvers.find(solver_guid);
    if (it != solvers.end())
    {
        for (auto& [code_id, solver] : it->second)
            solver->RemoveUserIdentifiers(request, reply);
    }
}

void Solvers::ClearExport(const std::string& document_guid, const rapidjson::Document& request, rapidjson::Document& reply)
{
    std::unique_lock<std::mutex> lock(solvers_mutex);
#ifdef YUTOVO_SOLVER_WORKER
    //the parser context lives in the calculator worker process of the document
    auto process = SolverProcess::FindProcess(document_guid);
    if (process)
    {
        lock.unlock();
        process->SendAction("", "clear_export", request, reply);
    }
#else
    auto it = parser_contexts.find(document_guid);
    if (it != parser_contexts.end())
        it->second->exports->Clear();
#endif
}

void Solvers::SetMaxTime(const uint64_t max_time)
{
    //new solvers are created with service_config->max_time, so it must be updated as well
    service_config->max_time = max_time;
    std::lock_guard<std::mutex> lock(solvers_mutex);
    for (auto& s : solvers)
    {
        for (auto& solver : s.second)
            solver.second->SetMaxTime(max_time);
    }
}

void Solvers::RemoveTimeouted()
{
    std::lock_guard<std::mutex> lock(solvers_mutex);
    //remove unusing solvers
    for (auto it = solvers.begin(); it != solvers.end(); ++it)
    {
        auto& m = it->second;
        for (auto it_s = m.begin(); it_s != m.end();)
        {
            SolverPtr& s = it_s->second;
            if (time(nullptr) - s->idle_time > service_config->solver_idle_timeout)
            {
                m.erase(it_s++); //remove solver
                continue;
            }
            ++it_s;
        }
    }

    //remove unusing parser contexts
    for (auto it_p = parser_contexts.begin(); it_p != parser_contexts.end();)
    {
        auto& document_guid = it_p->first;
        auto it = std::find_if(solvers.begin(), solvers.end(), 
            [document_guid](auto& s)
            {
                auto it_s = std::find_if(s.second.begin(), s.second.end(), 
                    [document_guid](auto& solver)
                    {
                        return solver.second->document_guid == document_guid;
                    });
                return it_s != s.second.end();
            });
        if (it == solvers.end())
            it_p = parser_contexts.erase(it_p);
        else
            ++it_p;
    }
}

//SolveTimeoutWatchdog

SolveTimeoutWatchdog::SolveTimeoutWatchdog(uint64_t _max_time)
{
    if (_max_time == 0)
        return; //no time limit is armed

    timeout_thread = std::thread(
        [this, deadline = _max_time + interrupt_reserve_ms]() -> void
        {
            auto start = std::chrono::steady_clock::now();
            while (!done.load())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
                if (elapsed < static_cast<int64_t>(deadline))
                    continue;
                if (done.load())
                    return;
                //interrupt a stuck giac evaluation with the giac flags only, the parser timer keeps its own handling
                fired.store(true);
#ifndef YUTOVO_SOLVER_TEST_NO_INTERRUPT
                //the test build of the worker cannot stop itself, so the parent kill path can be tested
                giac::ctrl_c = true;
                giac::interrupted = true;
#endif
                return;
            }
        });
}

SolveTimeoutWatchdog::~SolveTimeoutWatchdog()
{
    done.store(true);
    if (timeout_thread.joinable())
        timeout_thread.join();
}

}
