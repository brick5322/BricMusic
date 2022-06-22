#include "AudioFileManager.h"
#include <QTimerEvent>
#include <QtWidgets/QApplication>
#include <QStandardPaths>
#include <QMessageBox>

#ifdef _DEBUG
#include <QDebug>
#endif
extern"C"
{
#include <libavformat/avformat.h>
}

#ifdef _WIN32
#include <Windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#endif // _WIN32

const QString& AudioFileManager::sharedMemoryKey = QString("BricMusicSharedMemoryKey");

AudioFileManager::AudioFileManager(int nb_filepaths, char** filepaths)
	: QSharedMemory(sharedMemoryKey,nullptr),
	thr(nullptr),timerID(0),current_fp_pos(0),
	Config(luaL_newstate()), is_dynaticScript(false)
{
	if (!Config)
	{
		 emit luaOpenErr();
		 return;
	}
	luaL_openlibs(Config);
	for (size_t i = 0; i < nb_filepaths; i++)
	{
		if (!strcmp((filepaths[i] + strlen(filepaths[i]) - 4), ".blu"))
		{
			if (blu.isOpen())
			{
				lua_State* tmpL = luaL_newstate();
				if (!tmpL)
					continue;
#ifdef _WIN32
				if (luaL_dofile(Config, QString(filepaths[i]).toLocal8Bit().data()))
#elif defined(__linux__)
				if (luaL_dofile(Config, filepaths[i]))
#endif // _WIN32
				{
					QMessageBox::critical(nullptr,
						QMessageBox::tr("Lua Script Error"),
						QMessageBox::tr("File cannot Load!\nYou've got an error:\n    ")
						+ QString::fromLocal8Bit(lua_tostring(Config, -1)),
						QMessageBox::Cancel);
					lua_pop(Config, 1);
					continue;
			}
				if (lua_getglobal(tmpL, "MenuList") != LUA_TTABLE)
				{
					lua_pop(tmpL, 1);
					continue;
				}
				int len = luaL_len(tmpL, -1);
				for (int i = 1; i <= len; i++)
					if (lua_geti(tmpL, -1, i) == LUA_TSTRING)
					{
						append(lua_tostring(tmpL, -1));
						lua_pop(tmpL, 1);
					}
				lua_pop(tmpL, 1);
				lua_close(tmpL);
			}
			else
			{
#ifdef _WIN32
				if (luaL_dofile(Config, QString(filepaths[i]).toLocal8Bit().data()))
#elif defined(__linux__)
				if (luaL_dofile(Config, filepaths[i]))
#endif // _WIN32
				{
					QMessageBox::critical(nullptr,
						QMessageBox::tr("Lua Script Error"),
						QMessageBox::tr("File cannot Load!\nYou've got an error:\n    ")
						+ QString::fromLocal8Bit(lua_tostring(Config, -1)),
						QMessageBox::Cancel);
					lua_pop(Config, 1);
					continue;
				}
				if (lua_getglobal(Config, "MenuList") != LUA_TTABLE)
				{
					lua_pop(Config, 1);
					continue;
				}

				int len = luaL_len(Config, -1);
				for (int i = 1; i <= len; i++)
				{
					switch (lua_geti(Config, -1, i))
					{
					case LUA_TSTRING:
#ifdef _DEBUG
						qDebug() << lua_tostring(Config, -1);
#endif // _DEBUG
						append(lua_tostring(Config, -1));
						lua_pop(Config, 1);
						break;
					case LUA_TFUNCTION:
						is_dynaticScript = true;
						appendClosure(i);
						lua_pop(Config, 1);
						break;
					default:
						break;
					}
				}
				if(is_dynaticScript)
					blu.setFileName(filepaths[i]);
			}
		}
		else
			append(filepaths[i]);
	}
}

bool AudioFileManager::AudioFileManagerCreate(InputOption opt)
{
    timer.setInterval(30);
	if (create(sharedMemorySize))
	{
		if (is_dynaticScript)
		{
#ifdef _WIN32
			QString tempBluPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/.temp.blu");
#elif defined(__linux__)
			QString tempBluPath(QDir::homePath() + "/.BricMusic/.temp.blu");
#endif
			QFile(tempBluPath).remove();
			blu.copy(tempBluPath);
			blu.setFileName(tempBluPath);
			blu.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text);
		}
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
			unlock();
		}
	}
	return false;
}

