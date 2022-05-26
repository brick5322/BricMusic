#include "BricMusic.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BricMusic w(QColor("E799B0"));
    w.show();
    return a.exec();
}
