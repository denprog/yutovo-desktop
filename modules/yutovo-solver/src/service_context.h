/*
 * Yutovo Solver
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __SERVICE_CONTEXT_H__
#define __SERVICE_CONTEXT_H__

#ifdef REMOTE_MODE
#include <boost/beast/ssl.hpp>
#endif

#include <fstream>
#include "service_config.h"
#include "service_solver.h"

#ifdef REMOTE_MODE
namespace ssl = boost::asio::ssl;
namespace asio = boost::asio;
#endif

namespace yutovo_solver
{

struct ServiceContext
{
    ServiceContext(ServiceConfig* service_config, const std::string& _logs_path, bool _log_console, bool _log_file) :
        solvers(service_config, _logs_path, _log_console, _log_file)
    {
    }

    Solvers solvers;
    bool exit = false;
};

#ifdef REMOTE_MODE
struct RemoteServiceContext : ServiceContext
{
    RemoteServiceContext(asio::io_context& _io_context, ServiceConfig* config) :
        ServiceContext(config),
        io_context(_io_context)
    {
        ssl_context.use_certificate_chain_file("yutovo-solver.crt");
        ssl_context.use_private_key_file("yutovo-solver.key", ssl::context_base::file_format::pem);
        ssl_context.set_default_verify_paths();
        ssl_context.set_options(boost::asio::ssl::context::default_workarounds |
            boost::asio::ssl::context::no_compression |
            boost::asio::ssl::context::no_sslv2 |
            boost::asio::ssl::context::no_sslv3 |
            boost::asio::ssl::context::no_tlsv1
        );        
    }

    asio::io_context& io_context;
    ssl::context ssl_context{ssl::context::tlsv12};
};
#endif

}

#endif
