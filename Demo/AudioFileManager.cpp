#include "AudioFileManager.h"

#ifdef _DEBUG
#include <QDebug>
#include <QTime>
#endif
AudioFileManager::AudioFileManager(int argc,char** argv, QObject* parent)
	: QObject(parent),pos(0)
{
	filepaths(move);
	for (int i = 1; i < argc; i++)
		filepaths.add(argv[i]);
}

void AudioFileManager::findNextAudio()
{
	const char* i = filepaths[pos++];
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
