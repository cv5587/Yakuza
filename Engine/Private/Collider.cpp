#include "Collider.h"

#include "GameInstance.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CCollider::CCollider(const CCollider& rhs)
	: 
#ifdef _DEBUG
	m_pBatch{ rhs.m_pBatch },
	m_pShader{ rhs.m_pShader },
	m_pInputLayout{ rhs.m_pInputLayout },
#endif // _DEBUG
	CComponent{ rhs }
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif // _DEBUG


}

HRESULT CCollider::Initialize_Prototype()
{
#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pShader = new BasicEffect(m_pDevice);

	m_pShader->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t		iShaderByteCodeLength = { 0 };

	m_pShader->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout);
#endif
	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	COLLIDER_DESC* pDesc = static_cast<COLLIDER_DESC*>(pArg);

	m_ColliderType = pDesc->eType;

	switch (m_ColliderType)
	{
	case COLLIDER_AABB:
		m_pCurrentBounding = CBounding_AABB::Create(pArg);
		break;
	case COLLIDER_OBB:
		m_pCurrentBounding = CBounding_OBB::Create(pArg);
		break;
	case COLLIDER_SPHERE:
		m_pCurrentBounding = CBounding_Sphere::Create(pArg);
		break;
	case COLLIDER_END:
		break;
	default:
		break;
	}

	if (nullptr == m_pCurrentBounding)
		return E_FAIL;

#ifdef _DEBUG
	m_vColor = m_pCurrentBounding->Get_Color();
#endif // _DEBUG

	return S_OK;
}

void CCollider::Tick(_fmatrix WorldMatirx)
{
	m_pCurrentBounding->Tick(WorldMatirx);
}

void* CCollider::Get_Desc()
{
	return m_pCurrentBounding->Get_Desc();
}

void* CCollider::Get_OriginDesc()
{
	return m_pCurrentBounding->Get_OriginDesc();
}

const _float3& CCollider::Get_Center()
{
	return m_pCurrentBounding->Get_Center();
}

void CCollider::Set_Value(void* pDesc)
{
	m_pCurrentBounding->Set_Value(pDesc);
}

void CCollider::Set_Center(const _float3& vCenter)
{
	m_pCurrentBounding->Set_Center(vCenter);
}

void CCollider::Set_Rotation(const _float3& vRotation)
{
	m_pCurrentBounding->Set_Rotation(vRotation);
}

#ifdef _DEBUG
void CCollider::Set_Color(const _float4& vColor)
{
	m_pCurrentBounding->Set_Color(vColor);
}

void CCollider::Reset_Color()
{
	m_pCurrentBounding->Set_Color(m_vColor);
}
#endif // _DEBUG

_bool CCollider::Intersect(CCollider* pTargetCollider, _float fDistance)
{
	return m_isColl = m_pCurrentBounding->Intersect(pTargetCollider->m_ColliderType, pTargetCollider->m_pCurrentBounding, fDistance);
}

const _float3& CCollider::ImpulseResolution(CCollider* pTargetCollider, _float fDistance)
{
	return m_pCurrentBounding->ImpulseResolution(pTargetCollider->m_ColliderType, pTargetCollider->m_pCurrentBounding, fDistance);
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	//m_pContext->GSSetShader(nullptr, nullptr, 0);

	//m_pContext->OMSetDepthStencilState(nullptr, 0);
	//_float		fTemp = {};
	//m_pContext->OMSetBlendState(nullptr, &fTemp, 0);
	//m_pContext->RSSetState(nullptr);

	m_pShader->SetWorld(XMMatrixIdentity());
	m_pShader->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pShader->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pShader->Apply(m_pContext);

	m_pBatch->Begin();

	m_pCurrentBounding->Render(m_pBatch);

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();
#ifdef _DEBUG
	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pShader);
	}

	Safe_Release(m_pInputLayout);
#endif

	Safe_Release(m_pCurrentBounding);
}
