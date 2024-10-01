#include "TalkQuest.h"

CTalkQuest::CTalkQuest()
    : CQuest {}
{
}

HRESULT CTalkQuest::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    TALK_QUEST_DESC* pDesc = static_cast<TALK_QUEST_DESC*>(pArg);
    m_iObjectIndex = pDesc->iObjectIndex;

    return S_OK;
}

_bool CTalkQuest::Execute()
{
    if (m_isColl)
        return true;

    return false;
}

CTalkQuest* CTalkQuest::Create(void* pArg)
{
    CTalkQuest* pInstance = new CTalkQuest();

    if (FAILED(pInstance->Initialize(pArg)))
        Safe_Release(pInstance);

    return pInstance;
}

void CTalkQuest::Free()
{
    __super::Free();
}
