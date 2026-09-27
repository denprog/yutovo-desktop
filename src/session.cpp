/*
 * Yutovo Solver
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "session.h"
#include "service_solver.h"
#include "service_context.h"
#include <rapidjson/writer.h>
#include <memory>

namespace yutovo_solver
{

//Session

int Session::sessions_count = 0;

Session::Session(ServiceContext* _service_context, const std::string& _logs_path, bool _log_console, bool _log_file) :
    service_context(_service_context),
    logger(Logger::GetInstance(_logs_path + "/yutovo-solver", "yutovo-solver", _log_console, _log_file))
{
}

void Session::Parse(const std::string& json, std::string& reply)
{
    rapidjson::Document request_json;
    request_json.Parse<0>(json.c_str());
    if (request_json.HasParseError())
    {
        MakeError(ErrorCode::JSON_ERROR, reply);
        return;
    }

    //the session must not leak exceptions into the caller thread, the editor message loop has no handler
    try
    {
        ParseCommand(request_json, reply);
    }
    catch (ServiceException& ex)
    {
        MakeError(ex.error_code, reply);
    }
    catch (...)
    {
        MakeError(ErrorCode::SOLVER_ERROR, reply);
    }
}

void Session::ParseCommand(const rapidjson::Document& request_json, std::string& reply)
{
    if (!request_json.HasMember("command") || !request_json["command"].IsString())
    {
        MakeError(ErrorCode::NO_FIELD_ERROR, reply);
        return;
    }

    std::string command = request_json["command"].GetString();
    SolverPtr solver;
    std::string document_guid, solver_guid;

    if (command == "EXIT")
    {
        service_context->exit = true;
        MakeOk(reply);
        return;
    }
    else if (command == "REMOVE_SOLVER")
    {
        if (!request_json.HasMember("solver_guid") || !request_json["solver_guid"].IsString())
        {
            MakeError(ErrorCode::NO_FIELD_ERROR, reply);
            return;
        }
        solver_guid = request_json["solver_guid"].GetString();

        if (!request_json.HasMember("code_id") || !request_json["code_id"].IsInt())
        {
            MakeError(ErrorCode::NO_FIELD_ERROR, reply);
            return;
        }
        int code_id = request_json["code_id"].GetInt();
        if (!service_context->solvers.RemoveSolver(solver_guid, code_id))
            MakeError(ErrorCode::SOLVER_ERROR, reply);
        else
            MakeOk(reply);
        return;
    }
    else if (command == "SOLVE_CODE" || command == "REMOVE_IDENTIFIER" || command == "LIST_IDENTIFIERS" || command == "LIST_USER_IDENTIFIERS" || 
        command == "BREAK_SOLVING")
    {
        if (!request_json.HasMember("document_guid") || !request_json["document_guid"].IsString())
        {
            MakeError(ErrorCode::NO_FIELD_ERROR, reply);
            return;
        }
        document_guid = request_json["document_guid"].GetString();

        if (!request_json.HasMember("solver_guid") || !request_json["solver_guid"].IsString())
        {
            MakeError(ErrorCode::NO_FIELD_ERROR, reply);
            return;
        }
        solver_guid = request_json["solver_guid"].GetString();

        if (!request_json.HasMember("code_id") || !request_json["code_id"].IsInt())
        {
            MakeError(ErrorCode::NO_FIELD_ERROR, reply);
            return;
        }
        int code_id = request_json["code_id"].GetInt();

        if (!request_json.HasMember("solver_type") || !request_json["solver_type"].IsInt())
        {
            MakeError(ErrorCode::NO_FIELD_ERROR, reply);
            return;
        }
        SolverType solver_type = (SolverType)request_json["solver_type"].GetInt();

        solver = service_context->solvers.GetSolver(document_guid, solver_guid, code_id, solver_type);
        if (!solver)
        {
            MakeError(ErrorCode::SOLVER_ERROR, reply);
            return;
        }
    }

    rapidjson::Document response_json;
    response_json.SetObject();

    if (command == "SOLVE_CODE")
    {
        solver->Solve(request_json, response_json);
        MakeReply(response_json, reply);
        return;
    }

    if (command == "REMOVE_IDENTIFIER")
    {
        solver->RemoveIdentifier(request_json, response_json);
        MakeReply(response_json, reply);
        return;
    }

    if (command == "REMOVE_USER_IDENTIFIERS")
    {
        if (!request_json.HasMember("solver_guid") || !request_json["solver_guid"].IsString())
        {
            MakeError(ErrorCode::NO_FIELD_ERROR, reply);
            return;
        }
        solver_guid = request_json["solver_guid"].GetString();

        service_context->solvers.RemoveUserIdentifiers(solver_guid, request_json, response_json);
        MakeReply(response_json, reply);
        return;
    }

    if (command == "LIST_IDENTIFIERS")
    {
        solver->ListIdentifiers(request_json, response_json);
        MakeReply(response_json, reply);
        return;
    }

    if (command == "CLEAR_EXPORT")
    {
        if (!request_json.HasMember("document_guid") || !request_json["document_guid"].IsString())
        {
            MakeError(ErrorCode::NO_FIELD_ERROR, reply);
            return;
        }
        document_guid = request_json["document_guid"].GetString();

        service_context->solvers.ClearExport(document_guid, request_json, response_json);
        MakeReply(response_json, reply);
        return;
    }

    if (command == "BREAK_SOLVING")
    {
        solver->BreakSolving(request_json, response_json);
        MakeReply(response_json, reply);
        return;
    }

    if (command == "SET_LOCALE")
    {
        if (!request_json.HasMember("solver_guid") || !request_json["solver_guid"].IsString())
        {
            MakeError(ErrorCode::NO_FIELD_ERROR, reply);
            return;
        }
        std::string solver_guid = request_json["solver_guid"].GetString();

        if (!request_json.HasMember("language") || !request_json["language"].IsInt())
        {
            logger->Error("language error");
            MakeError(ErrorCode::NO_FIELD_ERROR, reply);
            return;
        }
        Language language = (Language)request_json["language"].GetInt();

        service_context->solvers.SetLocale(solver_guid, language, request_json, response_json);
        MakeReply(response_json, reply);
        return;
    }

    MakeError(ErrorCode::UNKNOWN_COMMAND, reply);
}

void Session::SetMaxTime(const uint64_t max_time)
{
    service_context->solvers.SetMaxTime(max_time);
}

void Session::MakeError(const ErrorCode error_code, std::string& reply)
{
    rapidjson::Document response_json;
    auto& alloc = response_json.GetAllocator();
    response_json.SetObject();
    response_json.AddMember("error", (int)error_code, alloc);
    MakeReply(response_json, reply);
}

void Session::MakeOk(std::string& reply)
{
    rapidjson::Document response_json;
    auto& alloc = response_json.GetAllocator();
    response_json.SetObject();
    response_json.AddMember("result", (int)ErrorCode::OK, alloc);
    MakeReply(response_json, reply);
}

void Session::MakeReply(const rapidjson::Document& json, std::string& reply)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);
    reply = buffer.GetString();
}

#ifdef REMOTE_MODE

//RemoteSession

RemoteSession::RemoteSession(tcp::socket&& socket, RemoteServiceContext* _service_context, Logger* _logger) :
    Session(_service_context, _logger),
    service_context(_service_context),
    ws(std::move(socket), _service_context->ssl_context)
{
    logger->Info("Sessions count: {}", ++sessions_count);
}

RemoteSession::~RemoteSession()
{
    logger->Info("Sessions count: {}", --sessions_count);
}

void RemoteSession::Run()
{
    asio::dispatch(ws.get_executor(), beast::bind_front_handler(&RemoteSession::OnRun, shared_from_this()));
}

void RemoteSession::OnRun()
{
    beast::get_lowest_layer(ws).expires_after(std::chrono::seconds(30));
    ws.next_layer().async_handshake(ssl::stream_base::server, beast::bind_front_handler(&RemoteSession::OnHandshake, shared_from_this()));
}

void RemoteSession::OnHandshake(beast::error_code ec)
{
    if (ec)
    {
        logger->Error("OnHandshake error: {}", ec.message());
        return;
    }

    beast::get_lowest_layer(ws).expires_never();
    ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));
    ws.set_option(websocket::stream_base::decorator(
        [](websocket::response_type& res)
        {
            res.set(http::field::server, "Yutovo service");
        }));

    ws.async_accept(beast::bind_front_handler(&RemoteSession::OnAccept, shared_from_this()));
}

void RemoteSession::OnAccept(beast::error_code ec)
{
    if (ec)
    {
        logger->Error("OnAccept error: {}", ec.message());
        return;
    }

    DoRead();
}

void RemoteSession::DoRead()
{
    ws.async_read(buffer, beast::bind_front_handler(&RemoteSession::OnRead, shared_from_this()));
}

void RemoteSession::OnRead(beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec == websocket::error::closed)
        return;

    if (ec)
    {
        logger->Error("OnRead error: {}", ec.message());
        return;
    }

    if (!ws.got_text())
        return;

    std::string json = beast::buffers_to_string(buffer.data());
    logger->Info("Request received:\n{}", json);

    buffer.clear();

    Parse(json, reply);

    ws.text(ws.got_text());

    ws.async_write(boost::asio::buffer(reply), beast::bind_front_handler(&RemoteSession::OnWrite, shared_from_this()));
}

void RemoteSession::OnWrite(beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec)
    {
        logger->Error("OnWrite error: {}", ec.message());
        return;
    }

    logger->Info("Reply sent:\n{}", reply);

    buffer.consume(buffer.size());

    DoRead();
}

//Listener

Listener::Listener(RemoteServiceContext* _service_context, tcp::endpoint end_point, Logger* _logger) :
    service_context(_service_context),
    acceptor(asio::make_strand(service_context->io_context)),
    logger(_logger)
{
    beast::error_code ec;

    acceptor.open(end_point.protocol(), ec);
    if (ec)
        throw boost::system::system_error(ec);

    acceptor.set_option(asio::socket_base::reuse_address(true), ec);
    if (ec)
        throw boost::system::system_error(ec);

    acceptor.bind(end_point, ec);
    if (ec)
        throw boost::system::system_error(ec);

    acceptor.listen(asio::socket_base::max_listen_connections, ec);
    if (ec)
        throw boost::system::system_error(ec);
}

void Listener::Run()
{
    DoAccept();
}

void Listener::DoAccept()
{
    acceptor.async_accept(asio::make_strand(service_context->io_context), beast::bind_front_handler(&Listener::OnAccept, shared_from_this()));
}

void Listener::OnAccept(beast::error_code ec, tcp::socket socket)
{
    if (ec)
    {
        logger->Error("OnAccept error: {}", ec.value());
    }
    else
    {
        try
        {
            std::make_shared<RemoteSession>(std::move(socket), service_context, logger)->Run();
        }
        catch (const boost::system::system_error& ec)
        {
            logger->Error("Error in RemoteSession: {}", ec.code().value());
        }
        catch (const std::exception& e)
        {
            logger->Error("Error in RemoteSession: {}", e.what());
        }
    }

    DoAccept();
}
#endif

}
