#include "document_widget.h"
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QGuiApplication>
#include <QCursor>
#include "mainwindow.h"

//DocumentWidget

DocumentWidget::DocumentWidget(QWidget *parent) :
    QWidget(parent),
    window(size().width(), size().height())
{
    connect(&window, &QtWindow::DocumentUpdated, this, &DocumentWidget::OnDocumentUpdated);
    connect(&window, &QtWindow::WindowUpdated, this, &DocumentWidget::OnWindowUpdated);
    connect(&window, &QtWindow::CaretMoved, this, &DocumentWidget::OnCaretMoved);

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

DocumentPtr DocumentWidget::CreateDocument()
{
    document.reset(new Document(&window));
    shortcuts_map.Init(document);
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

void DocumentWidget::OnDocumentUpdated(const Rect rect)
{
    update(rect.left, rect.top, rect.width, rect.height);
}

void DocumentWidget::OnWindowUpdated()
{
    update(rect());
    document->Redraw();
}

void DocumentWidget::OnCaretMoved(const EditorState editor_state)
{
    current_editor_state = editor_state;
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
    document->Resize(event->size().width(), event->size().height());
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
    if (event->buttons() == Qt::LeftButton || event->buttons() == Qt::RightButton)
        caret_moving_task_id = document->MoveCaret((int)event->pos().x() + window.document_point.x, (int)event->pos().y() + window.document_point.y);
}

void DocumentWidget::mouseMoveEvent(QMouseEvent *event)
{
    ElementId id;
    if (!GetElementAtCoords((int)event->pos().x(), (int)event->pos().y(), id))
    {
        setCursor(Qt::ArrowCursor);
        return;
    }
    if (document->IsString(id))
        setCursor(Qt::IBeamCursor);
    else
        setCursor(Qt::ArrowCursor);
}

void DocumentWidget::wheelEvent(QWheelEvent* event)
{
    QPoint num_pixels = event->pixelDelta() / 8;
    QPoint num_degrees = event->angleDelta() / 8;

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
