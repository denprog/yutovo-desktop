/*
 * Yutovo Solver
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "types.h"

#ifdef YUTOVO_SOLVER_WORKER

#include "service_solver.h"
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <yutovo-logger/logger.h>
#include <yutovo-calculator/parser_exception.h>
#include <giac/global.h>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

namespace yutovo_solver
{

//CalculatorWorkerHost keeps the parser state of one document inside the worker process
class CalculatorWorkerHost
{
public:
    CalculatorWorkerHost(const std::string& _document_guid, const std::string& _logs_path, bool _log_file) :
        document_guid(_document_guid),
        parser_context(new yutovo_calculator::ParserContext()),
        logger(Logger::GetInstance(_logs_path + "/yutovo-solver", "calculator-worker", false, _log_file))
    {
        logger->Info("Calculator worker started for document: {}", document_guid);
    }

    //handles a regular queued action, throws ServiceException to propagate it back to the parent process
    void HandleRequest(const rapidjson::Document& request, rapidjson::Document& reply)
    {
        std::string action = request["action"].GetString();
        std::string solver_key = request["solver_key"].GetString();
        logger->Info("worker action: {} for {}", action, solver_key);

        yutovo_calculator::Language language = yutovo_calculator::Language::English;
        if (request.HasMember("language") && request["language"].IsInt())
            language = (yutovo_calculator::Language)request["language"].GetInt();
        uint64_t max_time_value = 0;
        if (request.HasMember("max_time") && request["max_time"].IsUint64())
            max_time_value = request["max_time"].GetUint64();

        rapidjson::Document inner;
        if (request.HasMember("request") && request["request"].IsObject())
            inner.CopyFrom(request["request"], inner.GetAllocator());
        else
            inner.SetObject();

        if (action == "solve" || action == "remove_identifier" || action == "list_identifiers")
        {
            SolverPtr solver = GetSolver(solver_key, language, max_time_value);
            if (action == "solve")
                solver->Solve(inner, reply);
            else if (action == "remove_identifier")
                solver->RemoveIdentifier(inner, reply);
            else
                solver->ListIdentifiers(inner, reply);
            return;
        }

        if (action == "set_locale")
        {
            SolverPtr solver = GetSolver(solver_key, language, max_time_value);
            solver->SetLocale(inner, reply);
            return;
        }

        if (action == "remove_user_identifiers")
        {
            SolverPtr solver = GetExistingSolver(solver_key);
            if (solver)
                solver->RemoveUserIdentifiers(inner, reply);
            else
                MakeOkReply(reply);
            return;
        }

        if (action == "clear_export")
        {
            parser_context->exports->Clear();
            MakeOkReply(reply);
            return;
        }

        MakeErrorReply(ErrorCode::UNKNOWN_COMMAND, reply);
    }

    //handles BREAK_SOLVING in the reader thread so a long running solve can be interrupted
    void HandleBreak(const rapidjson::Document& request)
    {
        std::string solver_key = request["solver_key"].GetString();
        logger->Info("worker break for: {}", solver_key);
        SolverPtr solver = GetExistingSolver(solver_key);
        if (!solver)
            return;

        rapidjson::Document inner;
        if (request.HasMember("request") && request["request"].IsObject())
            inner.CopyFrom(request["request"], inner.GetAllocator());
        else
            inner.SetObject();

        rapidjson::Document reply;
        solver->BreakSolving(inner, reply);
    }

    void RemoveSolver(const std::string& solver_key)
    {
        std::lock_guard<std::mutex> lock(solvers_mutex);
        solvers.erase(solver_key);
    }

private:
    SolverPtr GetSolver(const std::string& solver_key, yutovo_calculator::Language language, uint64_t max_time_value)
    {
        std::lock_guard<std::mutex> lock(solvers_mutex);
        SolverPtr solver;
        auto it = solvers.find(solver_key);
        if (it != solvers.end())
        {
            solver = it->second;
        }
        else
        {
            solver.reset(new CalculatorSolver(document_guid, solver_key, parser_context, language, max_time_value, "", false, false));
            solvers[solver_key] = solver;
            return solver;
        }

        if (solver->locale.language != language)
        {
            rapidjson::Document request, reply;
            request.SetObject();
            request.AddMember("language", (int)language, request.GetAllocator());
            solver->SetLocale(request, reply);
        }
        solver->SetMaxTime(max_time_value);
        return solver;
    }

    SolverPtr GetExistingSolver(const std::string& solver_key)
    {
        std::lock_guard<std::mutex> lock(solvers_mutex);
        auto it = solvers.find(solver_key);
        if (it != solvers.end())
            return it->second;
        return SolverPtr();
    }

    static void MakeOkReply(rapidjson::Document& reply)
    {
        reply.SetObject();
        auto& alloc = reply.GetAllocator();
        rapidjson::Value ok;
        ok.SetObject();
        ok.AddMember("error_code", (int)ErrorCode::OK, alloc);
        reply.AddMember("result", ok, alloc);
    }

    static void MakeErrorReply(ErrorCode error_code, rapidjson::Document& reply)
    {
        reply.SetObject();
        auto& alloc = reply.GetAllocator();
        rapidjson::Value error;
        error.SetObject();
        error.AddMember("error_code", (int)error_code, alloc);
        reply.AddMember("error", error, alloc);
    }

private:
    std::string document_guid;
    ParserContextPtr parser_context;
    Logger* logger;

    std::mutex solvers_mutex;
    std::map<std::string, SolverPtr> solvers; //by solver key
};

struct PendingRequest
{
    rapidjson::Document request;
};

static std::mutex output_mutex;

static void WriteReply(const rapidjson::Document& reply)
{
    rapidjson::Document envelope;
    envelope.SetObject();
    auto& alloc = envelope.GetAllocator();
    rapidjson::Value reply_copy;
    reply_copy.CopyFrom(reply, alloc);
    envelope.AddMember("reply", reply_copy, alloc);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    envelope.Accept(writer);

    std::lock_guard<std::mutex> lock(output_mutex);
    std::cout << buffer.GetString() << std::endl;
}

static void WriteThrow(ErrorCode error_code)
{
    rapidjson::Document envelope;
    envelope.SetObject();
    envelope.AddMember("throw", (int)error_code, envelope.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    envelope.Accept(writer);

    std::lock_guard<std::mutex> lock(output_mutex);
    std::cout << buffer.GetString() << std::endl;
}

//an evaluation interrupted deep inside giac is reported like a break, with the request id carried over
static void MakeInterruptedReply(const rapidjson::Document& request, rapidjson::Document& reply)
{
    reply.SetObject();
    auto& alloc = reply.GetAllocator();
    rapidjson::Value error;
    error.SetObject();

    rapidjson::Value id(rapidjson::kArrayType);
    if (request.HasMember("request") && request["request"].IsObject() && request["request"].HasMember("id") &&
        request["request"]["id"].IsArray())
    {
        const rapidjson::Value& arr = request["request"]["id"];
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
        {
            if (arr[i].IsInt())
                id.PushBack(arr[i].GetInt(), alloc);
        }
    }
    error.AddMember("id", id, alloc);
    error.AddMember("error_code", (int)ErrorCode::PARSER_ERROR, alloc);
    error.AddMember("parser_error_code", (int)yutovo_calculator::ParserExceptionCode::Break, alloc);
    error.AddMember("pos", -1, alloc);
    error.AddMember("size", -1, alloc);
    error.AddMember("line", -1, alloc);
    reply.AddMember("error", error, alloc);
}

}

int main(int argc, char* argv[])
{
    using namespace yutovo_solver;

    std::string document_guid = argc > 1 ? argv[1] : "";
    std::string logs_path = argc > 2 ? argv[2] : "";
    bool log_file = argc > 3 && std::string(argv[3]) == "1";

    CalculatorWorkerHost host(document_guid, logs_path, log_file);

    std::queue<PendingRequest> request_queue;
    std::mutex queue_mutex;
    std::condition_variable queue_cv;
    std::atomic<bool> reader_done{false};

    //the reader thread handles break and remove_solver immediately, everything else is queued for the main thread
    std::thread reader(
        [&]()
        {
            std::string line;
            while (std::getline(std::cin, line))
            {
                if (line.empty())
                    continue;

                PendingRequest pending;
                pending.request.Parse<0>(line.c_str());
                if (pending.request.HasParseError() || !pending.request.IsObject() ||
                    !pending.request.HasMember("action") || !pending.request["action"].IsString())
                    continue;

                std::string action = pending.request["action"].GetString();

                if (action == "break")
                {
                    try
                    {
                        host.HandleBreak(pending.request);
                    }
                    catch (...)
                    {
                        //a malformed break must not kill the worker
                    }
                    continue;
                }

                if (action == "remove_solver" && pending.request.HasMember("solver_key") && pending.request["solver_key"].IsString())
                {
                    host.RemoveSolver(pending.request["solver_key"].GetString());
                    continue;
                }

                {
                    std::lock_guard<std::mutex> lock(queue_mutex);
                    request_queue.push(std::move(pending));
                }
                queue_cv.notify_one();
            }

            reader_done = true;
            queue_cv.notify_all();
        });

    while (true)
    {
        PendingRequest pending;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue_cv.wait(lock,
                [&]()
                {
                    return !request_queue.empty() || reader_done.load();
                });
            if (request_queue.empty())
                break;
            pending = std::move(request_queue.front());
            request_queue.pop();
        }

        rapidjson::Document reply;
        reply.SetObject();
        try
        {
            host.HandleRequest(pending.request, reply);
            WriteReply(reply);
        }
        catch (ServiceException& ex)
        {
            WriteThrow(ex.error_code);
        }
        catch (...)
        {
            //a break or a timeout lands inside a running giac evaluation and throws a runtime error
            //that the parser level catches do not translate, report it as an interrupted solving
            if (giac::ctrl_c || giac::interrupted)
            {
                rapidjson::Document break_reply;
                MakeInterruptedReply(pending.request, break_reply);
                WriteReply(break_reply);
            }
            else
            {
                WriteThrow(ErrorCode::SOLVER_ERROR);
            }
        }
    }

    reader.join();
    return 0;
}

#else

int main(int argc, char* argv[])
{
    return 0;
}

#endif
