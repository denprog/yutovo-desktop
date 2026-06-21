#include "files.h"
#include <QDebug>
#include <QLineEdit>
#include <QDialog>
#include <QProcess>
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

QTEST_MAIN(TestFiles)
