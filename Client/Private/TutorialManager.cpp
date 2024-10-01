#include "TutorialManager.h"

#include "UIManager.h"
#include "UITutorial.h"

#include "Tutorial.h"

CTutorialManager::CTutorialManager()
	: m_pUIManager{ CUIManager::GetInstance() }
{
	Safe_AddRef(m_pUIManager);
}

HRESULT CTutorialManager::Initialize()
{
	if (FAILED(Ready_Tutorial()))
		return E_FAIL;

	return S_OK;
}

_bool CTutorialManager::Tick()
{
	if (m_isFinished)
		return true;

	if (m_iCurrentIndex >= m_Tutorials.size())
	{
		m_isFinished = true;
		m_pUIManager->Close_Scene();
		return true;
	}

	if (m_Tutorials[m_iCurrentIndex]->Execute())
	{
		//튜토리얼보다 인덱스가 더 크다면 튜토리얼 끝.

		m_iCurrentIndex++;
	}

	return false;
}

void CTutorialManager::Start_Tutorial()
{
	m_pUIManager->Close_Scene();
	m_pUIManager->Open_Scene(TEXT("Tutorial"));
	m_pUIManager->Change_TutorialUI(0);
}

HRESULT CTutorialManager::Ready_Tutorial()
{
	CTutorial::TUTORIAL_DESC Desc{};
	Desc.iConstance = 12;
	Desc.TutorialUI = { CUITutorial::TOTU_RUSHCOMBO };
	Desc.iTutorialCheckUI = CUITutorial::TOTU_RUSHCHECKBOX;
	Desc.strPlayerSkillName = "RushCombo";
	Desc.isFinished = false;

	CTutorial* pTutorial = CTutorial::Create(&Desc);
	if (nullptr == pTutorial)
		return E_FAIL;

	m_Tutorials.emplace_back(pTutorial);

	Desc.iConstance = 3;
	Desc.TutorialUI = { CUITutorial::TOTU_FINISHBLOW };
	Desc.iTutorialCheckUI = CUITutorial::TOTU_FINISHBLOW_CHECKBOX;
	Desc.strPlayerSkillName = "FinishBlow";
	Desc.isFinished = false;

	pTutorial = CTutorial::Create(&Desc);
	if (nullptr == pTutorial)
		return E_FAIL;

	m_Tutorials.emplace_back(pTutorial);

	Desc.iConstance = 3;
	Desc.TutorialUI = { CUITutorial::TOTU_HP, CUITutorial::TOTU_GRAB };
	Desc.iTutorialCheckUI = CUITutorial::TOTU_GRAB_CHECKBOX;
	Desc.strPlayerSkillName = "Grap";
	Desc.isFinished = false;

	pTutorial = CTutorial::Create(&Desc);
	if (nullptr == pTutorial)
		return E_FAIL;

	m_Tutorials.emplace_back(pTutorial);

	Desc.iConstance = 10;
	Desc.TutorialUI = { CUITutorial::TOTU_CLEAR };
	Desc.iTutorialCheckUI = CUITutorial::TOTU_CLEAR_CEHCKBOX;
	Desc.strPlayerSkillName = "Clear";
	Desc.isFinished = true;

	pTutorial = CTutorial::Create(&Desc);
	if (nullptr == pTutorial)
		return E_FAIL;

	m_Tutorials.emplace_back(pTutorial);

	return S_OK;
}

CTutorialManager* CTutorialManager::Create()
{
	CTutorialManager* pInstance = new CTutorialManager();

	if (FAILED(pInstance->Initialize()))
		Safe_Release(pInstance);

	return pInstance;
}

void CTutorialManager::Free()
{
	Safe_Release(m_pUIManager);

	for (auto& pTutorial : m_Tutorials)
		Safe_Release(pTutorial);
	m_Tutorials.clear();
}
