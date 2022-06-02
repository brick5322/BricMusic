#include "AudioFileManager.h"
#include <QTimerEvent>
#include <QtWidgets/QApplication>
#include <QDebug>

#ifdef _WIN32
#include <Windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#endif // _WIN32

const QString& AudioFileManager::sharedMemoryKey = QString("BricMusicSharedMemoryKey");

AudioFileManager::AudioFileManager(int nb_filepaths, char** filepaths)
	: QSharedMemory(sharedMemoryKey,nullptr),thr(nullptr),timerID(0)
{
	this->filepaths = filepaths;
	this->nb_filepaths = nb_filepaths;
	this->current_fp_pos = 0;
	for (int i = 0; i < nb_filepaths; i++)
		addAudioPath(filepaths[i]);
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
		QObject::connect(&timer, &QTimer::timeout, this, &AudioFileManager::on_client_timeout);
		timer.start();
	}
	return false;
}

AudioFileManager::~AudioFileManager()
{
	this->moveToThread(QThread::currentThread());
	timer.stop();
	if (thr)
	{
		thr->quit();
		thr->wait();
		delete thr;
	}
}

void AudioFileManager::findNextAudio(const char*& path, int mode)
{

}

const char* AudioFileManager::findFirstAudio()
{
	return NULL;
}

void AudioFileManager::on_server_timeout()
{
	static qint64 latest_pid = 0;
    if (!*(qint64*)data())
	{
		if (latest_pid) {
#ifdef _WIN32
			HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, true, latest_pid);
			if (hRemoteProcess && hRemoteProcess != INVALID_HANDLE_VALUE)
			{
				CloseHandle(hRemoteProcess);
				return;
			}
#elif defined(__linux__)
			if (!kill(latest_pid, 0))
				return;
#endif
			latest_pid = 0;
			}
		return;
		}

	lock();
	qint64 current_pid = *(qint64*)data();
	addAudioPath((char*)data() + sizeof(qint64));
	if (latest_pid != current_pid)
		emit newProcesstask();
	latest_pid = current_pid;
	*(qint64*)data() = 0;
	unlock();
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
	if (current_fp_pos < nb_filepaths)
	{
#ifdef _WIN32
		* (qint64*)data() = (qint64)GetCurrentProcessId();
#elif defined(__linux__)
		* (qint64*)data() = (qint64)getpid();
#endif // _WIN32
		strcpy((char*)data() + sizeof(qint64), filepaths[current_fp_pos++]);
		unlock();
	}
	else
	{
		unlock();
		emit sendFinished();
		timer.stop();
	}
}

void AudioFileManager::addAudioPath(const char* path)
{
    qDebug()<<path;
}



