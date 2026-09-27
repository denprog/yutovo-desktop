/*
 * Yutovo Solver
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __SERVICE_SOLVER_H__
#define __SERVICE_SOLVER_H__

#include <map>
#include <memory>
#include <mutex>
#include <yutovo-logger/logger.h>
#include <yutovo-calculator/parser.h>
#include <yutovo-calculator/symbolic.h>
#include "rapidjson/document.h"
#include "types.h"

namespace yutovo_solver
{

class ServiceConfig;

using namespace yutovo_calculator;
using namespace yutovo;

struct SolverLocale
{
    yutovo_calculator::Language language = Language::English;
};

class Solver
{
public:
    Solver(const std::string& _document_guid, const std::string& _solver_guid, const yutovo_calculator::Language _language);

    virtual ~Solver() = default;

    virtual void Solve(const rapidjson::Document& request, rapidjson::Document& reply) = 0;
    virtual void BreakSolving(const rapidjson::Document& request, rapidjson::Document& reply) = 0;
    virtual void RemoveIdentifier(const rapidjson::Document& request, rapidjson::Document& reply) = 0;
    virtual void RemoveUserIdentifiers(const rapidjson::Document& request, rapidjson::Document& reply) = 0;
    virtual void ListIdentifiers(const rapidjson::Document& request, rapidjson::Document& reply) = 0;
    virtual bool SetLocale(const rapidjson::Document& request, rapidjson::Document& reply) = 0;
    virtual void SetMaxTime(const uint64_t _max_time);

protected:
    void ReplyOk(rapidjson::Document& reply);
    void ReplyError(const ErrorCode error_code, rapidjson::Document& reply);
    void ReplyError(const yutovo_calculator::ParserException& ex, rapidjson::Document& reply);

    void AddUnit(rapidjson::Value& reply, const Unit& unit, rapidjson::Document::AllocatorType& alloc);
    void AddCastUnits(rapidjson::Document& reply, const std::vector<Unit>& cast_units);
    void AddDependencies(rapidjson::Document& reply, const std::vector<std::u32string>* dependencies);

    bool GetLogicalId(const rapidjson::Document& request, LogicalId& id);
    bool GetTimestamp(const rapidjson::Document& request, uint64_t& time_stamp);

    rapidjson::Value LogicalIdToValue(rapidjson::Document& reply, const LogicalId& id);

    bool GetUnit(const rapidjson::Document& request, Unit& unit);

public:
    std::atomic<time_t> idle_time = time(nullptr);

    std::string document_guid;
    std::string solver_guid;
    SolverLocale locale;

protected:
    uint64_t max_time = 0;
};

typedef std::shared_ptr<Solver> SolverPtr;
typedef std::shared_ptr<yutovo_calculator::ParserContext> ParserContextPtr;

class SolverProcess;

class CalculatorSolver : public Solver
{
public:
    CalculatorSolver(const std::string& _document_guid, const std::string& _solver_guid, ParserContextPtr _parser_context, 
        const yutovo_calculator::Language _language, uint64_t _max_time, const std::string& _logs_path, bool _log_console, bool _log_file);
    ~CalculatorSolver();

    virtual void Solve(const rapidjson::Document& request, rapidjson::Document& reply);
    virtual void BreakSolving(const rapidjson::Document& request, rapidjson::Document& reply);
    virtual void RemoveIdentifier(const rapidjson::Document& request, rapidjson::Document& reply);
    virtual void RemoveUserIdentifiers(const rapidjson::Document& request, rapidjson::Document& reply);
    virtual void ListIdentifiers(const rapidjson::Document& request, rapidjson::Document& reply);
    virtual bool SetLocale(const rapidjson::Document& request, rapidjson::Document& reply);
    virtual void SetMaxTime(const uint64_t _max_time);

private:
    void SolveReal(const rapidjson::Document& request, rapidjson::Document& reply, std::vector<std::u32string>* dependencies);
    void SolveInteger(const rapidjson::Document& request, rapidjson::Document& reply, std::vector<std::u32string>* dependencies);
    void SolveRational(const rapidjson::Document& request, rapidjson::Document& reply, std::vector<std::u32string>* dependencies);
    void SolveComplex(const rapidjson::Document& request, rapidjson::Document& reply, std::vector<std::u32string>* dependencies);
    void SolveArrayReal(const rapidjson::Document& request, rapidjson::Document& reply, std::vector<std::u32string>* dependencies);
    void SolveSymbolicReal(const rapidjson::Document& request, rapidjson::Document& reply, std::vector<std::u32string>* dependencies);
    void SolveSymbolicRational(const rapidjson::Document& request, rapidjson::Document& reply, std::vector<std::u32string>* dependencies);
    void SolveSymbolicComplex(const rapidjson::Document& request, rapidjson::Document& reply, std::vector<std::u32string>* dependencies);

    void AddReal(rapidjson::Document& reply, rapidjson::Value& obj, const Real& value, const int exponent_size, const int precision);

private:
    std::mutex parsers_lock;
    yutovo_calculator::Parser<yutovo_calculator::Real> real_parser;
    yutovo_calculator::Parser<yutovo_calculator::Integer> integer_parser;
    yutovo_calculator::Parser<yutovo_calculator::Rational> rational_parser;
    yutovo_calculator::Parser<yutovo_calculator::Complex> complex_parser;
    yutovo_calculator::Parser<yutovo_calculator::Array<Real>> array_real_parser;
    yutovo_calculator::Parser<yutovo_calculator::Symbolic<Real>> symbolic_real_parser;
    yutovo_calculator::Parser<yutovo_calculator::Symbolic<Rational>> symbolic_rational_parser;
    yutovo_calculator::Parser<yutovo_calculator::Symbolic<Complex>> symbolic_complex_parser;

    ParserContextPtr parser_context;

#ifdef YUTOVO_SOLVER_WORKER
    //in proxy mode parser_context is null and the parsers live in the calculator worker process
    std::shared_ptr<SolverProcess> process;
#endif

    std::mutex solving_id_lock;
    LogicalId solving_id;
    uint64_t solving_time_stamp = 0;

    std::mutex break_lock;
    std::map<LogicalId, int64_t> break_solvings;

    std::atomic<bool> just_started = true;

    Logger* logger = nullptr;
};

class PythonSolver : public Solver
{
public:
    PythonSolver(const std::string& _document_guid, const std::string& _solver_guid, const yutovo_calculator::Language _language, uint64_t _max_time, 
        const std::string& _logs_path, bool _log_console, bool _log_file);
    ~PythonSolver();

    virtual void Solve(const rapidjson::Document& request, rapidjson::Document& reply);
    virtual void BreakSolving(const rapidjson::Document& request, rapidjson::Document& reply);
    virtual void RemoveIdentifier(const rapidjson::Document& request, rapidjson::Document& reply);
    virtual void RemoveUserIdentifiers(const rapidjson::Document& request, rapidjson::Document& reply);
    virtual void ListIdentifiers(const rapidjson::Document& request, rapidjson::Document& reply);
    virtual bool SetLocale(const rapidjson::Document& request, rapidjson::Document& reply);

private:
    Logger* logger = nullptr;
};

class Solvers
{
public:
    Solvers(ServiceConfig* _service_config, const std::string& _logs_path, bool _log_console, bool _log_file);

    SolverPtr GetSolver(const std::string& document_guid, const std::string& solver_guid, const int code_id, SolverType solver_type);
    bool RemoveSolver(const std::string& solver_guid, const int code_id);

    void SetLocale(const std::string& solver_guid, const yutovo_calculator::Language language, 
        const rapidjson::Document& request, rapidjson::Document& reply);
    void RemoveUserIdentifiers(const std::string& solver_guid, const rapidjson::Document& request, rapidjson::Document& reply);
    void ClearExport(const std::string& document_guid, const rapidjson::Document& request, rapidjson::Document& reply);
    void SetMaxTime(const uint64_t max_time);
    void RemoveTimeouted();

private:
    std::mutex solvers_mutex;
    std::map<std::string, std::map<int, SolverPtr>> solvers; //by solver guid and by code_id
    std::map<std::string, SolverLocale> solvers_locales; //by solver guid
    static std::map<std::string, ParserContextPtr> parser_contexts; //by document guid

    ServiceConfig* service_config;

    const std::string logs_path;
    bool log_console;
    bool log_file;
};

class SolveTimeoutWatchdog
{
public:
    SolveTimeoutWatchdog(uint64_t _max_time);
    ~SolveTimeoutWatchdog();

    std::atomic<bool> fired{false};

private:
    std::atomic<bool> done{false};
    std::thread timeout_thread;

    //gives the in-parser CPU timer a chance to fire first, then interrupts a giac evaluation stuck past the deadline
    const uint64_t interrupt_reserve_ms = 250;
};

}

#endif
