// ffmpeg_demo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
extern "C" {
#include "SDL.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
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

static bool ffmpeg_image_reformat_frame(ffmpeg_image *info, AVFrame *frame, uint8_t *out, int lineSize)
{
	struct SwsContext *sws_ctx = nullptr;
	int ret = 0;

	if (info->format == AV_PIX_FMT_RGBA || info->format == AV_PIX_FMT_BGRA || info->format == AV_PIX_FMT_BGR0)
	{
		if (lineSize != frame->linesize[0])
		{
			int min_size = lineSize < frame->linesize[0] ? lineSize : frame->linesize[0];

			for (int y = 0; y < info->cy; ++y)
			{
				memcpy(out + y*lineSize, frame->data[0] + y * frame->linesize[0], min_size);
			}
		}
		else
		{
			memcpy(out, frame->data[0], lineSize*info->cy);
		}
	}
	else
	{
		sws_ctx = sws_getContext(info->cx, info->cy, info->format, info->cx,
			info->cy, AV_PIX_FMT_BGRA, SWS_POINT, NULL, NULL, NULL);
		if (!sws_ctx)
		{
			return false;
		}

		ret = sws_scale(sws_ctx, (const uint8_t * const *)frame->data, frame->linesize, 0, info->cy,
			&out, &lineSize);
		sws_freeContext(sws_ctx);
		if (ret < 0)
			return false;

		info->format = AV_PIX_FMT_RGBA;
		return true;
	}

	return false;
}

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

	auto pData = new uint8_t[info.cx * info.cy * 4];
	memset(pData, 0, info.cx * info.cy * 4);

	AVPacket packet = { 0 };
	bool success = false;
	AVFrame *frame = av_frame_alloc();
	int got_frame = 0;
	ret = av_read_frame(info.fmt_ctx, &packet);
	while (!got_frame)
	{
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(57, 40, 101)
		ret = avcodec_send_packet(info.decoder_ctx, &packet);
		if (0 == ret)
			ret = avcodec_receive_frame(info.decoder_ctx, frame);
		got_frame = (0 == ret);
		if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
			ret = 0;
#else
		ret = avcodec_decode_video2(info.decoder_ctx, frame, &got_frame, &packet);
#endif
		if (ret < 0)
			return -1;//goto fail;
	}

	success = ffmpeg_image_reformat_frame(&info, frame, pData, info.cx*4);

	SDL_Renderer *sdlRender = SDL_CreateRenderer(screen, -1, 0);
	auto pixFmt = SDL_PIXELFORMAT_RGBA32;
	SDL_Texture *pSdlTexture = SDL_CreateTexture(sdlRender, pixFmt, SDL_TEXTUREACCESS_STREAMING,
		info.cx, info.cy);
	

	while (1)
	{
		SDL_Rect sdlRect;
		SDL_UpdateTexture(pSdlTexture, NULL, pData, info.cx*4);
		sdlRect.x = 0;
		sdlRect.y = 0;
		sdlRect.w = info.cx;
		sdlRect.h = info.cy;
		SDL_RenderClear(sdlRender);
		SDL_RenderCopy(sdlRender, pSdlTexture, NULL, &sdlRect);
		SDL_RenderPresent(sdlRender);

		SDL_Delay(50);
	}

	av_packet_unref(&packet);
	av_frame_free(&frame);
	avcodec_close(info.decoder_ctx);
	avformat_close_input(&info.fmt_ctx);

	return 0;
}