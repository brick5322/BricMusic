#include "BricMusic.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QColor color("#E799B0");
	BricMusic w(color);
    w.show();
    return a.exec();
}
