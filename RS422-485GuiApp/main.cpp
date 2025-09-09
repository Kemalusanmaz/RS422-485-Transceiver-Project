#include "mainwindow.h"
#include "logger.hpp"
#include "configuration.hpp"
#include <QApplication>

Logger g_logger;
//RSConfiguration g_conf;

int main(int argc, char *argv[])
{
  //  g_conf.init(argv[0]);
    RSConfiguration::init(argv[0]);

    g_logger.init(argv[0]);


    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
