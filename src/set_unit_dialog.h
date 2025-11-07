/*
 * Yutovo Desktop
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __SET_UNIT_DIALOG_H__
#define __SET_UNIT_DIALOG_H__

#include <QDialog>
#include <QListWidgetItem>
#include <QStyledItemDelegate>
#include <thread>
#include <mutex>
#include <yutovo-calculator/unit.h>
#include <yutovo-editor/window.h>
#include <yutovo-editor/document.h>
#include "qt_window.h"

namespace Ui
{
class SetUnitDialog;
}

using namespace yutovo;
using namespace yutovo_calculator;

class UnitsDelegate;

class SetUnitDialog : public QDialog
{
    Q_OBJECT

public:
    SetUnitDialog(std::vector<Unit>& cast_units, yutovo::Config& _config);
    ~SetUnitDialog();

private:
    void FillUnits();

public slots:
    void OnCurrentSystemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void OnUnitsItemDoubleClicked(QListWidgetItem *item);
    void OnUnitsItemsReady();
    void OnAccepted();

signals:
    void UnitsItemsReady();

public:
    Unit value;

private:
    std::shared_ptr<QtWindow> window;

    Ui::SetUnitDialog *ui;
    std::map<std::u32string, std::vector<Unit>> system_units;

    friend class UnitsDelegate;

    std::unique_ptr<UnitsDelegate> units_delegate;

    std::thread fill_thread;
    std::u32string current_system;
    std::mutex units_items_mutex;
    std::map<int, QPixmap> units_items;
    bool stop_fill_thread = false;
};

class UnitsDelegate : public QStyledItemDelegate
{
public:
    UnitsDelegate(SetUnitDialog* _dialog);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    SetUnitDialog* dialog;
};

#endif
