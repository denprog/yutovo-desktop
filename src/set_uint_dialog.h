/*
 * Yutovo Desktop
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef SET_UINT_DIALOG_H
#define SET_UINT_DIALOG_H

#include <QDialog>

namespace Ui
{
class SetUintDialog;
}

class SetUintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetUintDialog(uint _value, QString caption, QString label, QWidget *parent = nullptr);
    ~SetUintDialog();

    virtual void accept();

public:
    uint value = 0;

private:
    Ui::SetUintDialog *ui;
};

#endif
