#include "Chapter7_0.h"

#include "GameInstance.h"
#include "UIManager.h"
#include "Background.h"

#include "FileTotalMgr.h"

CChapter7_0::CChapter7_0()
	: CMainQuest{}
{
}

HRESULT CChapter7_0::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//씬 카메라 이동 후 스크립트 텍스쳐 나옴
	m_pUIManager->Open_Scene(TEXT("Talk"));
	m_pUIManager->Start_Talk(m_iScriptIndex);

	m_pKuze = dynamic_cast<CKuze*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Monster"), 0));
	Safe_AddRef(m_pKuze);
	m_pKuze->Set_Script(true);
	m_pKuze->Set_Animation("p_stand_nml");

	Player_Stop(true);

	m_pFileTotalMgr->Setting_Start_Cinemachine(25);

	return S_OK;
}

//대화가 끝나면 자동으로 다음 스테이지로 넘어감
_bool CChapter7_0::Execute()
{
	if (m_pUIManager->isTalkFinished())
	{
		m_pKuze->Set_Script(false);
		Player_Stop(false);
		return true;
	}
		
	if (m_pGameInstance->GetKeyState(DIK_E) == TAP)
	{
		//UI틱이 더 느림 -> index를 하나 씩 더 느리게 봐야 함.
		if (m_pUIManager->Get_CurrentPage() == 1)
			m_pFileTotalMgr->Setting_Start_Cinemachine(26);
		else if (m_pUIManager->Get_CurrentPage() == 2)
			m_pFileTotalMgr->Setting_Start_Cinemachine(27);
		else if (m_pUIManager->Get_CurrentPage() == 3)
		{
			m_pFileTotalMgr->Setting_Start_Cinemachine(28);
			m_pKuze->Set_Animation("e_kuz_stand_blend");
		}
			
	}

	return false;
}

CChapter7_0* CChapter7_0::Create(void* pArg)
{
	CChapter7_0* pInstance = new CChapter7_0();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter7_0::Free()
{
	__super::Free();

	Safe_Release(m_pKuze);
}
