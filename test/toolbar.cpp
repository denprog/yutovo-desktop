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
#include "../src/matrix_dialog.h"

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

void TestToolbar::testLimit()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    //the action is searched through the toolbar actions - a language change recreates the toolbars
    //and leaves stale duplicate actions parented to the window
    auto toolbar = window->findChild<QToolBar*>("calculus_toolbar");
    QVERIFY(toolbar);
    QAction* action = nullptr;
    for (QAction* a : toolbar->actions())
    {
        if (a->objectName() == "actionLimit")
            action = a;
    }
    QVERIFY(action);
    QVERIFY(!action->icon().pixmap(QSize(32, 32)).isNull());
    action->trigger();
    QTest::qWait(200);
    QCOMPARE(document->ToText(), U"lim[→]");

    //fill the variable, the value and the function after the limit
    document->InsertString(U"x", true);
    document->MoveCaretRight(false); //cross the arrow into the value row
    document->InsertString(U"0", true);
    document->MoveCaretRight(false); //leave the element into the containing row
    document->InsertString(U"x", true);
    document->WaitTask(document->InsertPlus(true));
    document->WaitTask(document->InsertString(U"1", true));
    QCOMPARE(document->ToText(), U"lim[x→0]x+1");

    document->Undo();
    document->WaitUndo();
    QTest::qWait(200);
    QCOMPARE(document->ToText(), U"lim[x→0]x+");
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

void TestToolbar::testUnorderedList()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    //the dropdown button offers all the four list markers
    auto button = window->findChild<QPushButton*>("unordered_list_button");
    QVERIFY(button);
    QVERIFY(!button->icon().isNull());
    QVERIFY(button->menu());
    QCOMPARE(button->menu()->actions().size(), 4);

    auto small_circle = window->findChild<QAction*>("unordered_list_small_circle_action");
    auto large_circle = window->findChild<QAction*>("unordered_list_large_circle_action");
    auto diamond = window->findChild<QAction*>("unordered_list_diamond_action");
    auto square = window->findChild<QAction*>("unordered_list_square_action");
    QVERIFY(small_circle);
    QVERIFY(large_circle);
    QVERIFY(diamond);
    QVERIFY(square);
    QVERIFY(!small_circle->icon().isNull());
    QVERIFY(!large_circle->icon().isNull());
    QVERIFY(!diamond->icon().isNull());
    QVERIFY(!square->icon().isNull());
    QCOMPARE(small_circle->text(), "Small circle");
    QCOMPARE(large_circle->text(), "Large circle");
    QCOMPARE(diamond->text(), "Diamond");
    QCOMPARE(square->text(), "Square");

    //the button is disabled while the caret sits inside the code block of a new document - typing there moves the caret and syncs the toolbar
    auto widget = window->findChild<DocumentWidget*>();
    QVERIFY(widget);
    QTest::keyClicks(widget, "1");
    QTest::qWait(200);
    QVERIFY(!button->isEnabled());

    //click into the document text below the code block - the button enables and a list item can be typed
    QTest::mouseClick(widget, Qt::LeftButton, Qt::NoModifier, QPoint(300, 350));
    QTest::qWait(200);
    QVERIFY(button->isEnabled());
    document->WaitTask(document->InsertString("Item", true));

    auto count_list_items =
        [document]() -> int
        {
            std::string html = document->ToHtml();
            int count = 0;
            for (size_t pos = html.find("<li>"); pos != std::string::npos; pos = html.find("<li>", pos + 4))
                ++count;
            return count;
        };

    small_circle->trigger();
    QTest::qWait(200);
    QCOMPARE(count_list_items(), 1);

    //Enter continues the list
    document->WaitTask(document->InsertParagraph(true));
    QCOMPARE(count_list_items(), 2);

    //another marker replaces the current one - the caret sits in the new (second) item
    square->trigger();
    QTest::qWait(200);
    QCOMPARE(count_list_items(), 2);

    //applying the same marker again toggles it off
    square->trigger();
    QTest::qWait(200);
    QCOMPARE(count_list_items(), 1);

    //the remaining marker variants apply as well - the caret stays in the second item
    large_circle->trigger();
    QTest::qWait(200);
    QCOMPARE(count_list_items(), 2);

    diamond->trigger();
    QTest::qWait(200);
    QCOMPARE(count_list_items(), 2);
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


