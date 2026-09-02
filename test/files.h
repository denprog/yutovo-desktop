#include <QtTest/QtTest>
#include <QApplication>
#include <QTextEdit>
#include <QAction>
#include <QMenuBar>
#include <QFileDialog>
#include <QSignalSpy>
#include <QTimer>
#include <QMessageBox>
#include <QPushButton>
#include "../src/mainwindow.h"

class TestFiles : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testNewDocument();
    void testChangeDocument();
    void testOpenDocument();
    void testPromptClick();
    void testPromptOutsideClick();
    void testDefiniteIntegralPrompt();
    void testIntegralPrompt();
    void testIntegralPartialPrompt();
    void testLogicalOperators();
    void testPowerShortcut();
    void testSaveAndCloseOnExit();
    void testCancelSaveAsOnExit();
    void testCancelSaveAsOnTabClose();
    void testCloseOnExitAnswerNo();
    void testCloseOnExitAnswerCancel();
    void testCloseUnchangedDocumentNoDialog();
    void testCloseTabAnswerNo();
    void testCloseTabAnswerCancel();
    void testCloseOnExitSaveAsConfirmed();
    void testCloseTabSaveAsConfirmedOtherTabsRemain();
    void testExitWithTwoUnsavedTabs();
    void testExitAfterCancelledSaveStillWorks();
    void testSaveAllWithSaveAsDialog();
    void testExportToPdf();
    void testPdfExportErrorHandling();
    void testMenuRebuildDoesNotLeak();
    void testCopyPasteGraph();
    void testCopyWrongParagraph();
    void testUserInterfaceParagraphFormats();

private:
    MainWindow* window;

    QMessageBox* FindMessageBox();
    QFileDialog* FindFileDialog();
    void ClickMessageBoxButton(QMessageBox::StandardButton button);
    void AcceptFileDialogWithPath(const QString& path);

    class LanguageSettingGuard
    {
    public:
        LanguageSettingGuard(int language)
        {
            settings = new QSettings("Yutovo", "Yutovo Desktop");
            old_value = settings->value("MainWindow/language");
            settings->setValue("MainWindow/language", language);
        }

        ~LanguageSettingGuard()
        {
            if (old_value.isValid())
                settings->setValue("MainWindow/language", old_value);
            else
                settings->remove("MainWindow/language");
            delete settings;
        }

    private:
        QSettings* settings;
        QVariant old_value;
    };
};
