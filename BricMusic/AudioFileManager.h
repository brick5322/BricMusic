#pragma once

#include <QObject>
#include <QSharedMemory>
#include <QThread>
#include <QTimer>
#include <QSet>
#include <QList>
#include <QMutex>


class AudioFileManager : public QSharedMemory
{
	Q_OBJECT

    static const QString& sharedMemoryKey;
    static constexpr size_t sharedMemorySize = 4096;
	bool insert(int i, const QString& str);
	bool append(const QString& str);
public:
	enum InputOption { Default, Pause, Prev, Next };
	AudioFileManager(int nb_filepaths, char** filepaths);
	~AudioFileManager(); 
	void Init(const QString& filepath);
	inline int size() { return lPaths.size(); }
	bool AudioFileManagerCreate(InputOption);
	QString findFirstAudio();// @todo
public slots:
    void findNextAudio(int mode);// @todo
private slots:
    void on_server_timeout();
    void on_client_timeout();
signals:
	void newProcesstask();
	void processSetPause();
	void processSetPrev();
	void processSetNext();

	void getPath(QString);
	void sendFinished();
private:
	QMutex mtx;
	QThread* thr;
	QTimer timer;
	int timerID;
	int interval;
	QString path;
	QSet<QString> sPaths;
	QList<QString> lPaths;
	int current_fp_pos;

};
