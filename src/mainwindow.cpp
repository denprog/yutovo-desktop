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
#include <filesystem>
#ifdef _WIN32
#include <shlobj_core.h>
#include <codecvt>
#endif
#include <yutovo_editor/editor_utils.h>
#include <yutovo_calculator/math_helper.h>
#include "document_window.h"
#include "about_dialog.h"
#include "settings_dialog.h"
#include "properties_dialog.h"
#include "link_dialog.h"
#include "terms_of_use_dialog.h"
#include "privacy_policy_dialog.h"

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

    logger = Logger::GetInstance(config.logs_path + "/yutovo_desktop", "yutovo_desktop", config.log_console, config.log_file);
    logger->SetLevel((int)config.log_level);

    InstallTranslation(config.language);

#ifdef REMOTE_SOLVER
    RestartService();
#endif

    ui->editor_tabs->clear();
    SetupGui();

    AddEditorTab("(No name)");

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
        else
            OpenFile(GetLibraryDir() + "en/Others/First page.yut");
    }

    logger->Info("Desktop start");
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event)
{
    DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->currentWidget();
    if (w)
        w->MakeContextMenu(event);
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
        SetupGui();
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
                file_name = "(No name)";
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

void MainWindow::SetupGui()
{
    ui->editor_tabs->setTabsClosable(true);
    ui->editor_tabs->disconnect();
    connect(ui->editor_tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(OnCloseEditorTab(int)));
    connect(ui->editor_tabs, SIGNAL(currentChanged(int)), this, SLOT(OnEditorChanged(int)));

    menuBar()->clear();

    QList<QToolBar*> toolbars = findChildren<QToolBar*>();
    for (auto& t : toolbars)
        removeToolBar(t);

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

    CreateStatusBar();

    bool b = settings.value("MainWindow/standard_toolbar", true).toBool();
    standard_toolbar_action->setChecked(b);
    b = settings.value("MainWindow/format_toolbar", true).toBool();
    format_toolbar_action->setChecked(b);
    b = settings.value("MainWindow/algebra_toolbar", true).toBool();
    algebra_toolbar_action->setChecked(b);
    b = settings.value("MainWindow/trigonometry_toolbar", false).toBool();
    trigonometry_toolbar_action->setChecked(b);
    b = settings.value("MainWindow/hyperbolic_toolbar", false).toBool();
    hyperbolic_toolbar_action->setChecked(b);
    b = settings.value("MainWindow/functions_toolbar", false).toBool();
    functions_toolbar_action->setChecked(b);
    b = settings.value("MainWindow/greek_toolbar", false).toBool();
    greek_toolbar_action->setChecked(b);
    b = settings.value("MainWindow/currency_toolbar", false).toBool();
    currency_toolbar_action->setChecked(b);
    b = settings.value("MainWindow/status_bar", true).toBool();
    status_bar_action->setChecked(b);

    int i = ui->editor_tabs->currentIndex();
    if (i >= 0)
        OnEditorChanged(i);
}

void MainWindow::AddEditorTab(const QString name)
{
    DocumentWindow* wnd = new DocumentWindow(config, this);

    connect(wnd, &DocumentWindow::CaretMoved, this, &MainWindow::OnCaretMoved);
    connect(wnd, &DocumentWindow::DocumentChanged, this, &MainWindow::OnDocumentChanged);
    connect(wnd, &DocumentWindow::SaveResult, this, &MainWindow::OnSaveResult);
    connect(wnd, &DocumentWindow::LoadResult, this, &MainWindow::OnLoadResult);
    connect(wnd, &DocumentWindow::ClipboardCopyResult, this, &MainWindow::OnClipboardCopyResult);
    connect(wnd, &DocumentWindow::ClipboardPasteResult, this, &MainWindow::OnClipboardPasteResult);
    connect(wnd, &DocumentWindow::LinkClicked, this, &MainWindow::OnLinkClicked);

    connect(wnd->document_widget, &DocumentWidget::NextEditorTab, this, &MainWindow::OnNextEditorTab);
    connect(wnd->document_widget, &DocumentWidget::PrevEditorTab, this, &MainWindow::OnPrevEditorTab);

#ifdef REMOTE_SOLVER
    connect(wnd->document_widget, &DocumentWidget::ServiceStatus, this, &MainWindow::OnServiceStatus);
#endif

    ui->editor_tabs->addTab(wnd, name);
    ui->editor_tabs->setCurrentIndex(ui->editor_tabs->count() - 1);
    wnd->SetFocus();

    wnd->CreateDocument();

    auto document = GetCurrentDocument();
    auto s = wnd->document_widget->size();
    document->Resize(s.width(), s.height());
    OnCaretMoved(document->GetEditorState());
    FillParagraphFormats();

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

    QAction* action = new QAction(QIcon(":/icons/images/standard/new.png"), tr("&New"), this);
    action->setShortcuts(QKeySequence::New);
    action->setStatusTip(tr("Create a new document"));
    connect(action, &QAction::triggered, this, &MainWindow::New);
    file_menu->addAction(action);
    standard_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/standard/open.png"), tr("&Open..."), this);
    action->setShortcuts(QKeySequence::Open);
    action->setStatusTip(tr("Open an existing file"));
    connect(action, &QAction::triggered, this, &MainWindow::Open);
    file_menu->addAction(action);
    standard_toolbar->addAction(action);

    recent_files_menu = file_menu->addMenu(tr("&Open recent file"));

    save_action = new QAction(QIcon(":/icons/images/standard/save.png"), tr("&Save"), this);
    save_action->setShortcuts(QKeySequence::Save);
    save_action->setStatusTip(tr("Save the document to disk"));
    connect(save_action, &QAction::triggered, this, &MainWindow::Save);
    file_menu->addAction(save_action);
    standard_toolbar->addAction(save_action);

    action = file_menu->addAction(QIcon(":/images/standard/new.png"), tr("Save &As..."), this, &MainWindow::SaveFileAsName);
    action->setStatusTip(tr("Save the document under a new name"));

    action = new QAction(tr("&Close"), this);
    action->setStatusTip(tr("Close the document"));
    connect(action, &QAction::triggered, this, &MainWindow::Close);
    file_menu->addAction(action);

    action = new QAction(tr("Close all"), this);
    action->setStatusTip(tr("Close all documents"));
    connect(action, &QAction::triggered, this, &MainWindow::CloseAll);
    file_menu->addAction(action);

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

    undo_action = new QAction(QIcon(":/icons/images/standard/undo.png"), tr("U&ndo"), this);
    undo_action->setShortcuts(QKeySequence::Undo);
    undo_action->setStatusTip(tr("Undo the last operation"));
    connect(undo_action, &QAction::triggered, this, &MainWindow::Undo);
    edit_menu->addAction(undo_action);
    standard_toolbar->addAction(undo_action);

    redo_action = new QAction(QIcon(":/icons/images/standard/redo.png"), tr("&Redo"), this);
    redo_action->setShortcuts(QKeySequence::Redo);
    redo_action->setStatusTip(tr("Redo the last operation"));
    connect(redo_action, &QAction::triggered, this, &MainWindow::Redo);
    edit_menu->addAction(redo_action);
    standard_toolbar->addAction(redo_action);

    edit_menu->addSeparator();
    standard_toolbar->addSeparator();

    cut_action = new QAction(QIcon(":/icons/images/standard/cut.png"), tr("Cu&t"), this);
    cut_action->setShortcuts(QKeySequence::Cut);
    cut_action->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    connect(cut_action, &QAction::triggered, this, &MainWindow::Cut);
    edit_menu->addAction(cut_action);
    standard_toolbar->addAction(cut_action);

    copy_action = new QAction(QIcon(":/icons/images/standard/copy.png"), tr("&Copy"), this);
    copy_action->setShortcuts(QKeySequence::Copy);
    copy_action->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
    connect(copy_action, &QAction::triggered, this, &MainWindow::Copy);
    edit_menu->addAction(copy_action);
    standard_toolbar->addAction(copy_action);

    paste_action = new QAction(QIcon(":/icons/images/standard/paste.png"), tr("&Paste"), this);
    paste_action->setShortcuts(QKeySequence::Paste);
    paste_action->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
    connect(paste_action, &QAction::triggered, this, &MainWindow::Paste);
    edit_menu->addAction(paste_action);
    standard_toolbar->addAction(paste_action);

    standard_toolbar->addSeparator();

    properties_action = new QAction(QIcon(":/icons/images/standard/properties.png"), tr("&Properties"), this);
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
    format_toolbar->setStyleSheet("QToolBar{spacing:4px;}");

    calculator_action = new QAction(QIcon(":/icons/images/format/code.png"), tr("Insert calculator"), this);
    calculator_action->setStatusTip(tr("Insert calculator"));
    connect(calculator_action, &QAction::triggered, this, &MainWindow::OnInsertCode);
    format_toolbar->addAction(calculator_action);

    format_toolbar->addSeparator();

    paragraph_format_combo = new QComboBox;
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

    status_bar_action = new QAction(tr("&Status bar"), this);
    status_bar_action->setCheckable(true);
    status_bar_action->setChecked(true);
    connect(status_bar_action, &QAction::toggled, this, &MainWindow::StatusBar);
    view_menu->addAction(status_bar_action);

    //library menu
    QMenu* library_menu = menuBar()->addMenu(tr("&Library"));
    UpdateLibraryMenu(library_menu);

    //help menu
    QMenu* help_menu = menuBar()->addMenu(tr("&Help"));

    action = help_menu->addAction(tr("Help online"), this, &MainWindow::HelpOnline);
    help_menu->setStatusTip(tr("Help system online"));

    help_menu->addSeparator();

    action = help_menu->addAction(tr("&Terms of use"), this, &MainWindow::TermsOfUse);
    help_menu->setStatusTip(tr("Show the application's Terms of use box"));

    action = help_menu->addAction(tr("&Privacy policy"), this, &MainWindow::PrivacyPolicy);
    help_menu->setStatusTip(tr("Show the application's Privacy policy box"));

    help_menu->addSeparator();

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
    algebra_toolbar->setStyleSheet("QToolBar{spacing:4px;}");

    QAction* action = new QAction(QIcon(":/icons/images/algebra/plus.png"), tr("Plus"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnPlus);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/minus.png"), tr("Minus"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnMinus);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/multiply.png"), tr("Multiply"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnMultiply);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/division.png"), tr("Division"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnDivision);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/power.png"), tr("Power"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnPower);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/sqrt.png"), tr("Square root"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnSquareRoot);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/nth_root.png"), tr("Nth root"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnNthRoot);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/subscript.png"), tr("Subscript"), this);
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

    action = new QAction(QIcon(":/icons/images/algebra/assignment.png"), tr("Assignment"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnAssignment);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/unit.png"), tr("Unit"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnUnit);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/equation.png"), tr("Equation"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnEquation);
    algebra_toolbar->addAction(action);
}

