/*
 * Yutovo Desktop
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __COLORS_SETTINGS_FORM_H__
#define __COLORS_SETTINGS_FORM_H__

#include <QWidget>
#include <QSettings>
#include <yutovo-editor/config.h>

namespace Ui
{
class ColorsSettingsForm;
}

class ColorsSettingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit ColorsSettingsForm(yutovo::Config& _config, QWidget *parent = nullptr);
    ~ColorsSettingsForm();

private slots:
    void OnCodeBlockBorderColorClicked();
    void OnNumbersColorClicked();
    void OnFunctionsColorClicked();
    void OnVariablesColorClicked();
    void OnUnitsColorClicked();
    void OnShapesColorClicked();
    void OnErrorsColorClicked();
    void OnFormulaBgColorClicked();
    void OnSelectionBgColorClicked();

private:
    Ui::ColorsSettingsForm *form;
    yutovo::Config& config;
};

#endif
