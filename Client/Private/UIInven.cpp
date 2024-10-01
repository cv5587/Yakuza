#include "UIInven.h"

#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include"GameInstance.h"
#include"InventoryManager.h"
CUIInven::CUIInven()
    :CUIScene{}
{
}

CUIInven::CUIInven(const CUIInven& rhs)
    :CUIScene{ rhs }
{
}

HRESULT CUIInven::Add_UIData(CUI_Object* pUIObject, wstring wstrPrototypeTag)
{
	if (pUIObject->Get_Event())
	{
		m_EventUI.push_back(pUIObject);
		return S_OK;
	}
	else if(CUI_Object::TYPE_BTN == pUIObject->Get_TypeIndex())
	{
		if(m_Toggle.size() <1)
		{
			m_Toggle.push_back(dynamic_cast<CBtn*>(pUIObject));
		}
		else
		{
			m_Button.push_back(dynamic_cast<CBtn*>(pUIObject));
		}
	}
	else
	{
		m_UI.push_back(pUIObject);
	}
	return S_OK;
}

HRESULT CUIInven::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice , pContext , pArg)))
		return E_FAIL;
	if (nullptr != pArg)
	{
		IVENSCENE_DESC* pDesc = static_cast<IVENSCENE_DESC*>(pArg);
		
		m_pInvenctory = pDesc->pInventory;	
		Safe_AddRef(m_pInvenctory);
	}

    return S_OK;
}

HRESULT CUIInven::Tick(const _float& fTimeDelta)
{

	__super::Tick(fTimeDelta);

	for (auto& iter : m_Toggle)
	{
		iter->Tick(fTimeDelta);
	}

	switch (m_iToggle)
	{
	case 0:
	{
		
		for (size_t i = 0; i < 20; i++)
		{
			m_Button[i]->Tick(fTimeDelta);
		}
		break;
	}
	case 1:
		break;
	case 2:
		break;
	default:
		break;
	}
	m_Toggle[m_iToggle]->Set_Click(true);
	//등장 애니메이션 끝난뒤 모든 행동 코드
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

HRESULT CUIInven::Late_Tick(const _float& fTimeDelta)
{

	__super::Late_Tick(fTimeDelta);

	for (auto& iter : m_Toggle)
	{
		iter->Late_Tick(fTimeDelta);
	}

	switch (m_iToggle)
	{
	case 0:
	{
		for (size_t i = 0; i < 20; i++)
		{
			m_Button[i]->Late_Tick(fTimeDelta);
		}
		break;
	}
	case 1:
		break;
	case 2:
		break;
	default:
		break;
	}

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

_bool CUIInven::Click_InterSect()
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
	for (size_t i = 0; i < m_Toggle.size(); i++)
	{
		if (m_Toggle[i]->Click_Intersect())
		{
			m_iToggle = i;
			Toggle_Action();
			return true;
		}
	}
	return false;
}

void CUIInven::Action()
{
	if (20 > m_iCurButton)
	{


		//인벤에서 아이템 상호 작용
	}

	
}

void CUIInven::Toggle_Action()
{
	for (size_t i = 0; i < m_Toggle.size(); i++)
	{
			m_Toggle[i]->Set_Click(false);

	}
}

void CUIInven::OverAction()
{
	//이벤트순서(자기가 만든 ui 순서를 기록해서 쓰기)
	// 1. 이펙트 2.설명 3.파티클 
	//수동으로 그룹으로 정리해서 들어온 이펙트,파티클을 기록하고 사용하자
	_matrix ButtonWorld = m_Button[m_iCurButton]->Get_TransformCom()->Get_WorldMatrix();
	_vector Position = ButtonWorld.r[3];
	ButtonWorld = XMMatrixTranslation(XMVectorGetX(Position), XMVectorGetY(Position), 0.f);

	switch (m_iToggle)
	{
	case 0:
	{

		if (m_iCurButton != m_iPrevButton)
		{
			m_Button[m_iCurButton]->Set_Click(true);

			if (-1 != m_iPrevButton)
			{
				m_Button[m_iPrevButton]->Set_Click(false);
			}
		}
		break;
	}

	case 1:
		break;

	case 2:
		break;
	default:
		break;
	}

	switch (m_iCurButton)
	{
	case 0:
	{

		//m_EventUI[0]->Show_UI();
		//m_EventUI[0]->Get_TransformCom()->Set_WorldMatrix(ButtonWorld);

		////그룹으로 제작 (설명창 다 모아두기)
		////버튼 번호 받아서 끄고 킬 이미지 관리(isPlay)
		//dynamic_cast<CGroup*>(m_EventUI[1])->Show_Choice(m_iCurButton);

		//m_EventUI[2]->Show_UI();
		//m_EventUI[2]->Get_TransformCom()->Set_WorldMatrix(ButtonWorld);
		break;
	}
	case 1:
	{
		//m_EventUI[0]->Show_UI();
		//m_EventUI[0]->Get_TransformCom()->Set_WorldMatrix(ButtonWorld);

		////그룹으로 제작 (설명창 다 모아두기)
		////버튼 번호 받아서 끄고 킬 이미지 관리(isPlay)
		//dynamic_cast<CGroup*>(m_EventUI[1])->Show_Choice(m_iCurButton);

		//m_EventUI[2]->Show_UI();
		//m_EventUI[2]->Get_TransformCom()->Set_WorldMatrix(ButtonWorld);
		break;
	}

	default:
		break;
	}
}

CUIInven* CUIInven::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUIInven* pInstance = new CUIInven();
	if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUIInven::Free()
{
	for (auto& iter : m_Toggle)
		Safe_Release(iter);

	Safe_Release(m_pInvenctory);	
	__super::Free();
}
