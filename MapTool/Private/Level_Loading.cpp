#include "../Default/framework.h"

#include "GameInstance.h"

#pragma region LEVEL_HEADER
#include "Loader.h"
#include "Level_Loading.h"
#include "Level_Logo.h"
#include "Level_Test.h"
#include "Level_RunMap.h"
#pragma endregion


CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	/* 로딩씬에 뭔가를 보여주려한다면 그 보여주기위한 객체들을 생성한다. (백그라운드 + 로딩바 + 폰트) */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	/* eNextLevel을 위한 자원을 로드한다.(추가적으로 생성한 스레드) */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(const _float& fTimeDelta)
{
	if (true == m_pLoader->is_Finished())
	{
		if (GetKeyState(VK_RETURN) & 0x8000)
		{
			CLevel* pNewLevel = { nullptr };

			switch (m_eNextLevel)
			{
			case LEVEL_LOGO:
				pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_TEST:
				pNewLevel = CLevel_Test::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_RUNMAP:
				pNewLevel = CLevel_RunMap::Create(m_pDevice, m_pContext);
				break;
			}

			if (nullptr == pNewLevel)
				return;

			if (FAILED(m_pGameInstance->Open_Level(m_eNextLevel, pNewLevel)))
				return;
		}
	}


#ifdef _DEBUG
	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());
#endif
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	/*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Prototype_GameObject_BackGround"), strLayerTag)))
		return E_FAIL;*/

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed To Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
