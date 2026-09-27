/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __SOLVER_TASK_H__
#define __SOLVER_TASK_H__

#include "caret_state.h"
#include <memory>
#include <rapidjson/document.h>
#include "web_socket.h"
#include "result_codes.h"
#include <yutovo-calculator/unit.h>
#include <yutovo-calculator/math_helper.h>

namespace yutovo
{

class Logger;

struct SolverTask
{
    SolverTask(const LogicalId& _id, Document* _document, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
        ExpressionType _expression_type, bool _include_document, const std::u32string& _expression, const uint _delay, Logger* _logger);
    SolverTask(const LogicalId& _id, Document* _document, const std::string& _solver_guid, uint _code_id, 
        ExpressionType _expression_type, bool _include_document, const std::u32string& _expression, const uint _delay, Logger* _logger);
    SolverTask(Document* _document, const std::string& _solver_guid, Logger* _logger);
    SolverTask(Document* _document, Logger* _logger);
    SolverTask(const LogicalId& _id, Document* _document, const std::string& _solver_guid, uint _code_id, Logger* _logger);
    virtual ~SolverTask() = default;

    virtual bool Execute(WebSocketPtr socket, Result& result) = 0;

    bool SendRequest(const rapidjson::Document& json, Result& result, WebSocketPtr& socket, bool log_action = false);

    void AddUnit(rapidjson::Document& json, const yutovo_calculator::Unit& unit);

    void GetResultType(const rapidjson::Document& json, Result& result);
    void GetDependencies(const rapidjson::Document& json, Result& result);

    void FillId(rapidjson::Document& doc);
    void FillUnit(const rapidjson::Value& doc, Result& result, Value& value);
    void FillCastUnits(const rapidjson::Value& doc, Value& value);
    void FillError(rapidjson::Document& doc, Result& result);

    bool FillRealResult(rapidjson::Document& doc, Result& result);
    bool FillIntegerResult(rapidjson::Document& doc, Result& result);
    bool FillRationalResult(rapidjson::Document& doc, Result& result);
    bool FillComplexResult(rapidjson::Document& doc, Result& result);
    bool FillArrayRealResult(rapidjson::Document& doc, Result& result);
    bool FillSymbolicResult(rapidjson::Document& doc, Result& result);

    Document* document;
    LogicalId id;
    std::string id_str;
    std::string solver_guid;
    std::string task_guid;
    uint code_id = 0;
    ExpressionType expression_type = ExpressionType::NONE;
    std::u32string expression;
    bool include_document = false;
    int delay = 0; //in milliseconds
    uint64_t cur_time = 0;
    Logger* logger;
};

struct AutoSolverTask : SolverTask
{
    AutoSolverTask(const LogicalId& _id, Document* _document, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
        ExpressionType _expression_type, Config::AutoResultConfig _config, bool _include_document, const std::u32string& _expression, 
        const uint _delay, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::AutoResultConfig config;
};

struct RealSolverTask : SolverTask
{
    RealSolverTask(const LogicalId& _id, Document* _document, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
        ExpressionType _expression_type, Config::RealResultConfig _config, bool _include_document, const std::u32string& _expression, 
        const uint _delay, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::RealResultConfig config;
};

struct IntegerSolverTask : SolverTask
{
    IntegerSolverTask(const LogicalId& _id, Document* _document, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
        ExpressionType _expression_type, Config::IntegerResultConfig _config, bool _include_document, const std::u32string& _expression, 
        const uint _delay, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::IntegerResultConfig config;
};

struct RationalSolverTask : SolverTask
{
    RationalSolverTask(const LogicalId& _id, Document* _document, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
        ExpressionType _expression_type, Config::RationalResultConfig _config, bool _include_document, const std::u32string& _expression, 
        const uint _delay, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::RationalResultConfig config;
};

struct ComplexSolverTask : SolverTask
{
    ComplexSolverTask(const LogicalId& _id, Document* _document, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
        ExpressionType _expression_type, Config::ComplexResultConfig _config, bool _include_document, const std::u32string& _expression, 
        const uint _delay, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::ComplexResultConfig config;
};

struct ArrayRealSolverTask : SolverTask
{
    ArrayRealSolverTask(const LogicalId& _id, Document* _document, const std::string& _solver_guid, const std::string& _task_guid,
        uint _code_id, ExpressionType _expression_type, Config::ArrayRealResultConfig _config, bool _include_document, const std::u32string& _expression,
        const uint _delay, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::ArrayRealResultConfig config;
};

struct SymbolicRealSolverTask : SolverTask
{
    SymbolicRealSolverTask(const LogicalId& _id, Document* _document, const std::string& _solver_guid, const std::string& _task_guid,
        uint _code_id, ExpressionType _expression_type, Config::RealResultConfig _config, bool _include_document, const std::u32string& _expression,
        const uint _delay, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::RealResultConfig config;
};

struct SymbolicRationalSolverTask : SolverTask
{
    SymbolicRationalSolverTask(const LogicalId& _id, Document* _document, const std::string& _solver_guid, const std::string& _task_guid,
        uint _code_id, ExpressionType _expression_type, Config::RationalResultConfig _config, bool _include_document, const std::u32string& _expression,
        const uint _delay, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::RationalResultConfig config;
};

struct SymbolicComplexSolverTask : SolverTask
{
    SymbolicComplexSolverTask(const LogicalId& _id, Document* _document, const std::string& _solver_guid, const std::string& _task_guid,
        uint _code_id, ExpressionType _expression_type, Config::ComplexResultConfig _config, bool _include_document, const std::u32string& _expression,
        const uint _delay, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::ComplexResultConfig config;
};

struct BreakSolverTask : SolverTask
{
    BreakSolverTask(const LogicalId& _id, Document* _document, const std::string& _solver_guid, uint _code_id, bool _wait, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    bool wait; //wait for starting the breaking task
};

struct SetIdentifierSolverTask : AutoSolverTask
{
    SetIdentifierSolverTask(const LogicalId& _id, Document* _document, const std::string& _solver_guid, const std::string& _task_guid, 
        uint _code_id, Config::AutoResultConfig _config, bool _include_document, const std::u32string& _identifier, const std::u32string& _expression, 
        const uint _delay, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    const std::u32string identifier;
};

struct RemoveIdentifierSolverTask : SolverTask
{
    RemoveIdentifierSolverTask(const LogicalId& _id, Document* _document, const std::string& _solver_guid, uint _code_id, 
        const std::u32string& _expression, const uint _delay, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);
};

struct RemoveUserIdentifiersSolverTask : SolverTask
{
    RemoveUserIdentifiersSolverTask(Document* _document, const std::string& _solver_guid, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);
};

struct ClearExportSolverTask : SolverTask
{
    ClearExportSolverTask(Document* _document, const std::string& _solver_guid, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);
};

struct SetLocaleSolverTask : SolverTask
{
    SetLocaleSolverTask(Document* _document, const std::string& _solver_guid, const yutovo_calculator::Language _language, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    yutovo_calculator::Language language;
};

struct ListIdentifiersSolverTask : SolverTask
{
    ListIdentifiersSolverTask(Document* _document, const std::string& _solver_guid, const uint _code_id, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    const uint code_id;
};

struct RemoveSolverTask : SolverTask
{
    RemoveSolverTask(Document* _document, const std::string& _solver_guid, uint _code_id, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);
};

typedef std::shared_ptr<SolverTask> SolverTaskPtr;

}

#endif
