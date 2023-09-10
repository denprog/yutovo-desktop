#ifndef __DOCUMENT_WINDOW_H__
#define __DOCUMENT_WINDOW_H__

#include <QScrollBar>
#include "document_widget.h"

class MainWindow;

class DocumentWindow : public QWidget
{
    Q_OBJECT

public:
    DocumentWindow(yutovo::Config& _config, QWidget* parent);

    void MakeContextMenu(QContextMenuEvent* event);

    void SetFocus();

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

    void OnPresentAsAuto();
    void OnPresentAsReal();
    void OnPresentAsInteger();
    void OnPresentAsRational();

    void OnSetPrecision();
    void OnSetExp();
    void OnSetUnit();

    void OnResultRadian();
    void OnResultDegree();
    void OnResultGrad();

    void OnBinaryNotation();
    void OnOctalNotation();
    void OnDecimalNotation();
    void OnHexadecimalNotation();

    void OnFractionFormProper();
    void OnFractionFormImproper();

signals:
    void CaretMoved(const EditorState editor_state);
    void SaveResult(const uint task_id, IOResult result);
    void LoadResult(const uint task_id, IOResult result);
    void ClipboardCopyResult(CopyResult result);
    void ClipboardPasteResult(PasteResult result);

private:
    friend class MainWindow;

    yutovo::Config& config;

    MainWindow* main_window;
    DocumentWidget* document_widget;
    DocumentPtr document;

    QScrollBar *vertical_scroll = nullptr, *horizontal_scroll = nullptr;

    QString path;

    QAction* copy = nullptr;
    QAction* paste = nullptr;
    QAction* cut = nullptr;

    QAction* present_as_auto = nullptr;
    QAction* present_as_real = nullptr;
    QAction* present_as_integer = nullptr;
    QAction* present_as_rational = nullptr;

    QAction* set_precision = nullptr;
    QAction* set_exp = nullptr;
    QAction* set_unit = nullptr;

    QAction* result_radian = nullptr;
    QAction* result_degree = nullptr;
    QAction* result_grad = nullptr;

    QAction* binary_notaion = nullptr;
    QAction* octal_notaion = nullptr;
    QAction* decimal_notaion = nullptr;
    QAction* hexadecimal_notaion = nullptr;

    QAction* fraction_form_proper = nullptr;
    QAction* fraction_form_improper = nullptr;
};

#endif
