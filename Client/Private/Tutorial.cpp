#include "Tutorial.h"

#include "GameInstance.h"
#include "UIManager.h"

#include "Player.h"
#include "Monster.h"
#include "MonsterGroup.h"
#include "UITutorial.h"

CTutorial::CTutorial()
	: m_pGameInstance{ CGameInstance::GetInstance() },
	m_pUIManager { CUIManager::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pUIManager);
}

HRESULT CTutorial::Intialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	TUTORIAL_DESC* pDesc = static_cast<TUTORIAL_DESC*>(pArg);
	m_iCoustance = pDesc->iConstance;
	m_strPlayerSkillName = pDesc->strPlayerSkillName;
	m_TutorialUIIndex = pDesc->TutorialUI;
	m_iTutorialCheckUIIndex = pDesc->iTutorialCheckUI;
	m_isFinished = pDesc->isFinished;

	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
	if (nullptr == m_pPlayer)
		return E_FAIL;

	m_iState = STATE_START;

	return S_OK;
}

/*
* 몬스터가 사용하는 스킬
* rush  combo
* finish blow
* grap
* clear
*/
_bool CTutorial::Start()
{
	if (m_pUIManager->isShowTutorialUI(m_TutorialUIIndex[m_iTutorialIndex]))
	{
		if (m_pGameInstance->GetKeyState(DIK_E) == TAP)
		{
			m_pGameInstance->StopSound(SOUND_UI);
			m_pGameInstance->PlaySound_W(TEXT("4681 [8].wav"), SOUND_UI, 0.5f);

			m_pUIManager->Change_TutorialUI(CUITutorial::TOTU_START);
			return true;
		}	
	}
	else
	{
		m_pUIManager->Change_TutorialUI(m_TutorialUIIndex[m_iTutorialIndex]);
	}
		
	//CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
	//pPlayer->Set_PlayerStop(true);

	return false;
}

