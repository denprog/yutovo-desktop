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
#include <QListWidget>
#include <QFileInfo>
#include <QSettings>
#include <QComboBox>
#include <QToolBar>
#include "../src/document_widget.h"
#include "../src/document_window.h"
#include "../src/prompt_form.h"

//TestFiles

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

QMessageBox* TestFiles::FindMessageBox()
{
    for (auto w : QApplication::topLevelWidgets())
    {
        auto box = qobject_cast<QMessageBox*>(w);
        if (box)
            return box;
    }
    return nullptr;
}

QFileDialog* TestFiles::FindFileDialog()
{
    for (auto w : QApplication::topLevelWidgets())
    {
        auto dialog = qobject_cast<QFileDialog*>(w);
        if (dialog)
            return dialog;
    }
    return nullptr;
}

void TestFiles::ClickMessageBoxButton(QMessageBox::StandardButton button)
{
    //wait for the box to appear: it may be created by an asynchronous save completion
    QTRY_VERIFY(FindMessageBox() != nullptr);
    QMessageBox* box = FindMessageBox();
    auto b = box->button(button);
    QVERIFY(b);
    b->click();
}

void TestFiles::AcceptFileDialogWithPath(const QString& path)
{
    QTRY_VERIFY(FindFileDialog() != nullptr);
    QFileDialog* dialog = FindFileDialog();
    QTRY_VERIFY(dialog->isVisible());

    auto line_edit = dialog->findChild<QLineEdit*>("fileNameEdit");
    QVERIFY(line_edit);
    line_edit->setFocus();
    line_edit->setText(path);
    QTest::qWait(200);
    QTest::keyClick(line_edit, Qt::Key_Return);
    QTest::qWait(200);
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

void TestFiles::testDefiniteIntegralPrompt()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);

    auto* prompt = editor->findChild<PromptForm*>();
    QVERIFY(prompt);

    QTest::qWait(2000);
    QTest::keyClicks(editor, "definite_integral");

    QTRY_VERIFY(prompt->isVisible());
    QTRY_VERIFY(prompt->count() > 0);

    QListWidgetItem* integral_item = nullptr;
    for (int i = 0; i < prompt->count(); ++i)
    {
        if (!prompt->item(i)->icon().isNull())
        {
            integral_item = prompt->item(i);
            break;
        }
    }
    QVERIFY(integral_item);

    QRect r = prompt->visualItemRect(integral_item);
    QTest::mouseClick(prompt->viewport(), Qt::LeftButton, Qt::NoModifier, r.center());
    QTest::qWait(200);

    QTRY_VERIFY(!prompt->isVisible());

    auto document = window->GetCurrentDocument();
    QVERIFY(document);
    QCOMPARE(document->ToText(), U"definite_integral(,,,)");
}

void TestFiles::testIntegralPrompt()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);

    auto* prompt = editor->findChild<PromptForm*>();
    QVERIFY(prompt);

    QTest::qWait(2000);
    QTest::keyClicks(editor, "indefinite_integral");

    QTRY_VERIFY(prompt->isVisible());
    QTRY_VERIFY(prompt->count() > 0);

    QListWidgetItem* integral_item = nullptr;
    for (int i = 0; i < prompt->count(); ++i)
    {
        auto* it = prompt->item(i);
        if (it->data(Qt::UserRole).toString() == "indefinite_integral")
        {
            integral_item = it;
            break;
        }
    }
    QVERIFY(integral_item);

    QRect r = prompt->visualItemRect(integral_item);
    QTest::mouseClick(prompt->viewport(), Qt::LeftButton, Qt::NoModifier, r.center());
    QTest::qWait(200);

    QTRY_VERIFY(!prompt->isVisible());

    auto document = window->GetCurrentDocument();
    QVERIFY(document);
    QCOMPARE(document->ToText(), U"indefinite_integral(,)");
}

void TestFiles::testIntegralPartialPrompt()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);

    auto* prompt = editor->findChild<PromptForm*>();
    QVERIFY(prompt);

    QTest::qWait(2000);
    QTest::keyClicks(editor, "indefinite");

    QTRY_VERIFY(prompt->isVisible());
    QTRY_VERIFY(prompt->count() > 0);

    QListWidgetItem* integral_item = nullptr;
    for (int i = 0; i < prompt->count(); ++i)
    {
        if (!prompt->item(i)->icon().isNull())
        {
            integral_item = prompt->item(i);
            break;
        }
    }
    QVERIFY(integral_item);
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

