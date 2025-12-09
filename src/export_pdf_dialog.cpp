/*
 * Yutovo Desktop
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "export_pdf_dialog.h"
#include "ui_export_pdf_dialog.h"

#include <QFileDialog>
#include <QPageSize>
#include <QPageLayout>
#include <QDir>

//ExportPdfDialog

ExportPdfDialog::ExportPdfDialog(QMarginsF margins, QWidget *parent) : 
    QDialog(parent),
    ui(new Ui::ExportPdfDialog)
{
    ui->setupUi(this);
    ui->pageSize->setCurrentIndex(0); //default is A4
    UpdatePageDimensions();

    ui->marginLeft->setValue(margins.left());
    ui->marginTop->setValue(margins.top());
    ui->marginRight->setValue(margins.right());
    ui->marginBottom->setValue(margins.bottom());

    file_path = QDir::homePath() + "/document.pdf";
    ui->filePath->setText(file_path);
}

ExportPdfDialog::~ExportPdfDialog()
{
    delete ui;
}

QPrinter::PageSize ExportPdfDialog::PageSize() const
{
    return GetPageSize(ui->pageSize->currentIndex());
}

QPrinter::Orientation ExportPdfDialog::Orientation() const
{
    return GetOrientation(ui->orientation->currentIndex());
}

QMarginsF ExportPdfDialog::Margins() const
{
    return QMarginsF(ui->marginLeft->value(), ui->marginTop->value(), ui->marginRight->value(), ui->marginBottom->value());
}

QSizeF ExportPdfDialog::GetPageSize() const
{
    return QSizeF(ui->width->value(), ui->height->value());
}

QString ExportPdfDialog::FilePath() const
{
    return file_path;
}

void ExportPdfDialog::PageSizeChanged(int index)
{
    UpdatePageDimensions();
}

void ExportPdfDialog::OrientationChanged(int index)
{
    UpdatePageDimensions();
}

void ExportPdfDialog::UpdatePageDimensions()
{
    bool custom = ui->pageSize->currentIndex() == 4;
    ui->width->setEnabled(custom);
    ui->height->setEnabled(custom);

    if (custom)
    {
        int w = ui->width->value();
        int h = ui->height->value();
        if (GetOrientation(ui->orientation->currentIndex()) == QPrinter::Landscape)
        {
            if (w < h)
                std::swap(w, h);
        }
        else
        {
            if (w > h)
                std::swap(w, h);
        }
        ui->width->setValue(w);
        ui->height->setValue(h);
    }
    else
    {
        QPrinter::PageSize ps = GetPageSize(ui->pageSize->currentIndex());
        QPrinter tempPrinter(QPrinter::ScreenResolution);
        tempPrinter.setPageSize(ps);
        tempPrinter.setOrientation(GetOrientation(ui->orientation->currentIndex()));
        QPageLayout layout = tempPrinter.pageLayout();
        QSizeF sizeMM = layout.fullRect(QPageLayout::Millimeter).size();
        ui->width->setValue(qRound(sizeMM.width()));
        ui->height->setValue(qRound(sizeMM.height()));
    }
}

void ExportPdfDialog::ChooseFileClicked()
{
    QString file = QFileDialog::getSaveFileName(this, tr("Save PDF File"), file_path, tr("PDF Files (*.pdf);;All Files (*)"));
    if (!file.isEmpty())
    {
        file_path = file;
        ui->filePath->setText(file_path);
    }
}

void ExportPdfDialog::FilePathChanged(const QString &text)
{
    file_path = text;
    NormalizeFilePath();
}

void ExportPdfDialog::NormalizeFilePath()
{
    if (file_path.isEmpty())
        file_path = QDir::homePath() + "/document.pdf";
    else if (!file_path.endsWith(".pdf", Qt::CaseInsensitive))
        file_path += ".pdf";
    ui->filePath->setText(file_path);
}

QPrinter::PageSize ExportPdfDialog::GetPageSize(int index) const
{
    switch (index)
    {
    case 0:
        return QPrinter::A4;
    case 1:
        return QPrinter::A3;
    case 2:
        return QPrinter::Letter;
    case 3:
        return QPrinter::Legal;
    case 4:
        return QPrinter::Custom;
    default:
        return QPrinter::A4;
    }
}

QPrinter::Orientation ExportPdfDialog::GetOrientation(int index) const
{
    return (index == 0) ? QPrinter::Portrait : QPrinter::Landscape;
}
