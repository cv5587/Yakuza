#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CSoundMgr :
	public CBase
{
private:
	CSoundMgr();
	virtual ~CSoundMgr();

public:
	HRESULT Initialize();

public:
	void Ready_Sound();
	_bool PlaySound_W(const wstring pSoundKey, CHANNELID eID, float fVolume);
	//채널에 소리가 재생중일때 끄고 재생
	_bool StopAndPlaySound(const wstring pSoundKey, CHANNELID eID, float fVolume);
	//채널에 소리가 재생하지 않을때 재생
	_bool PlaySoundIfNotPlay(const wstring pSoundKey, CHANNELID eID, float fVolume);
	void PlayBGM(const wstring pSoundKey, float fVolume);
	void Play_Loop(const wstring pSoundKey, CHANNELID eID, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);

	_float GetSoundPosition(const wstring pSoundKey, CHANNELID eID);
	_float GetSoundDuration(const wstring pSoundKey);

	_bool Get_Start(const wstring pSoundKey, CHANNELID eID);
	_bool Get_End(const wstring pSoundKey, CHANNELID eID);

	void Set_SoundPosition(const wstring pSoundKey, CHANNELID eID, _float fSeconds);

private:
	void LoadSoundFile(const char* szBasePath = "../../Client/Bin/Resources/Sounds/");
	//void LoadCarchaseSoundFile();

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<const wstring, FMOD_SOUND*> m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[SOUND_END];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem = { nullptr };

	class CGameInstance* m_pGameInstance = { nullptr };

public:
	static CSoundMgr* Create();
	virtual void Free() override;
};
END