#include "Test.h"

Test::Test(int interval, QObject* parent): QTimer(parent),interval(interval),buffer(nullptr),volume(100)
{
}

Test::~Test()
{
	this->stop();
	delete buffer;
}

void Test::setContext(AVSampleFormat sampleFormat, int channel_layout, int sample_rate) 
{ 
	context.freq = sample_rate;

	switch (sampleFormat)
	{
	case AV_SAMPLE_FMT_NONE:
		break;
	case AV_SAMPLE_FMT_U8:
		context.format = AUDIO_U8;
		break;
	case AV_SAMPLE_FMT_S16:
		context.format = AUDIO_S16SYS;
		break;
	case AV_SAMPLE_FMT_S32:
		context.format = AUDIO_S32SYS;
		break;
	case AV_SAMPLE_FMT_FLT:
		context.format = AUDIO_F32SYS;
		break;
	default:
		context.format = AUDIO_S32SYS;
		break;
	}
	context.samples = 4096;
	switch (channel_layout)
	{
	case AV_CH_LAYOUT_MONO:
		context.channels = 1;
		break;
	case AV_CH_LAYOUT_STEREO:
	default:
		context.channels = 2;
		break;
	}
	delete buffer;
	buffer = new FIFO(44100*2*2);
	emit resetContext(context);

}

void Test::decodecFin()
{

}
