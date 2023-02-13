#include "mainwindow.h"
#include <QMenu>
#include <QToolBar>
#include <QToolButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QMimeData>
#include <QScrollBar>
#include <yutovo_editor/util.h>
#include "about_dialog.h"

//MainWindow

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qRegisterMetaType<Rect>("Rect");
    qRegisterMetaType<CaretState>("CaretState");
    qRegisterMetaType<EditorState>("EditorState");
    qRegisterMetaType<IOResult>("IOResult");
    qRegisterMetaType<CopyResult>("CopyResult");
    qRegisterMetaType<std::vector<ElementPtr>>("std::vector<ElementPtr>");

    document_widget = new DocumentWidget(ui->centralwidget);
    document = document_widget->CreateDocument();

    CreateStatusBar();
    SetupGui();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetupGui()
{
    document_widget->setObjectName(QStringLiteral("document_widget"));

    vertical_scroll = new QScrollBar(Qt::Vertical);
    horizontal_scroll = new QScrollBar(Qt::Horizontal);
    ui->gridLayout->addWidget(document_widget, 0, 0);
    ui->gridLayout->addWidget(vertical_scroll, 0, 1);
    ui->gridLayout->addWidget(horizontal_scroll, 1, 0);

    connect(vertical_scroll, &QAbstractSlider::valueChanged, this, &MainWindow::OnVerticalValueChanged);
    connect(horizontal_scroll, &QAbstractSlider::valueChanged, this, &MainWindow::OnHorizontalValueChanged);
    vertical_scroll->setMinimum(0);
    vertical_scroll->setSingleStep(10);
    horizontal_scroll->setMinimum(0);
    horizontal_scroll->setSingleStep(10);

    connect(&document_widget->window, &QtWindow::CaretMoved, this, &MainWindow::OnCaretMoved);
    connect(&document_widget->window, &QtWindow::SaveResult, this, &MainWindow::OnSaveResult);
    connect(&document_widget->window, &QtWindow::LoadResult, this, &MainWindow::OnLoadResult);
    connect(&document_widget->window, &QtWindow::ClipboardCopyResult, this, &MainWindow::OnClipboardCopyResult);

    connect(&document_widget->window, &QtWindow::DocumentUpdated, this, &MainWindow::OnDocumentUpdated);

    CreateActions();
    addToolBarBreak();
    CreateAlgebraToolbar();
    addToolBarBreak();
    CreateTrigonometryToolbar();
    addToolBarBreak();
    CreateHyperbolicToolbar();
    addToolBarBreak();
    CreateFunctionsToolbar();

    document->Start();
}

