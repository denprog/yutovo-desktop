#include "mainwindow.h"
#include <QMenu>
#include <QToolBar>
#include <QToolButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QMimeData>
#include <QScrollBar>
#include <QLineEdit>
#include <QFileInfo> 
#include <yutovo_editor/util.h>
#include "document_window.h"
#include "about_dialog.h"
#include "settings_dialog.h"

//MainWindow

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings(new QSettings("Yutovo", "Yutovo Desktop")),
    logger(Logger::GetInstance(".", "yutovo_desktop", true, true))
{
    ui->setupUi(this);

    qRegisterMetaType<Rect>("Rect");
    qRegisterMetaType<CaretState>("CaretState");
    qRegisterMetaType<EditorState>("EditorState");
    qRegisterMetaType<IOResult>("IOResult");
    qRegisterMetaType<CopyResult>("CopyResult");
    qRegisterMetaType<std::vector<ElementPtr>>("std::vector<ElementPtr>");

    ReadSettings();

    SetupGui();

    UpdateRecentFiles();

    logger->Info("Desktop start");
}

MainWindow::~MainWindow()
{
    WriteSettings();
    delete ui;
    logger->Info("Desktop stop");
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event)
{
    DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->currentWidget();
    if (w)
        w->MakeContextMenu(event);
}

void MainWindow::SetupGui()
{
    ui->editor_tabs->clear();
    ui->editor_tabs->setTabsClosable(true);
    connect(ui->editor_tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(OnCloseEditorTab(int)));
    connect(ui->editor_tabs, SIGNAL(currentChanged(int)), this, SLOT(OnEditorChanged(int)));

    CreateActions();
    addToolBarBreak();
    CreateAlgebraToolbar();
    addToolBarBreak();
    CreateTrigonometryToolbar();
    addToolBarBreak();
    CreateHyperbolicToolbar();
    addToolBarBreak();
    CreateFunctionsToolbar();

    AddEditorTab("(No name)");

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
    b = settings.value("MainWindow/status_bar", true).toBool();
    status_bar_action->setChecked(b);
}

