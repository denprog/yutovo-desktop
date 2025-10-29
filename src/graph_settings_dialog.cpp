/*
 * Yutovo Desktop
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "graph_settings_dialog.h"
#include "ui_graph_settings_dialog.h"
#include <QColorDialog>

//GraphSettingsDialog

GraphSettingsDialog::GraphSettingsDialog(yutovo::GraphFormat& _settings) :
    form(new Ui::GraphSettingsDialog()),
    settings(_settings)
{
    form->setupUi(this);

    setWindowIcon(QIcon(":/icons/images/mainicon.png"));

    form->width->setValue(settings.size.width);
    form->height->setValue(settings.size.height);
    form->grid_width->setValue(settings.grid_width);

    QColor c = QColor::fromRgb(settings.color.ToInt());
    form->color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    connect(form->color, SIGNAL(clicked()), this, SLOT(OnColorClicked()));
    connect(this, &QDialog::accepted, this, &GraphSettingsDialog::OnAccepted);
}

void GraphSettingsDialog::OnColorClicked()
{
    QColorDialog d(QColor::fromRgba(settings.color.ToInt()), this);
    if (d.exec() == QDialog::Accepted)
    {
        QColor c = d.selectedColor();
        settings.color = yutovo::Color{(uint8_t)c.alpha(), (uint8_t)c.red(), (uint8_t)c.green(), (uint8_t)c.blue()};
        form->color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    }
}

void GraphSettingsDialog::OnAccepted()
{
    settings.size.width = form->width->value();
    settings.size.height = form->height->value();
    settings.grid_width = form->grid_width->value();

    close();
}
