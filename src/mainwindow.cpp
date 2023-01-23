#include "mainwindow.h"
#include <QMenu>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QMimeData>
#include <QScrollBar>
#include <yutovo_editor/util.h>

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

    document->Start();
}

void MainWindow::CreateActions()
{
    //file menu and toolbar
    QMenu *file_menu = menuBar()->addMenu(tr("&File"));
    QToolBar *file_toolbar = addToolBar(tr("File"));

    QAction* action = new QAction(QIcon(":/icons/images/new.png"), tr("&New"), this);
    action->setShortcuts(QKeySequence::New);
    action->setStatusTip(tr("Create a new document"));
    connect(action, &QAction::triggered, this, &MainWindow::New);
    file_menu->addAction(action);
    file_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/open.png"), tr("&Open..."), this);
    action->setShortcuts(QKeySequence::Open);
    action->setStatusTip(tr("Open an existing file"));
    connect(action, &QAction::triggered, this, &MainWindow::Open);
    file_menu->addAction(action);
    file_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/save.png"), tr("&Save"), this);
    action->setShortcuts(QKeySequence::Save);
    action->setStatusTip(tr("Save the document to disk"));
    connect(action, &QAction::triggered, this, &MainWindow::Save);
    file_menu->addAction(action);
    file_toolbar->addAction(action);

    action = file_menu->addAction(QIcon(":/images/new.png"), tr("Save &As..."), this, &MainWindow::SaveAs);
    action->setShortcuts(QKeySequence::SaveAs);
    action->setStatusTip(tr("Save the document under a new name"));

    file_menu->addSeparator();

    action = file_menu->addAction(QIcon::fromTheme("application-exit"), tr("E&xit"), this, &MainWindow::Exit);
    action->setShortcuts(QKeySequence::Quit);
    action->setStatusTip(tr("Exit the application"));

    //edit menu and toolbar
    QMenu *edit_menu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *edit_toolbar = addToolBar(tr("Edit"));

    action = new QAction(QIcon(":/icons/images/formula.png"), tr("&Code"), this);
    action->setStatusTip(tr("Insert code"));
    connect(action, &QAction::triggered, this, &MainWindow::OnInsertCode);
    edit_menu->addAction(action);
    edit_toolbar->addAction(action);

    edit_menu->addSeparator();
    edit_toolbar->addSeparator();

    undo_action = new QAction(QIcon(":/icons/images/undo.png"), tr("U&ndo"), this);
    undo_action->setShortcuts(QKeySequence::Undo);
    undo_action->setStatusTip(tr("Undo the last operation"));
    connect(undo_action, &QAction::triggered, this, &MainWindow::Undo);
    edit_menu->addAction(undo_action);
    edit_toolbar->addAction(undo_action);

    redo_action = new QAction(QIcon(":/icons/images/redo.png"), tr("&Redo"), this);
    redo_action->setShortcuts(QKeySequence::Redo);
    redo_action->setStatusTip(tr("Redo the last operation"));
    connect(redo_action, &QAction::triggered, this, &MainWindow::Redo);
    edit_menu->addAction(redo_action);
    edit_toolbar->addAction(redo_action);

    edit_menu->addSeparator();
    edit_toolbar->addSeparator();

    action = new QAction(QIcon(":/icons/images/cut.png"), tr("Cu&t"), this);
    action->setShortcuts(QKeySequence::Cut);
    action->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    connect(action, &QAction::triggered, this, &MainWindow::Cut);
    edit_menu->addAction(action);
    edit_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/copy.png"), tr("&Copy"), this);
    action->setShortcuts(QKeySequence::Copy);
    action->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
    connect(action, &QAction::triggered, this, &MainWindow::Copy);
    edit_menu->addAction(action);
    edit_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/paste.png"), tr("&Paste"), this);
    action->setShortcuts(QKeySequence::Paste);
    action->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
    connect(action, &QAction::triggered, this, &MainWindow::Paste);
    edit_menu->addAction(action);
    edit_toolbar->addAction(action);

    //fonts toolbar
    QToolBar *format_toolbat = addToolBar(tr("Format"));
    format_toolbat->setStyleSheet("QToolBar{spacing:4px;}");

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

    bold_action = new QAction(QIcon(":/icons/images/bold.png"), tr("Bold"), this);
    connect(bold_action, &QAction::triggered, this, &MainWindow::OnBold);
    bold_action->setCheckable(true);
    format_toolbat->addAction(bold_action);

    italic_action = new QAction(QIcon(":/icons/images/italic.png"), tr("Italic"), this);
    connect(italic_action, &QAction::triggered, this, &MainWindow::OnItalic);
    italic_action->setCheckable(true);
    format_toolbat->addAction(italic_action);

    underline_action = new QAction(QIcon(":/icons/images/underline.png"), tr("Underline"), this);
    connect(underline_action, &QAction::triggered, this, &MainWindow::OnUnderline);
    underline_action->setCheckable(true);
    format_toolbat->addAction(underline_action);

    //help menu
    QMenu* help_menu = menuBar()->addMenu(tr("&Help"));
    action = help_menu->addAction(tr("&About"), this, &MainWindow::About);
    help_menu->setStatusTip(tr("Show the application's About box"));
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
}

void MainWindow::OnVerticalValueChanged(int value)
{
    document_widget->window.document_point.y = value;
    document->Redraw();
}

void MainWindow::OnHorizontalValueChanged(int value)
{
    document_widget->window.document_point.x = value;
    document->Redraw();
}

void MainWindow::OnInsertCode()
{
    document->InsertCode(true);
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
            if (document->IsString(document->GetElement(state.id)))
            {
                format.Reset();
                break;
            }
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
    
    vertical_scroll->setMaximum(s.height - r.height);
    vertical_scroll->setPageStep(r.height);
    vertical_scroll->setValue(p.y);
    horizontal_scroll->setMaximum(s.width - r.width);
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
