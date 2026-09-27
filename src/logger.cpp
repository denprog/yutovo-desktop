/*
 * Yutovo Logger
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "logger.h"
#include <memory>
#include <iostream>
#include <filesystem>
#include <spdlog/sinks/stdout_sinks.h>

namespace yutovo
{

//Logger

Logger::Logger(const std::string& _path, const std::string& _name, bool in_console, bool in_file) :
    path(_path),
    name(_name)
{
#ifndef EMSCRIPTEN
    std::vector<spdlog::sink_ptr> sinks;
    if (in_console)
    {
        auto s = std::make_shared<spdlog::sinks::stdout_sink_st>();
        s->set_formatter(std::unique_ptr<spdlog::formatter>(new LoggerFormatter()));
        sinks.push_back(s);
    }

    if (in_file)
    {
        std::string p = path + "/" + name + ".log";
        sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>(p, 0, 0, false, 10));
    }
    log = std::make_shared<spdlog::logger>(name, begin(sinks), end(sinks));
    if (!log)
        throw std::runtime_error("Creating log error");
    log->set_level(spdlog::level::info);

    if (in_file)
        log->flush();

    spdlog::set_pattern("[%H:%M:%S.%e][%t][%n][%l] %v");
#endif
}

Logger* Logger::GetInstance(const std::string& path, const std::string& name, bool in_console, bool in_file)
{
    static std::mutex loggers_mutex;
    std::unique_lock<std::mutex> lock(loggers_mutex);
    static std::vector<std::shared_ptr<Logger>> loggers;
    for (auto& logger : loggers)
    {
        if (logger->path == path && logger->name == name)
            return logger.get();
    }
    loggers.emplace_back(new Logger(path, name, in_console, in_file));
    return loggers[loggers.size() - 1].get();
}

void Logger::Trace(const char* message)
{
#ifdef EMSCRIPTEN
    if (log_level > LogLevel::LEVEL_TRACE)
        return;
    printf("[trace] %s\n", message);
#else
    if (!log)
        return;
    log->trace(message);
    log->flush();
#endif
}

void Logger::Debug(const char* message)
{
#ifdef EMSCRIPTEN
    if (log_level > LogLevel::LEVEL_DEBUG)
        return;
    printf("[debug] %s\n", message);
#else
    if (!log)
        return;
    log->debug(message);
    log->flush();
#endif
}

void Logger::Info(const char* message)
{
#ifdef EMSCRIPTEN
    if (log_level > LogLevel::LEVEL_INFO)
        return;
    printf("[info] %s\n", message);
#else
    if (!log)
        return;
    log->info(message);
    log->flush();
#endif
}

void Logger::Warning(const char* message)
{
#ifdef EMSCRIPTEN
    if (log_level > LogLevel::LEVEL_WARNING)
        return;
    printf("[warning] %s\n", message);
#else
    if (!log)
        return;
    log->warn(message);
    log->flush();
#endif
}

void Logger::Error(const char* message)
{
#ifdef EMSCRIPTEN
    if (log_level > LogLevel::LEVEL_ERROR)
        return;
    printf("[error] %s\n", message);
#else
    if (!log)
        return;
    log->error(message);
    log->flush();
#endif
}

void Logger::Critical(const char* message)
{
#ifdef EMSCRIPTEN
    if (log_level > LogLevel::LEVEL_CRITICAL)
        return;
    printf("[critical] %s\n", message);
#else
    if (!log)
        return;
    log->critical(message);
    log->flush();
#endif
}

void Logger::SetLevel(LogLevel level)
{
#ifdef EMSCRIPTEN
    log_level = (LogLevel)level;
#else
    log->set_level((spdlog::level::level_enum)level);
#endif
}

LogLevel Logger::GetLevel()
{
#ifdef EMSCRIPTEN
    return log_level;
#else
    return (LogLevel)log->level();
#endif
}

//LoggerFormatter

LoggerFormatter::LoggerFormatter()
{
    std::unique_ptr<spdlog::details::aggregate_formatter> p_format = spdlog::details::make_unique<spdlog::details::aggregate_formatter>();
    p_format->add_ch('[');
    formatters.push_back(std::move(p_format));
    formatters.push_back(spdlog::details::make_unique<spdlog::details::level_formatter<spdlog::details::null_scoped_padder>>(spdlog::details::padding_info{}));
    p_format = spdlog::details::make_unique<spdlog::details::aggregate_formatter>();
    p_format->add_ch(']');
    p_format->add_ch(' ');
    formatters.push_back(std::move(p_format));
    formatters.push_back(spdlog::details::make_unique<spdlog::details::v_formatter<spdlog::details::null_scoped_padder>>(spdlog::details::padding_info{}));
}

void LoggerFormatter::format(const spdlog::details::log_msg &msg, spdlog::memory_buf_t &dest)
{
    std::tm t = spdlog::details::os::localtime(spdlog::log_clock::to_time_t(msg.time));

    for (auto &f : formatters)
       f->format(msg, t, dest);

    if (dest.size() > 0 && dest[dest.size() - 1] != '\r' && dest[dest.size() - 1] != '\n')
       spdlog::details::fmt_helper::append_string_view(spdlog::details::os::default_eol, dest);
}

std::unique_ptr<spdlog::formatter> LoggerFormatter::clone() const
{
    return std::unique_ptr<spdlog::formatter>(new LoggerFormatter());
}

}