void TestFiles::testCancelSaveAsOnExit()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);

    QTest::keyClicks(editor, "abc");
    QTest::qWait(200);

    const int tabs_before = window->ui->editor_tabs->count();
    QVERIFY(tabs_before > 0);

    //closing the window with an unsaved pathless document: "Save?" -> Yes opens "Save file as",
    //which is then cancelled; the pending exit intent must be dropped
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
                    dialog->reject();
                });
        });

    window->close();
    QTest::qWait(500);

    //the exit is abandoned: the intent is cleared and the application keeps running
    QVERIFY(window->isVisible());
    QCOMPARE(window->exit_after_save, false);
    QVERIFY(window->close_tab_after_save == nullptr);
    QCOMPARE(window->ui->editor_tabs->count(), tabs_before);

    //a later ordinary save must not close the tab or silently quit the application
    QString path = QDir::tempPath() + "/test_cancel_save_as_on_exit.yut";
    QFile::remove(path);

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

    auto action = window->findChild<QAction*>("actionSave");
    QVERIFY(action);
    action->trigger();

    QTRY_VERIFY(QFile::exists(path));
    QTest::qWait(500);

    QVERIFY(window->isVisible());
    QCOMPARE(window->ui->editor_tabs->count(), tabs_before);
}

void TestFiles::testCancelSaveAsOnTabClose()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);

    QTest::keyClicks(editor, "abc");
    QTest::qWait(200);

    const int tabs_before = window->ui->editor_tabs->count();
    QVERIFY(tabs_before > 0);

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
                    dialog->reject();
                });
        });

    //closing the tab with an unsaved pathless document: "Save?" -> Yes opens "Save file as",
    //which is then cancelled; the pending tab-close intent must be dropped
    emit window->ui->editor_tabs->tabCloseRequested(0);
    QTest::qWait(500);

    QVERIFY(window->close_tab_after_save == nullptr);
    QCOMPARE(window->exit_after_save, false);
    QCOMPARE(window->ui->editor_tabs->count(), tabs_before);
    QVERIFY(window->GetCurrentDocument() != nullptr);
}

void TestFiles::testCloseOnExitAnswerNo()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);
    QTest::keyClicks(editor, "123");
    QTest::qWait(200);

    QTimer::singleShot(0,
        [&]()
        {
            ClickMessageBoxButton(QMessageBox::No);
        });

    window->close();
    QTest::qWait(200);

    //No: the changes are discarded, no file dialog appears and the application quits
    QVERIFY(!window->isVisible());
    QCOMPARE(window->exit_after_save, false);
    QVERIFY(window->close_tab_after_save == nullptr);
}

void TestFiles::testCloseOnExitAnswerCancel()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);
    QTest::keyClicks(editor, "123");
    QTest::qWait(200);

    const int tabs_before = window->ui->editor_tabs->count();

    QTimer::singleShot(0,
        [&]()
        {
            ClickMessageBoxButton(QMessageBox::Cancel);
        });

    window->close();
    QTest::qWait(200);

    //Cancel: the application keeps running and the document stays unsaved
    QVERIFY(window->isVisible());
    QCOMPARE(window->ui->editor_tabs->count(), tabs_before);
    auto document = window->GetCurrentDocument();
    QVERIFY(document);
    QVERIFY(document->IsChanged());
    QCOMPARE(window->exit_after_save, false);
    QVERIFY(window->close_tab_after_save == nullptr);
}

void TestFiles::testCloseUnchangedDocumentNoDialog()
{
    //a clean document must close immediately without any dialog
    QTimer::singleShot(3000,
        [&]()
        {
            //watchdog: if a dialog unexpectedly appears, dismiss it so the test fails instead of hanging
            QMessageBox* box = FindMessageBox();
            if (box)
            {
                auto cancel = box->button(QMessageBox::Cancel);
                if (cancel)
                    cancel->click();
            }
        });

    window->close();

    QVERIFY(!window->isVisible());
    QCOMPARE(window->exit_after_save, false);
    QVERIFY(window->close_tab_after_save == nullptr);
}

