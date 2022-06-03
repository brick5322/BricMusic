#pragma once

#include <QObject>
#include <QSharedMemory>
#include <QThread>
#include <QTimer>
extern"C"
{
#include "LoopList.h"
}

class AudioFileManager : public QSharedMemory
{
	Q_OBJECT

    static const QString& sharedMemoryKey;
    static constexpr size_t sharedMemorySize = 4096;
public:
	AudioFileManager(int nb_filepaths, char** filepaths);
	~AudioFileManager(); 

	bool AudioFileManagerCreate();
	const char* findFirstAudio();// @todo
public slots:
    void findNextAudio(const char*& path, int mode);// @todo
    void addAudioPath(const char* path);// @todo
private slots:
    void on_server_timeout();
    void on_client_timeout();
signals:
	void newProcesstask();
	void sendFinished();
private:
	QThread* thr;
	QTimer timer;
	int timerID;
	int interval;

	LoopList* list;

	char** filepaths;
	int nb_filepaths;
	int current_fp_pos;

};
