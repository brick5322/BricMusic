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
	void Init(const char* filepath);

	bool AudioFileManagerCreate();
	const char* findFirstAudio();// @todo
public slots:
    void findNextAudio(int mode);// @todo
    void addAudioPath(const char* path);// @todo
private slots:
    void on_server_timeout();
    void on_client_timeout();
signals:
	void newProcesstask();
	void getPath(const char*);
	void sendFinished();
private:
	QThread* thr;
	QTimer timer;
	int timerID;
	int interval;

	LoopList* list;

	int current_fp_pos;

};
