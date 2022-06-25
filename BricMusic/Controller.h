#pragma once

#include <QTimer>
#include <QPixmap>
#include <functional>
#include "FIFO.h"
//#define _DEBUG

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <SDL.h>
}
static constexpr size_t AudioLevel = 64;

class Controller : public QObject
{
	Q_OBJECT
public:
	static constexpr size_t SDL_buffersz = 1024;
	enum PlayBackMode { None = 0,loop, loopPlayBack, singleTune, randomTune,quit = 0x40000000,prev = 0x80000000 };
	enum States { playing, changingPos, pausingAudio, terminated, finishing = 4, toNextAudio=5, toPrevAudio=6 };
	Controller(std::function<QByteArray(int)> getPath,QObject *parent = Q_NULLPTR);
	~Controller();
	SDL_mutex* mutex();
	FIFO& buffer();
	bool isFinishing();
	void setAction(States action);
signals:
	void getData(void*);
	void setContext(SDL_AudioSpec&);

	void playTaskReady();
	void playTaskFinished();

	void timestampChanged(int timestamp);

	void setDecode(const QByteArray&);
	void flushDecoder(unsigned int timestamp);
	void stopDecoder();

	void setPlaying();
	void setPausing();

	void setDuration(int duration);
	void paused();
public slots:
	void playTaskInit();
	void playTaskStart();
	void playTaskStop();

	void getNextAudio();
	void getPrevAudio();
	void getAudio();

	void posChange(int timestamp);
	void getContext(AVSampleFormat sampleFormat, int channel_layout, int sample_rate, double stream_duration);
	void setData(unsigned char* buffer, int len);
	void setNextPath(QByteArray);
	void setMode(PlayBackMode mode);
	void on_player_terminated();
	void start();
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
	States state;
	QByteArray recentPath;
	std::function<QByteArray(int)> getPath;
};
