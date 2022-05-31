#include "Controller.h"
#include <QImage>
#include <QBitmap>
#include <QPainter>
#ifdef _DEBUG
#include <QDebug>
#include <QTime>
#endif

Controller::Controller(QObject *parent)
	: QObject(parent),fifo(SDL_buffersz * AudioLevel * 2 *4,FIFO::StrictWrite|FIFO::ReadMostSz),
	is_finishing(false),is_paused(false),is_pausing(false),
	mtx(SDL_CreateMutex()),timerID(0),playTimestamp(0),audiopath(nullptr)
{
	qRegisterMetaType<Controller::PlayBackMode>("Controller::PlayBackMode");
}

Controller::~Controller()
{
	SDL_DestroyMutex(mtx);
}

SDL_mutex* Controller::mutex()
{
	return mtx;
}

void Controller::getContext(AVSampleFormat sampleFormat, int channel_layout, int sample_rate,double stream_duration)
{
	audioTimestamps = ceil(stream_duration * sample_rate / SDL_buffersz);
	switch (sampleFormat)
	{
	case AV_SAMPLE_FMT_U8:
		audioContext.format = AUDIO_U8;
		break;
	case AV_SAMPLE_FMT_S16:
		audioContext.format = AUDIO_S16SYS;
		break;
	case AV_SAMPLE_FMT_S32:
		audioContext.format = AUDIO_S32SYS;
		break;
	case AV_SAMPLE_FMT_FLT:
		audioContext.format = AUDIO_F32SYS;
		break;
	default:
		audioContext.format = AUDIO_S32SYS;
		break;
	}
	audioContext.freq = sample_rate;
	switch (channel_layout)
	{
	case AV_CH_LAYOUT_MONO:
		audioContext.channels = 1;
	case AV_CH_LAYOUT_STEREO:
		audioContext.channels = 2;
	default:
		audioContext.channels = 2;
		break;
	}
	int sz = SDL_buffersz * AudioLevel * audioContext.channels * (audioContext.format & 0x3f) >> 3;

#ifdef _DEBUG
	qDebug() << QTime::currentTime() << "getContext" << sz;
#endif // _DEBUG

	if (fifo.size() != sz)
		fifo = FIFO(sz);
	else
		fifo.reset();
	playTimestamp = 0;
#ifdef _DEBUG
	qDebug() << QTime::currentTime() << "Controller::start";
#endif // _DEBUG
	emit setContext(audioContext);
	emit setDuration(audioTimestamps);
	start();
}

void Controller::setData(unsigned char* buffer, int len)
{
	SDL_LockMutex(mtx);
	if (fifo[len] >> buffer)
		emit timestampChanged(playTimestamp++);
	SDL_UnlockMutex(mtx);
}

void Controller::setMode(PlayBackMode mode)
{
	this->mode = mode;
}

void Controller::on_player_paused()
{
	if (!is_finishing)
		return paused();
	is_finishing = false;
	is_paused = true;
	is_pausing = false;
#ifdef _DEBUG
	qDebug() << QTime::currentTime()<<"emit playTaskFinish";
#endif
	emit playTaskFinished();
}

void Controller::playTaskStart()
{
	emit setPlaying();
}

void Controller::start()
{
	timerID = startTimer(10);
}

void Controller::stop()
{
	if (!timerID)
		return;
	this->is_finishing = true;
	emit setPausing();
	killTimer(timerID);
}

void Controller::playTaskInit()
{
	if(!audiopath)
		emit getAudioPath(audiopath,mode);
	if (audiopath)
		emit setDecode(audiopath);
	else
		emit menuEmpty();
}

void Controller::timerEvent(QTimerEvent*)
{
	if (fifo.freesize())
		getData(fifo);
}
