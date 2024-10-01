#include "ModelBoneSphere.h"
#include "GameInstance.h"

CModelBoneSphere::CModelBoneSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject{pDevice, pContext}
{
}

CModelBoneSphere::CModelBoneSphere(const CModelBoneSphere& rhs)
    :CGameObject{ rhs }
{
}

HRESULT CModelBoneSphere::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CModelBoneSphere::Initialize(void* pArg)
{
    //GAMEOBJECT_DESC pDesc{};
    //pDesc.fRotatePecSec = XMConvertToRadians(90.f);

    if (FAILED(CGameObject::Initialize(pArg)))
        return E_FAIL;

    BoneSphereDesc* pDesc = static_cast<BoneSphereDesc*>(pArg);

    m_pBoneWorldMatrix = pDesc->pBoneWorldMatrix;
    m_pModelWorldMatrix = pDesc->pModelWorldMatrix;
    

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CModelBoneSphere::Priority_Tick(const _float& fTimeDelta)
{
}

void CModelBoneSphere::Tick(const _float& fTimeDelta)
{
    m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(m_pBoneWorldMatrix) * XMLoadFloat4x4(m_pModelWorldMatrix));

    if(0 == m_iTextureIndex)
        m_pTransformCom->Set_Scale(1, 1, 1);
    else
        m_pTransformCom->Set_Scale(3, 3, 3);

    if (nullptr != m_pColliderCom)
    {
        // Bone을 클릭했을 때 Sphere의 크기를 키우기 때문에, 크기를 제거한 매트릭스를 콜라이더에 적용해줘야해서 따로 만듦
        _matrix WorldMap = m_pTransformCom->Get_WorldMatrix() ;

        if (0 != m_iTextureIndex)
        {
            _matrix ScaleMatrix = XMMatrixScaling(ONE_THIRD, ONE_THIRD, ONE_THIRD);
            WorldMap = ScaleMatrix * WorldMap;
        }

        m_pColliderCom->Tick(WorldMap);

        if (m_isEventPlaying)
            m_pColliderCom->Set_Color(_float4(0, 1, 1, 1));
        else
            m_pColliderCom->Reset_Color();
    }


}

void CModelBoneSphere::Late_Tick(const _float& fTimeDelta)
{
    m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CModelBoneSphere::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    for (size_t i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndex);

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }

#ifdef _DEBUG
    if (nullptr != m_pColliderCom)
        m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

    return S_OK;
}

HRESULT CModelBoneSphere::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"),
        reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Model_Bone_Sphere"), TEXT("Com_Model"),
        reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Texture_BoneSphere"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CModelBoneSphere::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    return S_OK;
}

void CModelBoneSphere::Change_TexutreIndex(_bool isOn)
{
    if (isOn)
        m_iTextureIndex = 1;
    else
        m_iTextureIndex = 0;
}

void CModelBoneSphere::Change_ColliderColor(_bool isOn)
{
    m_isEventPlaying = isOn;
}

void CModelBoneSphere::Create_Collider(CCollider::TYPE eType, const CCollider::COLLIDER_DESC* pDesc)
{
    if (nullptr != m_pColliderCom) return;

    if ((__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Collider"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), (void*)pDesc)))
        return;

    //m_pColliderCom = static_cast<CCollider*>(m_pGameInstance->Add_Component_Clone(LEVEL_EDIT, TEXT("Prototype_Component_Collider"), ));
}

HRESULT CModelBoneSphere::Release_Collider()
{
    int iCount = Safe_Release(m_pColliderCom);
    m_Components.erase(TEXT("Com_Collider"));
    return 0 < iCount ? Safe_Release(m_pColliderCom) : S_OK;
}

CCollider::TYPE CModelBoneSphere::Get_Collider_Type()
{
    return m_pColliderCom->Get_Type();
}

const _float3& CModelBoneSphere::Get_Collider_Center()
{
    switch (m_pColliderCom->Get_Type())
    {
    case CCollider::COLLIDER_AABB:
    {
        BoundingBox* pBounding = (BoundingBox*)m_pColliderCom->Get_OriginDesc();
        return pBounding->Center;
    }
    case CCollider::COLLIDER_OBB:
    {
        BoundingOrientedBox* pBounding = (BoundingOrientedBox*)m_pColliderCom->Get_OriginDesc();
        return pBounding->Center;
    }
    case CCollider::COLLIDER_SPHERE:
    {
        BoundingSphere* pBounding = (BoundingSphere*)m_pColliderCom->Get_OriginDesc();
        return pBounding->Center;
    }
    }

    return _float3();
}

void* CModelBoneSphere::Get_Collider_Value()
{
    switch (m_pColliderCom->Get_Type())
    {
    case CCollider::COLLIDER_AABB:
    {
        BoundingBox* pBounding = (BoundingBox*)m_pColliderCom->Get_Desc();
        return (void*)&pBounding->Extents;
    }
    case CCollider::COLLIDER_OBB:
    {
        BoundingOrientedBox* pBounding = (BoundingOrientedBox*)m_pColliderCom->Get_Desc();
        return (void*)&pBounding->Extents;
    }
    case CCollider::COLLIDER_SPHERE:
    {
        BoundingSphere* pBounding = (BoundingSphere*)m_pColliderCom->Get_Desc();
        return &pBounding->Radius;
    }
    }

    return nullptr;
}

void CModelBoneSphere::Set_Collider_Center(const _float3& vCenter)
{
    if (nullptr == m_pColliderCom) return;
    m_pColliderCom->Set_Center(vCenter);
}

void CModelBoneSphere::Set_Collider_Value(void* pDesc)
{
    if (nullptr == m_pColliderCom) return;
    m_pColliderCom->Set_Value(pDesc);
}

_bool CModelBoneSphere::Created_Collider()
{
    return nullptr == m_pColliderCom ? false : true;
}

CModelBoneSphere* CModelBoneSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CModelBoneSphere* pInstance = new CModelBoneSphere(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CModelBoneSphere");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CModelBoneSphere::Clone(void* pArg)
{
    CModelBoneSphere* pInstance = new CModelBoneSphere(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CModelBoneSphere");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CModelBoneSphere::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
