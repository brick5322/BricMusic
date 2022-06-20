#include "BricMusic.h"
#include <QtWidgets/QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QTranslator>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMenu>
#include <QString>
#include <QFile>
#include <QDir>
#include "Decoder.h"
#include "Player.h"
#include "Controller.h"
#include "AudioFileManager.h"
#include "ArgSplitter.h"
#include <vld.h>

#ifdef _DEBUG
#include <QDebug>
#endif // _DEBUG


extern"C"
{
#include <lualib.h>
#include <lauxlib.h>
#include <lua.h>
}

QColor get_color(lua_State* L)
{
    QColor ret;

    if (lua_getglobal(L, "ThemeColor") != LUA_TTABLE)
        return QColor("#E799B0");
    lua_pushstring(L, "Red");
    if (lua_gettable(L, -2) != LUA_TNUMBER)
        return QColor("#E799B0");
    else
        ret.setRed(lua_tointeger(L, -1));
    lua_pop(L, 1);
    lua_pushstring(L, "Green");
    if (lua_gettable(L, -2) != LUA_TNUMBER)
        return QColor("#E799B0");
    else
        ret.setGreen(lua_tointeger(L, -1));
    lua_pop(L, 1);
    lua_pushstring(L, "Blue");
    if (lua_gettable(L, -2) != LUA_TNUMBER)
        return QColor("#E799B0");
    else
        ret.setBlue(lua_tointeger(L, -1));
    lua_pop(L, 1);
    return ret;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;
#ifdef _WIN32
    QString luaScriptPath = QCoreApplication::applicationDirPath() + QString("/settings.lua");
    QString languagePath = QCoreApplication::applicationDirPath() + QString("/translations/default.qm");
    QString qssPath = QCoreApplication::applicationDirPath() + QString("/qss/BricMusic.qss");
#elif defined(__linux__)
    QString luaScriptPath = QDir::homePath() + QString("/.BricMusic/settings.lua");
    QString languagePath = QDir::homePath() + QString("/.BricMusic/translations/default.qm");
    QString qssPath = QDir::homePath() + QString("/.BricMusic/qss/BricMusic.qss");
#endif // _WIN32
#ifdef _DEBUG
    qDebug() << luaScriptPath.toLocal8Bit().data();
    qDebug() << languagePath;
    qDebug() << qssPath;
#endif
    translator.load(languagePath);
    a.installTranslator(&translator);
	lua_State* Config = luaL_newstate();
    luaL_openlibs(Config);
    if (luaL_dofile(Config, luaScriptPath.toLocal8Bit().data()))
    {
        QMessageBox::critical(nullptr,
            QMessageBox::tr("Lua File Err!"),
            QMessageBox::tr("You might have a Lua script with an error or you might even have deleted the script.\nAnyway,see Readme.md(Windows) or try \"man BricMusic\"(Linux) to find solution."), QMessageBox::Cancel);
        return 0;
    }
#ifdef _DEBUG
    qDebug() << "setSpliter";
#endif // _DEBUG
    ArgSplitter splitter(argc, argv);
    splitter.setLongOptions(3, "pause", "prev", "next");
    splitter.parase();
    int len = splitter.argc();
#ifdef _DEBUG
    qDebug() << "Spliter setted"<<len;
#endif // _DEBUG
    AudioFileManager manager(splitter.argc(),splitter.argv());

    AudioFileManager::InputOption opt = AudioFileManager::Default;

    if (splitter.hasLongOption(0))
        opt = AudioFileManager::Pause;
    else if (splitter.hasLongOption(1))
        opt = AudioFileManager::Prev;
    else if (splitter.hasLongOption(2))
        opt = AudioFileManager::Next;
#ifdef _DEBUG
    qDebug() << "load_options" << opt;
#endif // _DEBUG

    if (!manager.AudioFileManagerCreate(opt))
    {
        if (opt != AudioFileManager::Default)
            return 0;
        if (argc == 1)
            return 0;
        QObject::connect(&manager, &AudioFileManager::sendFinished, &a, &QCoreApplication::quit);
        return a.exec();
    }
    else if(!manager.size())
    {
        if (lua_getglobal(Config, "AudioPaths") != LUA_TTABLE)
            lua_close(Config);
        else
        {
            int i = 0;
            while (true) {
                lua_pushnumber(Config, ++i);
                if (lua_gettable(Config, -2) != LUA_TSTRING)
                    break;
                else
                {
                    manager.append(lua_tostring(Config, -1));
                    lua_pop(Config, 1);
                }
            }
        }
    }
    if (!manager.size())
    {
        QMessageBox::critical(nullptr,
            QMessageBox::tr("Load Err!"),
            QMessageBox::tr("You load NOTHING!!!!\nAnyway,see Readme.md(Windows) or try \"man BricMusic\"(Linux) to find solution."), QMessageBox::Cancel);
        return 0;
    }
    QColor color = get_color(Config);
    if (lua_getglobal(Config, "LanguagePath") == LUA_TSTRING)
    {
        translator.load(lua_tostring(Config, -1));
        lua_pop(Config, 1);
        a.installTranslator(&translator);
    }

    lua_close(Config);

    Controller ctrler;
    Player player(std::bind(&Controller::setData, &ctrler, std::placeholders::_1, std::placeholders::_2));
    //Decoder decoder;

    Decoder decoder(ctrler.buffer());
    QThread dec_thr;
    decoder.moveToThread(&dec_thr);
    dec_thr.start();

	BricMusic w(ctrler,color);
    QSystemTrayIcon icon(&w);

	icon.setToolTip(QString("BricMusic"));
	icon.setIcon(QIcon(":/img/BM-LOGO/icon16.png"));
	icon.show();

    QMenu menu;

    QAction Show;
    QAction Exit;
    QAction Save;

    Show.setText(QAction::tr("Show"));
    Save.setText(QAction::tr("Save"));
    Exit.setText(QAction::tr("Exit"));

    menu.addAction(&Show);
    menu.addAction(&Save);
    menu.addAction(&Exit);

	QFile file(qssPath);
	file.open(QFile::ReadOnly);
	menu.setStyleSheet(file.readAll());
	file.close();

	icon.setContextMenu(&menu);

	QObject::connect(&icon, &QSystemTrayIcon::activated,
		[&w](QSystemTrayIcon::ActivationReason r){
			if (r == QSystemTrayIcon::DoubleClick)
				w.show();});

	QObject::connect(&w, &BricMusic::setVolume, &player, &Player::setVolume);
    QObject::connect(&manager, &AudioFileManager::newProcesstask, &ctrler, &Controller::getNextAudio);
    QObject::connect(&manager, &AudioFileManager::processSetPause, &w, &BricMusic::on_albumslider_clicked);
    QObject::connect(&manager, &AudioFileManager::processSetPrev, &ctrler, &Controller::getPrevAudio);
    QObject::connect(&manager, &AudioFileManager::processSetNext, &ctrler, &Controller::getNextAudio);
    QObject::connect(&manager, &AudioFileManager::getPath, &ctrler, &Controller::setNextPath);

	QObject::connect(&decoder, &Decoder::attachedPic, &w, &BricMusic::setPic);
	QObject::connect(&decoder, &Decoder::deformatErr, &decoder, &Decoder::close);
	QObject::connect(&decoder, &Decoder::decodeErr, &decoder, &Decoder::close);
	QObject::connect(&decoder, &Decoder::basicInfo, &ctrler, &Controller::getContext);
	QObject::connect(&decoder, &Decoder::decodeFinish, &ctrler, &Controller::playTaskStop);

	QObject::connect(&ctrler, &Controller::setDecode, &decoder, &Decoder::open);
	QObject::connect(&ctrler, &Controller::getData, &decoder, &Decoder::decode);
	QObject::connect(&ctrler, &Controller::flushDecoder, &decoder, &Decoder::flush);
	QObject::connect(&ctrler, &Controller::stopDecoder, &decoder, &Decoder::close);

	QObject::connect(&ctrler, &Controller::setContext, &player, &Player::resetContext);
	QObject::connect(&ctrler, &Controller::setPausing, &player, &Player::pause);
	QObject::connect(&ctrler, &Controller::setPlaying, &player, &Player::play);
	QObject::connect(&ctrler, &Controller::getAudioPath, &manager, &AudioFileManager::findNextAudio);

	QObject::connect(&player, &Player::playReady, &ctrler, &Controller::playTaskReady);
	QObject::connect(&player, &Player::terminated, &ctrler, &Controller::on_player_terminated);
    QObject::connect(&Exit, &QAction::triggered, &player,&Player::terminate);
    QObject::connect(&Exit, &QAction::triggered, &w,&BricMusic::close);
    QObject::connect(&Show, &QAction::triggered, &w,&BricMusic::show);
    QObject::connect(&Save, &QAction::triggered, [&](){
            manager.saveBLU(
                QFileDialog::getSaveFileName(
                    &w,
                    QFileDialog::tr("save .blu MenuList"),
                    QStandardPaths::writableLocation(QStandardPaths::MusicLocation)+ QFileDialog::tr("/new MenuList.blu"),
                    QFileDialog::tr("MenuList (*.blu)")
                ));
        });
    QObject::connect(&Exit, &QAction::triggered, [&]() {dec_thr.quit(); dec_thr.wait(); a.quit(); });
    //QObject::connect(&Exit, &QAction::triggered, &a, &QApplication::quit);

    ctrler.setDecode(manager.findFirstAudio());
    QRect s = QApplication::desktop()->screenGeometry();
    w.move(s.width() - w.width(), s.height() - w.height());
	w.show();
    return a.exec();
}   
