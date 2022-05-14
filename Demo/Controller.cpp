#include "Controller.h"
#include <QImage>
#include <QBitmap>
#include <QPainter>


Controller::Controller(QObject *parent)
	: QTimer(parent),fifo(44100*2*4),is_finishing(false),is_paused(false),is_pausing(false)
{
}

Controller::~Controller()
{
}

void Controller::getContext(AVSampleFormat sampleFormat, int channel_layout, int sample_rate)
{
	switch (sampleFormat)
	{
	case AV_SAMPLE_FMT_U8:
		audioContext.format = AUDIO_U8;
		break;
	case AV_SAMPLE_FMT_S16:
		audioContext.format = AUDIO_S16SYS;
		break;
	case AV_SAMPLE_FMT_S32:
		audioContext.format = AUDIO_S32SYS;
		break;
	case AV_SAMPLE_FMT_FLT:
		audioContext.format = AUDIO_F32SYS;
		break;
	default:
		audioContext.format = AUDIO_S32SYS;
		break;
	}
	audioContext.freq = sample_rate;
	switch (channel_layout)
	{
	case AV_CH_LAYOUT_MONO:
		audioContext.channels = 1;
	case AV_CH_LAYOUT_STEREO:
		audioContext.channels = 2;
	default:
		audioContext.channels = 2;
		break;
	}
	int sz = sample_rate * audioContext.channels * (audioContext.format & 0x3f) >> 3;
	if (fifo.size() != sz)
		fifo = FIFO(sz);
	else
		fifo.reset();
}

void Controller::setData(unsigned char* buffer, int len)
{
	fifo[len] >> buffer;
}

void Controller::getPic(uchar* picdata, int size)
{
	if (picdata)
	{
		albumImage.loadFromData(picdata, size);
		albumImage.scaled(AlbumSZ, AlbumSZ, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
		PixmapToRound(albumImage);
	}
	else
		//albumImage.load("qrc:/img/CD.png");
		return;
}

void Controller::on_controller_timeout()
{
	if(fifo.getFreesize())
		getData(fifo);
}

void Controller::on_player_terminated()
{
	if (!is_finishing)
		return;
	is_finishing = false;
	is_paused = false;
	is_pausing = false;
	emit playTaskFinish();
	//这个函数可以写的太多了，它代表着音频播放结束最后的处理，之后会返回文件管理
}

void Controller::stop()
{
	this->is_finishing = true;
	setPausing();
	QTimer::stop();
}

QPixmap PixmapToRound(const QPixmap& src)
{
	if (src.isNull()) {
		return QPixmap();
	}
	QBitmap mask(AlbumSZ,AlbumSZ);
	QPainter painter(&mask);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.fillRect(0, 0, AlbumSZ, AlbumSZ, Qt::white);
	painter.setBrush(QColor(0, 0, 0));
	painter.drawRoundedRect(0, 0, AlbumSZ, AlbumSZ, 100, 100);
	QPixmap image = src.scaled(AlbumSZ, AlbumSZ);
	image.setMask(mask);
	return image;
}