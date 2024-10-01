#include "UIFightScore.h"

#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include "Player.h"
#include"GameInstance.h"
CUIFightScore::CUIFightScore()
	:CUIScene{}
{
}

CUIFightScore::CUIFightScore(const CUIFightScore& rhs)
	:CUIScene{ rhs }
{
}

HRESULT CUIFightScore::Show_Scene()
{
	for (auto& iter : m_UI)
	{
		if (CUI_Object::TYPE_BTN != iter->Get_TypeIndex())
			iter->Show_UI();
		else
			iter->Close_UI();
	}

	m_isAnimFin = false;
	m_isClose = false;

	for (auto& iter : m_EventUI)
	{
		iter->Close_UI();
		iter->Set_isPlay(false);
	}

	for (auto& iter : m_Bonus)
	{
		iter->Close_UI();
		iter->Set_isPlay(false);
	}

	m_pAddMoney->Close_UI();
	m_pAddMoney->Set_isPlay(false);

	Set_Money();
	Money_Anim();
	m_isEnd = true;

	return S_OK;
}

HRESULT CUIFightScore::Close_Scene()
{
	if (FAILED(__super::Close_Scene()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIFightScore::Add_UIData(class CUI_Object* pUIObject, wstring wstrPrototypeTag)
{
	if (pUIObject->Get_Event())
	{
		if (m_Bonus.size() < 3)
			m_Bonus.push_back(pUIObject);
		else if (nullptr == m_pMoney)
			m_pMoney = static_cast<CGroup*>(pUIObject);
		else if(nullptr == m_pAddMoney)
			m_pAddMoney = static_cast<CGroup*>(pUIObject);
		else
			m_EventUI.push_back(pUIObject);
	}
	else
	{
		m_UI.push_back(pUIObject);
	}

	return S_OK;
}

HRESULT CUIFightScore::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIFightScore::Tick(const _float& fTimeDelta)
{
#ifdef _DEBUG

	if (m_pGameInstance->GetKeyState(DIK_8) == TAP)
	{
		AddMoney(13445);
		m_isEnd = false;
		m_iCurrentTime = 0.f;
		for (auto& iter : m_EventUI)
		{
			iter->Show_UI();
			iter->Set_isPlay(true);
		}
		m_pAddMoney->Show_UI();
		m_pAddMoney->Set_isPlay(true);

		for (auto& iter : m_Bonus)
		{
			iter->Set_isPlay(true);
			iter->Show_UI();
		}

	}
#endif // _DEBUG

	if (0 != m_iAddMoney)
	{
		m_fGetTime += fTimeDelta;
	}

	Set_Money();
	
	if(m_CurrentKiryuMoney != m_TargetKiryuMoney)
		Money_Anim();
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	if(!m_isEnd)
	{
		m_iCurrentTime += fTimeDelta;

		if (m_iCurrentTime > 5.f)
		{
			m_pGameInstance->StopSound(SOUND_UI);
			m_isEnd = true;
			for (auto& iter : m_EventUI)
				iter->Close_UI();
			for (auto& iter : m_Bonus)
				iter->Close_UI();

			CGameObject* pPlayer = m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0);

			dynamic_cast<CPlayer*>(pPlayer)->Set_Money(m_iAddMoney);

			m_pAddMoney->Close_UI();
		}
	}

	for (auto& iter : m_Bonus)
		iter->Tick(fTimeDelta);

	for (auto& iter : m_EventUI)
		iter->Tick(fTimeDelta);

	m_pMoney->Tick(fTimeDelta);
	m_pAddMoney->Tick(fTimeDelta);


	return S_OK;
}

HRESULT CUIFightScore::Late_Tick(const _float& fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	for (auto& iter : m_Bonus)
		iter->Late_Tick(fTimeDelta);

	for (auto& iter : m_EventUI)
		iter->Late_Tick(fTimeDelta);

	m_pMoney->Late_Tick(fTimeDelta);

	m_pAddMoney->Late_Tick(fTimeDelta);

	if (!m_isAnimFin)
		Check_AimFin();

	return S_OK;
}

_bool CUIFightScore::Click_InterSect()
{
	return _bool();
}

_bool CUIFightScore::Over_InterSect()
{
	return _bool();
}

void CUIFightScore::Action()
{
}

void CUIFightScore::OverAction()
{
}

void CUIFightScore::Set_Money()
{
	CGameObject* pPlayer = m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0);

	m_CurrentKiryuMoney = dynamic_cast<CPlayer*>(pPlayer)->Get_Money();

	m_TargetKiryuMoney = m_CurrentKiryuMoney + m_iAddMoney;

	if (m_fEndTime < m_fGetTime)
		m_fGetTime = m_fEndTime;

	m_iAnimMoney = m_pGameInstance->Lerp(m_CurrentKiryuMoney, m_TargetKiryuMoney, m_fGetTime / m_fEndTime);

}

void CUIFightScore::AddMoney(_uint AddMoney)
{
	m_iAddMoney = AddMoney;
	m_fGetTime = 0.f;

	vector<CUI_Object*> pMoneyGroup = m_pAddMoney->Get_PartObjects();


	string Number = to_string(m_iAddMoney);
	_int iIndex = 0;

	for (_int i = Number.length() - 1; 0 <= i; --i)
	{
		char digit = Number[i];
		_uint OneNum = digit - '0';

		_int Row = OneNum / 5;
		_int Col = OneNum % 5;


		_float U1 = Col * U;
		_float V1 = Row * V;
		_float U2 = (Col + 1) * U;
		_float V2 = (Row + 1) * V;
		dynamic_cast<CUI_Texture*>((pMoneyGroup)[iIndex])->Change_UV(_float2(U1, V1), _float2(U2, V2));
		++iIndex;

	}
	_bool isSign = false;
	_bool isAdd = false;
	for (size_t i = iIndex; i < pMoneyGroup.size(); i++)
	{
		if (!isSign)
		{
			dynamic_cast<CUI_Texture*>((pMoneyGroup)[i])->Change_UV(m_fSignStartUV, m_fSignEndUV);
			isSign = true;
		}
		else if(!isAdd)
		{
			dynamic_cast<CUI_Texture*>((pMoneyGroup)[i])->Change_UV(m_fAddStartUV, m_fAddEndUV);
			isAdd = true;
		}
		else
		{
			dynamic_cast<CUI_Texture*>((pMoneyGroup)[i])->Change_UV(_float2(0, 0), _float2(0, 0));
		}
	}

}

void CUIFightScore::StartAddMoney(_uint iAddMoney)
{
	AddMoney(iAddMoney);
	m_isEnd = false;
	m_iCurrentTime = 0.f;
	for (auto& iter : m_EventUI)
	{
		iter->Show_UI();
		iter->Set_isPlay(true);
	}
	m_pAddMoney->Show_UI();
	m_pAddMoney->Set_isPlay(true);

	for (auto& iter : m_Bonus)
	{
		iter->Set_isPlay(true);
		iter->Show_UI();
	}
}

void CUIFightScore::Money_Anim()
{
	vector<CUI_Object*> pMoneyGroup = m_pMoney->Get_PartObjects();

	string Number = to_string(m_iAnimMoney);
	_int iIndex = 0;

	for (_int i = Number.length() - 1; 0 <= i; --i)
	{
		char digit = Number[i];
		_uint OneNum = digit - '0';

		_int Row = OneNum / 5;
		_int Col = OneNum % 5;

		_float U1 = Col * U;
		_float V1 = Row * V;
		_float U2 = (Col + 1) * U;
		_float V2= (Row + 1) * V;
		dynamic_cast<CUI_Texture*>((pMoneyGroup)[iIndex])->Change_UV(_float2(U1, V1), _float2(U2, V2));
		++iIndex;

	}
	_bool isSign = false;
	for (size_t i = iIndex; i < pMoneyGroup.size(); i++)
	{
		if (!isSign)
		{
			dynamic_cast<CUI_Texture*>((pMoneyGroup)[i])->Change_UV(m_fSignStartUV, m_fSignEndUV);
			isSign = true;
		}
		else
		{
			dynamic_cast<CUI_Texture*>((pMoneyGroup)[i])->Change_UV(_float2(0, 0), _float2(0, 0));
		}
	}
	

	if (m_fGetTime >= m_fEndTime)
	{
		m_fGetTime = 0.f;
		m_CurrentKiryuMoney = m_TargetKiryuMoney;
		m_iAnimMoney = 0;
		m_iAddMoney = 0;
	}
}

CUIFightScore* CUIFightScore::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUIFightScore* pInstance = new CUIFightScore();
	if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUIFightScore::Free()
{
	__super::Free();

	for (auto& iter : m_Bonus)
		Safe_Release(iter);

	Safe_Release(m_pMoney);
	Safe_Release(m_pAddMoney);
}
