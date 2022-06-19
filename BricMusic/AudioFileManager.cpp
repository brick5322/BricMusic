#include "AudioFileManager.h"
#include <QTimerEvent>
#include <QtWidgets/QApplication>

#ifdef _DEBUG
#include <QDebug>
#endif
extern"C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

#ifdef _WIN32
#include <Windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#endif // _WIN32

extern"C"
{
#include <lualib.h>
#include <lauxlib.h>
#include <lua.h>
}

const QString& AudioFileManager::sharedMemoryKey = QString("BricMusicSharedMemoryKey");



AudioFileManager::AudioFileManager(int nb_filepaths, char** filepaths)
	: QSharedMemory(sharedMemoryKey,nullptr),thr(nullptr),timerID(0),current_fp_pos(0)
{
	lua_State* Config = luaL_newstate();
	luaL_openlibs(Config);
	for (size_t i = 0; i < nb_filepaths; i++)
	{
		if (!strcmp((filepaths[i] + strlen(filepaths[i]) - 4), ".blu"))
		{
#ifdef _WIN32
			if (luaL_dofile(Config, QString(filepaths[i]).toLocal8Bit().data()))
				continue;
#elif defined(__linux__)
			if (luaL_dofile(Config, filepaths[i]))
				continue;
#endif // _WIN32
			if (lua_getglobal(Config, "MenuList") != LUA_TTABLE)
				continue;
			int i = 0;
			while (true) {
				lua_pushnumber(Config, ++i);
				if (lua_gettable(Config, -2) != LUA_TSTRING)
					break;
				else
				{
#ifdef _DEBUG
					qDebug() << lua_tostring(Config, -1);
#endif // _DEBUG
					append(lua_tostring(Config, -1));
					lua_pop(Config, 1);
				}
			}
		}
		else
			append(filepaths[i]);
	}
	lua_close(Config);
}

bool AudioFileManager::AudioFileManagerCreate(InputOption opt)
{
    timer.setInterval(30);

	if (create(sharedMemorySize))
	{
		thr = new QThread;
		moveToThread(thr);
		thr->start();
		thr->setPriority(QThread::LowestPriority);
		QObject::connect(&timer, &QTimer::timeout, this, &AudioFileManager::on_server_timeout);
		timer.start();
		return true;
	}
	else if (attach())
	{
		if (opt == Default)
		{
			current_fp_pos = 0;
			QObject::connect(&timer, &QTimer::timeout, this, &AudioFileManager::on_client_timeout);
			timer.start();
		}
		else
		{
			lock();
			ProcProto::Header& header = *(ProcProto::Header*)data();
			header.set_code(opt);
			//((qint64*)data())[0] = 1;
			//((qint64*)data())[1] = opt;
			unlock();
		}
	}
	return false;
}

AudioFileManager::~AudioFileManager()
{
	timer.stop();
	if (thr)
	{
		thr->quit();
		thr->wait();
		delete thr;
	}
}

bool AudioFileManager::insert(int i, const QByteArray& filepath)
{
#ifdef _WIN32
	QByteArray str = filepath;
	str.replace('\\', '/');
#endif // _WIN32
	if (sPaths.contains(str))
	{
		current_fp_pos = lPaths.indexOf(str)-1;
		return false;
	}
	else
	{
		sPaths.insert(str);
		lPaths.insert(i, str);
		current_fp_pos = i-1;
		return true;
	}
}

void AudioFileManager::append(const QByteArray& filepath)
{

#ifdef _WIN32
	QByteArray str = filepath;
	str.replace('\\', '/');
#endif // _WIN32
#ifdef _DEBUG
	qDebug() << "try to append" << str;
#endif // _DEBUG
	AVFormatContext* ctx = nullptr;
	if (sPaths.contains(str))
	{
		current_fp_pos = lPaths.indexOf(str);
		return;
	}
	else if (avformat_open_input(&ctx, str.data(), NULL, NULL))
		return;
	else
	{
		avformat_close_input(&ctx);
		sPaths.insert(str);
		lPaths.append(str);
#ifdef _DEBUG
		qDebug() << "append" << str;
#endif // _DEBUG
		current_fp_pos++;
		return;
	}
}

void AudioFileManager::findNextAudio(int mode)
{
	mtx.lock();
	if (mode < 0)
		--current_fp_pos;
	else
	switch (mode)
	{
	case 2:
		++current_fp_pos;
		break;
	case 3:
		emit getPath(path);
		goto unl_ret;
	case 4:
		srand(time(0));
		current_fp_pos = rand();
		break;
	default:
		break;
	}
	current_fp_pos += lPaths.size();
	current_fp_pos %= lPaths.size();
	emit getPath(path = lPaths[current_fp_pos]);
#ifdef _DEBUG
	qDebug() << QString::fromLocal8Bit("ÏÂÒ»Ê×:") << current_fp_pos << QString::fromUtf8(path)<<QString::fromLocal8Bit(path);
#endif // _DEBUG
unl_ret:
	mtx.unlock();

}

QByteArray AudioFileManager::findFirstAudio()
{
	current_fp_pos = 0;
	return lPaths[current_fp_pos];
}

void AudioFileManager::on_server_timeout()
{
	mtx.lock();
	static qint64 latest_pid = 0;
	static int pos = 0;
    qint64 current_pid = 0;
	ProcProto::Header& header = *(ProcProto::Header*)data();
    if (!header.get_pid())
	{
		if (latest_pid) {
#ifdef __linux__
			if (!kill(latest_pid, 0))
				goto unl_ret;
#endif
			pos = 0;
			latest_pid = 0;
			}
		mtx.unlock();
		return;
	}
	else if (int code = header.get_code())
	{
		lock();
		switch (code)
		{
		case Pause:
			emit processSetPause();
			break;
		case Prev:
			emit processSetPrev();
			break;
		case Next:
			emit processSetNext();
			break;
		default:
			break;
		}
		header.set_pid(0);
		unlock();
		mtx.unlock();
		return;
	}

	lock();
    current_pid = header.get_pid();
	if (insert(current_fp_pos + pos + 1, header.get_buffer()))
		pos++;
	if (latest_pid != current_pid)
	{
		emit newProcesstask();
		latest_pid = current_pid;
	}
	header.set_pid(0);
	unlock();
	mtx.unlock();
}

void AudioFileManager::on_client_timeout()
{
	ProcProto::Header& header = *(ProcProto::Header*)data();
    if (header.get_pid())
    {
        detach();
        if(!attach())
        {
            emit sendFinished();
            timer.stop();
        }
        return;
    }
	lock();

#ifdef _WIN32
	header.set_pid(GetCurrentProcessId());
#elif defined(__linux__)
	header.set_pid(getpid());
#endif // _WIN32
	strcpy(header.get_buffer(), lPaths[current_fp_pos++].toStdString().c_str());
	if (current_fp_pos==lPaths.size())
	{
		emit sendFinished();
		unlock();
		timer.stop();
	}
	else
		unlock();
}

void AudioFileManager::saveBLU(const QString& filepath)
{
	if (filepath.isEmpty())
		return;
	QByteArray fpArr = filepath.toLocal8Bit();
	FILE* fp = fopen(fpArr.data(), "w");
	fputs("MenuList = {\n", fp);
	for (const QByteArray& i : lPaths)
	{
		fputs("    \"", fp);
		fputs(i.data(), fp);
		fputs("\",\n", fp);
	}
	fputs("}\n", fp);
	fclose(fp);
}

