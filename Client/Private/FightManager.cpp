#include "FightManager.h"

#include "GameInstance.h"

#include "TutorialManager.h"
#include "UIManager.h"

#include "Monster.h"
#include "MonsterGroup.h"

#include "UIFightScore.h"
#include "PlayerCamera.h"
#include "Player.h"

IMPLEMENT_SINGLETON(CFightManager)

CFightManager::CFightManager()
	: m_pGameInstance{CGameInstance::GetInstance()}
	,m_pUIManager{CUIManager::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pUIManager);
}

void CFightManager::Set_FightStage(_bool isFightStage, CMonsterGroup* pMonsterGroup)
{
	m_isFightStage = isFightStage;
	m_isStreetFight = false;
	m_isInverseEnd = false;
	m_isMoney = false;

	m_pUIManager->Set_AlwayUI(true);

	if (m_isFightStage)
	{
		_int iTitleIndex = -1;

		switch (m_pGameInstance->Get_CurrentLevel())
		{
		case LEVEL_DOGIMAZO:		//도지마조 조직원
			iTitleIndex = 16;
			break;
		case LEVEL_DOGIMAZO_BOSS:	//쿠제
			iTitleIndex = 5;
			break;
		case LEVEL_OFFICE_1F:		//동흥 크레디트
			iTitleIndex = 17;
			break;
		case LEVEL_OFFICE_BOSS:		//삥쟁
			iTitleIndex = 4;
			break;
		case LEVEL_CARCHASE:		//돔황챠
			iTitleIndex = 3;
			break;
		case LEVEL_TEST:			//테스트용으로 그냥 암거나넣음
			iTitleIndex = 15;
			break;
		case LEVEL_TUTORIAL:			//야큐자
		{
			m_pGameInstance->StopSound(SOUND_BGM);
			m_pGameInstance->PlayBGM(TEXT("Tutorial_Fight.mp3"), 0.6f);
			m_isStreetFight = true;
			iTitleIndex = 13;
			break;
		}
		case LEVEL_STREET:			//삥쟁
		{
			// 거리에서 만난상황이면 야쿠자
			if (nullptr != pMonsterGroup)
			{
				iTitleIndex = 12;
			}
			break;
		}
		default:
		{
			// 그 외 레벨에서 불렸으면 던전 내에서의 다른 레벨로 판단하여 아무런 처리를 하지 않음
			break;
		}
		}

		// 타이틀 인덱스가 0보다 큰 경우에만 타이틀을 띄우는 레벨임
		if (0 < iTitleIndex)
		{
			//타이틀 떠야 함.
			m_pGameInstance->StopSound(SOUND_EFFECT);
			m_pGameInstance->PlaySound_W(TEXT("47cd [5].wav"), SOUND_EFFECT, 0.5f);

			m_pUIManager->Open_Scene(TEXT("Title"));

			m_pUIManager->Start_Title(iTitleIndex);

			if (pMonsterGroup != nullptr)
			{
				m_pCurrentMonsterGroup = pMonsterGroup;
				Safe_AddRef(pMonsterGroup);
			}
			else
			{
				if (m_pGameInstance->Get_CurrentLevel() == LEVEL_CARCHASE)
					return;

				vector<CGameObject*> Monsters = m_pGameInstance->Get_GameObjects(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Monster"));

				for (auto& pMonster : Monsters)
				{
					CMonster* pTarget = dynamic_cast<CMonster*>(pMonster);
					pTarget->Set_Start(true);
				}
			}
		}
	}
	else
	{
		Safe_Release(pMonsterGroup);
	}
}

HRESULT CFightManager::Initialize()
{
	if (m_pGameInstance->Get_CurrentLevel() == LEVEL_TUTORIAL)
	{
		m_pTutorialManager = CTutorialManager::Create();
		if (nullptr == m_pTutorialManager)
			return E_FAIL;
	}
	m_pGameInstance->Set_InvertColorDuration(1.f);

	return S_OK;
}

_bool CFightManager::Tick(const _float& fTimeDelta)
{
	//전투중이 아니면 바로 반환
	if (!m_isFightStage) return false;

	//길거리 일 때, 몬스터가 다 죽으면 전투가 끝난다.
	if (m_isStreetFight)
	{
		//vector<CGameObject*> Monsters = m_pGameInstance->Get_GameObjects(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_MonsterGroup"));
		if (m_pGameInstance->Get_CurrentLevel() == LEVEL_TUTORIAL)
		{
			if (m_isTutorialEnd)
			{
				m_fFinishTime += m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));

				_float fRatio = (m_fFinishDuration - m_fFinishTime * 1.5f) / m_fFinishDuration;

				m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), fRatio < 0 ? 0.f : fRatio);
				m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), fRatio < 0 ? 0.f : fRatio);

				if (!m_isMoney)
				{
					m_pGameInstance->PlaySound_W(TEXT("4681 [31].wav"), SOUND_UI, 0.5f);

					m_isMoney = true;
					m_pUIManager->Open_Scene(TEXT("FightScore"));
					dynamic_cast<CUIFightScore*>(m_pUIManager->Find_Scene(TEXT("FightScore")))->StartAddMoney(500000);
				}

				CPlayerCamera* pCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
				pCamera->Set_CustomRatio(fRatio);
				pCamera->Set_TargetFoV(XMConvertToRadians(30.f));
				pCamera->Start_Zoom();

				if (m_fFinishDuration < m_fFinishTime)
				{
					Safe_Release(m_pCurrentMonsterGroup);
					m_isFightStage = false;
					m_pGameInstance->Set_InvertColor(false);
					//플레이어도 다시 돌아온다.
					CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
					pPlayer->Style_Change(CPlayer::ADVENTURE);
					m_pGameInstance->StopSound(SOUND_BGM);
					m_pUIManager->Set_AlwayUI(false);

					m_pGameInstance->PlayBGM(TEXT("Street_BGM.mp3"), 0.3f);

					m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
					m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), 1.f);

					m_pUIManager->Close_Scene(TEXT("FightScore"));

					m_pGameInstance->StopSound(SOUND_UI);

					return true;
				}
				m_pGameInstance->Set_InvertColor(true);
			}

			if (m_pUIManager->isTitleEnd())
			{
				if (!m_pUIManager->isOpen(TEXT("Tutorial")) && !m_isTutorialStart)
				{
					m_isTutorialStart = true;
					m_pTutorialManager->Start_Tutorial();
					return false;
				}

				//아래에 있는 코드 복붙. 전투 끝.
				if (m_pTutorialManager->Tick())
				{
					m_isTutorialEnd = true;
				}
			}

			return false;
		}

		vector<CMonster*> Monsters = m_pCurrentMonsterGroup->Get_Monsters();
		_bool isFinished = true;
		for (auto& pMonster : Monsters)
		{
			if (!pMonster->isObjectDead())
			{
				isFinished = false;
				break;
			}
		}
		
		//전투가 끝났을 때 속도가 느려지며 invertcolor 셰이더 호출한다.
		if (isFinished == true)
		{
			m_fFinishTime += fTimeDelta;

			if (m_fFinishDuration < m_fFinishTime)
			{
				Safe_Release(m_pCurrentMonsterGroup);
				m_isFightStage = false;
				m_pGameInstance->Set_InvertColor(false);
				return true;
			}
			m_pGameInstance->Set_InvertColor(true);
		}
	}
	else
	{
		if (Check_Stage_Clear())
		{
			_uint iLevelIndex = m_pGameInstance->Get_CurrentLevel();

			// 레벨단위가 아니라 던전 단위로 시작과 끝을 정의해야한다.(사채업자 or 도지마조 등)
			if (iLevelIndex == LEVEL_DOGIMAZO_BOSS || iLevelIndex == LEVEL_OFFICE_BOSS || iLevelIndex == LEVEL_TEST)
			{
				m_fFinishTime += m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));

				_float fRatio = (m_fFinishDuration - m_fFinishTime * 1.5f) / m_fFinishDuration;

				if (!m_isMoney)
				{
					m_pGameInstance->PlaySound_W(TEXT("4681 [31].wav"), SOUND_UI, 0.5f);

					m_isMoney = true;
					m_pUIManager->Open_Scene(TEXT("FightScore"));
					dynamic_cast<CUIFightScore*>(m_pUIManager->Find_Scene(TEXT("FightScore")))->AddMoney(CPlayer::PlayerInfo.iMoney);
				}

				m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), fRatio < 0 ? 0.f : fRatio);
				m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), fRatio < 0 ? 0.f : fRatio);

				CPlayerCamera* pCamera = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
				pCamera->Set_CustomRatio(fRatio);
				pCamera->Set_TargetFoV(XMConvertToRadians(30.f));
				pCamera->Start_Zoom();

				if (m_fFinishDuration < m_fFinishTime)
				{
					m_fFinishTime = 0.f;
					m_isInverseEnd = true;
					m_isFightStage = false;
					m_pGameInstance->Set_InvertColor(false);

					m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
					m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), 1.f);

					m_pUIManager->Close_Scene(TEXT("FightScore"));

					m_pGameInstance->StopSound(SOUND_UI);
						
					return true;
				}
				m_pGameInstance->Set_InvertColor(true);
			}
			// 던전 속 레벨이 마무리됐는지를 체크한다.
			else 
			{
				m_fFinishTime += m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));

				m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 0.f);
				m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), 0.f);

				if (0.5f < m_fFinishTime)
				{

					m_pUIManager->Set_AlwayUI(false);

					m_fFinishTime = 0.f;
					m_isFightStage = false;
#ifdef _DEBUG
					cout << "흑백 쉐이더 Off" << endl;
#endif

					m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
					m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), 1.f);

					return true;
				}
#ifdef _DEBUG
				cout << "흑백 쉐이더 On" << endl;
#endif
			}

		}

	}
}

_bool CFightManager::Check_Stage_Clear()
{
	auto pMonsters = m_pGameInstance->Get_GameObjects(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Monster"));
	auto pYonedas = m_pGameInstance->Get_GameObjects(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Yoneda"));

	_uint iEnemyCount = { 0 };

	for (auto& pMonster : pMonsters)
	{
		if (!pMonster->isObjectDead())
			iEnemyCount++;
	}

	for (auto& pYoneda : pYonedas)
	{
		if (!pYoneda->isObjectDead())
			iEnemyCount++;
	}

	// 죽지않은 객체가 없다면 종료
	return 1 > iEnemyCount;
}

void CFightManager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pUIManager);
	Safe_Release(m_pCurrentMonsterGroup);

	Safe_Release(m_pTutorialManager);
}
