/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef UPDATE_AVAILABLE_DIALOG_H
#define UPDATE_AVAILABLE_DIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui
{
class UpdateAvailableDialog;
}
QT_END_NAMESPACE

class UpdateAvailableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateAvailableDialog(const QString& version, const QString& url, QWidget* parent = nullptr);
    ~UpdateAvailableDialog();

private:
    Ui::UpdateAvailableDialog* form;
};

#endif // UPDATE_AVAILABLE_DIALOG_H
