#include "AudioFileManager.h"
#include "Controller.h"

#ifdef _DEBUG
#include <QDebug>
#include <QTime>
#endif

AudioFileManager::AudioFileManager(int argc,char** argv, QObject* parent)
	: QObject(parent),pos(0)
{
	filepaths(copy);
	for (int i = 1; i < argc; i++)
		filepaths.add(argv[i]);
}

void AudioFileManager::findNextAudio(const char*& path, int mode)
{
	path = nullptr;
	if (mode & Controller::prev)
	{
		if (!(path = filepaths[--pos]))
			path = filepaths[pos = filepaths.length() - 1];
	}
	else
		switch (mode)
		{
		case Controller::loop:
			path = filepaths[++pos];
			break;
		case Controller::loopPlayBack:
			path = filepaths[++pos];
			if (!path)
				path = filepaths[pos = 0];
			break;
		case Controller::singleTune:
			path = filepaths[pos];
			break;
		case Controller::randomTune:
			while (!path)
				path = filepaths[pos = rand() % filepaths.length()];
			break;
		default:
			break;
		}
#ifdef _DEBUG
	qDebug() << QTime::currentTime() << "findNextAudio:" << path;
#endif	
}

AudioFileManager::~AudioFileManager()
{
}

const char* AudioFileManager::findFirstAudio()
{
	return filepaths[0];
}

void AudioFileManager::addAudio(const char* path)
{
	filepaths.add(path);
}
