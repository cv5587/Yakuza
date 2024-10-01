#include "../Default/framework.h"

#include "GameInstance.h"
#include "UIManager.h"

#pragma region LEVEL_HEADER
#include "Loader_Anim.h"
#include "Loader_Other.h"
#include "Loader_Map.h"

#include "MultiLoader.h"

#include "Level_Loading.h"
#include "Level_Logo.h"
#include "Level_Office1F.h"
#include "Level_Office2F.h"
#include "Level_OfficeBoss.h"
#include "Level_Dogimazo.h"
#include "Level_DogimazoStairs.h"
#include "Level_DogimazoLobby.h"
#include "Level_DogimazoBoss.h"
#include "Level_Roadway.h"
#include "Level_Karaoke.h"
#include "Level_Street.h"
#include "Level_Tutorial.h"
#include "Level_NishikiWalk.h"
#include "Level_KaraokeLobby.h"
#include "Level_Test.h"
#pragma endregion

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevel)
{
	m_pGameInstance->StopSound(SOUND_BGM);
	m_pGameInstance->StopSound(SOUND_BGM_1);
	m_pGameInstance->StopSound(SOUND_BGM_2);
	m_pGameInstance->StopSound(SOUND_BGM_3);

	m_eNextLevel = eNextLevel;

	/* 로딩씬에 뭔가를 보여주려한다면 그 보여주기위한 객체들을 생성한다. (백그라운드 + 로딩바 + 폰트) */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	/* eNextLevel을 위한 자원을 로드한다.(추가적으로 생성한 스레드) */
	if (m_eNextLevel == LEVEL_TEST)
	{
		m_pMultiLoader = CMultiLoader::Create(m_pDevice, m_pContext, eNextLevel);
		if (nullptr == m_pMultiLoader)
			return E_FAIL;
	}
	else
	{
		m_pLoader[ANIM] = CLoader_Anim::Create(m_pDevice, m_pContext, eNextLevel);
		if (nullptr == m_pLoader[ANIM])
			return E_FAIL;

		m_pLoader[MAP] = CLoader_Map::Create(m_pDevice, m_pContext, eNextLevel);
		if (nullptr == m_pLoader[MAP])
			return E_FAIL;

		m_pLoader[OTEHR] = CLoader_Other::Create(m_pDevice, m_pContext, eNextLevel);
		if (nullptr == m_pLoader[OTEHR])
			return E_FAIL;
	}

	return S_OK;
}

void CLevel_Loading::Tick(const _float& fTimeDelta)
{
	if (m_eNextLevel == LEVEL_TEST)
	{
		if (true == m_pMultiLoader->is_Finished())
		{
			if (m_pGameInstance->GetKeyState(DIK_RETURN) == TAP)
			{
				CLevel* pNewLevel = { nullptr };

				m_pGameInstance->Set_CurrentLevel(m_eNextLevel);
				CUIManager::GetInstance()->Close_Scene();

				pNewLevel = CLevel_Test::Create(m_pDevice, m_pContext);

				if (nullptr == pNewLevel)
					return;

				if (FAILED(m_pGameInstance->Open_Level(m_eNextLevel, pNewLevel)))
					return;
			}
		}
	}
	else
	{
		_bool isFinished = { true };
		for (auto& pLoader : m_pLoader)
		{
			if (!pLoader->is_Finished())
			{
				isFinished = false;
				SetWindowText(g_hWnd, pLoader->Get_LoadingText());
				return;
			}
		}

		SetWindowText(g_hWnd, TEXT("로딩이 완료되었습니다."));

		if (true == isFinished)
		{
			if (m_pGameInstance->GetKeyState(DIK_RETURN) == TAP)
			{
				CLevel* pNewLevel = { nullptr };

 				m_pGameInstance->Set_CurrentLevel(m_eNextLevel);
				CUIManager::GetInstance()->Close_Scene();
				switch (m_eNextLevel)
				{
				case LEVEL_LOGO:
					pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_OFFICE_1F:
					pNewLevel = CLevel_Office1F::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_OFFICE_2F:
					pNewLevel = CLevel_Office2F::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_OFFICE_BOSS:
					pNewLevel = CLevel_OfficeBoss::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_DOGIMAZO:
					pNewLevel = CLevel_Dogimazo::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_DOGIMAZO_STAIRS:
					pNewLevel = CLevel_DogimazoStairs::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_DOGIMAZO_LOBBY:
					pNewLevel = CLevel_DogimazoLobby::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_DOGIMAZO_BOSS:
					pNewLevel = CLevel_DogimazoBoss::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_CARCHASE:
					pNewLevel = CLevel_Roadway::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_KARAOKE:
					pNewLevel = CLevel_Karaoke::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_TUTORIAL:
					pNewLevel = CLevel_Tutorial::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_NISHIKIWALK:
					pNewLevel = CLevel_NishikiWalk::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_TOKOSTREET:
					pNewLevel = CLevel_Street::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_KARAOKE_START:
					pNewLevel = CLevel_KaraokeLobby::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_KARAOKE_END:
					pNewLevel = CLevel_KaraokeLobby::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_STREET:
					pNewLevel = CLevel_Street::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_TEST:
					pNewLevel = CLevel_Test::Create(m_pDevice, m_pContext);
					break;
				}

				if (nullptr == pNewLevel)
					return;


				if (FAILED(m_pGameInstance->Open_Level(m_eNextLevel, pNewLevel)))
					return;
			}
		}
	}

#ifdef _DEBUG
	if (m_eNextLevel == LEVEL_TEST)
		SetWindowText(g_hWnd, m_pMultiLoader->Get_LoadingText());
#else
	// 로딩 화면 만들어지기 전까지 릴리즈 모드에서도 볼 수 있게 해야함
	//SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());
#endif
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Prototype_GameObject_BackGround"), strLayerTag)))
	//	return E_FAIL;
	static _bool isFirst = false;
	if(!isFirst)
	{
		CUIManager::GetInstance()->Open_Scene(TEXT("Loading"));
		isFirst = true;
	}
	else
	{
		CUIManager::GetInstance()->Open_Scene(TEXT("NowLoading"));
	}


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

	for (auto& pLoader : m_pLoader)
		Safe_Release(pLoader);

 	Safe_Release(m_pMultiLoader);
}