void MainWindow::CreateActions()
{
    //file menu and toolbar
    QMenu *file_menu = menuBar()->addMenu(tr("&File"));
    QToolBar *standard_toolbar = addToolBar(tr("Standard"));

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

    action = new QAction(QIcon(":/icons/images/standard/save.png"), tr("&Save"), this);
    action->setShortcuts(QKeySequence::Save);
    action->setStatusTip(tr("Save the document to disk"));
    connect(action, &QAction::triggered, this, &MainWindow::Save);
    file_menu->addAction(action);
    standard_toolbar->addAction(action);

    action = file_menu->addAction(QIcon(":/images/standard/new.png"), tr("Save &As..."), this, &MainWindow::SaveAs);
    action->setShortcuts(QKeySequence::SaveAs);
    action->setStatusTip(tr("Save the document under a new name"));

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

    action = new QAction(QIcon(":/icons/images/standard/cut.png"), tr("Cu&t"), this);
    action->setShortcuts(QKeySequence::Cut);
    action->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    connect(action, &QAction::triggered, this, &MainWindow::Cut);
    edit_menu->addAction(action);
    standard_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/standard/copy.png"), tr("&Copy"), this);
    action->setShortcuts(QKeySequence::Copy);
    action->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
    connect(action, &QAction::triggered, this, &MainWindow::Copy);
    edit_menu->addAction(action);
    standard_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/standard/paste.png"), tr("&Paste"), this);
    action->setShortcuts(QKeySequence::Paste);
    action->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
    connect(action, &QAction::triggered, this, &MainWindow::Paste);
    edit_menu->addAction(action);
    standard_toolbar->addAction(action);

    //fonts toolbar
    QToolBar *format_toolbat = addToolBar(tr("Format"));
    format_toolbat->setStyleSheet("QToolBar{spacing:4px;}");

    action = new QAction(QIcon(":/icons/images/format/code.png"), tr("&Code"), this);
    action->setStatusTip(tr("Insert code"));
    connect(action, &QAction::triggered, this, &MainWindow::OnInsertCode);
    format_toolbat->addAction(action);

    format_toolbat->addSeparator();

    paragraph_format_combo = new QComboBox;
    format_toolbat->addWidget(paragraph_format_combo);
    FillParagraphFormats();
    connect(paragraph_format_combo, &QComboBox::currentTextChanged, this, &MainWindow::OnCurrentParagraphFormatChanged);

    format_toolbat->addSeparator();

    family_combo = new QFontComboBox;
    connect(family_combo, &QFontComboBox::currentFontChanged, this, &MainWindow::OnCurrentFontChanged);
    format_toolbat->addWidget(family_combo);

    size_combo = new QComboBox;
    connect(size_combo, &QComboBox::currentTextChanged, this, &MainWindow::OnCurrentSizeChanged);
    format_toolbat->addWidget(size_combo);
    FillSizes(family_combo->currentFont());

    bold_action = new QAction(QIcon(":/icons/images/format/bold.png"), tr("Bold"), this);
    connect(bold_action, &QAction::triggered, this, &MainWindow::OnBold);
    bold_action->setCheckable(true);
    format_toolbat->addAction(bold_action);

    italic_action = new QAction(QIcon(":/icons/images/format/italic.png"), tr("Italic"), this);
    connect(italic_action, &QAction::triggered, this, &MainWindow::OnItalic);
    italic_action->setCheckable(true);
    format_toolbat->addAction(italic_action);

    underline_action = new QAction(QIcon(":/icons/images/format/underline.png"), tr("Underline"), this);
    connect(underline_action, &QAction::triggered, this, &MainWindow::OnUnderline);
    underline_action->setCheckable(true);
    format_toolbat->addAction(underline_action);

    //view menu
    QMenu* view_menu = menuBar()->addMenu(tr("&View"));

    //toolbars submenu
    QMenu* toolbars_menu = view_menu->addMenu(tr("&Toolbars"));

    algebra_toolbar_action = new QAction(tr("Algebraic operations"), this);
    algebra_toolbar_action->setCheckable(true);
    algebra_toolbar_action->setChecked(true);
    connect(algebra_toolbar_action, &QAction::triggered, this, &MainWindow::AlgebraToolbar);
    toolbars_menu->addAction(algebra_toolbar_action);

    trigonometry_toolbar_action = new QAction(tr("Trigonometric functions"), this);
    trigonometry_toolbar_action->setCheckable(true);
    trigonometry_toolbar_action->setChecked(true);
    connect(trigonometry_toolbar_action, &QAction::triggered, this, &MainWindow::TrigonometryToolbar);
    toolbars_menu->addAction(trigonometry_toolbar_action);

    hyperbolic_toolbar_action = new QAction(tr("Hyperbolic functions"), this);
    hyperbolic_toolbar_action->setCheckable(true);
    hyperbolic_toolbar_action->setChecked(true);
    connect(hyperbolic_toolbar_action, &QAction::triggered, this, &MainWindow::HyperbolicToolbar);
    toolbars_menu->addAction(hyperbolic_toolbar_action);

    functions_toolbar_action = new QAction(tr("Hyperbolic functions"), this);
    functions_toolbar_action->setCheckable(true);
    functions_toolbar_action->setChecked(true);
    connect(functions_toolbar_action, &QAction::triggered, this, &MainWindow::FunctionsToolbar);
    toolbars_menu->addAction(functions_toolbar_action);

    status_bar_action = new QAction(tr("&Status bar"), this);
    status_bar_action->setCheckable(true);
    status_bar_action->setChecked(true);
    connect(status_bar_action, &QAction::triggered, this, &MainWindow::StatusBar);
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
    //algebra_toolbar->setIconSize(QSize(30, 20));

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
    connect(action, &QAction::triggered, this, &MainWindow::OnDivision);
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

void MainWindow::New()
{
    document->New();
    current_file_name = "";
}

void MainWindow::Open()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open file"), "", tr("Yutovo files (*.yut)"));
    if (file_name == "")
        return;
    document->Load(file_name.toUtf8().data());
    current_file_name = file_name;
}

