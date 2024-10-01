#include "Quest.h"

#include "GameInstance.h"

CQuest::CQuest()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CQuest::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	QUEST_DESC* pDesc = static_cast<QUEST_DESC*>(pArg);
	m_iType = pDesc->iType;
	m_iQuestIndex = pDesc->iQuestIndex;
	m_iNextQuestIndex = pDesc->iNextQuestIndex;

	return S_OK;
}

void CQuest::Free()
{
	Safe_Release(m_pGameInstance);
}
