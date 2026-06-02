/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "mainwindow.h"
#include <QMenu>
#include <QToolBar>
#include <QToolButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QBuffer>
#include <QClipboard>
#include <QMimeData>
#include <QScrollBar>
#include <QLineEdit>
#include <QFileInfo> 
#include <QColorDialog>
#include <QCloseEvent>
#include <QUrl>
#include <QDesktopServices>
#include <QTextDocument>
#include <QTimer>
#include <filesystem>
#ifdef _WIN32
#include <shlobj_core.h>
#include <codecvt>
#endif
#include <yutovo-editor/editor_utils.h>
#include <yutovo-calculator/math_helper.h>
#include "document_window.h"
#include "about_dialog.h"
#include "settings_dialog.h"
#include "properties_dialog.h"
#include "link_dialog.h"
#include "terms_of_use_dialog.h"
#include "privacy_policy_dialog.h"
#include "graph_settings_dialog.h"
#include "export_pdf_dialog.h"
#include "whats_new_dialog.h"

//MainWindow

using namespace yutovo_calculator;

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings(new QSettings("Yutovo", "Yutovo Desktop"))
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/icons/images/mainicon.png"));

    qRegisterMetaType<Rect>("Rect");
    qRegisterMetaType<CaretState>("CaretState");
    qRegisterMetaType<EditorState>("EditorState");
    qRegisterMetaType<IOResult>("IOResult");
    qRegisterMetaType<CopyResult>("CopyResult");
    qRegisterMetaType<std::vector<ElementPtr>>("std::vector<ElementPtr>");
    qRegisterMetaType<ElementId>("ElementId");
    qRegisterMetaType<std::u32string>("std::u32string");
    qRegisterMetaType<std::vector<uint8_t>>("std::vector<uint8_t>");
    qRegisterMetaType<PdfResult>("PdfResult");

    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    WriteSettings();
    delete ui;
    if (logger)
        logger->Info("Desktop stop");
}

void MainWindow::Start(QString filename)
{
    bool first_run = false;
    if (!settings.childGroups().contains("MainWindow"))
        first_run = true;

    ReadSettings();

    logger = Logger::GetInstance(config.logs_path + "/yutovo-desktop", "yutovo-desktop", config.log_console, config.log_file);
    logger->SetLevel(config.log_level);

    InstallTranslation(config.language);

#ifdef REMOTE_SOLVER
    RestartService();
#endif

    ui->editor_tabs->clear();

    //create toolbars and menu - they will never be deleted, only recreated on language change
    CreateActions();
    addToolBarBreak();
    CreateAlgebraToolbar();
    addToolBarBreak();
    CreateTrigonometryToolbar();
    addToolBarBreak();
    CreateHyperbolicToolbar();
    addToolBarBreak();
    CreateFunctionsToolbar();
    CreateGreekToolbar();
    CreateCurrenciesToolbar();
    CreateGraphsToolbar();
    CreateLogicalToolbar();
    CreateStatusBar();

    //restore toolbar positions now that they exist
    const auto state = settings.value("MainWindow/state", QByteArray()).toByteArray();
    if (!state.isEmpty())
        restoreState(state);

    AddEditorTab(tr("(No name)"), "");

    UpdateRecentFiles();

    if (filename.isEmpty())
    {
        settings.beginGroup("Documents");
        if (settings.value("load_last_documents").toBool())
        {
            auto list = settings.value("last_documents").toStringList();
            for (auto it = list.begin(); it != list.end(); ++it)
            {
                if (*it != "")
                    OpenFile(*it);
            }
        }
        settings.endGroup();
    }
    else
    {
        OpenFile(filename.toUtf8().data());
    }

    if (first_run)
    {
        //it is the first run - open the hello document
        if (config.language == yutovo_calculator::Language::Russian)
            OpenFile(GetLibraryDir() + "ru/Другое/Первая страница.yut");
        else if (config.language == yutovo_calculator::Language::Spanish)
            OpenFile(GetLibraryDir() + "es/Otros/Primera página.yut");
        else if (config.language == yutovo_calculator::Language::BrazilianPortuguese)
            OpenFile(GetLibraryDir() + "pt_BR/Outros/Primeira página.yut");
        else
            OpenFile(GetLibraryDir() + "en/Others/First page.yut");
    }

    QTimer::singleShot(0, this, &MainWindow::CheckVersionAndShowWhatsNew);

    logger->Info("Desktop start");
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
        if (link_label)
        {
#ifdef _WIN32
            link_label->setText(tr("yutovo_web_windows"));
#else
            link_label->setText(tr("yutovo_web_linux"));
#endif
        }
        UpdateDocumentOnlineLink();
        SetupGuiActions();
        UpdateRecentFiles();
    }

    QMainWindow::changeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    for (int i = 0; i < ui->editor_tabs->count(); ++i)
    {
        DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->widget(i);
        if (!w->path.isEmpty() && last_documents.indexOf(w->path) == -1)
            last_documents.push_back(w->path);
        if (w->document->IsChanged())
        {
            QString file_name;
            QFileInfo file_info(w->path);
            if (file_info.fileName().isEmpty())
                file_name = tr("(No name)");
            else
                file_name = file_info.fileName();

            QMessageBox m(QMessageBox::Question, tr("Yutovo"), tr("Document %1 is unsaved. Save?").arg(file_name),
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);
            int r = m.exec();
            switch (r)
            {
            case QMessageBox::Yes:
            {
                close_tab_after_save = i;
                exit_after_save = true;
                SaveFile(i);
                event->ignore();
                return;
            }
            case QMessageBox::No:
                break;
            default:
                last_documents.clear();
                SetFocus();
                event->ignore();
                return;
            }
        }
    }
    QMainWindow::closeEvent(event);
}

bool MainWindow::focusNextPrevChild(bool next)
{
    return false;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (!event->mimeData()->hasUrls())
        return;

    const QList<QUrl> urls = event->mimeData()->urls();
    for (const QUrl &url : urls)
    {
        auto s = QFileInfo(url.toLocalFile()).suffix();
        if (s == "yut" || s == "txt")
        {
            event->acceptProposedAction();
            return;
        }
    }    
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    for (const QUrl& url : urls)
    {
        QString file_name = url.toLocalFile();
        if (!file_name.isEmpty())
            OpenFile(file_name);
    }

    event->acceptProposedAction();    
}

void MainWindow::SetupGuiActions()
{
    ui->editor_tabs->setTabsClosable(true);
    ui->editor_tabs->disconnect();
    connect(ui->editor_tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(OnCloseEditorTab(int)));
    connect(ui->editor_tabs, SIGNAL(currentChanged(int)), this, SLOT(OnEditorChanged(int)));
    
    //save toolbar positions before recreating
    QByteArray saved_state = saveState();
    
    //remove existing toolbars
    QList<QToolBar*> old_toolbars = findChildren<QToolBar*>();
    for (auto t : old_toolbars)
    {
        removeToolBar(t);
        delete t;
    }
    
    standard_toolbar = nullptr;
    format_toolbar = nullptr;
    algebra_toolbar = nullptr;
    trigonometry_toolbar = nullptr;
    hyperbolic_toolbar = nullptr;
    functions_toolbar = nullptr;
    greek_toolbar = nullptr;
    currency_toolbar = nullptr;
    graph_toolbar = nullptr;
    logical_toolbar = nullptr;
    
    menuBar()->clear();
    
    CreateActions();
    addToolBarBreak();
    CreateAlgebraToolbar();
    addToolBarBreak();
    CreateTrigonometryToolbar();
    addToolBarBreak();
    CreateHyperbolicToolbar();
    addToolBarBreak();
    CreateFunctionsToolbar();
    CreateGreekToolbar();
    CreateCurrenciesToolbar();
    CreateGraphsToolbar();
    CreateLogicalToolbar();
    
    if (!saved_state.isEmpty())
        restoreState(saved_state);
    
    //restore toolbar visibility from settings
    standard_toolbar_action->setChecked(settings.value("MainWindow/standard_toolbar", true).toBool());
    format_toolbar_action->setChecked(settings.value("MainWindow/format_toolbar", true).toBool());
    algebra_toolbar_action->setChecked(settings.value("MainWindow/algebra_toolbar", true).toBool());
    trigonometry_toolbar_action->setChecked(settings.value("MainWindow/trigonometry_toolbar", false).toBool());
    hyperbolic_toolbar_action->setChecked(settings.value("MainWindow/hyperbolic_toolbar", false).toBool());
    functions_toolbar_action->setChecked(settings.value("MainWindow/functions_toolbar", false).toBool());
    graph_toolbar_action->setChecked(settings.value("MainWindow/graphs_toolbar", false).toBool());
    greek_toolbar_action->setChecked(settings.value("MainWindow/greek_toolbar", false).toBool());
    currency_toolbar_action->setChecked(settings.value("MainWindow/currency_toolbar", false).toBool());
    logical_toolbar_action->setChecked(settings.value("MainWindow/logical_toolbar", false).toBool());
    status_bar_action->setChecked(settings.value("MainWindow/status_bar", true).toBool());

    int i = ui->editor_tabs->currentIndex();
    if (i >= 0)
        OnEditorChanged(i);
}

void MainWindow::AddEditorTab(const QString name, const QString tooltip)
{
    DocumentWindow* wnd = new DocumentWindow(config, settings, this);

    connect(wnd, &DocumentWindow::CaretMoved, this, &MainWindow::OnCaretMoved);
    connect(wnd, &DocumentWindow::DocumentChanged, this, &MainWindow::OnDocumentChanged);
    connect(wnd, &DocumentWindow::SaveResult, this, &MainWindow::OnSaveResult);
    connect(wnd, &DocumentWindow::LoadResult, this, &MainWindow::OnLoadResult);
    connect(wnd, &DocumentWindow::ClipboardCopyResult, this, &MainWindow::OnClipboardCopyResult);
    connect(wnd, &DocumentWindow::ClipboardPasteResult, this, &MainWindow::OnClipboardPasteResult);
    connect(wnd, &DocumentWindow::LinkClicked, this, &MainWindow::OnLinkClicked);

    connect(wnd->document_widget, &DocumentWidget::NextEditorTab, this, &MainWindow::OnNextEditorTab);
    connect(wnd->document_widget, &DocumentWidget::PrevEditorTab, this, &MainWindow::OnPrevEditorTab);
    connect(wnd->document_widget, &DocumentWidget::ScaleChanged, this, &MainWindow::OnScaleChanged);

#ifdef REMOTE_SOLVER
    connect(wnd->document_widget, &DocumentWidget::ServiceStatus, this, &MainWindow::OnServiceStatus);
#endif

    ui->editor_tabs->addTab(wnd, name);
    if (!tooltip.isEmpty())
        ui->editor_tabs->setTabToolTip(ui->editor_tabs->count() - 1, tooltip);
    ui->editor_tabs->setCurrentIndex(ui->editor_tabs->count() - 1);
    wnd->SetFocus();

    wnd->CreateDocument();

    auto document = GetCurrentDocument();
    auto s = wnd->document_widget->size();
    document->Resize(s.width(), s.height());
    OnCaretMoved(document->GetEditorState());
    FillParagraphFormats();
    FillScales();

    UpdateCaption();

    EnableButtons(true);
}

DocumentPtr MainWindow::GetCurrentDocument()
{
    DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->currentWidget();
    if (w)
    {
        w->SetFocus();
        return w->document;
    }
    return nullptr;
}

