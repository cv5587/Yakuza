#include "Land.h"
#include "GameInstance.h"
CLand::CLand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CLand::CLand(const CLand& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CLand::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLand::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CLand::Priority_Tick(const _float& fTimeDelta)
{

}

void CLand::Tick(const _float& fTimeDelta)
{

}

void CLand::Late_Tick(const _float& fTimeDelta)
{
    m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CLand::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CLand::Add_Components()
{
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_UVGrid"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLand::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;


    return S_OK;
}

CLand* CLand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLand* pInstance = new CLand(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CLand");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLand::Clone(void* pArg)
{
    CLand* pInstance = new CLand(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CLand");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLand::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);

        Safe_Release(m_pModelCom);
    

}
