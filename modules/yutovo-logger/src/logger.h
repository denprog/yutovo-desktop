/*
 * Yutovo Logger
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#define FMT_HEADER_ONLY
#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <utility>

namespace yutovo
{

enum class LogLevel
{
    LEVEL_TRACE = 0,
    LEVEL_DEBUG,
    LEVEL_INFO,
    LEVEL_WARNING,
    LEVEL_ERROR,
    LEVEL_CRITICAL,
    LEVEL_OFF
};

class Logger
{
private:
    Logger(const std::string& _path, const std::string& _name, bool in_console, bool in_file);

public:
    Logger(Logger const&) = delete;
    void operator=(Logger const&) = delete;

    static Logger* GetInstance(const std::string& path, const std::string& name, bool in_console, bool in_file);

    void Trace(const char* message);
    void Debug(const char* message);
    void Info(const char* message);
    void Warning(const char* message);
    void Error(const char* message);
    void Critical(const char* message);

    template<typename... Args>
    void Trace(fmt::format_string<Args...> format, Args... args)
    {
#ifdef EMSCRIPTEN
        if (log_level > LogLevel::LEVEL_TRACE)
            return;
        std::string m = fmt::format(format, std::forward<Args>(args)...);
        printf("[trace] %s\n", m.c_str());
#else
        log->trace(format, std::forward<Args>(args)...);
        log->flush();
#endif
    }

    template<typename... Args>
    void Debug(fmt::format_string<Args...> format, Args... args)
    {
#ifdef EMSCRIPTEN
        if (log_level > LogLevel::LEVEL_DEBUG)
            return;
        std::string m = fmt::format(format, std::forward<Args>(args)...);
        printf("[debug] %s\n", m.c_str());
#else
        log->debug(format, std::forward<Args>(args)...);
        log->flush();
#endif
    }

    template<typename... Args>
    void Info(fmt::format_string<Args...> format, Args... args)
    {
#ifdef EMSCRIPTEN
        if (log_level > LogLevel::LEVEL_INFO)
            return;
        std::string m = fmt::format(format, std::forward<Args>(args)...);
        printf("[info] %s\n", m.c_str());
#else
        log->info(format, std::forward<Args>(args)...);
        log->flush();
#endif
    }

    template<typename... Args>
    void Warning(fmt::format_string<Args...> format, Args... args)
    {
#ifdef EMSCRIPTEN
        if (log_level > LogLevel::LEVEL_WARNING)
            return;
        std::string m = fmt::format(format, std::forward<Args>(args)...);
        printf("[warning] %s\n", m.c_str());
#else
        log->warn(format, std::forward<Args>(args)...);
        log->flush();
#endif
    }

    template<typename... Args>
    void Error(fmt::format_string<Args...> format, Args... args)
    {
#ifdef EMSCRIPTEN
        if (log_level > LogLevel::LEVEL_ERROR)
            return;
        std::string m = fmt::format(format, std::forward<Args>(args)...);
        printf("[error] %s\n", m.c_str());
#else
        log->error(format, std::forward<Args>(args)...);
        log->flush();
#endif
    }

    template<typename... Args>
    void Critical(fmt::format_string<Args...> format, Args... args)
    {
#ifdef EMSCRIPTEN
        if (log_level > LogLevel::LEVEL_CRITICAL)
            return;
        std::string m = fmt::format(format, std::forward<Args>(args)...);
        printf("[critical] %s\n", m.c_str());
#else
        log->critical(format, std::forward<Args>(args)...);
        log->flush();
#endif
    }

    void SetLevel(LogLevel level);
    LogLevel GetLevel();

private:
    std::string path;
    std::string name;

#ifdef EMSCRIPTEN
    LogLevel log_level = LogLevel::LEVEL_INFO;
#else
    std::shared_ptr<spdlog::logger> log;
#endif
};

class LoggerFormatter : public spdlog::formatter
{
public:
    LoggerFormatter();

    virtual void format(const spdlog::details::log_msg &msg, spdlog::memory_buf_t &dest);
    virtual std::unique_ptr<spdlog::formatter> clone() const;

private:
    std::vector<std::unique_ptr<spdlog::details::flag_formatter>> formatters;
};

}

#endif
