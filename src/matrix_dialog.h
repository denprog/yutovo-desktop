/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef MATRIX_DIALOG_H
#define MATRIX_DIALOG_H

#include <QDialog>

namespace Ui
{
class MatrixDialog;
}

class MatrixDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MatrixDialog(QString caption, uint _rows = 2, uint _columns = 2, QWidget *parent = nullptr);
    ~MatrixDialog();

    virtual void accept();

public:
    uint rows = 2;
    uint columns = 2;

private:
    Ui::MatrixDialog *ui;
};

#endif