void TestFiles::testCloseTabAnswerNo()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);
    QTest::keyClicks(editor, "123");
    QTest::qWait(200);

    const int tabs_before = window->ui->editor_tabs->count();

    QTimer::singleShot(0,
        [&]()
        {
            ClickMessageBoxButton(QMessageBox::No);
        });

    emit window->ui->editor_tabs->tabCloseRequested(0);
    QTest::qWait(200);

    //No: the tab is closed without saving, the application keeps running
    QCOMPARE(window->ui->editor_tabs->count(), tabs_before - 1);
    QVERIFY(window->isVisible());
    QCOMPARE(window->exit_after_save, false);
    QVERIFY(window->close_tab_after_save == nullptr);
}

void TestFiles::testCloseTabAnswerCancel()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);
    QTest::keyClicks(editor, "123");
    QTest::qWait(200);

    const int tabs_before = window->ui->editor_tabs->count();

    QTimer::singleShot(0,
        [&]()
        {
            ClickMessageBoxButton(QMessageBox::Cancel);
        });

    emit window->ui->editor_tabs->tabCloseRequested(0);
    QTest::qWait(200);

    //Cancel: the tab stays open with the unsaved document
    QCOMPARE(window->ui->editor_tabs->count(), tabs_before);
    auto document = window->GetCurrentDocument();
    QVERIFY(document);
    QVERIFY(document->IsChanged());
    QVERIFY(window->isVisible());
}

void TestFiles::testCloseOnExitSaveAsConfirmed()
{
    QString path = QDir::tempPath() + "/test_close_exit_saveas_confirmed.yut";
    QFile::remove(path);

    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);
    QTest::keyClicks(editor, "123");
    QTest::qWait(200);

    QTimer::singleShot(0,
        [&]()
        {
            ClickMessageBoxButton(QMessageBox::Yes);
            QTimer::singleShot(0,
                [&]()
                {
                    AcceptFileDialogWithPath(path);
                });
        });

    window->close();

    //Yes + confirmed path: the document is saved and the application quits
    QTRY_VERIFY(QFile::exists(path));
    QTRY_VERIFY(!window->isVisible());
    QCOMPARE(window->exit_after_save, false);
    QVERIFY(window->close_tab_after_save == nullptr);
}

void TestFiles::testCloseTabSaveAsConfirmedOtherTabsRemain()
{
    QString path = QDir::tempPath() + "/test_close_tab_saveas_confirmed.yut";
    QFile::remove(path);

    //first tab: unsaved document without a path
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);
    QTest::keyClicks(editor, "111");
    QTest::qWait(200);

    //second tab
    auto new_action = window->findChild<QAction*>("actionNew");
    QVERIFY(new_action);
    new_action->trigger();
    QTest::qWait(200);

    DocumentWindow* second_tab = qobject_cast<DocumentWindow*>(window->ui->editor_tabs->widget(1));
    QVERIFY(second_tab);
    auto second_editor = second_tab->findChild<DocumentWidget*>();
    QVERIFY(second_editor);
    QTest::keyClicks(second_editor, "222");
    QTest::qWait(200);

    QCOMPARE(window->ui->editor_tabs->count(), 2);

    QTimer::singleShot(0,
        [&]()
        {
            ClickMessageBoxButton(QMessageBox::Yes);
            QTimer::singleShot(0,
                [&]()
                {
                    AcceptFileDialogWithPath(path);
                });
        });

    emit window->ui->editor_tabs->tabCloseRequested(0);

    //the first tab is saved and closed; the second tab must survive and the application must keep running
    QTRY_VERIFY(QFile::exists(path));
    QTest::qWait(500);

    QCOMPARE(window->ui->editor_tabs->count(), 1);
    QVERIFY(window->ui->editor_tabs->widget(0) == second_tab);
    QVERIFY(window->isVisible());
    QCOMPARE(window->exit_after_save, false);
    QVERIFY(window->close_tab_after_save == nullptr);

    auto document = window->GetCurrentDocument();
    QVERIFY(document);
    QCOMPARE(document->ToText(), U"222");
}

