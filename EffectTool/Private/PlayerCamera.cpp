#include "PlayerCamera.h"

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
	return E_NOTIMPL;
}

HRESULT CPlayerCamera::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CPlayerCamera::Priority_Tick(const _float& fTimeDelta)
{
}

void CPlayerCamera::Tick(const _float& fTimeDelta)
{
}

void CPlayerCamera::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CPlayerCamera::Render()
{
	return E_NOTIMPL;
}

CPlayerCamera* CPlayerCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CPlayerCamera::Clone(void* pArg)
{
	return nullptr;
}

void CPlayerCamera::Free()
{
}
