#include "AudioFileManager.h"

#ifdef _DEBUG
#include <QDebug>
#include <QTime>
#endif

AudioFileManager::AudioFileManager(int argc,char** argv,PlayBackMode mode, QObject* parent)
	: QObject(parent),pos(0),mode(mode)
{
	filepaths(move);
	for (int i = 1; i < argc; i++)
		filepaths.add(argv[i]);
}

void AudioFileManager::findNextAudio()
{
	const char* i = nullptr;
	switch (mode)
	{
	case AudioFileManager::loop:
		i = filepaths[pos++];
		break;
	case AudioFileManager::loopPlayBack:
		i = filepaths[pos++];
		if (!i)
			i = filepaths[pos = 0];
		break;
	case AudioFileManager::singleTune:
		i = filepaths[pos];
		break;
	case AudioFileManager::randomTune:
		while(!i)
			i = filepaths[rand() % filepaths.length()];
		break;
	default:
		break;
	}
	
#ifdef _DEBUG
	qDebug() << QTime::currentTime() << "findNextAudio:" << i;
#endif	
	if (i)
		emit setFilePath(i);
	else
		emit endofList();
}

AudioFileManager::~AudioFileManager()
{
}

AudioFileManager::PlayBackMode AudioFileManager::getMode()
{
	return mode;
}

void AudioFileManager::setMode(PlayBackMode mode)
{
	this->mode = mode;
}