void MainWindow::CreateActions()
{
    //file menu and toolbar
    QMenu* file_menu = menuBar()->addMenu(tr("&File"));
    standard_toolbar = addToolBar(tr("Standard"));
    standard_toolbar->setObjectName("standard_toolbar");

    QAction* action = new QAction(QIcon(":/icons/images/standard/new.png"), tr("&New"), this);
    action->setShortcuts(QKeySequence::New);
    action->setStatusTip(tr("Create a new document"));
    action->setObjectName("actionNew");
    connect(action, &QAction::triggered, this, &MainWindow::New);
    file_menu->addAction(action);
    standard_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/standard/open.png"), tr("&Open..."), this);
    action->setShortcuts(QKeySequence::Open);
    action->setStatusTip(tr("Open an existing file"));
    action->setObjectName("actionOpen");
    connect(action, &QAction::triggered, this, &MainWindow::Open);
    file_menu->addAction(action);
    standard_toolbar->addAction(action);

    recent_files_menu = file_menu->addMenu(tr("&Open recent file"));

    save_action = new QAction(QIcon(":/icons/images/standard/save.png"), tr("&Save"), this);
    save_action->setShortcuts(QKeySequence::Save);
    save_action->setStatusTip(tr("Save the document to disk"));
    save_action->setObjectName("actionSave");
    connect(save_action, &QAction::triggered, this, &MainWindow::Save);
    file_menu->addAction(save_action);
    standard_toolbar->addAction(save_action);

    save_all_action = new QAction(tr("Save &all"), this);
    save_all_action->setStatusTip(tr("Save all the documents to disk"));
    connect(save_all_action, &QAction::triggered, this, &MainWindow::SaveAll);
    file_menu->addAction(save_all_action);

    save_as_action = file_menu->addAction(tr("Save &As..."), this, &MainWindow::SaveFileAsName);
    save_as_action->setStatusTip(tr("Save the document under a new name"));

    close_action = new QAction(tr("&Close"), this);
    close_action->setStatusTip(tr("Close the document"));
    close_action->setObjectName("actionClose");
    connect(close_action, &QAction::triggered, this, &MainWindow::Close);
    file_menu->addAction(close_action);

    close_all_action = new QAction(tr("Close all"), this);
    close_all_action->setStatusTip(tr("Close all documents"));
    connect(close_all_action, &QAction::triggered, this, &MainWindow::CloseAll);
    file_menu->addAction(close_all_action);

    close_others_action = new QAction(tr("Close others"), this);
    close_others_action->setStatusTip(tr("Close all documents except current"));
    connect(close_others_action, &QAction::triggered, this, &MainWindow::CloseOthers);
    file_menu->addAction(close_others_action);

    file_menu->addSeparator();

    export_html_action = new QAction(tr("Export to HTML"), this);
    export_html_action->setStatusTip(tr("Export current document to HTML"));
    connect(export_html_action, &QAction::triggered, this, &MainWindow::ExportToHtml);
    file_menu->addAction(export_html_action);

    export_pdf_action = new QAction(tr("Export to PDF"), this);
    export_pdf_action->setStatusTip(tr("Export current document to PDF"));
    connect(export_pdf_action, &QAction::triggered, this, &MainWindow::ExportToPdf);
    file_menu->addAction(export_pdf_action);

    file_menu->addSeparator();

    action = new QAction(tr("&Settings"), this);
    action->setStatusTip(tr("Application settings"));
    connect(action, &QAction::triggered, this, &MainWindow::Settings);
    file_menu->addAction(action);

    file_menu->addSeparator();

    action = file_menu->addAction(QIcon::fromTheme("application-exit"), tr("E&xit"), this, &MainWindow::Exit);
    action->setShortcuts(QKeySequence::Quit);
    action->setStatusTip(tr("Exit the application"));

    //edit menu and toolbar
    QMenu* edit_menu = menuBar()->addMenu(tr("&Edit"));

    edit_menu->addSeparator();
    standard_toolbar->addSeparator();

    undo_action = new QAction(QIcon(":/icons/images/standard/undo.png"), tr("U&ndo") + " (Ctrl+Z)", this);
    undo_action->setShortcuts(QKeySequence::Undo);
    undo_action->setStatusTip(tr("Undo the last operation"));
    connect(undo_action, &QAction::triggered, this, &MainWindow::Undo);
    edit_menu->addAction(undo_action);
    standard_toolbar->addAction(undo_action);

    redo_action = new QAction(QIcon(":/icons/images/standard/redo.png"), tr("&Redo") + " (Ctrl+Y)", this);
    redo_action->setShortcuts(QKeySequence::Redo);
    redo_action->setStatusTip(tr("Redo the last operation"));
    connect(redo_action, &QAction::triggered, this, &MainWindow::Redo);
    edit_menu->addAction(redo_action);
    standard_toolbar->addAction(redo_action);

    edit_menu->addSeparator();
    standard_toolbar->addSeparator();

    cut_action = new QAction(QIcon(":/icons/images/standard/cut.png"), tr("Cu&t") + " (Shift+Del)", this);
    cut_action->setShortcuts(QKeySequence::Cut);
    cut_action->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    connect(cut_action, &QAction::triggered, this, &MainWindow::Cut);
    edit_menu->addAction(cut_action);
    standard_toolbar->addAction(cut_action);

    copy_action = new QAction(QIcon(":/icons/images/standard/copy.png"), tr("&Copy") + " (Ctrl+Ins)", this);
    copy_action->setShortcuts(QKeySequence::Copy);
    copy_action->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
    connect(copy_action, &QAction::triggered, this, &MainWindow::Copy);
    edit_menu->addAction(copy_action);
    standard_toolbar->addAction(copy_action);

    paste_action = new QAction(QIcon(":/icons/images/standard/paste.png"), tr("&Paste") + " (Shift+Ins)", this);
    paste_action->setShortcuts(QKeySequence::Paste);
    paste_action->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
    connect(paste_action, &QAction::triggered, this, &MainWindow::Paste);
    edit_menu->addAction(paste_action);
    standard_toolbar->addAction(paste_action);

    standard_toolbar->addSeparator();

    properties_action = new QAction(QIcon(":/icons/images/standard/properties.png"), tr("Document properties"), this);
    properties_action->setStatusTip(tr("Properties of the document"));
    connect(properties_action, &QAction::triggered, this, &MainWindow::Properties);
    standard_toolbar->addAction(properties_action);

    recalculate_action = new QAction(QIcon(":/icons/images/standard/recalculate.png"), tr("&Recalculate all"), this);
    recalculate_action->setStatusTip(tr("Recalculate the whole document"));
    connect(recalculate_action, &QAction::triggered, this, &MainWindow::Recalculate);
    standard_toolbar->addAction(recalculate_action);

    UpdateCopyPaste();

    //format toolbar
    format_toolbar = addToolBar(tr("Format"));
    format_toolbar->setObjectName("format_toolbar");
    format_toolbar->setStyleSheet("QToolBar{spacing:4px;}");

    calculator_action = new QAction(QIcon(":/icons/images/format/code.png"), tr("Insert calculator"), this);
    calculator_action->setStatusTip(tr("Insert calculator"));
    connect(calculator_action, &QAction::triggered, this, &MainWindow::OnInsertCode);
    format_toolbar->addAction(calculator_action);

    format_toolbar->addSeparator();

    scale_combo = new QComboBox;
    scale_combo->setEditable(true);
    connect(scale_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(OnCurrentScaleChanged(int)));
    connect(scale_combo->lineEdit(), &QLineEdit::editingFinished, this, &MainWindow::OnCurrentScaleEditingFinished);
    scale_combo->setInsertPolicy(QComboBox::NoInsert);
    format_toolbar->addWidget(scale_combo);
    FillScales();

    format_toolbar->addSeparator();

    paragraph_format_combo = new QComboBox;
    paragraph_format_combo->setFixedWidth(120);
    format_toolbar->addWidget(paragraph_format_combo);
    connect(paragraph_format_combo, &QComboBox::currentTextChanged, this, &MainWindow::OnCurrentParagraphFormatChanged);

    format_toolbar->addSeparator();

    family_combo = new QFontComboBox;
    family_combo->setFixedWidth(250);
    connect(family_combo, &QFontComboBox::currentFontChanged, this, &MainWindow::OnCurrentFontChanged);
    format_toolbar->addWidget(family_combo);

    size_combo = new QComboBox;
    size_combo->setEditable(true);
    connect(size_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(OnCurrentSizeChanged(int)));
    connect(size_combo->lineEdit(), &QLineEdit::editingFinished, this, &MainWindow::OnCurrentSizeEditingFinished);
    size_combo->setInsertPolicy(QComboBox::NoInsert);
    format_toolbar->addWidget(size_combo);
    FillSizes(family_combo->currentFont());

    bold_action = new QAction(QIcon(":/icons/images/format/bold.png"), tr("Bold"), this);
    connect(bold_action, &QAction::triggered, this, &MainWindow::OnBold);
    bold_action->setCheckable(true);
    bold_action->setStatusTip(tr("Bold font"));
    format_toolbar->addAction(bold_action);

    italic_action = new QAction(QIcon(":/icons/images/format/italic.png"), tr("Italic"), this);
    connect(italic_action, &QAction::triggered, this, &MainWindow::OnItalic);
    italic_action->setCheckable(true);
    italic_action->setStatusTip(tr("Italic font"));
    format_toolbar->addAction(italic_action);

    underline_action = new QAction(QIcon(":/icons/images/format/underline.png"), tr("Underline"), this);
    connect(underline_action, &QAction::triggered, this, &MainWindow::OnUnderline);
    underline_action->setCheckable(true);
    underline_action->setStatusTip(tr("Underline font"));
    format_toolbar->addAction(underline_action);

    strikethrough_action = new QAction(QIcon(":/icons/images/format/strikethrough.png"), tr("Strikethrough"), this);
    connect(strikethrough_action, &QAction::triggered, this, &MainWindow::OnStrikethrough);
    strikethrough_action->setCheckable(true);
    strikethrough_action->setStatusTip(tr("Strikethrough font"));
    format_toolbar->addAction(strikethrough_action);

    subscript_action = new QAction(QIcon(":/icons/images/format/subscript.png"), tr("Subscript"), this);
    connect(subscript_action, &QAction::triggered, this, &MainWindow::OnTextSubscript);
    subscript_action->setCheckable(true);
    subscript_action->setStatusTip(tr("Subscript"));
    format_toolbar->addAction(subscript_action);

    superscript_action = new QAction(QIcon(":/icons/images/format/superscript.png"), tr("Superscript"), this);
    connect(superscript_action, &QAction::triggered, this, &MainWindow::OnTextSuperscript);
    superscript_action->setCheckable(true);
    superscript_action->setStatusTip(tr("Superscript"));
    format_toolbar->addAction(superscript_action);

    text_color_action = new QAction(QIcon(":/icons/images/format/text_color.png"), tr("Text color"), this);
    connect(text_color_action, &QAction::triggered, this, &MainWindow::OnTextColor);
    text_color_action->setStatusTip(tr("Text color"));
    format_toolbar->addAction(text_color_action);

    bg_text_color_action = new QAction(QIcon(":/icons/images/format/bg_text_color.png"), tr("Background text color"), this);
    connect(bg_text_color_action, &QAction::triggered, this, &MainWindow::OnBgTextColor);
    bg_text_color_action->setStatusTip(tr("Background text color"));
    format_toolbar->addAction(bg_text_color_action);

    link_action = new QAction(QIcon(":/icons/images/format/link.png"), tr("Link"), this);
    connect(link_action, &QAction::triggered, this, &MainWindow::OnLink);
    link_action->setStatusTip(tr("Link"));
    format_toolbar->addAction(link_action);

    format_toolbar->addSeparator();

    align_left_action = new QAction(QIcon(":/icons/images/format/align_left.png"), tr("Align left"), this);
    connect(align_left_action, &QAction::triggered, this, &MainWindow::OnAlignLeft);
    align_left_action->setCheckable(true);
    align_left_action->setStatusTip(tr("Align text left"));
    format_toolbar->addAction(align_left_action);

    align_center_action = new QAction(QIcon(":/icons/images/format/align_center.png"), tr("Align center"), this);
    connect(align_center_action, &QAction::triggered, this, &MainWindow::OnAlignCenter);
    align_center_action->setCheckable(true);
    align_center_action->setStatusTip(tr("Align text center"));
    format_toolbar->addAction(align_center_action);

    align_right_action = new QAction(QIcon(":/icons/images/format/align_right.png"), tr("Align right"), this);
    connect(align_right_action, &QAction::triggered, this, &MainWindow::OnAlignRight);
    align_right_action->setCheckable(true);
    align_right_action->setStatusTip(tr("Align text right"));
    format_toolbar->addAction(align_right_action);

    align_justify_action = new QAction(QIcon(":/icons/images/format/align_justify.png"), tr("Align justify"), this);
    connect(align_justify_action, &QAction::triggered, this, &MainWindow::OnAlignJustify);
    align_justify_action->setCheckable(true);
    align_justify_action->setStatusTip(tr("Align text justify"));
    format_toolbar->addAction(align_justify_action);

    //view menu
    QMenu* view_menu = menuBar()->addMenu(tr("&View"));

    //toolbars submenu
    QMenu* toolbars_menu = view_menu->addMenu(tr("&Toolbars"));

    standard_toolbar_action = new QAction(tr("Standard toolbar"), this);
    standard_toolbar_action->setCheckable(true);
    standard_toolbar_action->setChecked(true);
    connect(standard_toolbar_action, &QAction::toggled, this, &MainWindow::StandardToolbar);
    toolbars_menu->addAction(standard_toolbar_action);

    format_toolbar_action = new QAction(tr("Format toolbar"), this);
    format_toolbar_action->setCheckable(true);
    format_toolbar_action->setChecked(true);
    connect(format_toolbar_action, &QAction::toggled, this, &MainWindow::FormatToolbar);
    toolbars_menu->addAction(format_toolbar_action);

    algebra_toolbar_action = new QAction(tr("Algebraic operations"), this);
    algebra_toolbar_action->setCheckable(true);
    algebra_toolbar_action->setChecked(true);
    connect(algebra_toolbar_action, &QAction::toggled, this, &MainWindow::AlgebraToolbar);
    toolbars_menu->addAction(algebra_toolbar_action);

    trigonometry_toolbar_action = new QAction(tr("Trigonometric functions"), this);
    trigonometry_toolbar_action->setCheckable(true);
    trigonometry_toolbar_action->setChecked(true);
    connect(trigonometry_toolbar_action, &QAction::toggled, this, &MainWindow::TrigonometryToolbar);
    toolbars_menu->addAction(trigonometry_toolbar_action);

    hyperbolic_toolbar_action = new QAction(tr("Hyperbolic functions"), this);
    hyperbolic_toolbar_action->setCheckable(true);
    hyperbolic_toolbar_action->setChecked(true);
    connect(hyperbolic_toolbar_action, &QAction::toggled, this, &MainWindow::HyperbolicToolbar);
    toolbars_menu->addAction(hyperbolic_toolbar_action);

    functions_toolbar_action = new QAction(tr("Math functions"), this);
    functions_toolbar_action->setCheckable(true);
    functions_toolbar_action->setChecked(true);
    connect(functions_toolbar_action, &QAction::toggled, this, &MainWindow::FunctionsToolbar);
    toolbars_menu->addAction(functions_toolbar_action);

    greek_toolbar_action = new QAction(tr("Greek letters"), this);
    greek_toolbar_action->setCheckable(true);
    greek_toolbar_action->setChecked(true);
    connect(greek_toolbar_action, &QAction::toggled, this, &MainWindow::GreekToolbar);
    toolbars_menu->addAction(greek_toolbar_action);

    currency_toolbar_action = new QAction(tr("Currencies"), this);
    currency_toolbar_action->setCheckable(true);
    currency_toolbar_action->setChecked(true);
    connect(currency_toolbar_action, &QAction::toggled, this, &MainWindow::CurrencyToolbar);
    toolbars_menu->addAction(currency_toolbar_action);

    graph_toolbar_action = new QAction(tr("Graphs"), this);
    graph_toolbar_action->setCheckable(true);
    graph_toolbar_action->setChecked(true);
    connect(graph_toolbar_action, &QAction::toggled, this, &MainWindow::GraphToolbar);
    toolbars_menu->addAction(graph_toolbar_action);

    logical_toolbar_action = new QAction(tr("Logical operations"), this);
    logical_toolbar_action->setCheckable(true);
    logical_toolbar_action->setChecked(true);
    connect(logical_toolbar_action, &QAction::toggled, this, &MainWindow::LogicalToolbar);
    toolbars_menu->addAction(logical_toolbar_action);

    status_bar_action = new QAction(tr("&Status bar"), this);
    status_bar_action->setCheckable(true);
    status_bar_action->setChecked(true);
    connect(status_bar_action, &QAction::toggled, this, &MainWindow::StatusBar);
    view_menu->addAction(status_bar_action);

    //library menu
    QMenu* library_menu = menuBar()->addMenu(tr("&Library"));
    UpdateLibraryMenu(library_menu, ".", tr("Help"));

    //help menu
    QMenu* help_menu = menuBar()->addMenu(tr("&Help"));

    QMenu* help_system_menu = help_menu->addMenu(tr("Help"));
    UpdateLibraryMenu(help_system_menu, tr("Help"), "");

    help_menu->addSeparator();

    action = help_menu->addAction(tr("&Terms of use"), this, &MainWindow::TermsOfUse);
    help_menu->setStatusTip(tr("Show the application's Terms of use box"));

    action = help_menu->addAction(tr("&Privacy policy"), this, &MainWindow::PrivacyPolicy);
    help_menu->setStatusTip(tr("Show the application's Privacy policy box"));

    help_menu->addSeparator();

    action = help_menu->addAction(tr("&What's New"), this, &MainWindow::WhatsNew);
    help_menu->setStatusTip(tr("Show what's new in this version"));

    action = help_menu->addAction(tr("&About"), this, &MainWindow::About);
    help_menu->setStatusTip(tr("Show the application's About box"));

    if (!link_label)
    {
        QHBoxLayout* layout = new QHBoxLayout();
        link_label = new QLabel();
#ifdef _WIN32
        link_label->setText(tr("yutovo_web_windows"));
#else
        link_label->setText(tr("yutovo_web_linux"));
#endif
        link_label->setTextFormat(Qt::RichText);
        link_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
        link_label->setOpenExternalLinks(true);
        layout->addWidget(link_label);
        QWidget* web_link = new QWidget(nullptr);
        web_link->setLayout(layout);
        menuBar()->setCornerWidget(web_link, Qt::TopRightCorner);
    }
}

