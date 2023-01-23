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
    window(this)
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
}

void DocumentWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
        document->MoveCaret((int)event->pos().x() + window.document_point.x, (int)event->pos().y() + window.document_point.y);
}

void DocumentWidget::mouseMoveEvent(QMouseEvent *event)
{
    ElementId id;
    if (!GetElementAtCoords((int)event->pos().x(), (int)event->pos().y(), id))
    {
        setCursor(Qt::ArrowCursor);
        return;
    }
    if (document->GetElementType(id) == ElementType::STRING || document->GetElementType(id) == ElementType::CODE_STRING)
        setCursor(Qt::IBeamCursor);
    else
        setCursor(Qt::ArrowCursor);
}
