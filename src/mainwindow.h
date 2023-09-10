#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QFontComboBox>
#include <QSettings>
#include <QSortFilterProxyModel>
#include <yutovo_editor/document.h>
#include "ui_mainwindow.h"
#include <cstring>
#include <sstream>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

using namespace yutovo;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    void SetupGui();

    void AddEditorTab(const QString name);

    DocumentPtr GetCurrentDocument();

    void CreateActions();
    void CreateAlgebraToolbar();
    void CreateTrigonometryToolbar();
    void CreateHyperbolicToolbar();
    void CreateFunctionsToolbar();
    void CreateStatusBar();

    void SetFocus();

    void New();
    void Open();
    void OpenRecentFile();
    void OpenFile(QString file_name);
    void Save();
    void SaveAs();
    void Close();
    void Settings();
    void Exit();

    void Copy();
    void Paste();
    void Cut();

    void Undo();
    void Redo();

    void About();

    void StandardToolbar();
    void FormatToolbar();
    void AlgebraToolbar();
    void TrigonometryToolbar();
    void HyperbolicToolbar();
    void FunctionsToolbar();

    void StatusBar();

private slots:
    void OnNextEditorTab();
    void OnPrevEditorTab();
    void OnCloseEditorTab(int index);
    void OnEditorChanged(int index);

    void OnInsertCode();

    void OnCurrentParagraphFormatChanged(const QString& format);
    void OnCurrentFontChanged(const QFont& font);
    void OnCurrentSizeEditingFinished();
    void OnCurrentSizeChanged(int index);

    void OnBold();
    void OnItalic();
    void OnUnderline();

    void OnPlus();
    void OnMinus();
    void OnMultiply();
    void OnDivision();
    void OnSquareRoot();
    void OnNthRoot();
    void OnPower();
    void OnSubscript();
    void OnFences();
    void OnAssignment();
    void OnEquation();

    void OnSin();
    void OnCos();
    void OnTg();
    void OnCtg();
    void OnSec();
    void OnCsc();
    void OnArcsin();
    void OnArccos();
    void OnArctg();
    void OnArcctg();
    void OnArcsec();
    void OnArccsc();

    void OnSinh();
    void OnCosh();
    void OnTgh();
    void OnCtgh();
    void OnSech();
    void OnCsch();
    void OnArsinh();
    void OnArcosh();
    void OnArtgh();
    void OnArctgh();
    void OnArsech();
    void OnArcsch();

    void OnExp();
    void OnLn();
    void OnLg();
    void OnLog();
    void OnInt();
    void OnFract();
    void OnRound();

    void OnCaretMoved(const EditorState editor_state);
    void OnSaveResult(const uint task_id, IOResult result);
    void OnLoadResult(const uint task_id, IOResult result);
    void OnClipboardCopyResult(CopyResult result);
    void OnClipboardPasteResult(PasteResult result);

private:
    void FillParagraphFormats();
    void FillSizes(const QFont& font);

    void WriteSettings();
    void ReadSettings();

    void UpdateFontSize();
    void UpdateCopyPaste();
    void UpdateRecentFiles(const QString add_file_name = "");

private:
    friend class DocumentWindow;

    Ui::MainWindow *ui;

    yutovo::Config config;
    QSettings settings;

    QString dialog_file_name; //file name to be loaded/saved

    std::u32string clipboard_json;
    std::u32string clipboard_text;

    QMenu* recent_files_menu = nullptr;
    int recent_files_count = 10;
    QList<QString> recent_files;

    bool block_format_slots = false;
    
    QComboBox* paragraph_format_combo = nullptr;
    
    QFontComboBox* family_combo = nullptr;
    QComboBox* size_combo = nullptr;

    int last_font_size = 0;

    QAction* undo_action = nullptr;
    QAction* redo_action = nullptr;

    QAction* copy_action = nullptr;
    QAction* paste_action = nullptr;
    QAction* cut_action = nullptr;

    QAction* bold_action = nullptr;
    QAction* italic_action = nullptr;
    QAction* underline_action = nullptr;

    QAction* standard_toolbar_action = nullptr;
    QAction* format_toolbar_action = nullptr;
    QAction* algebra_toolbar_action = nullptr;
    QAction* trigonometry_toolbar_action = nullptr;
    QAction* hyperbolic_toolbar_action = nullptr;
    QAction* functions_toolbar_action = nullptr;
    QAction* status_bar_action = nullptr;

    QToolBar* standard_toolbar = nullptr;
    QToolBar* format_toolbar = nullptr;
    QToolBar* algebra_toolbar = nullptr;
    QToolBar* trigonometry_toolbar = nullptr;
    QToolBar* hyperbolic_toolbar = nullptr;
    QToolBar* functions_toolbar = nullptr;
};

#endif
