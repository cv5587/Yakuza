#pragma once
#include "Base.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CLoader :
	public CBase
{
protected:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	const _tchar* Get_LoadingText() const {
		return m_szLoadingText;
	}

	_bool is_Finished() const {
		return m_isFinished;
	}

public:
	virtual HRESULT Initialize(LEVEL eNextLevel);
	virtual HRESULT Loading();

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	HANDLE					m_hThread = { 0 };
	CRITICAL_SECTION		m_Critical_Section = {};
	LEVEL					m_eNextLevel = { LEVEL_END };
	_tchar					m_szLoadingText[MAX_PATH] = { TEXT("") };
	_bool					m_isFinished = { false };
	CGameInstance* m_pGameInstance = { nullptr };

	/* 각 맵에 관해 로딩할 수 있도록 해주는 함수*/
protected:
	virtual HRESULT Loading_For_LogoLevel();
	virtual	HRESULT Loading_For_Office_1F();
	virtual	HRESULT Loading_For_Office_2F();
	virtual	HRESULT Loading_For_Office_Boss();
	virtual	HRESULT Loading_For_Dogimazo();
	virtual	HRESULT Loading_For_Dogimazo_Stairs();
	virtual	HRESULT Loading_For_Dogimazo_Lobby();
	virtual	HRESULT Loading_For_Dogimazo_Boss();
	virtual HRESULT Loading_For_Street();
	virtual	HRESULT Loading_For_CarChase();
	virtual	HRESULT Loading_For_Karaoke();
	virtual HRESULT Loading_For_Tutorial();
	virtual HRESULT Loading_For_NishikiWalk();

	/*파일 입출력에 관련된 함수*/
protected:
	HRESULT Add_Models_On_Path(_uint iLevel, const wstring& strPath, _bool bAnim = true);
	HRESULT Add_GameObject_Particle_On_Path(const wstring& strPath);
	HRESULT Add_Models_On_Path_NonAnim(_uint iLevel, const wstring& strPath);
	HRESULT Add_Components_On_Path_Material(_uint iLevel, const wstring& strPath);

protected:
	virtual HRESULT Loading_Default(); // 모든 level을 로딩할때 필요한 정보에 대해서 넣어둔다.

public:
	virtual void Free() override;
};
END
