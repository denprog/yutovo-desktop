#include "files.h"
#include <QDebug>
#include <QLineEdit>
#include <QDialog>
#include <QProcess>
#include <QClipboard>
#include <QMimeData>
#include <QImage>
#include <QMenu>
#include <QContextMenuEvent>
#include <QPointer>
#include "../src/document_widget.h"
#include "../src/document_window.h"

void TestFiles::initTestCase()
{
}

void TestFiles::cleanupTestCase()
{
}

void TestFiles::init()
{
    QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
    window = new MainWindow();
    window->Start("");
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window));
}

void TestFiles::cleanup()
{
    delete window;
}

void TestFiles::testNewDocument()
{
    auto button = window->findChild<QAction*>("actionNew");
    QVERIFY(button);
    auto document = window->GetCurrentDocument();
    QVERIFY(document);
    QVERIFY(document->ToText() == U"");
}

void TestFiles::testChangeDocument()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);
    QTest::keyClicks(editor, "12345");
    auto document = window->GetCurrentDocument();
    QVERIFY(document);
    QTest::qWait(200);
    QCOMPARE(document->ToText(), U"12345");
}

void TestFiles::testOpenDocument()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);
    QTest::keyClicks(editor, "12345");
    auto document = window->GetCurrentDocument();
    QVERIFY(document);
    QTest::qWait(200);
    
    QString path = QDir::tempPath() + "/test_document.yut";
    QFile::remove(path);

    auto action = window->findChild<QAction*>("actionSave");
    QVERIFY(action);

    QTimer::singleShot(0, 
        [&]()
        {
            QFileDialog* dialog = nullptr;
            for (auto w : QApplication::topLevelWidgets())
            {
                dialog = qobject_cast<QFileDialog*>(w);
                if (dialog)
                    break;
            }

            QVERIFY(dialog);
            QTRY_VERIFY(dialog->isVisible());

            auto line_edit = dialog->findChild<QLineEdit*>("fileNameEdit");
            QVERIFY(line_edit);
            line_edit->setFocus();
            line_edit->setText(path);
            QTest::qWait(200);
            QTest::keyClick(line_edit, Qt::Key_Return);
            QTest::qWait(200);
        });

    action->trigger();

    QTRY_VERIFY(QFile::exists(path));
    QTest::qWait(200);

    action = window->findChild<QAction*>("actionClose");
    QVERIFY(action);
    action->trigger();
    QTest::qWait(200);

    action = window->findChild<QAction*>("actionNew");
    QVERIFY(action);
    action->trigger();
    QTest::qWait(200);

    QTRY_VERIFY(window->GetCurrentDocument()->ToText() == U"");    

    action = window->findChild<QAction*>("actionOpen");
    QVERIFY(action);

    QTimer::singleShot(0, 
        [&]()
        {
            QFileDialog* dialog = nullptr;
            for (auto w : QApplication::topLevelWidgets())
            {
                dialog = qobject_cast<QFileDialog*>(w);
                if (dialog)
                    break;
            }

            QVERIFY(dialog);

            dialog->setDirectory(QFileInfo(path).absolutePath());

            auto line_edit = dialog->findChild<QLineEdit*>();
            QVERIFY(line_edit);
            line_edit->setText(path);
            QTest::qWait(200);
            QTest::keyClick(line_edit, Qt::Key_Return);
            QTest::qWait(200);
        });

    action->trigger();
    QTRY_VERIFY(window->GetCurrentDocument()->ToText() == U"12345");
}

void TestFiles::testPromptClick()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);

    auto* prompt = editor->findChild<PromptForm*>();
    QVERIFY(prompt);

    QTest::qWait(2000);
    QTest::keyClicks(editor, "s");

    QTRY_VERIFY(prompt->isVisible());
    QTRY_VERIFY(prompt->count() > 0);

    QListWidgetItem* sin_item = nullptr;
    for (int i = 0; i < prompt->count(); ++i)
    {
        if (prompt->item(i)->text() == "sin")
        {
            sin_item = prompt->item(i);
            break;
        }
    }
    QVERIFY(sin_item);

    QRect r = prompt->visualItemRect(sin_item);
    QTest::mouseClick(prompt->viewport(), Qt::LeftButton, Qt::NoModifier, r.center());
    QTest::qWait(200);

    QTRY_VERIFY(!prompt->isVisible());

    auto document = window->GetCurrentDocument();
    QVERIFY(document);
    //qDebug() << "ToText:" << QString::fromUcs4(document->ToText().data(), document->ToText().size());
    QCOMPARE(document->ToText(), U"sin");
}

void TestFiles::testPromptOutsideClick()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);

    auto* prompt = editor->findChild<PromptForm*>();
    QVERIFY(prompt);

    QTest::qWait(2000);
    QTest::keyClicks(editor, "s");

    QTRY_VERIFY(prompt->isVisible());
    QTRY_VERIFY(prompt->count() > 0);

    QRect r = prompt->visualItemRect(prompt->item(0));
    QTest::mouseClick(prompt->viewport(), Qt::LeftButton, Qt::NoModifier, QPoint(r.left() - 50, r.top()));
    QTest::qWait(200);

    QTRY_VERIFY(!prompt->isVisible());
    auto document = window->GetCurrentDocument();
    QVERIFY(document);
    QCOMPARE(document->ToText(), U"s");
}

