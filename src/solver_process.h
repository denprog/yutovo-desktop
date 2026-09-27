/*
 * Yutovo Solver
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __SOLVER_PROCESS_H__
#define __SOLVER_PROCESS_H__

#include "types.h"

#ifdef YUTOVO_SOLVER_WORKER

#include "rapidjson/document.h"
#include <yutovo-logger/logger.h>
#include <boost/process.hpp>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#ifndef _WIN32
#include <sys/types.h>
#endif

namespace yutovo_solver
{

using yutovo::Logger;

//SolverProcess owns the calculator worker process for one document. All requests are synchronous: one request in flight
class SolverProcess
{
public:
    SolverProcess(const std::string& _document_guid, uint64_t _max_time, const std::string& _logs_path, bool _log_console, bool _log_file);
    ~SolverProcess();

    static std::shared_ptr<SolverProcess> GetProcess(const std::string& document_guid, uint64_t max_time, const std::string& logs_path,
        bool log_console, bool log_file);
    static std::shared_ptr<SolverProcess> FindProcess(const std::string& document_guid);

    void SendAction(const std::string& solver_key, const std::string& action, const rapidjson::Document& request, rapidjson::Document& reply,
        int language = 0);
    void SendBreak(const std::string& solver_key, const rapidjson::Document& request, bool wait);
    void SendRemoveSolver(const std::string& solver_key);

    void SetMaxTime(uint64_t _max_time);

private:
    bool EnsureStarted();
    void DoTerminate();
    void SendLine(const std::string& line);
    bool ReceiveLine(std::string& line, int64_t timeout_ms);
    void WaitForRequestOrKill();

    static int ReadPipeWithWait(void* pipe_handle, char* buf, size_t buf_size, int wait_ms);

    std::string FindWorkerExecutable();
    static std::optional<std::filesystem::path> FindWorkerInDir(const std::filesystem::path& dir);

    static int64_t TimeoutReserveMs();

    static void MakeErrorReply(ErrorCode error_code, rapidjson::Document& reply);
    static void MakeBreakReply(const rapidjson::Document& request, rapidjson::Document& reply);

private:
    std::string document_guid;
    std::string logs_path;
    bool log_console;
    bool log_file;

    std::atomic<uint64_t> max_time{0};

    std::atomic<bool> alive{false};
    std::atomic<bool> request_pending{false};
    //pid_t is POSIX only, a windows process id fits as well
    std::atomic<int64_t> child_pid{-1};

    std::mutex action_mutex;
    std::mutex send_mutex;
    std::mutex receive_mutex;
    std::string receive_buffer;
    std::mutex pipes_mutex; //protects the in/out/child pointers across restarts and terminations
    std::mutex break_mutex;
    std::condition_variable break_cv;

    std::unique_ptr<boost::process::opstream> in;
    std::unique_ptr<boost::process::ipstream> out;
    std::unique_ptr<boost::process::child> child;

    Logger* logger = nullptr;
};

}

#endif

#endif
