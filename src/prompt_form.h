/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __PROMPT_FORM_H__
#define __PROMPT_FORM_H__

#include <QListWidget>
#include <yutovo-editor/editor_utils.h>

using namespace yutovo;

class PromptForm : public QListWidget
{
    Q_OBJECT

public:
    PromptForm(QWidget* parent = nullptr);
    ~PromptForm();

    void Fill(std::vector<std::pair<IdentifierType, std::string>>&& _prompt);

    void ActivateCurrentItem();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

public slots:
    void OnPromptActivated(QListWidgetItem *item);

signals:
    void PromptActivated(const IdentifierType id_type, const std::string& id_str);

private:
    std::vector<std::pair<IdentifierType, std::string>> prompt;
    const int max_visible_rows = 10;
    static const std::map<std::string, QString> icons;
};

#endif
