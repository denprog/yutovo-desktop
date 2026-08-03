/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __ABOUT_DIALOG_H__
#define __ABOUT_DIALOG_H__

#include <QDialog>

namespace Ui
{
class AboutDialog;
};

static constexpr const char* APP_VERSION = "1.6.1";

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    AboutDialog();

private slots:
    void OnOkClicked();

private:
    Ui::AboutDialog* form = nullptr;
};

#endif
