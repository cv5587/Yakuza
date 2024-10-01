#include "UIManager.h"
#include "GameInstance.h"
#include "UI_Object.h"
#include "UIScene.h"

#include"UINowLoading.h"
#include "UILoading.h"
#include "UIMenu.h"
#include "UILife.h"
#include "UIMoney.h"
#include "UIInven.h"
#include "UILogo.h"
#include "UIMainMenu.h"
#include "UITutorial.h"
#include "UITalk.h"
#include "UISkillMenu.h"
#include "UISkillHolligan.h"
#include"UISkillRush.h"
#include"UISkillDestroyer.h"
#include "UICarchase.h"
#include "UITitle.h"
#include "UIFightScore.h"
#include "UIStoreImage.h"
#include "UIQTE.h"
#include "UIFade.h"
#include "UIDamageMoney.h"

#include "UIKaraoke_Select.h"
#include "UIKaraoke_Play.h"
#include "UIKaraoke_Score.h"

#include "InventoryManager.h"
#include "Player.h"

IMPLEMENT_SINGLETON(CUIManager)

CUIManager::CUIManager()
	:m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

_bool CUIManager::isOpen(wstring strName)
{
	CUIScene* pUIScene = Find_Scene(strName);

	return pUIScene->Get_isOpen();
}

_bool CUIManager::isShowTutorialUI(_uint iUIType)
{
	CUITutorial* pScene = dynamic_cast<CUITutorial*>(Find_Scene(TEXT("Tutorial")));

	return pScene->isShow(iUIType);
}

_bool CUIManager::isCloseTutorialUIAnim()
{
	CUITutorial* pScene = dynamic_cast<CUITutorial*>(Find_Scene(TEXT("Tutorial")));

	return pScene->isCloseCurrentUIAnim();
}

_bool CUIManager::isTutorialStartEnd()
{
	CUITutorial* pScene = dynamic_cast<CUITutorial*>(Find_Scene(TEXT("Tutorial")));

	return pScene->isStartEnd();
}

void CUIManager::Set_TutorialState(_uint iType)
{
	CUITutorial* pScene = dynamic_cast<CUITutorial*>(Find_Scene(TEXT("Tutorial")));

	return pScene->Set_State(iType);
}

void CUIManager::Set_TutorialText(wstring strText)
{
	CUITutorial* pScene = dynamic_cast<CUITutorial*>(Find_Scene(TEXT("Tutorial")));

	pScene->Set_Text(strText);
}

HRESULT CUIManager::Add_Target(_uint iIndex, class CCarChase_Monster* pMonster, _bool isBullet)
{
	CUICarchase* pScene = dynamic_cast<CUICarchase*>(Find_Scene(TEXT("Carchase")));

	return pScene->Add_TargetingRound(iIndex, pMonster, isBullet);
}

HRESULT CUIManager::Update_TargetMatrix(_uint iIndex, _matrix TargetMatrix, _float iHP)
{
	CUICarchase* pScene = dynamic_cast<CUICarchase*>(Find_Scene(TEXT("Carchase")));

	return pScene->Update_TargetMatrix(iIndex, TargetMatrix, iHP);
}

HRESULT CUIManager::Remove_Target(_uint iIndex)
{
	CUICarchase* pScene = dynamic_cast<CUICarchase*>(Find_Scene(TEXT("Carchase")));

	return pScene->Remove_Target(iIndex);
}

CCarChase_Monster* CUIManager::Get_Target()
{
	CUICarchase* pScene = dynamic_cast<CUICarchase*>(Find_Scene(TEXT("Carchase")));

	return pScene->Get_Target();
}

void CUIManager::Set_TitleStart(_bool isStart)
{
	CUITitle* pScene = dynamic_cast<CUITitle*>(Find_Scene(TEXT("Title")));

	pScene->Set_Start(isStart);
}

_bool CUIManager::isTitleStart()
{
	CUITitle* pScene = dynamic_cast<CUITitle*>(Find_Scene(TEXT("Title")));

	return pScene->isStart();
}

void CUIManager::Start_Title(_int iIndex)
{
	CUITitle* pScene = dynamic_cast<CUITitle*>(Find_Scene(TEXT("Title")));

	pScene->Start_Title(iIndex);
}

_bool CUIManager::isTitleEnd()
{
	CUITitle* pScene = dynamic_cast<CUITitle*>(Find_Scene(TEXT("Title")));

	return pScene->isEnd();
}

