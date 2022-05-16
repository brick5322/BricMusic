#pragma once

#include <QObject>
#include <QString>

class AudioManager : public QObject
{
	Q_OBJECT

public:
	AudioManager(int argc,char**argv,QObject *parent);
	~AudioManager();
private:
};
