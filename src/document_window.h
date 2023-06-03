#ifndef __DOCUMENT_WINDOW_H__
#define __DOCUMENT_WINDOW_H__

#include <QScrollBar>
#include "document_widget.h"

class DocumentWindow : public QWidget
{
    Q_OBJECT

public:
    DocumentWindow(yutovo::Config& _config, QWidget *parent = nullptr);

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

signals:
    void CaretMoved(const EditorState editor_state);
    void SaveResult(const uint task_id, IOResult result);
    void LoadResult(const uint task_id, IOResult result);
    void ClipboardCopyResult(CopyResult result);
    void ClipboardPasteResult(PasteResult result);

private:
    friend class MainWindow;

    yutovo::Config& config;

    DocumentPtr document;
    DocumentWidget* document_widget;

    QScrollBar *vertical_scroll = nullptr, *horizontal_scroll = nullptr;

    QString path;

    QAction* present_as_auto = nullptr;
    QAction* present_as_real = nullptr;
    QAction* present_as_integer = nullptr;
    QAction* present_as_rational = nullptr;
};

#endif
