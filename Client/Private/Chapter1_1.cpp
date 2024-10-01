#include "Chapter1_1.h"

#include "GameInstance.h"
#include "UIManager.h"
#include "ScriptManager.h"
#include "FileTotalMgr.h"

#include "Nishiki.h"
#include "PlayerCamera.h"
#include "Player.h"

CChapter1_1::CChapter1_1()
	: CMainQuest{}
{
}

HRESULT CChapter1_1::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//dynamic_cast<CPlayerCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER))->Set_RotationBlock(true);
	
	m_pNishiki = dynamic_cast<CNishiki*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Nishiki"), 0));
	Safe_AddRef(m_pNishiki);
	m_pNishiki->Set_State(CNishiki::TALK);

	//위치이동이 필요함.
	_matrix NishikiMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationRollPitchYaw(XMConvertToRadians(179.978f), XMConvertToRadians(-89.475f) , XMConvertToRadians(-179.978f) ) * XMMatrixTranslation(20.890f, 0.095f, 60.361f);
	m_pNishiki->Get_TransformCom()->Set_WorldMatrix(NishikiMatrix);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
	pPlayer->Set_PlayerStop(true);
	
	_matrix PlayerMoveMatrix = XMMatrixScaling(1.f, 1.f, 1.f)* XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.f), XMConvertToRadians(88.167f), XMConvertToRadians(0.f))* XMMatrixTranslation(19.911f, 0.143f, 60.127f);
	pPlayer->Get_TransformCom()->Set_WorldMatrix(PlayerMoveMatrix);

	m_pUIManager->Fade_Out();

	m_pFileTotalMgr->Setting_Start_Cinemachine(8);

	return S_OK;
}

_bool CChapter1_1::Execute()
{
	if (m_isStart == false)
	{
		if (m_pUIManager->isFindFinished())
		{
			//씬 카메라 이동 후 스크립트 텍스쳐 나옴
			m_pUIManager->Close_Scene();
			m_pUIManager->Open_Scene(TEXT("Talk"));
			m_pUIManager->Start_Talk(m_iScriptIndex);

			m_isStart = true;
		}
	}
	else
	{
		if (m_pUIManager->isTalkFinished())
		{
			m_pNishiki->Set_State(CNishiki::IDLE);
			Player_Stop(false);
			PlayerCom_Stop(false);
			return true;
		}

		//
		if (m_pGameInstance->GetKeyState(DIK_E) == TAP)
		{
			//UI틱이 더 느림 -> index를 하나 씩 더 느리게 봐야 함.
			if (m_pUIManager->Get_CurrentPage() == 2)
				m_pFileTotalMgr->Setting_Start_Cinemachine(9);
			else if (m_pUIManager->Get_CurrentPage() == 3)
				m_pFileTotalMgr->Setting_Start_Cinemachine(10);
			else if (m_pUIManager->Get_CurrentPage() == 5)
				m_pFileTotalMgr->Setting_Start_Cinemachine(11);
			else if (m_pUIManager->Get_CurrentPage() == 6)
				m_pFileTotalMgr->Setting_Start_Cinemachine(12);
			else if (m_pUIManager->Get_CurrentPage() == 7)
				m_pFileTotalMgr->Setting_Start_Cinemachine(13);
			else if (m_pUIManager->Get_CurrentPage() == 9)
				m_pFileTotalMgr->Setting_Start_Cinemachine(14);
			else if (m_pUIManager->Get_CurrentPage() == 10)
				m_pFileTotalMgr->Setting_Start_Cinemachine(11);
			else if (m_pUIManager->Get_CurrentPage() == 13)
				m_pFileTotalMgr->Setting_Start_Cinemachine(15);
		}
	}

	return false;
}

CChapter1_1* CChapter1_1::Create(void* pArg)
{
	CChapter1_1* pInstance = new CChapter1_1();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter1_1::Free()
{
	__super::Free();

	Safe_Release(m_pNishiki);
}
