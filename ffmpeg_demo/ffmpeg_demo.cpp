// ffmpeg_demo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
extern "C" {
#include "SDL.h"
#include "libavformat/avformat.h"
}

#include "ffmpeg_demo.h"
#include <string>

constexpr int width = 640;
constexpr int height = 480;

struct ffmpeg_image
{
	std::string strFile;
	AVFormatContext *fmt_ctx{ nullptr };
	AVCodecContext *decoder_ctx{nullptr};
	AVCodec *decoder{nullptr};
	AVStream *stream{nullptr};
	int stream_idx;
	int cx;
	int cy;
	enum AVPixelFormat format;
};

int main(int argc, char *argv[])
{
	int ret = SDL_Init(SDL_INIT_VIDEO);
	if (ret != 0)
		return -1;

	SDL_Window *screen = SDL_CreateWindow("FFmpeg Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE);
	if (!screen)
		return -1;

	ffmpeg_image info;
	info.strFile = "123.png";
	ret = avformat_open_input(&info.fmt_ctx, info.strFile.c_str(), NULL, NULL);
	if (ret < 0)
		return -1;

	ret = avformat_find_stream_info(info.fmt_ctx, NULL);
	if (ret < 0)
		return -1;

	ret = av_find_best_stream(info.fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, 1, NULL, 0);
	if (ret < 0)
		return -1;

	info.stream_idx = ret;
	info.stream = info.fmt_ctx->streams[ret];
	info.decoder_ctx = info.stream->codec;
	info.decoder = avcodec_find_decoder(info.decoder_ctx->codec_id);
	ret = avcodec_open2(info.decoder_ctx, info.decoder, NULL);
	if (ret < 0)
		return -1;
	info.cx = info.decoder_ctx->width;
	info.cy = info.decoder_ctx->height;
	info.format = info.decoder_ctx->pix_fmt;

	while (1)
	{
		SDL_Delay(50);
	}

	return 0;
}