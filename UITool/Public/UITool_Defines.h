#pragma once

#include <process.h>

namespace UITool
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_TEST, LEVEL_RUNMAP, LEVEL_END };

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace UITool;