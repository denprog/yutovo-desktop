#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("Yutovo");
    MainWindow w;
    try
    {
        w.Start();
    }
    catch (std::exception& ex)
    {
        printf("Error stating: %s", ex.what());
        return 1;
    }
    w.show();
    return a.exec();
}
