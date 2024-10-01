#include "ItemData.h"

CItemData::CItemData()
{
}

CItemData::CItemData(const CItemData& rhs)
{
}


HRESULT CItemData::Initialize()
{
    return S_OK;
}


CItemData* CItemData::Create()
{
    CItemData* pInstance = new CItemData();

    if (FAILED(pInstance->Initialize()))
    {
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CItemData::Free()
{


}
