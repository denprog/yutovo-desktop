/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "matrix_dialog.h"
#include "ui_matrix_dialog.h"

//MatrixDialog

MatrixDialog::MatrixDialog(QString caption, uint _rows, uint _columns, bool vector_mode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MatrixDialog)
{
    ui->setupUi(this);

    setWindowTitle(caption);
    ui->rows->setValue(_rows);
    ui->columns->setValue(_columns);
    if (vector_mode)
    {
        ui->columns_label->hide();
        ui->columns->hide();
        ui->columns->setValue(1);
    }
}

MatrixDialog::~MatrixDialog()
{
    delete ui;
}

void MatrixDialog::accept()
{
    rows = ui->rows->value();
    columns = ui->columns->value();
    QDialog::accept();
}
