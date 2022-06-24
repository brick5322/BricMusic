#pragma once

#include <QObject>
#include <QSharedMemory>
#include <QThread>
#include <QTimer>
#include <QSet>
#include <QList>
#include <QMutex>
#include <QFile>
#include "ProcProto.h"

extern"C"
{
#include <lualib.h>
#include <lauxlib.h>
#include <lua.h>
}

#define FORMAT_INVALID 0xffffffff

class AudioFileManager : public QSharedMemory
{
	Q_OBJECT

    static const QString& sharedMemoryKey;
    static constexpr size_t sharedMemorySize = 4096;
	QByteArray luaCall(const QByteArray&);
public:
	enum InputOption { Default, Pause, Prev, Next };
	AudioFileManager(int nb_filepaths, char** filepaths);
	~AudioFileManager(); 
	bool insert(int i, const QByteArray& filepath);
	void append(const QByteArray& filepath);
	void appendClosure(int closure);
	inline int size() { return lPaths.size(); }
	bool AudioFileManagerCreate(InputOption opt = Default);
	bool isDynatic() { return is_dynaticScript; }
	QByteArray findFirstAudio();
public slots:
	QByteArray findNextAudio(int mode);
	void saveBLU(const QString& filepath);
private slots:
    void on_server_timeout();
    void on_client_timeout();
signals:
	void newProcesstask();
	void processSetPause();
	void processSetPrev();
	void processSetNext();

	void luaOpenErr();
	void getPath(QByteArray);
	void sendFinished();
private:
	lua_State* Config;
	QMutex mtx;
	QThread* thr;
	QTimer timer;
	int timerID;
	int interval;
	QByteArray path;
	QSet<QByteArray> sPaths;
	QList<QByteArray> lPaths;
	int current_fp_pos;
	QFile blu;
	bool is_dynaticScript;
};
