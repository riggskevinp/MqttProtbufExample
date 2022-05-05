#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    std::string xml_config = "templatetest.xml";
    if(argc >= 2){
        xml_config = std::string(argv[1]);
    }

    QApplication a(argc, argv);
    MainWindow w(nullptr, xml_config);
    w.show();

    return a.exec();
}
