#include "BricMusic.h"
#include <QtWidgets/QApplication>
#include "Decoder.h"
#include "Player.h"
#include "Controller.h"
#include "AudioFileManager.h"

//#include <QTime>
//#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Controller::PlayBackMode mode = Controller::loop;
    QColor color("#E799B0");
    int nb_menu = argc;
    char** menu = argv;
    AudioFileManager manager(nb_menu, menu);

	BricMusic w(color);
    Controller& ctrler = w.controller();

    Player player(&w.controller());
    Decoder decoder(&w.controller());

	QObject::connect(&w, &BricMusic::setVolume, &player, &Player::setVolume);

	QObject::connect(&decoder, &Decoder::basicInfo, &ctrler, &Controller::getContext);

	QObject::connect(&ctrler, &Controller::setDecode, &decoder, &Decoder::open);
	QObject::connect(&ctrler, &Controller::getData, &decoder, &Decoder::decode);
	QObject::connect(&ctrler, &Controller::setContext, &player, &Player::resetContext);
	QObject::connect(&ctrler, &Controller::setPausing, &player, &Player::pause);
	QObject::connect(&ctrler, &Controller::setPlaying, &player, &Player::play);
	QObject::connect(&ctrler, &Controller::getAudioPath, &manager, &AudioFileManager::findNextAudio);

	//QObject::connect(&player, &Player::getData, &ctrler, &Controller::setData);
	QObject::connect(&player, &Player::playReady, &ctrler, &Controller::playTaskReady);
	QObject::connect(&player, &Player::paused, &ctrler, &Controller::on_player_paused);

	QObject::connect(&decoder, &Decoder::decodeFinish, &ctrler, &Controller::stop);
	QObject::connect(&decoder, &Decoder::attachedPic, &w, &BricMusic::setPic);
	QObject::connect(&decoder, &Decoder::decodeErr, &decoder, &Decoder::close);


	//Debug
	//QObject::connect(&ctrler, &Controller::timestampChanged, [](int timestamp) {qDebug() << QTime::currentTime() << timestamp; });
	QObject::connect(&ctrler, &Controller::menuEmpty, &a, &QCoreApplication::quit);
	ctrler.playTaskInit();
	w.show();
    return a.exec();
}
