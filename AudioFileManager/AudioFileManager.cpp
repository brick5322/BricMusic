#include "AudioFileManager.h"
#include <QTimerEvent>
#include <QtWidgets/QApplication>
#include <QDebug>
extern "C"
{
#include "list_DNode.h"
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
	: QSharedMemory(sharedMemoryKey,nullptr),thr(nullptr),timerID(0),list(LoopList_alloc(0))
{
	LoopList_set_Destructor(list, 0, free);
	for (current_fp_pos = 0; current_fp_pos < nb_filepaths; current_fp_pos++)
		addAudioPath(filepaths[current_fp_pos]);
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
		list->latest = list->first;
		list->latestPos = 0;
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
	if(mode<0)
		path = Node_getData(char*, LoopList_get(list, --current_fp_pos));
	else
	switch (mode)
	{
	case 1:
		path = Node_getData(char*, LoopList_get(list,++current_fp_pos));
	case 2:
		path = Node_getData(char*, LoopList_get(list, current_fp_pos));
	case 3:
		srand(time(0));
		path = Node_getData(char*, LoopList_get(list, current_fp_pos = rand() % list->length));
	default:
		break;
	}
}

const char* AudioFileManager::findFirstAudio()
{
	current_fp_pos = 0;
	return Node_getData(char*,LoopList_get(list,0));
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
	if (latest_pid != current_pid)
	{
		LoopList_get(list, --current_fp_pos);
		addAudioPath((char*)data() + sizeof(qint64));
		emit newProcesstask();
	}
	else
		addAudioPath((char*)data() + sizeof(qint64));
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

#ifdef _WIN32
	* (qint64*)data() = (qint64)GetCurrentProcessId();
#elif defined(__linux__)
	* (qint64*)data() = (qint64)getpid();
#endif // _WIN32
	strcpy((char*)data() + sizeof(qint64), Node_getData(char*, list->latest));
	list->latest = list->latest->next;
	if (list->latest == list->first)
	{
		emit sendFinished();
		unlock();
		timer.stop();
	}
	else
		unlock();
}

void AudioFileManager::addAudioPath(const char* path)
{
	list_DNode* node = LoopList_add(list, 0, sizeof(char*));
	char* cp_path = new char[strlen(path) + 1];
	strcpy(cp_path,path);
	Node_getData(char*, node) = cp_path;
}



