#include "Bounding_OBB.h"

#include "DebugDraw.h"

CBounding_OBB::CBounding_OBB()
	: CBounding{}
{
}

void CBounding_OBB::Set_Value(void* pDesc)
{
	m_vExtents = *((_float3*)pDesc);

	m_pOriginalBox->Extents = m_vExtents;
}

void CBounding_OBB::Set_Center(const _float3& vCenter)
{
	m_vCenter = vCenter;

	m_pOriginalBox->Center = m_vCenter;
}

void CBounding_OBB::Set_Rotation(const _float3& vRotation)
{
	_float4		vQuaternion;
	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(vRotation.x, vRotation.y, vRotation.z));

	m_pOriginalBox->Orientation = vQuaternion;
}

HRESULT CBounding_OBB::Initialize(const void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	const BOUNDING_OBB_DESC* pDesc = static_cast<const BOUNDING_OBB_DESC*>(pArg);

	_float4		vQuaternion;
	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));

	m_vCenter = pDesc->vCenter;
	m_vExtents = pDesc->vExtents;

	m_vColor = _float4(0.f, 0.f, 1.f, 1.f);

	m_pOriginalBox = new BoundingOrientedBox(m_vCenter, m_vExtents, vQuaternion);
	m_pBoundingBox = new BoundingOrientedBox(*m_pOriginalBox);
	if (m_pBoundingBox == nullptr)
		return E_FAIL;

	return S_OK;
}

void CBounding_OBB::Tick(_fmatrix WorldMatrix)
{
	m_pOriginalBox->Transform(*m_pBoundingBox, WorldMatrix);
}

_bool CBounding_OBB::Intersect(CCollider::TYPE eTargetType, CBounding* pTargetBounding, _float fDistance )
{
	_bool isColl = { false };

	_vector vTargetPos = XMVectorZero();
	_vector vMyPos = XMLoadFloat3(&m_pBoundingBox->Center);

	switch (eTargetType)
	{
	case Engine::CCollider::COLLIDER_AABB:
	{
		BoundingBox* pDesc = static_cast<BoundingBox*>(pTargetBounding->Get_Desc());
		vTargetPos = XMLoadFloat3(&pDesc->Center);

		if (fDistance > XMVectorGetX(XMVector3Length(vTargetPos - vMyPos)))
			isColl = m_pBoundingBox->Intersects(*pDesc);
		break;
	}
	case Engine::CCollider::COLLIDER_OBB:
	{
		BoundingOrientedBox* pDesc = static_cast<BoundingOrientedBox*>(pTargetBounding->Get_Desc());
		vTargetPos = XMLoadFloat3(&pDesc->Center);

		if (fDistance > XMVectorGetX(XMVector3Length(vTargetPos - vMyPos)))
			isColl = m_pBoundingBox->Intersects(*pDesc);
		break;
	}
	case Engine::CCollider::COLLIDER_SPHERE:
	{
		BoundingSphere* pDesc = static_cast<BoundingSphere*>(pTargetBounding->Get_Desc());
		vTargetPos = XMLoadFloat3(&pDesc->Center);

		if (fDistance > XMVectorGetX(XMVector3Length(vTargetPos - vMyPos)))
			isColl = m_pBoundingBox->Intersects(*pDesc);

		break;
	}
	case Engine::CCollider::COLLIDER_END:
		break;
	default:
		break;
	}

	return isColl;
}

#ifdef _DEBUG
HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pBoundingBox, XMLoadFloat4(&m_vColor));

	return S_OK;
}
#endif // _DEBUG

CBounding_OBB* CBounding_OBB::Create(const void* pArg)
{
	CBounding_OBB* pInstance = new CBounding_OBB();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_OBB::Free()
{
	__super::Free();
}
