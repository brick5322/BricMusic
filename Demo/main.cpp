#include <QtCore/QCoreApplication>
#include "Decoder.h"
#include "Player.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Decoder decoder;
    Player player;
    return a.exec();
}
