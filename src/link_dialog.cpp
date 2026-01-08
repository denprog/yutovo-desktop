/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "link_dialog.h"
#include "ui_link_dialog.h"
#include <QPushButton>

//LinkDialog

LinkDialog::LinkDialog(QString _text, QString _url, QString caption, QWidget *parent) : 
    QDialog(parent),
    ui(new Ui::LinkDialog)
{
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    connect(ui->text, &QLineEdit::textChanged, this, &LinkDialog::OnTextChanged);
    connect(ui->url, &QLineEdit::textChanged, this, &LinkDialog::OnUrlChanged);

    setWindowTitle(caption);
    ui->text->setText(_text);
    ui->url->setText(_url);
}

void LinkDialog::OnTextChanged(const QString& _text)
{
    text = ui->text->text();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty() && !url.isEmpty());
}

void LinkDialog::OnUrlChanged(const QString& _text)
{
    url = ui->url->text();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty() && !url.isEmpty());
}

LinkDialog::~LinkDialog()
{
    delete ui;
}
