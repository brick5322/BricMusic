#include "Decoder.h"

#ifdef _DEBUG
#include <QDebug>
#include <QTime>
#endif

Decoder::Decoder(FIFO& buffer,QObject* parent) :QObject(parent),buffer(buffer),
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

int Decoder::open(const QByteArray& filepath)
{
	int err = 0;
#ifdef _DEBUG
	qDebug() <<"open" << filepath;
	qDebug() <<"open" << filepath.data();
#endif // DEBUG
	if (err = avformat_open_input(&fmt, filepath.data(), NULL, NULL)) {
		emit deformatErr(err);
		return err;
	}
	if ((err = avformat_find_stream_info(fmt, NULL)) < 0)	{
		emit deformatErr(err);
		return err;
	}
#ifdef _DEBUG
	av_dump_format(fmt, 0, filepath.data(), 0);
#endif
	for (int i = 0; i < fmt->nb_streams; i++)
		if (fmt->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
			stream = fmt->streams[i];
		else if (fmt->streams[i]->codecpar->codec_type != AVMEDIA_TYPE_VIDEO)
			continue;
		else if (picPacket = av_packet_clone(&fmt->streams[i]->attached_pic))
			if (!(picPacket->size)){
				av_packet_unref(picPacket);
				picPacket = NULL;
			}
	cdpr = stream->codecpar;
	if ((err = avcodec_parameters_to_context(ctx, cdpr)) < 0)	{
		emit deformatErr(err);
		return err;
	}

	if (avcodec_open2(ctx, avcodec_find_decoder(cdpr->codec_id), NULL))	{
		emit deformatErr(err);
		return err;
	}

	channel_layout = av_get_default_channel_layout(cdpr->channels);
	sampleFormat = AV_SAMPLE_FMT_S32;
	sample_rate = cdpr->sample_rate;
	if (picPacket)
		emit attachedPic(picPacket->data, picPacket->size);
	else
		emit attachedPic(nullptr, 0);

	switch (cdpr->format)
	{

	case AV_SAMPLE_FMT_U8:
	case AV_SAMPLE_FMT_S16:
	case AV_SAMPLE_FMT_S32:
		sampleFormat = (AVSampleFormat)cdpr->format;
		swr_free(&swrCtx);
		swrCtx = NULL;
		break;
	case AV_SAMPLE_FMT_FLT:
	case AV_SAMPLE_FMT_DBL:
		swrCtx = swr_alloc_set_opts(swrCtx,
			channel_layout, sampleFormat, sample_rate,
			channel_layout, (AVSampleFormat)cdpr->format, sample_rate,
			NULL, NULL);
		swr_init(swrCtx);
	break;
	case AV_SAMPLE_FMT_U8P:
	case AV_SAMPLE_FMT_S16P:
	case AV_SAMPLE_FMT_S32P:
		sampleFormat = (AVSampleFormat)(cdpr->format - 5);
	case AV_SAMPLE_FMT_FLTP:
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
	if(stream->start_time!= AV_NOPTS_VALUE)
		av_seek_frame(fmt, stream->index, (int64_t)stream->start_time, AVSEEK_FLAG_FRAME);
#ifdef _DEBUG
	qDebug() << QTime::currentTime() << "emit basicInfo";
#endif
	if(stream->start_time && stream->start_time != AV_NOPTS_VALUE)
		emit basicInfo(sampleFormat, channel_layout, sample_rate, (double)(stream->duration - stream->start_time) * stream->time_base.num / stream->time_base.den);
	else
		emit basicInfo(sampleFormat, channel_layout, sample_rate, (double)(stream->duration) * stream->time_base.num / stream->time_base.den);
	return err;
}

void Decoder::close()
{
	emit decoderClose();
	av_packet_free(&picPacket);
	avcodec_close(ctx);
	avformat_close_input(&fmt);
#ifdef _DEBUG
	qDebug() << QTime::currentTime() << "close context";
#endif
}

void Decoder::flush(unsigned int timeStamp)
{
	av_seek_frame(fmt, stream->index, (int64_t)timeStamp/stream->time_base.num* stream->time_base.den/sample_rate, AVSEEK_FLAG_FRAME);
}

void Decoder::decode(void* mtx) {
	int err = 0;
	int sz = buffer.freesize();
	if (!sz)
		return;
	if (!sz_decodeData)
		while (true)
		{
			av_packet_unref(decodecPacket);
			if (!fmt)
				return;
			if (err = av_read_frame(fmt, decodecPacket))
			{
				emit decodeFinish();
				close();
				return;
			}
			else
			{
				if (decodecPacket->stream_index != stream->index)
					continue;
				if (err = avcodec_send_packet(ctx, decodecPacket))
					goto errRet;
				switch (avcodec_receive_frame(ctx, decodecFrame))
				{
				case 0:
					break;
				case AVERROR(EAGAIN):
					return;
				default:
					goto errRet;
				}


				if (swrCtx)
				{
					sz_decodeData =
						swr_convert(swrCtx, &decodeData, BufferSize, (const uint8_t**)decodecFrame->data, decodecFrame->nb_samples)
						* av_get_bytes_per_sample(sampleFormat)
						* cdpr->channels;
					return;
				}
				else if (decodecFrame->linesize[0] > sz)
				{
					SDL_LockMutex(static_cast<SDL_mutex*>(mtx));
					buffer[sz] << decodecFrame->data[0];
					SDL_UnlockMutex(static_cast<SDL_mutex*>(mtx));
					memcpy(decodeData,decodecFrame->data[0] + sz, decodecFrame->linesize[0] - sz);
					sz_decodeData = decodecFrame->linesize[0] - sz;
					return;
				}
				else 
				{
					SDL_LockMutex(static_cast<SDL_mutex*>(mtx));
					buffer[decodecFrame->linesize[0]] << decodecFrame->data[0];
					SDL_UnlockMutex(static_cast<SDL_mutex*>(mtx));

					return;
				}
			}
		}

	if (sz <= sz_decodeData)
	{
		SDL_LockMutex(static_cast<SDL_mutex*>(mtx));
		buffer[sz] << decodeData;
		SDL_UnlockMutex(static_cast<SDL_mutex*>(mtx));

		(sz_decodeData -= sz)?decodeData += sz: decodeData = decodeBuffer ;
	}
	else if(sz_decodeData)
	{
		SDL_LockMutex(static_cast<SDL_mutex*>(mtx));
		buffer[sz_decodeData] << decodeData;
		SDL_UnlockMutex(static_cast<SDL_mutex*>(mtx));

		decodeData = decodeBuffer;
		sz_decodeData = 0;
	}
	return;
errRet:
	emit decodeErr(err);
	emit decodeFinish();
	close();
}
