#include "Bounding_AABB.h"

#include "DebugDraw.h"

CBounding_AABB::CBounding_AABB()
	: CBounding{}
{
}

void CBounding_AABB::Set_Value(void* pDesc)
{
	m_vExtents = *((_float3*)pDesc);

	m_pOriginalBox->Extents = m_vExtents;
}

void CBounding_AABB::Set_Center(const _float3& vCenter)
{
	m_vCenter = vCenter;

	m_pOriginalBox->Center = m_vCenter;
}

void CBounding_AABB::Set_Rotation(const _float3& vRotation)
{
}

HRESULT CBounding_AABB::Initialize(const void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	const BOUNDING_AABB_DESC* pDesc = static_cast<const BOUNDING_AABB_DESC*>(pArg);
	m_vCenter = pDesc->vCenter;
	m_vExtents = pDesc->vExtents;

	m_vColor = _float4(0.f, 1.f, 0.f, 1.f);

	m_pOriginalBox = new BoundingBox(m_vCenter, m_vExtents);
	m_pBoundingBox = new BoundingBox(*m_pOriginalBox);
	if (nullptr == m_pBoundingBox)
		return E_FAIL;

	return S_OK;
}

void CBounding_AABB::Tick(_fmatrix WorldMatrix)
{
	_matrix		TransformMatrix = WorldMatrix;


	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[0]);
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[1]);
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(WorldMatrix.r[2]);

	m_pOriginalBox->Transform(*m_pBoundingBox, TransformMatrix);
}

_bool CBounding_AABB::Intersect(CCollider::TYPE eTargetType, CBounding* pTargetBounding, _float fDistance)
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

#pragma optimize("", off)
const _float3& CBounding_AABB::ImpulseResolution(CCollider::TYPE eTargetType, CBounding* pTargetBounding, _float fDistance)
{
	static _float3 vResultPosition(0.f, 0.f, 0.f);
	const float scaleFactor = 0.2f; // 밀려나는 값을 줄이는 스케일 팩터 (0.5로 설정)

	// 밀어내기 기능은 AABB에서만 할 것
	if (!Intersect(eTargetType, pTargetBounding, fDistance) || eTargetType != Engine::CCollider::COLLIDER_AABB)
	{
		vResultPosition = _float3(0.f, 0.f, 0.f);
		return vResultPosition;
	}

	BoundingBox* pDesc = static_cast<BoundingBox*>(pTargetBounding->Get_Desc());
	vResultPosition = pDesc->Center;

	if (!m_pBoundingBox->Intersects(*pDesc))
	{
		vResultPosition = _float3(0.f, 0.f, 0.f);
		return vResultPosition;
	}

	XMVECTOR vBox1Min = XMVectorSubtract(XMLoadFloat3(&m_pBoundingBox->Center), XMLoadFloat3(&m_pBoundingBox->Extents));
	XMVECTOR vBox1Max = XMVectorAdd(XMLoadFloat3(&m_pBoundingBox->Center), XMLoadFloat3(&m_pBoundingBox->Extents));

	XMVECTOR vBox2Min = XMVectorSubtract(XMLoadFloat3(&pDesc->Center), XMLoadFloat3(&pDesc->Extents));
	XMVECTOR vBox2Max = XMVectorAdd(XMLoadFloat3(&pDesc->Center), XMLoadFloat3(&pDesc->Extents));

	XMVECTOR vIntersectMin = XMVectorMax(vBox1Min, vBox2Min);
	XMVECTOR vIntersectMax = XMVectorMin(vBox1Max, vBox2Max);

	XMVECTOR vIntersectSize = XMVectorSubtract(vIntersectMax, vIntersectMin);

	float fIntersectWidth = XMVectorGetX(vIntersectSize);
	float fIntersectHeight = XMVectorGetY(vIntersectSize);
	float fIntersectDepth = XMVectorGetZ(vIntersectSize);

	// Determine the smallest axis to push out along
	if (fIntersectWidth < fIntersectHeight && fIntersectWidth < fIntersectDepth)
	{
		// Push out along the X-axis
		float fPushX = (m_pBoundingBox->Center.x < pDesc->Center.x) ? -fIntersectWidth : fIntersectWidth;
		vResultPosition = _float3(fPushX * scaleFactor, 0.0f, 0.0f);
	}
	else if (fIntersectHeight < fIntersectDepth)
	{
		// Push out along the Y-axis
		float fPushY = (m_pBoundingBox->Center.y < pDesc->Center.y) ? -fIntersectHeight : fIntersectHeight;
		vResultPosition = _float3(0.0f, fPushY * scaleFactor, 0.0f);
	}
	else
	{
		// Push out along the Z-axis
		float fPushZ = (m_pBoundingBox->Center.z < pDesc->Center.z) ? -fIntersectDepth : fIntersectDepth;
		vResultPosition = _float3(0.0f, 0.0f, fPushZ * scaleFactor);
	}

	return vResultPosition;
}
#pragma optimize("", on)

#ifdef _DEBUG
HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pBoundingBox, XMLoadFloat4(&m_vColor));

	return S_OK;
}
#endif // _DEBUG

CBounding_AABB* CBounding_AABB::Create(const void* pArg)
{
	CBounding_AABB* pInstance = new CBounding_AABB();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginalBox);
	Safe_Delete(m_pBoundingBox);
}