void TestFiles::testLogicalOperators()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    auto and_action = window->findChild<QAction*>("actionLogicalAnd");
    QVERIFY(and_action);
    and_action->trigger();
    QTest::qWait(200);
    QCOMPARE(document->ToText(), U"∧");

    auto or_action = window->findChild<QAction*>("actionLogicalOr");
    QVERIFY(or_action);
    or_action->trigger();
    QTest::qWait(200);
    QCOMPARE(document->ToText(), U"∧∨");

    auto xor_action = window->findChild<QAction*>("actionLogicalXor");
    QVERIFY(xor_action);
    xor_action->trigger();
    QTest::qWait(200);
    QCOMPARE(document->ToText(), U"∧∨⊕");

    auto not_action = window->findChild<QAction*>("actionLogicalNot");
    QVERIFY(not_action);
    not_action->trigger();
    QTest::qWait(200);
    QCOMPARE(document->ToText(), U"∧∨⊕¬");
}

void TestFiles::testPowerShortcut()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);

    QTest::keyClicks(editor, "2^3");
    QTest::qWait(500);

    auto document = window->GetCurrentDocument();
    QVERIFY(document);
    auto text = document->ToText();
    QVERIFY(text.find(U"pow(2,3)") != std::u32string::npos);
}

void TestFiles::testSaveAndCloseOnExit()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);

    QString path = QDir::tempPath() + "/test_save_and_close.yut";
    QFile::remove(path);

    QTest::keyClicks(editor, "abc");
    QTest::qWait(200);

    QTimer::singleShot(0,
        [&]()
        {
            QFileDialog* dialog = nullptr;
            for (auto w : QApplication::topLevelWidgets())
            {
                dialog = qobject_cast<QFileDialog*>(w);
                if (dialog)
                    break;
            }

            QVERIFY(dialog);
            QTRY_VERIFY(dialog->isVisible());

            auto line_edit = dialog->findChild<QLineEdit*>("fileNameEdit");
            QVERIFY(line_edit);
            line_edit->setFocus();
            line_edit->setText(path);
            QTest::qWait(200);
            QTest::keyClick(line_edit, Qt::Key_Return);
        });

    window->SaveFileAsName();
    QTRY_VERIFY(QFile::exists(path));
    QTest::qWait(500);

    QTest::keyClicks(editor, "changed");
    QTest::qWait(200);

    QTimer::singleShot(0,
        [&]()
        {
            QMessageBox* box = nullptr;
            for (auto w : QApplication::topLevelWidgets())
            {
                box = qobject_cast<QMessageBox*>(w);
                if (box)
                    break;
            }

            QVERIFY(box);
            auto yes = box->button(QMessageBox::Yes);
            QVERIFY(yes);
            yes->click();
        });

    //closing the window with unsaved changes triggers "Save?" -> Yes -> async save -> tab close
    //the fix copies path before OnCloseEditorTab, so this must not crash on a dangling DocumentWindow*
    window->close();
    QTest::qWait(1500);

    QVERIFY(QFile::exists(path));
}

void TestFiles::testExportToPdf()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);

    QTest::keyClicks(editor, "2+2");
    QTest::qWait(200);

    QString path = QDir::tempPath() + "/test_export.pdf";
    QFile::remove(path);

    QTimer::singleShot(0,
        [&]()
        {
            QDialog* dialog = nullptr;
            for (auto w : QApplication::topLevelWidgets())
            {
                dialog = qobject_cast<QDialog*>(w);
                if (dialog && dialog->windowTitle().contains("PDF"))
                    break;
            }

            QVERIFY(dialog);
            QTRY_VERIFY(dialog->isVisible());

            auto line_edit = dialog->findChild<QLineEdit*>("filePath");
            QVERIFY(line_edit);
            line_edit->setText(path);
            QTest::qWait(200);
            dialog->accept();
        });

    window->ExportToPdf();

    QTRY_VERIFY(QFile::exists(path));
    QTest::qWait(500);

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray header = file.read(5);
    QCOMPARE(header, QByteArray("%PDF-"));
    QVERIFY(file.size() > 100);

    QProcess pdftotext;
    pdftotext.start("pdftotext", QStringList() << path << "-");
    QVERIFY(pdftotext.waitForFinished(5000));
    QCOMPARE(pdftotext.exitCode(), 0);
    QString pdf_text = QString::fromUtf8(pdftotext.readAllStandardOutput());
    QVERIFY(pdf_text.contains("2+2"));
    QVERIFY(pdf_text.contains("Yutovo"));
}

