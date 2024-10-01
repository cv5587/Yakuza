#pragma once
#include "Base.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CMultiLoader :
	public CBase
{
public:
	enum LOADING_TYPE { LOADING_ANIM, LOADING_NONANIM, LOADING_DEFAULT, LOADING_END };

private:
	CMultiLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMultiLoader() = default;

public:
	const _tchar* Get_LoadingText() const {
		return m_szLoadingText;
	}

	_bool is_Finished() {
		_bool isFinish = m_isFinished[0] && m_isFinished[1] && m_isFinished[2];
		if (isFinish)
			lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
		return isFinish;
	}

public:
	HRESULT Initialize(LEVEL eNextLevel);
	HRESULT Loading(_uint iType);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	HANDLE					m_hAnimThread = { 0 };
	HANDLE					m_hNonAnimThread = { 0 };
	HANDLE					m_hThread = { 0 };
	CRITICAL_SECTION		m_Critical_Section = {};
	LEVEL					m_eNextLevel = { LEVEL_END };
	_tchar					m_szLoadingText[MAX_PATH] = { TEXT("") };
	_bool					m_isFinished[3];
	CGameInstance*			m_pGameInstance = { nullptr };

private:
	HRESULT Loading_For_Anim();
	HRESULT Loading_For_NonAnim();
	HRESULT Loading_For_Default();

private:
	HRESULT Add_Models_On_Path(_uint iLevel, const wstring& strPath, _bool bAnim = true);		// 0627 혜원추가
	HRESULT Add_GameObject_Particle_On_Path(const wstring& strPath);
	HRESULT Add_Models_On_Path_NonAnim(_uint iLevel, const wstring& strPath);				// 현정추가
	HRESULT Add_Components_On_Path_Material(_uint iLevel, const wstring& strPath);

private:
	HRESULT Loading_Default();
	HRESULT Loading_Highway();


public:
	static CMultiLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel);
	virtual void Free() override;
};
END
