#include "Chapter6_0.h"

#include "GameInstance.h"
#include "FileTotalMgr.h"
#include "UIManager.h"
#include "Background.h"

#include "Player.h"

CChapter6_0::CChapter6_0()
	: CMainQuest{}
{
}

HRESULT CChapter6_0::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

	_matrix PlayerMoveMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.f), XMConvertToRadians(34.942f), XMConvertToRadians(0.f)) * XMMatrixTranslation(0.684f, 12.044f, -10.010f);
	pPlayer->Get_TransformCom()->Set_WorldMatrix(PlayerMoveMatrix);

	m_pUIManager->Fade_Out();

	Player_Stop(true);
	PlayerCom_Stop(true);

	m_pFileTotalMgr->Setting_Start_Cinemachine(35);

	return S_OK;
}

//대화가 끝나면 자동으로 다음 스테이지로 넘어감
_bool CChapter6_0::Execute()
{
	if (m_isStart == false)
	{
		if (m_pUIManager->isFindFinished())
		{
			//씬 카메라 이동 후 스크립트 텍스쳐 나옴
			m_pUIManager->Open_Scene(TEXT("Talk"));
			m_pUIManager->Start_Talk(m_iScriptIndex);

			m_isStart = true;
		}
	}
	else
	{
		if (m_pUIManager->isTalkFinished())
		{
			Player_Stop(false);
			PlayerCom_Stop(false);
			return true;
		}

		if (m_pGameInstance->GetKeyState(DIK_E) == TAP)
		{
			//UI틱이 더 느림 -> index를 하나 씩 더 느리게 봐야 함.
			if (m_pUIManager->Get_CurrentPage() == 1)
				m_pFileTotalMgr->Setting_Start_Cinemachine(36);
		}
	}

	return false;
}

CChapter6_0* CChapter6_0::Create(void* pArg)
{
	CChapter6_0* pInstance = new CChapter6_0();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter6_0::Free()
{
	__super::Free();
}
