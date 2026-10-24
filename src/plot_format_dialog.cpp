/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "plot_format_dialog.h"
#include "ui_plot_format_dialog.h"
#include <QColorDialog>

//PlotFormatDialog

PlotFormatDialog::PlotFormatDialog(yutovo::PlotFormat& _plot_format, bool surface, bool histogram) :
    form(new Ui::PlotFormatDialog()),
    plot_format(_plot_format)
{
    form->setupUi(this);

    setWindowIcon(QIcon(":/icons/images/mainicon.png"));

    form->width->setValue(plot_format.width);

    int style_index = 0;
    if (histogram)
    {
        form->style->addItem(tr("Bars"));
        form->style->addItem(tr("Bars with line"));
        form->style->addItem(tr("Bars without gaps"));
        form->style->addItem(tr("Stems"));
        form->style->addItem(tr("Area"));
        form->style->addItem(tr("Step"));
        form->style->addItem(tr("Marks"));
        style_index = (int)plot_format.histogram_style;
    }
    else
    {
        form->style->addItem(tr("Color by height"));
        form->style->addItem(tr("Solid color"));
        form->style->addItem(tr("Color by height with mesh"));
        form->style->addItem(tr("Wireframe"));
        form->style->addItem(tr("Points"));
        style_index = (int)plot_format.style;
    }
    form->style->setCurrentIndex(style_index);
    style_enabled = surface || histogram;
    histogram_style = histogram;
    if (!style_enabled)
    {
        form->label_style->hide();
        form->style->hide();
    }

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
    if (style_enabled)
    {
        if (histogram_style)
            plot_format.histogram_style = (yutovo::HistogramStyle)form->style->currentIndex();
        else
            plot_format.style = (yutovo::SurfaceStyle)form->style->currentIndex();
    }

    close();
}
