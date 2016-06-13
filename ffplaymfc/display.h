#ifndef DISPLAY_H
#define DISPLAY_H

extern "C"
{
#include "libavutil/avstring.h"
#include "libavutil/colorspace.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/avassert.h"
#include "libavutil/time.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavcodec/avfft.h"
#include "libswresample/swresample.h"

#include "SDL/SDL.h"
}


#define USE_SDL1 0

typedef void* HDisplayWindow;
typedef void* HTexture;

class CDisplay
{
public:
	CDisplay();
	virtual ~CDisplay();

public:
	int Init();
	int Release();
	void GetScreenSize(int *pnWidth, int *pnHeight);

	HDisplayWindow CreateDisplayWindow(int nWidth, int nHeight, int bFullScreen);
	int DestroyDisplayWindow();
	void GetWindowSize(int *pnWidth, int *pnHeight);
	int FillWindow(int x, int y, int w, int h, int color);
	int FillWindow(int x, int y, int w, int h, int r, int g, int b);
	int UpdateRect(int x, int y, int w, int h);

	HTexture CreateTexture(int w, int h);
	int DestroyTexture(HTexture hTexture);
	int UpdateTexture(HTexture hTexture, AVPixelFormat format, unsigned char *data[], int linesize[]);
	int DisplayTexture(HTexture hTexture, SDL_Rect rect);

protected:

private:

#if USE_SDL1
	SDL_Surface *_screen;
	int _nWidth, _nHeight;
	int _bFullScreen;
#else
	SDL_Window *_screen;
	int _nWidth, _nHeight;
	int _bFullScreen;

	SDL_Renderer* _sdlRenderer;
#endif

	struct SwsContext *_img_convert_ctx;
};

extern CDisplay g_disp;

#endif // DISPLAY_H