void TestFiles::testExitWithTwoUnsavedTabs()
{
    QString path = QDir::tempPath() + "/test_exit_two_tabs.yut";
    QFile::remove(path);

    //first tab: unsaved document without a path
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);
    QTest::keyClicks(editor, "111");
    QTest::qWait(200);

    //second tab: also unsaved
    auto new_action = window->findChild<QAction*>("actionNew");
    QVERIFY(new_action);
    new_action->trigger();
    QTest::qWait(200);
    auto second_tab = qobject_cast<DocumentWindow*>(window->ui->editor_tabs->widget(1));
    QVERIFY(second_tab);
    auto second_editor = second_tab->findChild<DocumentWidget*>();
    QVERIFY(second_editor);
    QTest::keyClicks(second_editor, "222");
    QTest::qWait(200);

    QCOMPARE(window->ui->editor_tabs->count(), 2);

    //the second tab is asked only after the first tab's asynchronous save completes,
    //so it is answered by a polling timer instead of a chained single shot
    QTimer second_box_timer;
    second_box_timer.setInterval(100);
    QObject::connect(&second_box_timer, &QTimer::timeout,
        [&]()
        {
            QMessageBox* box = FindMessageBox();
            if (box && box->isVisible())
            {
                auto no = box->button(QMessageBox::No);
                if (no)
                {
                    no->click();
                    second_box_timer.stop();
                }
            }
        });

    //closing the window asks for each unsaved tab: save the first one, discard the second one
    QTimer::singleShot(0,
        [&]()
        {
            ClickMessageBoxButton(QMessageBox::Yes);
            QTimer::singleShot(0,
                [&]()
                {
                    AcceptFileDialogWithPath(path);
                    second_box_timer.start();
                });
        });

    window->close();

    QTRY_VERIFY(QFile::exists(path));
    QTRY_VERIFY(!window->isVisible());
    QCOMPARE(window->exit_after_save, false);
    QVERIFY(window->close_tab_after_save == nullptr);
    second_box_timer.stop();
}

void TestFiles::testExitAfterCancelledSaveStillWorks()
{
    QString path = QDir::tempPath() + "/test_exit_after_cancelled_save.yut";
    QFile::remove(path);

    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);
    QTest::keyClicks(editor, "123");
    QTest::qWait(200);

    //first exit attempt: answer Yes but cancel the "Save file as" dialog
    QTimer::singleShot(0,
        [&]()
        {
            ClickMessageBoxButton(QMessageBox::Yes);
            QTimer::singleShot(0,
                [&]()
                {
                    QTRY_VERIFY(FindFileDialog() != nullptr);
                    FindFileDialog()->reject();
                });
        });

    window->close();
    QTest::qWait(300);

    QVERIFY(window->isVisible());
    QCOMPARE(window->exit_after_save, false);
    QVERIFY(window->close_tab_after_save == nullptr);

    //the second exit attempt after the cancelled one must work normally
    QTimer::singleShot(0,
        [&]()
        {
            ClickMessageBoxButton(QMessageBox::Yes);
            QTimer::singleShot(0,
                [&]()
                {
                    AcceptFileDialogWithPath(path);
                });
        });

    window->close();

    QTRY_VERIFY(QFile::exists(path));
    QTRY_VERIFY(!window->isVisible());
    QCOMPARE(window->exit_after_save, false);
    QVERIFY(window->close_tab_after_save == nullptr);
}

void TestFiles::testSaveAllWithSaveAsDialog()
{
    QString path0 = QDir::tempPath() + "/test_save_all_existing.yut";
    QString path1 = QDir::tempPath() + "/test_save_all_dialog.yut";
    QFile::remove(path0);
    QFile::remove(path1);

    //first tab: give it a path with an initial "Save as"
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);
    QTest::keyClicks(editor, "111");
    QTest::qWait(200);

    QTimer::singleShot(0,
        [&]()
        {
            AcceptFileDialogWithPath(path0);
        });

    window->SaveFileAsName();
    QTRY_VERIFY(QFile::exists(path0));
    QTest::qWait(300);

    //second tab: unsaved document without a path
    auto new_action = window->findChild<QAction*>("actionNew");
    QVERIFY(new_action);
    new_action->trigger();
    QTest::qWait(200);
    auto second_tab = qobject_cast<DocumentWindow*>(window->ui->editor_tabs->widget(1));
    QVERIFY(second_tab);
    auto second_editor = second_tab->findChild<DocumentWidget*>();
    QVERIFY(second_editor);
    QTest::keyClicks(second_editor, "222");
    QTest::qWait(200);

    QCOMPARE(window->ui->editor_tabs->count(), 2);

    //"Save All": only the pathless second tab opens the dialog, the first one is saved silently
    QTimer::singleShot(0,
        [&]()
        {
            AcceptFileDialogWithPath(path1);
        });

    window->SaveAll();

    QTRY_VERIFY(QFile::exists(path1));
    QTest::qWait(300);

    //Save All must not close tabs or quit the application
    QCOMPARE(window->ui->editor_tabs->count(), 2);
    QVERIFY(window->isVisible());
    QCOMPARE(window->exit_after_save, false);
    QVERIFY(window->close_tab_after_save == nullptr);
}

