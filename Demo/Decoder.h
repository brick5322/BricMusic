#pragma once

#include <QObject>
#include <functional>
#include "FIFO.h"

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

#define BufferSize 192000

typedef struct AudioConfigs
{
	AVSampleFormat sampleFormat;
	int channel_layout;
	int sample_rate;
}AudioConfigs;

class Decoder : public QObject {
	
	Q_OBJECT

public:
	Decoder(QObject* parent = Q_NULLPTR);
	~Decoder();
signals:
	void attachedPic(uchar* picdata,int size);
	void basicInfo(AVSampleFormat sampleFormat,	int channel_layout,	int sample_rate);
	void decodeFin();
	void decodeErr(int);
public:
	int open(const char* filepath);
public slots:
	void decode(FIFO& buffer);
	void flush(unsigned int timeStamp);
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
