#include "SocketCollider.h"
#include "GameInstance.h"

#include "Effect.h"

#include "LandObject.h"

CSocketCollider::CSocketCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CSocketObject{ pDevice, pContext }
{
}

CSocketCollider::CSocketCollider(const CSocketCollider & rhs)
	: CSocketObject{ rhs }
{
}

HRESULT CSocketCollider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSocketCollider::Initialize(void * pArg)
{
	SOCKET_COLLIDER_DESC* pDesc = static_cast<SOCKET_COLLIDER_DESC*>(pArg);
	m_pParentObject = pDesc->pParentObject;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	// ATTACK, HIT
	if (pDesc->ColliderState.iActionType == 0)
	{
		// HAND_A, FOOT_A, JOINT_A, HEAD_A
		switch (pDesc->ColliderState.iPartType)
		{
		case 0:
		{
			m_fDamage = 8.f;			// 원래는 8임

			break;
		}
		case 1:
		{
			m_fDamage = 16.f;

			break;
		}
		case 2:
		{
			m_fDamage = 13.f;

			break;
		}
		case 3:
		{
			m_fDamage = 8.f;
			break;
		}
		}
	}

	return S_OK;
}

void CSocketCollider::Priority_Tick(const _float& fTimeDelta)
{
}

void CSocketCollider::Tick(const _float& fTimeDelta)
{
	Filtering_Timer(fTimeDelta);

	m_vPrevMovePos = m_vMovePos;

	__super::Tick(fTimeDelta);

	if(m_pColliderCom)
		m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

	memcpy(&m_vMovePos, m_WorldMatrix.m[CTransform::STATE_POSITION], sizeof(_float3));

	_vector vDirection = XMVector3Normalize(XMLoadFloat3(&m_vPrevMovePos) - XMLoadFloat3(&m_vMovePos));
	XMStoreFloat3(&m_vMoveDir, vDirection);
}

void CSocketCollider::Late_Tick(const _float& fTimeDelta)
{
#ifdef _DEBUG
	if (m_pColliderCom && m_isOn)
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CSocketCollider::Render()
{	
	return S_OK;
}

_bool CSocketCollider::Intersect(CCollider* pTargetObject, _float fDistance)
{
	if (pTargetObject->Intersect(m_pColliderCom, fDistance))
	{
		if (!m_isFiltered)
		{
			m_isFiltered = true;
			return true;
		}
	}
		
	return false;
}

void CSocketCollider::Filtering_Timer(_float fTimeDelta)
{
	if (!m_isFiltered) return; 

	m_fFilteringTimer += fTimeDelta;

	if (FILTERING_TIME <= m_fFilteringTimer)
	{
		m_fFilteringTimer = 0.f;
		m_isFiltered = false;
	}
}

void CSocketCollider::ParentObject_Attack(CGameObject* pHitObejct, _bool isItem)
{
	// 어택했을 때 공격한 콜라이더에서 실행할 이벤트
	if(!isItem)
		m_pParentObject->Attack_Event(static_cast<CSocketCollider*>(pHitObejct)->Get_Parent(), isItem);
	else
		m_pParentObject->Attack_Event(pHitObejct, isItem);
}

void CSocketCollider::ParentObject_Hit(CSocketCollider* pAttackedCollider)
{
	// 나의 부모객체(플레이어 or 몬스터)의 데미지 입는 함수 실행
	m_pParentObject->Take_Damage(m_eColliderPartType, pAttackedCollider->Get_MoveDir(), pAttackedCollider->Get_Damage() * pAttackedCollider->Get_Parent()->Get_DamageAmplify(), pAttackedCollider->Get_Parent());
}

HRESULT CSocketCollider::Add_Components(void* pArg)
{
	SOCKET_COLLIDER_DESC* pDesc = static_cast<SOCKET_COLLIDER_DESC*>(pArg);

	//pDesc->iType는 소켓 콜라이더 타입
	m_eColliderActionType = static_cast<COLLIDER_ACTION_TYPE>(pDesc->ColliderState.iActionType);
	m_eColliderPartType = static_cast<COLLIDER_PART_TYPE>(pDesc->ColliderState.iPartType);

	// ColliderState.iType는 바운딩 구조체 생성용 타입
	switch (pDesc->ColliderState.iType)
	{
	case 0:			//AABB
	{
		CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

		ColliderDesc.eType = CCollider::COLLIDER_AABB;
		ColliderDesc.vExtents = pDesc->ColliderState.vValue;
		ColliderDesc.vCenter = pDesc->ColliderState.vCenter;

		if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
			return E_FAIL;
		break;
	}
	case 1:			//OBB
	{
		CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

		ColliderDesc.eType = CCollider::COLLIDER_OBB;
		ColliderDesc.vExtents = pDesc->ColliderState.vValue;
		ColliderDesc.vCenter = pDesc->ColliderState.vCenter;
		ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);

		if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
			return E_FAIL;
		break;
	}
	case 2:			//SPHERE
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

		ColliderDesc.eType = CCollider::COLLIDER_SPHERE;
		ColliderDesc.fRadius = pDesc->ColliderState.vValue.x;
		ColliderDesc.vCenter = pDesc->ColliderState.vCenter;

		if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
			return E_FAIL;
		break;
	}
	}


	return S_OK;
}

HRESULT CSocketCollider::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CSocketCollider * CSocketCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSocketCollider*		pInstance = new CSocketCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CSocketCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSocketCollider::Clone(void * pArg)
{
	CSocketCollider*		pInstance = new CSocketCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CSocketCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSocketCollider::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
}
