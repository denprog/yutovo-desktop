/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef DOCUMENTS_SETTINGS_FORM_H
#define DOCUMENTS_SETTINGS_FORM_H

#include <QWidget>
#include <QSettings>
#include <yutovo-editor/config.h>

namespace Ui
{
class DocumentsSettingsForm;
}

class DocumentsSettingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit DocumentsSettingsForm(QHash<QString, QVariant>& _settings, yutovo::Config& _config, QWidget *parent = nullptr);
    ~DocumentsSettingsForm();

private:
    Ui::DocumentsSettingsForm* form;
    yutovo::Config& config;
    QHash<QString, QVariant>& settings;
};

#endif
