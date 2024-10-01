#pragma once

#include <process.h>

#define ONE_THIRD 0.3333333333333333f

namespace Client
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_TEST, LEVEL_EDIT, LEVEL_END };

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

extern wstring g_wstrModelName;

using namespace Client;