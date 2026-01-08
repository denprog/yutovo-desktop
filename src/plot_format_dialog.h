/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __PLOT_FORMAT_DIALOG_H__
#define __PLOT_FORMAT_DIALOG_H__

#include <QDialog>
#include <yutovo-editor/style.h>

namespace Ui
{
class PlotFormatDialog;
};

class PlotFormatDialog : public QDialog
{
    Q_OBJECT

public:
    PlotFormatDialog(yutovo::PlotFormat& _plot_format);

private slots:
    void OnColorClicked();
    void OnAccepted();

private:
    Ui::PlotFormatDialog* form = nullptr;
    yutovo::PlotFormat& plot_format;
};

#endif
