/*
 * Yutovo Desktop
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "plot_format_dialog.h"
#include "ui_plot_format_dialog.h"
#include <QColorDialog>

//PlotFormatDialog

PlotFormatDialog::PlotFormatDialog(yutovo::PlotFormat& _plot_format) :
    form(new Ui::PlotFormatDialog()),
    plot_format(_plot_format)
{
    form->setupUi(this);

    setWindowIcon(QIcon(":/icons/images/mainicon.png"));

    form->width->setValue(plot_format.width);

    QColor c = QColor::fromRgb(plot_format.color.ToInt());
    form->color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    connect(form->color, SIGNAL(clicked()), this, SLOT(OnColorClicked()));
    connect(this, &QDialog::accepted, this, &PlotFormatDialog::OnAccepted);
}

void PlotFormatDialog::OnColorClicked()
{
    QColorDialog d(QColor::fromRgba(plot_format.color.ToInt()), this);
    if (d.exec() == QDialog::Accepted)
    {
        QColor c = d.selectedColor();
        plot_format.color = yutovo::Color{(uint8_t)c.alpha(), (uint8_t)c.red(), (uint8_t)c.green(), (uint8_t)c.blue()};
        form->color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    }
}

void PlotFormatDialog::OnAccepted()
{
    plot_format.width = form->width->value();

    close();
}
