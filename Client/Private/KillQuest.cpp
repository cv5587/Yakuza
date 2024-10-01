#include "KillQuest.h"

#include "GameInstance.h"
#include "GameObject.h"

CKillQuest::CKillQuest()
    : CQuest{}
{
}

HRESULT CKillQuest::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    KILL_QUEST_DESC* pDesc = static_cast<KILL_QUEST_DESC*>(pArg);
    m_iTargetIndex = pDesc->iTargetIndex;

    return S_OK;
}

_bool CKillQuest::Execute()
{
    //오브젝트가 죽은 상태인지 확인하기
    vector<CGameObject*> Monsters = m_pGameInstance->Get_GameObjects(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Monster"));

    for (auto& Monster : Monsters)
    {
        if (Monster->Get_ObjID() == m_iTargetIndex)
        {
            if (Monster->isObjectDead())
                return true;
        }
    }
        
    return false;
}

CKillQuest* CKillQuest::Create(void* pArg)
{
    CKillQuest* pInstance = new CKillQuest();

    if (FAILED(pInstance->Initialize(pArg)))
        Safe_Release(pInstance);

    return pInstance;
}

void CKillQuest::Free()
{
    __super::Free();
}
