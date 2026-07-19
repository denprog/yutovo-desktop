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

private:
    MainWindow* window;
};
