/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <QApplication>
#include <QFileInfo>
#include "mainwindow.h"
#include <QLocalServer>
#include <QLocalSocket>
#include <QDebug>
#ifdef _WIN32
#include <boost/throw_exception.hpp>

namespace boost
{
    BOOST_NORETURN void throw_exception(std::exception const& e)
    {
        throw;
    }

    BOOST_NORETURN void throw_exception(std::exception const& e, boost::source_location const&)
    {
        throw;
    }
}
#endif

bool SendToRunningInstance(const QString& socket_name, const QString& path)
{
    QLocalSocket socket;
    socket.connectToServer(socket_name);
    if (!socket.waitForConnected(100))
        return false;
    socket.write(path.toUtf8());
    socket.flush();
    socket.waitForBytesWritten();
    return true;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("Yutovo");
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    QString socket_name = "yutovo_instance";
    if (SendToRunningInstance(socket_name, app.arguments().value(1)))
        return 0; //an instance of the app is already running

    MainWindow w;

    QLocalServer s;
    s.removeServer(socket_name);
    s.listen(socket_name);
    QObject::connect(&s, &QLocalServer::newConnection, 
        [&]()
        {
            QLocalSocket* c = s.nextPendingConnection();
            c->waitForReadyRead();
            QString file = QString::fromUtf8(c->readAll());
            w.OpenFile(file);
            c->disconnectFromServer();
        });

    QString filename;
    QStringList args = QCoreApplication::arguments();
    if (args.size() > 1)
    {
        QFileInfo fi(args.at(1));
        if (fi.exists() && fi.isFile())
            filename = args.at(1);
    }

    try
    {
        w.Start(filename);
    }
    catch (std::exception& ex)
    {
        printf("Error starting: %s\n", ex.what());
        return 1;
    }
    w.show();
    return app.exec();
}
