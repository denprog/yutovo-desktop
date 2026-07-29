#include <QtTest/QtTest>
#include <QApplication>
#include <QSettings>
#include <QLocale>
#include <yutovo-calculator/math_helper.h>
#include "files.h"
#include "toolbar.h"

int main(int argc, char** argv)
{
    qunsetenv("LANGUAGE");
    qputenv("LANG", "en_US.UTF-8");
    QLocale::setDefault(QLocale(QLocale::English));

    QApplication app(argc, argv);
    app.setOrganizationName("Yutovo");
    app.setApplicationName("Yutovo Desktop");
    app.setAttribute(Qt::AA_Use96Dpi, true);

    //force English UI for all tests so action text comparisons are stable
    QSettings settings;
    settings.clear();
    settings.setValue("MainWindow/language", (int)yutovo_calculator::Language::English);
    //calculus toolbar must be visible for TestToolbar::testCalculusToolbar
    settings.setValue("MainWindow/calculus_toolbar", true);
    settings.sync();

    int result = 0;

    {
        TestFiles test_files;
        result |= QTest::qExec(&test_files, argc, argv);
    }

    {
        TestToolbar test_toolbar;
        result |= QTest::qExec(&test_toolbar, argc, argv);
    }

    return result;
}
