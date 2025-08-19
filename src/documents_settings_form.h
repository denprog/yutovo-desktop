/*
 * Yutovo Desktop
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef DOCUMENTS_SETTINGS_FORM_H
#define DOCUMENTS_SETTINGS_FORM_H

#include <QWidget>
#include <QSettings>

namespace Ui
{
class DocumentsSettingsForm;
}

class DocumentsSettingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit DocumentsSettingsForm(QHash<QString, QVariant>& _settings, QWidget *parent = nullptr);
    ~DocumentsSettingsForm();

private:
    Ui::DocumentsSettingsForm* form;
    QHash<QString, QVariant>& settings;
};

#endif
