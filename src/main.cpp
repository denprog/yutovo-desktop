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
#include <QDir>
#include <QFontDatabase>

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

QString FileNameFromArguments(const QStringList& args)
{
    for (int i = 1; i < args.size(); i++)
    {
        const QString& arg = args.at(i);
        if (arg.isEmpty() || arg.startsWith("--"))
            continue;
        QFileInfo fi(arg);
        if (fi.exists() && fi.isFile())
            return arg;
    }

    return QString();
}

#ifdef _WIN32
void RegisterApplicationFonts()
{
    QDir fonts_dir(QCoreApplication::applicationDirPath() + "/fonts");
    if (!fonts_dir.exists())
        return;

    QStringList filters;
    filters << "*.ttf" << "*.TTF" << "*.otf" << "*.OTF";
    for (const QString& file : fonts_dir.entryList(filters, QDir::Files))
    {
        QString path = fonts_dir.absoluteFilePath(file);
        if (QFontDatabase::addApplicationFont(path) == -1)
            qWarning() << "Failed to load font:" << path;
    }
}
#endif

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("Yutovo");
    app.setApplicationName("Yutovo Desktop");
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    QString socket_name = "yutovo_instance";
    if (SendToRunningInstance(socket_name, FileNameFromArguments(app.arguments())))
        return 0; //an instance of the app is already running

    MainWindow w;

#ifdef _WIN32
    RegisterApplicationFonts();
#endif

    QLocalServer s;
    s.removeServer(socket_name);
    s.listen(socket_name);
    QObject::connect(&s, &QLocalServer::newConnection,
        [&]()
        {
            QLocalSocket* c = s.nextPendingConnection();
            if (c == nullptr)
                return;
            QObject::connect(c, &QLocalSocket::disconnected,
                [&w, c]()
                {
                    QString file = QString::fromUtf8(c->readAll());
                    if (!file.isEmpty() && !file.startsWith("--"))
                        w.OpenFile(file);
                    c->deleteLater();
                });
        });

    QString filename = FileNameFromArguments(QCoreApplication::arguments());

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
