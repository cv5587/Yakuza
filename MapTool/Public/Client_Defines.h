#pragma once

#include <process.h>

namespace Client
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_TEST, LEVEL_RUNMAP, LEVEL_END };
	enum CAMERA { CAMERA_DEBUG, CAMERA_PLAYER, CAMERA_CINEMACHINE, CAMERA_END };
	

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;