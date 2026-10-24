/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <QtTest/QtTest>
#include <QApplication>
#include <QListWidget>
#include <QPushButton>
#include "../src/result_settings_form.h"

class TestResultSettings : public QObject
{
    Q_OBJECT

private slots:
    void testOrderMinimumOneEnabled();
    void testOrderUpDownMovesEnabledFlags();
    void testOrderLastItemMovesDown();
};
