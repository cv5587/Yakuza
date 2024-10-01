#pragma once

#include <d3d11.h>
#include "dinput.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include <random>
#include <regex>

#include <string>
#include "fx11\d3dx11effect.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
#include "DirectXTK\Keyboard.h"
#include "DirectXTK\Mouse.h"
#include "DirectXTK\ScreenGrab.h"
#include "DirectXTK\PrimitiveBatch.h"
#include "DirectXTK\VertexTypes.h"
#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"
#include "DirectXTK\Effects.h"
#include "fmode\inc\fmod.h"
#pragma comment(lib, "fmod_vc.lib")

#include "assimp\scene.h"
#include "assimp\Importer.hpp"
#include "assimp\postprocess.h"

using namespace DirectX;
using namespace TriangleTests;

#include <vector>
#include <list>
#include <map>
#include <stack>
#include <algorithm>
#include <functional>

#include <filesystem>
#include <locale>
#include <iostream>
#include <io.h>
#include <fstream>
#include <sstream>

#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"

//#if !defined(_CLIEND)
//#ifdef _DEBUG
//
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//
//#ifndef DBG_NEW 
//
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
//#define new DBG_NEW 
//
//#endif
//
//#endif // _DEBUG
//#endif // defined(_CLIEND)

namespace Engine
{
	enum DIRECTION { DIR_F, DIR_B, DIR_R, DIR_L, DIR_M, DIR_FL, DIR_FR, DIR_BL, DIR_BR, DIR_END };
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };
	enum KEYSTATE
	{
		NONE, //누르고 있지 않을 때
		TAP, //한번 눌렀을 때
		HOLD,//누르고 있을 때
		AWAY, //눌렀다 땠을 때
		KEY_END,
	};
	enum CHANNELID {
		SOUND_EFFECT,
		SOUND_BGM,
		SOUND_AMBIENT,
		SOUND_PLAYER_VOICE,
		SOUND_PLAYER_EFFECT,
		SOUND_ENEMY_VOICE,
		SOUND_ENEMY_EFFECT,
		SOUND_ENEMY_HIT,
		SOUND_ENEMY_DEAD,
		SOUND_WORD,
		SOUND_UI,
		SOUND_ATTACK,
		SOUND_ENVIRONMENT,
		SOUND_TAXI,
		SOUND_BIKE,
		SOUND_VAN,
		SOUND_SEDAN,
		SOUND_HELI,
		SOUND_NPC_1,
		SOUND_NPC_2,
		SOUND_NPC_3,
		SOUND_BGM_1,
		SOUND_BGM_2,
		SOUND_BGM_3,
		SOUND_EFFECT_SUB,
		SOUND_EFFECT_THIRD,
		SOUND_END
	};

}

using namespace Engine;
using namespace std;
namespace fs = std::filesystem;


