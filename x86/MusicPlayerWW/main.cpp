#include "MainWidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget w;
    if(argc < 2) w.show();
    if(argc >= 2 && argv[1] == QStringLiteral("-hide")) w.hide();
    return a.exec();
}
