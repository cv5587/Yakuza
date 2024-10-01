#include "MainQuest.h"

#include "ScriptManager.h"
#include "UIManager.h"
#include "FileTotalMgr.h"
#include "GameInstance.h"

#include "Player.h"
#include "PlayerCamera.h"

CMainQuest::CMainQuest()
	: m_pUIManager { CUIManager::GetInstance() },
	m_pFileTotalMgr{ CFileTotalMgr::GetInstance() }
{
	Safe_AddRef(m_pUIManager);
	Safe_AddRef(m_pFileTotalMgr);
}

HRESULT CMainQuest::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MAIN_QUEST_DESC* pDesc = static_cast<MAIN_QUEST_DESC*>(pArg);
	m_iScriptIndex = pDesc->iScriptIndex;

	return S_OK;
}

void CMainQuest::Player_Stop(_bool isStop)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
	pPlayer->Set_PlayerStop(isStop);
}

void CMainQuest::PlayerCom_Stop(_bool isStop)
{
	dynamic_cast<CPlayerCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER))->Set_RotationBlock(isStop);
}

void CMainQuest::Free()
{
	Safe_Release(m_pUIManager);
	Safe_Release(m_pFileTotalMgr);
}
