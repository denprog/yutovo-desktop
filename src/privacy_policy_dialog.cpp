/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "privacy_policy_dialog.h"
#include "ui_privacy_policy_dialog.h"

//PrivacyPolicyDialog

PrivacyPolicyDialog::PrivacyPolicyDialog() :
    form(new Ui::PrivacyPolicyDialog())
{
    form->setupUi(this);

    setFixedSize(width(), height());
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    setWindowIcon(QIcon(":/icons/images/mainicon.png"));

    form->icon->setPixmap(QIcon(":/icons/images/mainicon.png").pixmap(64, 64));
    form->privacy_policy->setText(tr("privacy_policy"));

    connect(form->ok, SIGNAL(clicked()), this, SLOT(OnOkClicked()));
}

void PrivacyPolicyDialog::OnOkClicked()
{
    close();
}
