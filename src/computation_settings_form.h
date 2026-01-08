/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __SOLVING_SETTINGS_FORM_H__
#define __SOLVING_SETTINGS_FORM_H__

#include <QWidget>
#include <QSettings>
#include <yutovo-editor/config.h>

namespace Ui
{
class ComputationSettingsForm;
}

class ComputationSettingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit ComputationSettingsForm(yutovo::Config& _config, QWidget *parent = nullptr);
    ~ComputationSettingsForm();

private slots:

private:
    Ui::ComputationSettingsForm *form;
    yutovo::Config& config;
};

#endif
