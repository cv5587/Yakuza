#include "UIScene.h"
#include "UI_Object.h"
#include "Btn.h"
#include"GameInstance.h"

CUIScene::CUIScene()
	:m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

CUIScene::CUIScene(const CUIScene& rhs)
{

}

HRESULT CUIScene::Add_UIData(CUI_Object* pUIObject, wstring wstrPrototypeTag)
{
	
	if (pUIObject->Get_Event())
	{
		m_EventUI.push_back(pUIObject);	
		return S_OK;
	}
	else
	{
		m_UI.push_back(pUIObject);
	}

	if (CUI_Object::TYPE_BTN == pUIObject->Get_TypeIndex())
	{
		Safe_AddRef(pUIObject);
		m_Button.push_back(dynamic_cast<CBtn*>(pUIObject));
	}


	return S_OK;
}

HRESULT CUIScene::Show_Scene()
{


	for (auto& iter : m_UI)
	{
		if (CUI_Object::TYPE_BTN != iter->Get_TypeIndex())
			iter->Show_UI();
		else
			iter->Close_UI();
	}

	for (auto& iter : m_EventUI)
	{
		if (CUI_Object::TYPE_BTN != iter->Get_TypeIndex())
			iter->Show_UI();
		else
			iter->Close_UI();
	}

	m_isAnimFin = false;
	m_isClose = false;
	m_isOpen = true;
		
		return S_OK;
}

HRESULT CUIScene::Close_Scene()
{
	for (auto& iter : m_UI)
	{
		iter->Close_UI();
	}
	
	for (auto& iter : m_EventUI)
	{
		if (CUI_Object::TYPE_BTN != iter->Get_TypeIndex())
			iter->Close_UI();
		else
			iter->Show_UI();
	}
	m_isAnimFin = false;
	m_isClose = true;
	m_isOpen = false;
	m_iCurButton = -1;
	m_iPrevButton = -1;
	return S_OK;
}

HRESULT CUIScene::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	if (nullptr != pArg)
	{
		SCENE_DESC* pDesc = static_cast<SCENE_DESC*>(pArg);
		m_isLoading = pDesc->isLoading;
		m_strSceneName = pDesc->strSceneName;
	}

	return S_OK;
}

HRESULT CUIScene::Tick(const _float& fTimeDelta)
{
	for (auto& iter : m_UI)
	{
		iter->Tick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CUIScene::Late_Tick(const _float& fTimeDelta)
{
	for (auto& iter : m_UI)
		iter->Late_Tick(fTimeDelta);


	return S_OK;
}

void CUIScene::Check_AimFin()
{
	for (auto& iter : m_UI)
		if (!iter->Check_AnimFin())
		{
			m_isAnimFin = false;
			return;
		}
	
	m_isAnimFin = true;
}

_bool CUIScene::Click_InterSect()
{
	for (size_t i = 0; i < m_Button.size(); i++)
	{
		if (m_Button[i]->Click_Intersect())
		{
			m_iCurButton = i;
			Action();
			return true;
		}
	}
	return false;
}

_bool CUIScene::Over_InterSect()
{
	for (size_t i = 0; i < m_Button.size(); i++)
	{
		if (m_Button[i]->Click_Intersect())
		{
			m_iCurButton = i;
			if (m_iPrevButton != m_iCurButton)
			{
				m_pGameInstance->StopSound(SOUND_UI);
				m_pGameInstance->PlaySound_W(TEXT("4681 [9].wav"), SOUND_UI, 0.5f);
				OverAction();
				m_iPrevButton = m_iCurButton;
				return true;
			}
			else
				return false;
		}
	}
	return false;
}

void CUIScene::Action()
{
}

void CUIScene::OverAction()
{

}

void CUIScene::Free()
{
	for (auto iter : m_UI)
		Safe_Release(iter);
	m_UI.clear();

	for (auto iter : m_Button)
		Safe_Release(iter);
	m_Button.clear();

	for (auto iter : m_EventUI)
		Safe_Release(iter);
	m_EventUI.clear();


	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);
}
