#include "stdafx.h"

#include "displayDef.h"
#include "display.h"

CDisplay g_disp;

CDisplay::CDisplay()
{
	_screen = NULL;
	_nWidth = -1;
	_nHeight = -1;
	_bFullScreen = 0;

	_img_convert_ctx = NULL;
}

CDisplay::~CDisplay()
{

}

int CDisplay::Init()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		TRACE("Could not initialize SDL - %s\n", SDL_GetError());
		return 0;
	}
	
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
	*pnWidth = GetSystemMetrics(SM_CXSCREEN);//ÆÁÄ»¿í¶È
	*pnHeight = GetSystemMetrics(SM_CYSCREEN);//ÆÁÄ»¸ß¶È

	return;
}

HDisplayWindow CDisplay::CreateDisplayWindow(int nWidth, int nHeight, int bFullScreen)
{
	if (_screen != NULL && _nWidth == nWidth && _nHeight == nHeight && _bFullScreen == bFullScreen)
		return _screen;

	SDL_Window *screen = SDL_CreateWindow("Video Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, nWidth, nHeight, SDL_WINDOW_OPENGL);
	_sdlRenderer = SDL_CreateRenderer(screen, -1, 0);

	_screen = screen;
	_nWidth = nWidth;
	_nHeight = nHeight;
	_bFullScreen = bFullScreen;

	return screen;
}

int CDisplay::DestroyDisplayWindow()
{
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

	return 1;
}

void CDisplay::GetWindowSize(int *pnWidth, int *pnHeight)
{
	SDL_GetWindowSize(_screen, pnWidth, pnHeight);
}

int CDisplay::FillWindow(int x, int y, int w, int h, int color)
{
	return 1;
}

int CDisplay::FillWindow(int x, int y, int w, int h, int r, int g, int b)
{
	return 1;
}

int CDisplay::UpdateRect(int x, int y, int w, int h)
{

	return 1;
}

HTexture CDisplay::CreateTexture(int w, int h)
{
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
}

int CDisplay::DestroyTexture(HTexture hTexture)
{
	TextureYUV *pTextureYUV = (TextureYUV *)hTexture;

	SDL_DestroyTexture(pTextureYUV->pSDLTexture);
	delete pTextureYUV;
	pTextureYUV = NULL;

	return 1;
}

int CDisplay::UpdateTexture(HTexture hTexture, AVPixelFormat format, unsigned char *data[], int linesize[])
{
	TextureYUV *pTextureYUV = (TextureYUV *)hTexture;
	AVPicture pict = { { 0 } };
	_img_convert_ctx = sws_getCachedContext(_img_convert_ctx,
		pTextureYUV->w, pTextureYUV->h, format, pTextureYUV->w, pTextureYUV->h, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	sws_scale(_img_convert_ctx, (const unsigned char* const*)data, linesize, 0, pTextureYUV->h, pTextureYUV->pFrameYUV->data, pTextureYUV->pFrameYUV->linesize);

	SDL_UpdateTexture(pTextureYUV->pSDLTexture, NULL, pTextureYUV->pFrameYUV->data[0], pTextureYUV->pFrameYUV->linesize[0]);
	SDL_RenderClear(_sdlRenderer);
	//SDL_RenderCopy( sdlRenderer, sdlTexture, &sdlRect, &sdlRect );  
	SDL_RenderCopy(_sdlRenderer, pTextureYUV->pSDLTexture, NULL, NULL);

	return 1;
}

int CDisplay::DisplayTexture(HTexture hTexture, SDL_Rect rect)
{
	SDL_RenderPresent(_sdlRenderer);

	return 1;
}
