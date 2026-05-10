/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "document_window.h"
#include <QGridLayout>
#include <QClipboard>
#include <QMimeData>
#include <QGuiApplication>
#include <QMenu>
#include <QContextMenuEvent>
#include "set_uint_dialog.h"
#include "set_unit_dialog.h"
#include "mainwindow.h"

//DocumentWindow

DocumentWindow::DocumentWindow(yutovo::Config& _config, QSettings& _settings, QWidget *parent) :
    QWidget(parent),
    config(_config),
    main_window((MainWindow*)parent)
{
    document_widget = new DocumentWidget(this, config, _settings);

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
    connect(&document_widget->window, &QtWindow::DocumentChanged, this, &DocumentWindow::OnDocumentChanged);
    connect(&document_widget->window, &QtWindow::SaveResult, this, &DocumentWindow::OnSaveResult);
    connect(&document_widget->window, &QtWindow::LoadResult, this, &DocumentWindow::OnLoadResult);
    connect(&document_widget->window, &QtWindow::ClipboardCopyResult, this, &DocumentWindow::OnClipboardCopyResult);
    connect(&document_widget->window, &QtWindow::ClipboardPasteResult, this, &DocumentWindow::OnClipboardPasteResult);
    connect(&document_widget->window, &QtWindow::DocumentUpdated, this, &DocumentWindow::OnDocumentUpdated);
    connect(&document_widget->window, &QtWindow::LinkClicked, this, &DocumentWindow::OnLinkClicked);

    CreateMenus();
}

void DocumentWindow::CreateDocument()
{
    document = document_widget->CreateDocument(config);
    document->Start();
    document->InsertCode(false, false);
}

