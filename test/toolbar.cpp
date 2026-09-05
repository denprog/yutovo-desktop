#include "toolbar.h"
#include <QDebug>
#include <QTest>
#include <QMouseEvent>
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
    QCOMPARE(document->ToText(), U"derivative(,)");
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
    QCOMPARE(document->ToText(), U"derivative(derivative(,),)");
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
    QCOMPARE(document->ToText(), U"derivative(,)");
}

void TestToolbar::testTextBlock()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    auto action = window->findChild<QAction*>("text_block_action");
    QVERIFY(action);
    QVERIFY(!action->icon().isNull());
    QCOMPARE(action->text(), "Insert text block");

    action->trigger();
    QTest::qWait(200);

    std::vector<yutovo::ElementId> els;
    document->GetElement({0})->GetElements(yutovo::ElementType::TEXT_BLOCK, els);
    QVERIFY(els.size() == 1);

    //typing inside the text block is not computed - the text equation keeps the entered right part
    document->WaitTask(document->InsertString("2", true));
    document->WaitTask(document->InsertPlus(true));
    document->WaitTask(document->InsertString("2", true));
    document->WaitTask(document->InsertEquation(yutovo_solver::ResultType::AUTO, true));
    document->WaitTask(document->InsertString("4", true));
    QTest::qWait(1000);
    QCOMPARE(document->ToText(), U"2+2=4");
}

void TestToolbar::testTextBlockNoCodeBlock()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    auto action = window->findChild<QAction*>("text_block_action");
    QVERIFY(action);
    action->trigger();
    QTest::qWait(200);

    //inserting a code block inside the text block is rejected
    document->WaitTask(document->InsertCode(false, true));
    QTest::qWait(200);

    std::vector<yutovo::ElementId> code_blocks, text_blocks;
    document->GetElement({0})->GetElements(yutovo::ElementType::CODE_BLOCK, code_blocks);
    document->GetElement({0})->GetElements(yutovo::ElementType::TEXT_BLOCK, text_blocks);
    QVERIFY(code_blocks.size() == 1); //only the code block seeded for a new document
    QVERIFY(text_blocks.size() == 1);
    QCOMPARE(document->ToText(), U"");
}

void TestToolbar::testTextBlockInText()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    //click into the document text below the code block and insert a text block there
    auto widget = window->findChild<DocumentWidget*>();
    QVERIFY(widget);
    QTest::mouseClick(widget, Qt::LeftButton, Qt::NoModifier, QPoint(300, 350));
    QTest::qWait(200);

    auto action = window->findChild<QAction*>("text_block_action");
    QVERIFY(action);
    action->trigger();
    QTest::qWait(300);

    //typing = inside a text block placed in the text creates a text equation
    for (auto c : std::u32string(U"2x=9"))
        QTest::keyClicks(widget, QString(c));
    QTest::qWait(500);

    std::vector<yutovo::ElementId> equations;
    document->GetElement({0})->GetElements(yutovo::ElementType::TEXT_EQUATION, equations);
    QVERIFY(equations.size() == 1);
    QCOMPARE(document->ToText(), U"2x=9");

    //the formula toolbar commands work inside a text block placed in the text
    QTest::keyClicks(widget, "+");
    QTest::qWait(200);
    std::vector<yutovo::ElementId> pluses;
    document->GetElement({0})->GetElements(yutovo::ElementType::PLUS, pluses);
    QVERIFY(pluses.size() == 1);
}

