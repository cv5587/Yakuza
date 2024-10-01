#include "SocketEffect.h"
#include "GameInstance.h"
#include "SystemManager.h"

#include "Effect.h"

CSocketEffect::CSocketEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CSocketObject{ pDevice, pContext }
{
}

CSocketEffect::CSocketEffect(const CSocketEffect & rhs)
	: CSocketObject{ rhs }
{
}

HRESULT CSocketEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSocketEffect::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSocketEffect::Priority_Tick(const _float& fTimeDelta)
{
	if (m_isOn)
		m_pEffect->Priority_Tick(fTimeDelta);
}

void CSocketEffect::Tick(const _float& fTimeDelta)
{
	if (m_isOn)
		m_pEffect->Tick(fTimeDelta);
	__super::Tick(fTimeDelta);
}

void CSocketEffect::Late_Tick(const _float& fTimeDelta)
{
	if(m_isOn)
		m_pEffect->Late_Tick(fTimeDelta);
	__super::Late_Tick(fTimeDelta);
}

HRESULT CSocketEffect::Render()
{

	return S_OK;
}

void CSocketEffect::On()
{
	if(m_isOn==false)
	{
		m_pEffect->Reset_Buffer();
	}
	m_isOn = true;

}

HRESULT CSocketEffect::Add_Components(void* pArg)
{
	if (nullptr != pArg)
	{
		CEffect::EFFECT_DESC EffectDesc;

		SOKET_EFFECT_DESC* pDesc = static_cast<SOKET_EFFECT_DESC*>(pArg);
		EffectDesc.pWorldMatrix = &m_WorldMatrix;
		
		m_wstrEffectName = pDesc->wstrEffectName;
		m_pEffect = reinterpret_cast<CEffect*>(m_pGameInstance->Clone_Object(pDesc->wstrEffectName, &EffectDesc));
	}

	return S_OK;
}

CSocketEffect * CSocketEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSocketEffect*		pInstance = new CSocketEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CSocketEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSocketEffect::Clone(void * pArg)
{
	CSocketEffect*		pInstance = new CSocketEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CSocketEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSocketEffect::Free()
{
	__super::Free();

	if(nullptr != m_pEffect)
		Safe_Release(m_pEffect);
}
