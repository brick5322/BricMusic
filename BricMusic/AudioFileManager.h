#pragma once

#include <QObject>
#include "List_cstr.h"

class AudioFileManager : public QObject
{
	Q_OBJECT
public:
	AudioFileManager(int argc = 0, char** argv = nullptr, QObject* parent = Q_NULLPTR);
	~AudioFileManager(); 
	const char* findFirstAudio();

public slots:
	void findNextAudio(const char*& path, int mode);
	void addAudio(const char* path);
private:
	List_cstr filepaths;
	int pos;
};
