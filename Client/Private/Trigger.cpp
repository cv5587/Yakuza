#include "Trigger.h"

CTrigger::CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTrigger::CTrigger(const CTrigger& rhs)
    : CGameObject { rhs }
{
}

HRESULT CTrigger::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTrigger::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;


    m_WorldMatrix = XMMatrixIdentity();


    return S_OK;
}

void CTrigger::Priority_Tick(const _float& fTimeDelta)
{
}

void CTrigger::Tick(const _float& fTimeDelta)
{

}

void CTrigger::Late_Tick(const _float& fTimeDelta)
{
#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CTrigger::Render()
{


    return S_OK;
}

bool CTrigger::Move_Scene(int& iLevelNum)
{
    return false;
}



HRESULT CTrigger::Add_Components(void* pArg)
{
    //TRIGGEROBJ_DESC* triggerDesc = (TRIGGEROBJ_DESC*)pArg;

    //m_tTriggerDesc = triggerDesc->tTriggerDesc;
    //

    //CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

    //
    //
    //ColliderDesc.eType = CCollider::COLLIDER_AABB;
    //ColliderDesc.vExtents = _float3(triggerDesc->tTriggerDesc.vTransform._11, triggerDesc->tTriggerDesc.vTransform._22, triggerDesc->tTriggerDesc.vTransform._33);
    //ColliderDesc.vCenter = _float3(triggerDesc->tTriggerDesc.vTransform._41, triggerDesc->tTriggerDesc.vTransform._42 , triggerDesc->tTriggerDesc.vTransform._43 );

    //if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
    //    TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
    //    return E_FAIL;

    return S_OK;
}

void CTrigger::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
    Safe_Release(m_pShaderCom);
}
