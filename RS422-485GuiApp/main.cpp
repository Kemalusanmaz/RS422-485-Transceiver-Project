#include "mainwindow.h"
#include "logger.hpp"
#include <QApplication>

Logger g_logger;

int main(int argc, char *argv[])
{
    g_logger.init(argv[0]);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
