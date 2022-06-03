#include "AudioFileManager.h"

#ifdef _DEBUG
#include <QDebug>
#include <QTime>
#endif

AudioFileManager::AudioFileManager(int argc,char** argv, QObject* parent)
	: QObject(parent),pos(-1)
{
	filepaths(move);
	for (int i = 1; i < argc; i++)
		filepaths.add(argv[i]);
}

void AudioFileManager::findNextAudio(const char*& path, int mode)
{
	const char* i = nullptr;
	switch (mode)
	{
	case Controller::loop:
		i = filepaths[++pos];
		break;
	case Controller::loopPlayBack:
		i = filepaths[++pos];
		if (!i)
			i = filepaths[pos = 0];
		break;
	case Controller::singleTune:
		i = filepaths[pos];
		break;
	case Controller::randomTune:
		while(!i)
			i = filepaths[rand() % filepaths.length()];
		break;
	default:
		break;
	}
	
#ifdef _DEBUG
	qDebug() << QTime::currentTime() << "findNextAudio:" << i;
#endif	
	path = i;

	//if (i)
	//	emit setFilePath(i);
	//else
	//	emit endofList();
}

AudioFileManager::~AudioFileManager()
{
}
