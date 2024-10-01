
#include "SoundMgr.h"
#include "GameInstance.h"

CSoundMgr::CSoundMgr()
	:m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
	m_pSystem = nullptr;
}


CSoundMgr::~CSoundMgr()
{
	Free();
}

HRESULT CSoundMgr::Initialize()
{
	Ready_Sound();

	return S_OK;
}

void Engine::CSoundMgr::Ready_Sound()
{
	// 사운드를 담당하는 대표객체를 생성하는 함수
	FMOD_System_Create(&m_pSystem, FMOD_VERSION);
	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);


	LoadSoundFile();
	//LoadCarchaseSoundFile();
}

_bool CSoundMgr::PlaySound_W(const wstring pSoundKey, CHANNELID eID, float fVolume)
{
	map<const wstring, FMOD_SOUND*>::iterator iter;
	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return pSoundKey == iter.first;
		});

	if (iter == m_mapSound.end())
		return FMOD_ERR_FILE_NOTFOUND;

	FMOD_BOOL bPlay = FALSE;
	_bool isPlay = false;

	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	{
 		FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[eID]);
		isPlay = true;
	}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);

	return isPlay;
}

_bool CSoundMgr::StopAndPlaySound(const wstring pSoundKey, CHANNELID eID, float fVolume)
{
	FMOD_BOOL bPlay = FALSE;
	_bool isPlay = false;

	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	{
		FMOD_Channel_Stop(m_pChannelArr[eID]);
	}

	map<const wstring, FMOD_SOUND*>::iterator iter;
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return pSoundKey == iter.first;
		});

	if (iter == m_mapSound.end())
		return FMOD_ERR_FILE_NOTFOUND;

	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[eID]);

	isPlay = true;

	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);

	return isPlay;
}

_bool CSoundMgr::PlaySoundIfNotPlay(const wstring pSoundKey, CHANNELID eID, float fVolume)
{
	FMOD_BOOL bPlay = FALSE;
	_bool isPlay = false;

	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	{
		isPlay = false;
		return isPlay;
	}

	map<const wstring, FMOD_SOUND*>::iterator iter;
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return pSoundKey == iter.first;
		});

	if (iter == m_mapSound.end())
		return FMOD_ERR_FILE_NOTFOUND;

	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);

	return isPlay;
}

void CSoundMgr::PlayBGM(const wstring pSoundKey, float fVolume)
{
	map<const wstring, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return pSoundKey == iter.first;
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr,  FALSE, &m_pChannelArr[SOUND_BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::Play_Loop(const wstring pSoundKey, CHANNELID eID, float fVolume)
{
	map<const wstring, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return pSoundKey == iter.first;
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[eID]);
	FMOD_Channel_SetMode(m_pChannelArr[eID], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::StopSound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSoundMgr::StopAll()
{
	for (int i = 0; i < SOUND_END; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSoundMgr::SetChannelVolume(CHANNELID eID, float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);
}

_float CSoundMgr::GetSoundPosition(const wstring pSoundKey, CHANNELID eID)
{
	map<const wstring, FMOD_SOUND*>::iterator iter;
	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return pSoundKey == iter.first;
		});

	if (iter == m_mapSound.end())
		return FMOD_ERR_FILE_NOTFOUND;

	_uint position = 0;

	FMOD_Channel_GetPosition(m_pChannelArr[eID], &position, FMOD_TIMEUNIT_MS);

	// 길이를 초 단위로 반환합니다.
	return static_cast<_float>(position) / 1000.0f;  // 밀리초를 초로 변환
}

_float CSoundMgr::GetSoundDuration(const wstring pSoundKey)
{
	map<const wstring, FMOD_SOUND*>::iterator iter;
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter) -> bool
		{
			return pSoundKey == iter.first;
		});

	if (iter == m_mapSound.end())
		return FMOD_ERR_FILE_NOTFOUND;

	// 사운드의 전체 길이를 저장할 변수를 선언합니다.
	unsigned int length = 0;
	FMOD_RESULT result = FMOD_Sound_GetLength(iter->second, &length, FMOD_TIMEUNIT_MS);

	if (result != FMOD_OK) {
		// 오류 처리 (필요한 경우)
		return -1.0f;
	}

	// 길이를 초 단위로 반환합니다.
	return static_cast<_float>(length) / 1000.0f;  // 밀리초를 초로 변환
}

_bool CSoundMgr::Get_Start(const wstring pSoundKey, CHANNELID eID)
{
	map<const wstring, FMOD_SOUND*>::iterator iter;
	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return pSoundKey == iter.first;
		});

	if (iter == m_mapSound.end())
		return FMOD_ERR_FILE_NOTFOUND;

	_uint position = 0;
	FMOD_Channel_GetPosition(m_pChannelArr[eID], &position, FMOD_TIMEUNIT_MS);

	_int isPlaying = { 0 };
	FMOD_Channel_IsPlaying(m_pChannelArr[eID], &isPlaying);

	if (isPlaying && position > 0) {
		return true;
	}

	return false;
}