void MainWindow::CreateAlgebraToolbar()
{
    //algebra toolbar
    algebra_toolbar = addToolBar(tr("Algebraic functions"));
    algebra_toolbar->setObjectName("algebra_toolbar");
    algebra_toolbar->setStyleSheet("QToolBar{spacing:4px;}");

    QAction* action = new QAction(QIcon(":/icons/images/algebra/plus.png"), tr("Plus") + " (+)", this);
    connect(action, &QAction::triggered, this, &MainWindow::OnPlus);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/minus.png"), tr("Minus") + " (-)", this);
    connect(action, &QAction::triggered, this, &MainWindow::OnMinus);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/multiply.png"), tr("Multiply") + " (*)", this);
    connect(action, &QAction::triggered, this, &MainWindow::OnMultiply);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/division.png"), tr("Division") + " (/)", this);
    connect(action, &QAction::triggered, this, &MainWindow::OnDivision);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/power.png"), tr("Power") + " (Ctrl+Shift+P)", this);
    connect(action, &QAction::triggered, this, &MainWindow::OnPower);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/sqrt.png"), tr("Square root") + " (Ctrl+Shift+Q)", this);
    connect(action, &QAction::triggered, this, &MainWindow::OnSquareRoot);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/nth_root.png"), tr("Nth root") + " (Ctrl+Shift+N)", this);
    connect(action, &QAction::triggered, this, &MainWindow::OnNthRoot);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/subscript.png"), tr("Subscript") + " (Ctrl+Shift+S)", this);
    connect(action, &QAction::triggered, this, &MainWindow::OnSubscript);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/sum.png"), tr("Sum"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnSum);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/product.png"), tr("Product"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnProduct);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/round_brackets.png"), tr("Round brackets"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnRoundBrackets);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/square_brackets.png"), tr("Square brackets"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnSquareBrackets);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/radian.png"), tr("Radian"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnRadian);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/degree.png"), tr("Degree"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnDegree);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/minute.png"), tr("Minute"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnMinute);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/second.png"), tr("Second"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnSecond);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/grad.png"), tr("Grad"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnGrad);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/infinity.png"), tr("Infinity"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnInfinity);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/assignment.png"), tr("Assignment") + " (:)", this);
    connect(action, &QAction::triggered, this, &MainWindow::OnAssignment);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/unit.png"), tr("Unit") + " (~)", this);
    connect(action, &QAction::triggered, this, &MainWindow::OnUnit);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/equation.png"), tr("Equation") + " (=)", this);
    connect(action, &QAction::triggered, this, &MainWindow::OnEquation);
    algebra_toolbar->addAction(action);
}

void MainWindow::CreateTrigonometryToolbar()
{
    //trigonometry toolbar
    trigonometry_toolbar = addToolBar(tr("Trigonometric functions"));
    trigonometry_toolbar->setObjectName("trigonometry_toolbar");
    trigonometry_toolbar->setStyleSheet("QToolBar{spacing:4px;}");

    QAction* action = new QAction(QIcon(":/icons/images/trigonometry/sin.png"), tr("Sine"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnSin);
    trigonometry_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/trigonometry/cos.png"), tr("Cosine"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnCos);
    trigonometry_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/trigonometry/tg.png"), tr("Tangens"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnTg);
    trigonometry_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/trigonometry/ctg.png"), tr("Cotangens"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnCtg);
    trigonometry_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/trigonometry/sec.png"), tr("Secans"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnSec);
    trigonometry_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/trigonometry/csc.png"), tr("Cosecans"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnCsc);
    trigonometry_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/trigonometry/arcsin.png"), tr("Arcsine"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnArcsin);
    trigonometry_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/trigonometry/arccos.png"), tr("Arccosine"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnArccos);
    trigonometry_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/trigonometry/arctg.png"), tr("Arctangens"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnArctg);
    trigonometry_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/trigonometry/arcctg.png"), tr("Arccotangens"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnArcctg);
    trigonometry_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/trigonometry/arcsec.png"), tr("Arcsecans"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnArcsec);
    trigonometry_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/trigonometry/arccsc.png"), tr("Arccosecans"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnArccsc);
    trigonometry_toolbar->addAction(action);
}

void MainWindow::CreateHyperbolicToolbar()
{
    //hyperbolic toolbar
    hyperbolic_toolbar = addToolBar(tr("Hyperbolic functions"));
    hyperbolic_toolbar->setObjectName("hyperbolic_toolbar");
    hyperbolic_toolbar->setStyleSheet("QToolBar{spacing:4px;}");

    QAction* action = new QAction(QIcon(":/icons/images/hyperbolic/sinh.png"), tr("Hyperbolic sine"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnSinh);
    hyperbolic_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/hyperbolic/cosh.png"), tr("Hyperbolic cosine"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnCosh);
    hyperbolic_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/hyperbolic/tgh.png"), tr("Hyperbolic tangens"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnTgh);
    hyperbolic_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/hyperbolic/ctgh.png"), tr("Hyperbolic cotangens"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnCtgh);
    hyperbolic_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/hyperbolic/sech.png"), tr("Hyperbolic secans"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnSech);
    hyperbolic_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/hyperbolic/csch.png"), tr("Hyperbolic cosecans"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnCsch);
    hyperbolic_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/hyperbolic/arsinh.png"), tr("Hyperbolic arsine"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnArsinh);
    hyperbolic_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/hyperbolic/arcosh.png"), tr("Hyperbolic arcosine"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnArsinh);
    hyperbolic_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/hyperbolic/artgh.png"), tr("Hyperbolic artangens"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnArtgh);
    hyperbolic_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/hyperbolic/arctgh.png"), tr("Hyperbolic arcotangens"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnArctgh);
    hyperbolic_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/hyperbolic/arsech.png"), tr("Hyperbolic arsecans"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnArsech);
    hyperbolic_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/hyperbolic/arcsch.png"), tr("Hyperbolic arcosecans"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnArcsch);
    hyperbolic_toolbar->addAction(action);
}

void MainWindow::CreateFunctionsToolbar()
{
    //functions toolbar
    functions_toolbar = addToolBar(tr("Hyperbolic functions"));
    functions_toolbar->setObjectName("functions_toolbar");
    functions_toolbar->setStyleSheet("QToolBar{spacing:4px;}");

    QAction* action = new QAction(QIcon(":/icons/images/functions/exp.png"), tr("Exponent"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnExp);
    functions_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/functions/ln.png"), tr("Natural logarithm"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnLn);
    functions_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/functions/lg.png"), tr("Decimal logarithm"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnLg);
    functions_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/functions/log.png"), tr("Logarithm"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnLog);
    functions_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/functions/int.png"), tr("Integer part"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnInt);
    functions_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/functions/fract.png"), tr("Fraction part"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnFract);
    functions_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/functions/round.png"), tr("Round"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnRound);
    functions_toolbar->addAction(action);
}

void MainWindow::CreateGreekToolbar()
{
    //greek toolbar
    greek_toolbar = addToolBar(tr("Greek letters"));
    greek_toolbar->setObjectName("greek_toolbar");
    greek_toolbar->setStyleSheet("QToolBar{spacing:4px;}");

    auto add_greek_letter = 
        [greek_toolbar = greek_toolbar, this](const QChar& letter)
        {
            QAction* action = new QAction(letter, this);
            action->setData(letter);
            action->setToolTip(tr(QString(letter).toUtf8().data()));
            connect(action, &QAction::triggered, this, &MainWindow::OnGreekLetter);
            greek_toolbar->addAction(action);
        };

    add_greek_letter(L'α');
    add_greek_letter(L'β');
    add_greek_letter(L'γ');
    add_greek_letter(L'δ');
    add_greek_letter(L'ε');
    add_greek_letter(L'ζ');
    add_greek_letter(L'η');
    add_greek_letter(L'θ');
    add_greek_letter(L'ι');
    add_greek_letter(L'κ');
    add_greek_letter(L'λ');
    add_greek_letter(L'μ');
    add_greek_letter(L'ν');
    add_greek_letter(L'ξ');
    add_greek_letter(L'ο');
    add_greek_letter(L'π');
    add_greek_letter(L'ρ');
    add_greek_letter(L'σ');
    add_greek_letter(L'τ');
    add_greek_letter(L'υ');
    add_greek_letter(L'φ');
    add_greek_letter(L'χ');
    add_greek_letter(L'ψ');
    add_greek_letter(L'ω');

    add_greek_letter(L'Α');
    add_greek_letter(L'Β');
    add_greek_letter(L'Γ');
    add_greek_letter(L'Δ');
    add_greek_letter(L'Ε');
    add_greek_letter(L'Ζ');
    add_greek_letter(L'Η');
    add_greek_letter(L'Θ');
    add_greek_letter(L'Ι');
    add_greek_letter(L'Κ');
    add_greek_letter(L'Λ');
    add_greek_letter(L'Μ');
    add_greek_letter(L'Ν');
    add_greek_letter(L'Ξ');
    add_greek_letter(L'Ο');
    add_greek_letter(L'Π');
    add_greek_letter(L'Ρ');
    add_greek_letter(L'Σ');
    add_greek_letter(L'Τ');
    add_greek_letter(L'Υ');
    add_greek_letter(L'Φ');
    add_greek_letter(L'Χ');
    add_greek_letter(L'Ψ');
    add_greek_letter(L'Ω');
}

void MainWindow::CreateCurrenciesToolbar()
{
    currency_toolbar = addToolBar(tr("Currencies"));
    currency_toolbar->setObjectName("currency_toolbar");
    currency_toolbar->setStyleSheet("QToolBar{spacing:4px;}");

    auto add_currency = 
        [currency_toolbar = currency_toolbar, this](const QString& symbol, const QString& tooltip)
        {
            QAction* action = new QAction(symbol, this);
            action->setToolTip(tooltip);
            action->setData(symbol);
            connect(action, &QAction::triggered, this, &MainWindow::OnCurrency);
            currency_toolbar->addAction(action);
        };

    add_currency("R$", tr("Brazilian real"));
    add_currency("¥", tr("Chinese yuan"));
    add_currency("€", tr("Euro"));
    add_currency("₹", tr("Indian rupee"));
    add_currency("₽", tr("Russian ruble"));
    add_currency("$", tr("US dollar"));
}

void MainWindow::CreateGraphsToolbar()
{
    //graphs toolbar
    graph_toolbar = addToolBar(tr("Graphs"));
    graph_toolbar->setObjectName("graph_toolbar");
    graph_toolbar->setStyleSheet("QToolBar{spacing:4px;}");

    QAction* action = new QAction(QIcon(":/icons/images/graphs/graph_line.png"), tr("Line graph"), this);
    connect(action, &QAction::triggered, this, &MainWindow::GraphLine);
    graph_toolbar->addAction(action);
}

void MainWindow::CreateLogicalToolbar()
{
    //logical operations toolbar
    logical_toolbar = addToolBar(tr("Logical operations"));
    logical_toolbar->setObjectName("logical_toolbar");
    logical_toolbar->setStyleSheet("QToolBar{spacing:4px;}");

    QAction* action = new QAction(QIcon(":/icons/images/logical/and.png"), tr("Logical AND"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnAnd);
    logical_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/logical/or.png"), tr("Logical OR"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnOr);
    logical_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/logical/xor.png"), tr("Logical XOR"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnXor);
    logical_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/logical/not.png"), tr("Logical NOT"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnNot);
    logical_toolbar->addAction(action);
}

void MainWindow::CreateStatusBar()
{
    if (!locale_status)
    {
        locale_status = new QLabel("");
        statusBar()->addWidget(locale_status);
    }
    if (!status_separator)
    {
        status_separator = new QFrame();
        status_separator->setFrameShape(QFrame::VLine);
        status_separator->setFrameShadow(QFrame::Sunken);
        statusBar()->addWidget(status_separator);
    }
    if (!document_online_label)
    {
        document_online_label = new QLabel("");
        document_online_label->setTextFormat(Qt::RichText);
        document_online_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
        document_online_label->setOpenExternalLinks(true);
        statusBar()->addWidget(document_online_label);
    }
}

void MainWindow::SetFocus()
{
    DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->currentWidget();
    if (w)
        w->SetFocus();
}

void MainWindow::New()
{
    AddEditorTab(tr("(No name)"), "");
}

void MainWindow::Open()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open file"), "", tr("Yutovo files (*.yut);;Text files (*.txt);;All files (*.*)"));
    if (file_name == "")
        return;
    OpenFile(file_name);
}

void MainWindow::OpenRecentFile()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action)
        return;
    QString file_name = action->data().toString();
    if (file_name == "")
        return;
    OpenFile(file_name);
}

void MainWindow::OpenLibraryFile()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action)
        return;
    QString file_name = action->data().toString();
    if (file_name == "")
        return;
    OpenFile(file_name);
}

