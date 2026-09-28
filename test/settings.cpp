/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "settings.h"
#include <QAction>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>
#include <QTreeWidget>
#include "../src/settings_dialog.h"

//TestSettings

void TestSettings::initTestCase()
{
}

void TestSettings::cleanupTestCase()
{
}

void TestSettings::init()
{
    QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
    window = new MainWindow();
    window->Start("");
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window));
}

void TestSettings::cleanup()
{
    delete window;
}

void TestSettings::OpenSettings(const std::function<void(SettingsDialog*)>& interact)
{
    QTimer::singleShot(0,
        [&]()
        {
            SettingsDialog* dialog = nullptr;
            for (auto w : QApplication::topLevelWidgets())
            {
                dialog = qobject_cast<SettingsDialog*>(w);
                if (dialog)
                    break;
            }

            QVERIFY(dialog);
            QTRY_VERIFY(dialog->isVisible());

            if (interact)
                interact(dialog);

            auto box = dialog->findChild<QDialogButtonBox*>("buttonBox");
            QVERIFY(box);
            auto ok = box->button(QDialogButtonBox::Ok);
            QVERIFY(ok);
            ok->click();
        });

    QAction* settings_action = nullptr;
    const auto actions = window->findChildren<QAction*>();
    for (auto* action : actions)
    {
        if (action->text() == "&Settings")
            settings_action = action;
    }

    QVERIFY(settings_action);
    settings_action->trigger();
}

void TestSettings::SetRealPrecision(SettingsDialog* dialog, uint precision)
{
    auto tree = dialog->findChild<QTreeWidget*>("settings_tree");
    QVERIFY(tree);
    auto items = tree->findItems("Result", Qt::MatchExactly | Qt::MatchRecursive);
    QVERIFY(items.size() == 1);
    emit tree->itemClicked(items.first(), 0);

    auto spin = dialog->findChild<QSpinBox*>("real_precision");
    QTRY_VERIFY(spin != nullptr);
    spin->setValue(precision);
}

//Result settings changed in the settings dialog must reach the open documents so that each new result picks them up
void TestSettings::testResultSettingsApplyToOpenDocuments()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    Config c;
    document->GetConfig(c);
    const uint original_precision = c.real_result.precision;
    const uint new_precision = (original_precision == 7 ? 4 : 7);

    OpenSettings(
        [&](SettingsDialog* dialog) -> void
        {
            SetRealPrecision(dialog, new_precision);
        });

    QTRY_VERIFY(
        [&]() -> bool
        {
            Config nc;
            document->GetConfig(nc);
            return nc.real_result.precision == new_precision && nc.auto_result.real_result.precision == new_precision;
        }());

    //restore the original value for the following tests
    OpenSettings(
        [&](SettingsDialog* dialog) -> void
        {
            SetRealPrecision(dialog, original_precision);
        });

    QTRY_VERIFY(
        [&]() -> bool
        {
            Config rc;
            document->GetConfig(rc);
            return rc.real_result.precision == original_precision;
        }());
}

//Result settings of the open documents must be kept when the dialog is accepted without changing them
void TestSettings::testResultSettingsKeptWhenNotChanged()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    //give the document its own result settings
    Config c;
    document->GetConfig(c);
    const uint own_precision = (c.real_result.precision == 5 ? 6 : 5);
    c.real_result.precision = own_precision;
    c.auto_result.real_result.precision = own_precision;
    document->SetConfig(c, false);

    QTRY_VERIFY(
        [&]() -> bool
        {
            Config nc;
            document->GetConfig(nc);
            return nc.real_result.precision == own_precision;
        }());

    OpenSettings(nullptr);

    Config nc;
    document->GetConfig(nc);
    QCOMPARE(nc.real_result.precision, own_precision);
    QCOMPARE(nc.auto_result.real_result.precision, own_precision);
}
