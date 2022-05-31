#include "BricMusic.h"
#include <QtWidgets/QApplication>
#include "Decoder.h"
#include "Player.h"
#include "Controller.h"
#include "AudioFileManager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AudioFileManager::PlayBackMode mode = AudioFileManager::loop;
    QColor color("#E799B0");
    int nb_menu = argc;
    char** menu = argv;
    AudioFileManager manager(nb_menu, menu, mode);

	BricMusic w(color);

    Player player(&w.controller());
    Decoder decoder(&w.controller());

    QObject::connect(&decoder, &Decoder::basicInfo, &w.controller(), &Controller::getContext);

    QObject::connect(&w.controller(), &Controller::getData, &decoder, &Decoder::decode);
    QObject::connect(&w.controller(), &Controller::setContext, &player, &Player::resetContext);
    QObject::connect(&w.controller(), &Controller::setPausing, &player, &Player::pause);
    QObject::connect(&w.controller(), &Controller::playTaskFinish, &manager, &AudioFileManager::findNextAudio);

    QObject::connect(&player, &Player::getData, &w.controller(), &Controller::setData);

    QObject::connect(&manager, &AudioFileManager::endofList, &a, &QCoreApplication::quit);
    QObject::connect(&manager, &AudioFileManager::setFilePath, &decoder, &Decoder::open);

    QObject::connect(&decoder, &Decoder::deformatErr, &manager, &AudioFileManager::findNextAudio);
    QObject::connect(&decoder, &Decoder::decodeErr, &decoder, &Decoder::close);

    w.show();
    return a.exec();
}