_bool CTutorial::Running()
{
	if (m_pUIManager->isShowTutorialUI(CUITutorial::TOTU_START) || m_pUIManager->isShowTutorialUI(CUITutorial::TOTU_GRAB))
	{
		if (!m_pUIManager->isCloseTutorialUIAnim())
			return false;

		if (m_pGameInstance->GetKeyState(DIK_E) == TAP || 
			(m_pUIManager->isShowTutorialUI(CUITutorial::TOTU_START) && m_pUIManager->isTutorialStartEnd()))
		{
			if (m_TutorialUIIndex.size() - 1 > m_iTutorialIndex)
			{
				m_iTutorialIndex++;
				m_pUIManager->Change_TutorialUI(m_TutorialUIIndex[m_iTutorialIndex]);
			}
			else
			{
				if (!m_pUIManager->isShowTutorialUI(CUITutorial::TOTU_START))
				{
					m_pGameInstance->StopSound(SOUND_UI);
					m_pGameInstance->PlaySound_W(TEXT("4681 [8].wav"), SOUND_UI, 0.8f);
				}

				m_pUIManager->Change_TutorialUI(m_iTutorialCheckUIIndex);
			}
		}
		return false;
	}

	//CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
	//pPlayer->Set_PlayerStop(false);

	//체크박스가 보이고 있다면
	//플레이어에 관한 스킬 카운트 관리하기
	if (m_pUIManager->isShowTutorialUI(m_iTutorialCheckUIIndex))
	{
		//Rush Combo일 경우
		if (string_view(m_strPlayerSkillName) == string_view("RushCombo"))
		{
			if (string_view(m_pPlayer->Get_CurrentAnimationName()) == string_view("p_krs_cmb_02") ||
				string_view(m_pPlayer->Get_CurrentAnimationName()) == string_view("p_krs_cmb_03") ||
				string_view(m_pPlayer->Get_CurrentAnimationName()) == string_view("p_krs_cmb_04"))
			{
				
				vector<CMonster*> Monsters = dynamic_cast<CMonsterGroup*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_MonsterGroup"), 0))->Get_Monsters();
				//몬스터들 돌려가면서 체크
				for (auto& pMonster : Monsters)
					if (dynamic_cast<CMonster*>(pMonster)->isTutorialAttack())
					{
						m_pGameInstance->StopSound(SOUND_EFFECT);
						m_pGameInstance->PlaySound_W(TEXT("4681 [38].wav"), SOUND_EFFECT, 0.5f);
						m_iCount++;
					}
			}
		}
		//FinishBlow일 경우
		else if (string_view(m_strPlayerSkillName) == string_view("FinishBlow"))
		{
			if (string_view(m_pPlayer->Get_CurrentAnimationName()) == string_view("p_krs_cmb_01_fin")||
				string_view(m_pPlayer->Get_CurrentAnimationName()) == string_view("p_krs_cmb_02_fin") ||
				string_view(m_pPlayer->Get_CurrentAnimationName()) == string_view("p_krs_cmb_03_fin") || 
				string_view(m_pPlayer->Get_CurrentAnimationName()) == string_view("p_krs_cmb_04_fin"))
			{
				vector<CMonster*> Monsters = dynamic_cast<CMonsterGroup*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_MonsterGroup"), 0))->Get_Monsters();

				//몬스터들 돌려가면서 체크
				for (auto& pMonster : Monsters)
					if (dynamic_cast<CMonster*>(pMonster)->isTutorialAttack()) m_iCount++;
			}
		}
		//Grap일 경우
		else if (string_view(m_strPlayerSkillName) == string_view("Grap"))
		{
			if (string_view(m_pPlayer->Get_CurrentAnimationName()) == string_view("p_kru_sync_lapel_nage")
				|| string_view(m_pPlayer->Get_CurrentAnimationName()) == string_view("h20021"))
			{
				vector<CMonster*> Monsters = dynamic_cast<CMonsterGroup*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_MonsterGroup"), 0))->Get_Monsters();
				//몬스터들 돌려가면서 체크
				for (auto& pMonster : Monsters)
					if (dynamic_cast<CMonster*>(pMonster)->isTutorialAttack()) m_iCount++;
			}
		}
		//Clear일 경우
		else if (string_view(m_strPlayerSkillName) == string_view("Clear"))
		{
			_bool isFinished = true;
			vector<CMonster*> Monsters = dynamic_cast<CMonsterGroup*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_MonsterGroup"), 0))->Get_Monsters();
			//몬스터들 돌려가면서 체크
			for (auto& pMonster : Monsters)
			{
				if (!dynamic_cast<CMonster*>(pMonster)->isObjectDead()) isFinished = false;
			}

			return isFinished;
		}

		m_pUIManager->Set_TutorialText(to_wstring(m_iCoustance - m_iCount));
	}

	if (m_iCoustance <= m_iCount)
		return true;

	return false;
}

_bool CTutorial::End()
{
	if (!m_pUIManager->isShowTutorialUI(CUITutorial::TOTU_OK))
		m_pUIManager->Change_TutorialUI(CUITutorial::TOTU_OK);
	else
	{
		// OK 일 때, 애니메이션이 끝났는지 확인해야 함
		if (m_pUIManager->isTutorialStartEnd())
			return true;
	}

	return false;
}

_bool CTutorial::Execute()
{
	if (m_iState == STATE_START)
		m_iState = Start() ? STATE_RUNNGIN : STATE_START;
	else if (m_iState == STATE_RUNNGIN)
		m_iState = Running() ? STATE_END : STATE_RUNNGIN;
	else if (m_iState == STATE_END)
		m_iState = End() ? TUTORIAL_STATE_END : STATE_END;

	return m_iState == TUTORIAL_STATE_END;
}

CTutorial* CTutorial::Create(void* pArg)
{
	CTutorial* pInstance = new CTutorial();

	if (FAILED(pInstance->Intialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTutorial::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pUIManager);
}