void DocumentWindow::MakeContextMenu(QContextMenuEvent* event)
{
    QMenu menu(this);
    document->WaitTask(document_widget->caret_moving_task_id, 100);

    auto add_copy_paste_menu = 
        [&]()
        {
            EditorState s = document->GetEditorState();
            if (s.caret_state.IsEmpty() || s.caret_state.id.size() == 1)
                return;
            
            bool editable = document->IsEditable(yutovo::GetParent(s.caret_state.id));
            menu.addAction(copy);
            copy->setEnabled(!s.selection_state.IsEmpty());
            menu.addAction(paste);
            QClipboard* clipboard = QGuiApplication::clipboard();
            const QMimeData* mime_data = clipboard->mimeData();
            paste->setEnabled(editable && (mime_data->hasText() || mime_data->hasImage()));

            menu.addAction(cut);
            cut->setEnabled(editable && !s.selection_state.IsEmpty());
        };

    auto add_link_menu = 
        [&]()
        {
            EditorState s = document->GetEditorState();
            if (s.caret_state.IsEmpty())
                return;
            
            auto t = document->GetElementType(s.caret_state.id);
            if (t != ElementType::LINK)
                return;

            if (!menu.isEmpty())
                menu.addSeparator();
            menu.addAction(link);
        };

    auto add_present_as_menu = 
        [&]()
        {
            if (!menu.isEmpty())
                menu.addSeparator();
            QMenu* present_as_menu = menu.addMenu(tr("Present as"));
            present_as_menu->addAction(present_as_auto);
            present_as_menu->addAction(present_as_real);
            present_as_menu->addAction(present_as_integer);
            present_as_menu->addAction(present_as_rational);
            present_as_menu->addAction(present_as_complex);
            present_as_symbolic_menu->addAction(present_as_symbolic_real);
            present_as_symbolic_menu->addAction(present_as_symbolic_rational);
            present_as_symbolic_menu->addAction(present_as_symbolic_complex);
            present_as_menu->addMenu(present_as_symbolic_menu);
            present_as_auto->setChecked(false);
            present_as_real->setChecked(false);
            present_as_integer->setChecked(false);
            present_as_rational->setChecked(false);
            present_as_complex->setChecked(false);
            present_as_symbolic_real->setChecked(false);
            present_as_symbolic_rational->setChecked(false);
            present_as_symbolic_complex->setChecked(false);
        };

    auto add_real_menu = 
        [&](ElementId id)
        {
            AngleMeasure default_angle_measure = document->GetDefaultAngleMeasure(id);
            AngleMeasure result_angle_measure = document->GetResultAngleMeasure(id);

            menu.addSeparator();

            menu.addAction(set_precision);
            menu.addAction(set_exp);

            if (document->HasUnit(id))
                menu.addAction(set_unit);

            QMenu* angle_measure_menu = menu.addMenu(tr("Default angle measure"));
            angle_measure_menu->addAction(default_radian);
            angle_measure_menu->addAction(default_degree);
            angle_measure_menu->addAction(default_grad);

            default_radian->setChecked(default_angle_measure == AngleMeasure::Radian);
            default_degree->setChecked(default_angle_measure == AngleMeasure::Degree);
            default_grad->setChecked(default_angle_measure == AngleMeasure::Grad);

            angle_measure_menu = menu.addMenu(tr("Result angle measure"));
            angle_measure_menu->addAction(result_radian);
            angle_measure_menu->addAction(result_degree);
            angle_measure_menu->addAction(result_grad);

            result_radian->setChecked(result_angle_measure == AngleMeasure::Radian);
            result_degree->setChecked(result_angle_measure == AngleMeasure::Degree);
            result_grad->setChecked(result_angle_measure == AngleMeasure::Grad);
        };
    
    auto add_integer_menu = 
        [&](ElementId id)
        {
            menu.addSeparator();

            auto notation = document->GetDefaultNotation(id);
            QMenu* notation_menu = menu.addMenu(tr("Default notation"));
            notation_menu->addAction(default_binary_notaion);
            default_binary_notaion->setChecked(notation == Notation::Binary);
            notation_menu->addAction(default_octal_notaion);
            default_octal_notaion->setChecked(notation == Notation::Octal);
            notation_menu->addAction(default_decimal_notaion);
            default_decimal_notaion->setChecked(notation == Notation::Decimal);
            notation_menu->addAction(default_hexadecimal_notaion);
            default_hexadecimal_notaion->setChecked(notation == Notation::Hexadecimal);

            notation = document->GetResultNotation(id);
            notation_menu = menu.addMenu(tr("Result notation"));
            notation_menu->addAction(result_binary_notaion);
            result_binary_notaion->setChecked(notation == Notation::Binary);
            notation_menu->addAction(result_octal_notaion);
            result_octal_notaion->setChecked(notation == Notation::Octal);
            notation_menu->addAction(result_decimal_notaion);
            result_decimal_notaion->setChecked(notation == Notation::Decimal);
            notation_menu->addAction(result_hexadecimal_notaion);
            result_hexadecimal_notaion->setChecked(notation == Notation::Hexadecimal);
        };
    
    auto add_rational_menu = 
        [&](ElementId id)
        {
            auto fraction_form = document->GetFractionForm(id);

            menu.addSeparator();
            QMenu* fraction_type_menu = menu.addMenu(tr("Fraction type"));
            fraction_type_menu->addAction(fraction_form_proper);
            fraction_form_proper->setChecked(fraction_form == FractionForm::Proper);
            fraction_type_menu->addAction(fraction_form_improper);
            fraction_form_improper->setChecked(fraction_form == FractionForm::Improper);

            if (document->HasUnit(id))
                menu.addAction(set_unit);
        };

    auto add_complex_menu = 
        [&](ElementId id)
        {
            menu.addSeparator();

            menu.addAction(set_precision);
            menu.addAction(set_exp);
            
            auto complex_form = document->GetComplexForm(id);

            menu.addSeparator();
            QMenu* complex_form_menu = menu.addMenu(tr("Complex form"));
            complex_form_menu->addAction(complex_form_arithmetic);
            complex_form_arithmetic->setChecked(complex_form == ComplexForm::Arithmetic);
            complex_form_menu->addAction(complex_form_trigonometric);
            complex_form_trigonometric->setChecked(complex_form == ComplexForm::Trigonometric);
            complex_form_menu->addAction(complex_form_exponential);
            complex_form_exponential->setChecked(complex_form == ComplexForm::Exponential);

            if (document->HasUnit(id))
                menu.addAction(set_unit);

            AngleMeasure default_angle_measure = document->GetDefaultAngleMeasure(id);
            AngleMeasure result_angle_measure = document->GetResultAngleMeasure(id);

            QMenu* angle_measure_menu = menu.addMenu(tr("Default angle measure"));
            angle_measure_menu->addAction(default_radian);
            angle_measure_menu->addAction(default_degree);
            angle_measure_menu->addAction(default_grad);
            default_radian->setChecked(default_angle_measure == AngleMeasure::Radian);
            default_degree->setChecked(default_angle_measure == AngleMeasure::Degree);
            default_grad->setChecked(default_angle_measure == AngleMeasure::Grad);

            angle_measure_menu = menu.addMenu(tr("Result angle measure"));
            angle_measure_menu->addAction(result_radian);
            angle_measure_menu->addAction(result_degree);
            angle_measure_menu->addAction(result_grad);
            result_radian->setChecked(result_angle_measure == AngleMeasure::Radian);
            result_degree->setChecked(result_angle_measure == AngleMeasure::Degree);
            result_grad->setChecked(result_angle_measure == AngleMeasure::Grad);
        };

    auto add_symbolic_menu =
        [&](ElementId id)
        {
            menu.addSeparator();
            menu.addAction(set_precision);
        };

    auto add_array_real_menu = 
        [&](ElementId id)
        {
            AngleMeasure default_angle_measure = document->GetDefaultAngleMeasure(id);
            AngleMeasure result_angle_measure = document->GetResultAngleMeasure(id);

            menu.addSeparator();

            menu.addAction(set_precision);
            menu.addAction(set_exp);

            if (document->HasUnit(id))
                menu.addAction(set_unit);

            QMenu* angle_measure_menu = menu.addMenu(tr("Default angle measure"));
            angle_measure_menu->addAction(default_radian);
            angle_measure_menu->addAction(default_degree);
            angle_measure_menu->addAction(default_grad);

            default_radian->setChecked(default_angle_measure == AngleMeasure::Radian);
            default_degree->setChecked(default_angle_measure == AngleMeasure::Degree);
            default_grad->setChecked(default_angle_measure == AngleMeasure::Grad);

            angle_measure_menu = menu.addMenu(tr("Result angle measure"));
            angle_measure_menu->addAction(result_radian);
            angle_measure_menu->addAction(result_degree);
            angle_measure_menu->addAction(result_grad);

            result_radian->setChecked(result_angle_measure == AngleMeasure::Radian);
            result_degree->setChecked(result_angle_measure == AngleMeasure::Degree);
            result_grad->setChecked(result_angle_measure == AngleMeasure::Grad);
        };

    auto add_graph_menu = 
        [&]()
        {
            EditorState s = document->GetEditorState();
            if (s.caret_state.IsEmpty())
                return;
            
            auto t = document->GetElementType(yutovo::GetParent(s.caret_state.id));
            if (t != ElementType::GRAPH_LINE)
                return;

            if (!menu.isEmpty())
                menu.addSeparator();
            menu.addAction(graph);
        };

    add_copy_paste_menu();
    add_link_menu();
    add_graph_menu();

    ElementId id = document->FindCurrentParentByType(ElementType::AUTO_RESULT);
    if (!id.empty())
    {
        add_present_as_menu();
        present_as_auto->setChecked(true);

        switch (document->GetResultType(id))
        {
        case ResultType::REAL:
            add_real_menu(id);
            break;
        case ResultType::INTEGER:
            add_integer_menu(id);
            break;
        case ResultType::RATIONAL:
            add_rational_menu(id);
            break;
        case ResultType::COMPLEX:
            add_complex_menu(id);
            break;
        case ResultType::ARRAY_REAL:
            add_array_real_menu(id);
            break;
        case ResultType::SYMBOLIC_REAL:
        case ResultType::SYMBOLIC_RATIONAL:
        case ResultType::SYMBOLIC_COMPLEX:
            add_symbolic_menu(id);
            break;
        }
    }
    else
    {
        id = document->FindCurrentParentByType(ElementType::REAL_RESULT);
        if (!id.empty())
        {
            add_present_as_menu();
            present_as_real->setChecked(true);
            add_real_menu(id);
        }
        else
        {
            id = document->FindCurrentParentByType(ElementType::INTEGER_RESULT);
            if (!id.empty())
            {
                add_present_as_menu();
                present_as_integer->setChecked(true);
                add_integer_menu(id);
            }
            else
            {
                id = document->FindCurrentParentByType(ElementType::RATIONAL_RESULT);
                if (!id.empty())
                {
                    add_present_as_menu();
                    present_as_rational->setChecked(true);
                    add_rational_menu(id);
                }
                else
                {
                    id = document->FindCurrentParentByType(ElementType::COMPLEX_RESULT);
                    if (!id.empty())
                    {
                        add_present_as_menu();
                        present_as_complex->setChecked(true);
                        add_complex_menu(id);
                    }
                    else
                    {
                        id = document->FindCurrentParentByType(ElementType::SYMBOLIC_REAL_RESULT);
                        if (!id.empty())
                        {
                            add_present_as_menu();
                            present_as_symbolic_real->setChecked(true);
                            add_symbolic_menu(id);
                        }
                        else
                        {
                            id = document->FindCurrentParentByType(ElementType::SYMBOLIC_RATIONAL_RESULT);
                            if (!id.empty())
                            {
                                add_present_as_menu();
                                present_as_symbolic_rational->setChecked(true);
                                add_symbolic_menu(id);
                            }
                            else
                            {
                                id = document->FindCurrentParentByType(ElementType::SYMBOLIC_COMPLEX_RESULT);
                                if (!id.empty())
                                {
                                    add_present_as_menu();
                                    present_as_symbolic_complex->setChecked(true);
                                    add_symbolic_menu(id);
                                }
                                else
                                {
                                    id = document->FindCurrentParentByType(ElementType::ARRAY_REAL_RESULT);
                                    if (!id.empty())
                                    {
                                        add_array_real_menu(id);
                                    }
                                }
                            }
                        }
                    }
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

void DocumentWindow::CreateMenus()
{
    //context menu
    if (copy)
        copy->deleteLater();
    copy = new QAction(tr("Copy"), this);
    connect(copy, &QAction::triggered, main_window, &MainWindow::Copy);
    if (paste)
        paste->deleteLater();
    paste = new QAction(tr("Paste"), this);
    connect(paste, &QAction::triggered, main_window, &MainWindow::Paste);
    if (cut)
        cut->deleteLater();
    cut = new QAction(tr("Cut"), this);
    connect(cut, &QAction::triggered, main_window, &MainWindow::Cut);

    if (link)
        link->deleteLater();
    link = new QAction(tr("Link"), this);
    connect(link, &QAction::triggered, main_window, &MainWindow::Link);

    if (present_as_auto)
        present_as_auto->deleteLater();
    present_as_auto = new QAction(tr("Auto"), this);
    present_as_auto->setCheckable(true);
    connect(present_as_auto, &QAction::triggered, this, &DocumentWindow::OnPresentAsAuto);
    if (present_as_real)
        present_as_real->deleteLater();
    present_as_real = new QAction(tr("Real"), this);
    present_as_real->setCheckable(true);
    connect(present_as_real, &QAction::triggered, this, &DocumentWindow::OnPresentAsReal);
    if (present_as_integer)
        present_as_integer->deleteLater();
    present_as_integer = new QAction(tr("Integer"), this);
    present_as_integer->setCheckable(true);
    connect(present_as_integer, &QAction::triggered, this, &DocumentWindow::OnPresentAsInteger);
    if (present_as_rational)
        present_as_rational->deleteLater();
    present_as_rational = new QAction(tr("Rational"), this);
    present_as_rational->setCheckable(true);
    connect(present_as_rational, &QAction::triggered, this, &DocumentWindow::OnPresentAsRational);
    if (present_as_complex)
        present_as_complex->deleteLater();
    present_as_complex = new QAction(tr("Complex"), this);
    present_as_complex->setCheckable(true);
    connect(present_as_complex, &QAction::triggered, this, &DocumentWindow::OnPresentAsComplex);
    if (present_as_symbolic_menu)
        present_as_symbolic_menu->deleteLater();
    present_as_symbolic_menu = new QMenu(tr("Symbolic"), this);
    if (present_as_symbolic_real)
        present_as_symbolic_real->deleteLater();
    present_as_symbolic_real = new QAction(tr("Real"), this);
    present_as_symbolic_real->setCheckable(true);
    connect(present_as_symbolic_real, &QAction::triggered, this, &DocumentWindow::OnPresentAsSymbolicReal);
    if (present_as_symbolic_rational)
        present_as_symbolic_rational->deleteLater();
    present_as_symbolic_rational = new QAction(tr("Rational"), this);
    present_as_symbolic_rational->setCheckable(true);
    connect(present_as_symbolic_rational, &QAction::triggered, this, &DocumentWindow::OnPresentAsSymbolicRational);
    if (present_as_symbolic_complex)
        present_as_symbolic_complex->deleteLater();
    present_as_symbolic_complex = new QAction(tr("Complex"), this);
    present_as_symbolic_complex->setCheckable(true);
    connect(present_as_symbolic_complex, &QAction::triggered, this, &DocumentWindow::OnPresentAsSymbolicComplex);

    if (set_precision)
        set_precision->deleteLater();
    set_precision = new QAction(tr("Set precision"), this);
    connect(set_precision, &QAction::triggered, this, &DocumentWindow::OnSetPrecision);
    if (set_exp)
        set_exp->deleteLater();
    set_exp = new QAction(tr("Set exponential threshold"), this);
    connect(set_exp, &QAction::triggered, this, &DocumentWindow::OnSetExp);
    if (set_unit)
        set_unit->deleteLater();
    set_unit = new QAction(tr("Set unit"), this);
    connect(set_unit, &QAction::triggered, this, &DocumentWindow::OnSetUnit);

    if (default_radian)
        default_radian->deleteLater();
    default_radian = new QAction(tr("Radian"), this);
    default_radian->setCheckable(true);
    connect(default_radian, &QAction::triggered, this, &DocumentWindow::OnDefaultRadian);
    if (default_degree)
        default_degree->deleteLater();
    default_degree = new QAction(tr("Degree"), this);
    default_degree->setCheckable(true);
    connect(default_degree, &QAction::triggered, this, &DocumentWindow::OnDefaultDegree);
    if (default_grad)
        default_grad->deleteLater();
    default_grad = new QAction(tr("Grad"), this);
    default_grad->setCheckable(true);
    connect(default_grad, &QAction::triggered, this, &DocumentWindow::OnDefaultGrad);

    if (result_radian)
        result_radian->deleteLater();
    result_radian = new QAction(tr("Radian"), this);
    result_radian->setCheckable(true);
    connect(result_radian, &QAction::triggered, this, &DocumentWindow::OnResultRadian);
    if (result_degree)
        result_degree->deleteLater();
    result_degree = new QAction(tr("Degree"), this);
    result_degree->setCheckable(true);
    connect(result_degree, &QAction::triggered, this, &DocumentWindow::OnResultDegree);
    if (result_grad)
        result_grad->deleteLater();
    result_grad = new QAction(tr("Grad"), this);
    result_grad->setCheckable(true);
    connect(result_grad, &QAction::triggered, this, &DocumentWindow::OnResultGrad);

    if (default_binary_notaion)
        default_binary_notaion->deleteLater();
    default_binary_notaion = new QAction(tr("Binary"), this);
    default_binary_notaion->setCheckable(true);
    connect(default_binary_notaion, &QAction::triggered, this, &DocumentWindow::OnDefaultBinaryNotation);
    if (default_octal_notaion)
        default_octal_notaion->deleteLater();
    default_octal_notaion = new QAction(tr("Octal"), this);
    default_octal_notaion->setCheckable(true);
    connect(default_octal_notaion, &QAction::triggered, this, &DocumentWindow::OnDefaultOctalNotation);
    if (default_decimal_notaion)
        default_decimal_notaion->deleteLater();
    default_decimal_notaion = new QAction(tr("Decimal"), this);
    default_decimal_notaion->setCheckable(true);
    connect(default_decimal_notaion, &QAction::triggered, this, &DocumentWindow::OnDefaultDecimalNotation);
    if (default_hexadecimal_notaion)
        default_hexadecimal_notaion->deleteLater();
    default_hexadecimal_notaion = new QAction(tr("Hexadecimal"), this);
    default_hexadecimal_notaion->setCheckable(true);
    connect(default_hexadecimal_notaion, &QAction::triggered, this, &DocumentWindow::OnDefaultHexadecimalNotation);

    if (result_binary_notaion)
        result_binary_notaion->deleteLater();
    result_binary_notaion = new QAction(tr("Binary"), this);
    result_binary_notaion->setCheckable(true);
    connect(result_binary_notaion, &QAction::triggered, this, &DocumentWindow::OnResultBinaryNotation);
    if (result_octal_notaion)
        result_octal_notaion->deleteLater();
    result_octal_notaion = new QAction(tr("Octal"), this);
    result_octal_notaion->setCheckable(true);
    connect(result_octal_notaion, &QAction::triggered, this, &DocumentWindow::OnResultOctalNotation);
    if (result_decimal_notaion)
        result_decimal_notaion->deleteLater();
    result_decimal_notaion = new QAction(tr("Decimal"), this);
    result_decimal_notaion->setCheckable(true);
    connect(result_decimal_notaion, &QAction::triggered, this, &DocumentWindow::OnResultDecimalNotation);
    if (result_hexadecimal_notaion)
        result_hexadecimal_notaion->deleteLater();
    result_hexadecimal_notaion = new QAction(tr("Hexadecimal"), this);
    result_hexadecimal_notaion->setCheckable(true);
    connect(result_hexadecimal_notaion, &QAction::triggered, this, &DocumentWindow::OnResultHexadecimalNotation);

    if (fraction_form_proper)
        fraction_form_proper->deleteLater();
    fraction_form_proper = new QAction(tr("Proper"), this);
    fraction_form_proper->setCheckable(true);
    connect(fraction_form_proper, &QAction::triggered, this, &DocumentWindow::OnFractionFormProper);
    if (fraction_form_improper)
        fraction_form_improper->deleteLater();
    fraction_form_improper = new QAction(tr("Improper"), this);
    fraction_form_improper->setCheckable(true);
    connect(fraction_form_improper, &QAction::triggered, this, &DocumentWindow::OnFractionFormImproper);

    if (complex_form_arithmetic)
        complex_form_arithmetic->deleteLater();
    complex_form_arithmetic = new QAction(tr("Arithmetic"), this);
    complex_form_arithmetic->setCheckable(true);
    connect(complex_form_arithmetic, &QAction::triggered, this, &DocumentWindow::OnComplexFormArithmetic);
    if (complex_form_trigonometric)
        complex_form_trigonometric->deleteLater();
    complex_form_trigonometric = new QAction(tr("Trigonometric"), this);
    complex_form_trigonometric->setCheckable(true);
    connect(complex_form_trigonometric, &QAction::triggered, this, &DocumentWindow::OnComplexFormTrigonometric);
    if (complex_form_exponential)
        complex_form_exponential->deleteLater();
    complex_form_exponential = new QAction(tr("Exponential"), this);
    complex_form_exponential->setCheckable(true);
    connect(complex_form_exponential, &QAction::triggered, this, &DocumentWindow::OnComplexFormExponential);

    if (graph)
        graph->deleteLater();
    graph = new QAction(tr("Graph"), this);
    connect(graph, &QAction::triggered, main_window, &MainWindow::Graph);
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
    vertical_scroll->setSliderPosition(vertical_scroll->sliderPosition() - value * 2);
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

void DocumentWindow::OnDocumentChanged(const bool changed)
{
    emit DocumentChanged(changed);
}

void DocumentWindow::OnLinkClicked(const ElementId& id, const std::u32string& url)
{
    emit LinkClicked(url);
}

void DocumentWindow::contextMenuEvent(QContextMenuEvent* event)
{
    MakeContextMenu(event);
}

void DocumentWindow::OnPresentAsAuto()
{
    document->SetResultType(document_widget->current_editor_state.caret_state.id, ResultType::AUTO, true);
}

void DocumentWindow::OnPresentAsReal()
{
    document->SetResultType(document_widget->current_editor_state.caret_state.id, ResultType::REAL, true);
}

void DocumentWindow::OnPresentAsInteger()
{
    document->SetResultType(document_widget->current_editor_state.caret_state.id, ResultType::INTEGER, true);
}

void DocumentWindow::OnPresentAsRational()
{
    document->SetResultType(document_widget->current_editor_state.caret_state.id, ResultType::RATIONAL, true);
}

void DocumentWindow::OnPresentAsComplex()
{
    document->SetResultType(document_widget->current_editor_state.caret_state.id, ResultType::COMPLEX, true);
}

void DocumentWindow::OnPresentAsSymbolicReal()
{
    document->SetResultType(document_widget->current_editor_state.caret_state.id, ResultType::SYMBOLIC_REAL, true);
}

void DocumentWindow::OnPresentAsSymbolicRational()
{
    document->SetResultType(document_widget->current_editor_state.caret_state.id, ResultType::SYMBOLIC_RATIONAL, true);
}

void DocumentWindow::OnPresentAsSymbolicComplex()
{
    document->SetResultType(document_widget->current_editor_state.caret_state.id, ResultType::SYMBOLIC_COMPLEX, true);
}

void DocumentWindow::OnSetPrecision()
{
    ElementId id = document->FindCurrentParentByType(ElementType::AUTO_RESULT);
    if (id.empty())
        id = document->FindCurrentParentByType(ElementType::REAL_RESULT);
    if (id.empty())
        id = document->FindCurrentParentByType(ElementType::COMPLEX_RESULT);
    if (id.empty())
        id = document->FindCurrentParentByType(ElementType::SYMBOLIC_REAL_RESULT);
    if (id.empty())
        id = document->FindCurrentParentByType(ElementType::SYMBOLIC_RATIONAL_RESULT);
    if (id.empty())
        id = document->FindCurrentParentByType(ElementType::SYMBOLIC_COMPLEX_RESULT);
    if (id.empty())
        return;
    
    int precision = document->GetPrecision(id);
    SetUintDialog dialog(precision, tr("Set precision"), tr("Precision"));
    if (!dialog.exec())
        return;
    document->SetPrecision(document_widget->current_editor_state.caret_state.id, dialog.value, true);
}

void DocumentWindow::OnSetExp()
{
    ElementId id = document->FindCurrentParentByType(ElementType::AUTO_RESULT);
    if (id.empty())
        id = document->FindCurrentParentByType(ElementType::REAL_RESULT);
    if (id.empty())
        id = document->FindCurrentParentByType(ElementType::COMPLEX_RESULT);
    if (id.empty())
        return;
    
    int exp = document->GetExp(id);
    SetUintDialog dialog(exp, tr("Set exponential threshold"), tr("Exponential threshold"));
    if (!dialog.exec())
        return;
    document->SetExp(document_widget->current_editor_state.caret_state.id, dialog.value, true);
}

void DocumentWindow::OnSetUnit()
{
    ElementId id = document->FindCurrentParentByType(ElementType::AUTO_RESULT);
    if (id.empty())
        id = document->FindCurrentParentByType(ElementType::REAL_RESULT);
    if (id.empty())
        id = document->FindCurrentParentByType(ElementType::RATIONAL_RESULT);
    if (id.empty())
        return;
    if (!document->HasUnit(id))
        return;
    
    std::vector<yutovo_calculator::Unit> cast_units;
    document->GetCastUnits(id, cast_units);

    SetUnitDialog dialog(cast_units, config);
    if (!dialog.exec())
        return;
    
    document->SetUnit(document_widget->current_editor_state.caret_state.id, dialog.value, true);
}

void DocumentWindow::OnDefaultRadian()
{
    ElementId id = GetResultId();
    if (id.empty())
        return;
    AngleMeasure m = document->GetResultAngleMeasure(id);
    document->SetAngleMeasure(document_widget->current_editor_state.caret_state.id, AngleMeasure::Radian, m, true);
}

void DocumentWindow::OnDefaultDegree()
{
    ElementId id = GetResultId();
    if (id.empty())
        return;
    AngleMeasure m = document->GetResultAngleMeasure(id);
    document->SetAngleMeasure(document_widget->current_editor_state.caret_state.id, AngleMeasure::Degree, m, true);
}

void DocumentWindow::OnDefaultGrad()
{
    ElementId id = GetResultId();
    if (id.empty())
        return;
    AngleMeasure m = document->GetResultAngleMeasure(id);
    document->SetAngleMeasure(document_widget->current_editor_state.caret_state.id, AngleMeasure::Grad, m, true);
}

void DocumentWindow::OnResultRadian()
{
    ElementId id = GetResultId();
    if (id.empty())
        return;
    AngleMeasure m = document->GetDefaultAngleMeasure(id);
    document->SetAngleMeasure(document_widget->current_editor_state.caret_state.id, m, AngleMeasure::Radian, true);
}

void DocumentWindow::OnResultDegree()
{
    ElementId id = GetResultId();
    if (id.empty())
        return;
    AngleMeasure m = document->GetDefaultAngleMeasure(id);
    document->SetAngleMeasure(document_widget->current_editor_state.caret_state.id, m, AngleMeasure::Degree, true);
}

void DocumentWindow::OnResultGrad()
{
    ElementId id = GetResultId();
    if (id.empty())
        return;
    AngleMeasure m = document->GetDefaultAngleMeasure(id);
    document->SetAngleMeasure(document_widget->current_editor_state.caret_state.id, m, AngleMeasure::Grad, true);
}

void DocumentWindow::OnDefaultBinaryNotation()
{
    auto _id = document->GetParentId(document_widget->current_editor_state.caret_state.id, ElementType::INTEGER_RESULT);
    if (_id.empty())
        return;
    Notation n = document->GetResultNotation(_id);
    document->SetNotation(document_widget->current_editor_state.caret_state.id, Notation::Binary, n, true);
}

void DocumentWindow::OnDefaultOctalNotation()
{
    auto _id = document->GetParentId(document_widget->current_editor_state.caret_state.id, ElementType::INTEGER_RESULT);
    if (_id.empty())
        return;
    Notation n = document->GetResultNotation(_id);
    document->SetNotation(document_widget->current_editor_state.caret_state.id, Notation::Octal, n, true);
}

void DocumentWindow::OnDefaultDecimalNotation()
{
    auto _id = document->GetParentId(document_widget->current_editor_state.caret_state.id, ElementType::INTEGER_RESULT);
    if (_id.empty())
        return;
    Notation n = document->GetResultNotation(_id);
    document->SetNotation(document_widget->current_editor_state.caret_state.id, Notation::Decimal, n, true);
}

void DocumentWindow::OnDefaultHexadecimalNotation()
{
    auto _id = document->GetParentId(document_widget->current_editor_state.caret_state.id, ElementType::INTEGER_RESULT);
    if (_id.empty())
        return;
    Notation n = document->GetResultNotation(_id);
    document->SetNotation(document_widget->current_editor_state.caret_state.id, Notation::Hexadecimal, n, true);
}

void DocumentWindow::OnResultBinaryNotation()
{
    auto _id = document->GetParentId(document_widget->current_editor_state.caret_state.id, ElementType::INTEGER_RESULT);
    if (_id.empty())
        return;
    Notation n = document->GetDefaultNotation(_id);
    document->SetNotation(document_widget->current_editor_state.caret_state.id, n, Notation::Binary, true);
}

void DocumentWindow::OnResultOctalNotation()
{
    auto _id = document->GetParentId(document_widget->current_editor_state.caret_state.id, ElementType::INTEGER_RESULT);
    if (_id.empty())
        return;
    Notation n = document->GetDefaultNotation(_id);
    document->SetNotation(document_widget->current_editor_state.caret_state.id, n, Notation::Octal, true);
}

void DocumentWindow::OnResultDecimalNotation()
{
    auto _id = document->GetParentId(document_widget->current_editor_state.caret_state.id, ElementType::INTEGER_RESULT);
    if (_id.empty())
        return;
    Notation n = document->GetDefaultNotation(_id);
    document->SetNotation(document_widget->current_editor_state.caret_state.id, n, Notation::Decimal, true);
}

void DocumentWindow::OnResultHexadecimalNotation()
{
    auto _id = document->GetParentId(document_widget->current_editor_state.caret_state.id, ElementType::INTEGER_RESULT);
    if (_id.empty())
        return;
    Notation n = document->GetDefaultNotation(_id);
    document->SetNotation(document_widget->current_editor_state.caret_state.id, n, Notation::Hexadecimal, true);
}

void DocumentWindow::OnFractionFormProper()
{
    document->SetFractionForm(document_widget->current_editor_state.caret_state.id, FractionForm::Proper, true);
}

void DocumentWindow::OnFractionFormImproper()
{
    document->SetFractionForm(document_widget->current_editor_state.caret_state.id, FractionForm::Improper, true);
}

void DocumentWindow::OnComplexFormArithmetic()
{
    document->SetComplexForm(document_widget->current_editor_state.caret_state.id, ComplexForm::Arithmetic, true);
}

void DocumentWindow::OnComplexFormTrigonometric()
{
    document->SetComplexForm(document_widget->current_editor_state.caret_state.id, ComplexForm::Trigonometric, true);
}

void DocumentWindow::OnComplexFormExponential()
{
    document->SetComplexForm(document_widget->current_editor_state.caret_state.id, ComplexForm::Exponential, true);
}

ElementId DocumentWindow::GetResultId()
{
    ElementId id = document->FindCurrentParentByType(ElementType::AUTO_RESULT);
    if (id.empty())
        id = document->FindCurrentParentByType(ElementType::REAL_RESULT);
    if (id.empty())
        id = document->FindCurrentParentByType(ElementType::COMPLEX_RESULT);
    return id;
}
