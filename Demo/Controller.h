#pragma once

#include <QTimer>
#include <QPixmap>
#include "FIFO.h"

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <SDL.h>
}

constexpr int AlbumSZ = 100;

class Controller : public QTimer
{
	Q_OBJECT
signals:
	void getData(FIFO&);
	void setContext(SDL_AudioSpec&);
	void playTaskFinish();
	void setPausing();
	void setPlaying();
public slots:
	void on_controller_timeout();
	void getContext(AVSampleFormat sampleFormat, int channel_layout, int sample_rate);
	void setData(unsigned char* buffer, int len);
	void getPic(uchar* picdata, int size);
	void on_player_terminated();
	void stop();
public:
	Controller(QObject *parent = Q_NULLPTR);
	~Controller();
private:
	FIFO fifo;
	QPixmap albumImage;
	SDL_AudioSpec audioContext;
	bool is_finishing;
	bool is_pausing;
	bool is_paused;
};
