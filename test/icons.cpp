#include "icons.h"
#include <QAction>
#include <QImage>
#include <QToolBar>

//TestIcons

void TestIcons::initTestCase()
{
    original_palette = qApp->palette();

    QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
    window = new MainWindow();
    window->Start("");
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window));
}

void TestIcons::cleanupTestCase()
{
    qApp->setPalette(original_palette);
    delete window;
}

void TestIcons::init()
{
}

void TestIcons::cleanup()
{
    qApp->setPalette(original_palette);
}

int TestIcons::AverageLuminance(const QIcon& icon)
{
    //the pixmap is premultiplied, divide the color channels back by alpha to get the ink color
    QImage image = icon.pixmap(QSize(32, 32)).toImage();
    double sum = 0;
    double weight = 0;
    for (int y = 0; y < image.height(); ++y)
    {
        for (int x = 0; x < image.width(); ++x)
        {
            const QRgb pixel = image.pixel(x, y);
            const int alpha = qAlpha(pixel);
            if (alpha == 0)
                continue;
            const int luminance = qGray(qRed(pixel) * 255 / alpha, qGreen(pixel) * 255 / alpha, qBlue(pixel) * 255 / alpha);
            sum += luminance * alpha;
            weight += alpha;
        }
    }
    return weight > 0 ? (int)(sum / weight) : -1;
}

QPalette TestIcons::DarkPalette()
{
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(49, 49, 49));
    palette.setColor(QPalette::WindowText, QColor(239, 239, 239));
    palette.setColor(QPalette::Base, QColor(37, 37, 37));
    palette.setColor(QPalette::Text, QColor(239, 239, 239));
    palette.setColor(QPalette::Button, QColor(59, 59, 59));
    palette.setColor(QPalette::ButtonText, QColor(239, 239, 239));
    palette.setColor(QPalette::ToolTipBase, QColor(49, 49, 49));
    palette.setColor(QPalette::ToolTipText, QColor(239, 239, 239));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, QColor(49, 49, 49));
    return palette;
}

QPalette TestIcons::LightPalette()
{
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(239, 239, 239));
    palette.setColor(QPalette::WindowText, QColor(0, 0, 0));
    palette.setColor(QPalette::Base, QColor(255, 255, 255));
    palette.setColor(QPalette::Text, QColor(0, 0, 0));
    palette.setColor(QPalette::Button, QColor(239, 239, 239));
    palette.setColor(QPalette::ButtonText, QColor(0, 0, 0));
    palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));
    palette.setColor(QPalette::ToolTipText, QColor(0, 0, 0));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    return palette;
}

void TestIcons::testLoad()
{
    QIcon icon = MainWindow::LoadIcon(":/icons/images/standard/new.png");
    QVERIFY(!icon.isNull());
    QVERIFY(!icon.pixmap(QSize(32, 32)).isNull());
}

void TestIcons::testDarkPaletteTint()
{
    qApp->setPalette(DarkPalette());
    QVERIFY(AverageLuminance(MainWindow::LoadIcon(":/icons/images/standard/new.png")) > 150);
    QVERIFY(AverageLuminance(MainWindow::LoadIcon(":/icons/images/algebra/plus.png")) > 150);
}

void TestIcons::testLightPaletteTint()
{
    qApp->setPalette(LightPalette());
    QVERIFY(AverageLuminance(MainWindow::LoadIcon(":/icons/images/standard/new.png")) < 100);
    QVERIFY(AverageLuminance(MainWindow::LoadIcon(":/icons/images/algebra/plus.png")) < 100);
}

void TestIcons::testTonePreservation()
{
    //grayscale tones of the source (e.g. the placeholder box of the round brackets icon) must
    //survive tinting: with black ink composited over white the result matches the source pixels
    qApp->setPalette(LightPalette());

    const QImage source = QImage(":/icons/images/algebra/round_brackets.png").convertToFormat(QImage::Format_ARGB32);
    const QImage rendered = MainWindow::LoadIcon(":/icons/images/algebra/round_brackets.png").pixmap(source.size()).toImage();
    QCOMPARE(rendered.size(), source.size());

    for (int y = 0; y < source.height(); ++y)
    {
        for (int x = 0; x < source.width(); ++x)
        {
            const QRgb s = source.pixel(x, y);
            const QRgb t = rendered.pixel(x, y);
            const int s_over_white = qGray(s) * qAlpha(s) / 255 + 255 * (255 - qAlpha(s)) / 255;
            const int t_over_white = qGray(t) * qAlpha(t) / 255 + 255 * (255 - qAlpha(t)) / 255;
            QVERIFY2(qAbs(s_over_white - t_over_white) <= 3,
                qPrintable(QString("tone changed at %1,%2: %3 -> %4").arg(x).arg(y).arg(s_over_white).arg(t_over_white)));
        }
    }
}

void TestIcons::testDarkPaletteTwoToneTint()
{
    //on a dark palette the brackets are full strength ink and the placeholder box is half strength
    qApp->setPalette(DarkPalette());

    const QImage rendered = MainWindow::LoadIcon(":/icons/images/algebra/round_brackets.png").pixmap(QSize(20, 20)).toImage();
    bool full = false;
    bool half = false;
    for (int y = 0; y < rendered.height(); ++y)
    {
        for (int x = 0; x < rendered.width(); ++x)
        {
            const int alpha = qAlpha(rendered.pixel(x, y));
            if (alpha >= 250)
                full = true;
            else if (alpha >= 100 && alpha <= 170)
                half = true;
        }
    }
    QVERIFY(full);
    QVERIFY(half);
}

void TestIcons::testRetintOnPaletteChange()
{
    //the icons are searched through the toolbar actions - rebuilding the gui leaves stale
    //duplicate actions parented to the window
    auto FindAction =
        [&]() -> QAction*
        {
            auto toolbar = window->findChild<QToolBar*>("standard_toolbar");
            if (!toolbar)
                return nullptr;
            QAction* action = nullptr;
            for (QAction* a : toolbar->actions())
            {
                if (a->objectName() == "actionNew")
                    action = a;
            }
            return action;
        };

    qApp->setPalette(LightPalette());
    QTest::qWait(100); //the gui rebuild is deferred to the next event loop iteration
    QAction* action = FindAction();
    QVERIFY(action);
    QVERIFY(AverageLuminance(action->icon()) < 100);

    //switching the application palette must rebuild the toolbar with re-tinted icons
    qApp->setPalette(DarkPalette());
    QTest::qWait(100);
    action = FindAction();
    QVERIFY(action);
    QVERIFY(AverageLuminance(action->icon()) > 150);

    qApp->setPalette(LightPalette());
    QTest::qWait(100);
    action = FindAction();
    QVERIFY(action);
    QVERIFY(AverageLuminance(action->icon()) < 100);
}
