/*
 * Yutovo Desktop
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "document_widget.h"
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QGuiApplication>
#include <QCursor>
#include "mainwindow.h"

//DocumentWidget

DocumentWidget::DocumentWidget(QWidget *parent, yutovo::Config& _config) :
    QWidget(parent),
    window(size().width(), size().height(), _config),
    logger(Logger::GetInstance(_config.logs_path + "/yutovo-desktop", "yutovo-desktop", _config.log_console, _config.log_file))
{
    connect(&window, &QtWindow::DocumentUpdated, this, &DocumentWidget::OnDocumentUpdated);
    connect(&window, &QtWindow::CaretMoved, this, &DocumentWidget::OnCaretMoved);
    connect(&window, &QtWindow::FormatingStarted, this, &DocumentWidget::OnFormatingStarted);
    connect(&window, &QtWindow::FormatingFinished, this, &DocumentWidget::OnFormatingFinished);
    connect(&window, &QtWindow::ResizeStarted, this, &DocumentWidget::OnResizeStarted);
    connect(&window, &QtWindow::ResizeFinished, this, &DocumentWidget::OnResizeFinished);
#ifdef REMOTE_SOLVER
    connect(&window, &QtWindow::ServiceStatus, this, &DocumentWidget::OnServiceStatus);
#endif

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

DocumentPtr DocumentWidget::CreateDocument(Config& config)
{
    document.reset(new Document(&window, config));
    shortcuts_map.Init(document, this);
    return document;
}

void DocumentWidget::InsertText(const std::string& str, const StringFormatPtr string_format)
{
    document->InsertString(str, string_format, true);
}

bool DocumentWidget::GetElementAtCoords(const int x, const int y, ElementId& id)
{
    auto p = window.GetDocumentPoint();
    return document->GetElementAtCoords(x + p.x, y + p.y, id);
}

void DocumentWidget::OnNextEditorTab()
{
    emit NextEditorTab();
}

void DocumentWidget::OnPrevEditorTab()
{
    emit PrevEditorTab();
}

void DocumentWidget::OnDocumentUpdated(const Rect rect)
{
    update(rect.left, rect.top, rect.width, rect.height);
}

void DocumentWidget::OnCaretMoved(const EditorState editor_state)
{
    current_editor_state = editor_state;
}

void DocumentWidget::OnFormatingStarted()
{
    formating = true;
    if (loading || resizing)
        return;
    if (wait_timer > 0)
        killTimer(wait_timer);
    wait_timer = startTimer(std::chrono::milliseconds(100));
}

void DocumentWidget::OnFormatingFinished()
{
    formating = false;
    if (loading || resizing)
        return;
    if (wait_timer != 0)
    {
        killTimer(wait_timer);
        wait_timer = 0;
    }
    QApplication::restoreOverrideCursor();
}

void DocumentWidget::OnResizeStarted()
{
    resizing = true;
    if (loading || formating)
        return;
    if (wait_timer > 0)
        killTimer(wait_timer);
    wait_timer = startTimer(std::chrono::milliseconds(500));
}

void DocumentWidget::OnResizeFinished()
{
    resizing = false;
    if (loading || formating)
        return;
    if (wait_timer != 0)
    {
        killTimer(wait_timer);
        wait_timer = 0;
    }
    QApplication::restoreOverrideCursor();
}

#ifdef REMOTE_SOLVER
void DocumentWidget::OnServiceStatus(IOResult result)
{
    emit ServiceStatus(result);
}
#endif

void DocumentWidget::paintEvent(QPaintEvent *event)
{
    const QRect& rect = event->rect();
    QPixmap pixmap;
    window.GetPixmap(pixmap, rect);

    QPainter p(this);
    p.drawPixmap(QPoint(rect.left(), rect.top()), pixmap);
}

void DocumentWidget::resizeEvent(QResizeEvent *event)
{
    if (document)
    {
        if (resize.width() == 0)
        {
            document->Resize(event->size().width(), event->size().height());
            resize = event->size();
            return;
        }
        resize = event->size();
        if (delay_timer != 0)
            killTimer(delay_timer);
        delay_timer = startTimer(std::chrono::milliseconds(500));
    }
}

void DocumentWidget::keyPressEvent(QKeyEvent *event)
{
    QKeySequence s(event->modifiers() | event->key());
    QString str = event->text();
    if (shortcuts_map.Call(s, str.length() > 0 ? str[0] : QChar(), current_editor_state))
        return;

    for (auto ch : str)
    {
        if (!ch.isPrint())
            return;
    }
    if (!str.isEmpty())
        document->InsertString(str.toUtf8().data(), true);
    setFocus();
}

void DocumentWidget::mousePressEvent(QMouseEvent *event)
{
    EditorState s = document->GetEditorState();
    if (event->buttons() == Qt::LeftButton)
    {
        ElementId id;
        if (GetElementAtCoords((int)event->pos().x(), (int)event->pos().y(), id))
        {
            ElementPtr el = document->GetElement(id);
            if (el && el->OnMouseLButtonDown((int)event->pos().x(), (int)event->pos().y()))
            {
                mouse_capture_id = id;
                return;
            }
        }
    }
    if (event->buttons() == Qt::LeftButton || (event->buttons() == Qt::RightButton && s.selection_state.IsEmpty()))
    {
        caret_moving_task_id = document->MoveCaret((int)event->pos().x() + window.document_point.x, (int)event->pos().y() + window.document_point.y, 
            event->modifiers() == Qt::ControlModifier);
    }
    if (event->buttons() == Qt::LeftButton)
        left_click_pos = QPoint{event->pos().x() + window.document_point.x, event->pos().y() + window.document_point.y};
}

void DocumentWidget::mouseReleaseEvent(QMouseEvent *event)
{
    EditorState s = document->GetEditorState();
    if (event->button() == Qt::LeftButton)
    {
        ElementPtr el = document->GetElement(mouse_capture_id);
        if (el && el->OnMouseLButtonUp((int)event->pos().x(), (int)event->pos().y()))
            return;
        ElementId id;
        if (GetElementAtCoords((int)event->pos().x(), (int)event->pos().y(), id))
        {
            el = document->GetElement(id);
            if (el && el->OnMouseLButtonUp((int)event->pos().x(), (int)event->pos().y()))
                return;
        }
    }
}

void DocumentWidget::mouseMoveEvent(QMouseEvent *event)
{
    ElementId id;
    if (!GetElementAtCoords((int)event->pos().x(), (int)event->pos().y(), id))
    {
        setCursor(Qt::ArrowCursor);
        return;
    }

    ElementPtr el = document->GetElement(mouse_capture_id);
    if (el && el->OnMouseMove((int)event->pos().x(), (int)event->pos().y()))
        return;
    el = document->GetElement(id);
    if (el && el->OnMouseMove((int)event->pos().x(), (int)event->pos().y()))
        return;
    
    if (document->IsString(id))
    {
        if (document->GetElementType(id) == ElementType::LINK && event->modifiers() == Qt::ControlModifier)
            setCursor(Qt::PointingHandCursor);
        else
            setCursor(Qt::IBeamCursor);
    }
    else
        setCursor(Qt::ArrowCursor);
    
    if (event->buttons() == Qt::MouseButton::LeftButton)
    {
        //selection with mouse
        document->Select(left_click_pos.x(), left_click_pos.y(), (int)event->pos().x() + window.document_point.x, (int)event->pos().y() + window.document_point.y);
    }
}

void DocumentWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::MouseButton::LeftButton)
        document->SelectOut();
}

void DocumentWidget::wheelEvent(QWheelEvent* event)
{
    QPoint num_pixels = event->pixelDelta() / 8;
    QPoint num_degrees = event->angleDelta() / 8;

    EditorState s = document->GetEditorState();
    ElementId id;
    ElementPtr el;
    if (GetElementAtCoords((int)event->pos().x(), (int)event->pos().y(), id))
        el = document->GetElement(id);

    if (!num_pixels.isNull())
    {
        if (num_pixels.x() != 0)
        {
            if (el && el->OnMouseWheelHorizontal(num_pixels.x()))
                return;
            emit WheelHorizontal(num_pixels.x());
        }
        if (num_pixels.y() != 0)
        {
            if (el && el->OnMouseWheelVertical(num_pixels.y()))
                return;
            emit WheelVertical(num_pixels.y());
        }
    }
    else if (!num_degrees.isNull())
    {
        if (num_degrees.x() != 0)
        {
            if (el && el->OnMouseWheelHorizontal(num_pixels.x()))
                return;
            emit WheelHorizontal(num_degrees.x());
        }
        if (num_degrees.y() != 0)
        {
            if (el && el->OnMouseWheelVertical(num_pixels.y()))
                return;
            emit WheelVertical(num_degrees.y());
        }
    }

    event->accept();
}

void DocumentWidget::focusInEvent(QFocusEvent *event)
{
    if (document)
        document->SetCaretVisible(true);
}

void DocumentWidget::focusOutEvent(QFocusEvent *event)
{
    if (document)
        document->SetCaretVisible(false);
}

void DocumentWidget::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == wait_timer)
    {
        killTimer(wait_timer);
        wait_timer = 0;
        QApplication::setOverrideCursor(Qt::WaitCursor);
    }
    else if (event->timerId() == delay_timer)
    {
        if (document)
            document->Resize(resize.width(), resize.height());
        killTimer(delay_timer);
        delay_timer = 0;
    }
}