void MainWindow::Save()
{
    if (current_file_name == "")
        SaveAs();
    else
        document->Save(current_file_name.toUtf8().data());
}

void MainWindow::SaveAs()
{
    QFileDialog save_dialog(this, tr("Save file as"), "", tr("Yutovo files (*.yut)"));
    save_dialog.setDefaultSuffix("yut");
    save_dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (!save_dialog.exec())
        return;
    QStringList file_names = save_dialog.selectedFiles();
    if (file_names.empty())
        return;
    document->Save(file_names[0].toUtf8().data());
    current_file_name = file_names[0];
}

void MainWindow::Exit()
{
    close();
}

void MainWindow::Copy()
{
    document->Copy(clipboard_array, clipboard_text);
}

void MainWindow::Paste()
{
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* mime_data = clipboard->mimeData();
    if (mime_data->hasFormat("yutovo/elements"))
    {
        QByteArray arr = mime_data->data("yutovo/elements"); //firstly check the own format
        if (arr.isEmpty())
        {
            //otherwise check if there is a text in the clipboard
            QString s;
            if (mime_data->hasText())
                s = mime_data->text();
            if (s == "")
                return;
            document->Paste(s.toUtf8().data());
            return;
        }
        std::stringstream str(arr.toStdString());
        document->Paste(str);
    }
    else
    {
        QString s = clipboard->text();
        if (s != "")
            document->Paste(s.toUtf8().data());
    }
}

void MainWindow::Cut()
{
    document->Cut(clipboard_array, clipboard_text);
}

void MainWindow::Undo()
{
    document->Undo();
}

void MainWindow::Redo()
{
    document->Redo();
}

void MainWindow::About()
{
    AboutDialog about_dialog;
    about_dialog.exec();
}

void MainWindow::AlgebraToolbar()
{
    algebra_toolbar_action->isChecked() ? algebra_toolbar->show() : algebra_toolbar->hide();
}

void MainWindow::TrigonometryToolbar()
{
    trigonometry_toolbar_action->isChecked() ? trigonometry_toolbar->show() : trigonometry_toolbar->hide();
}

void MainWindow::HyperbolicToolbar()
{
    hyperbolic_toolbar_action->isChecked() ? hyperbolic_toolbar->show() : hyperbolic_toolbar->hide();
}

void MainWindow::FunctionsToolbar()
{
    functions_toolbar_action->isChecked() ? functions_toolbar->show() : functions_toolbar->hide();
}

void MainWindow::StatusBar()
{
    status_bar_action->isChecked() ? statusBar()->show() : statusBar()->hide();
}

void MainWindow::OnVerticalValueChanged(int value)
{
    document_widget->window.document_point.y = (value > 0 ? value : 0);
    document->Redraw();
}

void MainWindow::OnHorizontalValueChanged(int value)
{
    document_widget->window.document_point.x = (value > 0 ? value : 0);
    document->Redraw();
}