void MainWindow::OpenFile(QString file_name)
{
    try
    {
#ifdef _WIN32
        if (!std::filesystem::exists(ToWString(file_name.toUtf8().data())))
        {
            QMessageBox::critical(this, tr("Yutovo"), tr("Error loading document") + QString(": ") + file_name);
            return;
        }
        auto p = std::filesystem::absolute(ToWString(file_name.toUtf8().data()));
        file_name = ToBasicString(std::filesystem::canonical(p).wstring()).c_str();
#else
        if (!std::filesystem::exists(file_name.toUtf8().data()))
        {
            QMessageBox::critical(this, tr("Yutovo"), tr("File not found") + QString(": ") + file_name);
            return;
        }
        std::filesystem::path path = std::filesystem::u8path(file_name.toUtf8().constData());
        std::filesystem::path abs_path = std::filesystem::canonical(std::filesystem::absolute(path));
        file_name = QString::fromStdString(abs_path.u8string());
#endif
    }
    catch (const std::filesystem::filesystem_error& ex)
    {
        QMessageBox::critical(this, tr("Yutovo"), tr("Error loading document") + QString(": ") + file_name + 
            QString("\n") + ex.what());
        return;
    }

    dialog_file_name = file_name;

    for (int i = 0; i < ui->editor_tabs->count(); ++i)
    {
        DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->widget(i);
        if (w->path == file_name)
        {
            ui->editor_tabs->setCurrentIndex(i);
            return;
        }
    }

    {
        //close the current document if it is empty
        auto document = GetCurrentDocument();
        if (document && !document->CanUndo() && !document->CanRedo())
        {
            document.reset();
            DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->currentWidget();
            if (w->path == "")
                OnCloseEditorTab(ui->editor_tabs->currentIndex());
        }
    }

    //open new tab with document
    QFileInfo file_info(file_name);
    AddEditorTab(file_info.fileName(), file_info.canonicalFilePath());

    auto document = GetCurrentDocument();
    if (!document)
        return;
    
    DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->currentWidget();
    w->document_widget->loading = true;

    loading_files[document->Load(file_name.toUtf8().data())] = ui->editor_tabs->currentIndex();

    if (!w->document_widget->formating && !w->document_widget->resizing)
        QApplication::setOverrideCursor(Qt::WaitCursor);
    w->path = file_name;

    UpdateLocaleMessage();
}

void MainWindow::Save()
{
    SaveFile(ui->editor_tabs->currentIndex());
}

void MainWindow::SaveFile(int index)
{
    DocumentWindow* w = nullptr;
    if (index != -1)
        w = (DocumentWindow*)ui->editor_tabs->widget(index);
    else
        w = (DocumentWindow*)ui->editor_tabs->currentWidget();
    auto document = w->document;
    if (!document)
        return;
    if (w->path == "")
        SaveFileAs(index);
    else
    {
        dialog_file_name = w->path;
        save_tasks[document->Save(w->path.toUtf8().data())] = index;
    }

    UpdateCaption();
}

void MainWindow::SaveFileAsName()
{
    SaveFileAs(ui->editor_tabs->currentIndex());
}

void MainWindow::SaveFileAs(int index)
{
    DocumentWindow* w = nullptr;
    if (index != -1)
        w = (DocumentWindow*)ui->editor_tabs->widget(index);
    else
        w = (DocumentWindow*)ui->editor_tabs->currentWidget();
    if (!w || !w->document)
        return;
    QFileDialog save_dialog(this, tr("Save file as"), "", tr("Yutovo files (*.yut);;Text files (*.txt)"));
    save_dialog.setDefaultSuffix("yut");
    save_dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (!save_dialog.exec())
        return;
    QStringList file_names = save_dialog.selectedFiles();
    if (file_names.empty())
        return;
    dialog_file_name = file_names[0];
    w->document->Save(file_names[0].toUtf8().data());

    UpdateCaption();
}

void MainWindow::SaveAll()
{
    for (int i = 0; i < ui->editor_tabs->count(); ++i)
        SaveFile(i);
}

void MainWindow::Close()
{
    int p = ui->editor_tabs->currentIndex();
    if (p == -1)
        return;
    OnCloseEditorTab(p);
}

void MainWindow::CloseAll()
{
    for (int i = 0; i < ui->editor_tabs->count();)
    {
        if (!OnCloseEditorTab(0))
            return;
    }
}

void MainWindow::CloseOthers()
{
    int p = ui->editor_tabs->currentIndex();
    for (int i = 0; i < p; ++i)
    {
        if (!OnCloseEditorTab(0))
            return;
    }
    for (int i = 1; i < ui->editor_tabs->count();)
    {
        if (!OnCloseEditorTab(1))
            return;
    }
}

void MainWindow::ExportToHtml()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save file as"), "", tr("Html files (*.html)"));
    if (path.isEmpty())
        return;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Error"), tr("Cannot save file"));
        return;
    }
    
    auto document = GetCurrentDocument();
    if (!document)
        return;
    std::string html = "<!DOCTYPE html>\n";
    html += "<meta charset=\"UTF-8\">\n";
    html += document->ToHtml() + "\n";
    size_t p = html.find("</body>");
    if (p != std::string::npos)
    {
        std::string footer = 
            "<p>\n"\
                "<hr>"\
                "<span style=\"font-family:'Arial';font-size:12px;\">" + tr("This document was created with ").toStdString() + "</span>\n"\
                "<a href=\"https://yutovo.com?ref=html_export\" style=\"font-family:'Arial';font-size:12px;\">Yutovo</a>\n"\
                "<span style=\"font-family:'Arial';font-size:12px;\">.</span>\n"\
            "</p>";
        html.insert(p, footer);
    }

    file.write(html.c_str());
    file.close();
}

void MainWindow::ExportToPdf()
{
    DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->currentWidget();
    if (!w)
        return;

    TextFormat f;
    w->document->GetTextFormat(f);
    ExportPdfDialog export_pdf_dialog({(qreal)f.left_indent, (qreal)f.top_indent, (qreal)f.right_indent, (qreal)f.bottom_indent});
    if (export_pdf_dialog.exec() == QDialog::Accepted)
    {
        dialog_file_name = export_pdf_dialog.FilePath();
        Config config;
        w->document->GetConfig(config);
        config.with_border = false;
        config.code_block_border = true;
        config.caret_visible = false;
        config.hilight_caret_element = false;
        config.draw_whole = true;
        config.pdf = true;

        QMarginsF m = export_pdf_dialog.Margins();
        f.left_indent = (int)m.left();
        f.top_indent = (int)m.top();
        f.right_indent = (int)m.right();
        f.bottom_indent = (int)m.bottom();

        QSizeF s = export_pdf_dialog.GetPageSize();
        pdf_window.reset(new QtPdfWindow({(int)(s.width() * 72 / 25.4), (int)(s.height() * 72 / 25.4)}));
        connect(pdf_window.get(), &QtPdfWindow::PdfExportResult, this, &MainWindow::OnPdfExportResult);
        Document pdf_document(pdf_window.get(), config, *w->document.get());
        pdf_document.Start(f);
    }
}

void MainWindow::Settings()
{
    QHash<QString, QVariant> _settings;
    QStringList keys = settings.allKeys();
    Q_FOREACH(QString key, keys)
    {
        _settings[key] = settings.value(key);
    }

    yutovo_calculator::Language last_language = config.language;
    Config _config = config;
    int r;
    {
        SettingsDialog settings_dialog(_config, _settings);
        r = settings_dialog.exec();
    }

    if (r == QDialog::Accepted)
    {
        config = _config;
        config.auto_result.real_result = config.real_result;
        config.auto_result.integer_result = config.integer_result;
        config.auto_result.rational_result = config.rational_result;
        config.auto_result.complex_result = config.complex_result;
        
        for (int i = 0; i < ui->editor_tabs->count(); ++i)
        {
            DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->widget(i);
            Config c;
            w->document->GetConfig(c);
            //update all but language and result settings
            _config.language = c.language;
            _config.auto_result = c.auto_result;
            _config.real_result = c.real_result;
            _config.integer_result = c.integer_result;
            _config.rational_result = c.rational_result;
            _config.complex_result = c.complex_result;
            w->document->SetConfig(_config, false);
        }

        keys = _settings.keys();
        Q_FOREACH(QString key, keys)
        {
            settings.setValue(key, _settings.value(key));
        }

        standard_toolbar_action->setChecked(settings.value("MainWindow/standard_toolbar", true).toBool());
        format_toolbar_action->setChecked(settings.value("MainWindow/format_toolbar", true).toBool());
        algebra_toolbar_action->setChecked(settings.value("MainWindow/algebra_toolbar", true).toBool());
        trigonometry_toolbar_action->setChecked(settings.value("MainWindow/trigonometry_toolbar", false).toBool());
        hyperbolic_toolbar_action->setChecked(settings.value("MainWindow/hyperbolic_toolbar", false).toBool());
        functions_toolbar_action->setChecked(settings.value("MainWindow/functions_toolbar", false).toBool());
        graph_toolbar_action->setChecked(settings.value("MainWindow/graphs_toolbar", false).toBool());
        greek_toolbar_action->setChecked(settings.value("MainWindow/greek_toolbar", false).toBool());
        logical_toolbar_action->setChecked(settings.value("MainWindow/logical_toolbar", false).toBool());

        if (last_language != config.language)
        {
            InstallTranslation(config.language);
            UpdateLocaleMessage();
            for (int i = 0; i < ui->editor_tabs->count(); ++i)
            {
                DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->widget(i);
                w->CreateMenus();
            }
        }
        
        logger->SetLevel(config.log_level);
    }
}

void MainWindow::Exit()
{
    close();
}

void MainWindow::Copy()
{
    auto document = GetCurrentDocument();
    if (document)
        document->Copy(clipboard_json, clipboard_text);
}

void MainWindow::Paste()
{
    auto document = GetCurrentDocument();
    if (!document)
        return;
    
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* mime_data = clipboard->mimeData();

    std::vector<std::string> v;
    QStringList l = mime_data->formats();
    for (auto s : l)
        v.push_back(s.toStdString());

    std::string format;
    if (mime_data->hasFormat("application/web;type=\"custom/formatmap\"")) //firstly check the own web format
    {
        QByteArray arr = mime_data->data("application/web;type=\"custom/formatmap\"");
        if (!arr.isEmpty())
        {
            std::stringstream str(arr.toStdString());
            std::string s(str.str());
            rapidjson::Document doc;
            if (!doc.Parse<0>(str.str().c_str()).HasParseError())
            {
                if (doc.HasMember("yutovo/elements") && doc["yutovo/elements"].IsString())
                    format = doc["yutovo/elements"].GetString();
            }
        }
    }

    if (!format.empty() && mime_data->hasFormat(format.c_str()))
    {
        QByteArray arr = mime_data->data(format.c_str());
        if (arr.isEmpty())
        {
            //otherwise check if there is a text in the clipboard
            QString s;
            if (mime_data->hasText())
                s = mime_data->text();
            if (s == "")
                return;
            document->PasteText(yutovo::ToUtfString(s.toUtf8().data()));
            return;
        }
        std::stringstream str(arr.toStdString());
        auto s = yutovo::ToUtfString(str.str());
        document->Paste(s);
    }
    else if (mime_data->hasImage() || mime_data->hasFormat("image/png") || mime_data->hasFormat("image/jpeg"))
    {
        QImage image = clipboard->image();
        QByteArray arr;
        QBuffer buffer(&arr);
        buffer.open(QIODevice::WriteOnly);
        if (!image.save(&buffer, "PNG"))
        {
            logger->Error("Error converting image");
            return;
        }
        std::vector<unsigned char> data(arr.begin(), arr.end());
        document->PasteImage(data);
    }
    else if (mime_data->hasUrls())
    {
        for (auto& url : mime_data->urls())
        {
            if (url.isLocalFile())
            {
                QString path = url.toLocalFile();
                QImage image(path);
                if (!image.isNull())
                {
                    QByteArray arr;
                    QBuffer buffer(&arr);
                    buffer.open(QIODevice::WriteOnly);
                    if (!image.save(&buffer, "PNG"))
                    {
                        logger->Error("Error converting image");
                        return;
                    }
                    std::vector<unsigned char> data(arr.begin(), arr.end());
                    document->PasteImage(data);
                }
            }
        }        
    }
    else if (mime_data->hasText())
    {
        QString s = clipboard->text();
        if (s != "")
            document->PasteText(ToUtfString(s.toUtf8().data()));
    }
}

void MainWindow::Cut()
{
    auto document = GetCurrentDocument();
    if (document)
        document->Cut(clipboard_json, clipboard_text);
}

void MainWindow::Link()
{
    auto document = GetCurrentDocument();
    if (!document)
        return;

    EditorState s = document->GetEditorState();
    std::u32string str, url;
    if (!document->GetLink(s.caret_state.id, str, url))
        return;
    
    LinkDialog dialog(ToBasicString(str).c_str(), ToBasicString(url).c_str(), tr("Change link"));
    if (!dialog.exec())
        return;
    
    document->InsertLink(dialog.text.toStdString(), dialog.url.toStdString(), true);
}

void MainWindow::Graph()
{
    auto document = GetCurrentDocument();
    if (!document)
        return;

    EditorState s = document->GetEditorState();
    GraphFormat f;
    if (!document->GetGraphFormat(yutovo::GetParent(s.caret_state.id), f))
        return;
    GraphSettingsDialog dialog(f);
    if (!dialog.exec())
        return;
    document->SetGraphFormat(yutovo::GetParent(s.caret_state.id), f, true);
}

void MainWindow::Undo()
{
    auto document = GetCurrentDocument();
    if (document)
        document->Undo();
}

void MainWindow::Redo()
{
    auto document = GetCurrentDocument();
    if (document)
        document->Redo();
}