//The matrix toolbar button: the action exists, the dialog prefills the sizes, the insert fills the grid
void TestToolbar::testMatrix()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    auto action = window->findChild<QAction*>("actionMatrix");
    QVERIFY(action);
    QVERIFY(!action->icon().isNull());
    QVERIFY(!action->icon().pixmap(QSize(32, 32)).isNull());

    //the dialog is checked directly - triggering the action opens a modal dialog
    MatrixDialog dialog("Insert matrix", 3, 2);
    auto rows = dialog.findChild<QSpinBox*>("rows");
    auto columns = dialog.findChild<QSpinBox*>("columns");
    QVERIFY(rows && columns);
    QVERIFY(rows->value() == 3 && columns->value() == 2);
    QVERIFY(rows->minimum() == 1 && rows->maximum() == 100 && columns->maximum() == 100);
    rows->setValue(2);
    columns->setValue(2);
    dialog.accept();
    QVERIFY(dialog.rows == 2 && dialog.columns == 2);

    //insert a 2x2 matrix through the document api and fill it
    document->InsertCode(false, true);
    document->WaitTask(document->InsertMatrix(true, 2, 2));
    document->InsertString("1", true);
    document->MoveCaretRight(false);
    document->InsertString("2", true);
    document->MoveCaretRight(false);
    document->InsertString("3", true);
    document->MoveCaretRight(false);
    document->WaitTask(document->InsertString("4", true));
    QTest::qWait(500);
    QCOMPARE(document->ToText(), U"[[1,2],[3,4]]");
}

//The determinant toolbar button
void TestToolbar::testDeterminant()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    auto action = window->findChild<QAction*>("actionDeterminant");
    QVERIFY(action);
    QVERIFY(!action->icon().isNull());
    QVERIFY(!action->icon().pixmap(QSize(32, 32)).isNull());

    document->InsertCode(false, true);
    document->WaitTask(document->InsertDeterminant(true, 2, 2));
    document->InsertString("1", true);
    document->MoveCaretRight(false);
    document->InsertString("2", true);
    document->MoveCaretRight(false);
    document->InsertString("3", true);
    document->MoveCaretRight(false);
    document->WaitTask(document->InsertString("4", true));
    QTest::qWait(500);
    QCOMPARE(document->ToText(), U"det([[1,2],[3,4]])");
}

//The matrix size dialog of the context menu grows and shrinks the grid
void TestToolbar::testMatrixResize()
{
    auto document = window->GetCurrentDocument();
    QVERIFY(document);

    document->InsertCode(false, true);
    document->WaitTask(document->InsertMatrix(true, 2, 2));

    auto el = document->FindByType(yutovo::ElementId{0}, yutovo::ElementType::MATRIX);
    QVERIFY(el);

    //the context menu dialog prefills the current size
    uint rows = 0, columns = 0;
    QVERIFY(document->GetMatrixSize(el->id, rows, columns));
    QVERIFY(rows == 2 && columns == 2);
    MatrixDialog dialog("Matrix size", rows, columns);
    dialog.findChild<QSpinBox*>("rows")->setValue(3);
    dialog.findChild<QSpinBox*>("columns")->setValue(3);
    dialog.accept();
    QVERIFY(dialog.rows == 3 && dialog.columns == 3);

    document->WaitTask(document->SetMatrixSize(el->id, dialog.rows, dialog.columns, true));
    QTest::qWait(500);
    QVERIFY(document->GetMatrixSize(el->id, rows, columns));
    QVERIFY(rows == 3 && columns == 3);
    QCOMPARE(document->ToText(), U"[[,,],[,,],[,,]]");

    //shrinking removes the cells from the right and bottom
    document->WaitTask(document->SetMatrixSize(el->id, 2, 2, true));
    QTest::qWait(500);
    QVERIFY(document->GetMatrixSize(el->id, rows, columns));
    QVERIFY(rows == 2 && columns == 2);
    QCOMPARE(document->ToText(), U"[[,],[,]]");
}


//The matrices toolbar holds the matrix and determinant buttons, the View menu toggles it
void TestToolbar::testMatrixToolbar()
{
    auto toolbar = window->findChild<QToolBar*>("matrix_toolbar");
    QVERIFY(toolbar);
    QVERIFY(toolbar->isVisible());

    //both buttons live in the matrices toolbar, not in the algebra toolbar;
    //the lookup goes through the toolbar actions - window-wide findChild can return an action
    //of a previous toolbar generation recreated by a language change
    QAction* matrix = nullptr;
    QAction* determinant = nullptr;
    for (QAction* a : toolbar->actions())
    {
        if (a->objectName() == "actionMatrix")
            matrix = a;
        else if (a->objectName() == "actionDeterminant")
            determinant = a;
    }
    QVERIFY(matrix && determinant);

    auto algebra_toolbar = window->findChild<QToolBar*>("algebra_toolbar");
    QVERIFY(algebra_toolbar);
    for (QAction* a : algebra_toolbar->actions())
        QVERIFY(a->objectName() != "actionMatrix" && a->objectName() != "actionDeterminant");

    //the View menu entry hides and shows the toolbar
    QVERIFY(window->matrix_toolbar_action);
    QVERIFY(window->matrix_toolbar_action->isChecked());
    window->matrix_toolbar_action->toggle();
    QVERIFY(!toolbar->isVisible());
    window->matrix_toolbar_action->toggle();
    QVERIFY(toolbar->isVisible());

    //the buttons are enabled with a document open - triggering them opens the modal MatrixDialog,
    //the insertion itself is covered by testMatrix/testDeterminant through the dialog and the document api
    QVERIFY(matrix->isEnabled());
    QVERIFY(determinant->isEnabled());
}
