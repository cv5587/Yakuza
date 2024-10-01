#include "..\Public\Decal.h"

#include "GameInstance.h"
#include "Transform.h"

CDecal::CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CDecal::CDecal(const CDecal& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CDecal::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDecal::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (nullptr != pArg)
    {
        DECALOBJ_DESC* decalDesc = (DECALOBJ_DESC*)pArg;
        /* For.Prototype_Component_Texture_Terrain */
        m_pTextureCom = decalDesc->pTexture;
        Safe_AddRef(m_pTextureCom);
        m_iMaterialNum = decalDesc->iMaterialNum;
        m_pTransformCom->Set_WorldMatrix(decalDesc->vStartPos);
    }

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CDecal::Priority_Tick(const _float& fTimeDelta)
{
}

void CDecal::Tick(const _float& fTimeDelta)
{

}

void CDecal::Late_Tick(const _float& fTimeDelta)
{
    m_pGameInstance->Add_Renderer(CRenderer::RENDER_DECAL, this);
}

HRESULT CDecal::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCubeCom->Begin(1);
    m_pVIBufferCubeCom->Render();

    //m_pShaderPosCom->Begin(1);
    //m_pVIBufferRectCom->Render();

    return S_OK;
}


HRESULT CDecal::Add_Components()
{
    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_VIBuffer_Cube"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCubeCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxCube"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCubeCom))))
        return E_FAIL;


    return S_OK;
}

HRESULT CDecal::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCubeCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCubeCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCubeCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pShaderCubeCom->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCubeCom->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCubeCom, "g_Texture", 0)))
        return E_FAIL;


    XMMATRIX worldInv = XMMatrixInverse(nullptr, m_pTransformCom->Get_WorldMatrix());
    XMFLOAT4X4 wordInvFloat;
    XMStoreFloat4x4(&wordInvFloat, worldInv);
    if (FAILED(m_pShaderCubeCom->Bind_Matrix("g_WorldMatrixInv", &wordInvFloat)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShaderCubeCom, "g_DepthTexture")))
        return E_FAIL;

    if (FAILED(m_pShaderCubeCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
        return E_FAIL;


    return S_OK;
}

CDecal* CDecal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDecal* pInstance = new CDecal(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CDecal");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDecal::Clone(void* pArg)
{
    CDecal* pInstance = new CDecal(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CDecal");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDecal::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCubeCom);
    Safe_Release(m_pVIBufferCubeCom);
}
