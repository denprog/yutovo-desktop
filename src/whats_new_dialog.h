/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __WHATS_NEW_DIALOG_H__
#define __WHATS_NEW_DIALOG_H__

#include <QDialog>

namespace Ui
{
class WhatsNewDialog;
};

class WhatsNewDialog : public QDialog
{
    Q_OBJECT

public:
    WhatsNewDialog(const QString& language, QWidget* parent = nullptr);

private slots:
    void OnOkClicked();

private:
    Ui::WhatsNewDialog* form = nullptr;

    QString ExtractCurrentVersion(const QString& text);
};

#endif
