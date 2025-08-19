/*
 * Yutovo Desktop
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "terms_of_use_dialog.h"
#include "ui_terms_of_use_dialog.h"

//TermsOfUseDialog

TermsOfUseDialog::TermsOfUseDialog() :
    form(new Ui::TermsOfUseDialog())
{
    form->setupUi(this);

    setFixedSize(width(), height());
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    setWindowIcon(QIcon(":/icons/images/mainicon.png"));

    form->icon->setPixmap(QIcon(":/icons/images/mainicon.png").pixmap(64, 64));
    form->terms_of_use->setText(tr("terms_of_use"));

    connect(form->ok, SIGNAL(clicked()), this, SLOT(OnOkClicked()));
}

void TermsOfUseDialog::OnOkClicked()
{
    close();
}
