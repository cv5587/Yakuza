#include "PlayerCamera.h"
#include "GameInstance.h"

CPlayerCamera::CPlayerCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CPlayerCamera::CPlayerCamera(const CPlayerCamera& rhs)
	: CCamera { rhs }
{
}

HRESULT CPlayerCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerCamera::Priority_Tick(const _float& fTimeDelta)
{
}

void CPlayerCamera::Tick(const _float& fTimeDelta)
{
	_float fEditTimeDelta = m_pGameInstance->Get_TimeDelta(TEXT("Timer_Edit"));

	if (m_pGameInstance->GetKeyState(DIK_UP) == HOLD)
		m_pTransformCom->Go_Straight(fEditTimeDelta);
	if (m_pGameInstance->GetKeyState(DIK_DOWN) == HOLD)
		m_pTransformCom->Go_Backward(fEditTimeDelta);
	if (m_pGameInstance->GetKeyState(DIK_LEFT) == HOLD)
		m_pTransformCom->Go_Left(fEditTimeDelta);
	if (m_pGameInstance->GetKeyState(DIK_RIGHT) == HOLD)
		m_pTransformCom->Go_Right(fEditTimeDelta);
	if (m_pGameInstance->GetKeyState(DIK_PGUP) == HOLD)
		m_pTransformCom->Go_Up(fEditTimeDelta);
	if (m_pGameInstance->GetKeyState(DIK_PGDN) == HOLD)
		m_pTransformCom->Go_Down(fEditTimeDelta);

	m_WorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
		
	__super::Tick(fEditTimeDelta);
}

void CPlayerCamera::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CPlayerCamera::Render()
{
	return S_OK;
}

CPlayerCamera* CPlayerCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerCamera* pInstance = new CPlayerCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPlayerCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayerCamera::Clone(void* pArg)
{
	CPlayerCamera* pInstance = new CPlayerCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayerCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerCamera::Free()
{
	__super::Free();
}
