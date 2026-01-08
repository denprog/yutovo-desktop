/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef INTERFACE_SETTINGS_FORM_H
#define INTERFACE_SETTINGS_FORM_H

#include <QWidget>
#include <QSettings>

namespace Ui
{
class InterfaceSettingsForm;
}

class InterfaceSettingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit InterfaceSettingsForm(QHash<QString, QVariant>& _settings, QWidget *parent = nullptr);
    ~InterfaceSettingsForm();

private:
    Ui::InterfaceSettingsForm *form;
    QHash<QString, QVariant>& settings;
};

#endif