_bool CUIManager::isBattleStart()
{
	CUITitle* pScene = dynamic_cast<CUITitle*>(Find_Scene(TEXT("Title")));

	return pScene->isBattleStart();
}

_bool CUIManager::isFindFinished()
{
	CUIFade* pScene = dynamic_cast<CUIFade*>(Find_Scene(TEXT("Fade")));

	return pScene->isFinished();
}

HRESULT CUIManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pInventory = CInventoryManager::Create();//¿øº»
	
	CUIScene::SCENE_DESC Desc{};
	Desc.isLoading = true;
	Desc.strSceneName = TEXT("Loading");
	CUIScene* pScene = CUILoading::Create(m_pDevice ,m_pContext ,&Desc);
	m_AllScene.emplace(make_pair(TEXT("Loading"), pScene));

	Desc.strSceneName = TEXT("Logo");
	 pScene = CUILogo::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("Logo"), pScene));

	Desc.strSceneName = TEXT("NowLoading");
	pScene = CUINowLoading::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("NowLoading"), pScene));

	Desc.strSceneName = TEXT("MainMenu");
	pScene = CUIMainMenu::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("MainMenu"), pScene));



	Desc.isLoading = false;
	Desc.strSceneName = TEXT("Menu");
	pScene = CUIMenu::Create(m_pDevice, m_pContext,&Desc);
	m_AllScene.emplace( make_pair(TEXT("Menu"), pScene) );

	Desc.strSceneName = TEXT("Life");
	pScene = CUILife::Create(m_pDevice, m_pContext,&Desc);
	m_AllScene.emplace(make_pair(TEXT("Life"), pScene));
	Safe_AddRef(pScene);
	m_AlwaysUI.push_back(pScene);

	Desc.strSceneName = TEXT("Money");
	pScene = CUIMoney::Create(m_pDevice, m_pContext,&Desc);
	m_AllScene.emplace(make_pair(TEXT("Money"), pScene));
	Safe_AddRef(pScene);
	m_AlwaysUI.push_back(pScene);

	Desc.strSceneName = TEXT("DamageMoney");
	pScene = CUIDamageMoney::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("DamageMoney"), pScene));
	Safe_AddRef(pScene);
	m_AlwaysUI.push_back(pScene);

	CUIInven::IVENSCENE_DESC InvenDesc{};

	InvenDesc.pInventory = m_pInventory;

	InvenDesc.isLoading = false;
	InvenDesc.strSceneName = TEXT("Inven");
	pScene = CUIInven::Create(m_pDevice, m_pContext ,&InvenDesc);
	m_AllScene.emplace(make_pair(TEXT("Inven"), pScene));


	Desc.strSceneName = TEXT("Tutorial");
	pScene = CUITutorial::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("Tutorial"), pScene));

	Desc.strSceneName = TEXT("Talk");
	pScene = CUITalk::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("Talk"), pScene));

	Desc.strSceneName = TEXT("SkillMenu");
	pScene = CUISkillMenu::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("SkillMenu"), pScene));
	
	Desc.strSceneName = TEXT("SkillHolligan");
	pScene = CUISkillHolligan::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("SkillHolligan"), pScene));

	Desc.strSceneName = TEXT("SkillRush");
	pScene = CUISkillRush::Create(m_pDevice, m_pContext,&Desc);
	m_AllScene.emplace(make_pair(TEXT("SkillRush"), pScene));

	Desc.strSceneName = TEXT("SkillDestroyer");
	pScene = CUISkillDestroyer::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("SkillDestroyer"), pScene));

	Desc.strSceneName = TEXT("Carchase");
	pScene = CUICarchase::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("Carchase"), pScene));

	Desc.isLoading = true;
	Desc.strSceneName = TEXT("Karaoke_Select");
	pScene = CUIKaraoke_Select::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("Karaoke_Select"), pScene));

	Desc.isLoading = true;
	Desc.strSceneName = TEXT("Karaoke_Play");
	pScene = CUIKaraoke_Play::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("Karaoke_Play"), pScene));

	Desc.isLoading = true;
	Desc.strSceneName = TEXT("Karaoke_Score");
	pScene = CUIKaraoke_Score::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("Karaoke_Score"), pScene));

	Desc.isLoading = true;
	Desc.strSceneName = TEXT("Title");
	pScene = CUITitle::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("Title"), pScene));

	Desc.isLoading = true;
	Desc.strSceneName = TEXT("FightScore");
	pScene = CUIFightScore::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("FightScore"), pScene));

	Desc.isLoading = true;
	Desc.strSceneName = TEXT("StoreImage");
	pScene = CUIStoreImage::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("StoreImage"), pScene));

	Desc.isLoading = true;
	Desc.strSceneName = TEXT("QTE");
	pScene = CUIQTE::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("QTE"), pScene));

	Desc.isLoading = true;
	Desc.strSceneName = TEXT("Fade");
	pScene = CUIFade::Create(m_pDevice, m_pContext, &Desc);
	m_AllScene.emplace(make_pair(TEXT("Fade"), pScene));
	


	return S_OK;
}

