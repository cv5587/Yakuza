#include "UIStoreImage.h"
#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include"GameInstance.h"
CUIStoreImage::CUIStoreImage()
    :CUIScene{}
{
}

CUIStoreImage::CUIStoreImage(const CUIStoreImage& rhs)
    :CUIScene{ rhs }
{
}

void CUIStoreImage::Close_Image()
{
    Close_Scene();
}

HRESULT CUIStoreImage::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
        return E_FAIL;
    return S_OK;
}

HRESULT CUIStoreImage::Tick(const _float& fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    for (auto& iter : m_EventUI)
        iter->Tick(fTimeDelta);
    return S_OK;
}

HRESULT CUIStoreImage::Late_Tick(const _float& fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;
    for (auto& iter : m_EventUI)
        iter->Late_Tick(fTimeDelta);
    return S_OK;
}

CUIStoreImage* CUIStoreImage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CUIStoreImage* pInstance = new CUIStoreImage();
    if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CUIStoreImage::Free()
{
    __super::Free();
}
