#include "UILoading.h"
#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include"GameInstance.h"
CUILoading::CUILoading()
    :CUIScene{}
{
}

CUILoading::CUILoading(const CUILoading& rhs)
    :CUIScene{ rhs }
{
}

HRESULT CUILoading::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice , pContext , pArg)))
		return E_FAIL;

    return S_OK;
}

HRESULT CUILoading::Tick(const _float& fTimeDelta)
{

	__super::Tick(fTimeDelta);



	return S_OK;
}

HRESULT CUILoading::Late_Tick(const _float& fTimeDelta)
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

void CUILoading::Action()
{

}

void CUILoading::OverAction()
{
}

CUILoading* CUILoading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUILoading* pInstance = new CUILoading();
	if (FAILED(pInstance->Initialize(pDevice , pContext ,pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUILoading::Free()
{
	__super::Free();
}
