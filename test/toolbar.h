#include <QtTest/QtTest>
#include <QApplication>
#include <QAction>
#include <QToolBar>
#include "../src/mainwindow.h"

class TestToolbar : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testCalculusToolbar();
    void testIndefiniteIntegral();
    void testDerivative();
    void testSecondDerivative();
    void testPartialDerivative();

    void testTextBlock();
    void testTextBlockNoCodeBlock();
    void testTextBlockInText();

private:
    MainWindow* window;
};
