#include <QtCore/QCoreApplication>
#include "AudioFileManager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    AudioFileManager manager(argc - 1, argv+1);
    if(manager.AudioFileManagerCreate())
        printf("Hello World!\n");
    else
        printf("Hello Kitty!\n");
    QObject::connect(&manager, &AudioFileManager::sendFinished, &a, &QCoreApplication::quit);
    return a.exec();
}
