/*
 * Yutovo Desktop
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __DOCUMENT_WIDGET_H__
#define __DOCUMENT_WIDGET_H__

#include <QWidget>
#include <yutovo-editor/document.h>
#include "qt_window.h"
#include "command_map.h"

using namespace yutovo;

class DocumentWidget : public QWidget
{
    Q_OBJECT

public:
    DocumentWidget(QWidget *parent, yutovo::Config& _config);

    DocumentPtr CreateDocument(Config& config);

    void InsertText(const std::string& str, const StringFormatPtr string_format);

    bool GetElementAtCoords(const int x, const int y, ElementId& id);

public:
    void OnNextEditorTab();
    void OnPrevEditorTab();

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

signals:
    void WheelVertical(const int value);
    void WheelHorizontal(const int value);
    void NextEditorTab();
    void PrevEditorTab();
    void ServiceStatus(IOResult result);

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

    ShortcutsMap shortcuts_map;
    
    EditorState current_editor_state;

    DocumentPtr document;

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
