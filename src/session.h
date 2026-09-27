/*
 * Yutovo Solver
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __SESSION_H__
#define __SESSION_H__

#include "service_config.h"

#ifdef REMOTE_MODE
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/dispatch.hpp>
#endif

#include "service_context.h"
#include <rapidjson/document.h>
#include "types.h"
#include <yutovo-logger/logger.h>

#ifdef REMOTE_MODE
using tcp = boost::asio::ip::tcp;
namespace asio = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace http = beast::http;
namespace ssl = boost::asio::ssl;
#endif

namespace yutovo_solver
{

struct ServiceContext;

using namespace yutovo;

class Session
{
public:
    Session(ServiceContext* _service_context, const std::string& _logs_path, bool _log_console, bool _log_file);

    void Parse(const std::string& json, std::string& reply);
    void SetMaxTime(const uint64_t max_time);

protected:
    void ParseCommand(const rapidjson::Document& request_json, std::string& reply);
    void MakeError(const ErrorCode error_code, std::string& reply);
    void MakeOk(std::string& reply);
    void MakeReply(const rapidjson::Document& json, std::string& reply);

protected:
    static int sessions_count;

    Logger* logger = nullptr;

private:
    ServiceContext* service_context;
};

#ifdef REMOTE_MODE
class RemoteSession : public Session, public std::enable_shared_from_this<RemoteSession>
{
public:
    RemoteSession(tcp::socket&& socket, RemoteServiceContext* _service_context, Logger* _logger);
    ~RemoteSession();

    void Run();

private:
    void OnRun();
    void OnHandshake(beast::error_code ec);
    void OnAccept(beast::error_code ec);
    void DoRead();
    void OnRead(beast::error_code ec, std::size_t bytes_transferred);
    void OnWrite(beast::error_code ec, std::size_t bytes_transferred);

private:
    RemoteServiceContext* service_context;

    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws;
    beast::flat_buffer buffer;
    std::string reply;
};

class Listener : public std::enable_shared_from_this<Listener>
{
public:
    Listener(RemoteServiceContext* _service_context, tcp::endpoint end_point, Logger* _logger);

    void Run();

private:
    void DoAccept();
    void OnAccept(beast::error_code ec, tcp::socket socket);

private:
    RemoteServiceContext* service_context;
    tcp::acceptor acceptor;
    Logger* logger;
};
#endif

}

#endif
