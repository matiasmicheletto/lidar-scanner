
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);

    MainWindow *window = new MainWindow;

    window->show();

    return app.exec();

    delete window;
    qDebug() << "Programa terminado.";

    return 0;
}
