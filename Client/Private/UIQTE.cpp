#include "UIQTE.h"

#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include "Text.h"
#include"GameInstance.h"

CUIQTE::CUIQTE()
	:CUIScene{}
{
}

CUIQTE::CUIQTE(const CUIQTE& rhs)
	:CUIScene{ rhs }
{
}

HRESULT CUIQTE::Show_Scene()
{
	if (FAILED(__super::Show_Scene()))
		return E_FAIL;

	m_isPress = false;

	return S_OK;
}

HRESULT CUIQTE::Close_Scene()
{
	if (FAILED(__super::Close_Scene()))
		return E_FAIL;

	m_isPress = false;

	return S_OK;
}

HRESULT CUIQTE::Add_UIData(CUI_Object* pUIObject, wstring wstrPrototypeTag)
{
	if (pUIObject->Get_Event())
	{
		m_EventUI.push_back(pUIObject);

	}
	else
	{
		m_UI.push_back(pUIObject);
	}

	return S_OK;
}

HRESULT CUIQTE::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQTE::Tick(const _float& fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	if(!m_isPress)
		m_EventUI[CIRCLE]->Tick(fTimeDelta);
	else
		m_EventUI[BUTTON]->Tick(fTimeDelta);

	return S_OK;
}

HRESULT CUIQTE::Late_Tick(const _float& fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	for (auto& iter : m_EventUI)
		iter->Late_Tick(fTimeDelta);

	if (m_EventUI[BUTTON]->Check_AnimFin())
	{
		m_isAnimFin = true;
		m_isPress = false;
		CUIManager::GetInstance()->Close_Scene(TEXT("QTE"));
	}

	return S_OK;
}

void CUIQTE::Set_Key(wstring strKey)
{
	CGroup* pButtonGroup = static_cast<CGroup*>(m_EventUI[BUTTON]);

	CUI_Object* pKey = pButtonGroup->Get_PartObject(1);

	static_cast<CText*>(pKey)->Set_Text(strKey);
}

CUIQTE* CUIQTE::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUIQTE* pInstance = new CUIQTE();
	if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUIQTE::Free()
{
	__super::Free();
}
