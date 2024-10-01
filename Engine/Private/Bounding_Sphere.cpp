#include "Bounding_Sphere.h"

#include "DebugDraw.h"

CBounding_Sphere::CBounding_Sphere()
	: CBounding{}
{
}

void CBounding_Sphere::Set_Value(void* pDesc)
{
	m_fRadius = *((_float*)pDesc);

	m_pOriginalBox->Radius = m_fRadius;
}

void CBounding_Sphere::Set_Center(const _float3& vCenter)
{
	m_vCenter = vCenter;

	m_pOriginalBox->Center = m_vCenter;
}

HRESULT CBounding_Sphere::Initialize(const void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	const BOUNDING_SPHERE_DESC* pDesc = static_cast<const BOUNDING_SPHERE_DESC*>(pArg);

	m_fRadius = pDesc->fRadius;

	m_vColor = _float4(1.f, 0.f, 0.f, 1.f);

	m_pOriginalBox = new BoundingSphere(m_vCenter, m_fRadius);
	m_pBoundingBox = new BoundingSphere(*m_pOriginalBox);
	if (m_pBoundingBox == nullptr)
		return E_FAIL;

	return S_OK;
}

void CBounding_Sphere::Tick(_fmatrix WorldMatrix)
{
	m_pOriginalBox->Transform(*m_pBoundingBox, WorldMatrix);
}

_bool CBounding_Sphere::Intersect(CCollider::TYPE eTargetType, CBounding* pTargetBounding, _float fDistance)
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
HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pBoundingBox, XMLoadFloat4(&m_vColor));

	return S_OK;
}
#endif // _DEBUG

CBounding_Sphere* CBounding_Sphere::Create(const void* pArg)
{
	CBounding_Sphere* pInstance = new CBounding_Sphere();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginalBox);
	Safe_Delete(m_pBoundingBox);
}
