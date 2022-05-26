#pragma once

#include <QObject>
#include "List_cstr.h"
//#define _DEBUG

class AudioFileManager : public QObject
{
	Q_OBJECT
public:
	enum PlayBackMode { loop, loopPlayBack, singleTune, randomTune };

	AudioFileManager(int argc, char** argv, PlayBackMode mode = loop, QObject* parent = Q_NULLPTR);
	~AudioFileManager(); 

	PlayBackMode getMode();
signals:
	void setFilePath(const char*);
	void endofList();
public slots:
	void findNextAudio();
	void setMode(PlayBackMode);
private:
	List_cstr filepaths;
	int pos;
	PlayBackMode mode;
};