void MainWindow::OnInsertCode()
{
    document->InsertCode(false, true);
}

void MainWindow::OnCurrentParagraphFormatChanged(const QString& format)
{
    document->SetCurrentParagraphFormat(format.toUtf8().data());
    document_widget->setFocus();
}

void MainWindow::OnCurrentFontChanged(const QFont& font)
{
    FillSizes(font);
    document->SetFontFamily(font.family().toUtf8().data());
    document_widget->setFocus();
}

void MainWindow::OnCurrentSizeChanged(const QString& size)
{
    int s;
    try
    {
        s = std::stoi(size.toUtf8().data());
    }
    catch (...)
    {
        return;
    }
    document->SetFontSize(s);
    document_widget->setFocus();
}

void MainWindow::OnBold()
{
    document->SetBold(bold_action->isChecked());
}

void MainWindow::OnItalic()
{
    document->SetItalic(italic_action->isChecked());
}

void MainWindow::OnUnderline()
{
    document->SetUnderline(underline_action->isChecked());
}

void MainWindow::OnPlus()
{
    document->InsertPlus(true);
}

void MainWindow::OnMinus()
{
    document->InsertMinus(true);
}

void MainWindow::OnMultiply()
{
    document->InsertMultiply(true);
}

void MainWindow::OnDivision()
{
    document->InsertDivision(true);
}

void MainWindow::OnSquareRoot()
{
    document->InsertSquareRoot(true);
}

void MainWindow::OnNthRoot()
{
    document->InsertNthRoot(true);
}

void MainWindow::OnPower()
{
    document->InsertPower(true);
}

void MainWindow::OnSubscript()
{
    document->InsertSubscript(true);
}

void MainWindow::OnExp()
{
    document->InsertFunction("exp", true);
}

void MainWindow::OnLn()
{
    document->InsertFunction("ln", true);
}

void MainWindow::OnLg()
{
    document->InsertFunction("lg", true);
}

void MainWindow::OnFences()
{
    document->InsertFences(true);
}

void MainWindow::OnLog()
{
    document->InsertSubscriptFunction("log", true);
}

void MainWindow::OnInt()
{
    document->InsertFunction("int", true);
}

void MainWindow::OnFract()
{
    document->InsertFunction("fract", true);
}

void MainWindow::OnRound()
{
    document->InsertFunction("round", true);
}

void MainWindow::OnEquation()
{
    document->InsertEquation(ResultType::AUTO, true);
}

void MainWindow::OnAssignment()
{
    document->InsertAssignment(true);
}

void MainWindow::OnSin()
{
    document->InsertFunction("sin", true);
}

void MainWindow::OnCos()
{
    document->InsertFunction("cos", true);
}

void MainWindow::OnTg()
{
    document->InsertFunction("tg", true);
}

void MainWindow::OnCtg()
{
    document->InsertFunction("ctg", true);
}

void MainWindow::OnSec()
{
    document->InsertFunction("sec", true);
}

void MainWindow::OnCsc()
{
    document->InsertFunction("csc", true);
}

void MainWindow::OnArcsin()
{
    document->InsertFunction("arcsin", true);
}

void MainWindow::OnArccos()
{
    document->InsertFunction("arccos", true);
}

void MainWindow::OnArctg()
{
    document->InsertFunction("arctg", true);
}

void MainWindow::OnArcctg()
{
    document->InsertFunction("arcctg", true);
}

void MainWindow::OnArcsec()
{
    document->InsertFunction("arcsec", true);
}

void MainWindow::OnArccsc()
{
    document->InsertFunction("arccsc", true);
}

void MainWindow::OnSinh()
{
    document->InsertFunction("sinh", true);
}

void MainWindow::OnCosh()
{
    document->InsertFunction("cosh", true);
}

void MainWindow::OnTgh()
{
    document->InsertFunction("tgh", true);
}

void MainWindow::OnCtgh()
{
    document->InsertFunction("ctgh", true);
}

