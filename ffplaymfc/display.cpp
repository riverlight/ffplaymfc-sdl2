#include "stdafx.h"

#include "displayDef.h"
#include "display.h"

CDisplay g_disp;

CDisplay::CDisplay()
{
#if USE_SDL1
	_screen = NULL;
	_nWidth = -1;
	_nHeight = -1;
	_bFullScreen = 0;
#else
	_screen = NULL;
	_nWidth = -1;
	_nHeight = -1;
	_bFullScreen = 0;

#endif

	_img_convert_ctx = NULL;
}

CDisplay::~CDisplay()
{

}

int CDisplay::Init()
{
#if USE_SDL1
	int flags;

	flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;
	char sdl_var[128];
	//sprintf(sdl_var, "SDL_WINDOWID=0x%lx", hWnd );//主窗口句柄 
	SDL_putenv(sdl_var);
	char *myvalue = SDL_getenv("SDL_WINDOWID");
	if (SDL_Init(flags)) {
		AfxMessageBox(_T("Could not initialize SDL "));
		exit(1);
	}
#else
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		TRACE("Could not initialize SDL - %s\n", SDL_GetError());
		return 0;
	}
#endif
	
	return 1;
}

int CDisplay::Release()
{
	if (_img_convert_ctx)
		sws_freeContext(_img_convert_ctx);
	return 1;
}

void CDisplay::GetScreenSize(int *pnWidth, int *pnHeight)
{
#if USE_SDL1
	const SDL_VideoInfo *vi = SDL_GetVideoInfo();
	*pnWidth = vi->current_w;
	*pnHeight = vi->current_h;
#else
	*pnWidth = GetSystemMetrics(SM_CXSCREEN);//屏幕宽度
	*pnHeight = GetSystemMetrics(SM_CYSCREEN);//屏幕高度
#endif

	return;
}

HDisplayWindow CDisplay::CreateDisplayWindow(int nWidth, int nHeight, int bFullScreen)
{
#if USE_SDL1
	if (_screen != NULL && _nWidth == nWidth && _nHeight == nHeight && _bFullScreen == bFullScreen)
		return _screen;

	int flags = SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL;
	if (bFullScreen) flags |= SDL_FULLSCREEN;
	else                flags |= SDL_RESIZABLE;

	SDL_Surface *screen = SDL_SetVideoMode(nWidth, nHeight, 0, flags);
	if (screen == NULL)
		AfxMessageBox(_T("SDL: could not set video mode - exiting"));

	//注意：设置视频窗口标题！
	char *window_title = "Video Window";
	SDL_WM_SetCaption(window_title, window_title);

	_screen = screen;
	_nWidth = nWidth;
	_nHeight = nHeight;
	_bFullScreen = bFullScreen;
#else
	if (_screen != NULL && _nWidth == nWidth && _nHeight == nHeight && _bFullScreen == bFullScreen)
		return _screen;

	SDL_Window *screen = SDL_CreateWindow("Video Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, nWidth, nHeight, SDL_WINDOW_OPENGL);
	_sdlRenderer = SDL_CreateRenderer(screen, -1, 0);

	_screen = screen;
	_nWidth = nWidth;
	_nHeight = nHeight;
	_bFullScreen = bFullScreen;
#endif

	return screen;
}

int CDisplay::DestroyDisplayWindow()
{
#if USE_SDL1
	_screen = NULL;
	_nWidth = -1;
	_nHeight = -1;
#else
	if (_sdlRenderer != NULL)
	{
		SDL_DestroyRenderer(_sdlRenderer);
		_sdlRenderer = NULL;
	}

	if (_screen != NULL)
	{
		SDL_DestroyWindow(_screen);
		_screen = NULL;
	}
#endif

	return 1;
}

void CDisplay::GetWindowSize(int *pnWidth, int *pnHeight)
{
#if USE_SDL1
	*pnWidth = _screen->w;
	*pnHeight = _screen->h;
#else
	SDL_GetWindowSize(_screen, pnWidth, pnHeight);
#endif
}

int CDisplay::FillWindow(int x, int y, int w, int h, int color)
{
#if USE_SDL1
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	SDL_FillRect(_screen, &rect, color);
#else
#endif

	return 1;
}

int CDisplay::FillWindow(int x, int y, int w, int h, int r, int g, int b)
{
#if USE_SDL1
	int color = SDL_MapRGB(_screen->format, r, g, b);
	FillWindow(x, y, w, h, color);
#else
#endif

	return 1;
}

int CDisplay::UpdateRect(int x, int y, int w, int h)
{
#if USE_SDL1
	SDL_UpdateRect(_screen, x, y, w, h);
#else
#endif

	return 1;
}

HTexture CDisplay::CreateTexture(int w, int h)
{
#if USE_SDL1
	 SDL_Overlay *pOverlay = SDL_CreateYUVOverlay(w, h, SDL_YV12_OVERLAY, _screen);
	 return pOverlay;
#else
	TextureYUV *pTextureYUV = new TextureYUV;
	pTextureYUV->pSDLTexture = SDL_CreateTexture(_sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, w, h);
	if (pTextureYUV->pSDLTexture == NULL)
		return NULL;

	pTextureYUV->w = w;
	pTextureYUV->h = h;

	pTextureYUV->pFrameYUV = av_frame_alloc();
	pTextureYUV->out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, _nWidth, _nHeight, 1));
	av_image_fill_arrays(pTextureYUV->pFrameYUV->data, pTextureYUV->pFrameYUV->linesize, pTextureYUV->out_buffer, AV_PIX_FMT_YUV420P, pTextureYUV->w, pTextureYUV->h, 1);

	return pTextureYUV;
