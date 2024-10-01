#include "UINowLoading.h"
#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include"GameInstance.h"
CUINowLoading::CUINowLoading()
    :CUIScene{}
{
}

CUINowLoading::CUINowLoading(const CUINowLoading& rhs)
    :CUIScene{ rhs }
{
}

HRESULT CUINowLoading::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice, pContext,pArg)))
		return E_FAIL;


    return S_OK;
}

HRESULT CUINowLoading::Tick(const _float& fTimeDelta)
{
    __super::Tick(fTimeDelta);
    return S_OK;
}

HRESULT CUINowLoading::Late_Tick(const _float& fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

	//등장 애니메이션 끝난뒤 모든 행동 코드
	if (m_isAnimFin)
	{

		if (Over_InterSect())
		{
			OverAction();
		}


		if (-1 != m_iCurButton)
		{
			for (auto& iter : m_EventUI)
				iter->Late_Tick(fTimeDelta);
		}


	}

	if (!m_isAnimFin)
		Check_AimFin();
    return S_OK;
}

void CUINowLoading::Action()
{
}

void CUINowLoading::OverAction()
{
}

CUINowLoading* CUINowLoading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUINowLoading* pInstance = new CUINowLoading();
	if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUINowLoading::Free()
{
	__super::Free();
}