void TestFiles::testSaveAsOverwriteConfirmed()
{
    QString path = QDir::tempPath() + "/test_saveas_overwrite_confirmed.yut";
    QFile::remove(path);

    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);

    //initial save: the file does not exist yet, no confirmation is expected
    QTest::keyClicks(editor, "111");
    QTest::qWait(200);

    QTimer::singleShot(0,
        [&]()
        {
            AcceptFileDialogWithPath(path);
        });

    window->SaveFileAsName();
    QTRY_VERIFY(QFile::exists(path));
    QTRY_VERIFY(!window->GetCurrentDocument()->IsChanged());

    //save under the same name again: the own overwrite confirmation must appear with No as the default button
    QTest::keyClicks(editor, "222");
    QTest::qWait(200);

    QTimer::singleShot(0,
        [&]()
        {
            AcceptFileDialogWithPath(path);
            QTimer::singleShot(0,
                [&]()
                {
                    QMessageBox* box = FindMessageBox();
                    QTRY_VERIFY(box != nullptr);
                    QVERIFY(box->defaultButton() == box->button(QMessageBox::No));
                    ClickMessageBoxButton(QMessageBox::Yes);
                });
        });

    window->SaveFileAsName();

    //wait for the asynchronous overwrite to finish before reopening the document
    QTRY_VERIFY(!window->GetCurrentDocument()->IsChanged());
    QTest::qWait(200);

    auto close_action = window->findChild<QAction*>("actionClose");
    QVERIFY(close_action);
    close_action->trigger();
    QTest::qWait(200);

    auto new_action = window->findChild<QAction*>("actionNew");
    QVERIFY(new_action);
    new_action->trigger();
    QTest::qWait(200);
    QTRY_VERIFY(window->GetCurrentDocument()->ToText() == U"");

    //the reopened document must contain the overwritten content
    QTimer::singleShot(0,
        [&]()
        {
            AcceptFileDialogWithPath(path);
        });

    auto open_action = window->findChild<QAction*>("actionOpen");
    QVERIFY(open_action);
    open_action->trigger();

    QTRY_VERIFY(window->GetCurrentDocument()->ToText() == U"111222");
}

void TestFiles::testSaveAsOverwriteDeclinedReopensDialog()
{
    QString path = QDir::tempPath() + "/test_saveas_overwrite_declined.yut";
    QFile::remove(path);

    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);

    QTest::keyClicks(editor, "111");
    QTest::qWait(200);

    QTimer::singleShot(0,
        [&]()
        {
            AcceptFileDialogWithPath(path);
        });

    window->SaveFileAsName();
    QTRY_VERIFY(QFile::exists(path));
    QTRY_VERIFY(!window->GetCurrentDocument()->IsChanged());

    QTest::keyClicks(editor, "222");
    QTest::qWait(200);

    QTimer::singleShot(0,
        [&]()
        {
            AcceptFileDialogWithPath(path);
            QTimer::singleShot(0,
                [&]()
                {
                    ClickMessageBoxButton(QMessageBox::No);
                    QTimer::singleShot(0,
                        [&]()
                        {
                            //declining the overwrite must reopen the save dialog: cancel it with Escape
                            QFileDialog* dialog = FindFileDialog();
                            QTRY_VERIFY(dialog != nullptr);
                            QTRY_VERIFY(dialog->isVisible());
                            QTest::keyClick(dialog, Qt::Key_Escape);
                        });
                });
        });

    window->SaveFileAsName();

    //the save is cancelled: the tab survives with unsaved changes and no close/exit intent is pending
    QCOMPARE(window->ui->editor_tabs->count(), 1);
    QVERIFY(window->isVisible());
    QCOMPARE(window->exit_after_save, false);
    QVERIFY(window->close_tab_after_save == nullptr);
    QTRY_VERIFY(window->GetCurrentDocument()->IsChanged());

    //the tab still owns the file path, and OpenFile would just switch to it instead of
    //loading the file from disk; discard the tab (answer No to "Save?") before reopening
    QTimer::singleShot(0,
        [&]()
        {
            ClickMessageBoxButton(QMessageBox::No);
        });

    auto close_action = window->findChild<QAction*>("actionClose");
    QVERIFY(close_action);
    close_action->trigger();
    QTest::qWait(200);
    QCOMPARE(window->ui->editor_tabs->count(), 0);

    //the file on disk must keep the original content: reopen it in a new tab
    QTimer::singleShot(0,
        [&]()
        {
            AcceptFileDialogWithPath(path);
        });

    auto open_action = window->findChild<QAction*>("actionOpen");
    QVERIFY(open_action);
    open_action->trigger();

    QTRY_VERIFY(window->GetCurrentDocument()->ToText() == U"111");
    //the discarded tab is gone, only the reopened one remains
    QCOMPARE(window->ui->editor_tabs->count(), 1);
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

