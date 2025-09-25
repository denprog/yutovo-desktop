/*
 * Yutovo Desktop
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLabel>
#include <QFontComboBox>
#include <QSettings>
#include <QTranslator>
#include <QProcess>
#include <QSortFilterProxyModel>
#include <yutovo-editor/document.h>
#include <yutovo-logger/logger.h>
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

    void Start(QString filename = "");
    
protected:
    void changeEvent(QEvent* event) override;
    void closeEvent(QCloseEvent *event) override;
    bool focusNextPrevChild(bool next) override;

private:
    void SetupGui();

    void AddEditorTab(const QString name, const QString tooltip);

    DocumentPtr GetCurrentDocument();

    void CreateActions();
    void CreateAlgebraToolbar();
    void CreateTrigonometryToolbar();
    void CreateHyperbolicToolbar();
    void CreateFunctionsToolbar();
    void CreateGreekToolbar();
    void CreateCurrenciesToolbar();
    void CreateGraphsToolbar();
    void CreateStatusBar();

    void SetFocus();

    void New();
    void Open();
    void OpenRecentFile();
    void OpenLibraryFile();
    void OpenFile(QString file_name);
    void Save();
    void SaveFile(int index);
    void SaveFileAsName();
    void SaveFileAs(int index);
    void SaveAll();
    void Close();
    void CloseAll();
    void CloseOthers();
    void Settings();
    void Exit();

    void Copy();
    void Paste();
    void Cut();

    void Link();

    void Properties();

    void Recalculate();

    void Undo();
    void Redo();

    void HelpOnline();
    void TermsOfUse();
    void PrivacyPolicy();
    void About();

    void StandardToolbar();
    void FormatToolbar();
    void AlgebraToolbar();
    void TrigonometryToolbar();
    void HyperbolicToolbar();
    void FunctionsToolbar();
    void GreekToolbar();
    void CurrencyToolbar();
    void GraphToolbar();

    void StatusBar();

private slots:
    void OnNextEditorTab();
    void OnPrevEditorTab();
    bool OnCloseEditorTab(int index);
    void OnEditorChanged(int index);

    void OnLinkClicked(const std::u32string& url);

    void OnInsertCode();

    void OnCurrentParagraphFormatChanged(const QString& format);
    void OnCurrentFontChanged(const QFont& font);
    void OnCurrentSizeEditingFinished();
    void OnCurrentSizeChanged(int index);

    void OnAlignLeft();
    void OnAlignRight();
    void OnAlignCenter();
    void OnAlignJustify();

    void OnBold();
    void OnItalic();
    void OnUnderline();
    void OnStrikethrough();
    void OnTextSubscript();
    void OnTextSuperscript();
    void OnTextColor();
    void OnBgTextColor();
    void OnLink();

    void OnPlus();
    void OnMinus();
    void OnMultiply();
    void OnDivision();
    void OnSquareRoot();
    void OnNthRoot();
    void OnPower();
    void OnSubscript();
    void OnSum();
    void OnProduct();
    void OnRoundBrackets();
    void OnSquareBrackets();
    void OnRadian();
    void OnDegree();
    void OnMinute();
    void OnSecond();
    void OnGrad();
    void OnAssignment();
    void OnUnit();
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

    void OnGreekLetter();

    void OnCurrency();

    void GraphLine();

    void OnCaretMoved(const EditorState editor_state);
    void OnDocumentChanged(const bool changed);
    void OnSaveResult(const uint task_id, IOResult result);
    void OnLoadResult(const uint task_id, IOResult result);
    void OnClipboardCopyResult(CopyResult result);
    void OnClipboardPasteResult(PasteResult result);

#ifdef REMOTE_SOLVER
    void OnServiceStatus(IOResult result);
#endif

private:
    void FillParagraphFormats();
    void FillSizes(const QFont& font);

    void WriteSettings();
    void ReadSettings();

    void UpdateFontSize();
    void UpdateCopyPaste();
    void UpdateRecentFiles(const QString add_file_name = "");
    void UpdateLibraryMenu(QMenu* library_menu);

    void InstallTranslation(const yutovo_calculator::Language language);

    void UpdateCaption(int tab = -1, bool update_title = true);

    void UpdateLocaleMessage();

    void EnableButtons(bool enable);

#ifdef REMOTE_SOLVER
    void RestartService();
#endif

public:
    static QString GetLibraryDir();
    static QString GetTranslationDir(QString filename);

private:
    friend class DocumentWindow;

    Ui::MainWindow *ui;

    yutovo::Config config;
    QSettings settings;

    std::unique_ptr<QProcess> service;

    QTranslator desktop_translator, editor_translator;

    QString dialog_file_name; //file name to be loaded/saved
    std::map<uint, int> loading_files; //files being loaded by tabs

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

    bool exit_after_save = false;
    int close_tab_after_save = -1;

    std::map<uint, int> save_tasks;

    StringFormat string_format;

    QAction* save_action = nullptr;

    QAction* undo_action = nullptr;
    QAction* redo_action = nullptr;

    QAction* copy_action = nullptr;
    QAction* paste_action = nullptr;
    QAction* cut_action = nullptr;

    QAction* properties_action = nullptr;

    QAction* calculator_action = nullptr;

    QAction* recalculate_action = nullptr;

    QAction* align_left_action = nullptr;
    QAction* align_right_action = nullptr;
    QAction* align_center_action = nullptr;
    QAction* align_justify_action = nullptr;

    QAction* bold_action = nullptr;
    QAction* italic_action = nullptr;
    QAction* underline_action = nullptr;
    QAction* strikethrough_action = nullptr;
    QAction* subscript_action = nullptr;
    QAction* superscript_action = nullptr;

    QAction* text_color_action = nullptr;
    QAction* bg_text_color_action = nullptr;
    QAction* link_action = nullptr;

    QAction* standard_toolbar_action = nullptr;
    QAction* format_toolbar_action = nullptr;
    QAction* algebra_toolbar_action = nullptr;
    QAction* trigonometry_toolbar_action = nullptr;
    QAction* hyperbolic_toolbar_action = nullptr;
    QAction* functions_toolbar_action = nullptr;
    QAction* greek_toolbar_action = nullptr;
    QAction* currency_toolbar_action = nullptr;
    QAction* status_bar_action = nullptr;
    QAction* graph_toolbar_action = nullptr;

    QToolBar* standard_toolbar = nullptr;
    QToolBar* format_toolbar = nullptr;
    QToolBar* algebra_toolbar = nullptr;
    QToolBar* trigonometry_toolbar = nullptr;
    QToolBar* hyperbolic_toolbar = nullptr;
    QToolBar* functions_toolbar = nullptr;
    QToolBar* greek_toolbar = nullptr;
    QToolBar* currency_toolbar = nullptr;
    QToolBar* graph_toolbar = nullptr;

    QList<QString> last_documents;

    QLabel* locale_status = nullptr;

    QLabel* link_label = nullptr;

    Logger* logger = nullptr;
};

#endif
