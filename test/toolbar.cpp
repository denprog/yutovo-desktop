#include "toolbar.h"
#include <QDebug>
#include "../src/document_widget.h"
#include "../src/document_window.h"

void TestToolbar::initTestCase()
{
}

void TestToolbar::cleanupTestCase()
{
}

void TestToolbar::init()
{
    QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
    window = new MainWindow();
    window->Start("");
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window));
}

void TestToolbar::cleanup()
{
    delete window;
}

void TestToolbar::testCalculusToolbar()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    auto toolbar = window->findChild<QToolBar*>("calculus_toolbar");
    QVERIFY(toolbar);
    QVERIFY(toolbar->isVisible());

    auto integral_action = window->findChild<QAction*>("actionDefiniteIntegral");
    QVERIFY(integral_action);
    QVERIFY(!integral_action->icon().isNull());
    integral_action->trigger();
    QTest::qWait(200);
    QCOMPARE(document->ToText(), U"definite_integral(,,,)");
}
