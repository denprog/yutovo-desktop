/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __WEB_SOCKET_H__
#define __WEB_SOCKET_H__

#include "config.h"
#include "result_codes.h"

#ifndef REMOTE_SOLVER
#include <yutovo-solver/service_config.h>
#include <yutovo-solver/service_solver.h>
#include <yutovo-solver/service_context.h>
#include <yutovo-solver/session.h>
#endif

#ifndef EMSCRIPTEN
#ifdef REMOTE_SOLVER
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/buffers_iterator.hpp>
#endif
#include <fstream>
#endif

#ifdef REMOTE_SOLVER
#ifndef EMSCRIPTEN
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;
#endif
#endif

namespace yutovo
{

class Window;
class Logger;

class WebSocket : public std::enable_shared_from_this<WebSocket>
{
public:
#ifdef REMOTE_SOLVER
    WebSocket(Config& _config, Window* _window);
#else
    WebSocket(Config& _config, Window* _window, yutovo_solver::Session& _session);
#endif
    ~WebSocket();

    bool Connect();
    bool Send(const std::string& message, Result& result);
    bool Receive(std::string& message, Result& result);
    bool Reset();
    bool IsOpen();
    void Close();

private:
#ifdef REMOTE_SOLVER
#ifndef EMSCRIPTEN
    void OnConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);
    void OnSslHandshake(beast::error_code ec);
    void OnHandshake(beast::error_code ec);
    void OnWrite(beast::error_code ec, std::size_t bytes_transferred);
    void OnRead(beast::error_code ec, std::size_t bytes_transferred);
#endif
#endif

private:
    Config& config;
    Window* window;

    Logger* logger;

#ifdef REMOTE_SOLVER
#ifdef EMSCRIPTEN
    int socket_id = 0;
#else
    struct SslContext
    {
        SslContext(Logger* logger);

        ssl::context ssl_context{ssl::context::tlsv12_client};
    };

    asio::io_context ioc;
    SslContext ssl_context;
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws;

    bool connected = false;
    bool connection = false;
    bool writing = false, reading = false;
    beast::error_code last_error;
#endif

    std::string host, port;
#else
    yutovo_solver::Session& session;
    std::string reply;
#endif

    bool exit = false;
};

typedef std::shared_ptr<WebSocket> WebSocketPtr;

}

#endif
