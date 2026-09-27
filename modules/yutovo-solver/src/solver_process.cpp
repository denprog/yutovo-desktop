/*
 * Yutovo Solver
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "types.h"

#ifdef YUTOVO_SOLVER_WORKER

#include "solver_process.h"
#include <boost/process.hpp>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <yutovo-calculator/parser_exception.h>
#include <chrono>
#include <condition_variable>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <map>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#else
#include <cerrno>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace yutovo_solver
{

//Extra wall-clock headroom on top of max_time before the process is killed, the in-worker CPU timer should fire first
static constexpr int64_t timeout_reserve_ms = 5000;
static constexpr int64_t break_wait_ms = 1000;
static std::once_flag ignore_sigpipe_once;

static std::mutex worker_registry_mutex;
static std::map<std::string, std::weak_ptr<SolverProcess>> worker_registry; //worker processes by document guid

//SolverProcess

SolverProcess::SolverProcess(const std::string& _document_guid, uint64_t _max_time, const std::string& _logs_path, bool _log_console,
    bool _log_file) :
    document_guid(_document_guid),
    logs_path(_logs_path),
    log_console(_log_console),
    log_file(_log_file),
    max_time(_max_time),
    logger(Logger::GetInstance(_logs_path + "/yutovo-solver", "solver-process", _log_console, _log_file))
{
}

SolverProcess::~SolverProcess()
{
    DoTerminate();
}

//One shared worker process per document
std::shared_ptr<SolverProcess> SolverProcess::GetProcess(const std::string& document_guid, uint64_t max_time, const std::string& logs_path,
    bool log_console, bool log_file)
{
    std::lock_guard<std::mutex> lock(worker_registry_mutex);
    auto it = worker_registry.find(document_guid);
    if (it != worker_registry.end())
    {
        std::shared_ptr<SolverProcess> process = it->second.lock();
        if (process)
            return process;
    }

    std::shared_ptr<SolverProcess> process(new SolverProcess(document_guid, max_time, logs_path, log_console, log_file));
    worker_registry[document_guid] = process;
    return process;
}

//Returns an existing worker process without starting a new one
std::shared_ptr<SolverProcess> SolverProcess::FindProcess(const std::string& document_guid)
{
    std::lock_guard<std::mutex> lock(worker_registry_mutex);
    auto it = worker_registry.find(document_guid);
    if (it != worker_registry.end())
        return it->second.lock();
    return std::shared_ptr<SolverProcess>();
}

//Send an action and wait for the worker reply; throws ServiceException when the worker threw one
void SolverProcess::SendAction(const std::string& solver_key, const std::string& action, const rapidjson::Document& request,
    rapidjson::Document& reply, int language)
{
    std::lock_guard<std::mutex> action_lock(action_mutex);

    if (!alive && !EnsureStarted())
    {
        MakeErrorReply(ErrorCode::SOLVER_ERROR, reply);
        return;
    }

    //build the envelope around the original session request, the worker applies language and max_time to its solvers
    rapidjson::Document envelope;
    envelope.SetObject();
    auto& alloc = envelope.GetAllocator();
    envelope.AddMember("action", rapidjson::Value(action.c_str(), alloc), alloc);
    envelope.AddMember("solver_key", rapidjson::Value(solver_key.c_str(), alloc), alloc);
    envelope.AddMember("language", language, alloc);
    rapidjson::Value max_time_value;
    max_time_value.SetUint64(max_time.load());
    envelope.AddMember("max_time", max_time_value, alloc);
    rapidjson::Value request_copy;
    request_copy.CopyFrom(request, alloc);
    envelope.AddMember("request", request_copy, alloc);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    envelope.Accept(writer);
    std::string line = buffer.GetString();
    line += '\n';

    request_pending = true;

    SendLine(line);

    uint64_t current_max_time = max_time.load();
    int64_t timeout_ms = -1;
    if (current_max_time > 0)
        timeout_ms = static_cast<int64_t>(current_max_time) + TimeoutReserveMs();

    std::string response_line;
    if (!ReceiveLine(response_line, timeout_ms))
    {
        bool killed = !alive; //terminated by BREAK_SOLVING from another thread
        bool timed_out = alive; //no reply and no death within the deadline
        logger->Error("Worker did not reply, terminating");
        DoTerminate();
        if (action == "solve" && (killed || timed_out))
            MakeBreakReply(request, reply); //report the killed solve as a time exceed
        else
            MakeErrorReply(ErrorCode::SOLVER_ERROR, reply);
        return;
    }

    request_pending = false;
    {
        std::lock_guard<std::mutex> l(break_mutex);
        break_cv.notify_all();
    }

    rapidjson::Document response;
    response.Parse<0>(response_line.c_str());
    if (response.HasParseError() || !response.IsObject())
    {
        logger->Error("Worker returned malformed reply");
        bool killed = !alive; //a partial line read after the worker was killed
        DoTerminate();
        if (action == "solve" && killed)
            MakeBreakReply(request, reply);
        else
            MakeErrorReply(ErrorCode::SOLVER_ERROR, reply);
        return;
    }

    if (response.HasMember("throw") && response["throw"].IsInt())
    {
        reply.SetObject();
        throw ServiceException{(ErrorCode)response["throw"].GetInt()};
    }

    reply.SetObject();
    if (response.HasMember("reply"))
        reply.CopyFrom(response["reply"], reply.GetAllocator());
    else
        MakeErrorReply(ErrorCode::SOLVER_ERROR, reply);
}

//Send BREAK_SOLVING without waiting for a reply, then optionally wait up to 1 second for the active request to finish, killing the process otherwise
void SolverProcess::SendBreak(const std::string& solver_key, const rapidjson::Document& request, bool wait)
{
    //action_mutex must not be taken here: an in-flight solve holds it until its reply arrives,
    //while the worker reader thread consumes the break line immediately
    if (!alive)
        return; //nothing to break

    rapidjson::Document envelope;
    envelope.SetObject();
    auto& alloc = envelope.GetAllocator();
    envelope.AddMember("action", "break", alloc);
    envelope.AddMember("solver_key", rapidjson::Value(solver_key.c_str(), alloc), alloc);
    rapidjson::Value request_copy;
    request_copy.CopyFrom(request, alloc);
    envelope.AddMember("request", request_copy, alloc);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    envelope.Accept(writer);
    std::string line = buffer.GetString();
    line += '\n';

    SendLine(line);

    if (wait)
        WaitForRequestOrKill();
}

//Ask the worker to forget a solver, no reply is expected
void SolverProcess::SendRemoveSolver(const std::string& solver_key)
{
    std::lock_guard<std::mutex> action_lock(action_mutex);
    if (!alive)
        return;

    //the worker handles remove_solver in its reader thread and never replies
    rapidjson::Document envelope;
    envelope.SetObject();
    auto& alloc = envelope.GetAllocator();
    envelope.AddMember("action", "remove_solver", alloc);
    envelope.AddMember("solver_key", rapidjson::Value(solver_key.c_str(), alloc), alloc);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    envelope.Accept(writer);
    std::string line = buffer.GetString();
    line += '\n';

    SendLine(line);
}

void SolverProcess::WaitForRequestOrKill()
{
    std::unique_lock<std::mutex> lock(break_mutex);
    if (!request_pending)
        return;

    bool finished = break_cv.wait_for(lock, std::chrono::milliseconds(break_wait_ms),
        [this]
        {
            return !request_pending || !alive;
        });
    if (!finished && request_pending && alive)
    {
        //release the lock before terminating, DoTerminate notifies through the same mutex
        lock.unlock();
        logger->Error("Worker did not finish within 1 second, terminating");
        DoTerminate();
    }
}

void SolverProcess::SetMaxTime(uint64_t _max_time)
{
    max_time.store(_max_time);
}

bool SolverProcess::EnsureStarted()
{
    if (alive)
        return true;

#ifndef _WIN32
    //writing into a pipe of a killed worker must not kill the parent service with SIGPIPE
    std::call_once(ignore_sigpipe_once,
        []() -> void
        {
            ::signal(SIGPIPE, SIG_IGN);
        });
#endif

    std::string exe = FindWorkerExecutable();
    if (exe.empty())
    {
        logger->Error("Calculator worker executable not found");
        return false;
    }

    //worker stdout is the IPC channel, so its console logging must stay off
    std::lock_guard<std::mutex> pipes_lock(pipes_mutex);
    in.reset(new boost::process::opstream());
    out.reset(new boost::process::ipstream());
    receive_buffer.clear();
    try
    {
        child.reset(new boost::process::child(exe, document_guid, logs_path, log_file ? "1" : "0",
            boost::process::std_in < *in, boost::process::std_out > *out, boost::process::std_err > stderr));
    }
    catch (const std::exception& e)
    {
        logger->Error("Failed to start calculator worker: {}", e.what());
        in.reset();
        out.reset();
        return false;
    }

    if (!child->running())
    {
        logger->Error("Failed to start calculator worker");
        in.reset();
        out.reset();
        child.reset();
        return false;
    }

    logger->Info("Calculator worker started for document: {} (pid {})", document_guid, child->id());
    child_pid = static_cast<int64_t>(child->id());
    alive = true;
    return true;
}

void SolverProcess::DoTerminate()
{
    //kill by the remembered pid, the child object is only touched under pipes_mutex
    int64_t pid = child_pid.exchange(-1);
    if (pid > 0)
    {
#ifdef _WIN32
        //TerminateProcess is the windows equivalent of a hard kill
        HANDLE process = ::OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, static_cast<DWORD>(pid));
        if (process)
        {
            ::TerminateProcess(process, 1);
            ::CloseHandle(process);
        }
#else
        //SIGTERM is ignored by a stuck giac, kill the worker process directly
        ::kill(static_cast<pid_t>(pid), SIGKILL);
#endif
    }

    alive = false;
    request_pending = false;

    std::unique_ptr<boost::process::child> finished_child;
    {
        std::lock_guard<std::mutex> pipes_lock(pipes_mutex);
        finished_child = std::move(child);
    }
    if (finished_child)
    {
        //reap the child asynchronously so this thread never blocks
        std::thread(
            [c = std::move(finished_child)]() mutable
            {
                try
                {
                    if (c)
                        c->wait();
                }
                catch (...)
                {
                    //ignore
                }
            }).detach();
    }

    {
        std::lock_guard<std::mutex> l(break_mutex);
        break_cv.notify_all();
    }
}

void SolverProcess::SendLine(const std::string& line)
{
    std::lock_guard<std::mutex> pipes_lock(pipes_mutex);
    if (!in)
        return;
    std::lock_guard<std::mutex> lock(send_mutex);
    *in << line;
    in->flush();
}

//Returns the number of bytes read into the buffer, 0 when nothing arrived during the wait, -1 when the pipe is closed
int SolverProcess::ReadPipeWithWait(void* pipe_handle, char* buf, size_t buf_size, int wait_ms)
{
#ifdef _WIN32
    HANDLE handle = static_cast<HANDLE>(pipe_handle);
    DWORD available = 0;
    if (!::PeekNamedPipe(handle, nullptr, 0, nullptr, &available, nullptr))
        return -1; //the write end is closed, the worker is gone
    if (available == 0)
    {
        ::Sleep(wait_ms);
        return 0;
    }
    DWORD read_bytes = 0;
    if (!::ReadFile(handle, buf, static_cast<DWORD>(std::min(buf_size, static_cast<size_t>(available))), &read_bytes, nullptr))
        return -1;
    return static_cast<int>(read_bytes);
#else
    int fd = static_cast<int>(reinterpret_cast<intptr_t>(pipe_handle));
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    int ret = ::poll(&pfd, 1, wait_ms);
    if (ret < 0)
    {
        if (errno == EINTR)
            return 0;
        return -1;
    }
    if (ret == 0)
        return 0;
    ssize_t n = ::read(fd, buf, buf_size);
    if (n < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return 0;
        return -1;
    }
    if (n == 0)
        return -1; //EOF, the worker closed its stdout
    return static_cast<int>(n);
#endif
}

bool SolverProcess::ReceiveLine(std::string& line, int64_t timeout_ms)
{
    std::lock_guard<std::mutex> lock(receive_mutex);
    void* pipe_handle = nullptr;
    {
        //the pipe object is only read here and replaced in EnsureStarted between requests
        std::lock_guard<std::mutex> pipes_lock(pipes_mutex);
        if (out)
        {
            auto* pipebuf = dynamic_cast<boost::process::basic_pipebuf<char>*>(out->rdbuf());
            if (pipebuf)
            {
#ifdef _WIN32
                pipe_handle = pipebuf->pipe().native_handle();
#else
                pipe_handle = reinterpret_cast<void*>(static_cast<intptr_t>(pipebuf->pipe().native_source()));
#endif
            }
        }
    }
    if (!pipe_handle)
        return false;

    auto start = std::chrono::steady_clock::now();
    line.clear();

    while (true)
    {
        auto newline_pos = receive_buffer.find('\n');
        if (newline_pos != std::string::npos)
        {
            line = receive_buffer.substr(0, newline_pos);
            receive_buffer.erase(0, newline_pos + 1);
            return true;
        }

        int wait_ms = 100;
        if (timeout_ms >= 0)
        {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
            if (elapsed >= timeout_ms)
                return false;
            wait_ms = static_cast<int>(std::min<int64_t>(timeout_ms - elapsed, 100));
        }

        char buf[4096];
        int n = ReadPipeWithWait(pipe_handle, buf, sizeof(buf), wait_ms);
        if (n < 0)
        {
            if (!receive_buffer.empty())
            {
                line = std::move(receive_buffer);
                receive_buffer.clear();
                return true;
            }
            return false;
        }
        if (n > 0)
            receive_buffer.append(buf, static_cast<size_t>(n));
    }
}

std::string SolverProcess::FindWorkerExecutable()
{
    const char* env_path = std::getenv("YUTOVO_SOLVER_WORKER_PATH");
    if (env_path)
    {
        std::filesystem::path p(env_path);
        std::error_code ec;
        if (std::filesystem::exists(p, ec))
            return p.string();
    }

    //try next to the running executable and its ../src directory
    std::filesystem::path self_dir;
#ifdef _WIN32
    char path[MAX_PATH];
    DWORD length = ::GetModuleFileNameA(nullptr, path, MAX_PATH);
    if (length > 0 && length < MAX_PATH)
        self_dir = std::filesystem::path(path).parent_path();
#elif defined(__linux__)
    std::error_code ec;
    std::filesystem::path self_exe("/proc/self/exe");
    if (std::filesystem::exists(self_exe, ec))
        self_dir = std::filesystem::read_symlink(self_exe).parent_path();
#endif
    if (!self_dir.empty())
    {
        auto candidate = FindWorkerInDir(self_dir);
        if (candidate)
            return candidate->string();

        candidate = FindWorkerInDir(self_dir / ".." / "src");
        if (candidate)
        {
            std::error_code canonical_ec;
            std::filesystem::path canonical_path = std::filesystem::canonical(*candidate, canonical_ec);
            return canonical_ec ? candidate->string() : canonical_path.string();
        }
    }

    const char* deploy = std::getenv("YUTOVO_DEPLOY");
    if (deploy)
    {
        auto candidate = FindWorkerInDir(std::filesystem::path(deploy) / "bin");
        if (candidate)
            return candidate->string();
    }

    std::error_code ec2;
    auto candidate = FindWorkerInDir(std::filesystem::current_path(ec2));
    if (candidate)
        return candidate->string();

    //rely on PATH
    return "yutovo-solver-calculator-worker";
}

std::optional<std::filesystem::path> SolverProcess::FindWorkerInDir(const std::filesystem::path& dir)
{
    std::error_code ec;
    std::filesystem::path candidate = dir / ("yutovo-solver-calculator-worker");
#ifdef _WIN32
    candidate += ".exe";
#endif
    if (std::filesystem::exists(candidate, ec))
        return candidate;
    candidate = dir / ("yutovo-solver-calculator-workerd");
#ifdef _WIN32
    candidate += ".exe";
#endif
    if (std::filesystem::exists(candidate, ec))
        return candidate;
    return std::nullopt;
}

int64_t SolverProcess::TimeoutReserveMs()
{
#ifdef DEBUG
    const char* env = std::getenv("YUTOVO_SOLVER_TIMEOUT_RESERVE_MS");
    if (env && *env)
        return std::atoll(env);
#endif
    return timeout_reserve_ms;
}

void SolverProcess::MakeErrorReply(ErrorCode error_code, rapidjson::Document& reply)
{
    reply.SetObject();
    auto& alloc = reply.GetAllocator();
    rapidjson::Value error;
    error.SetObject();
    error.AddMember("error_code", (int)error_code, alloc);
    reply.AddMember("error", error, alloc);
}

void SolverProcess::MakeBreakReply(const rapidjson::Document& request, rapidjson::Document& reply)
{
    reply.SetObject();
    auto& alloc = reply.GetAllocator();
    rapidjson::Value error;
    error.SetObject();

    rapidjson::Value id(rapidjson::kArrayType);
    if (request.HasMember("id") && request["id"].IsArray())
    {
        const rapidjson::Value& arr = request["id"];
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
        {
            if (arr[i].IsInt())
                id.PushBack(arr[i].GetInt(), alloc);
        }
    }
    error.AddMember("id", id, alloc);
    error.AddMember("error_code", (int)ErrorCode::PARSER_ERROR, alloc);
    error.AddMember("parser_error_code", (int)yutovo_calculator::ParserExceptionCode::TimeExceed, alloc);
    error.AddMember("pos", -1, alloc);
    error.AddMember("size", -1, alloc);
    error.AddMember("line", -1, alloc);
    reply.AddMember("error", error, alloc);
}

}

#endif
