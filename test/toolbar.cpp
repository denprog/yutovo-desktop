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

void TestToolbar::testIndefiniteIntegral()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    auto action = window->findChild<QAction*>("actionIndefiniteIntegral");
    QVERIFY(action);
    QVERIFY(!action->icon().isNull());
    action->trigger();
    QTest::qWait(200);
    QCOMPARE(document->ToText(), U"indefinite_integral(,)");
}

void TestToolbar::testDerivative()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    auto action = window->findChild<QAction*>("actionDerivative");
    QVERIFY(action);
    QVERIFY(!action->icon().isNull());
    action->trigger();
    QTest::qWait(200);
    QCOMPARE(document->ToText(), U"diff(,)");
}

void TestToolbar::testSecondDerivative()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    auto action = window->findChild<QAction*>("actionSecondDerivative");
    QVERIFY(action);
    QVERIFY(!action->icon().isNull());
    action->trigger();
    QTest::qWait(200);
    QCOMPARE(document->ToText(), U"diff(diff(,),)");
}

void TestToolbar::testPartialDerivative()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    auto action = window->findChild<QAction*>("actionPartialDerivative");
    QVERIFY(action);
    QVERIFY(!action->icon().isNull());
    action->trigger();
    QTest::qWait(200);
    QCOMPARE(document->ToText(), U"diff(,)");
}
