#include <QApplication>
#include <QFileInfo>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("Yutovo");

    QString filename;
    QStringList args = QCoreApplication::arguments();
    if (args.size() > 1)
    {
        QFileInfo fi(args.at(1));
        if (fi.exists() && fi.isFile())
            filename = args.at(1);
    }

    MainWindow w;
    try
    {
        w.Start(filename);
    }
    catch (std::exception& ex)
    {
        printf("Error starting: %s\n", ex.what());
        return 1;
    }
    w.show();
    return a.exec();
}