HRESULT CUIManager::Add_Data(const wstring strSceneName, wstring strProtoName)
{
	CUIScene* pScene = Find_Scene(strSceneName);

	if (nullptr == pScene)	
		return E_FAIL;

	if (FAILED(pScene->Add_UIData(dynamic_cast<CUI_Object*>(m_pGameInstance->Clone_Object(strProtoName, nullptr)), strProtoName)))
		return E_FAIL;
	
	return S_OK;
}

void CUIManager::Open_Scene(const wstring strSceneName)
{
	CUIScene* pUIScene = Find_Scene(strSceneName);	

	if (pUIScene->Get_isLoading())
	{
		if(!m_PlayScene.empty())
		{
			m_PlayScene.pop_back();
		}
	}

	pUIScene->Set_Open();
	m_PlayScene.push_back(pUIScene);
	m_PlayScene.back()->Show_Scene();
	m_isClose = false;
}

void CUIManager::Close_Scene(const wstring strSceneName)
{
	if (!m_PlayScene.empty())
	{
		if (m_PlayScene.back()->Get_isLoading())
		{
			m_PlayScene.pop_back();
			return;
		}
		m_PlayScene.back()->Close_Scene();
		m_isClose = true;
	}
}

void CUIManager::Click()
{
	if (!m_PlayScene.empty())
		m_PlayScene.back()->Click_InterSect();
}

HRESULT CUIManager::Tick(const _float& fTimeDelta)
{
	if(!m_PlayScene.empty())
	{

		m_PlayScene.back()->Tick(fTimeDelta);

	}

	if (!m_PlayScene.empty() )
	{
		if(!m_PlayScene.back()->Get_isLoading()	&&	m_PlayScene.back()->Get_SceneName()!= TEXT("Carchase"))
		{
			for (auto& pUIScene : m_AlwaysUI)
			{
				pUIScene->Tick(fTimeDelta);
			}
		}
	}
	else
	{
		for (auto& pUIScene : m_AlwaysUI)
		{
			pUIScene->Tick(fTimeDelta);
		}
	}



	return S_OK;
}

HRESULT CUIManager::Late_Tick(const _float& fTimeDelta)
{
	if (!m_PlayScene.empty())
	{

		if (m_PlayScene.back()->Get_isClose() && m_PlayScene.back()->Get_isAnimFinish())
		{
			m_PlayScene.pop_back();
		}

		if (!m_PlayScene.empty())
		{

			m_PlayScene.back()->Late_Tick(fTimeDelta);
		}
	}


#ifdef _DEBUG
	if (m_isRender && m_isAlways)
	{
		if (!m_PlayScene.empty())
		{
			if (!m_PlayScene.back()->Get_isLoading() && m_PlayScene.back()->Get_SceneName() != TEXT("Carchase"))
			{
				for (auto& pUIScene : m_AlwaysUI)
				{
					pUIScene->Late_Tick(fTimeDelta);
				}
			}
		}
		else
		{
			for (auto& pUIScene : m_AlwaysUI)
			{
				pUIScene->Late_Tick(fTimeDelta);
			}
		}
	}
#else
	if (m_isAlways)
	{
		if (!m_PlayScene.empty())
		{
			if (!m_PlayScene.back()->Get_isLoading() && m_PlayScene.back()->Get_SceneName() != TEXT("Carchase"))
			{
				for (auto& pUIScene : m_AlwaysUI)
				{
					pUIScene->Late_Tick(fTimeDelta);
				}
			}
		}
		else
		{
			for (auto& pUIScene : m_AlwaysUI)
			{
				pUIScene->Late_Tick(fTimeDelta);
			}
		}
}
#endif // _DEBUG

	return S_OK;
}



void CUIManager::Fade_In()
{
	CUIScene* pUIScene = Find_Scene(TEXT("Fade"));

	m_PlayScene.push_back(pUIScene);
	dynamic_cast<CUIFade*>(m_PlayScene.back())->Fade_In();
}

