#include "files.h"
#include <QLineEdit>
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

QTEST_MAIN(TestFiles)
