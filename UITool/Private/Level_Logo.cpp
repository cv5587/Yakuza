#include "Level_Logo.h"

#include "GameInstance.h"

#include "Level_Loading.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()
{
	return S_OK;
}

void CLevel_Logo::Tick(const _float& fTimeDelta)
{
	if (m_pGameInstance->GetKeyState(DIK_SPACE) == TAP)
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_RUNMAP))))
			return;
	}

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("로고레벨"));
#endif
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