_bool CSoundMgr::Get_End(const wstring pSoundKey, CHANNELID eID)
{
	map<const wstring, FMOD_SOUND*>::iterator iter;
	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return pSoundKey == iter.first;
		});

	if (iter == m_mapSound.end())
		return FMOD_ERR_FILE_NOTFOUND;

	_uint position = 0;
	FMOD_Channel_GetPosition(m_pChannelArr[eID], &position, FMOD_TIMEUNIT_MS);

	_uint length = 0;
	FMOD_RESULT result = FMOD_Sound_GetLength(iter->second, &length, FMOD_TIMEUNIT_MS);

	_int isPlaying = { 0 };
	FMOD_Channel_IsPlaying(m_pChannelArr[eID], &isPlaying);

	if (!isPlaying && position >= length) {
		return true;
	}

	return false;
}

void CSoundMgr::Set_SoundPosition(const wstring pSoundKey, CHANNELID eID, _float fSeconds)
{
	map<const wstring, FMOD_SOUND*>::iterator iter;
	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return pSoundKey == iter.first;
		});

	if (iter == m_mapSound.end())
		return;

	// 초 단위를 밀리초 단위로 변환합니다.
	_uint newPosition = static_cast<_uint>(fSeconds * 1000.0f);

	FMOD_Channel_SetPosition(m_pChannelArr[eID], newPosition, FMOD_TIMEUNIT_MS);
}
#include <io.h>
#include <string>

#include <io.h>
#include <string>

#include <io.h>
#include <string>

void CSoundMgr::LoadSoundFile(const char* szBasePath)
{
	_wfinddata64_t fd;
	intptr_t handle;
	char szFullPath[MAX_PATH];
	char szFilename[MAX_PATH];
	wchar_t wPath[MAX_PATH];
	std::string basePath(szBasePath);

	// 검색할 경로를 지정하여 와이드 문자열로 변환
	std::string searchPath = basePath + "*.*";
	mbstowcs(wPath, searchPath.c_str(), MAX_PATH);

	// 파일과 디렉토리를 검색
	handle = _wfindfirst64(wPath, &fd);
	if (handle == -1)
		return;

	do
	{
		// 디렉토리일 경우, 재귀적으로 호출
		if (fd.attrib & _A_SUBDIR)
		{
			if (wcscmp(fd.name, L".") != 0 && wcscmp(fd.name, L"..") != 0)
			{
				// 하위 디렉토리를 탐색하기 위한 경로 생성
				WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
				std::string newDir = basePath + szFilename + "/";

				// 재귀 호출하여 하위 디렉토리를 탐색
				LoadSoundFile(newDir.c_str());
			}
		}
		else
		{
			// 파일일 경우, 사운드 파일 로드
			WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
			strcpy_s(szFullPath, basePath.c_str());
			strcat_s(szFullPath, szFilename);

			FMOD_SOUND* pSound = nullptr;
			FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_3D | 0x00000020, 0, &pSound);
			if (eRes == FMOD_OK)
			{
				int iLength = strlen(szFilename) + 1;

				_tchar* pSoundKey = new _tchar[iLength];
				ZeroMemory(pSoundKey, sizeof(_tchar) * iLength);
				MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);

				m_mapSound.emplace(pSoundKey, pSound);
			}
		}
	} while (_wfindnext64(handle, &fd) == 0);

	_findclose(handle);

	// FMOD 시스템 업데이트
	FMOD_System_Update(m_pSystem);
	
}
/*
void CSoundMgr::LoadCarchaseSoundFile()
{
	_wfinddata64_t fd;
	__int64 handle = _wfindfirst64(L"../../Client/Bin/Resources/Sounds/CarChase/*.*", &fd);
	if (handle == -1 || handle == 0)
		return;

	int iResult = 0;

	char szCurPath[128] = "../../Client/Bin/Resources/Sounds/CarChase/";
	char szFullPath[128] = "";
	char szFilename[MAX_PATH];
	while (iResult != -1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, szFilename);
		FMOD_SOUND* pSound = nullptr;

		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_3D | 0x00000020, 0, &pSound);
		if (eRes == FMOD_OK)
		{
			int iLength = strlen(szFilename) + 1;

			_tchar* pSoundKey = new _tchar[iLength];
			ZeroMemory(pSoundKey, sizeof(_tchar) * iLength);
			MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		iResult = _wfindnext64(handle, &fd);
	}
	FMOD_System_Update(m_pSystem);
	_findclose(handle);
}
*/

CSoundMgr* CSoundMgr::Create()
{
	CSoundMgr* pInstance = new CSoundMgr();

	if (FAILED(pInstance->Initialize()))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::CSoundMgr::Free()
{
	Safe_Release(m_pGameInstance);

	for (auto& Mypair : m_mapSound)
	{
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}
