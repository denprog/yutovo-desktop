/*
 * Yutovo Desktop
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "interface_settings_form.h"
#include "ui_interface_settings_form.h"

//InterfaceSettingsForm

InterfaceSettingsForm::InterfaceSettingsForm(QHash<QString, QVariant>& _settings, QWidget *parent) :
    QWidget(parent),
    form(new Ui::InterfaceSettingsForm),
    settings(_settings)
{
    form->setupUi(this);

    form->standard_toolbar->setChecked(settings.value("MainWindow/standard_toolbar", false).toBool());
    form->format_toolbar->setChecked(settings.value("MainWindow/format_toolbar", false).toBool());
    form->algebra_toolbar->setChecked(settings.value("MainWindow/algebra_toolbar", false).toBool());
    form->trigonometry_toolbar->setChecked(settings.value("MainWindow/trigonometry_toolbar", false).toBool());
    form->hyperbolic_toolbar->setChecked(settings.value("MainWindow/hyperbolic_toolbar", false).toBool());
    form->functions_toolbar->setChecked(settings.value("MainWindow/functions_toolbar", false).toBool());
    form->graphs_toolbar->setChecked(settings.value("MainWindow/graphs_toolbar", false).toBool());
}

InterfaceSettingsForm::~InterfaceSettingsForm()
{
    settings["MainWindow/standard_toolbar"] = form->standard_toolbar->isChecked();
    settings["MainWindow/format_toolbar"] = form->format_toolbar->isChecked();
    settings["MainWindow/algebra_toolbar"] = form->algebra_toolbar->isChecked();
    settings["MainWindow/trigonometry_toolbar"] = form->trigonometry_toolbar->isChecked();
    settings["MainWindow/hyperbolic_toolbar"] = form->hyperbolic_toolbar->isChecked();
    settings["MainWindow/functions_toolbar"] = form->functions_toolbar->isChecked();
    settings["MainWindow/graphs_toolbar"] = form->graphs_toolbar->isChecked();

    delete form;
}
