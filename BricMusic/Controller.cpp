#include "Controller.h"
#include <QImage>
#include <QBitmap>
#include <QPainter>
#ifdef _DEBUG
#include <QDebug>
#include <QTime>
#endif

Controller::Controller(std::function<QByteArray(int)> getPath,QObject* parent)
	: QObject(parent), fifo(SDL_buffersz* AudioLevel * 2 * 4, FIFO::StrictWrite | FIFO::ReadMostSz),getPath(getPath),
	state(playing),
	mtx(SDL_CreateMutex()), timerID(0), playTimestamp(0)
{
	qRegisterMetaType<AVSampleFormat>("AVSampleFormat");
	recentPath.clear();
}

Controller::~Controller()
{
	SDL_DestroyMutex(mtx);
}

SDL_mutex* Controller::mutex()
{
	return mtx;
}

FIFO& Controller::buffer()
{
	return this->fifo;
}


bool Controller::isFinishing()
{
	return state & finishing;
}

void Controller::setAction(States state)
{
	this->state = state;
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
		break;
	case AV_CH_LAYOUT_STEREO:
		audioContext.channels = 2;
		break;
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

void Controller::setNextPath(QByteArray p)
{
	this->recentPath = p;
}

void Controller::setMode(PlayBackMode mode)
{
	this->mode = mode;
}

void Controller::on_player_terminated()
{
	switch (state)
	{
	case Controller::changingPos:
		state = playing;
		emit setPlaying();
		break;
	case Controller::pausingAudio:
		state = terminated;
		emit paused();
		break;
	case Controller::toNextAudio:
	case Controller::toPrevAudio:
		state = terminated;
#ifdef _DEBUG
		qDebug() << QTime::currentTime() << "emit playTaskFinish";
#endif
		emit playTaskFinished();
		break;
	default:
		break;
	}
}

void Controller::posChange(int timestamp)
{
	if (state & finishing)
		return;
	else if (state != playing)
	{
		SDL_LockMutex(mtx);
		fifo.reset();
		SDL_UnlockMutex(mtx);
	}
	else
	{
		state = changingPos;
		emit setPausing();
	}
	playTimestamp = timestamp - AudioLevel;
	emit flushDecoder(timestamp * SDL_buffersz);
}


void Controller::playTaskStart()
{
	recentPath.clear();
	state = playing;
	emit setPlaying();
}

void Controller::playTaskStop()
{
	if(recentPath.isEmpty())
		recentPath = getPath(mode & 0x3fffffff);
	emit setPausing();
	if (timerID)
		killTimer(timerID);
	timerID = 0;
	if (state == terminated)
		emit playTaskFinished();
}

void Controller::getNextAudio()
{
	recentPath = getPath(loopPlayBack);
	if(state !=terminated)
		state = toNextAudio;
	emit stopDecoder();
}

void Controller::getPrevAudio()
{
	recentPath = getPath(loopPlayBack | prev);
	if (state != terminated)
		state = toPrevAudio;
	emit stopDecoder();
}

void Controller::start()
{
	//act = playing;
	timerID = startTimer(5);
}

void Controller::playTaskInit()
{
	emit setDecode(recentPath);
}

void Controller::timerEvent(QTimerEvent*)
{
	if (fifo.freesize())
		getData(mtx);
}
