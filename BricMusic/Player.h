#pragma once
#include <QObject>
#include "Controller.h"

extern"C"
{
#include <SDL.h>
}

class Player : public QObject
{

	Q_OBJECT

public slots:
	void play();
	void pause();
	void terminate();
	void close();
	void setVolume(int volume);
	void resetContext(SDL_AudioSpec& context);
signals:
	void getData(unsigned char* buffer,int len);
	void paused();
	void playReady();
public:
	Player(Controller* parent = Q_NULLPTR);
	static constexpr int SDL_buffersz = 1024;
	static void Player_Callback(Player* plr, Uint8* stream, int len);
private:
	SDL_AudioSpec audioFormat;
	int externVolume;
	int privateVolume;
	bool pausing;
};
