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
private:
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
	HRESULT Initialize(LEVEL eNextLevel);
	HRESULT Loading();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	HANDLE					m_hThread = { 0 };
	CRITICAL_SECTION		m_Critical_Section = {};
	LEVEL					m_eNextLevel = { LEVEL_END };
	_tchar					m_szLoadingText[MAX_PATH] = { TEXT("") };
	_bool					m_isFinished = { false };
	CGameInstance* m_pGameInstance = { nullptr };

private:
	HRESULT Loading_For_LogoLevel();
	HRESULT Loading_For_GamePlayLevel();
	HRESULT	Loading_For_Static();
	HRESULT Loading_For_RunMapLevel(int iLevel);

private:
	HRESULT Add_Models_On_Path(_uint iLevel, const wstring& strPath, _bool bAnim = true);		// 0627 Çý¿øÃß°¡
	HRESULT Add_Models_On_Path_NonAnim(_uint iLevel, const wstring& strPath);

public:
    static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel);
    virtual void Free() override;
};
END
