#include <QtCore/QCoreApplication>
#include <QThread>
#include "Decoder.h"
#include "Player.h"
#include "Controller.h"
#include "AudioFileManager.h"

#ifdef _DEBUG
#include <QDebug>
#include <QTime>
#endif


int main(int argc, char *argv[])
{
    if (argc < 2)
        return 0;
#ifdef _DEBUG
    for (int i = 1; i < argc; i++)
        qDebug() << QTime::currentTime() << argv[i];
#endif 
    QCoreApplication a(argc, argv);
    AudioFileManager manager(argc, argv);
    Controller ctrler;
    Player player(&ctrler);
    Decoder decoder(&ctrler);

    ctrler.setInterval(10);

    //QObject::connect(&decoder, &Decoder::decodeFin, &ctrler, &Controller::stop);
    QObject::connect(&decoder, &Decoder::basicInfo, &ctrler, &Controller::getContext);
    QObject::connect(&decoder, &Decoder::attachedPic, &ctrler, &Controller::getPic);

    QObject::connect(&ctrler, &Controller::getData, &decoder, &Decoder::decode);
    QObject::connect(&ctrler, &Controller::setContext, &player, &Player::resetContext);
    QObject::connect(&ctrler, &Controller::playTaskFinish, &manager ,&AudioFileManager::findNextAudio);
    QObject::connect(&ctrler, &QTimer::timeout, &ctrler, &Controller::on_controller_timeout);
    QObject::connect(&ctrler, &Controller::setPausing, &player, &Player::pause);

    QObject::connect(&player, &Player::getData, &ctrler, &Controller::setData);
    //QObject::connect(&player, &Player::terminated, &ctrler, &Controller::on_player_terminated);

    QObject::connect(&manager, &AudioFileManager::endofList, &a, &QCoreApplication::quit);
    QObject::connect(&manager, &AudioFileManager::setFilePath, &decoder, &Decoder::open);
    QObject::connect(&manager, &AudioFileManager::setFilePath, &ctrler, &Controller::start);

    manager.findNextAudio();
    return a.exec();
}
