/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <QtTest/QtTest>
#include <QApplication>
#include <functional>
#include "../src/mainwindow.h"

class SettingsDialog;

class TestSettings : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testResultSettingsApplyToOpenDocuments();
    void testResultSettingsKeptWhenNotChanged();

private:
    MainWindow* window;

    void OpenSettings(const std::function<void(SettingsDialog*)>& interact);
    void SetRealPrecision(SettingsDialog* dialog, uint precision);
};
