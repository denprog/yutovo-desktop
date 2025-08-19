/*
 * Yutovo Desktop
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef RESULT_SETTINGS_FORM_H
#define RESULT_SETTINGS_FORM_H

#include <QWidget>
#include <yutovo-editor/config.h>

namespace Ui
{
class ResultSettingsForm;
}

using namespace yutovo;

class ResultSettingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit ResultSettingsForm(yutovo::Config& _config, QWidget *parent = nullptr);
    ~ResultSettingsForm();

public slots:
    void OnUpResultOrderClicked();
    void OnDownResultOrderClicked();

private:
    void FillResultsOrder();

private:
    Ui::ResultSettingsForm *ui;
    yutovo::Config& config;
};

#endif
