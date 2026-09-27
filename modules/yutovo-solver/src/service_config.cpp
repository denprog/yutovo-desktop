/*
 * Yutovo Solver
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "service_config.h"
#include <yutovo-logger/logger.h>
#include <boost/property_tree/ini_parser.hpp>

namespace yutovo_solver
{

//ServiceConfig

ServiceConfig::ServiceConfig(yutovo::Logger* _logger) :
    logger(_logger)
{
}

bool ServiceConfig::Read()
{
    boost::property_tree::ptree pt;
    try
    {
        boost::property_tree::ini_parser::read_ini(file_name, pt);
    }
    catch (boost::property_tree::ptree_error& ex)
    {
        logger->Info("Error reading {}: {}", file_name, ex.what());
        return false;
    }

    try
    {
        port = pt.get<unsigned short>("Main.port", 8010);
        threads_count = pt.get<int>("Main.threads_count", 2);
        wss = pt.get<bool>("Main.wss", false);
        proxy_idle_timeout = pt.get<int>("Proxy.idle_timeout", 10);
        solver_idle_timeout = pt.get<int>("Solver.idle_timeout", 20);
        max_time = pt.get<uint64_t>("Solver.max_time", 10000);
    }
    catch (boost::property_tree::ptree_error& ex)
    {
        logger->Info("Error reading config: {}", ex.what());
        return false;
    }

    return true;
}

}
