#include "Chapter5_0.h"

#include "GameInstance.h"
#include "UIManager.h"
#include "Background.h"

#include "Player.h"
#include "PlayerCamera.h"

CChapter5_0::CChapter5_0()
	: CMainQuest{}
{
}

HRESULT CChapter5_0::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//씬 카메라 이동 후 스크립트 텍스쳐 나옴
	m_pUIManager->Open_Scene(TEXT("Talk"));
	m_pUIManager->Start_Talk(m_iScriptIndex);

	PlayerCom_Stop(true);
	Player_Stop(true);

	return S_OK;
}

//대화가 끝나면 자동으로 다음 스테이지로 넘어감
_bool CChapter5_0::Execute()
{
	if (m_pUIManager->isTalkFinished())
	{
		PlayerCom_Stop(false);
		Player_Stop(false);
		return true;
	}
		

	return false;
}

CChapter5_0* CChapter5_0::Create(void* pArg)
{
	CChapter5_0* pInstance = new CChapter5_0();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter5_0::Free()
{
	__super::Free();
}
