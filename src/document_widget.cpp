/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
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
#include "plot_format_dialog.h"

//DocumentWidget

DocumentWidget::DocumentWidget(QWidget *parent, yutovo::Config& _config, QSettings& _settings) :
    QWidget(parent),
    window(size().width(), size().height(), _config),
    prompt_form(this),
    settings(_settings),
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

    connect(&prompt_form, &PromptForm::PromptActivated, this, &DocumentWidget::OnPromptActivated);

    prompt_form.hide();

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

void DocumentWidget::OnNextEditorTab()
{
    emit NextEditorTab();
}

void DocumentWidget::OnPrevEditorTab()
{
    emit PrevEditorTab();
}

void DocumentWidget::OnPrompt()
{
    prompt_form.hide();

    std::vector<std::pair<IdentifierType, std::string>> prompt;
    document->GetPrompt(prompt);
    if (prompt.empty())
        return;
    
    Rect r;
    if (!document->GetCaretRect(r))
        return;
    prompt_form.move(r.GetRight(), r.GetBottom());
    prompt_form.Fill(std::move(prompt));
    prompt_form.show();
}

void DocumentWidget::OnDocumentUpdated(const Rect rect)
{
    update(rect.left, rect.top, rect.width, rect.height);
}

void DocumentWidget::OnCaretMoved(const EditorState editor_state)
{
    current_editor_state = editor_state;
    if (show_prompt)
    {
        OnPrompt();
        show_prompt = false;
    }
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

void DocumentWidget::OnPromptActivated(const IdentifierType id_type, const std::string& id_str)
{
    using InsertFunc = uint (Document::*)(bool, bool);
    static const std::map<std::string, InsertFunc> insert_map = 
    {
        { "plus", &Document::InsertPlus },
        { "minus", &Document::InsertMinus },
        { "mul", &Document::InsertMultiply },
        { "div", &Document::InsertDivision },
        { "power", &Document::InsertPower },
        { "root", &Document::InsertNthRoot },
        { "sqrt", &Document::InsertSquareRoot },
        { "sub", &Document::InsertSubscript },
        { "sum", &Document::InsertSum },
        { "prod", &Document::InsertProduct }
    };

    show_prompt = false;
    auto it = insert_map.find(id_str);
    if (it == insert_map.end())
        document->ReplaceString(ToUtfString(id_str), true);
    else
    {
        InsertFunc func = it->second;
        ((document.get())->*func)(true, true);
    }
}

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
    if (prompt_form.isVisible())
    {
        if (event->type() == QEvent::KeyPress)
        {
            const int count = prompt_form.count();
            if (count == 0)
                return;
            int row = prompt_form.currentRow();
            int page = prompt_form.viewport()->height() / prompt_form.sizeHintForRow(0);
            if (page <= 0)
                page = 1;

            auto* key = static_cast<QKeyEvent*>(event);
            switch (key->key())
            {
            case Qt::Key_Down:
                prompt_form.setCurrentRow((prompt_form.currentRow() + 1) % prompt_form.count());
                return;
            case Qt::Key_Up:
                prompt_form.setCurrentRow((prompt_form.currentRow() - 1 + prompt_form.count()) % prompt_form.count());
                return;
            case Qt::Key_PageDown:
                prompt_form.setCurrentRow(qMin(row + page, count - 1));
                return;
            case Qt::Key_PageUp:
                prompt_form.setCurrentRow(qMax(row - page, 0));
                return;                
            case Qt::Key_Return:
            case Qt::Key_Enter:
                prompt_form.ActivateCurrentItem();
                return;
            case Qt::Key_Escape:
                prompt_form.hide();
                return;
            }
        }
    }

    QKeySequence s(event->modifiers() | event->key());
    QString str = event->text();
    if (shortcuts_map.Call(s, str.length() > 0 ? str[0] : QChar(), current_editor_state))
    {
        if (prompt_form.isVisible())
            show_prompt = true;
        return;
    }

    for (auto ch : str)
    {
        if (!ch.isPrint())
            return;
    }
    if (!str.isEmpty())
    {
        document->InsertString(str.toUtf8().data(), true);
        if (prompt_form.isVisible() || document->config.auto_prompt)
            show_prompt = true;
    }
    else
        prompt_form.hide();
    setFocus();
}

