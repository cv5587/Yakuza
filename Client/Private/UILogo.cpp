#include "UILogo.h"

#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include"GameInstance.h"
CUILogo::CUILogo()
	:CUIScene{}
{
}

CUILogo::CUILogo(const CUILogo& rhs)
	:CUIScene{ rhs }
{
}


HRESULT CUILogo::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice , pContext , pArg)))	
		return E_FAIL;
	return S_OK;
}

HRESULT CUILogo::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (!m_isBGMStart)
	{
		m_isBGMStart = true;
		m_pGameInstance->PlaySound_W(TEXT("title_logo_0.wav"), SOUND_EFFECT);
	}

	return S_OK;
}

HRESULT CUILogo::Late_Tick(const _float& fTimeDelta)
{

	__super::Late_Tick(fTimeDelta);

	if (m_pGameInstance->GetKeyState(DIK_SPACE) == TAP)
	{
		CUIManager::GetInstance()->Open_Scene(TEXT("MainMenu"));
	}
	//등장 애니메이션 끝난뒤 모든 행동 코드

	if (!m_isAnimFin)
		Check_AimFin();

	return S_OK;
}

void CUILogo::Action()
{
}

void CUILogo::OverAction()
{
}

CUILogo* CUILogo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,void* pArg)
{
	CUILogo* pInstance = new CUILogo();
	if (FAILED(pInstance->Initialize(pDevice ,pContext ,pArg)))
	{
		MSG_BOX("Make Failed :CUILogo");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUILogo::Free()
{
	__super::Free();
}
