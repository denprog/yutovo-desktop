/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "about_dialog.h"
#include "ui_about.h"

//AboutDialog

AboutDialog::AboutDialog() :
    form(new Ui::AboutDialog())
{
    form->setupUi(this);

    setFixedSize(width(), height());
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    setWindowIcon(QIcon(":/icons/images/mainicon.png"));

    form->icon->setPixmap(QIcon(":/icons/images/mainicon.png").pixmap(64, 64));
    form->about->setText(tr("about_text"));
    form->libraries->setText(tr("libraries_text"));
    form->web_link->setText(tr("web_link_text"));
    form->web_link->setOpenExternalLinks(true);
    form->support_links->setText(tr("support_links_text"));
    form->support_links->setOpenExternalLinks(true);
    form->bug_tracker_link->setText(tr("bug_tracker_link"));
    form->bug_tracker_link->setOpenExternalLinks(true);
    form->source_code_link->setText(tr("source_code_link_text"));
    form->source_code_link->setOpenExternalLinks(true);
    form->version->setText(tr("version") + APP_VERSION + ".");

    connect(form->ok, SIGNAL(clicked()), this, SLOT(OnOkClicked()));
}

void AboutDialog::OnOkClicked()
{
    close();
}
