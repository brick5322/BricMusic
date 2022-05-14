#include "Decoder.h"

struct test_saver {
	FILE* fp;
	test_saver() {
		fp = fopen("out2.pcm", "wb");
	}

	~test_saver() {
		fclose(fp);
	}
	void write(uchar* buf, int sz)
	{
		//printf("out2 write:%d\n", sz);
		//fwrite(buf, sz, 1, fp);
	}
}saver2;


Decoder::Decoder( QObject* parent) :QObject(parent),
fmt(avformat_alloc_context()), cdpr(NULL),
stream(NULL), ctx(avcodec_alloc_context3(NULL)),
swrCtx(swr_alloc()), picPacket(NULL),decodeBuffer(new uchar[BufferSize]), decodeData(decodeBuffer),sz_decodeData(0),
decodecPacket(av_packet_alloc()), decodecFrame(av_frame_alloc()),channel_layout(0),sampleFormat(AV_SAMPLE_FMT_S32),sample_rate(44100)
{
	if (!fmt || !ctx || !swrCtx || !decodecPacket || !decodecFrame)
		goto errAllocRet;
	return;
errAllocRet:
	avformat_free_context(fmt);
	avcodec_free_context(&ctx);
	av_packet_free(&picPacket);
	av_packet_free(&decodecPacket);
	av_frame_free(&decodecFrame);
	swr_free(&swrCtx);
	delete decodeBuffer;
}

Decoder::~Decoder()
{
	avformat_free_context(fmt);
	avcodec_free_context(&ctx);
	av_packet_free(&picPacket);
	av_packet_free(&decodecPacket);
	swr_free(&swrCtx);
	delete decodeBuffer;
}

int Decoder::open(const char* filepath)
{
	int err = 0;
	if (err = avformat_open_input(&fmt, filepath, NULL, NULL))	{
		emit decodeErr(err);
		return err;
	}
	if ((err = avformat_find_stream_info(fmt, NULL)) < 0)	{
		emit decodeErr(err);
		return err;
	}
#ifdef _DEBUG
	av_dump_format(fmt, 0, filepath, 0);
#endif
	for (int i = 0; i < fmt->nb_streams; i++)
		if (fmt->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
			stream = fmt->streams[i];
		else if (fmt->streams[i]->codecpar->codec_type != AVMEDIA_TYPE_VIDEO)
			continue;
		else if (!(picPacket = av_packet_clone(&fmt->streams[i]->attached_pic)))
			continue;
	cdpr = stream->codecpar;
	if ((err = avcodec_parameters_to_context(ctx, cdpr)) < 0)	{
		emit decodeErr(err);
		return err;
	}

	if (avcodec_open2(ctx, avcodec_find_decoder(cdpr->codec_id), NULL))	{
		emit decodeErr(err);
		return err;
	}

	channel_layout = av_get_default_channel_layout(cdpr->channels);
	sampleFormat = AV_SAMPLE_FMT_S32;
	sample_rate = cdpr->sample_rate;
	if (picPacket)
		emit attachedPic(picPacket->data, picPacket->size);
	else
		emit attachedPic(nullptr, 0);
	emit basicInfo(sampleFormat, channel_layout, sample_rate);

	switch (cdpr->format)
	{

	case AV_SAMPLE_FMT_U8:
	case AV_SAMPLE_FMT_S16:
	case AV_SAMPLE_FMT_S32:
	case AV_SAMPLE_FMT_FLT:
		swr_free(&swrCtx);
		swrCtx = NULL;
		break;
	case AV_SAMPLE_FMT_DBL:
		sampleFormat = AV_SAMPLE_FMT_FLT;
	case AV_SAMPLE_FMT_U8P:
	case AV_SAMPLE_FMT_S16P:
	case AV_SAMPLE_FMT_S32P:
	case AV_SAMPLE_FMT_FLTP:
		sampleFormat = (AVSampleFormat)(cdpr->format - 5);
	case AV_SAMPLE_FMT_DBLP:
	case AV_SAMPLE_FMT_S64:
	case AV_SAMPLE_FMT_S64P:
		swrCtx = swr_alloc_set_opts(swrCtx,
			channel_layout, sampleFormat, sample_rate,
			channel_layout, (AVSampleFormat)cdpr->format, sample_rate,
			NULL, NULL);
		swr_init(swrCtx);
		break;
	case AV_SAMPLE_FMT_NB:
		break;
	default:
		break;
	}
	return err;
}

void Decoder::flush(unsigned int timeStamp)
{
	av_seek_frame(fmt, stream->index, (int64_t)timeStamp * stream->time_base.den/stream->time_base.num, AVSEEK_FLAG_BACKWARD);
}

void Decoder::decode(FIFO& buffer) {
	int err = 0;
	int sz = buffer.getFreesize();
	if (!sz)
		return;
	if (!sz_decodeData)
		while (true)
		{
			av_packet_unref(decodecPacket);
			if (av_read_frame(fmt, decodecPacket))
				emit decodeFin();
			else
			{
				if (decodecPacket->stream_index != stream->index)
					continue;
				if (err = avcodec_send_packet(ctx, decodecPacket))
					goto errRet;
				if (err = avcodec_receive_frame(ctx, decodecFrame))
					goto errRet;

				if (swrCtx)
				{
					swr_convert(swrCtx, &decodeData, BufferSize, (const uint8_t**)decodecFrame->data, decodecFrame->nb_samples);
					sz_decodeData = cdpr->channels * cdpr->frame_size * av_get_bytes_per_sample(sampleFormat);
					return;
				}
				else if (decodecFrame->linesize[0] > sz)
				{
					buffer[sz] << decodecFrame->data[0];
					saver2.write(decodecFrame->data[0], sz);
					memcpy(decodeData,decodecFrame->data[0] + sz, decodecFrame->linesize[0] - sz);
					sz_decodeData = decodecFrame->linesize[0] - sz;
					return;
				}
				else 
				{
					buffer[decodecFrame->linesize[0]] << decodecFrame->data[0];
					saver2.write(decodecFrame->data[0], decodecFrame->linesize[0]);
					return;
				}
			}
		}

	if (sz <= sz_decodeData)
	{
		buffer[sz] << decodeData;
		saver2.write(decodeData, sz);

		(sz_decodeData -= sz)?decodeData += sz: decodeData = decodeBuffer ;
	}
	else if(sz_decodeData)
	{
		buffer[sz_decodeData] << decodeData;
		saver2.write(decodeData, sz_decodeData);

		decodeData = decodeBuffer;
		sz_decodeData = 0;
	}
	return;
errRet:
	emit decodeErr(err);
}