#endif
}

int CDisplay::DestroyTexture(HTexture hTexture)
{
#if USE_SDL1
	SDL_Overlay *pOverlay = (SDL_Overlay *)hTexture;
	SDL_FreeYUVOverlay(pOverlay);
#else
	TextureYUV *pTextureYUV = (TextureYUV *)hTexture;

	SDL_DestroyTexture(pTextureYUV->pSDLTexture);
	delete pTextureYUV;
	pTextureYUV = NULL;
#endif

	return 1;
}

int CDisplay::UpdateTexture(HTexture hTexture, AVPixelFormat format, unsigned char *data[], int linesize[])
{
#if USE_SDL1
	SDL_Overlay *pOverlay = (SDL_Overlay *)hTexture;
	int w = pOverlay->w;
	int h = pOverlay->h;

	AVPicture pict = { { 0 } };
	
	/* get a pointer on the bitmap */
	SDL_LockYUVOverlay(pOverlay);

	//显示YUV数据
	pict.data[0] = pOverlay->pixels[0];
	pict.data[1] = pOverlay->pixels[2];
	pict.data[2] = pOverlay->pixels[1];
	pict.linesize[0] = pOverlay->pitches[0];
	pict.linesize[1] = pOverlay->pitches[2];
	pict.linesize[2] = pOverlay->pitches[1];

	_img_convert_ctx = sws_getCachedContext(_img_convert_ctx,
		w, h, format, w, h, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	sws_scale(_img_convert_ctx, data, linesize,	0, h, pict.data, pict.linesize);
	SDL_UnlockYUVOverlay(pOverlay);
#else
	TextureYUV *pTextureYUV = (TextureYUV *)hTexture;
	AVPicture pict = { { 0 } };
	_img_convert_ctx = sws_getCachedContext(_img_convert_ctx,
		pTextureYUV->w, pTextureYUV->h, format, pTextureYUV->w, pTextureYUV->h, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	sws_scale(_img_convert_ctx, (const unsigned char* const*)data, linesize, 0, pTextureYUV->h, pTextureYUV->pFrameYUV->data, pTextureYUV->pFrameYUV->linesize);

	SDL_UpdateTexture(pTextureYUV->pSDLTexture, NULL, pTextureYUV->pFrameYUV->data[0], pTextureYUV->pFrameYUV->linesize[0]);
	SDL_RenderClear(_sdlRenderer);
	//SDL_RenderCopy( sdlRenderer, sdlTexture, &sdlRect, &sdlRect );  
	SDL_RenderCopy(_sdlRenderer, pTextureYUV->pSDLTexture, NULL, NULL);

#endif

	return 1;
}

int CDisplay::DisplayTexture(HTexture hTexture, SDL_Rect rect)
{
#if USE_SDL1
	SDL_Overlay *pOverlay = (SDL_Overlay *)hTexture;

	SDL_DisplayYUVOverlay(pOverlay, &rect);
#else
	SDL_RenderPresent(_sdlRenderer);
#endif

	return 1;
}
