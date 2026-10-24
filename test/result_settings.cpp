/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "result_settings.h"

//At least one result type must stay enabled in the order list
void TestResultSettings::testOrderMinimumOneEnabled()
{
    Config config;
    ResultSettingsForm form(config);
    QListWidget* list = form.findChild<QListWidget*>("auto_result_order");
    QVERIFY(list != nullptr);
    QCOMPARE(list->count(), 8);

    //uncheck all items one by one, the last one must revert to checked
    for (int i = 0; i < list->count(); ++i)
        list->item(i)->setCheckState(Qt::Unchecked);

    int enabled_count = 0;
    for (size_t i = 0; i < sizeof(config.auto_result.results_enabled) / sizeof(config.auto_result.results_enabled[0]); ++i)
    {
        if (config.auto_result.results_enabled[i])
            ++enabled_count;
    }
    QCOMPARE(enabled_count, 1);

    int checked_count = 0;
    for (int i = 0; i < list->count(); ++i)
    {
        if (list->item(i)->checkState() == Qt::Checked)
            ++checked_count;
    }
    QCOMPARE(checked_count, 1);

    //the reverted checkbox still toggles
    list->item(0)->setCheckState(Qt::Checked);
    QVERIFY(config.auto_result.results_enabled[0]);
    list->item(0)->setCheckState(Qt::Unchecked);
    QVERIFY(!config.auto_result.results_enabled[0]);
}

//Up and Down buttons move the enabled flags together with the result types
void TestResultSettings::testOrderUpDownMovesEnabledFlags()
{
    Config config;
    ResultSettingsForm form(config);
    QListWidget* list = form.findChild<QListWidget*>("auto_result_order");
    QPushButton* down = form.findChild<QPushButton*>("down_result_order");
    QPushButton* up = form.findChild<QPushButton*>("up_result_order");
    QVERIFY(list != nullptr);
    QVERIFY(down != nullptr);
    QVERIFY(up != nullptr);

    //disable the first type and swap the first two positions
    list->item(0)->setCheckState(Qt::Unchecked);
    ResultType type0 = config.auto_result.results_order[0];
    ResultType type1 = config.auto_result.results_order[1];

    list->setCurrentRow(0);
    down->click();
    QVERIFY(config.auto_result.results_order[0] == type1);
    QVERIFY(config.auto_result.results_order[1] == type0);
    QVERIFY(config.auto_result.results_enabled[0] == true);
    QVERIFY(config.auto_result.results_enabled[1] == false);
    QVERIFY(list->item(0)->checkState() == Qt::Checked);
    QVERIFY(list->item(1)->checkState() == Qt::Unchecked);

    list->setCurrentRow(1);
    up->click();
    QVERIFY(config.auto_result.results_order[0] == type0);
    QVERIFY(config.auto_result.results_order[1] == type1);
    QVERIFY(config.auto_result.results_enabled[0] == false);
    QVERIFY(config.auto_result.results_enabled[1] == true);
    QVERIFY(list->item(0)->checkState() == Qt::Unchecked);
    QVERIFY(list->item(1)->checkState() == Qt::Checked);
}

//Rows below the fifth can move down (the guard used to block rows 5-7)
void TestResultSettings::testOrderLastItemMovesDown()
{
    Config config;
    ResultSettingsForm form(config);
    QListWidget* list = form.findChild<QListWidget*>("auto_result_order");
    QPushButton* down = form.findChild<QPushButton*>("down_result_order");
    QVERIFY(list != nullptr);
    QVERIFY(down != nullptr);

    ResultType type6 = config.auto_result.results_order[6];
    ResultType type7 = config.auto_result.results_order[7];

    list->setCurrentRow(7);
    down->click();
    QVERIFY(config.auto_result.results_order[6] == type6);
    QVERIFY(config.auto_result.results_order[7] == type7);

    list->setCurrentRow(6);
    down->click();
    QVERIFY(config.auto_result.results_order[6] == type7);
    QVERIFY(config.auto_result.results_order[7] == type6);
}
