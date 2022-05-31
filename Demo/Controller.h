#pragma once

#include <QTimer>
#include <QPixmap>
#include "FIFO.h"
//#define _DEBUG

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <SDL.h>
}

class Controller : public QObject
{
	Q_OBJECT
signals:
	void getData(FIFO&);
	void setContext(SDL_AudioSpec&);
	void playTaskFinish();
	void timestampChanged(int timestamp);
	void setPausing();
	void setPlaying();
public slots:
	void getContext(AVSampleFormat sampleFormat, int channel_layout, int sample_rate,double stream_duration);
	void setData(unsigned char* buffer, int len);
	void on_player_terminated();
	void start();
	void stop();

protected:
	void timerEvent(QTimerEvent*);
public:
	static constexpr size_t SDL_buffersz = 1024;
	Controller(QObject *parent = Q_NULLPTR);
	~Controller();
	SDL_mutex* mutex();
private:
	FIFO fifo;
	SDL_mutex* mtx;
	SDL_AudioSpec audioContext;
	int timerID;
	int audioTimestamps;
	int playTimestamp;
	bool is_finishing;
	bool is_pausing;
	bool is_paused;
};
