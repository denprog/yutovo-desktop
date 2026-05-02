/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "whats_new_dialog.h"
#include "ui_whats_new_dialog.h"
#include <QFile>
#include <QTextStream>

//WhatsNewDialog

WhatsNewDialog::WhatsNewDialog(const QString& language, QWidget* parent) :
    QDialog(parent),
    form(new Ui::WhatsNewDialog())
{
    form->setupUi(this);

    setFixedSize(width(), height());
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint
#ifdef _WIN32
        | Qt::WindowTitleHint
#endif
        );
    setWindowIcon(QIcon(":/icons/images/mainicon.png"));

    QString suffix;
    if (language == "ru")
        suffix = "ru";
    else if (language == "es")
        suffix = "es";
    else if (language == "pt_BR")
        suffix = "pt_BR";
    else
        suffix = "en";

    QFile file(":/CHANGELOG_" + suffix + ".md");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        stream.setCodec("UTF-8");
#endif
        QString content = stream.readAll();
        form->text_edit->setHtml(ExtractCurrentVersion(content));
    }
    else
    {
        form->text_edit->setPlainText(tr("Changelog not found."));
    }

    connect(form->ok, SIGNAL(clicked()), this, SLOT(OnOkClicked()));
}

void WhatsNewDialog::OnOkClicked()
{
    close();
}

QString WhatsNewDialog::ExtractCurrentVersion(const QString& text)
{
    QStringList lines = text.split('\n');
    bool in_current = false;
    QStringList items;
    QString version;

    for (const QString& raw : lines)
    {
        QString line = raw.trimmed();
        if (line.startsWith("# "))
        {
            if (in_current)
                break;
            in_current = true;
            version = line.mid(2).trimmed();
        }
        else if (!line.isEmpty() && in_current)
        {
            if (line.startsWith("- "))
                items.append(line.mid(2));
            else
                items.append(line);
        }
    }

    QString html = "<html><body style='font-family:sans-serif; font-size:13px;'>";
    html += "<p style='font-size:15px; font-weight:bold; margin:0 0 8px 0;'>" + tr("Version").toHtmlEscaped() + " " + version.toHtmlEscaped() + "</p>";
    if (!items.isEmpty())
    {
        html += "<ul style='margin-top:4px; padding-left:16px;'>";
        for (const QString& item : items)
            html += "<li style='margin-bottom:4px;'>" + item.toHtmlEscaped() + "</li>";
        html += "</ul>";
    }
    html += "</body></html>";
    return html;
}
