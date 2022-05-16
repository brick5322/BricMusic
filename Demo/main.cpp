#include <QtCore/QCoreApplication>
#include <QThread>
#include "Decoder.h"
#include "Player.h"
#include "Controller.h"

const char* audio2 = "../testfile/ãöğ°yousa - 09.ÏçÇéÇú£¨·­³ª£©.flac";
const char* audio3 = "../testfile/skycity1.mp3";
const char* audio4 = "../testfile/chleh,¾ÅÖØÁÙ - Ö¦½­ÍùÊÂ ¸ÖÇÙ°æ.flac";

int main(int argc, char *argv[])
{
    if (argc < 2)
        return 0;
    QCoreApplication a(argc, argv);
    Controller ctrler;
    Player player(&ctrler);
    Decoder decoder(&ctrler);
    ctrler.setInterval(10);
    ctrler.setTimerType(Qt::CoarseTimer);

    QObject::connect(&decoder, &Decoder::decodeFin, &ctrler, &Controller::stop);
    QObject::connect(&decoder, &Decoder::basicInfo, &ctrler, &Controller::getContext);
    QObject::connect(&decoder, &Decoder::attachedPic, &ctrler, &Controller::getPic);

    QObject::connect(&ctrler, &Controller::getData, &decoder, &Decoder::decode);
    QObject::connect(&ctrler, &Controller::setContext, &player, &Player::resetContext);
    QObject::connect(&ctrler, &Controller::playTaskFinish, &a, &QCoreApplication::quit);
    QObject::connect(&ctrler, &QTimer::timeout, &ctrler, &Controller::on_controller_timeout);
    QObject::connect(&ctrler, &Controller::setPausing, &player, &Player::pause);

    QObject::connect(&player, &Player::getData, &ctrler, &Controller::setData);
    QObject::connect(&player, &Player::terminated, &ctrler, &Controller::on_player_terminated);

    decoder.open(argv[1]);
    ctrler.start();
    return a.exec();
}
