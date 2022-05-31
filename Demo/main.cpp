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


int main(int argc, char* argv[])
{
	if (argc < 2)
		return 0;
#ifdef _DEBUG
	for (int i = 1; i < argc; i++)
		qDebug() << QTime::currentTime() << argv[i];
#endif 
	QCoreApplication a(argc, argv);

	Controller::PlayBackMode mode = Controller::loop;
	char** _argv = argv;
	if (argv[1][0] == '-')
	{
		switch (argv[1][1])
		{
		case 's':
			mode = Controller::singleTune;
			_argv++;
			argc--;
			break;
		case 'L':
			mode = Controller::loopPlayBack;
			_argv++;
			argc--;
			break;
		case 'l':
			mode = Controller::loop;
			_argv++;
			argc--;
			break;
		case 'r':
			mode = Controller::randomTune;
			_argv++;
			argc--;
			break;
		default:
			break;
		}
	}

	AudioFileManager manager(argc, _argv);
	Controller ctrler;
	Player player(&ctrler);
	Decoder decoder(&ctrler);


	QObject::connect(&decoder, &Decoder::basicInfo, &ctrler, &Controller::getContext);

	QObject::connect(&ctrler, &Controller::setDecode, &decoder, &Decoder::open);
	QObject::connect(&ctrler, &Controller::getData, &decoder, &Decoder::decode);
	QObject::connect(&ctrler, &Controller::setContext, &player, &Player::resetContext);
	QObject::connect(&ctrler, &Controller::setPausing, &player, &Player::pause);
	QObject::connect(&ctrler, &Controller::setPlaying, &player, &Player::play);
	QObject::connect(&ctrler, &Controller::getAudioPath, &manager, &AudioFileManager::findNextAudio);
	
	//QObject::connect(&player, &Player::getData, &ctrler, &Controller::setData);
	QObject::connect(&player, &Player::playReady, &ctrler, &Controller::playTaskReady);
	QObject::connect(&player, &Player::terminated, &ctrler, &Controller::on_player_terminated);

	QObject::connect(&decoder, &Decoder::decodeFinish, &ctrler, &Controller::stop);
	QObject::connect(&decoder, &Decoder::decodeErr, &decoder, &Decoder::close);


	//Debug
	QObject::connect(&ctrler, &Controller::playTaskReady, &ctrler, &Controller::playTaskStart);
	QObject::connect(&ctrler, &Controller::playTaskFinished, &ctrler, &Controller::playTaskInit);
	QObject::connect(&ctrler, &Controller::timestampChanged, [](int timestamp) {qDebug() << QTime::currentTime() << timestamp; });
	QObject::connect(&ctrler, &Controller::menuEmpty, &a, &QCoreApplication::quit);
	ctrler.setMode(Controller::loop);
	ctrler.playTaskInit();

	return a.exec();
}
