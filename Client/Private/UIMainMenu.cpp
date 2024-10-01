#include "UIMainMenu.h"

#include "Level_Loading.h"
#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include"GameInstance.h"
CUIMainMenu::CUIMainMenu()
	:CUIScene{}
{
}

CUIMainMenu::CUIMainMenu(const CUIMainMenu& rhs)
	:CUIScene{ rhs }
{
}

HRESULT CUIMainMenu::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice , pContext , pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMainMenu::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (!m_isBGM)
	{
		m_isBGM = true;
		m_pGameInstance->PlayBGM(TEXT("Pandoras Place.mp3"), 0.7f);
	}
	
	if (m_isAnimFin)
	{

		if (-1 != m_iCurButton)
		{
			for (auto& iter : m_EventUI)
				iter->Tick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CUIMainMenu::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	for (auto& iter : m_Button)
		iter->Late_Tick(fTimeDelta);
	

	//등장 애니메이션 끝난뒤 모든 행동 코드
	if (m_isAnimFin)
	{

		Over_InterSect();



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

void CUIMainMenu::Action()
{
	switch (m_iCurButton)
	{
	case 0:
	{
		m_pGameInstance->StopSound(SOUND_UI);
		m_pGameInstance->PlaySound_W(TEXT("4681 [8].wav"), SOUND_UI, 0.5f);

		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TUTORIAL))))
			return;
		break;
	}
	case 1 :
	{
		m_pGameInstance->StopSound(SOUND_UI);
		m_pGameInstance->PlaySound_W(TEXT("4681 [8].wav"), SOUND_UI, 0.5f);

		// 레벨지정
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_KARAOKE))))

			return;
		break;
	}
	case 2:
	{
		m_pGameInstance->StopSound(SOUND_UI);
		m_pGameInstance->PlaySound_W(TEXT("4681 [8].wav"), SOUND_UI, 0.5f);

		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_DOGIMAZO_BOSS))))
			return;
		break;
	}
	default:
		break;
	}

}

void CUIMainMenu::OverAction()
{
	_matrix ButtonWorld = m_Button[m_iCurButton]->Get_TransformCom()->Get_WorldMatrix();
	_vector Position = ButtonWorld.r[3];
	ButtonWorld = XMMatrixTranslation(XMVectorGetX(Position), XMVectorGetY(Position), 0.f);

	if (m_iCurButton != m_iPrevButton)
	{
		m_Button[m_iCurButton]->Show_UI();

		if (-1 != m_iPrevButton)
		{
			m_Button[m_iPrevButton]->Close_UI();
		}
	}

	switch (m_iCurButton)
	{
	case 0:
	{
		//그룹으로 제작 (설명창 다 모아두기)
		//버튼 번호 받아서 끄고 킬 이미지 관리(isPlay)
		m_EventUI[0]->Show_UI();
		m_EventUI[0]->Get_TransformCom()->Set_WorldMatrix(ButtonWorld);

		break;
	}
	case 1:
	{
		m_EventUI[0]->Show_UI();
		m_EventUI[0]->Get_TransformCom()->Set_WorldMatrix(ButtonWorld);

		break;
	}
	case 2:
	{
		m_EventUI[0]->Show_UI();	
		m_EventUI[0]->Get_TransformCom()->Set_WorldMatrix(ButtonWorld);	

		break;
	}
	default:
		break;
	}
}

CUIMainMenu* CUIMainMenu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUIMainMenu* pInstance = new CUIMainMenu();
	if (FAILED(pInstance->Initialize(pDevice ,pContext, pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUIMainMenu::Free()
{
	__super::Free();
}
