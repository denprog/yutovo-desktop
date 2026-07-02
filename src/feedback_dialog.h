/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef FEEDBACK_DIALOG_H
#define FEEDBACK_DIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#ifdef BUILD_TESTS
#define FEEDBACK_SERVER_URL "http://localhost:9001/service/send-feedback"
#else
#define FEEDBACK_SERVER_URL "https://yutovo.com/service/send-feedback"
#endif

QT_BEGIN_NAMESPACE
namespace Ui
{
class FeedbackDialog;
}
QT_END_NAMESPACE

class FeedbackDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FeedbackDialog(QWidget* parent = nullptr);
    ~FeedbackDialog();

public slots:
    void SendFeedback();
    void OnNetworkReplyFinished(QNetworkReply* reply);

private:
    bool ValidateInput();
    void ReadFeedbackSettings();
    void SaveFeedbackSettings();
    QJsonArray BuildAttachmentArray();
    QString GetCurrentDocumentPath();
    QString GetPlatformString();

private slots:
    void OnAttachClicked();
    void OnClearAttachmentClicked();
    void OnMessageTextChanged();
    void UpdateCharacterCount();

private:
    Ui::FeedbackDialog* form;
    QNetworkAccessManager* network_manager;
    QString attachment_path;
};

#endif // FEEDBACK_DIALOG_H
