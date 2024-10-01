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
    m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CDecal::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCubeCom->Begin(0);
    m_pVIBufferCubeCom->Render();

    m_pShaderPosCom->Begin(0);
    m_pVIBufferRectCom->Render();

    return S_OK;
}

void CDecal::Get_Decal_Desc_IO(DECAL_DESC_IO* pDecalIODesc)
{
    pDecalIODesc->iMaterialNum = m_iMaterialNum;
    XMStoreFloat4x4(&pDecalIODesc->vTransform, m_pTransformCom->Get_WorldMatrix());
}

HRESULT CDecal::Add_Components()
{
    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_RUNMAP, TEXT("Prototype_Component_VIBuffer_Cube"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCubeCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_RUNMAP, TEXT("Prototype_Component_Shader_VtxCube"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCubeCom))))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_RUNMAP, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBufferRect"), reinterpret_cast<CComponent**>(&m_pVIBufferRectCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_RUNMAP, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_ShaderRect"), reinterpret_cast<CComponent**>(&m_pShaderPosCom))))
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
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCubeCom, "g_Texture2D", 0)))
        return E_FAIL;


    if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderPosCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderPosCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderPosCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderPosCom, "g_Texture", 0)))
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
    Safe_Release(m_pShaderPosCom);
    Safe_Release(m_pVIBufferCubeCom);
    Safe_Release(m_pVIBufferRectCom);
}
