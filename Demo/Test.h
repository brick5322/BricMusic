#pragma once

#include <QTimer>
#include "FIFO.h"

extern "C"{
#include <SDL.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

class Test : public QTimer
{
	Q_OBJECT
signals:
	void call_write(FIFO&);
	void resetContext(SDL_AudioSpec);
public slots:
	void setContext(AVSampleFormat sampleFormat, int channel_layout, int sample_rate);
	void decodecFin();
public:
	SDL_AudioSpec context;
	int interval;
	int volume;
	FIFO* buffer;
	Test(int interval,QObject *parent = Q_NULLPTR);
	~Test();
};
