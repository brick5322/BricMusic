#include <QtCore/QCoreApplication>
#include "Decoder.h"
#include "Player.h"
#include "Controller.h"

const char* audio3 = "skycity1.mp3";

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Decoder decoder;
    Player player;
    Controller ctrler;
    ctrler.setInterval(10);

    QObject::connect(&decoder, &Decoder::decodeFin, &ctrler, &Controller::stop);
    QObject::connect(&decoder, &Decoder::basicInfo, &ctrler, &Controller::getContext);
    QObject::connect(&decoder, &Decoder::attachedPic, &ctrler, &Controller::getPic);

    QObject::connect(&ctrler, &Controller::getData, &decoder, &Decoder::decode);
    QObject::connect(&ctrler, &Controller::setContext, &player, &Player::resetContext);
    QObject::connect(&ctrler, &Controller::playTaskFinish, &a, &QCoreApplication::quit);
    QObject::connect(&ctrler, &QTimer::timeout, &ctrler, &Controller::on_controller_timeout);

    QObject::connect(&player, &Player::getData, &ctrler, &Controller::setData);
    QObject::connect(&player, &Player::terminated, &ctrler, &Controller::on_player_terminated);

    decoder.open(audio3);
    ctrler.start();
    return a.exec();
}
