/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "feedback_dialog.h"
#include "ui_feedback_dialog.h"
#include "mainwindow.h"
#include "document_window.h"
#include "about_dialog.h"

#include <QApplication>
#include <QMessageBox>
#include <QSettings>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QSysInfo>
#include <QMimeDatabase>
#include <QMimeType>

//FeedbackDialog

FeedbackDialog::FeedbackDialog(QWidget* parent) :
    QDialog(parent),
    form(new Ui::FeedbackDialog())
{
    form->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    setModal(true);
    setWindowTitle(tr("Send Feedback"));

    network_manager = new QNetworkAccessManager(this);
    connect(network_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(OnNetworkReplyFinished(QNetworkReply*)));

    ReadFeedbackSettings();

    UpdateCharacterCount();

    connect(form->messageEdit, SIGNAL(textChanged()), this, SLOT(OnMessageTextChanged()));
    connect(form->attachButton, SIGNAL(clicked()), this, SLOT(OnAttachClicked()));
    connect(form->clearAttachmentButton, SIGNAL(clicked()), this, SLOT(OnClearAttachmentClicked()));

    QPushButton* send_button = form->buttonBox->button(QDialogButtonBox::Save);
    if (send_button)
        send_button->setText(tr("Send"));

    connect(form->buttonBox, SIGNAL(accepted()), this, SLOT(SendFeedback()));
    connect(form->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

FeedbackDialog::~FeedbackDialog()
{
    delete form;
}

bool FeedbackDialog::ValidateInput()
{
    if (form->nameEdit->text().trimmed().isEmpty() || form->emailEdit->text().trimmed().isEmpty() ||
        form->topicCombo->currentText().trimmed().isEmpty() || form->messageEdit->toPlainText().trimmed().isEmpty())
    {
        QMessageBox::warning(this, tr("Send Feedback"), tr("Please fill in all required fields"));
        return false;
    }

    if (!form->emailEdit->text().contains('@'))
    {
        QMessageBox::warning(this, tr("Send Feedback"), tr("Please enter a valid email address"));
        return false;
    }

    return true;
}

void FeedbackDialog::SendFeedback()
{
    if (!ValidateInput())
        return;

    SaveFeedbackSettings();

    QJsonObject json;
    json["name"] = form->nameEdit->text();
    json["email"] = form->emailEdit->text();
    json["topic"] = form->topicCombo->currentText();
    json["message"] = form->messageEdit->toPlainText();
    json["version"] = APP_VERSION;
    json["platform"] = GetPlatformString();
    json["attachments"] = BuildAttachmentArray();

    QNetworkRequest request(QUrl(FEEDBACK_SERVER_URL));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc(json);
    network_manager->post(request, doc.toJson());

    setEnabled(false);
    QApplication::setOverrideCursor(Qt::WaitCursor);
}

void FeedbackDialog::OnNetworkReplyFinished(QNetworkReply* reply)
{
    setEnabled(true);
    QApplication::restoreOverrideCursor();

    bool success = false;
    if (reply->error() == QNetworkReply::NoError)
    {
        int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (status == 200)
            success = true;
    }

    reply->deleteLater();

    if (success)
    {
        QMessageBox::information(this, tr("Send Feedback"), tr("Feedback sent successfully"));
        accept();
    }
    else
    {
        QMessageBox::warning(this, tr("Send Feedback"), tr("Failed to send feedback"));
    }
}

QString FeedbackDialog::GetPlatformString()
{
    return QSysInfo::prettyProductName() + ", " + QSysInfo::currentCpuArchitecture() + ", kernel " + QSysInfo::kernelVersion();
}

QJsonArray FeedbackDialog::BuildAttachmentArray()
{
    QJsonArray attachments;
    QMimeDatabase mime_db;

    auto add_file = 
        [&](const QString& path)
        {
            QFile file(path);
            if (!file.open(QIODevice::ReadOnly))
                return;

            QFileInfo info(path);
            QMimeType mime = mime_db.mimeTypeForFile(info);

            QJsonObject attachment;
            attachment["filename"] = info.fileName();
            attachment["content_type"] = mime.name();
            attachment["data"] = QString(file.readAll().toBase64());
            attachments.append(attachment);
        };

    if (!attachment_path.isEmpty())
        add_file(attachment_path);

    if (form->attachCurrentDocumentCheck->isChecked())
    {
        QString doc_path = GetCurrentDocumentPath();
        if (!doc_path.isEmpty() && doc_path != attachment_path)
            add_file(doc_path);
    }

    return attachments;
}

QString FeedbackDialog::GetCurrentDocumentPath()
{
    MainWindow* main_window = qobject_cast<MainWindow*>(parentWidget());
    if (!main_window)
        return QString();

    return main_window->GetCurrentDocumentPath();
}

void FeedbackDialog::ReadFeedbackSettings()
{
    QSettings settings;
    settings.beginGroup("Feedback");
    form->nameEdit->setText(settings.value("name").toString());
    form->emailEdit->setText(settings.value("email").toString());
    settings.endGroup();
}

void FeedbackDialog::SaveFeedbackSettings()
{
    QSettings settings;
    settings.beginGroup("Feedback");
    settings.setValue("name", form->nameEdit->text());
    settings.setValue("email", form->emailEdit->text());
    settings.endGroup();
}

void FeedbackDialog::OnAttachClicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Attach file..."));
    if (file_name.isEmpty())
        return;

    attachment_path = file_name;
    QFileInfo info(file_name);
    form->attachmentLabel->setText(info.fileName());
}

void FeedbackDialog::OnClearAttachmentClicked()
{
    attachment_path.clear();
    form->attachmentLabel->setText(tr("No file attached"));
}

void FeedbackDialog::OnMessageTextChanged()
{
    const int max_length = 5000;
    QString text = form->messageEdit->toPlainText();
    if (text.length() > max_length)
    {
        int pos = form->messageEdit->textCursor().position();
        form->messageEdit->blockSignals(true);
        form->messageEdit->setPlainText(text.left(max_length));
        QTextCursor cursor = form->messageEdit->textCursor();
        cursor.setPosition(qMin(pos, max_length));
        form->messageEdit->setTextCursor(cursor);
        form->messageEdit->blockSignals(false);
    }
    UpdateCharacterCount();
}

void FeedbackDialog::UpdateCharacterCount()
{
    const int max_length = 5000;
    form->characterCountLabel->setText(tr("Characters: %1 / %2").arg(form->messageEdit->toPlainText().length()).arg(max_length));
}
