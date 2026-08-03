/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "update_available_dialog.h"
#include "ui_update_available_dialog.h"

//UpdateAvailableDialog

UpdateAvailableDialog::UpdateAvailableDialog(const QString& version, const QString& url, QWidget* parent) :
    QDialog(parent),
    form(new Ui::UpdateAvailableDialog())
{
    form->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    setModal(true);
    setWindowTitle(tr("Update Available"));

    form->message_label->setText(tr("A new version %1 is available.").arg(version));
    form->link_label->setText(tr("Download: <a href=\"%1\">%1</a>").arg(url));
}

UpdateAvailableDialog::~UpdateAvailableDialog()
{
    delete form;
}
