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

public:
	static constexpr size_t SDL_buffersz = 1024;
	static constexpr size_t AudioLevel = 64;
	enum PlayBackMode { loop, loopPlayBack, singleTune, randomTune };
	Controller(QObject *parent = Q_NULLPTR);
	~Controller();
	SDL_mutex* mutex();
	void flush_playtask();
	bool isFinishing();
signals:
	void getData(FIFO&);
	void setContext(SDL_AudioSpec&);
	void playTaskReady();
	void playTaskFinished();
	void timestampChanged(int timestamp);
	void getAudioPath(const char*&, PlayBackMode);
	void setDecode(const char*);
	void setPausing();
	void setPlaying();
	void setDuration(int duration);
	void menuEmpty();
	void paused();
public slots:
	void playTaskInit();
	void playTaskStart();
	void getContext(AVSampleFormat sampleFormat, int channel_layout, int sample_rate, double stream_duration);
	void setData(unsigned char* buffer, int len);
	void setMode(PlayBackMode mode);
	void on_player_paused();
	void start();
	void stop();
protected:
	void timerEvent(QTimerEvent*);
private:
	FIFO fifo;
	SDL_mutex* mtx;
	SDL_AudioSpec audioContext;
	int timerID;
	int audioTimestamps;
	int playTimestamp;
	PlayBackMode mode;
	const char* audiopath;
	bool is_finishing;
	bool is_pausing;
	bool is_paused;
};
