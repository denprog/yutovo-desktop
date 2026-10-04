#include "toolbar.h"
#include <QDebug>
#include <QTest>
#include <QMouseEvent>
#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>
#include "../src/document_widget.h"
#include "../src/document_window.h"
#include "../src/graph_settings_dialog.h"

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

void TestToolbar::testEvaluationBar()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    auto action = window->findChild<QAction*>("actionEvaluationBar");
    QVERIFY(action);
    QVERIFY(!action->icon().isNull());

    document->InsertCode(false, true);
    document->InsertString("x", true);
    document->InsertPower(true);
    document->InsertString("2", true);
    document->MoveCaretRight(false); //move out of the exponent
    QTest::qWait(200);

    action->trigger();
    QTest::qWait(200);
    document->MoveCaretRight(false);
    document->InsertString("x", true);
    document->MoveCaretRight(false);
    document->MoveCaretRight(false);
    document->InsertString("3", true);
    QTest::qWait(200);
    QCOMPARE(document->ToText(), U"pow(x,2)[x=3]");
}

void TestToolbar::testFunctionAtPoint()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    auto action = window->findChild<QAction*>("actionFunctionAtPoint");
    QVERIFY(action);
    QVERIFY(!action->icon().isNull());

    document->InsertCode(false, true);
    document->InsertString("f", true);
    document->InsertOpenRoundBracket(true);
    document->InsertString("x", true);
    document->InsertCloseRoundBracket(true);
    document->InsertAssignment(true);
    document->InsertString("x", true);
    document->InsertMultiply(true);
    document->WaitTask(document->InsertString("x", true));
    QTest::qWait(1000);

    document->WaitTask(document->InsertParagraph(true));

    action->trigger();
    QTest::qWait(200);
    document->InsertString("f", true);
    document->MoveCaretRight(false);
    document->MoveCaretRight(false);
    document->WaitTask(document->InsertString("x", true));
    for (int i = 0; i < 4; ++i)
        document->MoveCaretRight(false);
    document->WaitTask(document->InsertString("x", true));
    document->MoveCaretRight(false);
    document->MoveCaretRight(false);
    document->WaitTask(document->InsertString("3", true));
    QTest::qWait(200);
    QCOMPARE(document->ToText(),
        U"f(x)=x*x\n"
        U"f(x)[x=3]");
}

void TestToolbar::testFunctionX()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    auto action = window->findChild<QAction*>("actionFunctionX");
    QVERIFY(action);
    QVERIFY(!action->icon().isNull());

    //fresh document: the caret is in a plain text paragraph, the whole template must land in one code block
    action->trigger();
    QTest::qWait(200);
    QCOMPARE(document->ToText(), U"f(x)=");

    //one undo removes the whole template
    document->Undo();
    document->WaitUndo();
    QCOMPARE(document->ToText(), U"");

    //redo restores the template with the caret after the assignment
    document->Redo();
    document->WaitRedo();
    QCOMPARE(document->ToText(), U"f(x)=");

    //typing continues with the function body
    document->WaitTask(document->InsertString("x", true));
    QTest::qWait(200);
    QCOMPARE(document->ToText(), U"f(x)=x");
}

void TestToolbar::testFunctionXY()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    auto action = window->findChild<QAction*>("actionFunctionXY");
    QVERIFY(action);
    QVERIFY(!action->icon().isNull());

    //fresh document: the caret is in a plain text paragraph, the whole template must land in one code block
    action->trigger();
    QTest::qWait(200);
    QCOMPARE(document->ToText(), U"f(x,y)=");

    //one undo removes the whole template
    document->Undo();
    document->WaitUndo();
    QCOMPARE(document->ToText(), U"");

    //redo restores the template with the caret after the assignment
    document->Redo();
    document->WaitRedo();
    QCOMPARE(document->ToText(), U"f(x,y)=");

    //typing continues with the function body
    document->InsertString("x", true);
    document->InsertPlus(true);
    document->WaitTask(document->InsertString("y", true));
    QTest::qWait(200);
    QCOMPARE(document->ToText(), U"f(x,y)=x+y");
}