void MainWindow::CreateTrigonometryToolbar()
{
    //trigonometry toolbar
    trigonometry_toolbar = addToolBar(tr("Trigonometric functions"));
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
    greek_toolbar->setStyleSheet("QToolBar{spacing:4px;}");

    auto add_greek_letter = 
        [greek_toolbar = greek_toolbar, this](const QChar& letter)
        {
            QAction* action = new QAction(letter, this);
            action->setData(letter);
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

void MainWindow::CreateStatusBar()
{
    if (!locale_status)
    {
        locale_status = new QLabel("");
        statusBar()->addWidget(locale_status);
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
    AddEditorTab("(No name)");
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
    AddEditorTab(file_info.fileName());

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
        document->Save(w->path.toUtf8().data());
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
    auto document = w->document;
    if (!document)
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
    document->Save(file_names[0].toUtf8().data());

    UpdateCaption();
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
            w->document->SetConfig(_config, true);
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
        greek_toolbar_action->setChecked(settings.value("MainWindow/greek_toolbar", false).toBool());

        if (last_language != config.language)
        {
            InstallTranslation(config.language);
            UpdateLocaleMessage();
        }
        
        logger->SetLevel((int)config.log_level);
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
    else if (mime_data->hasImage())
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

void MainWindow::HelpOnline()
{
    QUrl u(tr("help_online"));
    QDesktopServices::openUrl(u);
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
            file_name = "(No name)";
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
        return;
    
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
}

void MainWindow::OnSaveResult(const uint task_id, IOResult result)
{
    if (result != IOResult::Success)
    {
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

    UpdateCaption();
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

#ifdef REMOTE_SOLVER
void MainWindow::OnServiceStatus(IOResult result)
{
    if (result != IOResult::Success)
        RestartService();
}
#endif

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
    settings.setValue("MainWindow/standard_toolbar", standard_toolbar_action->isChecked());
    settings.setValue("MainWindow/format_toolbar", format_toolbar_action->isChecked());
    settings.setValue("MainWindow/algebra_toolbar", algebra_toolbar_action->isChecked());
    settings.setValue("MainWindow/trigonometry_toolbar", trigonometry_toolbar_action->isChecked());
    settings.setValue("MainWindow/hyperbolic_toolbar", hyperbolic_toolbar_action->isChecked());
    settings.setValue("MainWindow/functions_toolbar", functions_toolbar_action->isChecked());
    settings.setValue("MainWindow/greek_toolbar", greek_toolbar_action->isChecked());
    settings.setValue("MainWindow/currency_toolbar", currency_toolbar_action->isChecked());
    settings.setValue("MainWindow/status_bar", status_bar_action->isChecked());
    settings.setValue("MainWindow/language", (int)config.language);

    settings.setValue("Documents/last_documents", last_documents.isEmpty() ? "" : QVariant(last_documents));
    settings.setValue("Documents/load_last_documents", true);

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
    if (lang != "en" && lang != "ru")
        lang = "en";

    config.language = (yutovo_calculator::Language)settings.value("MainWindow/language",
        lang == "en" ? (int)yutovo_calculator::Language::English : (int)yutovo_calculator::Language::Russian).toInt();

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

void MainWindow::UpdateLibraryMenu(QMenu* library_menu)
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

            std::vector<std::filesystem::path> sorted_dirs;
            std::vector<std::filesystem::path> sorted_files;
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
                        auto it = std::find(order.begin(), order.end(), s);
                        if (it != order.end())
                            pos = std::distance(order.begin(), it);
                        if (pos == -1)
                            sorted_dirs.push_back(entry.path());
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
                                sorted_files.push_back(entry.path());
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

    auto p = std::string(GetLibraryDir().toUtf8().data()) + (config.language == yutovo_calculator::Language::English ? "en" : "ru");
    if (!std::filesystem::exists(p))
        return;
    
    get_files(std::filesystem::path(p), L"library", library_menu);
}

void MainWindow::InstallTranslation(const yutovo_calculator::Language language)
{
    qApp->removeTranslator(&desktop_translator);
    qApp->removeTranslator(&editor_translator);
    
    if (language == yutovo_calculator::Language::Russian)
    {
        if (!desktop_translator.load("yutovo_desktop_ru", GetTranslationDir("yutovo_desktop_ru")))
            logger->Error("Error loading translation: yutovo_desktop_ru");
        if (!editor_translator.load("yutovo_editor_ru", GetTranslationDir("yutovo_editor_ru")))
            logger->Error("Error loading translation: yutovo_editor_ru");
    }
    else if (language == yutovo_calculator::Language::English)
    {
        if (!desktop_translator.load("yutovo_desktop_en", GetTranslationDir("yutovo_desktop_en")))
            logger->Error("Error loading translation: yutovo_desktop_en");
        if (!editor_translator.load("yutovo_editor_en", GetTranslationDir("yutovo_editor_en")))
            logger->Error("Error loading translation: yutovo_editor_en");
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
        file_name = "(No name)";
    else
        file_name = file_info.fileName();
    if (w->document->IsChanged())
        file_name += " *";

    ui->editor_tabs->setTabText(tab == -1 ? ui->editor_tabs->currentIndex() : tab, file_name);
    if (update_title)
        setWindowTitle(file_name + " - " + tr("Yutovo"));
}

void MainWindow::UpdateLocaleMessage()
{
    auto document = GetCurrentDocument();
    if (!document)
        return;
    Config c;
    document->GetConfig(c);
    locale_status->setText(tr("Locale: ") + (c.language == yutovo_calculator::Language::English ? tr("English") : tr("Russian")));
}

void MainWindow::EnableButtons(bool enable)
{
    save_action->setEnabled(enable);
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
    paragraph_format_combo->setEnabled(enable);
    family_combo->setEnabled(enable);
    size_combo->setEnabled(enable);
    algebra_toolbar->setEnabled(enable);
    trigonometry_toolbar->setEnabled(enable);
    hyperbolic_toolbar->setEnabled(enable);
    functions_toolbar->setEnabled(enable);
    greek_toolbar->setEnabled(enable);
    currency_toolbar->setEnabled(enable);
}

#ifdef REMOTE_SOLVER
void MainWindow::RestartService()
{
    logger->Info("Restarting service");
    service.reset(new QProcess(this));
    service->setWorkingDirectory(".");
    service->start("./yutovo_serviced");
}
#endif

QString MainWindow::GetLibraryDir()
{
    if (std::filesystem::exists("./library/"))
        return "./library/";
    QString p = QCoreApplication::applicationDirPath();
    return p + "/library/";
}

QString MainWindow::GetTranslationDir(QString filename)
{
    if (std::filesystem::exists(filename.toUtf8().data()))
        return "./";
    return QCoreApplication::applicationDirPath();
}