void MainWindow::Properties()
{
    auto document = GetCurrentDocument();
    if (!document)
        return;
    
    Config _config;
    document->GetConfig(_config);
    int r;
    {
        PropertiesDialog settings_dialog(_config);
        r = settings_dialog.exec();
    }

    if (r == QDialog::Accepted)
        document->SetConfig(_config, true);

    UpdateLocaleMessage();
}

void MainWindow::Recalculate()
{
    auto document = GetCurrentDocument();
    if (document)
        document->ReSolve(ElementId{});
}

void MainWindow::TermsOfUse()
{
    TermsOfUseDialog terms_of_use_dialog;
    terms_of_use_dialog.exec();
}

void MainWindow::PrivacyPolicy()
{
    PrivacyPolicyDialog privacy_policy_dialog;
    privacy_policy_dialog.exec();
}

void MainWindow::About()
{
    AboutDialog about_dialog;
    about_dialog.exec();
}

void MainWindow::WhatsNew()
{
    WhatsNewDialog whats_new_dialog(LanguageToString(config.language), this);
    whats_new_dialog.exec();
}

void MainWindow::CheckVersionAndShowWhatsNew()
{
    QString saved_version = settings.value("MainWindow/version").toString();
    if (saved_version != APP_VERSION)
    {
        WhatsNewDialog dialog(LanguageToString(config.language), this);
        dialog.exec();
        settings.setValue("MainWindow/version", APP_VERSION);
    }
}

void MainWindow::StandardToolbar()
{
    standard_toolbar_action->isChecked() ? standard_toolbar->show() : standard_toolbar->hide();
    settings.setValue("MainWindow/standard_toolbar", standard_toolbar_action->isChecked());
}

void MainWindow::FormatToolbar()
{
    format_toolbar_action->isChecked() ? format_toolbar->show() : format_toolbar->hide();
    settings.setValue("MainWindow/format_toolbar", format_toolbar_action->isChecked());
}

void MainWindow::AlgebraToolbar()
{
    algebra_toolbar_action->isChecked() ? algebra_toolbar->show() : algebra_toolbar->hide();
    settings.setValue("MainWindow/algebra_toolbar", algebra_toolbar_action->isChecked());
}

void MainWindow::TrigonometryToolbar()
{
    trigonometry_toolbar_action->isChecked() ? trigonometry_toolbar->show() : trigonometry_toolbar->hide();
    settings.setValue("MainWindow/trigonometry_toolbar", trigonometry_toolbar_action->isChecked());
}

void MainWindow::HyperbolicToolbar()
{
    hyperbolic_toolbar_action->isChecked() ? hyperbolic_toolbar->show() : hyperbolic_toolbar->hide();
    settings.setValue("MainWindow/hyperbolic_toolbar", hyperbolic_toolbar_action->isChecked());
}

void MainWindow::FunctionsToolbar()
{
    functions_toolbar_action->isChecked() ? functions_toolbar->show() : functions_toolbar->hide();
    settings.setValue("MainWindow/functions_toolbar", functions_toolbar_action->isChecked());
}

void MainWindow::GreekToolbar()
{
    greek_toolbar_action->isChecked() ? greek_toolbar->show() : greek_toolbar->hide();
    settings.setValue("MainWindow/greek_toolbar", greek_toolbar_action->isChecked());
}

void MainWindow::CurrencyToolbar()
{
    currency_toolbar_action->isChecked() ? currency_toolbar->show() : currency_toolbar->hide();
    settings.setValue("MainWindow/currency_toolbar", currency_toolbar_action->isChecked());
}

void MainWindow::GraphToolbar()
{
    graph_toolbar_action->isChecked() ? graph_toolbar->show() : graph_toolbar->hide();
    settings.setValue("MainWindow/graphs_toolbar", graph_toolbar_action->isChecked());
}

void MainWindow::LogicalToolbar()
{
    logical_toolbar_action->isChecked() ? logical_toolbar->show() : logical_toolbar->hide();
    settings.setValue("MainWindow/logical_toolbar", logical_toolbar_action->isChecked());
}

void MainWindow::StatusBar()
{
    status_bar_action->isChecked() ? statusBar()->show() : statusBar()->hide();
    settings.setValue("MainWindow/status_bar", status_bar_action->isChecked());
}

void MainWindow::OnNextEditorTab()
{
    int c = ui->editor_tabs->currentIndex();
    if (c == ui->editor_tabs->count() - 1)
        ui->editor_tabs->setCurrentIndex(0);
    else
        ui->editor_tabs->setCurrentIndex(c + 1);
}

void MainWindow::OnPrevEditorTab()
{
    int c = ui->editor_tabs->currentIndex();
    if (c == 0)
        ui->editor_tabs->setCurrentIndex(ui->editor_tabs->count() - 1);
    else
        ui->editor_tabs->setCurrentIndex(c - 1);
}

void MainWindow::OnScaleChanged(const float scale)
{
    scale_combo->setCurrentText(QString::number(std::round(scale * 100)) + "%");
}

bool MainWindow::OnCloseEditorTab(int index)
{
    if (index == -1)
        return false;

    DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->widget(index);
    if (w->document->IsChanged())
    {
        QString file_name;
        QFileInfo file_info(w->path);
        if (file_info.fileName().isEmpty())
            file_name = tr("(No name)");
        else
            file_name = file_info.fileName();

        QMessageBox m(QMessageBox::Question, tr("Yutovo"), tr("Document %1 is unsaved. Save?").arg(file_name), 
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);
        int r = m.exec();
        switch (r)
        {
        case QMessageBox::Yes:
            close_tab_after_save = index;
            SaveFile(index);
            return true;
        case QMessageBox::No:
            break;
        default:
            UpdateCaption();
            SetFocus();
            return false;
        }
    }

    int c = ui->editor_tabs->currentIndex();
    QWidget* tab_item = ui->editor_tabs->widget(index);
    if (tab_item)
    {
        ui->editor_tabs->removeTab(index);
        tab_item->setParent(nullptr);
        delete(tab_item);
    }
    SetFocus();

    if (index == c)
        UpdateCaption();
    return true;
}

void MainWindow::OnEditorChanged(int index)
{
    auto document = GetCurrentDocument();
    if (!document)
        return;

    FillParagraphFormats();
    FillScales();
    OnCaretMoved(document->GetEditorState());

    UpdateCaption();
    UpdateLocaleMessage();
}

void MainWindow::OnLinkClicked(const std::u32string& url)
{
    //if this is a file, open it
    DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->currentWidget();
    if (!w)
        return;
    int p = w->path.lastIndexOf('/');
    if (p == -1)
        return;
    QString s = w->path.left(p + 1);
    s += ToBasicString(url).c_str();
    if (!s.endsWith(".yut"))
        s += ".yut";
    if (QFile::exists(s))
    {
        OpenFile(s);
        return;
    }

    QUrl u(ToBasicString(url).c_str());
    QDesktopServices::openUrl(u);
}

void MainWindow::OnInsertCode()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertCode(false, true);
}

void MainWindow::OnCurrentScaleChanged(int index)
{
    if (block_scale_slots)
        return;
    auto document = GetCurrentDocument();
    if (!document)
        return;

    QString s = scale_combo->currentText().trimmed();
    s.remove('%');
    bool ok = false;
    int scale = s.toInt(&ok);
    if (ok && scale >= 50 && scale <= 500)
    {
        Config c;
        document->GetConfig(c);
        c.scale = scale / 100.;
        document->SetConfig(c, false);
    }
}

void MainWindow::OnCurrentScaleEditingFinished()
{
    OnCurrentScaleChanged(scale_combo->currentIndex());
}

void MainWindow::OnCurrentParagraphFormatChanged(const QString& format)
{
    if (block_format_slots)
        return;
    auto document = GetCurrentDocument();
    if (!document)
        return;
    document->SetCurrentParagraphFormat(format.toUtf8().data());
    SetFocus();
}

void MainWindow::OnCurrentFontChanged(const QFont& font)
{
    if (block_format_slots)
        return;
    auto document = GetCurrentDocument();
    if (!document)
        return;
    FillSizes(font);
    document->SetFontFamily(font.family().toUtf8().data());
    SetFocus();
}

void MainWindow::OnCurrentSizeEditingFinished()
{
    int cur_size = size_combo->currentText().toInt();
    for (int i = 0; i < size_combo->count(); ++i)
    {
        int s = size_combo->itemText(i).toInt();
        if (s == cur_size)
        {
            size_combo->setCurrentIndex(i);
            return;
        }
        else if (s > cur_size)
        {
            size_combo->insertItem(i, size_combo->currentText());
            size_combo->setCurrentIndex(i);
            return;
        }
    }
    size_combo->insertItem(size_combo->count(), size_combo->currentText());
    size_combo->setCurrentIndex(size_combo->count() - 1);
}

void MainWindow::OnCurrentSizeChanged(int index)
{
    if (block_format_slots)
        return;
    UpdateFontSize();
}

void MainWindow::OnAlignLeft()
{
    auto document = GetCurrentDocument();
    if (!document)
        return;
    if (!align_left_action->isChecked())
        OnCaretMoved(document->GetEditorState());
    document->ChangeParagraphFormat(ParagraphFormat::Alignment::Left, true);
}

void MainWindow::OnAlignRight()
{
    auto document = GetCurrentDocument();
    if (!document)
        return;
    if (!align_right_action->isChecked())
        OnCaretMoved(document->GetEditorState());
    document->ChangeParagraphFormat(ParagraphFormat::Alignment::Right, true);
}

void MainWindow::OnAlignCenter()
{
    auto document = GetCurrentDocument();
    if (!document)
        return;
    if (!align_center_action->isChecked())
        OnCaretMoved(document->GetEditorState());
    document->ChangeParagraphFormat(ParagraphFormat::Alignment::Center, true);
}

void MainWindow::OnAlignJustify()
{
    auto document = GetCurrentDocument();
    if (!document)
        return;
    if (!align_justify_action->isChecked())
        OnCaretMoved(document->GetEditorState());
    document->ChangeParagraphFormat(ParagraphFormat::Alignment::Justify, true);
}

void MainWindow::OnBold()
{
    if (block_format_slots)
        return;
    auto document = GetCurrentDocument();
    if (document)
        document->SetBold(bold_action->isChecked());
}

void MainWindow::OnItalic()
{
    if (block_format_slots)
        return;
    auto document = GetCurrentDocument();
    if (document)
        document->SetItalic(italic_action->isChecked());
}

void MainWindow::OnUnderline()
{
    if (block_format_slots)
        return;
    auto document = GetCurrentDocument();
    if (document)
        document->SetUnderline(underline_action->isChecked());
}

void MainWindow::OnStrikethrough()
{
    if (block_format_slots)
        return;
    auto document = GetCurrentDocument();
    if (document)
        document->SetStrikethrough(strikethrough_action->isChecked());
}

void MainWindow::OnTextSubscript()
{
    superscript_action->setChecked(false);
    if (block_format_slots)
        return;
    auto document = GetCurrentDocument();
    if (document)
        document->SetSubscript(subscript_action->isChecked());
}

void MainWindow::OnTextSuperscript()
{
    subscript_action->setChecked(false);
    if (block_format_slots)
        return;
    auto document = GetCurrentDocument();
    if (document)
        document->SetSuperscript(superscript_action->isChecked());
}

void MainWindow::OnTextColor()
{
    QColorDialog d(QColor::fromRgba(string_format.text_color.ToInt()), this);
    if (d.exec() == QDialog::Accepted)
    {
        auto document = GetCurrentDocument();
        if (document)
        {
            QColor c = d.selectedColor();
            document->SetColor(Color{ (uint8_t)c.alpha(), (uint8_t)c.red(), (uint8_t)c.green(), (uint8_t)c.blue() });
        }
    }
}

void MainWindow::OnBgTextColor()
{
    QColorDialog d(QColor::fromRgba(string_format.text_bg_color.ToInt()), this);
    if (d.exec() == QDialog::Accepted)
    {
        auto document = GetCurrentDocument();
        if (document)
        {
            QColor c = d.selectedColor();
            document->SetBgColor(Color{ (uint8_t)c.alpha(), (uint8_t)c.red(), (uint8_t)c.green(), (uint8_t)c.blue() });
        }
    }
}

void MainWindow::OnLink()
{
    auto document = GetCurrentDocument();
    if (!document)
        return;

    EditorState s = document->GetEditorState();
    std::u32string str, url;
    if (document->GetLink(s.caret_state.id, str, url))
    {
        LinkDialog dialog(ToBasicString(str).c_str(), ToBasicString(url).c_str(), tr("Change link"));
        if (!dialog.exec())
            return;
        document->InsertLink(dialog.text.toStdString(), dialog.url.toStdString(), true);
    }
    else
    {
        if (s.selection_state.state.size() == 1)
        {
            auto t = document->GetElementType(s.caret_state.id);
            if (t == ElementType::STRING)
            {
                str = document->ToText(s.caret_state.id);
                ElementSelectionState& el_s = s.selection_state.state[0];
                if (el_s.id == yutovo::GetParent(s.caret_state.id) && str.length() >= el_s.start + el_s.size)
                    str = str.substr(el_s.start, el_s.size);
            }
        }
        LinkDialog dialog(ToBasicString(str).c_str(), "", tr("Insert link"));
        if (!dialog.exec())
            return;
        document->InsertLink(dialog.text.toStdString(), dialog.url.toStdString(), true);
    }
}

void MainWindow::OnPlus()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertPlus(true);
}

void MainWindow::OnMinus()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertMinus(true);
}

void MainWindow::OnMultiply()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertMultiply(true);
}

void MainWindow::OnDivision()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertDivision(true);
}

void MainWindow::OnSquareRoot()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertSquareRoot(true);
}

void MainWindow::OnNthRoot()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertNthRoot(true);
}

void MainWindow::OnPower()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertPower(true);
}

void MainWindow::OnSubscript()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertSubscript(true);
}

void MainWindow::OnSum()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertSum(true);
}

void MainWindow::OnProduct()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertProduct(true);
}

void MainWindow::OnExp()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("exp", true);
}

void MainWindow::OnLn()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("ln", true);
}

void MainWindow::OnLg()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("lg", true);
}

