/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "system_settings_form.h"
#include "ui_system_settings_form.h"

//SystemSettingsForm

SystemSettingsForm::SystemSettingsForm(yutovo::Config& _config, QWidget* parent) :
    QWidget(parent),
    form(new Ui::SystemSettingsForm()),
    config(_config)
{
    form->setupUi(this);

    form->language->setCurrentIndex((int)config.language - 1);
    form->log_level->setCurrentIndex((int)config.log_level);
}

SystemSettingsForm::~SystemSettingsForm()
{
    config.language = (yutovo_calculator::Language)(form->language->currentIndex() + 1);
    config.log_level = (yutovo::LogLevel)form->log_level->currentIndex();
    
    delete form;
}
