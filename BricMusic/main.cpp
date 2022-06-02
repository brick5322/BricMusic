#include "BricMusic.h"
#include <QtWidgets/QApplication>
#include "Decoder.h"
#include "Player.h"
#include "Controller.h"
#include "AudioFileManager.h"
#include <QSystemTrayIcon>

extern"C"
{
#include <lualib.h>
#include <lauxlib.h>
#include <lua.h>
}

#define CONFIG_PATH "./settings.lua"

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

	lua_State* Config = luaL_newstate();
    luaL_openlibs(Config);
    luaL_dofile(Config, CONFIG_PATH);

    QColor color = get_color(Config);


    AudioFileManager manager(argc, argv);
	BricMusic w(color,manager.findFirstAudio());
    Controller& ctrler = w.controller();
    Player player(&ctrler);
    Decoder decoder(&ctrler);
	QSystemTrayIcon icon(&w);

	icon.setToolTip(QString("BricMusic"));
	icon.setIcon(QIcon(":/img/BM-LOGO/icon16.png"));
	icon.show();

	QObject::connect(&icon, &QSystemTrayIcon::activated,
		[&w](QSystemTrayIcon::ActivationReason r){
			if (r == QSystemTrayIcon::DoubleClick)
				w.show();});

	QObject::connect(&w, &BricMusic::setVolume, &player, &Player::setVolume);

	QObject::connect(&decoder, &Decoder::attachedPic, &w, &BricMusic::setPic);
	QObject::connect(&decoder, &Decoder::decodeErr, &w, &BricMusic::close);
	QObject::connect(&decoder, &Decoder::basicInfo, &ctrler, &Controller::getContext);
	QObject::connect(&decoder, &Decoder::decodeFinish, &ctrler, &Controller::stop);

	QObject::connect(&ctrler, &Controller::setDecode, &decoder, &Decoder::open);
	QObject::connect(&ctrler, &Controller::getData, &decoder, &Decoder::decode);
	QObject::connect(&ctrler, &Controller::flushDecoder, &decoder, &Decoder::flush);
	QObject::connect(&ctrler, &Controller::stopDecoder, &decoder, &Decoder::close);

	QObject::connect(&ctrler, &Controller::setContext, &player, &Player::resetContext);
	QObject::connect(&ctrler, &Controller::setPausing, &player, &Player::pause);
	QObject::connect(&ctrler, &Controller::setPlaying, &player, &Player::play);
	QObject::connect(&ctrler, &Controller::getAudioPath, &manager, &AudioFileManager::findNextAudio);

	QObject::connect(&player, &Player::playReady, &ctrler, &Controller::playTaskReady);
	QObject::connect(&player, &Player::paused, &ctrler, &Controller::on_player_paused);

	ctrler.playTaskInit();
	w.show();

    return a.exec();
}
