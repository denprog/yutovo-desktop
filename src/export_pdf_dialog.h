/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __EXPORT_PDF_DIALOG_H__
#define __EXPORT_PDF_DIALOG_H__

#include <QDialog>
#include <QPrinter>
#include <QSizeF>

namespace Ui
{
class ExportPdfDialog;
}

class ExportPdfDialog : public QDialog
{
    Q_OBJECT

public:
    ExportPdfDialog(QMarginsF margins, QWidget *parent = nullptr);
    ~ExportPdfDialog();

    QPrinter::PageSize PageSize() const;
    QPrinter::Orientation Orientation() const;
    QMarginsF Margins() const;
    QSizeF GetPageSize() const;
    QString FilePath() const;

public slots:
    void PageSizeChanged(int index);
    void OrientationChanged(int index);

private slots:
    void ChooseFileClicked();
    void FilePathChanged(const QString &text);

private:
    void UpdatePageDimensions();
    QPrinter::PageSize GetPageSize(int index) const;
    QPrinter::Orientation GetOrientation(int index) const;
    void NormalizeFilePath();

private:
    Ui::ExportPdfDialog *ui;
    QString file_path;
};

#endif
