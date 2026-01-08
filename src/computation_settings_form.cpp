/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "computation_settings_form.h"
#include "ui_computation_settings_form.h"

//ComputationSettingsForm

ComputationSettingsForm::ComputationSettingsForm(yutovo::Config& _config, QWidget *parent) :
    QWidget(parent),
    form(new Ui::ComputationSettingsForm()),
    config(_config)
{
    form->setupUi(this);

    form->solve_delay->setValue(config.solve_delay / 1000);
    form->solve_timeout->setValue(config.service_timeout / 1000);
}

ComputationSettingsForm::~ComputationSettingsForm()
{
    config.solve_delay = form->solve_delay->value() * 1000;
    config.service_timeout = form->solve_timeout->value() * 1000;
    
    delete form;
}