void MainWindow::OnRoundBrackets()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertRoundBrackets(true);
}

void MainWindow::OnSquareBrackets()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertSquareBrackets(true);
}

void MainWindow::OnRadian()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertString(tr("rad").toUtf8().data(), true);
}

void MainWindow::OnDegree()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertString("°", true);
}

void MainWindow::OnMinute()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertString("'", true);
}

void MainWindow::OnSecond()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertString("''", true);
}

void MainWindow::OnGrad()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertString(tr("grad").toUtf8().data(), true);
}

void MainWindow::OnLog()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertSubscriptFunction("log", true);
}

void MainWindow::OnInt()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("int", true);
}

void MainWindow::OnFract()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("fract", true);
}

void MainWindow::OnRound()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("round", true);
}

void MainWindow::OnEquation()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertEquation(ResultType::AUTO, true);
}

void MainWindow::OnAssignment()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertAssignment(true);
}

void MainWindow::OnUnit()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertUnit(true);
}

void MainWindow::OnInfinity()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertString("∞", true);
}

void MainWindow::OnSin()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("sin", true);
}

void MainWindow::OnCos()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("cos", true);
}

void MainWindow::OnTg()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("tg", true);
}

void MainWindow::OnCtg()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("ctg", true);
}

void MainWindow::OnSec()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("sec", true);
}

void MainWindow::OnCsc()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("csc", true);
}

void MainWindow::OnArcsin()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("arcsin", true);
}

void MainWindow::OnArccos()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("arccos", true);
}

void MainWindow::OnArctg()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("arctg", true);
}

void MainWindow::OnArcctg()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("arcctg", true);
}

void MainWindow::OnArcsec()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("arcsec", true);
}

void MainWindow::OnArccsc()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("arccsc", true);
}

void MainWindow::OnSinh()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("sinh", true);
}

void MainWindow::OnCosh()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("cosh", true);
}

void MainWindow::OnTgh()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("tgh", true);
}

void MainWindow::OnCtgh()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("ctgh", true);
}

void MainWindow::OnSech()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("sech", true);
}

void MainWindow::OnCsch()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("csch", true);
}

void MainWindow::OnArsinh()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("arsinh", true);
}

void MainWindow::OnArcosh()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("arcosh", true);
}

void MainWindow::OnArtgh()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("artgh", true);
}

void MainWindow::OnArctgh()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("arctgh", true);
}

void MainWindow::OnArsech()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("arsech", true);
}

void MainWindow::OnArcsch()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFunction("arcsch", true);
}

void MainWindow::OnGreekLetter()
{
    QAction* action = qobject_cast<QAction*>(sender());
    QVariant v = action->data();
    QChar letter = v.toChar();
    auto document = GetCurrentDocument();
    if (document)
        document->InsertString(QString(letter).toStdString(), true);
}

void MainWindow::OnCurrency()
{
    QAction* action = qobject_cast<QAction*>(sender());
    QVariant v = action->data();
    QString symbol = v.toString();
    auto document = GetCurrentDocument();
    if (document)
        document->InsertString(symbol.toStdString(), true);
}

void MainWindow::GraphLine()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertGraph(true);
}

void MainWindow::OnAnd()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertString("&", true);
}

void MainWindow::OnOr()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertString("|", true);
}

void MainWindow::OnXor()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertString("^", true);
}

void MainWindow::OnNot()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertString("!", true);
}

void MainWindow::OnCaretMoved(const EditorState editor_state)
{
    DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->currentWidget();
    if (!w)
        return;

    QtWindow& window = w->document_widget->window;
    const CaretState& c = window.editor_state.caret_state;
    const SelectionState& s = window.editor_state.selection_state;
    ParagraphFormat paragraph_format = window.common_paragraph_format;

    bold_action->setEnabled(true);
    italic_action->setEnabled(true);
    underline_action->setEnabled(true);
    strikethrough_action->setEnabled(true);
    subscript_action->setEnabled(true);
    superscript_action->setEnabled(true);
    text_color_action->setEnabled(true);
    bg_text_color_action->setEnabled(true);

    if (c.id.empty() || c.id.size() == 1)
        return;

    if (!window.is_string && !window.is_row)
    {
        bold_action->setEnabled(false);
        italic_action->setEnabled(false);
        underline_action->setEnabled(false);
        strikethrough_action->setEnabled(false);
        subscript_action->setEnabled(false);
        superscript_action->setEnabled(false);
        text_color_action->setEnabled(false);
        bg_text_color_action->setEnabled(false);
    }

    //update the interface elements
    block_format_slots = true;
    paragraph_format_combo->setCurrentText(paragraph_format.name.c_str());
    family_combo->setCurrentText(window.string_format.family.c_str());
    if (window.string_format.size == 0)
        size_combo->setCurrentText("");
    else
        size_combo->setCurrentText(std::to_string(window.string_format.size).c_str());
    
    auto document = GetCurrentDocument();
    if (!document)
    {
        block_format_slots = false;
        return;
    }
    
    bool code_block = document->GetParentId(c.id, ElementType::CODE_BLOCK) != ElementId{};
    align_left_action->setEnabled(!code_block);
    align_right_action->setEnabled(!code_block);
    align_center_action->setEnabled(!code_block);
    align_justify_action->setEnabled(!code_block);
    if (code_block)
    {
        align_left_action->setChecked(false);
        align_right_action->setChecked(false);
        align_center_action->setChecked(false);
        align_justify_action->setChecked(false);
    }
    else
    {
        align_left_action->setChecked(paragraph_format.alignment == ParagraphFormat::Alignment::Left);
        align_right_action->setChecked(paragraph_format.alignment == ParagraphFormat::Alignment::Right);
        align_center_action->setChecked(paragraph_format.alignment == ParagraphFormat::Alignment::Center);
        align_justify_action->setChecked(paragraph_format.alignment == ParagraphFormat::Alignment::Justify);
    }

    bold_action->setChecked(window.string_format.bold);
    italic_action->setChecked(window.string_format.italic);
    underline_action->setChecked(window.string_format.underline);
    strikethrough_action->setChecked(window.string_format.strikethrough);
    subscript_action->setChecked(window.string_format.subscript);
    superscript_action->setChecked(window.string_format.superscript);
    block_format_slots = false;

    undo_action->setEnabled(window.can_undo);
    redo_action->setEnabled(window.can_redo);

    UpdateCopyPaste();
}

void MainWindow::OnDocumentChanged(const bool changed)
{
    DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->currentWidget();
    if (!w)
        return;
    if (w->last_changed != changed)
    {
        UpdateCaption();
        w->last_changed = changed;
    }

    UpdateLocaleMessage();

    QtWindow& window = w->document_widget->window;
    undo_action->setEnabled(window.can_undo);
    redo_action->setEnabled(window.can_redo);
}

void MainWindow::OnSaveResult(const uint task_id, IOResult result)
{
    int index = -1;
    auto it = save_tasks.find(task_id);
    if (it != save_tasks.end())
    {
        index = it->second;
        save_tasks.erase(it);
    }

    if (result != IOResult::Success)
    {
        if (result == IOResult::PermissionDenied) //for read-only files call the SaveAs dialog
        {
            SaveFileAsName();
            return;
        }

        exit_after_save = false;
        close_tab_after_save = -1;
        recent_files.removeAll(dialog_file_name);
        UpdateRecentFiles();
        UpdateCaption();
        QMessageBox::critical(this, tr("Yutovo"), tr("Error saving document") + QString(": ") + dialog_file_name);
        return;
    }

    UpdateRecentFiles(dialog_file_name);

    DocumentWindow* w = (close_tab_after_save != -1 ? (DocumentWindow*)ui->editor_tabs->widget(close_tab_after_save) : 
        (DocumentWindow*)ui->editor_tabs->currentWidget());
    if (w)
        w->path = dialog_file_name;

    if (close_tab_after_save != -1)
    {
        OnCloseEditorTab(close_tab_after_save);
        close_tab_after_save = -1;
        if (exit_after_save)
        {
            if (!w->path.isEmpty() && last_documents.indexOf(w->path) == -1)
                last_documents.push_back(w->path);
            close();
        }
    }
    else if (exit_after_save)
    {
        if (!w->path.isEmpty() && last_documents.indexOf(w->path) == -1)
            last_documents.push_back(w->path);
        Close();
        close();
    }

    UpdateCaption(index);
}

void MainWindow::OnLoadResult(const uint task_id, IOResult result)
{
    auto it = loading_files.find(task_id);
    if (it == loading_files.end())
        return;
    int tab = it->second;

    DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->widget(tab);
    if (w)
    {
        if (!w->document_widget->formating && !w->document_widget->resizing)
            QApplication::restoreOverrideCursor();
        w->document_widget->loading = false;
    }

    loading_files.erase(it);
    
    if (result != IOResult::Success)
    {
        QString p = w->path;
        recent_files.removeAll(w->path);
        if (w)
            w->path = "";
        UpdateRecentFiles();
        UpdateCaption();
        QMessageBox::critical(this, tr("Yutovo"), tr("Error loading document") + QString(": ") + p);
        return;
    }
    UpdateRecentFiles(w->path);
    UpdateCaption(tab, ui->editor_tabs->currentIndex() == tab);
    FillScales();
}

void MainWindow::OnClipboardCopyResult(CopyResult result)
{
    if (result != CopyResult::Success)
        return;
    QClipboard* clipboard = QGuiApplication::clipboard();
    QMimeData* mime_data = new QMimeData;
    auto s = yutovo::ToBasicString(clipboard_json);

    QByteArray item_data(s.c_str(), s.length());
    //custom web clipboard type
    mime_data->setData("application/web;type=\"custom/format0\"", item_data);
    mime_data->setData("application/web;type=\"custom/formatmap\"", "{\"yutovo/elements\":\"application/web;type=\\\"custom/format0\\\"\"}");

    mime_data->setText(yutovo::ToBasicString(clipboard_text).c_str()); //text clipboard type

    clipboard->setMimeData(mime_data);
    clipboard_json = U"";
    clipboard_text = U"";
}

void MainWindow::OnClipboardPasteResult(PasteResult result)
{
}

void MainWindow::OnPdfExportResult(const std::vector<uint8_t>& pdf, const yutovo::PdfResult result)
{
    if (result != PdfResult::Success)
    {
        QMessageBox::critical(this, tr("Yutovo"), tr("Error exporting document") + QString(": ") + dialog_file_name);
        pdf_window.reset();
        return;
    }

    std::ofstream file(dialog_file_name.toUtf8().data(), std::ios::binary);
    file.write(reinterpret_cast<const char*>(pdf.data()), pdf.size());
    pdf_window.reset();
}

#ifdef REMOTE_SOLVER
void MainWindow::OnServiceStatus(IOResult result)
{
    if (result != IOResult::Success)
        RestartService();
}
#endif

void MainWindow::FillScales()
{
    auto document = GetCurrentDocument();
    if (!document)
        return;
    
    block_scale_slots = true;
    scale_combo->clear();
    scale_combo->addItem("50%");
    scale_combo->addItem("80%");
    scale_combo->addItem("90%");
    scale_combo->addItem("100%");
    scale_combo->addItem("110%");
    scale_combo->addItem("120%");
    scale_combo->addItem("150%");
    scale_combo->setCurrentText(QString::number(std::round(document->config.scale * 100)) + "%");
    block_scale_slots = false;
}

void MainWindow::FillParagraphFormats()
{
    auto document = GetCurrentDocument();
    if (!document)
        return;

    block_format_slots = true;
    paragraph_format_combo->clear();
    std::vector<ParagraphFormatPtr> formats;
    document->paragraph_formats->GetFormats(formats);
    for (auto& f : formats)
        paragraph_format_combo->addItem(f->name.c_str());
    block_format_slots = false;
}

void MainWindow::FillSizes(const QFont& font)
{
    QFontDatabase font_database;
    QString current_size = size_combo->currentText();

    {
        const QSignalBlocker blocker(size_combo);
        //signals are now blocked until end of scope
        size_combo->clear();

        if (font_database.isSmoothlyScalable(font.family(), font_database.styleString(font)))
        {
            const QList<int> sizes = QFontDatabase::standardSizes();
            for (const int size : sizes)
            {
                size_combo->addItem(QVariant(size).toString());
                size_combo->setEditable(true);
            }
        }
        else
        {
            const QList<int> sizes = font_database.smoothSizes(font.family(), font_database.styleString(font));
            for (const int size : sizes)
            {
                size_combo->addItem(QVariant(size).toString());
                size_combo->setEditable(false);
            }
        }
    }

    int i = size_combo->findText(current_size);
    if (i != -1)
        size_combo->setCurrentIndex(i);
}

