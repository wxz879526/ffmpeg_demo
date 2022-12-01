#include "window.h"
#include "pch.h"
#include <QDebug>

extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/bprint.h"
#include "libavutil/error.h"
#include "libavutil/imgutils.h"
}

void log_callback(void* avcl, int level, const char* fmt, va_list vl)
{
	AVBPrint bprint;
	av_bprint_init(&bprint, 1024, 10240);
	av_vbprintf(&bprint, fmt, vl);
}

// API-use
//avformat_alloc_context -> avformat_open_input -> avcodec_alloc_context3 -> avcodec_parameters_to_context ->avcodec_find_decoder(
// -> avcodec_open2

// av_read_frame -> avcodec_send_packet -> avcodec_receive_frame

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);

	//av_log_set_callback(log_callback);

	QDir appDir(app.applicationDirPath());
	QString filePath = QDir::toNativeSeparators(appDir.absoluteFilePath("juren-30s.mp4"));

	AVFormatContext* fmt_ctx = avformat_alloc_context();
	if (!fmt_ctx)
	{
		qDebug() << "failed to alloc format context: " << AVERROR(ENOMEM);
		return ENOMEM;
	}
	
	int error = 0;
	if ((error = avformat_open_input(&fmt_ctx, filePath.toUtf8().data(), nullptr, nullptr)) < 0)
	{
		char szError[100]{ 0 };
		av_strerror(error, szError, 100);
		qDebug() << "can not open file: " << szError;
		return error;
	}

	int type = 1;
	if (type == 1)
	{
		AVCodecContext* avCtx = avcodec_alloc_context3(nullptr);
		int ret = avcodec_parameters_to_context(avCtx, fmt_ctx->streams[0]->codecpar);
		if (ret < 0)
		{
			char szError[100]{ 0 };
			av_strerror(error, szError, 100);
			qDebug() << "avcodec_parameters_to_context: " << szError;
			return error;
		}

		AVCodec* codec = avcodec_find_decoder(avCtx->codec_id);
		if ((ret = avcodec_open2(avCtx, codec, nullptr)) < 0)
		{
			char szError[100]{ 0 };
			av_strerror(error, szError, 100);
			qDebug() << "can not open decoder context: " << szError;
			return error;
		}

		AVPacket* pkt = av_packet_alloc();
		AVFrame* frame = av_frame_alloc();
		int frameNum = 0;
		int read_end = 0;
		for (;;)
		{
			if (1 == read_end)
				break;

			ret = av_read_frame(fmt_ctx, pkt);
			if (pkt->stream_index == AVMEDIA_TYPE_AUDIO)
			{
				av_packet_unref(pkt);
				continue;
			}

			if (AVERROR_EOF == ret)
			{
				avcodec_send_packet(avCtx, pkt);
				av_packet_unref(pkt);

				// try decode frame
				for (;;)
				{
					ret = avcodec_receive_frame(avCtx, frame);
					if (AVERROR(EAGAIN) == ret)
					{
						// 提示EAGAIN 代表解码器需要更多AVPacket
						break;
					}
					else if (AVERROR_EOF == ret)
					{
						// AVERROR_EOF 代表之前已经往编码器发送了一个data和size都是null的AVPacket
						// 发送null的AVPacket是提示编码器把所有缓存帧全部都刷出来
						// 通常只有在读完输入文件才会发送null的AVPacket 或者需要用现有的解码器解码另一个的视频流才会这么干
						read_end = 1;
						break;
					}
					else if (ret >= 0)
					{
						// 解码出一帧YUV数据 打印出一些信息
						qDebug() << "decode success ---------";
						qDebug() << QString("width: %1, height: %2").arg(frame->width).arg(frame->height);
						qDebug() << QString("pts: %1, duration: %2").arg(frame->pts).arg(frame->pkt_duration);
						qDebug() << "format: " << frame->format;
						qDebug() << "keyframe: " << frame->key_frame;
						qDebug() << "avpicture type: " << frame->pict_type;
						int num = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, 1920, 1080, 1);
						qDebug() << "num: " << num;
					}
					else
					{
						qDebug() << "othe failed";
						return ret;
					}
				}
			}
			else if (0 == ret)
			{
			retry:
				if (avcodec_send_packet(avCtx, pkt) == AVERROR(EAGAIN))
				{
					qDebug() << "Receive_frame and send_packet both return EAGAIN, which is an API violation.";
					// 考虑休眠下重试
					goto retry;
				}
				else
				{
					av_packet_unref(pkt);
					for (;;)
					{
						ret = avcodec_receive_frame(avCtx, frame);
						if (AVERROR(EAGAIN) == ret)
						{
							// 提示EAGAIN 代表解码器需要更多AVPacket
							break;
						}
						else if (AVERROR_EOF == ret)
						{
							// AVERROR_EOF 代表之前已经往编码器发送了一个data和size都是null的AVPacket
							// 发送null的AVPacket是提示编码器把所有缓存帧全部都刷出来
							// 通常只有在读完输入文件才会发送null的AVPacket 或者需要用现有的解码器解码另一个的视频流才会这么干
							read_end = 1;
							break;
						}
						else if (ret >= 0)
						{
							// 解码出一帧YUV数据 打印出一些信息
							qDebug() << "decode success ---------";
							qDebug() << QString("width: %1, height: %2").arg(frame->width).arg(frame->height);
							qDebug() << QString("pts: %1, duration: %2").arg(frame->pts).arg(frame->pkt_duration);
							qDebug() << "format: " << frame->format; // AVPixelFormat-AV_PIX_FMT_YUV420P
							qDebug() << "keyframe: " << frame->key_frame; 
							qDebug() << "avpicture type: " << frame->pict_type; //AVPictureType-AV_PICTURE_TYPE_I...
							int num = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, 1920, 1080, 1);
							qDebug() << "num: " << num;
							qDebug() << "Y size: " << frame->linesize[0];
							qDebug() << "U size: " << frame->linesize[1];
							qDebug() << "V size: " << frame->linesize[2];

							for (int i = 0; i < 5; ++i)
							{
								frame->data[0][i] = 0x00; // modify y
								//frame->data[0][i+1] = 0x00; // modify y
								//frame->data[0][i+2] = 0x00; // modify y
							}

							QFile file(QDir(app.applicationDirPath()).absoluteFilePath(QString("yuv_%1").arg(frameNum++)));
							file.open(QFile::WriteOnly);
							QDataStream in(&file);
							in.writeRawData((const char*)frame->data[0], frame->linesize[0] * frame->height);
							in.writeRawData((const char*)frame->data[1], frame->linesize[1] * frame->height / 2);
							in.writeRawData((const char*)frame->data[2], frame->linesize[2] * frame->height / 2);
							file.close();
						}
						else
						{
							qDebug() << "othe failed";
							return ret;
						}
					}
				}
			}
		}

		av_frame_free(&frame);
		av_packet_free(&pkt);
		avcodec_close(avCtx);
	}

	app.exec();

	return 0;
}