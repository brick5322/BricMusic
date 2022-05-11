#pragma once
#include <QObject>

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
	void resetContext(SDL_AudioSpec context);
signals:
	void getData(unsigned char* buffer,int len);
	void terminated();
public:
	Player(int& externVolume, QObject* parent = Q_NULLPTR);

	static void Player_Callback(Player* plr, Uint8* stream, int len);
private:
	SDL_AudioSpec audioFormat;
	int& externVolume;
	int privateVolume;
	bool pausing;
};
