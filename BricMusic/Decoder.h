#pragma once

#include <QObject>
#include <functional>
#include "FIFO.h"

extern "C"
{
#include <SDL.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

#define BufferSize 192000

class Decoder : public QObject {
	
	Q_OBJECT

public:
	Decoder(FIFO& buffer,QObject* parent = Q_NULLPTR);
	~Decoder();
signals:
	void attachedPic(uchar* picdata,int size);
	void basicInfo(AVSampleFormat sampleFormat,	int channel_layout,	int sample_rate,double stream_duration);
	void decodeFinish();
	void decodeErr(int);
	void deformatErr(int);
public slots:
	void decode(void* mtx);
	void flush(unsigned int timeStamp);
	int open(const QByteArray& filepath);
	void close();
private:
	FIFO& buffer;
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
