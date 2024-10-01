#include "UITalk.h"

#include"GameInstance.h"

#include "ScriptManager.h"

#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include "Text.h"

CUITalk::CUITalk()
    :CUIScene{}
{
}

CUITalk::CUITalk(const CUITalk& rhs)
    :CUIScene{ rhs }
{
}

HRESULT CUITalk::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
        return E_FAIL;

	m_pScriptManager = CScriptManager::Create();
	if (nullptr == m_pScriptManager)
		return E_FAIL;

	Safe_AddRef(m_pScriptManager);

    return S_OK;
}

HRESULT CUITalk::Tick(const _float& fTimeDelta)
{
	m_PlayTime += fTimeDelta;
    __super::Tick(fTimeDelta);

	if(!m_TalkFin)
	{
		if(m_PlayTime > 0.04f)
		{
			m_PlayTime = 0.f;
			Talk_Effect(fTimeDelta);
			m_pGameInstance->PlaySound_W(TEXT("word_03_0.wav"), SOUND_WORD, 0.5f);
		}
	}
	else
	{
		if(m_pGameInstance->GetKeyState(DIK_E) == TAP)
		{
			//E누르면 다음대화[다른곳에서 해도됨]
			Ready_Talk();
			
			if (false == Read_Script())
				Close_Scene();
		}
	}

	for (size_t i = 0; i < m_LineIndex+1; i++)
	{
		m_EventUI[i]->Tick(fTimeDelta);
	}

	m_EventUI[NAME]->Tick(fTimeDelta);

    return S_OK;
}

HRESULT CUITalk::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	for (size_t i = 0; i < m_LineIndex + 1; i++)
	{
		m_EventUI[i]->Late_Tick(fTimeDelta);
	}

	m_EventUI[NAME]->Late_Tick(fTimeDelta);

	if (!m_isAnimFin)
		Check_AimFin();

	return S_OK;
}

void CUITalk::Action()
{
}

void CUITalk::OverAction()
{
}

void CUITalk::Start_Talk(_uint iScriptIndex)
{
	m_iQuestIndex = 0;
	m_iCurrentScriptIndex = iScriptIndex;
	m_isFinished = false;

	Ready_Talk();
	Read_Script();
}

_bool CUITalk::Read_Script()
{
	CScriptManager::SCRIPT_INFO Script = m_pScriptManager->Get_Script(m_iCurrentScriptIndex, m_iQuestIndex);

	if (Script.strName == TEXT("끝"))
	{
		m_isFinished = true;
		return false;
	}

	dynamic_cast<CText*>(m_EventUI[NAME])->Set_Text(Script.strName);
	m_TalkData = Script.strLine;

	m_iQuestIndex++;

	return true;
}

void CUITalk::Ready_Talk()
{
	m_TalkFin = false;
	m_PlayTime = 0.f;
	m_LineIndex = 0;
	m_iIndex = 0;
	for (auto& Text : m_EffectText)
		Text = TEXT("");

	//퀘스트 카운트 인덱스 초기화
	for (auto& iter : m_EventUI)
	{
		dynamic_cast<CText*>(iter)->Set_Text(TEXT(""));
	}
}

void CUITalk::Talk_Effect(const _float& fTimeDelta)
{
	 if (L'\0' == m_TalkData[m_iIndex])
	{
		m_TalkFin = true;
		return;
	}
	else	if (L'\\' == m_TalkData[m_iIndex])
	 {
		 m_LineIndex++;
		 m_iIndex ++;
		 return;
	 }


	m_EffectText[m_LineIndex] += m_TalkData[m_iIndex];
	m_iIndex++;

	for (size_t i = 0; i < m_LineIndex+1; i++)
	{
		dynamic_cast<CText*>(m_EventUI[i])->Set_Text(m_EffectText[i]);
	}
}

CUITalk* CUITalk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUITalk* pInstance = new CUITalk();
	if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
	{
		MSG_BOX("Make Failed :CUITalk");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUITalk::Free()
{
	__super::Free();

	Safe_Release(m_pScriptManager);
}
