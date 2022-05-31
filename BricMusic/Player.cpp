#include "Player.h"

#ifdef _DEBUG
#include <QDebug>
#include <QTime>
#endif

struct SDL_Initializer {
	SDL_Initializer() {
		SDL_Init(SDL_INIT_AUDIO);
	}

	~SDL_Initializer() {
		SDL_Quit();
	}
}initializer;

Player::Player(Controller* parent): QObject(parent),externVolume(128),privateVolume(128),pausing(false)
{
	audioFormat.freq = 44100;
	audioFormat.channels = 2;
	audioFormat.samples = SDL_buffersz;
	audioFormat.format = AUDIO_S32SYS;

	audioFormat.userdata = this;
	audioFormat.callback = (SDL_AudioCallback)Player::Player_Callback;
	SDL_OpenAudio(&audioFormat, nullptr);
}

void Player::Player_Callback(Player* plr, Uint8* stream, int len)
{
	if (plr->pausing && plr->privateVolume == 0){
#ifdef _DEBUG
		qDebug() << QTime::currentTime() << "emit terminate";
#endif
		emit plr->terminate();
		return;
	}
	if (plr->pausing)
		plr->privateVolume--;
	else if (plr->privateVolume < plr->externVolume)
		plr->privateVolume++;
	else
		plr->privateVolume = plr->externVolume;
	SDL_memset(stream, 0, len);
	static_cast<Controller*>(plr->parent())->setData(stream, len);

	if (plr->privateVolume == 128)
		goto ret;
	switch (plr->audioFormat.format)
	{
	case AUDIO_S16SYS:
		for (int i = 0; i < len/sizeof(short); i++)
			((short*)stream)[i] *= (double)plr->privateVolume/128;
		break;
	case AUDIO_S32SYS:
		for (int i = 0; i < len/sizeof(int); i++)
			((int*)stream)[i] *= (double)plr->privateVolume / 128;
		break;
	case AUDIO_U16SYS:
		for (int i = 0; i < len / sizeof(short); i++)
			((uint16_t*)stream)[i] *= (double)plr->privateVolume / 128;
		break;
	case AUDIO_U8:
		for (int i = 0; i < len; i++)
			stream[i] *= (double)plr->privateVolume / 128;
		break;
	default:
		break;
	}
ret:
	//initializer.write(stream, len);
	return;
}

void Player::play(){
	SDL_PauseAudio(false);
	pausing = false;
}

void Player::pause(){
#ifdef _DEBUG
	qDebug() << QTime::currentTime() << "start pause";
#endif
	pausing = true;
}

void Player::terminate(){
#ifdef _DEBUG
	qDebug() << QTime::currentTime() << "start terminate";
#endif
	SDL_PauseAudio(true);
	pausing = false;
	emit terminated();
}

void Player::close()
{
	SDL_CloseAudio();
}

void Player::setVolume(int volume) {
	this->externVolume = volume;
}


void Player::resetContext(SDL_AudioSpec& context) {
	if (SDL_GetAudioStatus() != SDL_AUDIO_STOPPED)
	{
		SDL_PauseAudio(true);
		SDL_CloseAudio();
	}
	audioFormat = context;
	audioFormat.samples = SDL_buffersz;
	audioFormat.userdata = this;
	audioFormat.callback = (SDL_AudioCallback)Player::Player_Callback;
	SDL_OpenAudio(&audioFormat,nullptr);
	play();
}