void TestFiles::testPdfExportErrorHandling()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);

    QTest::keyClicks(editor, "1");
    QTest::qWait(200);

    //path inside a non-existent directory -> ofstream cannot open it
    QString path = QDir::tempPath() + "/pdf_export_nonexistent_dir/test.pdf";
    QTimer::singleShot(0,
        [&]()
        {
            QDialog* dialog = nullptr;
            for (auto w : QApplication::topLevelWidgets())
            {
                dialog = qobject_cast<QDialog*>(w);
                if (dialog && dialog->windowTitle().contains("PDF"))
                    break;
            }

            QVERIFY(dialog);
            QTRY_VERIFY(dialog->isVisible());

            auto line_edit = dialog->findChild<QLineEdit*>("filePath");
            QVERIFY(line_edit);
            line_edit->setText(path);
            QTest::qWait(200);
            dialog->accept();
        });

    //poll for the error QMessageBox and dismiss it
    QTimer box_timer;
    box_timer.setInterval(100);
    QObject::connect(&box_timer, &QTimer::timeout,
        [&]()
        {
            for (auto w : QApplication::topLevelWidgets())
            {
                auto box = qobject_cast<QMessageBox*>(w);
                if (box)
                {
                    auto button = box->button(QMessageBox::Ok);
                    if (!button)
                        button = box->button(QMessageBox::Close);
                    if (button)
                        button->click();
                    box_timer.stop();
                    return;
                }
            }
        });
    box_timer.start();

    window->ExportToPdf();
    QTest::qWait(2500);
    box_timer.stop();

    QVERIFY(!QFile::exists(path));
}

void TestFiles::testMenuRebuildDoesNotLeak()
{
    const int c = window->menuBar()->actions().size();
    QVERIFY(c > 0);
    for (int i = 0; i < 5; ++i)
    {
        window->SetupGuiActions();
        QCOMPARE(window->menuBar()->actions().size(), c);
    }
}

void TestFiles::testCopyPasteGraph()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);
    auto document = window->GetCurrentDocument();
    QVERIFY(document);
    auto document_window = window->findChild<DocumentWindow*>();
    QVERIFY(document_window);

    auto clipboard = QGuiApplication::clipboard();
    clipboard->clear();

    document->WaitTask(document->InsertGraph(true), 5000);
    QTest::qWait(500);
    QTRY_VERIFY(document->FindByType(ElementId{0}, ElementType::GRAPH_LINE) != nullptr);

    auto graph = document->FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    QVERIFY(graph);

    auto copyGraphViaContextMenu = 
        [&](const char* scenario)
        {
            Rect current_r;
            QVERIFY2(document->GetElementRect(graph->id, current_r), scenario);

            QPoint doc_point = document_window->GetDocumentPoint();
            QPoint graph_point(current_r.left + 5 - doc_point.x(), current_r.top + current_r.height / 2 - doc_point.y());
            QPoint global_pos = editor->mapToGlobal(graph_point);
            QPoint window_pos = document_window->mapFromGlobal(global_pos);

            QPointer<QMenu> menu;
            std::atomic<bool> triggered{false};

            QTimer::singleShot(100,
                [&]()
                {
                    menu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
                    if (!menu)
                        return;
                    QAction* copy_action = nullptr;
                    for (auto* a : menu->actions())
                    {
                        if (a->text() == "Copy image")
                        {
                            copy_action = a;
                            break;
                        }
                    }
                    if (copy_action)
                    {
                        menu->setActiveAction(copy_action);
                        copy_action->trigger();
                        triggered = true;
                    }
                    else
                    {
                        menu->close();
                    }
                });

            QContextMenuEvent event(QContextMenuEvent::Mouse, window_pos, global_pos);
            QApplication::sendEvent(document_window, &event);
            QTest::qWait(200);

            QVERIFY2(triggered.load(), scenario);
        };

    //caret is at the end of normal text, outside the graph
    document->MoveCaretToDocumentEnd(false);
    QTest::keyClicks(editor, "abc");
    QTest::qWait(200);
    QVERIFY(document->FindCurrentParentByType(ElementType::GRAPH_LINE).empty());
    copyGraphViaContextMenu("scenario end-of-text");

    auto mime_data = clipboard->mimeData();
    QVERIFY(mime_data);
    QVERIFY(mime_data->hasImage() || mime_data->hasFormat("image/png"));
    QImage image = qvariant_cast<QImage>(mime_data->imageData());
    QVERIFY(!image.isNull());
    QVERIFY(image.width() > 0);
    QVERIFY(image.height() > 0);

    //caret is at the beginning of normal text, outside the graph
    document->MoveCaretHome(false);
    QVERIFY(document->FindCurrentParentByType(ElementType::GRAPH_LINE).empty());
    copyGraphViaContextMenu("scenario beginning-of-text");

    mime_data = clipboard->mimeData();
    QVERIFY(mime_data);
    QVERIFY(mime_data->hasImage() || mime_data->hasFormat("image/png"));
    image = qvariant_cast<QImage>(mime_data->imageData());
    QVERIFY(!image.isNull());
    QVERIFY(image.width() > 0);
    QVERIFY(image.height() > 0);
}

QTEST_MAIN(TestFiles)
