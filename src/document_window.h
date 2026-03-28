/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __DOCUMENT_WINDOW_H__
#define __DOCUMENT_WINDOW_H__

#include <QScrollBar>
#include "document_widget.h"

class MainWindow;

class DocumentWindow : public QWidget
{
    Q_OBJECT

public:
    DocumentWindow(yutovo::Config& _config, QSettings& _settings, QWidget* parent);

    void CreateDocument();

    void MakeContextMenu(QContextMenuEvent* event);

    void SetFocus();

    void CreateMenus();

private slots:
    void OnVerticalValueChanged(int value);
    void OnHorizontalValueChanged(int value);

    void OnWheelVertical(const int value);
    void OnWheelHorizontal(const int value);

    void OnCaretMoved(const EditorState editor_state);
    void OnSaveResult(const uint task_id, IOResult result);
    void OnLoadResult(const uint task_id, IOResult result);
    void OnClipboardCopyResult(CopyResult result);
    void OnClipboardPasteResult(PasteResult result);
    void OnDocumentUpdated(const Rect rect);
    void OnDocumentChanged(const bool changed);
    void OnLinkClicked(const ElementId& id, const std::u32string& url);

    void OnPresentAsAuto();
    void OnPresentAsReal();
    void OnPresentAsInteger();
    void OnPresentAsRational();
    void OnPresentAsComplex();

    void OnSetPrecision();
    void OnSetExp();
    void OnSetUnit();

    void OnDefaultRadian();
    void OnDefaultDegree();
    void OnDefaultGrad();

    void OnResultRadian();
    void OnResultDegree();
    void OnResultGrad();

    void OnDefaultBinaryNotation();
    void OnDefaultOctalNotation();
    void OnDefaultDecimalNotation();
    void OnDefaultHexadecimalNotation();

    void OnResultBinaryNotation();
    void OnResultOctalNotation();
    void OnResultDecimalNotation();
    void OnResultHexadecimalNotation();

    void OnFractionFormProper();
    void OnFractionFormImproper();

    void OnComplexFormArithmetic();
    void OnComplexFormTrigonometric();
    void OnComplexFormExponential();

signals:
    void CaretMoved(const EditorState editor_state);
    void DocumentChanged(const bool changed);
    void SaveResult(const uint task_id, IOResult result);
    void LoadResult(const uint task_id, IOResult result);
    void ClipboardCopyResult(CopyResult result);
    void ClipboardPasteResult(PasteResult result);
    void LinkClicked(const std::u32string& url);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    
private:
    ElementId GetResultId();

#ifdef TEST_APP
public:
#else
private:
#endif
    friend class MainWindow;

    yutovo::Config& config;

    MainWindow* main_window;
    DocumentWidget* document_widget;
    DocumentPtr document;

    QScrollBar *vertical_scroll = nullptr, *horizontal_scroll = nullptr;

    QString path;

    bool last_changed = false;

    QAction* copy = nullptr;
    QAction* paste = nullptr;
    QAction* cut = nullptr;

    QAction* link = nullptr;

    QAction* present_as_auto = nullptr;
    QAction* present_as_real = nullptr;
    QAction* present_as_integer = nullptr;
    QAction* present_as_rational = nullptr;
    QAction* present_as_complex = nullptr;

    QAction* set_precision = nullptr;
    QAction* set_exp = nullptr;
    QAction* set_unit = nullptr;

    QAction* default_radian = nullptr;
    QAction* default_degree = nullptr;
    QAction* default_grad = nullptr;

    QAction* result_radian = nullptr;
    QAction* result_degree = nullptr;
    QAction* result_grad = nullptr;

    QAction* default_binary_notaion = nullptr;
    QAction* default_octal_notaion = nullptr;
    QAction* default_decimal_notaion = nullptr;
    QAction* default_hexadecimal_notaion = nullptr;

    QAction* result_binary_notaion = nullptr;
    QAction* result_octal_notaion = nullptr;
    QAction* result_decimal_notaion = nullptr;
    QAction* result_hexadecimal_notaion = nullptr;

    QAction* fraction_form_proper = nullptr;
    QAction* fraction_form_improper = nullptr;

    QAction* complex_form_arithmetic = nullptr;
    QAction* complex_form_trigonometric = nullptr;
    QAction* complex_form_exponential = nullptr;

    QAction* graph = nullptr;
};

#endif