AudioFileManager::~AudioFileManager()
{
	blu.close();
	blu.remove();
	timer.stop();
	if (thr)
	{
		thr->quit();
		thr->wait();
		delete thr;
	}
	lua_close(Config);
}

QByteArray AudioFileManager::luaCall(const QByteArray& str)
{
	if (str.indexOf("closure:/") == -1)
		return str;
	QByteArray ret;
	lua_getglobal(Config, "MenuList");
	if (lua_geti(Config, -1, str.mid(9).toInt()) == LUA_TFUNCTION)
	{
		if (lua_pcall(Config, 0, 1, 0) == LUA_OK)
		{
			AVFormatContext* ctx = nullptr;
			ret = QByteArray(lua_tostring(Config, -1));
			if (avformat_open_input(&ctx, ret.data(), NULL, NULL))
				ret.clear();
			else
				avformat_close_input(&ctx);
		}
		lua_pop(Config, 2);
	}
#ifdef _DEBUG
	qDebug() <<"luaGC" << lua_gc(Config, LUA_GCCOUNT);
#endif // _DEBUG
	lua_gc(Config, LUA_GCCOLLECT);
	return ret;
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
		if (blu.isOpen())
		{
			char tmp[64];
			sprintf(tmp, "table.insert(MenuList , %d , \"", i + 1);
			blu.write(QByteArray(tmp) + str + QByteArray("\")"));
		}
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

void AudioFileManager::appendClosure(int closure)
{
	lPaths.append(QByteArray("closure:/") + QByteArray::number(closure));
}

void AudioFileManager::findNextAudio(int mode)
{
	mtx.lock();
	QByteArray tmp;
	if (mode < 0)
		--current_fp_pos;
	else
		switch (mode)
		{
		case 2:
			++current_fp_pos;
			break;
		case 3:
			tmp = luaCall(path);
			while (tmp.isEmpty())
				if (++current_fp_pos < lPaths.size())
					tmp = luaCall(path = lPaths[current_fp_pos]);
				else
					current_fp_pos = 0;
#ifdef _DEBUG
			qDebug() << QString::fromLocal8Bit("下一首:") << current_fp_pos;
			qDebug() << QString::fromUtf8(tmp);
#endif // _DEBUG
			emit getPath(tmp);
			return mtx.unlock();
		case 4:
			srand(time(0));
			current_fp_pos = rand();
			break;
		default:
			break;
		}
	current_fp_pos += lPaths.size();
	current_fp_pos %= lPaths.size();
	path = lPaths[current_fp_pos];
	tmp = luaCall(path);
	while (tmp.isEmpty())
	{
		if (mode < 0)
			if (--current_fp_pos > 0)
				tmp = luaCall(path = lPaths[current_fp_pos]);
			else
				current_fp_pos = lPaths.size();
		else
			if (++current_fp_pos < lPaths.size())
				tmp = luaCall(path = lPaths[current_fp_pos]);
			else
				current_fp_pos = 0;
	}
	emit getPath(tmp);
#ifdef _DEBUG
	qDebug() << QString::fromLocal8Bit("下一首:") << current_fp_pos;
	qDebug() << QString::fromUtf8(tmp);
#endif // _DEBUG
	mtx.unlock();

}

QByteArray AudioFileManager::findFirstAudio()
{
	current_fp_pos = 0;
	QByteArray ret;
	while ((ret = luaCall(lPaths[current_fp_pos])).isEmpty())
		current_fp_pos++;
	return ret;
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
	while (current_fp_pos < lPaths.size())
		if (lPaths[current_fp_pos].indexOf("closure:/") == -1)
			break;
		else
			current_fp_pos++;
	if (current_fp_pos == lPaths.size())
	{
		emit sendFinished();
		unlock();
		timer.stop();
	}
	else
	{
		strcpy(header.get_buffer(), lPaths[current_fp_pos++].toStdString().c_str());
		unlock();
	}

}

void AudioFileManager::saveBLU(const QString& filepath)
{
	if (filepath.isEmpty())
		return;
	if (!is_dynaticScript)
	{
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
	else
		blu.copy(filepath);
}

