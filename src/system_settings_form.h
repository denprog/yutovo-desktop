/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __SYSTEM_SETTINGS_FORM_H__
#define __SYSTEM_SETTINGS_FORM_H__

#include <QWidget>
#include <QHash>
#include <QString>
#include <QVariant>
#include <yutovo-editor/config.h>

namespace Ui
{
class SystemSettingsForm;
};

class SystemSettingsForm : public QWidget
{
public:
    SystemSettingsForm(yutovo::Config& _config, QHash<QString, QVariant>& _settings, QWidget* parent = nullptr);
    ~SystemSettingsForm();

private:
    Ui::SystemSettingsForm* form = nullptr;
    yutovo::Config& config;
    QHash<QString, QVariant>& settings;
};

#endif