void TestToolbar::testGraphSurface()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    auto action = window->findChild<QAction*>("graph_surface_action");
    QVERIFY(action);
    QVERIFY(!action->icon().isNull());
    QVERIFY(!action->icon().pixmap(QSize(32, 32)).isNull());

    document->InsertCode(false, true);

    action->trigger();
    QTest::qWait(200);
    //fields: y top, expression, y bottom, x left, x variable, x right, y variable
    document->InsertString("2", true);
    document->MoveCaretRight(false);
    document->InsertString("x", true);
    document->InsertPlus(true);
    document->WaitTask(document->InsertString("y", true));
    //the variable rows are prefilled with "x"/"y": entering a row stops before the string,
    //so passing through it takes three MoveCaretRight calls (enter, cross the string, exit)
    document->MoveCaretRight(false);
    document->MoveCaretRight(false);
    document->MoveCaretRight(false);
    document->InsertMinus(true);
    document->WaitTask(document->InsertString("2", true));
    document->MoveCaretRight(false);
    document->InsertMinus(true);
    document->WaitTask(document->InsertString("4", true));
    document->MoveCaretRight(false);
    document->MoveCaretRight(false);
    document->MoveCaretRight(false);
    document->WaitTask(document->InsertString("4", true));
    document->WaitTask(document->MoveCaretRight(false));
    QTest::qWait(3000);
    QCOMPARE(document->ToText(), U"graph_surface(2,x+y,-2,-4,x,4,y)");
}

void TestToolbar::testGraphHistogram()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    auto action = window->findChild<QAction*>("graph_histogram_action");
    QVERIFY(action);
    QVERIFY(!action->icon().isNull());
    QVERIFY(!action->icon().pixmap(QSize(32, 32)).isNull());

    document->InsertCode(false, true);

    action->trigger();
    QTest::qWait(200);
    //one field: the expressions block, one paragraph is one array of bars
    document->InsertOpenSquareBracket(true);
    document->InsertString("1", true);
    document->InsertComma(true);
    document->WaitTask(document->InsertString("5", true));
    document->WaitTask(document->InsertCloseSquareBracket(true));
    QTest::qWait(3000);
    QCOMPARE(document->ToText(), U"graph_bar([1,5])");
}

void TestToolbar::testGraphFormatAxes()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    auto action = window->findChild<QAction*>("graph_histogram_action");
    QVERIFY(action);
    document->InsertCode(false, true);
    action->trigger();
    QTest::qWait(200);
    document->InsertOpenSquareBracket(true);
    document->InsertString("1", true);
    document->InsertComma(true);
    document->WaitTask(document->InsertString("5", true));
    document->WaitTask(document->InsertCloseSquareBracket(true));
    QTest::qWait(3000);

    auto el = document->FindByType(yutovo::ElementId{0}, yutovo::ElementType::GRAPH_HISTOGRAM);
    QVERIFY(el);

    //fill the axis fields in the graph format dialog
    yutovo::GraphFormat format;
    QVERIFY(document->GetGraphFormat(el->id, format));
    GraphSettingsDialog dialog(format);
    auto axis_color = dialog.findChild<QPushButton*>("axis_color");
    auto width = dialog.findChild<QSpinBox*>("axis_width");
    auto ticks = dialog.findChild<QCheckBox*>("axis_ticks");
    QVERIFY(axis_color && width && ticks);
    //defaults: tick marks on, thickness 1; thickness 0 hides the axis lines
    QVERIFY(ticks->isChecked());
    QVERIFY(width->value() == 1 && width->minimum() == 0);
    width->setValue(3);
    ticks->setChecked(false);
    dialog.accept();
    QVERIFY(format.axis.width == 3 && !format.axis.ticks);

    //push the format through the document and undo it
    document->WaitTask(document->SetGraphFormat(el->id, format, true));
    QTest::qWait(500);
    yutovo::GraphFormat current;
    QVERIFY(document->GetGraphFormat(el->id, current));
    QVERIFY(current.axis.width == 3 && !current.axis.ticks);

    document->Undo();
    QTest::qWait(500);
    QVERIFY(document->GetGraphFormat(el->id, current));
    QVERIFY(current.axis.width == 1 && current.axis.ticks);
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

