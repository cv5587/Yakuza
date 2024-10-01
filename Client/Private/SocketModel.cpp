#include "SocketModel.h"

CSocketModel::CSocketModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSocketObject{ pDevice, pContext }
{
}

CSocketModel::CSocketModel(const CSocketModel& rhs)
	:CSocketObject{ rhs }
{
}

HRESULT CSocketModel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSocketModel::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSocketModel::Priority_Tick(const _float& fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CSocketModel::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CSocketModel::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CSocketModel::Render()
{
	return S_OK;
}

void CSocketModel::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
