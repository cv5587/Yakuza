#include "PartObject.h"

CPartObject::CPartObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPartObject::CPartObject(const CPartObject & rhs)
	: CGameObject{ rhs }
{
}

HRESULT CPartObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPartObject::Initialize(void * pArg)
{
	PARTOBJ_DESC*		pPartObjDesc = (PARTOBJ_DESC*)pArg;

	m_pParentMatrix = pPartObjDesc->pParentMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPartObject::Priority_Tick(const _float& fTimeDelta)
{
}

void CPartObject::Tick(const _float& fTimeDelta)
{
}

void CPartObject::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CPartObject::Render()
{
	return S_OK;
}


void CPartObject::Free()
{
	__super::Free();
}
