/*
 * Yutovo Solver
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <yutovo-logger/logger.h>
#include <yutovo-calculator/parser_exception.h>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <thread>
#include <vector>
#include "service_config.h"
#include "service_context.h"
#include "session.h"
#include "types.h"

namespace yutovo_test
{

using namespace yutovo_solver;
using namespace yutovo;

struct SessionTest : public testing::Test
{
    Logger* logger = nullptr;
    ServiceConfig config;
    ServiceContext context;
    Session session;

    SessionTest() :
        logger(Logger::GetInstance("", "test", false, false)),
        config(logger),
        context(&config, "", false, false),
        session(&context, "", false, false)
    {
    }

    rapidjson::Document ParseReply(const std::string& reply)
    {
        rapidjson::Document doc;
        doc.Parse<0>(reply.c_str());
        EXPECT_FALSE(doc.HasParseError());
        return doc;
    }

    static std::string MakeSolveRequest(const char* document_guid, const char* solver_guid, int code_id, int result_type, const char* expression)
    {
        rapidjson::Document doc;
        doc.SetObject();
        auto& alloc = doc.GetAllocator();
        doc.AddMember("command", "SOLVE_CODE", alloc);
        doc.AddMember("document_guid", rapidjson::Value(document_guid, alloc), alloc);
        doc.AddMember("solver_guid", rapidjson::Value(solver_guid, alloc), alloc);
        doc.AddMember("code_id", code_id, alloc);
        doc.AddMember("solver_type", static_cast<int>(SolverType::CALCULATOR), alloc);
        doc.AddMember("result_type", result_type, alloc);
        std::string expr = expression;
        expr += ";";
        doc.AddMember("expression", rapidjson::Value(expr.c_str(), alloc), alloc);
        doc.AddMember("id", rapidjson::Value(rapidjson::kArrayType).Move(), alloc);
        doc.AddMember("timestamp", 1, alloc);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);
        return buffer.GetString();
    }

    static std::string MakeBreakRequest(const char* document_guid, const char* solver_guid, int code_id, int64_t timestamp)
    {
        rapidjson::Document doc;
        doc.SetObject();
        auto& alloc = doc.GetAllocator();
        doc.AddMember("command", "BREAK_SOLVING", alloc);
        doc.AddMember("document_guid", rapidjson::Value(document_guid, alloc), alloc);
        doc.AddMember("solver_guid", rapidjson::Value(solver_guid, alloc), alloc);
        doc.AddMember("code_id", code_id, alloc);
        doc.AddMember("solver_type", static_cast<int>(SolverType::CALCULATOR), alloc);
        doc.AddMember("id", rapidjson::Value(rapidjson::kArrayType).Move(), alloc);
        doc.AddMember("timestamp", timestamp, alloc);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);
        return buffer.GetString();
    }
};

TEST_F(SessionTest, invalid_json_returns_json_error)
{
    std::string reply;
    session.Parse("not a json", reply);
    auto doc = ParseReply(reply);
    ASSERT_TRUE(doc.HasMember("error"));
    ASSERT_EQ(doc["error"].GetInt(), static_cast<int>(ErrorCode::JSON_ERROR));
}

TEST_F(SessionTest, missing_command_returns_no_field_error)
{
    std::string reply;
    session.Parse("{}", reply);
    auto doc = ParseReply(reply);
    ASSERT_TRUE(doc.HasMember("error"));
    ASSERT_EQ(doc["error"].GetInt(), static_cast<int>(ErrorCode::NO_FIELD_ERROR));
}

TEST_F(SessionTest, command_not_string_returns_no_field_error)
{
    std::string reply;
    session.Parse("{\"command\":123}", reply);
    auto doc = ParseReply(reply);
    ASSERT_TRUE(doc.HasMember("error"));
    ASSERT_EQ(doc["error"].GetInt(), static_cast<int>(ErrorCode::NO_FIELD_ERROR));
}

TEST_F(SessionTest, unknown_command_returns_unknown_command)
{
    std::string reply;
    session.Parse("{\"command\":\"UNKNOWN\"}", reply);
    auto doc = ParseReply(reply);
    ASSERT_TRUE(doc.HasMember("error"));
    ASSERT_EQ(doc["error"].GetInt(), static_cast<int>(ErrorCode::UNKNOWN_COMMAND));
}

TEST_F(SessionTest, exit_command_sets_exit_flag_and_returns_ok)
{
    ASSERT_FALSE(context.exit);
    std::string reply;
    session.Parse("{\"command\":\"EXIT\"}", reply);
    ASSERT_TRUE(context.exit);
    auto doc = ParseReply(reply);
    ASSERT_TRUE(doc.HasMember("result"));
    ASSERT_EQ(doc["result"].GetInt(), static_cast<int>(ErrorCode::OK));
}

TEST_F(SessionTest, remove_solver_missing_solver_guid_returns_no_field_error)
{
    std::string reply;
    session.Parse("{\"command\":\"REMOVE_SOLVER\",\"code_id\":1}", reply);
    auto doc = ParseReply(reply);
    ASSERT_TRUE(doc.HasMember("error"));
    ASSERT_EQ(doc["error"].GetInt(), static_cast<int>(ErrorCode::NO_FIELD_ERROR));
}

TEST_F(SessionTest, remove_solver_not_found_returns_solver_error)
{
    std::string reply;
    session.Parse("{\"command\":\"REMOVE_SOLVER\",\"solver_guid\":\"missing\",\"code_id\":1}", reply);
    auto doc = ParseReply(reply);
    ASSERT_TRUE(doc.HasMember("error"));
    ASSERT_EQ(doc["error"].GetInt(), static_cast<int>(ErrorCode::SOLVER_ERROR));
}

TEST_F(SessionTest, solve_code_missing_document_guid_returns_no_field_error)
{
    std::string reply;
    session.Parse("{\"command\":\"SOLVE_CODE\"}", reply);
    auto doc = ParseReply(reply);
    ASSERT_TRUE(doc.HasMember("error"));
    ASSERT_EQ(doc["error"].GetInt(), static_cast<int>(ErrorCode::NO_FIELD_ERROR));
}

TEST_F(SessionTest, solve_code_first_call_returns_restarted_error)
{
    std::string reply;
    session.Parse(MakeSolveRequest("doc_json", "solver_json", 1, static_cast<int>(ResultType::REAL), "1+2"), reply);
    auto doc = ParseReply(reply);
    ASSERT_TRUE(doc.HasMember("error"));
    ASSERT_EQ(doc["error"]["error_code"].GetInt(), static_cast<int>(ErrorCode::SOLVER_RESTARTED_ERROR));
}

TEST_F(SessionTest, break_solving_terminates_long_computation_within_timeout)
{
    //warm up the solver so the next solve is not rejected as restarted
    std::string reply;
    session.Parse(MakeSolveRequest("doc_break", "solver_break", 1, static_cast<int>(ResultType::REAL), "1+2"), reply);

    auto start = std::chrono::steady_clock::now();
    std::string solve_reply;
    std::thread solve_thread(
        [&]()
        {
            session.Parse(MakeSolveRequest("doc_break", "solver_break", 1, static_cast<int>(ResultType::AUTO),
                "definite_integral(0,1,inv(x+j),x)"), solve_reply);
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    std::string break_reply;
    session.Parse(MakeBreakRequest("doc_break", "solver_break", 1, 1), break_reply);

    solve_thread.join();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
    EXPECT_LT(elapsed, 5000) << "solve did not return soon after BREAK_SOLVING";
}

TEST_F(SessionTest, stress_multiple_concurrent_breaks)
{
    constexpr int documents_number = 5;
    session.SetMaxTime(3000);

    std::vector<std::thread> solve_threads;
    std::vector<std::string> solve_replies(documents_number);
    std::atomic<int> completed{0};

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < documents_number; ++i)
    {
        std::string doc_guid = "doc_stress_" + std::to_string(i);
        std::string solver_guid = "solver_stress_" + std::to_string(i);

        //warm up so the long solve is not rejected as restarted
        std::string warmup_reply;
        session.Parse(MakeSolveRequest(doc_guid.c_str(), solver_guid.c_str(), 1, static_cast<int>(ResultType::REAL), "1+2"), warmup_reply);

        solve_threads.emplace_back(
            [&, i, doc_guid, solver_guid]()
            {
                session.Parse(MakeSolveRequest(doc_guid.c_str(), solver_guid.c_str(), 1, static_cast<int>(ResultType::AUTO),
                    "definite_integral(0,1,inv(x+j),x)"), solve_replies[i]);
                ++completed;
            });
    }

    //let all computations start before breaking them
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    for (int i = 0; i < documents_number; ++i)
    {
        std::string break_reply;
        session.Parse(MakeBreakRequest(("doc_stress_" + std::to_string(i)).c_str(),
            ("solver_stress_" + std::to_string(i)).c_str(), 1, 1), break_reply);
    }

    for (auto& t : solve_threads)
        t.join();

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
    EXPECT_EQ(completed.load(), documents_number) << "not all solve threads completed";
    EXPECT_LT(elapsed, 8000) << "stress test took too long, possible worker hang";
}

TEST_F(SessionTest, stress_twenty_documents_heavy_computations)
{
    constexpr int documents_number = 20;
    session.SetMaxTime(5000);

    //use a single known-heavy expression to avoid flakiness from unexpected fast/slow integrals
    std::vector<std::string> expressions(documents_number, "definite_integral(0,1,inv(x+j),x)");

    std::vector<std::thread> solve_threads;
    std::vector<std::string> solve_replies(documents_number);
    std::atomic<int> completed{0};

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < documents_number; ++i)
    {
        std::string doc_guid = "doc_heavy_" + std::to_string(i);
        std::string solver_guid = "solver_heavy_" + std::to_string(i);
        const std::string& expression = expressions[i % expressions.size()];

        //warm up so the long solve is not rejected as restarted
        std::string warmup_reply;
        session.Parse(MakeSolveRequest(doc_guid.c_str(), solver_guid.c_str(), 1, static_cast<int>(ResultType::REAL), "1+2"), warmup_reply);

        solve_threads.emplace_back(
            [&, i, doc_guid, solver_guid, expression]()
            {
                session.Parse(MakeSolveRequest(doc_guid.c_str(), solver_guid.c_str(), 1,
                    static_cast<int>(ResultType::AUTO), expression.c_str()), solve_replies[i]);
                ++completed;
            });
    }

    //let all heavy computations start before breaking them
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    for (int i = 0; i < documents_number; ++i)
    {
        std::string break_reply;
        session.Parse(MakeBreakRequest(("doc_heavy_" + std::to_string(i)).c_str(), ("solver_heavy_" + std::to_string(i)).c_str(), 1, 1), break_reply);
    }

    for (auto& t : solve_threads)
        t.join();

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
    EXPECT_EQ(completed.load(), documents_number) << "not all solve threads completed";
    EXPECT_LT(elapsed, 60000) << "stress test took too long, possible worker hang";
}

static void SetEnvValue(const char* name, const char* value)
{
#ifdef _WIN32
    _putenv_s(name, value);
#else
    setenv(name, value, 1);
#endif
}

//Points the solver at the worker build that cannot interrupt its own giac evaluations
struct UninterruptibleWorkerPath
{
    UninterruptibleWorkerPath()
    {
        std::error_code ec;
        std::filesystem::path self_dir;
#ifdef _WIN32
        const char* names[2] = {"yutovo-solver-calculator-worker-testd.exe", "yutovo-solver-calculator-worker-test.exe"};
        char path[MAX_PATH];
        DWORD length = ::GetModuleFileNameA(nullptr, path, MAX_PATH);
        if (length > 0 && length < MAX_PATH)
            self_dir = std::filesystem::path(path).parent_path();
#else
        const char* names[2] = {"yutovo-solver-calculator-worker-testd", "yutovo-solver-calculator-worker-test"};
        std::filesystem::path self_exe("/proc/self/exe");
        if (std::filesystem::exists(self_exe, ec))
            self_dir = std::filesystem::read_symlink(self_exe, ec).parent_path();
#endif
        for (const char* name : names)
        {
            //the test worker target is built into the tests binary directory
            std::filesystem::path candidate = self_dir / name;
            if (std::filesystem::exists(candidate, ec))
            {
                SetEnvValue("YUTOVO_SOLVER_WORKER_PATH", candidate.string().c_str());
                found = true;
                return;
            }
        }
        found = false;
    }

    ~UninterruptibleWorkerPath()
    {
        SetEnvValue("YUTOVO_SOLVER_WORKER_PATH", "");
    }

    bool found;
};

//a worker stuck inside giac is killed after max_time plus the reserve, the reply is a time exceed
TEST_F(SessionTest, receive_timeout_kills_uninterruptible_worker)
{
    UninterruptibleWorkerPath worker_path;
    ASSERT_TRUE(worker_path.found) << "build yutovo-solver-calculator-worker-test next to the tests";
    SetEnvValue("YUTOVO_SOLVER_TIMEOUT_RESERVE_MS", "500");

    session.SetMaxTime(500);

    //warm up the fresh worker so the heavy solve is not rejected as restarted
    std::string warmup_reply;
    session.Parse(MakeSolveRequest("doc_kill", "solver_kill", 1, static_cast<int>(ResultType::REAL), "1+2"), warmup_reply);

    auto start = std::chrono::steady_clock::now();
    std::string reply;
    session.Parse(MakeSolveRequest("doc_kill", "solver_kill", 1, static_cast<int>(ResultType::AUTO), "definite_integral(0,1,inv(x+j),x)"), reply);
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();

    auto doc = ParseReply(reply);
    ASSERT_TRUE(doc.HasMember("error"));
    EXPECT_EQ(doc["error"]["error_code"].GetInt(), static_cast<int>(ErrorCode::PARSER_ERROR));
    EXPECT_EQ(doc["error"]["parser_error_code"].GetInt(), static_cast<int>(ParserExceptionCode::TimeExceed));
    EXPECT_LT(elapsed, 5000) << "the stuck worker was not killed in time";

    //the killed worker is replaced, so the first solve of the new process reports a restart
    SetEnvValue("YUTOVO_SOLVER_WORKER_PATH", "");
    std::string restart_reply;
    session.Parse(MakeSolveRequest("doc_kill", "solver_kill", 1, static_cast<int>(ResultType::REAL), "1+2"), restart_reply);
    auto restart_doc = ParseReply(restart_reply);
    ASSERT_TRUE(restart_doc.HasMember("error"));
    EXPECT_EQ(restart_doc["error"]["error_code"].GetInt(), static_cast<int>(ErrorCode::SOLVER_RESTARTED_ERROR));
    SetEnvValue("YUTOVO_SOLVER_TIMEOUT_RESERVE_MS", "");
}

//BREAK_SOLVING kills a stuck worker within one second instead of waiting for the receive timeout
TEST_F(SessionTest, break_kills_uninterruptible_worker)
{
    UninterruptibleWorkerPath worker_path;
    ASSERT_TRUE(worker_path.found) << "build yutovo-solver-calculator-worker-test next to the tests";
    SetEnvValue("YUTOVO_SOLVER_TIMEOUT_RESERVE_MS", "30000");

    session.SetMaxTime(60000);

    //warm up the fresh worker so the heavy solve is not rejected as restarted
    std::string warmup_reply;
    session.Parse(MakeSolveRequest("doc_break_kill", "solver_break_kill", 1, static_cast<int>(ResultType::REAL), "1+2"), warmup_reply);

    auto start = std::chrono::steady_clock::now();
    std::string solve_reply;
    std::thread solve_thread(
        [&]()
        {
            session.Parse(MakeSolveRequest("doc_break_kill", "solver_break_kill", 1, static_cast<int>(ResultType::AUTO),
                "definite_integral(0,1,inv(x+j),x)"), solve_reply);
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::string break_reply;
    session.Parse(MakeBreakRequest("doc_break_kill", "solver_break_kill", 1, 1), break_reply);

    solve_thread.join();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();

    auto doc = ParseReply(solve_reply);
    ASSERT_TRUE(doc.HasMember("error"));
    EXPECT_EQ(doc["error"]["error_code"].GetInt(), static_cast<int>(ErrorCode::PARSER_ERROR));
    EXPECT_EQ(doc["error"]["parser_error_code"].GetInt(), static_cast<int>(ParserExceptionCode::TimeExceed));
    EXPECT_LT(elapsed, 5000) << "BREAK_SOLVING did not kill the stuck worker within its one second wait";
    SetEnvValue("YUTOVO_SOLVER_TIMEOUT_RESERVE_MS", "");
}

}
