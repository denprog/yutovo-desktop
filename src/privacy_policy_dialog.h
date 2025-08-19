/*
 * Yutovo Desktop
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __PRIVACY_POLICY_DIALOG_H__
#define __PRIVACY_POLICY_DIALOG_H__

#include <QDialog>

namespace Ui
{
class PrivacyPolicyDialog;
};

class PrivacyPolicyDialog : public QDialog
{
    Q_OBJECT

public:
    PrivacyPolicyDialog();

private slots:
    void OnOkClicked();

private:
    Ui::PrivacyPolicyDialog* form = nullptr;
};

#endif
