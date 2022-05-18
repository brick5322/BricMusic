#pragma once

#include <QObject>
#include "List_cstr.h"

class AudioFileManager : public QObject
{
	Q_OBJECT
signals:
	void setFilePath(const char*);
	void endofList();
public slots:
	void findNextAudio();
public:
	AudioFileManager(int argc,char** argv,QObject *parent = Q_NULLPTR);
	~AudioFileManager();
private:
	List_cstr filepaths;
	int pos;
};