void MainWindow::WriteSettings()
{
    if (!standard_toolbar_action)
        return;
    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/state", saveState());
    settings.setValue("MainWindow/version", APP_VERSION);
    settings.setValue("MainWindow/standard_toolbar", standard_toolbar_action->isChecked());
    settings.setValue("MainWindow/format_toolbar", format_toolbar_action->isChecked());
    settings.setValue("MainWindow/algebra_toolbar", algebra_toolbar_action->isChecked());
    settings.setValue("MainWindow/trigonometry_toolbar", trigonometry_toolbar_action->isChecked());
    settings.setValue("MainWindow/hyperbolic_toolbar", hyperbolic_toolbar_action->isChecked());
    settings.setValue("MainWindow/functions_toolbar", functions_toolbar_action->isChecked());
    settings.setValue("MainWindow/greek_toolbar", greek_toolbar_action->isChecked());
    settings.setValue("MainWindow/currency_toolbar", currency_toolbar_action->isChecked());
    settings.setValue("MainWindow/graphs_toolbar", graph_toolbar_action->isChecked());
    settings.setValue("MainWindow/status_bar", status_bar_action->isChecked());
    settings.setValue("MainWindow/language", (int)config.language);

    settings.setValue("Documents/last_documents", last_documents.isEmpty() ? "" : QVariant(last_documents));

    settings.beginGroup("RecentFiles");
    settings.setValue("max_count", recent_files_count);
    settings.setValue("files", QVariant(recent_files));
    settings.endGroup();

    settings.beginGroup("Document");
    settings.setValue("use_tabs", config.use_tabs);
    settings.setValue("tab_spaces", config.tab_spaces);
    settings.endGroup();

    settings.beginGroup("Service");
    settings.setValue("ip", config.service_ip.c_str());
    settings.setValue("port", config.service_port);
    settings.setValue("timeout", config.service_timeout);
    settings.endGroup();

    settings.beginGroup("Colors");
    settings.setValue("code_block_border_color", config.code_block_border_color.ToInt());
    settings.setValue("numbers_color", config.numbers_color.ToInt());
    settings.setValue("functions_color", config.functions_color.ToInt());
    settings.setValue("variables_color", config.variables_color.ToInt());
    settings.setValue("units_color", config.units_color.ToInt());
    settings.setValue("shapes_color", config.shapes_color.ToInt());
    settings.setValue("error_marks_color", config.error_marks_color.ToInt());
    settings.setValue("formula_bg_color", config.formula_bg_color.ToInt());
    settings.setValue("bg_selection_color", config.bg_selection_color.ToInt());
    settings.setValue("hilight_color", config.hilight_color.ToInt());
    settings.setValue("formula_frame_color", config.formula_frame_color.ToInt());
    settings.setValue("page_color", config.page_color.ToInt());
    settings.setValue("page_border_color", config.page_border_color.ToInt());
    settings.endGroup();

    settings.beginGroup("Fonts");
    settings.setValue("use_numbers_gaps", config.use_numbers_gaps);
    settings.setValue("binary_gap", config.binary_gap);
    settings.setValue("octal_gap", config.octal_gap);
    settings.setValue("decimal_gap", config.decimal_gap);
    settings.setValue("hexadecimal_gap", config.hexadecimal_gap);
    settings.endGroup();

    settings.beginGroup("Log");
    settings.setValue("level", (int)config.log_level);
    settings.setValue("path", config.logs_path.c_str());
    settings.endGroup();

    settings.beginGroup("Calculator");
    settings.setValue("solve_delay", config.solve_delay);
    settings.setValue("result_auto_advance", config.auto_result.result_auto_advance);
    QList<QVariant> v;
    for (auto r : config.auto_result.results_order)
        v.push_back((int)r);
    settings.setValue("results_order", v);

    settings.setValue("real_precision", config.real_result.precision);
    settings.setValue("real_exp", config.real_result.exp);
    settings.setValue("real_default_angle_measure", (int)config.real_result.default_angle_measure);
    settings.setValue("real_result_angle_measure", (int)config.real_result.result_angle_measure);
    settings.setValue("real_show_angle_measure", config.real_result.show_angle_measure);

    settings.setValue("integer_default_notation", (int)config.integer_result.default_notation);
    settings.setValue("integer_result_notation", (int)config.integer_result.result_notation);
    settings.setValue("integer_show_notation", config.integer_result.show_notation);

    settings.setValue("rational_fraction_form", (int)config.rational_result.fraction_form);

    settings.setValue("complex_precision", config.complex_result.precision);
    settings.setValue("complex_exp", config.complex_result.exp);
    settings.setValue("complex_default_angle_measure", (int)config.complex_result.default_angle_measure);
    settings.setValue("complex_result_angle_measure", (int)config.complex_result.result_angle_measure);
    settings.setValue("complex_show_angle_measure", config.complex_result.show_angle_measure);
    settings.setValue("complex_form", (int)config.complex_result.form);
    settings.setValue("complex_max_count", config.complex_result.max_count);
    settings.endGroup();
}

void MainWindow::ReadSettings()
{
    const auto geometry = settings.value("MainWindow/geometry", QByteArray()).toByteArray();
    if (!geometry.isEmpty())
        restoreGeometry(geometry);

    std::string lang = QLocale::system().name().toUtf8().data(); //get current system language, it will be default one
    lang = lang.substr(0, lang.find('_'));
    if (lang != "en" && lang != "ru" && lang != "es" && lang != "pt")
        lang = "en";

    if (lang == "ru")
        config.language = (yutovo_calculator::Language)settings.value("MainWindow/language", (int)yutovo_calculator::Language::Russian).toInt();
    else if (lang == "es")
        config.language = (yutovo_calculator::Language)settings.value("MainWindow/language", (int)yutovo_calculator::Language::Spanish).toInt();
    else if (lang == "pt")
        config.language = (yutovo_calculator::Language)settings.value("MainWindow/language", (int)yutovo_calculator::Language::BrazilianPortuguese).toInt();
    else
        config.language = (yutovo_calculator::Language)settings.value("MainWindow/language", (int)yutovo_calculator::Language::English).toInt();

    settings.beginGroup("RecentFiles");
    recent_files_count = settings.value("max_count", 10).toInt();
    if (recent_files_count > 50)
        recent_files_count = 10;
    auto list = settings.value("files").toStringList();
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        if (*it != "")
            recent_files.push_back(*it);
    }
    settings.endGroup();

    settings.beginGroup("Document");
    config.use_tabs = settings.value("use_tabs", true).toBool();
    config.tab_spaces = settings.value("tab_spaces", 4).toInt();
    settings.endGroup();

    settings.beginGroup("Documents");
    config.auto_prompt = settings.value("auto_prompt", true).toBool();
    config.undo_size = settings.value("undo_size", 100).toInt();
    settings.endGroup();

    settings.beginGroup("Service");
    config.service_ip = settings.value("ip", "localhost").toString().toUtf8().data();
    config.service_port = settings.value("port", 8010).toInt();
    config.service_timeout = settings.value("timeout", 10000).toInt();
    settings.endGroup();

    settings.beginGroup("Fonts");
    config.use_numbers_gaps = settings.value("use_numbers_gaps", true).toBool();
    config.binary_gap = settings.value("binary_gap", 4).toInt();
    config.octal_gap = settings.value("octal_gap", 3).toInt();
    config.decimal_gap = settings.value("decimal_gap", 3).toInt();
    config.hexadecimal_gap = settings.value("hexadecimal_gap", 4).toInt();
    settings.endGroup();

    settings.beginGroup("Log");
    config.log_level = (LogLevel)settings.value("level", (int)LogLevel::LEVEL_INFO).toInt();
    config.log_console = settings.value("level", false).toBool();
    config.log_file = settings.value("level", true).toBool();
#ifdef _WIN32
    std::string p;
    char szPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
        p = std::string(szPath) + "/Yutovo/";
    p += "log";
    config.logs_path = settings.value("path", p.c_str()).toString().toUtf8().data();
#else
    std::string p;
    QStringList args = qApp->arguments();
    for (int i = 0; i < args.length(); i++)
    {
        QString s = args.at(i);
        if (s.startsWith("--logs-path"))
        {
            QStringList pair = s.split("=");
            if (pair.length() == 2)
            {
                p = pair[1].toUtf8().data();
                break;
            }
        }
    }
    if (!p.empty())
        config.logs_path = p;
    else
        config.logs_path = settings.value("path", "./log").toString().toUtf8().data();
#endif
    settings.endGroup();

    settings.beginGroup("Colors");
    config.code_block_border_color = Color::FromInt(settings.value("code_block_border_color", Color::Blue().ToInt()).toInt());
    config.numbers_color = Color::FromInt(settings.value("numbers_color", Color::Blue().ToInt()).toInt());
    config.functions_color = Color::FromInt(settings.value("functions_color", Color::FromHex("#ff5500").ToInt()).toInt());
    config.variables_color = Color::FromInt(settings.value("variables_color", Color::FromHex("#00193e").ToInt()).toInt());
    config.units_color = Color::FromInt(settings.value("units_color", Color::FromHex("#005500").ToInt()).toInt());
    config.shapes_color = Color::FromInt(settings.value("shapes_color", Color::Black().ToInt()).toInt());
    config.error_marks_color = Color::FromInt(settings.value("error_marks_color", Color::Red().ToInt()).toInt());
    config.formula_bg_color = Color::FromInt(settings.value("formula_bg_color", Color::White().ToInt()).toInt());
    config.bg_selection_color = Color::FromInt(settings.value("bg_selection_color", Color::Blue().ToInt()).toInt());
    config.hilight_color = Color::FromInt(settings.value("hilight_color", Color::FromHex("#97deff").ToInt()).toInt());
    config.formula_frame_color = Color::FromInt(settings.value("formula_frame_color", Color::FromHex("#b8d3ff").ToInt()).toInt());
    config.page_color = Color::FromInt(settings.value("page_color", Color::White().ToInt()).toInt());
    config.page_border_color = Color::FromInt(settings.value("page_border_color", Color::Blue().ToInt()).toInt());
    settings.endGroup();

    settings.beginGroup("Calculator");
    config.solve_delay = settings.value("solve_delay", config.solve_delay).toInt();
    config.auto_result.result_auto_advance = settings.value("result_auto_advance", true).toBool();
    QList<QVariant> v = settings.value("results_order").toList();
    size_t i = 0;
    for (auto r : v)
    {
        if (i < sizeof(Config::AutoResultConfig::results_order) / sizeof(Config::AutoResultConfig::results_order[0]))
            config.auto_result.results_order[i++] = (ResultType)r.toInt();
    }
    auto& results_order = config.auto_result.results_order;
    for (; i < sizeof(Config::AutoResultConfig::results_order) / sizeof(Config::AutoResultConfig::results_order[0]); ++i)
    {
        if (std::find(std::begin(results_order), std::end(results_order), ResultType::REAL) == std::end(results_order))
            results_order[i++] = ResultType::REAL;
        if (std::find(std::begin(results_order), std::end(results_order), ResultType::INTEGER) == std::end(results_order))
            results_order[i++] = ResultType::INTEGER;
        if (std::find(std::begin(results_order), std::end(results_order), ResultType::RATIONAL) == std::end(results_order))
            results_order[i++] = ResultType::RATIONAL;
        if (std::find(std::begin(results_order), std::end(results_order), ResultType::COMPLEX) == std::end(results_order))
            results_order[i++] = ResultType::COMPLEX;
        if (std::find(std::begin(results_order), std::end(results_order), ResultType::ARRAY_REAL) == std::end(results_order))
            results_order[i++] = ResultType::ARRAY_REAL;
        if (std::find(std::begin(results_order), std::end(results_order), ResultType::SYMBOLIC_REAL) == std::end(results_order))
            results_order[i++] = ResultType::SYMBOLIC_REAL;
        if (std::find(std::begin(results_order), std::end(results_order), ResultType::SYMBOLIC_RATIONAL) == std::end(results_order))
            results_order[i++] = ResultType::SYMBOLIC_RATIONAL;
        if (std::find(std::begin(results_order), std::end(results_order), ResultType::SYMBOLIC_COMPLEX) == std::end(results_order))
            results_order[i++] = ResultType::SYMBOLIC_COMPLEX;
    }

    config.real_result.precision = settings.value("real_precision", 3).toInt();
    if (config.real_result.precision < 1)
        config.real_result.precision = 3;
    config.real_result.exp = settings.value("real_exp", 10).toInt();
    if (config.real_result.exp < 1)
        config.real_result.exp = 10;
    config.real_result.default_angle_measure = (AngleMeasure)settings.value("real_default_angle_measure", 0).toInt();
    if ((int)config.real_result.default_angle_measure < 0 || (int)config.real_result.default_angle_measure > 2)
        config.real_result.default_angle_measure = AngleMeasure::Radian;
    config.real_result.result_angle_measure = (AngleMeasure)settings.value("real_result_angle_measure", 0).toInt();
    if ((int)config.real_result.result_angle_measure < 0 || (int)config.real_result.result_angle_measure > 2)
        config.real_result.result_angle_measure = AngleMeasure::Radian;
    config.real_result.show_angle_measure = settings.value("real_show_angle_measure", false).toBool();

    config.integer_result.default_notation = (Notation)settings.value("integer_default_notation", 2).toInt();
    config.integer_result.result_notation = (Notation)settings.value("integer_result_notation", 2).toInt();
    config.integer_result.show_notation = settings.value("integer_show_notation", true).toBool();

    config.rational_result.fraction_form = (FractionForm)settings.value("rational_fraction_form", 0).toInt();

    config.complex_result.precision = settings.value("complex_precision", 3).toInt();
    if (config.complex_result.precision < 1)
        config.complex_result.precision = 3;
    config.complex_result.exp = settings.value("complex_exp", 10).toInt();
    if (config.complex_result.exp < 1)
        config.complex_result.exp = 10;
    config.complex_result.default_angle_measure = (AngleMeasure)settings.value("complex_default_angle_measure", 0).toInt();
    if ((int)config.complex_result.default_angle_measure < 0 || (int)config.complex_result.default_angle_measure > 2)
        config.complex_result.default_angle_measure = AngleMeasure::Radian;
    config.complex_result.result_angle_measure = (AngleMeasure)settings.value("complex_result_angle_measure", 0).toInt();
    if ((int)config.complex_result.result_angle_measure < 0 || (int)config.complex_result.result_angle_measure > 2)
        config.complex_result.result_angle_measure = AngleMeasure::Radian;
    config.complex_result.show_angle_measure = settings.value("complex_show_angle_measure", false).toBool();
    config.complex_result.form = (ComplexForm)settings.value("complex_form", 0).toInt();
    if ((int)config.complex_result.form < 0 || (int)config.complex_result.form > 2)
        config.complex_result.form = ComplexForm::Arithmetic;
    config.complex_result.max_count = settings.value("complex_max_count", 10).toInt();
    if (config.complex_result.max_count < 0 || config.complex_result.max_count > 20)
        config.complex_result.max_count = 10;

    config.auto_result.real_result = config.real_result;
    config.auto_result.integer_result = config.integer_result;
    config.auto_result.rational_result = config.rational_result;
    config.auto_result.complex_result = config.complex_result;
    settings.endGroup();
}

void MainWindow::UpdateFontSize()
{
    int s;
    try
    {
        s = std::stoi(size_combo->currentText().toUtf8().data());
    }
    catch (...)
    {
        return;
    }

    if (s != last_font_size)
    {
        auto document = GetCurrentDocument();
        if (document)
            document->SetFontSize(s);
        last_font_size = s;
    }
    SetFocus();
}

