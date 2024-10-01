#include "SocketObject.h"

CSocketObject::CSocketObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPartObject{ pDevice, pContext }
{
}

CSocketObject::CSocketObject(const CSocketObject& rhs)
	:CPartObject{ rhs }
{
}

HRESULT CSocketObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSocketObject::Initialize(void* pArg)
{
	SOCKETOBJECT_DESC* pDesc = static_cast<SOCKETOBJECT_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pCombinedTransformationMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSocketObject::Priority_Tick(const _float& fTimeDelta)
{
}

void CSocketObject::Tick(const _float& fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
	//m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatrix));
}

void CSocketObject::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CSocketObject::Render()
{
	return S_OK;
}

void CSocketObject::Free()
{
	__super::Free();
}
