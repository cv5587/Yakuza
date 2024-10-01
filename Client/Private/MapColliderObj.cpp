#include "MapColliderObj.h"

#include "GameInstance.h"

CMapColliderObj::CMapColliderObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CMapColliderObj::CMapColliderObj(const CMapColliderObj& rhs)
    : CGameObject(rhs)
{
}

HRESULT CMapColliderObj::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMapColliderObj::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;


    if (FAILED(Add_Components(pArg)))
        return E_FAIL;

    COLLIDEROBJ_DESC* colliderDescobj = (COLLIDEROBJ_DESC*)pArg;
    _float3     vScale = colliderDescobj->vExtents;
    m_pTransformCom->Set_Scale(vScale.x, vScale.y, vScale.z);

    _vector     vPos = XMLoadFloat3(&colliderDescobj->vCenter);
    vPos.m128_f32[3] = 1;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


    return S_OK;
}

void CMapColliderObj::Priority_Tick(const _float& fTimeDelta)
{
}

void CMapColliderObj::Tick(const _float& fTimeDelta)
{


}

void CMapColliderObj::Late_Tick(const _float& fTimeDelta)
{
    m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CMapColliderObj::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;


    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        /*m_pShaderCom->Begin(m_iShaderPassNum);*/
        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CMapColliderObj::Add_Components(void* pArg)
{
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Bone_Sphere"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxMeshSky"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapColliderObj::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    return S_OK;
}

CMapColliderObj* CMapColliderObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMapColliderObj* pInstance = new CMapColliderObj(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CMapCollider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMapColliderObj::Clone(void* pArg)
{
    CMapColliderObj* pInstance = new CMapColliderObj(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CMapCollider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapColliderObj::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