void MainWindow::OnSech()
{
    document->InsertFunction("sech", true);
}

void MainWindow::OnCsch()
{
    document->InsertFunction("csch", true);
}

void MainWindow::OnArsinh()
{
    document->InsertFunction("arsinh", true);
}

void MainWindow::OnArcosh()
{
    document->InsertFunction("arcosh", true);
}

void MainWindow::OnArtgh()
{
    document->InsertFunction("artgh", true);
}

void MainWindow::OnArctgh()
{
    document->InsertFunction("arctgh", true);
}

void MainWindow::OnArsech()
{
    document->InsertFunction("arsech", true);
}

void MainWindow::OnArcsch()
{
    document->InsertFunction("arcsch", true);
}

void MainWindow::OnCaretMoved(const EditorState editor_state)
{
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
    auto t = document->GetElementType(c.GetElement());
    if (!document->IsString(document->GetElement(c.GetElement())) && !document->IsRow(document->GetElement(c.GetElement())))
    {
        format.Reset();
    }
    else if (document->GetStringFormat(c.GetElement(), format))
    {
        for (auto& state : s.state)
        {
            StringFormat f;
            document->GetStringFormat(state.id, f);
            if (format.family != "" && format.family != f.family)
                format.family = "";
            if (format.size != 0 && format.size != f.size)
                format.size = 0;
            if (format.bold != false && format.bold != f.bold)
                format.bold = false;
            if (format.italic != false && format.italic != f.italic)
                format.italic = false;
            if (format.underline != false && format.underline != f.underline)
                format.underline = false;
        }
    }

    //update the interface elements
    paragraph_format_combo->setCurrentText(paragraph_format.name.c_str());
    family_combo->setCurrentText(format.family.c_str());
    if (format.size == 0)
        size_combo->setCurrentText("");
    else
        size_combo->setCurrentText(std::to_string(format.size).c_str());
    bold_action->setChecked(format.bold);
    italic_action->setChecked(format.italic);
    underline_action->setChecked(format.underline);

    undo_action->setEnabled(document->CanUndo());
    redo_action->setEnabled(document->CanRedo());
}

void MainWindow::OnSaveResult(const uint task_id, IOResult result)
{
    if (result != IOResult::Success)
        QMessageBox::critical(this, tr("Yutovo"), tr("Error saving document"));
}

void MainWindow::OnLoadResult(const uint task_id, IOResult result)
{
    if (result != IOResult::Success)
        QMessageBox::critical(this, tr("Yutovo"), tr("Error loading document"));
}

void MainWindow::OnClipboardCopyResult(CopyResult result)
{
    if (result != CopyResult::Success)
        return;
    QClipboard* clipboard = QGuiApplication::clipboard();
    QMimeData* mime_data = new QMimeData;
    std::string s = clipboard_array.str();
    QByteArray item_data(s.c_str(), s.size());
    mime_data->setData("yutovo/elements", item_data); //custom clipboard type
    mime_data->setText(clipboard_text.c_str());
    clipboard->setMimeData(mime_data);
    clipboard_array.clear();
    clipboard_text = "";
}

void MainWindow::OnDocumentUpdated(const Rect rect)
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

void MainWindow::FillParagraphFormats()
{
    std::vector<ParagraphFormatPtr> formats;
    document->paragraph_formats->GetFormats(formats);
    for (auto& f : formats)
    {
        paragraph_format_combo->addItem(f->name.c_str());
    }
}

void MainWindow::FillSizes(const QFont& font)
{
    QFontDatabase font_database;
    QString current_size = size_combo->currentText();

    {
        const QSignalBlocker blocker(size_combo);
        // sizeCombo signals are now blocked until end of scope
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

    if (i == -1)
        size_combo->setCurrentIndex(qMax(0, size_combo->count() / 3));
    else
        size_combo->setCurrentIndex(i);
}
