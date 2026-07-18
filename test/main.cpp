#include <QtTest/QtTest>
#include <QApplication>
#include "files.h"
#include "toolbar.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);

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
