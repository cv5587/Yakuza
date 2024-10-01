#include "KaraokeQuest.h"

CKaraokeQuest::CKaraokeQuest()
    : CQuest {}
{
}

HRESULT CKaraokeQuest::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

_bool CKaraokeQuest::Execute()
{
    if (m_isColl)
        return true;

    return false;
}

CKaraokeQuest* CKaraokeQuest::Create(void* pArg)
{
    CKaraokeQuest* pInstance = new CKaraokeQuest();

    if (FAILED(pInstance->Initialize(pArg)))
        Safe_Release(pInstance);

    return pInstance;
}

void CKaraokeQuest::Free()
{
    __super::Free();
}