void DocumentWidget::mousePressEvent(QMouseEvent *event)
{
    ElementId id;
    int x = (int)event->pos().x() + window.document_point.x;
    int y = (int)event->pos().y() + window.document_point.y;
    int m = document->config.resize_margin_width;
    if (document->GetElementAtCoords(x, y, m, id))
    {
        if (document->IsResizable(id))
        {
            Rect rect;
            if (document->GetElementRect(id, rect))
            {
                if ((x <= rect.left + m && y <= rect.top + m) || (x >= rect.GetRight() - m && y >= rect.GetBottom() - m))
                    mouse_capture_id = id;
                else if ((x >= rect.GetRight() - m && y <= rect.top + m) || (x <= rect.left + m && y >= rect.GetBottom() - m))
                    mouse_capture_id = id;
                else if (x <= rect.left + m || (x <= rect.GetRight() + m && x >= rect.GetRight() - m))
                    mouse_capture_id = id;
                else if (y <= rect.top + m || (y <= rect.GetBottom() + m && y >= rect.GetBottom() - m))
                    mouse_capture_id = id;
            }
        }
    }

    EditorState s = document->GetEditorState();
    if (event->buttons() == Qt::LeftButton)
    {
        MouseHoldType hold_type;
        ElementId hold_id;
        if (document->MouseLButtonDown(x, y, hold_type, hold_id))
        {
            switch (hold_type)
            {
            case MouseHoldType::PLOT_FORMAT_DIALOG:
                {
                    yutovo::PlotFormat f;
                    if (!document->GetPlotFormat(hold_id, f))
                        return;
                    PlotFormatDialog dialog(f);
                    if (!dialog.exec())
                        return;
                    document->SetPlotFormat(hold_id, f, true);
                }
                return;
            default:
                return;
            }
        }

        int c = settings.value("Documents/click_link", 0).toInt();
        caret_moving_task_id = document->MoveCaret(x, y, ((event->modifiers() == Qt::ControlModifier && c == 1) || 
            (event->modifiers() == 0 && c == 0)));
        left_click_pos = QPoint{x, y};
    }
}

void DocumentWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mouse_capture_id = ElementId{};
    EditorState s = document->GetEditorState();
    if (event->button() == Qt::LeftButton)
        document->MouseLButtonUp((int)event->pos().x() + window.document_point.x, (int)event->pos().y() + window.document_point.y);
}

void DocumentWidget::mouseMoveEvent(QMouseEvent *event)
{
    ElementId id;
    int x = (int)event->pos().x() + window.document_point.x;
    int y = (int)event->pos().y() + window.document_point.y;
    int m = document->config.resize_margin_width;
    if (document->GetElementAtCoords(x, y, m, id))
    {
        if (document->IsResizable(id))
        {
            Rect rect;
            if (document->GetElementRect(id, rect))
            {
                if ((x <= rect.left + m && y <= rect.top + m) || (x >= rect.GetRight() - m && y >= rect.GetBottom() - m))
                {
                    if (mouse_capture_id == ElementId{})
                        setCursor(Qt::SizeFDiagCursor);
                    document->MouseMove(x, y);
                    return;
                }
                if ((x >= rect.GetRight() - m && y <= rect.top + m) || (x <= rect.left + m && y >= rect.GetBottom() - m))
                {
                    if (mouse_capture_id == ElementId{})
                        setCursor(Qt::SizeBDiagCursor);
                    document->MouseMove(x, y);
                    return;
                }
                if (x <= rect.left + m || (x <= rect.GetRight() + m && x >= rect.GetRight() - m))
                {
                    if (mouse_capture_id == ElementId{})
                        setCursor(Qt::SizeHorCursor);
                    document->MouseMove(x, y);
                    return;
                }
                if (y <= rect.top + m || (y <= rect.GetBottom() + m && y >= rect.GetBottom() - m))
                {
                    if (mouse_capture_id == ElementId{})
                        setCursor(Qt::SizeVerCursor);
                    document->MouseMove(x, y);
                    return;
                }
            }
        }
    }

    if (!document->GetElementAtCoords(x, y, 0, id))
    {
        if (mouse_capture_id == ElementId{})
            setCursor(Qt::ArrowCursor);
        return;
    }

    if (document->MouseMove(x, y))
        return;
    
    if (document->IsString(id))
    {
        int c = settings.value("Documents/click_link", 0).toInt();
        if (document->GetElementType(id) == ElementType::LINK && ((event->modifiers() == Qt::ControlModifier && c == 1) || 
            (event->modifiers() == 0 && c == 0)))
        {
            setCursor(Qt::PointingHandCursor);
        }
        else
            setCursor(Qt::IBeamCursor);
    }
    else
        setCursor(Qt::ArrowCursor);
    
    if (event->buttons() == Qt::MouseButton::LeftButton)
    {
        //selection with mouse
        document->Select(left_click_pos.x(), left_click_pos.y(), x, y);
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

    if (event->modifiers() == Qt::ControlModifier)
    {
        Config c;
        document->GetConfig(c);
        if (num_degrees.y() > 0)
            c.scale /= 0.9;
        else
            c.scale *= 0.9;
        if (c.scale >= 0.5 && c.scale <= 5)
        {
            document->SetConfig(c, false);
            emit ScaleChanged(c.scale);
        }
        event->accept();
        return;
    }

    if (document->MouseWheel((int)event->pos().x() + window.document_point.x, (int)event->pos().y() + window.document_point.y, 
        yutovo::Point{num_pixels.x(), num_pixels.y()}, yutovo::Point{num_degrees.x(), num_degrees.y()}))
    {
        event->accept();
        return;
    }

    if (!num_pixels.isNull())
    {
        if (num_pixels.x() != 0)
            emit WheelHorizontal(num_pixels.x());
        if (num_pixels.y() != 0)
            emit WheelVertical(num_pixels.y());
    }
    else if (!num_degrees.isNull())
    {
        if (num_degrees.x() != 0)
            emit WheelHorizontal(num_degrees.x());
        if (num_degrees.y() != 0)
            emit WheelVertical(num_degrees.y());
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
