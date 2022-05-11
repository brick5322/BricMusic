#include "Player.h"


struct SDL_Initializer {
	//FILE* fp;
	SDL_Initializer() {
		//fp = fopen("playeroutput.pcm", "wb");
		SDL_Init(SDL_INIT_AUDIO);
	}

	~SDL_Initializer() {
		//fclose(fp);
		SDL_Quit();
	}
}initializer;

Player::Player( QObject* parent): QObject(parent),externVolume(100),privateVolume(100),pausing(false)
{
	audioFormat.freq = 44100;
	audioFormat.channels = 2;
	audioFormat.samples = 1024;
	audioFormat.format = AUDIO_S32SYS;

	audioFormat.userdata = this;
	audioFormat.callback = (SDL_AudioCallback)Player::Player_Callback;
	SDL_OpenAudio(&audioFormat, nullptr);
}

#define Player_vol_weighting(Type,buffer,volume,samples) for (int i = 0; i < (samples)/sizeof(Type); i++) \
		((Type*)(buffer))[i] *= (volume);

void Player::setVolume(int volume)
{
	externVolume = volume;
}

void Player::Player_Callback(Player* plr, Uint8* stream, int len)
{
	if (plr->privateVolume == 0){
		emit plr->terminate();
		return;
	}

	if (plr->pausing)
		plr->privateVolume--;
	else if (plr->privateVolume < 100)
		plr->privateVolume++;
	else
		plr->privateVolume = 100;

	float volume = (float)plr->externVolume * plr->privateVolume / 10000;
	plr->getData(stream, len);
	switch (plr->audioFormat.format)
	{
	case AUDIO_S16SYS:
		Player_vol_weighting(short, stream, volume, plr->audioFormat.samples);
		break;
	case AUDIO_S32SYS:
		Player_vol_weighting(int, stream, volume, plr->audioFormat.samples);
		break;
	case AUDIO_F32SYS:
		Player_vol_weighting(float, stream, volume, plr->audioFormat.samples);
		break;
	case AUDIO_U16SYS:
		Player_vol_weighting(unsigned short, stream, volume, plr->audioFormat.samples);
		break;
	case AUDIO_U8:
		Player_vol_weighting(unsigned char, stream, volume, plr->audioFormat.size);
		break;
	default:
		break;
	}
}

void Player::play(){
	SDL_PauseAudio(false);
	pausing = false;
}

void Player::pause(){
	pausing = true;
}

void Player::terminate(){
	SDL_PauseAudio(true);
	pausing = false;
	emit terminated();
}

void Player::resetContext(SDL_AudioSpec context,FIFO*& fifo) {
	SDL_CloseAudio();
	context.userdata = audioFormat.userdata;
	context.callback = audioFormat.callback;
	SDL_OpenAudio(&context,nullptr);
	int sz = context.freq * context.channels * (context.format & 0x3f) >> 3;
	fifo = new FIFO(sz);
	play();
}