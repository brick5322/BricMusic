#include "AudioFileManager.h"
#include <QTimerEvent>
#include <QtWidgets/QApplication>
#include <QDebug>

extern"C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

#ifdef _WIN32
#include <Windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#endif // _WIN32

const QString& AudioFileManager::sharedMemoryKey = QString("BricMusicSharedMemoryKey");



AudioFileManager::AudioFileManager(int nb_filepaths, char** filepaths)
	: QSharedMemory(sharedMemoryKey,nullptr),thr(nullptr),timerID(0),current_fp_pos(0)
{
	for (size_t i = 0; i < nb_filepaths; i++)
		Init(filepaths[i]);
}

bool AudioFileManager::AudioFileManagerCreate()
{
    timer.setInterval(30);

	if (create(sharedMemorySize))
	{
		thr = new QThread;
		moveToThread(thr);
		thr->start();
		thr->setPriority(QThread::LowestPriority);
		QObject::connect(&timer, &QTimer::timeout, this, &AudioFileManager::on_server_timeout);
		timer.start();
		return true;
	}
	else if (attach())
	{
		current_fp_pos = 0;
		QObject::connect(&timer, &QTimer::timeout, this, &AudioFileManager::on_client_timeout);
		timer.start();
	}
	return false;
}

AudioFileManager::~AudioFileManager()
{
	timer.stop();
	if (thr)
	{
		thr->quit();
		thr->wait();
		delete thr;
	}
}

bool AudioFileManager::insert(int i, const QString& str)
{
	if (sPaths.contains(str))
		return false;
	else
	{
		sPaths.insert(str);
		lPaths.insert(i,str);
		return true;
	}
}

bool AudioFileManager::append(const QString& str)
{
	AVFormatContext* ctx = nullptr;
	if (sPaths.contains(str))
		return false;
	else if (avformat_open_input(&ctx, str.toStdString().c_str(), NULL, NULL))
		return false;
	else
	{
		avformat_close_input(&ctx);
		sPaths.insert(str);
		lPaths.append(str);
		return true;
	}
}

void AudioFileManager::findNextAudio(int mode)
{
	mtx.lock();
	if (mode < 0)
		--current_fp_pos;
	else
	switch (mode)
	{
	case 2:
		++current_fp_pos;
		break;
	case 3:
		emit getPath(path);
		goto unl_ret;
	case 4:
		srand(time(0));
		current_fp_pos = rand();
		break;
	default:
		break;
	}
	current_fp_pos %= lPaths.size();
	emit getPath(path = lPaths[current_fp_pos]);
#ifdef _DEBUG
	qDebug() << QString::fromLocal8Bit("ÏÂÒ»Ê×:") << current_fp_pos << path;
#endif // _DEBUG
unl_ret:
	mtx.unlock();

}

QString AudioFileManager::findFirstAudio()
{
	current_fp_pos = 0;
	return lPaths[current_fp_pos];
}

void AudioFileManager::on_server_timeout()
{
	mtx.lock();
	static qint64 latest_pid = 0;
	static int pos = 0;
    if (!*(qint64*)data())
	{
		if (latest_pid) {
#ifdef __linux__
			if (!kill(latest_pid, 0))
				goto unl_ret;
#endif
			pos = 0;
			latest_pid = 0;
			}
		goto unl_ret;
	}

	lock();
	qint64 current_pid = *(qint64*)data();
	if (latest_pid != current_pid)
	{
		insert(current_fp_pos + ++pos, (char*)data() + sizeof(qint64));
		emit newProcesstask();
	}
	else
		insert(current_fp_pos + ++pos, (char*)data() + sizeof(qint64));
	latest_pid = current_pid;
	*(qint64*)data() = 0;
	unlock();
unl_ret:
	mtx.unlock();
}

void AudioFileManager::on_client_timeout()
{
    if (*(qint64*)data())
    {
        detach();
        if(!attach())
        {
            emit sendFinished();
            timer.stop();
        }
        return;
    }
	lock();

#ifdef _WIN32
	* (qint64*)data() = (qint64)GetCurrentProcessId();
#elif defined(__linux__)
	* (qint64*)data() = (qint64)getpid();
#endif // _WIN32
	strcpy((char*)data() + sizeof(qint64), lPaths[current_fp_pos++].toStdString().c_str());
	if (current_fp_pos==lPaths.size())
	{
		emit sendFinished();
		unlock();
		timer.stop();
	}
	else
		unlock();
}

void AudioFileManager::Init(const QString& filepath)
{
	append(filepath);
	current_fp_pos++;
}


