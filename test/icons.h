#include <QtTest/QtTest>
#include <QApplication>
#include <QPalette>
#include "../src/mainwindow.h"

class TestIcons : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testLoad();
    void testDarkPaletteTint();
    void testLightPaletteTint();
    void testTonePreservation();
    void testDarkPaletteTwoToneTint();
    void testRetintOnPaletteChange();

private:
    static int AverageLuminance(const QIcon& icon);
    static QPalette DarkPalette();
    static QPalette LightPalette();

    QPalette original_palette;
    MainWindow* window = nullptr;
};