void CUIManager::Fade_Out()
{
	CUIScene* pUIScene = Find_Scene(TEXT("Fade"));

	m_PlayScene.push_back(pUIScene);
	dynamic_cast<CUIFade*>(m_PlayScene.back())->Fade_Out();
}

_bool CUIManager::Fade_Finished()
{
	CUIScene* pUIScene = Find_Scene(TEXT("Fade"));

	return dynamic_cast<CUIFade*>(m_PlayScene.back())->isFinished();
}

void CUIManager::PressKey()
{
	CUIScene* pUIScene = Find_Scene(TEXT("QTE"));

	dynamic_cast<CUIQTE*>(pUIScene)->Press();
}

void CUIManager::Set_QTEKey(wstring strKey)
{
	CUIScene* pUIScene = Find_Scene(TEXT("QTE"));

	dynamic_cast<CUIQTE*>(pUIScene)->Set_Key(strKey);
}

_bool CUIManager::QTE_Finished()
{
	CUIScene* pUIScene = Find_Scene(TEXT("QTE"));

	return dynamic_cast<CUIQTE*>(pUIScene)->Get_isAnimFinish();
}

void CUIManager::Add_MoneyEffect(_fmatrix world, _uint iMoney, _uint iTime)
{
	CUIScene* pUIScene = Find_Scene(TEXT("DamageMoney"));

	dynamic_cast<CUIDamageMoney*>(pUIScene)->Add_MoneyEffect(world, iMoney, iTime);
}

CUIScene* CUIManager::Find_Scene(wstring strSceneName)
{
	auto PairScene =m_AllScene.find(strSceneName);
	
	if (m_AllScene.end() == PairScene)
		return nullptr;
	
	return PairScene->second;
}

void CUIManager::Free()
{
	for (auto iter : m_AlwaysUI)
	{
		Safe_Release(iter);
	}

	m_AlwaysUI.clear();

	for (auto iter : m_AllScene)
	{
		Safe_Release(iter.second);
	}
	m_AllScene.clear();

	Safe_Release(m_pInventory);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}

void CUIManager::Start_Talk(_uint iScriptIndex)
{
	CUITalk* pScene = dynamic_cast<CUITalk*>(Find_Scene(TEXT("Talk")));

	pScene->Start_Talk(iScriptIndex);
}

_int CUIManager::Get_CurrentPage()
{
	CUITalk* pScene = dynamic_cast<CUITalk*>(Find_Scene(TEXT("Talk")));

	return pScene->Get_CurrentScriptPage();
}

_bool CUIManager::isTalkFinished()
{
	CUITalk* pScene = dynamic_cast<CUITalk*>(Find_Scene(TEXT("Talk")));

	return pScene->isFinished();
}

void CUIManager::Change_TutorialUI(_uint iUIType)
{
	CUITutorial* pScene = dynamic_cast<CUITutorial*>(Find_Scene(TEXT("Tutorial")));

	pScene->Set_State(iUIType);
}

_bool CUIManager::Check_Scene(wstring SceneName)
{
	if (SceneName == m_PlayScene.back()->Get_SceneName())
		return true;
	else
		return false;
}

void CUIManager::Close_Image()
{
	CUIStoreImage* pScene = dynamic_cast<CUIStoreImage*>(Find_Scene(TEXT("StoreImage")));

	pScene->Close_Image();
}

void CUIManager::Set_Score(_uint iScore)
{
	if (TEXT("Karaoke_Score") == m_PlayScene.back()->Get_SceneName())
	{
		dynamic_cast<CUIKaraoke_Score*>(m_PlayScene.back())->Set_Score(iScore);
	}
}

void CUIManager::Change_Style(_uint iStyle)
{
	CUILife* pScene = dynamic_cast<CUILife*>(Find_Scene(TEXT("Life")));

	pScene->Change_Style(iStyle);
}

_bool* CUIManager::Get_Skill_Holligan()
{
	CUISkillHolligan* pScene = dynamic_cast<CUISkillHolligan*>(Find_Scene(TEXT("SkillHolligan")));
	return pScene->Get_Skill();
}

_bool* CUIManager::Get_Skill_Rush()
{
	CUISkillRush* pScene = dynamic_cast<CUISkillRush*>(Find_Scene(TEXT("SkillRush")));
	return pScene->Get_Skill();
}

_bool* CUIManager::Get_Skill_Destoryer()
{
	CUISkillDestroyer* pScene = dynamic_cast<CUISkillDestroyer*>(Find_Scene(TEXT("SkillDestroyer")));
	return pScene->Get_Skill();
}
