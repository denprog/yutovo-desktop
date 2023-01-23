#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QFontComboBox>
#include "ui_mainwindow.h"
#include <cstring>
#include <sstream>
#include "document_widget.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void SetupGui();
    void CreateActions();
    void CreateStatusBar();

    void New();
    void Open();
    void Save();
    void SaveAs();
    void Exit();

    void Copy();
    void Paste();
    void Cut();

    void Undo();
    void Redo();

    void About();

private slots:
    void OnVerticalValueChanged(int value);
    void OnHorizontalValueChanged(int value);

    void OnInsertCode();

    void OnCurrentParagraphFormatChanged(const QString& format);
    void OnCurrentFontChanged(const QFont& font);
    void OnCurrentSizeChanged(const QString& size);

    void OnBold();
    void OnItalic();
    void OnUnderline();

    void OnCaretMoved(const EditorState editor_state);
    void OnSaveResult(const uint task_id, IOResult result);
    void OnLoadResult(const uint task_id, IOResult result);
    void OnClipboardCopyResult(CopyResult result);
    void OnDocumentUpdated(const Rect rect);

private:
    void FillParagraphFormats();
    void FillSizes(const QFont& font);

private:
    Ui::MainWindow *ui;
    DocumentWidget* document_widget;
    DocumentPtr document;

    QScrollBar *vertical_scroll = nullptr, *horizontal_scroll = nullptr;

    QString current_file_name;

    std::stringstream clipboard_array;
    std::string clipboard_text;

    QComboBox* paragraph_format_combo = nullptr;
    
    QFontComboBox* family_combo = nullptr;
    QComboBox* size_combo = nullptr;

    QAction* undo_action = nullptr;
    QAction* redo_action = nullptr;

    QAction* bold_action = nullptr;
    QAction* italic_action = nullptr;
    QAction* underline_action = nullptr;
};

#endif
