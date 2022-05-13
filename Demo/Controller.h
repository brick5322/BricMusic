#pragma once

#include <QTimer>
#include "FIFO.h"

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <SDL.h>
}

class Controller : public QTimer
{
	Q_OBJECT
signals:
	void getData(FIFO&);
	void setContext(SDL_AudioSpec&);
public slots:
	void getContext(AVSampleFormat sampleFormat, int channel_layout, int sample_rate);
	void getPic(uchar* picdata, int size);
	void on_player_terminated();//检查状态位看看是不是直接结束了
	void stop();//因为要设置状态位，所以还是要重写的
public:
	Controller(QObject *parent);
	~Controller();
private:
	
	bool is_finishing;
	bool is_pausing;
	bool is_paused;
};
