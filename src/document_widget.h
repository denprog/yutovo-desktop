/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __DOCUMENT_WIDGET_H__
#define __DOCUMENT_WIDGET_H__

#include <QWidget>
#include <QSettings>
#include <yutovo-editor/document.h>
#include "qt_window.h"
#include "command_map.h"
#include "prompt_form.h"

using namespace yutovo;

class DocumentWidget : public QWidget
{
    Q_OBJECT

public:
    DocumentWidget(QWidget *parent, yutovo::Config& _config, QSettings& _settings);

    DocumentPtr CreateDocument(Config& config);

    void InsertText(const std::string& str, const StringFormatPtr string_format);

public:
    void OnNextEditorTab();
    void OnPrevEditorTab();
    void OnPrompt();

public slots:
    void OnDocumentUpdated(const Rect rect);
    void OnCaretMoved(const EditorState editor_state);
    void OnFormatingStarted();
    void OnFormatingFinished();
    void OnResizeStarted();
    void OnResizeFinished();
#ifdef REMOTE_SOLVER
    void OnServiceStatus(IOResult result);
#endif
    void OnPromptActivated(const IdentifierType id_type, const std::string& id_str);

signals:
    void WheelVertical(const int value);
    void WheelHorizontal(const int value);
    void NextEditorTab();
    void PrevEditorTab();
    void ServiceStatus(IOResult result);
    void ScaleChanged(const float scale);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent* event);
    virtual void focusInEvent(QFocusEvent *event);
    virtual void focusOutEvent(QFocusEvent *event);
    virtual void timerEvent(QTimerEvent *event) override;

private:
    friend class DocumentWindow;
    friend class MainWindow;

    QtWindow window;
    DocumentPtr document;

    PromptForm prompt_form;
    bool show_prompt = false;

    ShortcutsMap shortcuts_map;
    
    EditorState current_editor_state;

    QSettings& settings;

    uint caret_moving_task_id = 0;

    int delay_timer = 0;
    int wait_timer = 0;

    bool formating = false;
    bool loading = false;
    bool resizing = false;

    QSize resize;

    QPoint left_click_pos;

    ElementId mouse_capture_id;

    Logger* logger;
};

#endif
