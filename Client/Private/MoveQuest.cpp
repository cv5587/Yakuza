#include "MoveQuest.h"

CMoveQuest::CMoveQuest()
    : CQuest{}
{
}

HRESULT CMoveQuest::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    MOVE_QUEST_DESC* pDesc = static_cast<MOVE_QUEST_DESC*>(pArg);
    m_iTriggerIndex = pDesc->iTriggerIndex;

    return S_OK;
}

_bool CMoveQuest::Execute()
{
    if (m_isColl)
        return true;

    return false;
}

CMoveQuest* CMoveQuest::Create(void* pArg)
{
    CMoveQuest* pInstance = new CMoveQuest();

    if (FAILED(pInstance->Initialize(pArg)))
        Safe_Release(pInstance);

    return pInstance;
}

void CMoveQuest::Free()
{
    __super::Free();
}
