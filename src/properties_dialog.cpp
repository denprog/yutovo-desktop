/*
 * Yutovo Desktop
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "properties_dialog.h"
#include "result_settings_form.h"
#include "ui_properties_dialog.h"

//PropertiesDialog

PropertiesDialog::PropertiesDialog(yutovo::Config& _config) :
    form(new Ui::PropertiesDialog()),
    config(_config)
{
    form->setupUi(this);

    form->results_layout->addWidget(new ResultSettingsForm(config));

    form->language->setCurrentIndex((int)config.language - 1);

    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    
    connect(form->includes, &QListWidget::itemChanged, this, &PropertiesDialog::IncludesItemChanged);
    connect(form->move_file_up, &QAbstractButton::clicked, this, &PropertiesDialog::OnMoveFileUp);
    connect(form->move_file_down, &QAbstractButton::clicked, this, &PropertiesDialog::OnMoveFileDown);

    FillIncludes();

    connect(form->use_spaces, &QRadioButton::toggled, form->indent_size, &QSpinBox::setEnabled);
    form->use_tabs->setChecked(config.use_tabs);
    form->use_spaces->setChecked(!config.use_tabs);
    form->indent_size->setValue(config.tab_spaces);
    form->indent_size->setEnabled(form->use_spaces->isChecked());
}

void PropertiesDialog::accept()
{
    config.language = (yutovo_calculator::Language)(form->language->currentIndex() + 1);

    config.include_documents.documents.clear();
    for (int i = 0; i < form->includes->count(); ++i)
    {
        QListWidgetItem* item = form->includes->item(i);
        if (item->text() == "")
            continue;
        config.include_documents.documents.emplace_back(yutovo::Config::IncludeDocument{item->text().toUtf8().data()});
    }

    config.use_tabs = form->use_tabs->isChecked();
    config.tab_spaces = form->indent_size->value();

    QDialog::accept();
}

void PropertiesDialog::IncludesItemChanged(QListWidgetItem *item)
{
    int r = form->includes->row(item);
    if (item->text() != "" && r == form->includes->count() - 1)
    {
        //add empty item
        QListWidgetItem *item = new QListWidgetItem("");
        item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
        form->includes->addItem(item);
    }
    else if (item->text() == "" && form->includes->count() > 1 && r < form->includes->count() - 1)
    {
        //delete empty item
        form->includes->removeItemWidget(item);
        delete item;
    }
}

void PropertiesDialog::OnMoveFileUp()
{
    int r = form->includes->currentRow();
    if (r == 0)
        return;
    auto* cur = form->includes->takeItem(r);
    form->includes->insertItem(r - 1, cur);
    form->includes->setCurrentRow(r - 1);
}

void PropertiesDialog::OnMoveFileDown()
{
    int r = form->includes->currentRow();
    if (r >= form->includes->count() - 2)
        return;
    auto* cur = form->includes->takeItem(r);
    form->includes->insertItem(r + 1, cur);
    form->includes->setCurrentRow(r + 1);
}

void PropertiesDialog::FillIncludes()
{
    form->includes->clear();
    for (auto& inc : config.include_documents.documents)
    {
        QListWidgetItem *item = new QListWidgetItem(inc.file_name.c_str());
        item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
        form->includes->addItem(item);
    }

    //add empty item
    QListWidgetItem *item = new QListWidgetItem("");
    item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
    form->includes->addItem(item);
}
