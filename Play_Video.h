#ifndef __PLAY_VIDEO_H__
#define __PLAY_VIDEO_H__

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <libswscale/swscale.h>
#include <Windows.h>

typedef struct {
	SDL_Window      *window;
	SDL_Renderer    *renderer;
	SDL_Texture     *texture;
	int             width;
	int             height;
} myWindow;
int playVideo(const char *filename);
void cleanup_player();

int mainloop(AVFormatContext *formatCtx, AVCodecContext *dec_ctx, myWindow *window);
int display_frame(AVFrame *pFrame, myWindow *window);
int decode(AVCodecContext *pContext, AVFrame *pFrame, int *got_frame, AVPacket *pkt);
static int open_input_file(const char *filename, AVFormatContext **formatCtx, AVCodecContext **dec_ctx);
void initialize_player();
static void initialize_ffmpeg();

unsigned __stdcall PlayVideo();

#endif