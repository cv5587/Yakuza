#include "UIKaraoke_Score.h"
#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include"GameInstance.h"
CUIKaraoke_Score::CUIKaraoke_Score()
	:CUIScene{}
{
}

CUIKaraoke_Score::CUIKaraoke_Score(const CUIKaraoke_Score& rhs)
	:CUIScene{ rhs }
{
}

HRESULT CUIKaraoke_Score::Show_Scene()
{
	if (FAILED(__super::Show_Scene()))
		return E_FAIL;

	m_iOnce = false;
	m_iScore = 0;

	return S_OK;
}

HRESULT CUIKaraoke_Score::Close_Scene()
{
	if (FAILED(__super::Close_Scene()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIKaraoke_Score::Add_UIData(CUI_Object* pUIObject, wstring wstrPrototypeTag)
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

HRESULT CUIKaraoke_Score::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIKaraoke_Score::Tick(const _float& fTimeDelta)
{
	if(!m_iOnce)
		ScoreUpdate();

	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	for (auto& iter : m_EventUI)
		iter->Tick(fTimeDelta);

	return S_OK;
}

HRESULT CUIKaraoke_Score::Late_Tick(const _float& fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    for (auto& iter : m_EventUI)
        iter->Late_Tick(fTimeDelta);

	if (m_pGameInstance->GetKeyState(DIK_E) == TAP)
	{
		CUIManager::GetInstance()->Close_Scene(m_strSceneName);
	}


	if (!m_isAnimFin)
		Check_AimFin();
	else
	{
		if (Over_InterSect())
		{
			OverAction();
		}
	}

	return S_OK;
}

_bool CUIKaraoke_Score::Click_InterSect()
{
	return _bool();
}

_bool CUIKaraoke_Score::Over_InterSect()
{
	return _bool();
}

void CUIKaraoke_Score::Action()
{
}

void CUIKaraoke_Score::OverAction()
{
}

void CUIKaraoke_Score::ScoreUpdate()
{

	string strScore = to_string(m_iScore);
	_int iIndex = 0;

	for (_int i = strScore.length() - 1; 0 <= i; --i)
	{
		char digit = strScore[i];
		_uint OneNum = digit - '0';
		dynamic_cast<CGroup*>(m_EventUI[iIndex])->Show_Choice(OneNum+1);
		++iIndex;
	}

	for (size_t i = iIndex; i <= HUNDRED; i++)
	{
		dynamic_cast<CGroup*>(m_EventUI[iIndex])->Show_Choice(0);
	}

	if(m_iScore<60)
		dynamic_cast<CGroup*>(m_EventUI[IMAGE])->Show_Choice(BAD);
	else if(m_iScore<85)
		dynamic_cast<CGroup*>(m_EventUI[IMAGE])->Show_Choice(GOOD);
	else if(m_iScore<=100)
		dynamic_cast<CGroup*>(m_EventUI[IMAGE])->Show_Choice(GREAT);

	m_iOnce = true;
}

CUIKaraoke_Score* CUIKaraoke_Score::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUIKaraoke_Score* pInstance = new CUIKaraoke_Score();
	if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUIKaraoke_Score::Free()
{
	__super::Free();
}
