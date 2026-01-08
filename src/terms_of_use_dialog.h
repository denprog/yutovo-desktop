/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __TERMS_OF_USE_DIALOG_H__
#define __TERMS_OF_USE_DIALOG_H__

#include <QDialog>

namespace Ui
{
class TermsOfUseDialog;
};

class TermsOfUseDialog : public QDialog
{
    Q_OBJECT

public:
    TermsOfUseDialog();

private slots:
    void OnOkClicked();

private:
    Ui::TermsOfUseDialog* form = nullptr;
};

#endif
