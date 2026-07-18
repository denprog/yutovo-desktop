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
    void testExportToPdf();
    void testPdfExportErrorHandling();
    void testMenuRebuildDoesNotLeak();
    void testCopyPasteGraph();

private:
    MainWindow* window;
};
