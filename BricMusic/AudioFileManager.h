#pragma once

#include <QObject>
#include "List_cstr.h"
#include "Controller.h"

class AudioFileManager : public QObject
{
	Q_OBJECT
public:
	AudioFileManager(int argc, char** argv, QObject* parent = Q_NULLPTR);
	~AudioFileManager(); 

public slots:
	void findNextAudio(const char*& path, int mode);
private:
	List_cstr filepaths;
	int pos;
};
