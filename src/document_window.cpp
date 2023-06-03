#include "document_window.h"
#include <QGridLayout>
#include <QClipboard>
#include <QMimeData>
#include <QGuiApplication>
#include <QMenu>
 #include <QContextMenuEvent>

//DocumentWindow

DocumentWindow::DocumentWindow(yutovo::Config& _config, QWidget *parent) :
    QWidget(parent),
    config(_config)
{
    document_widget = new DocumentWidget(this);
    document = document_widget->CreateDocument();
    document->Start(config);

    document_widget->setObjectName(QStringLiteral("document_widget"));

    QGridLayout* grid_layout = new QGridLayout();
    setLayout(grid_layout);
    vertical_scroll = new QScrollBar(Qt::Vertical);
    horizontal_scroll = new QScrollBar(Qt::Horizontal);
    grid_layout->addWidget(document_widget, 0, 0);
    grid_layout->addWidget(vertical_scroll, 0, 1);
    grid_layout->addWidget(horizontal_scroll, 1, 0);

    connect(vertical_scroll, &QAbstractSlider::valueChanged, this, &DocumentWindow::OnVerticalValueChanged);
    connect(horizontal_scroll, &QAbstractSlider::valueChanged, this, &DocumentWindow::OnHorizontalValueChanged);
    vertical_scroll->setMinimum(0);
    vertical_scroll->setSingleStep(10);
    horizontal_scroll->setMinimum(0);
    horizontal_scroll->setSingleStep(10);

    connect(document_widget, &DocumentWidget::WheelVertical, this, &DocumentWindow::OnWheelVertical);
    connect(document_widget, &DocumentWidget::WheelHorizontal, this, &DocumentWindow::OnWheelHorizontal);

    connect(&document_widget->window, &QtWindow::CaretMoved, this, &DocumentWindow::OnCaretMoved);
    connect(&document_widget->window, &QtWindow::SaveResult, this, &DocumentWindow::OnSaveResult);
    connect(&document_widget->window, &QtWindow::LoadResult, this, &DocumentWindow::OnLoadResult);
    connect(&document_widget->window, &QtWindow::ClipboardCopyResult, this, &DocumentWindow::OnClipboardCopyResult);
    connect(&document_widget->window, &QtWindow::ClipboardPasteResult, this, &DocumentWindow::OnClipboardPasteResult);

    connect(&document_widget->window, &QtWindow::DocumentUpdated, this, &DocumentWindow::OnDocumentUpdated);

    //context menu
    present_as_auto = new QAction(tr("Present as Auto"), this);
    connect(present_as_auto, &QAction::triggered, this, &DocumentWindow::OnPresentAsAuto);
    present_as_real = new QAction(tr("Present as Real"), this);
    connect(present_as_real, &QAction::triggered, this, &DocumentWindow::OnPresentAsReal);
    present_as_integer = new QAction(tr("Present as Intger"), this);
    connect(present_as_integer, &QAction::triggered, this, &DocumentWindow::OnPresentAsInteger);
    present_as_rational = new QAction(tr("Present as Rational"), this);
    connect(present_as_rational, &QAction::triggered, this, &DocumentWindow::OnPresentAsRational);
}

void DocumentWindow::MakeContextMenu(QContextMenuEvent* event)
{
    QMenu menu(this);
    document->WaitTask(document_widget->caret_moving_task_id, 100);
    ElementId id = document->FindCurrentParentByType(ElementType::AUTO_RESULT);
    if (!id.empty())
    {
        menu.addAction(present_as_real);
        menu.addAction(present_as_integer);
        menu.addAction(present_as_rational);
    }
    else
    {
        id = document->FindCurrentParentByType(ElementType::REAL_RESULT);
        if (!id.empty())
        {
            menu.addAction(present_as_auto);
            menu.addAction(present_as_integer);
            menu.addAction(present_as_rational);
        }
        else
        {
            id = document->FindCurrentParentByType(ElementType::INTEGER_RESULT);
            if (!id.empty())
            {
                menu.addAction(present_as_auto);
                menu.addAction(present_as_real);
                menu.addAction(present_as_rational);
            }
            else
            {
                id = document->FindCurrentParentByType(ElementType::RATIONAL_RESULT);
                if (!id.empty())
                {
                    menu.addAction(present_as_auto);
                    menu.addAction(present_as_real);
                    menu.addAction(present_as_integer);
                }
            }
        }
    }
    
    if (!menu.isEmpty())
        menu.exec(event->globalPos());
}

void DocumentWindow::SetFocus()
{
    document_widget->setFocus();
}

void DocumentWindow::OnVerticalValueChanged(int value)
{
    document_widget->window.document_point.y = (value > 0 ? value : 0);
    document->Redraw();
}

void DocumentWindow::OnHorizontalValueChanged(int value)
{
    document_widget->window.document_point.x = (value > 0 ? value : 0);
    document->Redraw();
}

void DocumentWindow::OnWheelVertical(const int value)
{
    vertical_scroll->setSliderPosition(vertical_scroll->sliderPosition() - value);
}

void DocumentWindow::OnWheelHorizontal(const int value)
{
    horizontal_scroll->setSliderPosition(horizontal_scroll->sliderPosition() - value);
}

void DocumentWindow::OnCaretMoved(const EditorState editor_state)
{
    emit CaretMoved(editor_state);
}

void DocumentWindow::OnSaveResult(const uint task_id, IOResult result)
{
    emit SaveResult(task_id, result);
}

void DocumentWindow::OnLoadResult(const uint task_id, IOResult result)
{
    emit LoadResult(task_id, result);
}

void DocumentWindow::OnClipboardCopyResult(CopyResult result)
{
    emit ClipboardCopyResult(result);
}

void DocumentWindow::OnClipboardPasteResult(PasteResult result)
{
    emit ClipboardPasteResult(result);
}

void DocumentWindow::OnDocumentUpdated(const Rect rect)
{
    Rect r = document_widget->window.GetViewPort(0);
    Size& s = document_widget->window.document_size;
    Point& p = document_widget->window.document_point;
    
    vertical_scroll->setMinimum(0);
    vertical_scroll->setMaximum(s.height - r.height > 0 ? s.height - r.height : 1);
    vertical_scroll->setPageStep(r.height);
    vertical_scroll->setValue(p.y);

    horizontal_scroll->setMinimum(0);
    horizontal_scroll->setMaximum(s.width - r.width > 0 ? s.width - r.width : 1);
    horizontal_scroll->setPageStep(r.width);
    horizontal_scroll->setValue(p.x);
}

void DocumentWindow::OnPresentAsAuto()
{
    document->SetResult(document_widget->current_editor_state.caret_state.id, ResultType::AUTO);
}

void DocumentWindow::OnPresentAsReal()
{
    document->SetResult(document_widget->current_editor_state.caret_state.id, ResultType::REAL);
}

void DocumentWindow::OnPresentAsInteger()
{
    document->SetResult(document_widget->current_editor_state.caret_state.id, ResultType::INTEGER);
}

void DocumentWindow::OnPresentAsRational()
{
    document->SetResult(document_widget->current_editor_state.caret_state.id, ResultType::RATIONAL);
}
