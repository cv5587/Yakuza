#include "Chapter1_0.h"

#include "GameInstance.h"
#include "FileTotalMgr.h"
#include "SystemManager.h"

#include "UIManager.h"
#include "ScriptManager.h"
#include "UITutorial.h"

#include "Background.h"
#include "Player.h"

CChapter1_0::CChapter1_0()
	: CMainQuest{},
	m_pSystemManager{CSystemManager::GetInstance()}
{
	Safe_AddRef(m_pSystemManager);
}

HRESULT CChapter1_0::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//백그라운드 생성
	if (FAILED(m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_BackGround"), TEXT("Layer_StoryImage"), nullptr)))
		return E_FAIL;

	m_pBackground = dynamic_cast<CBackground*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_StoryImage"), 0));
	Safe_AddRef(m_pBackground);

	//씬 카메라 이동 후 스크립트 텍스쳐 나옴
	m_pFileTotalMgr->Setting_Start_Cinemachine(6);

	//m_pUIManager->Open_Scene(TEXT("Talk"));
	//m_pUIManager->Start_Talk(m_iScriptIndex); // 0번째 대화

	//플레이어 움직임 막기
	Player_Stop(true);
	PlayerCom_Stop(true);

	return S_OK;
}

_bool CChapter1_0::Execute()
{
	if (m_isTitleFinished)
	{
		Player_Stop(false);
		PlayerCom_Stop(false);
		return true;
	}

	if (m_pUIManager->isTalkFinished())
	{
		if (!m_isTitleStart) // 타이틀 띄우기
		{
			m_pSystemManager->Set_Camera(CAMERA_PLAYER);

			m_pGameInstance->PlaySound_W(TEXT("mission.wav"), SOUND_UI, 1.f);
			m_pUIManager->Close_Scene();
			m_pUIManager->Open_Scene(TEXT("Title"));
			m_pUIManager->Set_TitleStart(true); // 니시키야마를 찾아라
			m_pUIManager->Start_Title(14);
			m_isTitleStart = true;
		}
		else
		{
			m_isTitleFinished = true;
		}
	}
	else 
	{
		if (m_pUIManager->Get_CurrentPage() == 1)
		{
			if (!m_pBackground->isShow())
			{
				m_pSystemManager->Set_Camera(CAMERA_PLAYER);
				m_pBackground->Set_Show(true);

				m_pGameInstance->StopSound(SOUND_EFFECT);
				m_pGameInstance->PlaySound_W(TEXT("pokebell_ring01.wav"), SOUND_EFFECT, 0.5f);
			}
				
			return false;
		}

		else if (m_pUIManager->Get_CurrentPage() == 2)
		{
			if (m_pBackground->isShow())
			{
				m_pFileTotalMgr->Setting_Start_Cinemachine(7);
				m_pBackground->Set_Show(false);
			}
			return false;
		}
	}

	if (m_isStartCameraEnd)
		return false;

	//6번 씬 카메라가 끝나면 대화창 호출하기
	//한번 호출 한 이후로는 진입하지 않음.
	m_fStartTime += m_pGameInstance->Get_TimeDelta(TEXT("Timer_60"));

	if (m_fStartDuration < m_fStartTime)
	{
		m_isStartCameraEnd = true;
		m_pUIManager->Open_Scene(TEXT("Talk"));
		m_pUIManager->Start_Talk(m_iScriptIndex); // 0번째 대화
	}

	return false;
}

CChapter1_0* CChapter1_0::Create(void* pArg)
{
	CChapter1_0* pInstance = new CChapter1_0();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter1_0::Free()
{
	__super::Free();

	Safe_Release(m_pBackground);
	Safe_Release(m_pSystemManager);
}
