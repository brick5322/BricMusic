#include "BricMusic.h"
#include <QtWidgets/QApplication>
#include "Decoder.h"
#include "Player.h"
#include "Controller.h"
#include "AudioFileManager.h"
#include <QSystemTrayIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QColor color("#E799B0");
    int nb_menu = argc;
    char** menu = argv;


    AudioFileManager manager(nb_menu, menu);
	BricMusic w(color,manager.findFirstAudio());
    Controller& ctrler = w.controller();
    Player player(&ctrler);
    Decoder decoder(&ctrler);
	QSystemTrayIcon icon(&w);

	icon.setToolTip(QString("BricMusic"));
	icon.setIcon(QIcon(":/img/BM-LOGO/icon16.png"));
	icon.show();

	QObject::connect(&icon, &QSystemTrayIcon::activated,
		[&w](QSystemTrayIcon::ActivationReason r){
			if (r == QSystemTrayIcon::DoubleClick)
				w.show();});

	QObject::connect(&w, &BricMusic::setVolume, &player, &Player::setVolume);

	QObject::connect(&decoder, &Decoder::attachedPic, &w, &BricMusic::setPic);
	QObject::connect(&decoder, &Decoder::decodeErr, &w, &BricMusic::close);
	QObject::connect(&decoder, &Decoder::basicInfo, &ctrler, &Controller::getContext);
	QObject::connect(&decoder, &Decoder::decodeFinish, &ctrler, &Controller::stop);

	QObject::connect(&ctrler, &Controller::setDecode, &decoder, &Decoder::open);
	QObject::connect(&ctrler, &Controller::getData, &decoder, &Decoder::decode);
	QObject::connect(&ctrler, &Controller::flushDecoder, &decoder, &Decoder::flush);
	QObject::connect(&ctrler, &Controller::stopDecoder, &decoder, &Decoder::close);

	QObject::connect(&ctrler, &Controller::setContext, &player, &Player::resetContext);
	QObject::connect(&ctrler, &Controller::setPausing, &player, &Player::pause);
	QObject::connect(&ctrler, &Controller::setPlaying, &player, &Player::play);
	QObject::connect(&ctrler, &Controller::getAudioPath, &manager, &AudioFileManager::findNextAudio);

	QObject::connect(&player, &Player::playReady, &ctrler, &Controller::playTaskReady);
	QObject::connect(&player, &Player::paused, &ctrler, &Controller::on_player_paused);

	ctrler.playTaskInit();
	w.show();

    return a.exec();
}
