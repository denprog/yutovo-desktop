/*
 * Yutovo Desktop
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __GRAPH_SETTINGS_DIALOG_H__
#define __GRAPH_SETTINGS_DIALOG_H__

#include <QDialog>
#include <yutovo-editor/style.h>

namespace Ui
{
class GraphSettingsDialog;
};

class GraphSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    GraphSettingsDialog(yutovo::GraphFormat& _settings);

private slots:
    void OnPlotColorClicked();
    void OnAccepted();

private:
    Ui::GraphSettingsDialog* form = nullptr;
    yutovo::GraphFormat& settings;
};

#endif
