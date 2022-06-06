#pragma once

#include <QObject>
#include <functional>
#include "FIFO.h"
#include "Controller.h"

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

#define BufferSize 192000

class Decoder : public QObject {
	
	Q_OBJECT

public:
	Decoder(Controller* parent = Q_NULLPTR);
	~Decoder();
signals:
	void attachedPic(uchar* picdata,int size);
	void basicInfo(AVSampleFormat sampleFormat,	int channel_layout,	int sample_rate,double stream_duration);
	void decodeFinish();
	void decodeErr(int);
	void deformatErr(int);
public slots:
	void decode(FIFO& buffer);
	void flush(unsigned int timeStamp);
	int open(const QString& filepath);
	void close();
private:
	AVSampleFormat sampleFormat;
	int channel_layout;
	int sample_rate;
	AVFormatContext* fmt;
	AVCodecContext* ctx;
	AVCodecParameters* cdpr;
	AVStream* stream;
	SwrContext* swrCtx;
	AVPacket* picPacket;
	AVPacket* decodecPacket;
	AVFrame* decodecFrame;

	uchar* const decodeBuffer;
	uchar* decodeData;
	int sz_decodeData;
}; 
