#include "Level_Logo.h"

#include "GameInstance.h"

#include "Level_Loading.h"

#include "UIManager.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()
{
 	CUIManager::GetInstance()->Open_Scene(TEXT("Logo"));

	return S_OK;
}

void CLevel_Logo::Tick(const _float& fTimeDelta)
{
	if(m_pGameInstance->GetMouseState(DIM_LB) == AWAY)
		CUIManager::GetInstance()->Click();

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("로고레벨 Space : TEST / F1 : Office 1F"));
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
