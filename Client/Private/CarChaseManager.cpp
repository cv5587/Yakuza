#include "CarChaseManager.h"

#include "GameInstance.h"
#include "FightManager.h"

#include "CarChase.h"
#include "CarChase_Monster.h"
#include "CarChase_Reactor.h"

#include "Highway_Taxi.h"

void CCarChaseManager::Set_Coll()
{
}

CCarChaseManager::CCarChaseManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice },
	m_pContext{ pContext },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCarChaseManager::Initialize()
{
	Ready_Stage();

	//임시로 플레이어를 생성한다.
	CHighway_Taxi::HIGHWAY_TEXI_DESC Desc{};
	Desc.fRotatePecSec = XMConvertToRadians(180.f);
	Desc.fSpeedPecSec = 10.f;
	Desc.iNaviNum = 0;
	Desc.iNaviRouteNum = LINE_A;
	Desc.iObjectIndex = 999999;
	if (FAILED(m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Taxi"), TEXT("Layer_Taxi"), &Desc)))
		return E_FAIL;

	m_pGameInstance->PlayBGM(L"p_car_run_loop_0.wav", 0.4f);

	return S_OK;
}

_bool CCarChaseManager::Tick()
{
	if (m_isFinished)
		return true;

	if (m_isStart)
	{
		//시작할 때 퀘스트 창 뜨도록 구현 
	}

	//다음 스테이지로 넘어가기
	if (m_Stages[m_iCurrentStage]->Tick())
	{
		m_iCurrentStage++;

		if (m_iCurrentStage >= m_Stages.size())
		{
			m_isFinished = true;
		}
	}

	return false;
}

/*
Stage가 DIR_F, DIR_B	 일 경우 무조건 몬스터 방향은 
DIR_F, DIR_B 이 된다.
*/
HRESULT CCarChaseManager::Ready_Stage()
{
	
	////1 스테이지
	vector<CCarChase::STAGE_MONSTER_INFO> MonsterInfo_1 = {
		CCarChase::STAGE_MONSTER_INFO{
			100000,
			LINE_C,
			DIR_M,
			CCarChase_Monster::REACTOR_SEDAN,
			{ CCarChase_Monster::GUN_L, CCarChase_Monster::GUN_R}
		},
		CCarChase::STAGE_MONSTER_INFO{
			110000,
			LINE_D,
			DIR_B,
			CCarChase_Monster::REACTOR_BIKE,
			{ CCarChase_Monster::DRV, CCarChase_Monster::RKT}
		}
	};

	CCarChase::STAGE_INFO StageInfo_1 = CCarChase::STAGE_INFO{
		LINE_A,
		LINE_A,
		DIR_R,
		MonsterInfo_1
	};

	CCarChase::CARCHASE_DESC Desc{};
	Desc.Info = StageInfo_1;

	CCarChase* pStage1 = CCarChase::Create(&Desc);
	if (nullptr == pStage1)
		return E_FAIL;
	
	m_Stages.emplace_back(pStage1);

	//2 스테이지
	vector<CCarChase::STAGE_MONSTER_INFO> MonsterInfo_2 = {
		CCarChase::STAGE_MONSTER_INFO{
			200000,
			LINE_C,
			DIR_F,
			CCarChase_Monster::REACTOR_VAN,
			{ CCarChase_Monster::WPR, -1}
		},
		CCarChase::STAGE_MONSTER_INFO{
			210000,
			LINE_B,
			DIR_M,
			CCarChase_Monster::REACTOR_BIKE,
			{ CCarChase_Monster::GUN, -1}
		}
	};

	CCarChase::STAGE_INFO StageInfo_2 = CCarChase::STAGE_INFO{
		LINE_A,
		LINE_B,
		DIR_R,
		MonsterInfo_2
	};

	Desc.Info = StageInfo_2;
	CCarChase* pStage2 = CCarChase::Create(&Desc);
	if (nullptr == pStage2)
		return E_FAIL;

	m_Stages.emplace_back(pStage2);

	//3 스테이지
	vector<CCarChase::STAGE_MONSTER_INFO> MonsterInfo_3 = {
		CCarChase::STAGE_MONSTER_INFO{
			300000,
			LINE_A,
			DIR_M,
			CCarChase_Monster::REACTOR_BIKE,
			{ CCarChase_Monster::DRV, CCarChase_Monster::RKT}
		}
	};

	CCarChase::STAGE_INFO StageInfo_3 = CCarChase::STAGE_INFO{
		LINE_B,
		LINE_B,
		DIR_L,
		MonsterInfo_3
	};

	Desc.Info = StageInfo_3;
	CCarChase* pStage3 = CCarChase::Create(&Desc);
	if (nullptr == pStage3)
		return E_FAIL;

	m_Stages.emplace_back(pStage3);

	//4 스테이지
	vector<CCarChase::STAGE_MONSTER_INFO> MonsterInfo_4 = {
		CCarChase::STAGE_MONSTER_INFO{
			400000,
			LINE_C,
			DIR_M,
			CCarChase_Monster::REACTOR_BIKE,
			{ CCarChase_Monster::DRV, CCarChase_Monster::RKT}
		}
	};

	CCarChase::STAGE_INFO StageInfo_4 = CCarChase::STAGE_INFO{
		LINE_B,
		LINE_C,
		DIR_R,
		MonsterInfo_4
	};

	Desc.Info = StageInfo_4;
	CCarChase* pStage4 = CCarChase::Create(&Desc);
	if (nullptr == pStage4)
		return E_FAIL;

	m_Stages.emplace_back(pStage4);

	//5 스테이지
	vector<CCarChase::STAGE_MONSTER_INFO> MonsterInfo_5 = {
		CCarChase::STAGE_MONSTER_INFO{
			500000,
			LINE_A,
			DIR_M,
			CCarChase_Monster::REACTOR_SEDAN,
			{ CCarChase_Monster::GUN_L, CCarChase_Monster::GUN_R}
		},
		CCarChase::STAGE_MONSTER_INFO{
			510000,
			LINE_B,
			DIR_B,
			CCarChase_Monster::REACTOR_SEDAN,
			{ CCarChase_Monster::GUN_L, -1}
		}
	};

	CCarChase::STAGE_INFO StageInfo_5 = CCarChase::STAGE_INFO{
		LINE_C,
		LINE_C,
		DIR_L,
		MonsterInfo_5
	};

	Desc.Info = StageInfo_5;
	CCarChase* pStage5 = CCarChase::Create(&Desc);
	if (nullptr == pStage5)
		return E_FAIL;

	m_Stages.emplace_back(pStage5);

	//6 스테이지
	vector<CCarChase::STAGE_MONSTER_INFO> MonsterInfo_6 = {
		CCarChase::STAGE_MONSTER_INFO{
			600000,
			LINE_A,
			DIR_F,
			CCarChase_Monster::REACTOR_SEDAN,
			{ CCarChase_Monster::GUN_L, -1}
		}
	};

	CCarChase::STAGE_INFO StageInfo_6 = CCarChase::STAGE_INFO{
		LINE_C,
		LINE_A,
		DIR_L,
		MonsterInfo_6
	};

	Desc.Info = StageInfo_6;
	CCarChase* pStage6 = CCarChase::Create(&Desc);
	if (nullptr == pStage6)
		return E_FAIL;
	 
	m_Stages.emplace_back(pStage6);


	//7 스테이지
	vector<CCarChase::STAGE_MONSTER_INFO> MonsterInfo_7 = {
		CCarChase::STAGE_MONSTER_INFO{
			700000,
			LINE_C,
			DIR_M,
			CCarChase_Monster::REACTOR_HELI,
			{ CCarChase_Monster::RKT, CCarChase_Monster::GAT }
		}
	};

	CCarChase::STAGE_INFO StageInfo_7 = CCarChase::STAGE_INFO{
		LINE_A,
		LINE_A,
		DIR_R,
		MonsterInfo_7
	};

	Desc.Info = StageInfo_7;
	CCarChase* pStage7 = CCarChase::Create(&Desc);
	if (nullptr == pStage7)
		return E_FAIL;

	m_Stages.emplace_back(pStage7);
}

CCarChaseManager* CCarChaseManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCarChaseManager* pInstanec = new CCarChaseManager(pDevice, pContext);

	if (FAILED(pInstanec->Initialize()))
		Safe_Release(pInstanec);

	return pInstanec;
}

void CCarChaseManager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	for (auto& pStage : m_Stages)
		Safe_Release(pStage);
}