void MainWindow::UpdateCopyPaste()
{
    DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->currentWidget();
    if (!w)
        return;
    
    QtWindow& window = w->document_widget->window;
    if (window.editor_state.caret_state.IsEmpty() || window.editor_state.caret_state.id.size() == 1)
        return;
    bool editable = window.parent_editable;
    copy_action->setEnabled(!window.editor_state.selection_state.IsEmpty());
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* mime_data = clipboard->mimeData();
    paste_action->setEnabled(editable && (mime_data->hasText() || mime_data->hasImage()));
    cut_action->setEnabled(editable && !window.editor_state.selection_state.IsEmpty());
}

void MainWindow::UpdateRecentFiles(const QString add_file_name)
{
    if (add_file_name != "")
    {
        if (recent_files.size() > recent_files_count)
            recent_files.erase(recent_files.end() - 1);
        recent_files.removeAll(add_file_name);
        recent_files.push_front(add_file_name);
    }

    recent_files_menu->clear();
    for (auto it = recent_files.begin(); it != recent_files.end(); ++it)
    {
        QAction* action = new QAction(*it, this);
        action->setData(*it);
        connect(action, &QAction::triggered, this, &MainWindow::OpenRecentFile);
        recent_files_menu->addAction(action);
    }
}

void MainWindow::UpdateLibraryMenu(QMenu* library_menu, const QString start_topic, const QString except_topic)
{
    std::function<void(const std::filesystem::path& path, const std::wstring& name, QMenu* menu)> get_files =
        [&](const std::filesystem::path& path, const std::wstring& name, QMenu* menu)
        {
            std::vector<std::string> order;
            if (std::filesystem::exists(path / ".order"))
            {
                try
                {
#ifdef _WIN32
                    std::wstring order_file((path / ".order").wstring());
                    std::wifstream file(order_file);
                    if (file.is_open())
                    {
                        file.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
                        std::wstringstream ws;
                        ws << file.rdbuf();
                        std::wstring line;
                        while (std::getline(ws, line))
                            order.push_back(ToBasicString(line));
                    }
#else
                    std::string order_file((path / ".order").string());
                    std::ifstream file(order_file);
                    if (file.is_open())
                    {
                        std::string line;
                        while (std::getline(file, line))
                            order.push_back(line);
                    }
#endif
                }
                catch (const std::ios_base::failure& ex)
                {
                }
            }

            std::vector<std::filesystem::path> sorted_dirs, others_dirs;
            std::vector<std::filesystem::path> sorted_files, others_files;
            try
            {
                for (const auto& entry : std::filesystem::directory_iterator(path))
                {
                    if (entry.is_directory())
                    {
                        int pos = -1;
                        if (!order.empty())
                        {
#ifdef _WIN32
                            std::string s = ToBasicString(entry.path().filename().wstring());
#else
                            std::string s = entry.path().filename().string();
#endif
                            if (s == except_topic.toUtf8().data())
                                continue;
                            
                            auto it = std::find(order.begin(), order.end(), s);
                            if (it != order.end())
                                pos = std::distance(order.begin(), it);
                            if (pos == -1)
                                others_dirs.push_back(entry.path());
                            else
                            {
                                if (pos < sorted_dirs.size())
                                {
                                    sorted_dirs[pos] = entry.path();
                                }
                                else
                                {
                                    for (int i = sorted_dirs.size(); i < pos; ++i)
                                        sorted_dirs.push_back(std::filesystem::path());
                                    sorted_dirs.push_back(entry.path());
                                }
                            }
                        }
                        else
                            sorted_dirs.push_back(entry.path());
                    }
                    else if (entry.is_regular_file())
                    {
                        if (entry.path().stem() != ".order")
                        {
                            int pos = -1;
                            if (!order.empty())
                            {
#ifdef _WIN32
                                auto it = std::find(order.begin(), order.end(), ToBasicString(entry.path().filename().wstring()));
#else
                                auto it = std::find(order.begin(), order.end(), entry.path().filename().string());
#endif
                                if (it != order.end())
                                    pos = std::distance(order.begin(), it);
                                if (pos == -1)
                                    others_files.push_back(entry.path());
                                else
                                {
                                    if (pos < sorted_files.size())
                                    {
                                        sorted_files[pos] = entry.path();
                                    }
                                    else
                                    {
                                        for (int i = sorted_files.size(); i < pos; ++i)
                                            sorted_files.push_back(std::filesystem::path());
                                        sorted_files.push_back(entry.path());
                                    }
                                }
                            }
                            else
                                sorted_files.push_back(entry.path());
                        }
                    }
                }
            }
            catch (const std::filesystem::filesystem_error&)
            {
            }

            sorted_dirs.insert(sorted_dirs.end(), others_dirs.begin(), others_dirs.end());
            sorted_files.insert(sorted_files.end(), others_files.begin(), others_files.end());

            if (order.empty())
            {
                std::sort(sorted_dirs.begin(), sorted_dirs.end());
                std::sort(sorted_files.begin(), sorted_files.end());
            }

            for (const auto& entry : sorted_dirs)
            {
                if (entry.empty())
                    continue;
                QMenu* dir = menu->addMenu(QString::fromWCharArray(entry.filename().wstring().c_str()));
                get_files(entry, entry.stem().wstring(), dir);
            }

            for (const auto& entry : sorted_files)
            {
                if (entry.empty())
                    continue;
                QAction* action = new QAction(QString::fromWCharArray(entry.stem().wstring().c_str()), this);
                action->setData(QString(QString::fromWCharArray(std::filesystem::absolute(entry).wstring().c_str())));
                connect(action, &QAction::triggered, this, &MainWindow::OpenLibraryFile);
                menu->addAction(action);
            }
        };
    
    library_menu->clear();

    std::string dir = "en";
    switch (config.language)
    {
    case yutovo_calculator::Language::Russian:
        dir = "ru";
        break;
    case yutovo_calculator::Language::Spanish:
        dir = "es";
        break;
    case yutovo_calculator::Language::BrazilianPortuguese:
        dir = "pt_BR";
        break;
    }

    auto p = std::string(GetLibraryDir().toUtf8().data()) + dir;
    if (!std::filesystem::exists(p))
        return;
    
    auto s = std::filesystem::u8path(start_topic.toUtf8().constData());
    get_files(std::filesystem::path(p) / s, L"library", library_menu);
}

void MainWindow::InstallTranslation(const yutovo_calculator::Language language)
{
    qApp->removeTranslator(&desktop_translator);
    qApp->removeTranslator(&editor_translator);
    
    if (language == yutovo_calculator::Language::Russian)
    {
        if (!desktop_translator.load("yutovo-desktop_ru", GetTranslationDir("yutovo-desktop_ru.qm")))
            logger->Error("Error loading translation: yutovo-desktop_ru");
        if (!editor_translator.load("yutovo-editor_ru", GetTranslationDir("yutovo-editor_ru.qm")))
            logger->Error("Error loading translation: yutovo-editor_ru");
    }
    else if (language == yutovo_calculator::Language::Spanish)
    {
        if (!desktop_translator.load("yutovo-desktop_es", GetTranslationDir("yutovo-desktop_es.qm")))
            logger->Error("Error loading translation: yutovo-desktop_es");
        if (!editor_translator.load("yutovo-editor_es", GetTranslationDir("yutovo-editor_es.qm")))
            logger->Error("Error loading translation: yutovo-editor_es");
    }
    else if (language == yutovo_calculator::Language::BrazilianPortuguese)
    {
        if (!desktop_translator.load("yutovo-desktop_pt_BR", GetTranslationDir("yutovo-desktop_pt_BR.qm")))
            logger->Error("Error loading translation: yutovo-desktop_pt_BR");
        if (!editor_translator.load("yutovo-editor_pt_BR", GetTranslationDir("yutovo-editor_pt_BR.qm")))
            logger->Error("Error loading translation: yutovo-editor_pt_BR");
    }
    else if (language == yutovo_calculator::Language::English)
    {
        if (!desktop_translator.load("yutovo-desktop_en", GetTranslationDir("yutovo-desktop_en.qm")))
            logger->Error("Error loading translation: yutovo-desktop_en");
        if (!editor_translator.load("yutovo-editor_en", GetTranslationDir("yutovo-editor_en.qm")))
            logger->Error("Error loading translation: yutovo-editor_en");
    }

    if (!qApp->installTranslator(&desktop_translator))
        logger->Error("Error installing translation");
    if (!qApp->installTranslator(&editor_translator))
        logger->Error("Error installing translation");
}

void MainWindow::UpdateCaption(int tab, bool update_title)
{
    DocumentWindow* w = (tab == -1 ? (DocumentWindow*)ui->editor_tabs->currentWidget() : (DocumentWindow*)ui->editor_tabs->widget(tab));
    if (!w)
    {
        setWindowTitle(tr("Yutovo"));
        EnableButtons(false);
        return;
    }
    
    QString file_name;
    QFileInfo file_info(w->path);
    if (file_info.fileName().isEmpty())
        file_name = tr("(No name)");
    else
        file_name = file_info.fileName();
    if (w->document->IsChanged())
        file_name += " *";

    ui->editor_tabs->setTabText(tab == -1 ? ui->editor_tabs->currentIndex() : tab, file_name);
    if (!w->path.isEmpty())
        ui->editor_tabs->setTabToolTip(tab == -1 ? ui->editor_tabs->currentIndex() : tab, file_info.canonicalFilePath());
    if (update_title)
        setWindowTitle(file_name + " - " + tr("Yutovo"));
}

void MainWindow::UpdateLocaleMessage()
{
    auto document = GetCurrentDocument();
    if (!document)
    {
        UpdateDocumentOnlineLink();
        return;
    }
    Config c;
    document->GetConfig(c);
    locale_status->setText(tr("Locale: ") + (tr(yutovo_calculator::LanguageToString(c.language).c_str())));
    UpdateDocumentOnlineLink();
}

void MainWindow::UpdateDocumentOnlineLink()
{
    if (!document_online_label)
        return;
    DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->currentWidget();
    if (!w || w->path.isEmpty())
    {
        if (status_separator)
            status_separator->hide();
        document_online_label->clear();
        document_online_label->hide();
        return;
    }
    QString libDir = GetLibraryDir();
    QFileInfo libFi(libDir);
    QString canonicalLibDir = libFi.canonicalFilePath();
    if (canonicalLibDir.isEmpty())
        canonicalLibDir = libDir;
    if (!canonicalLibDir.endsWith('/'))
        canonicalLibDir += '/';
    if (!w->path.startsWith(canonicalLibDir))
    {
        if (status_separator)
            status_separator->hide();
        document_online_label->clear();
        document_online_label->hide();
        return;
    }
    QString relPath = w->path.mid(canonicalLibDir.length());
    QString domain = relPath.startsWith("ru/") ? "https://yutovo.ru" : "https://yutovo.com";
    QUrl url(domain + "/library/" + relPath);
    document_online_label->setText(QString("<a href=\"%1\">%2</a>").arg(url.toString(), tr("This document online")));
    if (status_separator)
        status_separator->show();
    document_online_label->show();
}

void MainWindow::EnableButtons(bool enable)
{
    save_action->setEnabled(enable);
    save_all_action->setEnabled(enable);
    save_as_action->setEnabled(enable);
    close_action->setEnabled(enable);
    close_all_action->setEnabled(enable);
    close_others_action->setEnabled(enable);
    export_html_action->setEnabled(enable);
    export_pdf_action->setEnabled(enable);

    undo_action->setEnabled(enable);
    redo_action->setEnabled(enable);
    copy_action->setEnabled(enable);
    paste_action->setEnabled(enable);
    cut_action->setEnabled(enable);
    properties_action->setEnabled(enable);
    recalculate_action->setEnabled(enable);
    calculator_action->setEnabled(enable);
    align_left_action->setEnabled(enable);
    align_right_action->setEnabled(enable);
    align_center_action->setEnabled(enable);
    align_justify_action->setEnabled(enable);
    bold_action->setEnabled(enable);
    italic_action->setEnabled(enable);
    underline_action->setEnabled(enable);
    strikethrough_action->setEnabled(enable);
    subscript_action->setEnabled(enable);
    superscript_action->setEnabled(enable);
    text_color_action->setEnabled(enable);
    bg_text_color_action->setEnabled(enable);
    link_action->setEnabled(enable);
    scale_combo->setEnabled(enable);
    paragraph_format_combo->setEnabled(enable);
    family_combo->setEnabled(enable);
    size_combo->setEnabled(enable);
    algebra_toolbar->setEnabled(enable);
    trigonometry_toolbar->setEnabled(enable);
    hyperbolic_toolbar->setEnabled(enable);
    functions_toolbar->setEnabled(enable);
    greek_toolbar->setEnabled(enable);
    currency_toolbar->setEnabled(enable);
    graph_toolbar->setEnabled(enable);
    logical_toolbar->setEnabled(enable);
}

QString MainWindow::LanguageToString(yutovo_calculator::Language lang)
{
    switch (lang)
    {
    case yutovo_calculator::Language::Russian:
        return "ru";
    case yutovo_calculator::Language::Spanish:
        return "es";
    case yutovo_calculator::Language::BrazilianPortuguese:
        return "pt_BR";
    default:
        return "en";
    }
}

#ifdef REMOTE_SOLVER
void MainWindow::RestartService()
{
    logger->Info("Restarting service");
    service.reset(new QProcess(this));
    service->setWorkingDirectory(".");
    service->start("./yutovo-serviced");
}
#endif

QString MainWindow::GetLibraryDir()
{
    if (std::filesystem::exists("./library/"))
        return "./library/";
    QString p = QCoreApplication::applicationDirPath();
    if (std::filesystem::exists((p + "/library/").toUtf8().data()))
        return p + "/library/";
#ifndef _WIN32
    if (std::filesystem::exists("/usr/share/yutovo/library/"))
        return "/usr/share/yutovo/library/";
#endif
    return p;
}

QString MainWindow::GetTranslationDir(QString filename)
{
    if (std::filesystem::exists(filename.toUtf8().data()))
        return "./";
#ifndef _WIN32
    QString p = "/usr/share/yutovo/translations/";
    if (std::filesystem::exists((p + filename).toUtf8().data()))
        return p;
#endif
    return QCoreApplication::applicationDirPath();
}
