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

    form->graph_width->setValue(settings.size.width);
    form->graph_height->setValue(settings.size.height);
    form->plot_width->setValue(settings.plot_width);

    QColor c = QColor::fromRgb(settings.plot_color.ToInt());
    form->plot_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    connect(form->plot_color, SIGNAL(clicked()), this, SLOT(OnPlotColorClicked()));

    connect(this, &QDialog::accepted, this, &GraphSettingsDialog::OnAccepted);
}

void GraphSettingsDialog::OnPlotColorClicked()
{
    QColorDialog d(QColor::fromRgba(settings.plot_color.ToInt()), this);
    if (d.exec() == QDialog::Accepted)
    {
        QColor c = d.selectedColor();
        settings.plot_color = yutovo::Color{(uint8_t)c.alpha(), (uint8_t)c.red(), (uint8_t)c.green(), (uint8_t)c.blue()};
        form->plot_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    }
}

void GraphSettingsDialog::OnAccepted()
{
    settings.size.width = form->graph_width->value();
    settings.size.height = form->graph_height->value();
    settings.plot_width = form->plot_width->value();

    close();
}
