#include "LandObject.h"  

#include "SystemManager.h"

CLandObject::CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
}
CLandObject::CLandObject(const CLandObject& rhs)
	: CGameObject{ rhs },
	m_pSystemManager{ CSystemManager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
}

HRESULT CLandObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLandObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CLandObject::Priority_Tick(const _float& fTimeDelta)
{
}

void CLandObject::Tick(const _float& fTimeDelta)
{
}

void CLandObject::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CLandObject::Render()
{

	return S_OK;
}

HRESULT CLandObject::Add_Componenets()
{
	return S_OK;
}

HRESULT CLandObject::Bind_ResourceData()
{
	return S_OK;
}

void CLandObject::Free()
{
	__super::Free();

	Safe_Release(m_pSystemManager);
}
