/*
 * Yutovo Solver
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "service_config.h"
#include <yutovo-logger/logger.h>
#include "service_context.h"
#include "session.h"
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>

using namespace yutovo_solver;

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

using stream = websocket::stream<typename beast::tcp_stream::rebind_executor<typename asio::use_awaitable_t<>::executor_with_default<asio::any_io_executor>>::other>;

int main(int argc, char *argv[])
{
    char* p = std::getenv("YUTOVO_DEPLOY");
    std::string s(p == nullptr ? "./log" : std::string(p) + "/log/yutovo-solver/");
    Logger* logger = Logger::GetInstance(s, "solver", true, true);

    logger->Info("Yutovo solver start");

    ServiceConfig config(logger);
    config.Read();

    asio::io_context io_context{config.threads_count};
    RemoteServiceContext service_context(io_context, &config);

    auto const address = asio::ip::make_address("0.0.0.0");

    try
    {
        std::make_shared<Listener>(&service_context, tcp::endpoint{address, config.port}, logger)->Run();
    }
    catch (boost::system::system_error& ec)
    {
        logger->Error("Error in RemoteSession: {}", ec.code().value());
        return 1;
    }
    catch (std::exception& e)
    {
        logger->Error("Error in Listener: {}", e.what());
        return 1;
    }

    //run the I/O service on the requested number of threads
    std::vector<std::thread> v;
    v.reserve(config.threads_count - 1);
    for (int i = config.threads_count - 1; i > 0; --i)
    {
        v.emplace_back(
            [&io_context]
            {
                io_context.run();
            });
    }
    io_context.run();

    logger->Info("Yutovo service finish");
    return 0;   
}
