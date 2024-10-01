#include "UISkillHolligan.h"
#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include "Text.h"	
#include "Image_Texture.h"
#include"GameInstance.h"
CUISkillHolligan::CUISkillHolligan()
	:CUIScene{}
{
}

CUISkillHolligan::CUISkillHolligan(const CUISkillHolligan& rhs)
	:CUIScene{ rhs }
{
}

HRESULT CUISkillHolligan::Add_UIData(CUI_Object* pUIObject, wstring wstrPrototypeTag)
{
	if (pUIObject->Get_Event())
	{
		if(m_pBall.empty()||m_pBall.size()<8)
			m_pBall.push_back(pUIObject);
		else if (m_EventUI.empty() || m_EventUI.size() < 10)
			m_EventUI.push_back(pUIObject);
		else
			m_pGetter.push_back(pUIObject);

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

HRESULT CUISkillHolligan::Show_Scene()
{
	__super::Show_Scene();

	m_pGameInstance->StopSound(SOUND_UI);
	m_pGameInstance->PlaySound_W(TEXT("4681 [20].wav"), SOUND_UI, 1.f);

	for (size_t i = 0; i < m_pBall.size(); i++)
	{
		if (m_isSkill[i])
			m_pBall[i]->Show_UI();
		else
			m_pBall[i]->Close_UI();
	}

	for (auto& iter : m_EventUI)
		iter->Close_UI();



	return S_OK;
}

HRESULT CUISkillHolligan::Close_Scene()
{
	if (m_isGet)
	{

		m_isGet = false;
	}
	else
		__super::Close_Scene();

	return S_OK;
}

HRESULT CUISkillHolligan::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUISkillHolligan::Tick(const _float& fTimeDelta)
{

	for (size_t i = 0; i < m_UI.size(); i++)
	{
		if (m_fGetTime > 0.f)
		{
			if (3 == i)
			{
				vector<CUI_Object*>* pPartObjects = dynamic_cast<CGroup*>(m_UI[i])->Get_pPartObjects();
				(*pPartObjects)[0]->Tick(fTimeDelta);
				if (m_fGetTime > 0.25f)
					(*pPartObjects)[1]->Tick(fTimeDelta);
			}
			else
				m_UI[i]->Tick(fTimeDelta);
		}
		else
		{
			if (2 != i && 3 != i)
				m_UI[i]->Tick(fTimeDelta);
		}

	}

	for (auto& iter : m_pBall)
		iter->Tick(fTimeDelta);


	if (-1 != m_iCurButton)
	{
		m_EventUI[0]->Tick(fTimeDelta);
		m_EventUI[m_iCurButton + 1]->Tick(fTimeDelta);

		if (m_isSkill[m_iCurButton])
			m_EventUI[7]->Tick(fTimeDelta);
	}

	if (m_isGet)
	{
		CUI_Object* pGauge = dynamic_cast<CGroup*>(m_pGetter[1])->Get_PartObject(0);


		if (m_pGameInstance->GetKeyState(DIK_E) == HOLD)
		{

			if (m_fGetTime <= m_fEndTime)
			{
				m_pGameInstance->PlaySound_W(TEXT("4681 [31].wav"), SOUND_UI, 0.5f);
				m_fGetTime += fTimeDelta;

				_float CurGauge = 1 - (m_fGetTime / m_fEndTime);
				dynamic_cast<CImage_Texture*>(pGauge)->Change_Point(_float4(CurGauge, 0.f, 0.f, 0.f), _float4(CurGauge, 0.f, 0.f, 0.f));
			}

			if (m_fGetTime > m_fEndTime)
			{
				if (!m_isMoneyEndStart)
				{
					m_pGameInstance->StopSound(SOUND_UI);
					m_pGameInstance->PlaySound_W(TEXT("4681 [43].wav"), SOUND_UI, 0.5f);
					m_isMoneyEndStart = true;
				}

				m_fGetTime = m_fEndTime;
				m_isFinMoney = true;
			}
		}
		else
		{
			if (m_fGetTime >= 0)
			{
				m_fGetTime -= fTimeDelta;


				_float CurGauge = 1 - (m_fGetTime / m_fEndTime);
				if (1 >= CurGauge)
				{
					dynamic_cast<CImage_Texture*>(pGauge)->Change_Point(_float4(CurGauge, 0.f, 0.f, 0.f), _float4(CurGauge, 0.f, 0.f, 0.f));
				}
			}

			if (0 > m_fGetTime)
				m_fGetTime = 0;

		}
		Set_Money(fTimeDelta);

		for (size_t i = 0; i < 3; i++)
		{
			m_pGetter[i]->Tick(fTimeDelta);
		}
		if (m_isFinMoney)
		{
			if (!m_isFinishMoney)
			{
				m_pGameInstance->StopSound(SOUND_EFFECT);
				m_pGameInstance->PlaySound_W(TEXT("4681 [41].wav"), SOUND_EFFECT, 0.5f);
				m_isFinishMoney = true;
			}

			vector<CUI_Object*>* pPartObjects = dynamic_cast<CGroup*>(m_pGetter[4])->Get_pPartObjects();
			if (!m_fCurFinAnim)
			{
				m_fCurFinMoney += fTimeDelta;
				(*pPartObjects)[0]->Tick(fTimeDelta);
				(*pPartObjects)[1]->Tick(fTimeDelta);
				(*pPartObjects)[5]->Tick(fTimeDelta);
				if (0.25f < m_fCurFinMoney)
				{
					(*pPartObjects)[2]->Tick(fTimeDelta);
					(*pPartObjects)[6]->Tick(fTimeDelta);
				}
				if (0.5f < m_fCurFinMoney)
				{
					(*pPartObjects)[3]->Tick(fTimeDelta);
					(*pPartObjects)[7]->Tick(fTimeDelta);
				}
				if (0.75f < m_fCurFinMoney)
				{
					(*pPartObjects)[4]->Tick(fTimeDelta);
					(*pPartObjects)[8]->Tick(fTimeDelta);
				}
				if (dynamic_cast<CUI_Texture*>((*pPartObjects)[8])->Check_AnimFin())
				{


					m_fCurFinAnim = true;
					(*pPartObjects)[9]->Show_UI();
				}
			}
			else
			{
				(*pPartObjects)[0]->Tick(fTimeDelta);
				(*pPartObjects)[9]->Tick(fTimeDelta);
			}
		}
	}

	return S_OK;
}

HRESULT CUISkillHolligan::Late_Tick(const _float& fTimeDelta)
{
	for (size_t i = 0; i < m_UI.size(); i++)
	{
		if (m_fGetTime > 0.f)
			m_UI[i]->Late_Tick(fTimeDelta);
		else
		{
			if (2 != i && 3 != i)
				m_UI[i]->Late_Tick(fTimeDelta);
		}
	}

	for (auto& iter : m_pBall)
		iter->Late_Tick(fTimeDelta);

	if (-1 != m_iCurButton)
	{
		m_EventUI[0]->Late_Tick(fTimeDelta);
		m_EventUI[m_iCurButton + 1]->Late_Tick(fTimeDelta);

		if (m_isSkill[m_iCurButton])
			m_EventUI[7]->Late_Tick(fTimeDelta);

	}

	if (m_isGet)
	{
		for (size_t i = 0; i < 3; i++)
		{
			m_pGetter[i]->Late_Tick(fTimeDelta);
		}
		m_pGetter[3]->Late_Tick(fTimeDelta);

		if (m_isFinMoney)
		{
			vector<CUI_Object*>* pPartObjects = dynamic_cast<CGroup*>(m_pGetter[4])->Get_pPartObjects();
			if (!m_fCurFinAnim)
			{
				m_fCurFinMoney += fTimeDelta;
				(*pPartObjects)[0]->Late_Tick(fTimeDelta);

				(*pPartObjects)[1]->Late_Tick(fTimeDelta);
				(*pPartObjects)[5]->Late_Tick(fTimeDelta);

				if (0.25f < m_fCurFinMoney)
				{
					(*pPartObjects)[2]->Late_Tick(fTimeDelta);
					(*pPartObjects)[6]->Late_Tick(fTimeDelta);
				}
				if (0.5f < m_fCurFinMoney)
				{
					(*pPartObjects)[3]->Late_Tick(fTimeDelta);
					(*pPartObjects)[7]->Late_Tick(fTimeDelta);
				}
				if (0.75f < m_fCurFinMoney)
				{
					(*pPartObjects)[4]->Late_Tick(fTimeDelta);
					(*pPartObjects)[8]->Late_Tick(fTimeDelta);
				}
				if (dynamic_cast<CUI_Texture*>((*pPartObjects)[8])->Check_AnimFin())
					m_fCurFinAnim = true;
			}
			else
			{
				(*pPartObjects)[0]->Late_Tick(fTimeDelta);
				(*pPartObjects)[9]->Late_Tick(fTimeDelta);
				if (dynamic_cast<CUI_Texture*>((*pPartObjects)[9])->Check_AnimFin())
				{
					m_fCurFinAnim = false;
					m_isFinMoney = false;
					m_isGet = false;
					m_isSkill[m_iCurButton] = true;
					m_pBall[m_iCurButton]->Show_UI();
					m_fGetTime = 0.f;
					m_fCurFinMoney = 0.f;
					(*pPartObjects)[9]->Close_UI();
					m_UI[3]->Close_UI();
				}

			}
		}
	}

	if (!m_isAnimFin)
		Check_AimFin();
	else
	{
		if (!m_isGet)
		{
			if (Over_InterSect())
			{
				OverAction();
			}
		}
	}
	return S_OK;
}

_bool CUISkillHolligan::Click_InterSect()
{
	for (size_t i = 0; i < m_pBall.size(); i++)
	{
		if (m_pBall[i]->Click_Intersect(0))
		{
			m_iCurButton = i;
			Action();
			return true;
		}
	}
	return false;
}

_bool CUISkillHolligan::Over_InterSect()
{
	for (size_t i = 0; i < m_pBall.size(); i++)
	{
		if (m_pBall[i]->Click_Intersect(0))
		{
			m_iCurButton = i;
			if (m_iPrevButton != m_iCurButton)
			{
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

void CUISkillHolligan::Action()
{

	if (!m_isSkill[m_iCurButton])
	{
		m_pGameInstance->StopSound(SOUND_UI);
		m_pGameInstance->PlaySound_W(TEXT("4681 [8].wav"), SOUND_UI, 0.5f);

		CUI_Object* pGauge = dynamic_cast<CGroup*>(m_pGetter[1])->Get_PartObject(0);
		dynamic_cast<CImage_Texture*>(pGauge)->Change_Point(_float4(1.f, 0.f, 0.f, 0.f), _float4(1.f, 0.f, 0.f, 0.f));

		(m_pGetter)[4]->Show_UI();
		m_UI[3]->Show_UI();
		m_isGet = true;
		m_isMoneyEndStart = false;
		m_isFinishMoney = false;
	}
}

void CUISkillHolligan::OverAction()
{
	if (m_iCurButton != m_iPrevButton)
	{
		m_pGameInstance->StopSound(SOUND_UI);
		m_pGameInstance->PlaySound_W(TEXT("4681 [9].wav"), SOUND_UI, 0.5f);

		m_EventUI[0]->Show_UI();
		m_EventUI[m_iCurButton + 1]->Show_UI();
		if (-1 != m_iPrevButton)
		{
			m_EventUI[m_iPrevButton + 1]->Close_UI();
		}
		if (m_isSkill[m_iCurButton])
		{
			m_EventUI[7]->Show_UI();
		}
	}

}

void CUISkillHolligan::Set_Skill(_int Index)
{

}

void CUISkillHolligan::Set_Money(const _float& fTimeDelta)
{
	vector<CUI_Object*>* pPartObjects = dynamic_cast<CGroup*>(m_pGetter[3])->Get_pPartObjects();

	m_iCurMoney = m_pGameInstance->Lerp(0, m_iPrice[m_iCurButton], m_fGetTime / m_fEndTime);

	string Number = to_string(m_iCurMoney);
	_int iIndex = 0;

	for (_int i = Number.length() - 1; 0 <= i; --i)
	{
		char digit = Number[i];
		_uint OneNum = digit - '0';
		dynamic_cast<CUI_Texture*>((*pPartObjects)[iIndex])->Change_UV(_float2(0.98, m_ImageNumber[OneNum].x), _float2(0.994, m_ImageNumber[OneNum].y));
		dynamic_cast<CUI_Texture*>((*pPartObjects)[iIndex])->Tick(fTimeDelta);
		++iIndex;

	}

	for (size_t i = iIndex; i < pPartObjects->size(); i++)
	{
		dynamic_cast<CUI_Texture*>((*pPartObjects)[i])->Change_UV(_float2(0.98, m_ImageNumber[0].x), _float2(0.994, m_ImageNumber[0].y));
		dynamic_cast<CUI_Texture*>((*pPartObjects)[i])->Tick(fTimeDelta);
	}
}

CUISkillHolligan* CUISkillHolligan::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUISkillHolligan* pInstance = new CUISkillHolligan();
	if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
	{
		MSG_BOX("Make Failed :CUISkillHolligan");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUISkillHolligan::Free()
{
	__super::Free();
	for (auto& iter : m_pBall)
		Safe_Release(iter);
	for (auto& iter : m_pGetter)
		Safe_Release(iter);
}