void TestFiles::testExportToPdfDefaultPath()
{
    //open a document from a known directory, then check the export dialog pre-fills
    //that directory and the document name with the .pdf extension
    QString path = QFileInfo(__FILE__).dir().filePath("tests/wrong_paragraph.yut");
    QVERIFY(QFile::exists(path));

    window->OpenFile(path);

    auto document_window = qobject_cast<DocumentWindow*>(window->ui->editor_tabs->currentWidget());
    QVERIFY(document_window);

    QSignalSpy load_spy(document_window, &DocumentWindow::LoadResult);
    QVERIFY(load_spy.wait(5000));

    QString expected = QFileInfo(path).dir().filePath("wrong_paragraph.pdf");

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
            QCOMPARE(line_edit->text(), expected);
            dialog->reject();
        });

    window->ExportToPdf();

    QTest::qWait(200);
    QVERIFY(!QFile::exists(expected));
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

void TestFiles::testCopyWrongParagraph()
{
    auto editor = window->findChild<DocumentWidget*>();
    QVERIFY(editor);

    QString path = QFileInfo(__FILE__).dir().filePath("tests/wrong_paragraph.yut");
    QVERIFY(QFile::exists(path));

    window->OpenFile(path);

    auto document_window = qobject_cast<DocumentWindow*>(window->ui->editor_tabs->currentWidget());
    QVERIFY(document_window);

    QSignalSpy load_spy(document_window, &DocumentWindow::LoadResult);
    QVERIFY(load_spy.wait(5000));
    QCOMPARE(load_spy.count(), 1);

    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    //OpenFile may replace the current editor tab, so re-find the widget
    editor = document_window->findChild<DocumentWidget*>("document_widget");
    QVERIFY(editor);

    editor->setFocus();
    QTest::qWait(500);

    //move to the beginning of the document and select the first line
    QTest::keyClick(editor, Qt::Key_Home, Qt::ControlModifier);
    QTest::qWait(200);
    QTest::keyClick(editor, Qt::Key_End, Qt::ShiftModifier);
    QTest::qWait(200);

    //copy the selection to the clipboard; this used to crash on wrong_paragraph.yut
    QSignalSpy copy_spy(document_window, &DocumentWindow::ClipboardCopyResult);
    window->Copy();
    QVERIFY(copy_spy.wait(5000));
}

void TestFiles::testUserInterfaceParagraphFormats()
{
    LanguageSettingGuard guard((int)yutovo_calculator::Language::English);

    //recreate the main window so the interface is initialized in English
    delete window;
    window = nullptr;
    window = new MainWindow();
    window->Start("");
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window));

    QString path = QFileInfo(__FILE__).dir().filePath("tests/User interface.yut"); //document with Russian paragraph names
    QVERIFY(QFile::exists(path));

    window->OpenFile(path);

    auto document_window = qobject_cast<DocumentWindow*>(window->ui->editor_tabs->currentWidget());
    QVERIFY(document_window);

    QSignalSpy load_spy(document_window, &DocumentWindow::LoadResult);
    QVERIFY(load_spy.wait(5000));
    QCOMPARE(load_spy.count(), 1);

    QTest::qWait(500);

    auto paragraph_format_combo = window->findChild<QComboBox*>("paragraph_format_combo");
    QVERIFY(paragraph_format_combo);

    QVERIFY2(paragraph_format_combo->findText("Основной текст") != -1, "Combo box does not contain 'Основной текст'");
    QVERIFY2(paragraph_format_combo->findText("Заголовок 1") != -1, "Combo box does not contain 'Заголовок 1'");
}
