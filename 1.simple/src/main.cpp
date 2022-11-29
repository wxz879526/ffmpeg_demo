#include "window.h"
#include "pch.h"
#include <QDebug>

extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/bprint.h"
}

void log_callback(void* avcl, int level, const char* fmt, va_list vl)
{
	AVBPrint bprint;
	av_bprint_init(&bprint, 1024, 10240);
	av_vbprintf(&bprint, fmt, vl);
}

int main(int argc, char** argv)
{
	QGuiApplication app(argc, argv);

	QSurfaceFormat fmt;
	fmt.setVersion(3, 3);
	fmt.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(fmt);

	av_log_set_callback(log_callback);

	AVFormatContext* fmt_ctx = avformat_alloc_context();
	if (!fmt_ctx)
		return -1;

	int cc = av_log_get_level();

	const int type = 1;
	if (1 == type) // mp4
	{
		AVDictionary* fmt_opts = nullptr;
		av_dict_set(&fmt_opts, "formatprobesize", "10485760", AV_DICT_MATCH_CASE);
		av_dict_set(&fmt_opts, "export_all", "1", AV_DICT_MATCH_CASE);
		av_dict_set(&fmt_opts, "export_666", "1", AV_DICT_MATCH_CASE);

		AVDictionaryEntry* t;
		int cnt1 = av_dict_count(fmt_opts);
		if ((t = av_dict_get(fmt_opts, "", nullptr, AV_DICT_IGNORE_SUFFIX)))
			av_log(nullptr, AV_LOG_INFO, "Option: key: %s, value: %s", t->key, t->value);

		int cnt2 = av_dict_count(fmt_opts);

		QString mp4FilePath = QApplication::applicationDirPath();
		QDir filePath(mp4FilePath);
		int err = avformat_open_input(&fmt_ctx, QDir::toNativeSeparators(filePath.absoluteFilePath("juren-30s.mp4")).toUtf8().data(), nullptr, &fmt_opts);
		//char szError[256]{ 0 };
		//av_strerror(err, szError, 256);
		if (err < 0)
			return -2;

		int cnt3 = av_dict_count(fmt_opts);
		if ((t = av_dict_get(fmt_opts, "", nullptr, AV_DICT_IGNORE_SUFFIX)))
			av_log(nullptr, AV_LOG_WARNING, "Option: key: %s, value: %s", t->key, t->value);

		qDebug() << "open success";
		qDebug() << "filename: " << fmt_ctx->filename;
		qDebug() << "duration: " << fmt_ctx->duration;
		qDebug() << "nb streams: " << fmt_ctx->nb_streams;
		for (int i = 0; i < fmt_ctx->nb_streams; ++i)
			qDebug() << "stream codec_type: " << fmt_ctx->streams[i]->codecpar->codec_type;
		qDebug() << "iformat name: " << fmt_ctx->iformat->name;
		qDebug() << "iformat long name: " << fmt_ctx->iformat->long_name;

		AVPacket* pkt = av_packet_alloc();
		int readError = av_read_frame(fmt_ctx, pkt);
		if (readError < 0)
			return -3;

		qDebug() << "packet media index: " << pkt->stream_index;
		qDebug() << "packet media duration: " << pkt->duration;
		qDebug() << "packet media timebase: " << fmt_ctx->streams[pkt->stream_index]->time_base.num << "/" << fmt_ctx->streams[pkt->stream_index]->time_base.den;
		qDebug() << "packet media data: " << hex << pkt->data[0] << " " << pkt->data[1] << " " << pkt->data[2] << " " << pkt->data[3];
		av_packet_free(&pkt);
	}
	else if (2 == type)
	{
		AVDictionary* fmt_opts = nullptr;
		av_dict_set(&fmt_opts, "probesize", "32", 0);

		QString flvFilePath = QApplication::applicationDirPath();
		QDir filePath(flvFilePath);
		int err = avformat_open_input(&fmt_ctx, QDir::toNativeSeparators(filePath.absoluteFilePath("juren-30s.flv")).toUtf8().data(), nullptr, &fmt_opts);
		//char szError[256]{ 0 }; 
		av_dict_free(&fmt_opts);

		if (err < 0)
			return -2;


		avformat_find_stream_info(fmt_ctx, nullptr);

		qDebug() << "open success";
		qDebug() << "filename: " << fmt_ctx->filename;
		qDebug() << "duration: " << fmt_ctx->duration;
		qDebug() << "nb streams: " << fmt_ctx->nb_streams;
		for (int i = 0; i < fmt_ctx->nb_streams; ++i)
			qDebug() << "stream codec_type: " << fmt_ctx->streams[i]->codecpar->codec_type;
		qDebug() << "iformat name: " << fmt_ctx->iformat->name;
		qDebug() << "iformat long name: " << fmt_ctx->iformat->long_name;
	}

	Window w;
	w.show();

	app.exec();

	return 0;
}