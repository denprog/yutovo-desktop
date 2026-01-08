/*
 * Yutovo Desktop
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "settings_dialog.h"
#include "system_settings_form.h"
#include "documents_settings_form.h"
#include "interface_settings_form.h"
#include "result_settings_form.h"
#include "colors_settings_form.h"
#include "fonts_settings_form.h"
#include "computation_settings_form.h"
#include "ui_settings_dialog.h"
#include <QLabel>

//SettingsDialog

SettingsDialog::SettingsDialog(yutovo::Config& _config, QHash<QString, QVariant>& _settings) :
    form(new Ui::SettingsDialog()),
    config(_config),
    settings(_settings)
{
    form->setupUi(this);

    form->settings_tree->setColumnCount(1);
    connect(form->settings_tree, &QTreeWidget::itemClicked, this, &SettingsDialog::OnSettingsTreeItemActivated);

    QTreeWidgetItem *basic_item = new QTreeWidgetItem(form->settings_tree);
    basic_item->setText(0, tr("Basic"));
    QTreeWidgetItem *item = new QTreeWidgetItem(basic_item);
    item->setText(0, tr("System"));
    form->settings_tree->setCurrentItem(item);
    OnSettingsTreeItemActivated(item, 0);
    item = new QTreeWidgetItem(basic_item);
    item->setText(0, tr("Documents"));
    item = new QTreeWidgetItem(basic_item);
    item->setText(0, tr("Interface"));

    QTreeWidgetItem *formula_item = new QTreeWidgetItem(form->settings_tree);
    formula_item->setText(0, tr("Formula"));
    item = new QTreeWidgetItem(formula_item);
    item->setText(0, tr("Fonts"));
    item = new QTreeWidgetItem(formula_item);
    item->setText(0, tr("Colors"));

    QTreeWidgetItem *calculator_item = new QTreeWidgetItem(form->settings_tree);
    calculator_item->setText(0, tr("Calculator"));
    item = new QTreeWidgetItem(calculator_item);
    item->setText(0, tr("Computation"));
    item = new QTreeWidgetItem(calculator_item);
    item->setText(0, tr("Result"));

    form->settings_tree->expandAll();
}

void SettingsDialog::OnSettingsTreeItemActivated(QTreeWidgetItem *item, int column)
{
    QLayoutItem* _item;
    while ((_item = form->settings_page_layout->takeAt(0)) != nullptr)
    {
        delete _item->widget();
        delete _item;
    }
    
    if (item->text(0) == tr("Basic"))
    {
        QList<QTreeWidgetItem*> items = form->settings_tree->findItems(tr("System"), Qt::MatchExactly | Qt::MatchRecursive);
        form->settings_tree->setCurrentItem(*items.begin());
        form->settings_page_layout->addWidget(new SystemSettingsForm(config));
    }
    else if (item->text(0) == tr("System"))
    {
        form->settings_page_layout->addWidget(new SystemSettingsForm(config));
    }
    else if (item->text(0) == tr("Documents"))
    {
        form->settings_page_layout->addWidget(new DocumentsSettingsForm(settings, config));
    }
    else if (item->text(0) == tr("Interface"))
    {
        form->settings_page_layout->addWidget(new InterfaceSettingsForm(settings));
    }
    else if (item->text(0) == tr("Formula"))
    {
        QList<QTreeWidgetItem*> items = form->settings_tree->findItems(tr("Fonts"), Qt::MatchExactly | Qt::MatchRecursive);
        form->settings_tree->setCurrentItem(*items.begin());
        form->settings_page_layout->addWidget(new FontsSettingsForm(config));
    }
    else if (item->text(0) == tr("Fonts"))
    {
        form->settings_page_layout->addWidget(new FontsSettingsForm(config));
    }
    else if (item->text(0) == tr("Colors"))
    {
        form->settings_page_layout->addWidget(new ColorsSettingsForm(config));
    }
    else if (item->text(0) == tr("Calculator"))
    {
        QList<QTreeWidgetItem*> items = form->settings_tree->findItems(tr("Computation"), Qt::MatchExactly | Qt::MatchRecursive);
        form->settings_tree->setCurrentItem(*items.begin());
        form->settings_page_layout->addWidget(new ComputationSettingsForm(config));
    }
    else if (item->text(0) == tr("Computation"))
    {
        form->settings_page_layout->addWidget(new ComputationSettingsForm(config));
    }
    else if (item->text(0) == tr("Result"))
    {
        form->settings_page_layout->addWidget(new QLabel(tr("Result settings for new documents")));
        form->settings_page_layout->addWidget(new ResultSettingsForm(config));
    }
}
