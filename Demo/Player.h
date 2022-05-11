#pragma once
#include <QObject>
#include "FIFO.h"

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
	void resetContext(SDL_AudioSpec context,FIFO*& fifo);

signals:
	void getData(unsigned char* buffer,int len);
	void terminated();
public:
	Player(QObject* parent = Q_NULLPTR);
	void setVolume(int volume);
	static void Player_Callback(Player* plr, Uint8* stream, int len);
private:
	SDL_AudioSpec audioFormat;
	int externVolume;
	int privateVolume;
	bool pausing;
};
