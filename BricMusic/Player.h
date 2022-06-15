#pragma once
#include <QObject>
#include <functional>

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
	void terminated();
	void playReady();
public:
	Player(std::function<void(unsigned char*, int)> dataCallback, QObject* parent = Q_NULLPTR);
	~Player(){}
	static constexpr int SDL_buffersz = 1024;
	static void Player_Callback(Player* plr, Uint8* stream, int len);
private:
	SDL_AudioSpec audioFormat;
	std::function<void(unsigned char*,int)> setData;
	int externVolume;
	int privateVolume;
	bool pausing;
};
