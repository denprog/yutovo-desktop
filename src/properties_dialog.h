/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __PROPERTIES_DIALOG_H__
#define __PROPERTIES_DIALOG_H__

#include <QDialog>
#include <QTreeWidget>
#include <QSettings>
#include <QListWidgetItem>
#include <yutovo-editor/config.h>

namespace Ui
{
class PropertiesDialog;
};

class PropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    PropertiesDialog(yutovo::Config& _config);

    virtual void accept();

public slots:
    void IncludesItemChanged(QListWidgetItem *item);
    void OnMoveFileUp();
    void OnMoveFileDown();

private:
    void FillIncludes();

private:
    Ui::PropertiesDialog* form = nullptr;

public:
    yutovo::Config& config; //copy of config
};

#endif