void MainWindow::AddEditorTab(const QString name)
{
    DocumentWindow* wnd = new DocumentWindow(config, this);

    connect(wnd, &DocumentWindow::CaretMoved, this, &MainWindow::OnCaretMoved);
    connect(wnd, &DocumentWindow::SaveResult, this, &MainWindow::OnSaveResult);
    connect(wnd, &DocumentWindow::LoadResult, this, &MainWindow::OnLoadResult);
    connect(wnd, &DocumentWindow::ClipboardCopyResult, this, &MainWindow::OnClipboardCopyResult);
    connect(wnd, &DocumentWindow::ClipboardPasteResult, this, &MainWindow::OnClipboardPasteResult);

    connect(wnd->document_widget, &DocumentWidget::NextEditorTab, this, &MainWindow::OnNextEditorTab);
    connect(wnd->document_widget, &DocumentWidget::PrevEditorTab, this, &MainWindow::OnPrevEditorTab);

    ui->editor_tabs->addTab(wnd, name);
    ui->editor_tabs->setCurrentIndex(ui->editor_tabs->count() - 1);
    wnd->SetFocus();

    wnd->CreateDocument();

    auto document = GetCurrentDocument();
    auto s = wnd->document_widget->size();
    document->Resize(s.width(), s.height());
    OnCaretMoved(document->GetEditorState());
    FillParagraphFormats();
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
    QMenu *file_menu = menuBar()->addMenu(tr("&File"));
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

    action = new QAction(QIcon(":/icons/images/standard/save.png"), tr("&Save"), this);
    action->setShortcuts(QKeySequence::Save);
    action->setStatusTip(tr("Save the document to disk"));
    connect(action, &QAction::triggered, this, &MainWindow::Save);
    file_menu->addAction(action);
    standard_toolbar->addAction(action);

    action = file_menu->addAction(QIcon(":/images/standard/new.png"), tr("Save &As..."), this, &MainWindow::SaveAs);
    action->setShortcuts(QKeySequence::SaveAs);
    action->setStatusTip(tr("Save the document under a new name"));

    action = new QAction(tr("&Close"), this);
    connect(action, &QAction::triggered, this, &MainWindow::Close);
    file_menu->addAction(action);

    file_menu->addSeparator();

    action = new QAction(tr("&Settings"), this);
    connect(action, &QAction::triggered, this, &MainWindow::Settings);
    file_menu->addAction(action);

    file_menu->addSeparator();

    action = file_menu->addAction(QIcon::fromTheme("application-exit"), tr("E&xit"), this, &MainWindow::Exit);
    action->setShortcuts(QKeySequence::Quit);
    action->setStatusTip(tr("Exit the application"));

    //edit menu and toolbar
    QMenu *edit_menu = menuBar()->addMenu(tr("&Edit"));

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

    UpdateCopyPaste();

    //format toolbar
    format_toolbar = addToolBar(tr("Format"));
    format_toolbar->setStyleSheet("QToolBar{spacing:4px;}");

    action = new QAction(QIcon(":/icons/images/format/code.png"), tr("&Code"), this);
    action->setStatusTip(tr("Insert code"));
    connect(action, &QAction::triggered, this, &MainWindow::OnInsertCode);
    format_toolbar->addAction(action);

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
    format_toolbar->addAction(bold_action);

    italic_action = new QAction(QIcon(":/icons/images/format/italic.png"), tr("Italic"), this);
    connect(italic_action, &QAction::triggered, this, &MainWindow::OnItalic);
    italic_action->setCheckable(true);
    format_toolbar->addAction(italic_action);

    underline_action = new QAction(QIcon(":/icons/images/format/underline.png"), tr("Underline"), this);
    connect(underline_action, &QAction::triggered, this, &MainWindow::OnUnderline);
    underline_action->setCheckable(true);
    format_toolbar->addAction(underline_action);

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

    status_bar_action = new QAction(tr("&Status bar"), this);
    status_bar_action->setCheckable(true);
    status_bar_action->setChecked(true);
    connect(status_bar_action, &QAction::toggled, this, &MainWindow::StatusBar);
    view_menu->addAction(status_bar_action);

    //help menu
    QMenu* help_menu = menuBar()->addMenu(tr("&Help"));
    action = help_menu->addAction(tr("&About"), this, &MainWindow::About);
    help_menu->setStatusTip(tr("Show the application's About box"));
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

    action = new QAction(QIcon(":/icons/images/algebra/fences.png"), tr("Fences"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnFences);
    algebra_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/algebra/assignment.png"), tr("Assignment"), this);
    connect(action, &QAction::triggered, this, &MainWindow::OnAssignment);
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

void MainWindow::CreateStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
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

    auto document = GetCurrentDocument();
    if (!document)
        return;
    document->New();
}

void MainWindow::Open()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open file"), "", tr("Yutovo files (*.yut);;Text files (*.txt)"));
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

void MainWindow::OpenFile(QString file_name)
{
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
    document->Load(file_name.toUtf8().data());

    DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->currentWidget();
    w->path = file_name;
}

void MainWindow::Save()
{
    auto document = GetCurrentDocument();
    if (!document)
        return;
    DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->currentWidget();
    if (w->path == "")
        SaveAs();
    else
    {
        dialog_file_name = w->path;
        document->Save(w->path.toUtf8().data());
    }
}

void MainWindow::SaveAs()
{
    auto document = GetCurrentDocument();
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

    int p = ui->editor_tabs->currentIndex();
    if (p == -1)
        return;
    QFileInfo file_info(file_names[0]);
    ui->editor_tabs->setTabText(p, file_info.fileName());

    DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->currentWidget();
    w->path = file_names[0];
}

void MainWindow::Close()
{
    int p = ui->editor_tabs->currentIndex();
    if (p == -1)
        return;
    OnCloseEditorTab(p);
}

void MainWindow::Settings()
{
    QHash<QString, QVariant> _settings;
    QStringList keys = settings.allKeys();
    Q_FOREACH(QString key, keys)
    {
        _settings[key] = settings.value(key);
    }

    Config _config = config;
    int r;
    {
        SettingsDialog settings_dialog(_config, _settings);
        r = settings_dialog.exec();
    }

    if (r == QDialog::Accepted)
    {
        config = _config;
        for (int i = 0; i < ui->editor_tabs->count(); ++i)
        {
            DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->widget(i);
            w->document->SetConfig(config);
        }

        keys = _settings.keys();
        Q_FOREACH(QString key, keys)
        {
            settings.setValue(key, _settings.value(key));
        }

        standard_toolbar_action->setChecked(settings.value("MainWindow/standard_toolbar", false).toBool());
        format_toolbar_action->setChecked(settings.value("MainWindow/format_toolbar", false).toBool());
        algebra_toolbar_action->setChecked(settings.value("MainWindow/algebra_toolbar", false).toBool());
        trigonometry_toolbar_action->setChecked(settings.value("MainWindow/trigonometry_toolbar", false).toBool());
        hyperbolic_toolbar_action->setChecked(settings.value("MainWindow/hyperbolic_toolbar", false).toBool());
        functions_toolbar_action->setChecked(settings.value("MainWindow/functions_toolbar", false).toBool());
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
            document->PasteText(ToUtfString(s.toUtf8().data()));
            return;
        }
        std::stringstream str(arr.toStdString());
        auto s = ToUtfString(str.str());
        document->Paste(s);
    }
    else if (mime_data->hasImage())
    {
        QImage image = clipboard->image();
        image.convertTo(QImage::Format_ARGB32);
        std::vector<unsigned char> data(image.bits(), image.bits() + image.sizeInBytes());
        document->PasteImage(data, image.width(), image.height());
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

void MainWindow::OnCloseEditorTab(int index)
{
    if (index == -1)
        return;
    QWidget* tab_item = ui->editor_tabs->widget(index);
    ui->editor_tabs->removeTab(index);
    delete(tab_item);
    SetFocus();
}

void MainWindow::OnEditorChanged(int index)
{
    auto document = GetCurrentDocument();
    if (!document)
        return;
    FillParagraphFormats();
    OnCaretMoved(document->GetEditorState());
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

void MainWindow::OnFences()
{
    auto document = GetCurrentDocument();
    if (document)
        document->InsertFences(true);
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

void MainWindow::OnCaretMoved(const EditorState editor_state)
{
    auto document = GetCurrentDocument();
    if (!document)
        return;
    
    const CaretState& c = editor_state.caret_state;
    const SelectionState& s = editor_state.selection_state;
    StringFormat format;
    ParagraphFormat paragraph_format;

    //find common paragraph format
    document->GetParagraphFormat(c.id, paragraph_format);
    for (auto& state : s.state)
    {
        ParagraphFormat p;
        if (document->GetParagraphFormat(c.id, p))
        {
            if (p.name != paragraph_format.name)
            {
                paragraph_format.name = "";
                break;
            }
        }
    }

    //find common string format
    if (c.id.empty() || c.id.size() == 1)
        return;
    ElementId _id = GetParent(c.id);
    auto t = document->GetElementType(_id);
    if (!document->IsString(document->GetElement(_id)) && !document->IsRow(document->GetElement(_id)))
    {
        format.Reset();
    }
    else if (!s.IsEmpty())
    {
        bool set_family = true, set_size = true, set_bold = true, set_italic = true, set_underline = true;
        for (auto& state : s.state)
        {
            for (int i = state.start; i < state.start + state.size; ++i)
            {
                ElementId _id = GetChild(state.id, i);
                StringFormat f;
                if (document->GetStringFormat(_id, f))
                {
                    if (set_family)
                    {
                        if (format.family == "")
                            format.family = f.family;
                        else if (format.family != f.family)
                        {
                            format.family = "";
                            set_family = false;
                        }
                    }
                    if (set_size)
                    {
                        if (format.size == 0)
                            format.size = f.size;
                        else if (format.size != f.size)
                        {
                            format.size = 0;
                            set_size = false;
                        }
                    }
                    if (set_bold)
                    {
                        if (format.bold == true && f.bold == false)
                        {
                            format.bold = false;
                            set_bold = false;
                        }
                        else
                            format.bold = f.bold;
                    }
                    if (set_italic)
                    {
                        if (format.italic == true && f.italic == false)
                        {
                            format.italic = false;
                            set_italic = false;
                        }
                        else
                            format.italic = f.italic;
                    }
                    if (set_underline)
                    {
                        if (format.underline == true && f.underline == false)
                        {
                            format.underline = false;
                            set_underline = false;
                        }
                        else
                            format.underline = f.underline;
                    }
                }
            }
        }
    }
    else
    {
        document->GetStringFormat(_id, format);
    }

    //update the interface elements
    block_format_slots = true;
    paragraph_format_combo->setCurrentText(paragraph_format.name.c_str());
    family_combo->setCurrentText(format.family.c_str());
    if (format.size == 0)
        size_combo->setCurrentText("");
    else
        size_combo->setCurrentText(std::to_string(format.size).c_str());
    bold_action->setChecked(format.bold);
    italic_action->setChecked(format.italic);
    underline_action->setChecked(format.underline);
    block_format_slots = false;

    undo_action->setEnabled(document->CanUndo());
    redo_action->setEnabled(document->CanRedo());

    UpdateCopyPaste();
}

void MainWindow::OnSaveResult(const uint task_id, IOResult result)
{
    if (result != IOResult::Success)
    {
        recent_files.removeAll(dialog_file_name);
        UpdateRecentFiles();
        QMessageBox::critical(this, tr("Yutovo"), tr("Error saving document"));
        return;
    }
    UpdateRecentFiles(dialog_file_name);
}

void MainWindow::OnLoadResult(const uint task_id, IOResult result)
{
    if (result != IOResult::Success)
    {
        recent_files.removeAll(dialog_file_name);
        DocumentWindow* w = (DocumentWindow*)ui->editor_tabs->currentWidget();
        if (w)
        {
            w->path = "";
            ui->editor_tabs->setTabText(ui->editor_tabs->currentIndex(), "(No name)");
        }
        UpdateRecentFiles();
        QMessageBox::critical(this, tr("Yutovo"), tr("Error loading document"));
        return;
    }
    UpdateRecentFiles(dialog_file_name);
}

void MainWindow::OnClipboardCopyResult(CopyResult result)
{
    if (result != CopyResult::Success)
        return;
    QClipboard* clipboard = QGuiApplication::clipboard();
    QMimeData* mime_data = new QMimeData;
    auto s = ToBasicString(clipboard_json);

    QByteArray item_data(s.c_str(), s.length());
    //custom web clipboard type
    mime_data->setData("application/web;type=\"custom/format0\"", item_data);
    mime_data->setData("application/web;type=\"custom/formatmap\"", "{\"yutovo/elements\":\"application/web;type=\\\"custom/format0\\\"\"}");

    mime_data->setText(ToBasicString(clipboard_text).c_str()); //text clipboard type

    clipboard->setMimeData(mime_data);
    clipboard_json = U"";
    clipboard_text = U"";
}

void MainWindow::OnClipboardPasteResult(PasteResult result)
{
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
    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/standard_toolbar", standard_toolbar_action->isChecked());
    settings.setValue("MainWindow/format_toolbar", format_toolbar_action->isChecked());
    settings.setValue("MainWindow/algebra_toolbar", algebra_toolbar_action->isChecked());
    settings.setValue("MainWindow/trigonometry_toolbar", trigonometry_toolbar_action->isChecked());
    settings.setValue("MainWindow/hyperbolic_toolbar", hyperbolic_toolbar_action->isChecked());
    settings.setValue("MainWindow/functions_toolbar", functions_toolbar_action->isChecked());
    settings.setValue("MainWindow/status_bar", status_bar_action->isChecked());

    settings.beginGroup("RecentFiles");
    settings.setValue("max_count", recent_files_count);
    settings.setValue("files", QVariant(recent_files));
    settings.endGroup();

    settings.beginGroup("Service");
    settings.setValue("ip", config.service_ip.c_str());
    settings.setValue("port", config.service_port);
    settings.setValue("timeout", config.service_timeout);
    settings.endGroup();

    settings.beginGroup("Calculator");
    settings.setValue("result_auto_advance", config.auto_result.result_auto_advance);
    QList<QVariant> v;
    for (auto r : config.auto_result.results_order)
        v.push_back((int)r);
    settings.setValue("results_order", v);

    settings.setValue("real_precision", config.real_result.precision);
    settings.setValue("real_exp", config.real_result.exp);
    settings.setValue("real_result_angle_measure", (int)config.real_result.result_angle_measure);
    settings.setValue("real_show_angle_measure", config.real_result.show_angle_measure);

    settings.setValue("integer_default_notation", (int)config.integer_result.default_notation);
    settings.setValue("integer_result_notation", (int)config.integer_result.result_notation);
    settings.setValue("integer_show_notation", config.integer_result.show_notation);

    settings.setValue("rational_fraction_form", (int)config.rational_result.fraction_form);

    settings.setValue("complex_precision", config.complex_result.precision);
    settings.setValue("complex_exp", config.complex_result.exp);
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

    settings.beginGroup("Service");
    config.service_ip = settings.value("ip", "localhost").toString().toUtf8().data();
    config.service_port = settings.value("port", 8010).toInt();
    config.service_timeout = settings.value("timeout", 20).toInt();
    settings.endGroup();

    settings.beginGroup("Calculator");
    config.auto_result.result_auto_advance = settings.value("result_auto_advance", true).toBool();
    QList<QVariant> v = settings.value("results_order").toList();
    size_t i = 0;
    for (auto r : v)
    {
        if (i < 4)
            config.auto_result.results_order[i++] = (ResultType)r.toInt();
    }
    auto& results_order = config.auto_result.results_order;
    for (; i < 4; ++i)
    {
        if (std::find(std::begin(results_order), std::end(results_order), ResultType::REAL) == std::end(results_order))
            results_order[i++] = ResultType::REAL;
        if (std::find(std::begin(results_order), std::end(results_order), ResultType::INTEGER) == std::end(results_order))
            results_order[i++] = ResultType::INTEGER;
        if (std::find(std::begin(results_order), std::end(results_order), ResultType::RATIONAL) == std::end(results_order))
            results_order[i++] = ResultType::RATIONAL;
        if (std::find(std::begin(results_order), std::end(results_order), ResultType::COMPLEX) == std::end(results_order))
            results_order[i++] = ResultType::COMPLEX;
    }

    config.real_result.precision = settings.value("real_precision", 3).toInt();
    if (config.real_result.precision < 1)
        config.real_result.precision = 3;
    config.real_result.exp = settings.value("real_exp", 10).toInt();
    if (config.real_result.exp < 1)
        config.real_result.exp = 10;
    config.real_result.result_angle_measure = (AngleMeasure)settings.value("real_result_angle_measure", 0).toInt();
    if ((int)config.real_result.result_angle_measure < 0 || (int)config.real_result.result_angle_measure > 2)
        config.real_result.result_angle_measure = AngleMeasure::RADIAN;
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
    config.complex_result.result_angle_measure = (AngleMeasure)settings.value("complex_result_angle_measure", 0).toInt();
    if ((int)config.complex_result.result_angle_measure < 0 || (int)config.complex_result.result_angle_measure > 2)
        config.complex_result.result_angle_measure = AngleMeasure::RADIAN;
    config.complex_result.show_angle_measure = settings.value("complex_show_angle_measure", false).toBool();
    config.complex_result.form = (ComplexForm)settings.value("complex_form", 0).toInt();
    if ((int)config.complex_result.form < 0 || (int)config.complex_result.form > 2)
        config.complex_result.form = ComplexForm::ARITHMETIC;
    config.complex_result.max_count = settings.value("complex_max_count", 10).toInt();
    if (config.complex_result.max_count < 0 || config.complex_result.max_count > 20)
        config.complex_result.max_count = 10;
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
    auto document = GetCurrentDocument();
    if (!document)
        return;
    
    EditorState s = document->GetEditorState();
    bool editable = document->IsEditable(s.caret_state.id);

    copy_action->setEnabled(!s.selection_state.IsEmpty());

    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* mime_data = clipboard->mimeData();
    paste_action->setEnabled(editable && mime_data->hasText());

    cut_action->setEnabled(editable && !s.selection_state.IsEmpty());
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
