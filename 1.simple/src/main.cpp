#include "window.h"
#include <QDebug>

extern "C" {
#include "libavformat/avformat.h"
}

int main(int argc, char** argv)
{
	QGuiApplication app(argc, argv);

	QSurfaceFormat fmt;
	fmt.setVersion(3, 3);
	fmt.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(fmt);

	AVFormatContext* fmt_ctx = avformat_alloc_context();
	if (!fmt_ctx)
		return -1;

	const int type = 2;
	if (1 == type) // mp4
	{
		QString mp4FilePath = QApplication::applicationDirPath();
		QDir filePath(mp4FilePath);
		int err = avformat_open_input(&fmt_ctx, QDir::toNativeSeparators(filePath.absoluteFilePath("juren-30s.mp4")).toUtf8().data(), nullptr, nullptr);
		//char szError[256]{ 0 };
		//av_strerror(err, szError, 256);
		if (err < 0)
			return -2;

		qDebug() << "open success";
		qDebug() << "filename: " << fmt_ctx->filename;
		qDebug() << "duration: " << fmt_ctx->duration;
		qDebug() << "nb streams: " << fmt_ctx->nb_streams;
		for (int i = 0; i < fmt_ctx->nb_streams; ++i)
			qDebug() << "stream codec_type: " << fmt_ctx->streams[i]->codecpar->codec_type;
		qDebug() << "iformat name: " << fmt_ctx->iformat->name;
		qDebug() << "iformat long name: " << fmt_ctx->iformat->long_name;
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