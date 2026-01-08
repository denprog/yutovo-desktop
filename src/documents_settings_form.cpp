/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "documents_settings_form.h"
#include "ui_documents_settings_form.h"

//DocumentsSettingsForm

DocumentsSettingsForm::DocumentsSettingsForm(QHash<QString, QVariant>& _settings, yutovo::Config& _config, QWidget *parent) :
    QWidget(parent),
    form(new Ui::DocumentsSettingsForm),
    config(_config),
    settings(_settings)
{
    form->setupUi(this);

    form->load_last_documents->setChecked(settings.value("Documents/load_last_documents", false).toBool());
    form->click_link->setCurrentIndex(settings.value("Documents/click_link", 0).toInt());
    form->undo_size->setValue(settings.value("Documents/undo_size", 100).toInt());
}

DocumentsSettingsForm::~DocumentsSettingsForm()
{
    settings["Documents/load_last_documents"] = form->load_last_documents->isChecked();
    settings["Documents/click_link"] = form->click_link->currentIndex();
    settings["Documents/undo_size"] = form->undo_size->value();
    config.undo_size = form->undo_size->value();

    delete form;
}
