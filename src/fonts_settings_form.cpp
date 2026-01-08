/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "fonts_settings_form.h"
#include "ui_fonts_settings_form.h"

//FontsSettingsForm

FontsSettingsForm::FontsSettingsForm(yutovo::Config& _config, QWidget *parent) :
    QWidget(parent),
    form(new Ui::FontsSettingsForm),
    config(_config)
{
    form->setupUi(this);

    form->use_numbers_gaps->setChecked(config.use_numbers_gaps);
    form->binary_gap->setValue(config.binary_gap);
    form->octal_gap->setValue(config.decimal_gap);
    form->decimal_gap->setValue(config.decimal_gap);
    form->hexadecimal_gap->setValue(config.hexadecimal_gap);
}

FontsSettingsForm::~FontsSettingsForm()
{
    config.use_numbers_gaps = form->use_numbers_gaps->isChecked();
    config.binary_gap = form->binary_gap->value();
    config.octal_gap = form->octal_gap->value();
    config.decimal_gap = form->decimal_gap->value();
    config.hexadecimal_gap = form->hexadecimal_gap->value();

    delete form;
}
