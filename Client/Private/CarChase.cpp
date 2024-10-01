#include "CarChase.h"

#include "GameInstance.h"
#include "UIManager.h"

#include "CarChase_Monster.h"
#include "CarChase_Reactor.h"
#include "CarChaseCamera.h"

#include "Highway_Taxi.h"
#include "Highway_Kiryu.h"

CCarChase::CCarChase()
	: m_pGameInstance{ CGameInstance::GetInstance() },
	m_pUIManager{ CUIManager::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pUIManager);
}

CCarChase::~CCarChase()
{
}

HRESULT CCarChase::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	CARCHASE_DESC* pDesc = static_cast<CARCHASE_DESC*>(pArg);
	m_Info = pDesc->Info;

	m_iState = START;

	return S_OK;
}

//틱을 돌면서 스테이트 관리를 시작한다.
_bool CCarChase::Tick()
{
	if (m_iState == START)
		m_iState = Start() ? RUNNING : START;
	else if (m_iState == RUNNING)
		m_iState = Running() ? STAGE_END : RUNNING;
	else
		return End();
		
	return false;
}

//몬스터 생성 & 플레이어 dir / 카메라 dir 수정
_bool CCarChase::Start()
{
	vector<STAGE_MONSTER_INFO> MonsterInfo = m_Info.MonsterInfo;
	CHighway_Taxi* pPlayer = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Taxi"), 0));

	//몬스터의 위치는 플레이어 웨이포인트 인덱스의 -10만큼
	_int iWaypoint = m_Info.iStageDir == DIR_F ? pPlayer->Get_CurrentWaypointIndex() + 3: pPlayer->Get_CurrentWaypointIndex() - 10;

	if (iWaypoint < 0)
	{
		//0보다 작다면
		iWaypoint = pPlayer->Get_WaypointSize() + iWaypoint;
	}

	//몬스터 생성
	for (auto& iter : MonsterInfo)
	{
		CCarChase_Reactor::HIGHWAY_IODESC Desc{};
		Desc.fRotatePecSec = XMConvertToRadians(180.f);
		Desc.fSpeedPecSec = 10.f;
		Desc.iNaviNum = 0;
		Desc.iStageDir = m_Info.iStageDir; // 스테이지 등장 방향
		Desc.iLineDir = iter.iMonsterDir; // 몬스터 앞, 옆, 뒤 방향
		Desc.iObjectIndex = iter.iObjectIndex; // 오브젝트 id 값
		Desc.iNaviRouteNum = iter.iMonsterLine; // 몬스터가 사용할 line의 index
		Desc.iWaypointIndex = iWaypoint; // 생성될 waypoint
		memcpy(Desc.iMonsterWeaponType, iter.iWeaponType, sizeof(_int) * 2); // 몬스터 종류 (탈 것마다 몬스터가 다르다)

		wstring strGameObject = TEXT("");

		if (iter.iReactorType == CCarChase_Monster::REACTOR_VAN)
			strGameObject = TEXT("Prototype_GameObject_ReactorVan");
		else if (iter.iReactorType == CCarChase_Monster::REACTOR_SEDAN)
			strGameObject = TEXT("Prototype_GameObject_ReactorSedan");
		else if (iter.iReactorType == CCarChase_Monster::REACTOR_BIKE)
			strGameObject = TEXT("Prototype_GameObject_ReactorBike");
		else if (iter.iReactorType == CCarChase_Monster::REACTOR_HELI)
			strGameObject = TEXT("Prototype_GameObject_ReactorHeli");

		if (FAILED(m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), strGameObject, TEXT("Layer_Reactor"), &Desc)))
			return false;
	}

	Set_TaxiStageDir();

	//카메라에 스테이지 방향 넣어주기
	CCarChaseCamera* pCamera = dynamic_cast<CCarChaseCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_CARCHASE));
	pCamera->Set_StageDir(m_Info.iStageDir);

	return true;
}

//다음 스테이지로 넘어가기 위한 조건문 생성
_bool CCarChase::Running()
{
	vector<CGameObject*> Reactors = m_pGameInstance->Get_GameObjects(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Reactor"));
	vector<STAGE_MONSTER_INFO> MonsterInfo = m_Info.MonsterInfo;

	/*_uint iCount = { 0 };
	for (auto& Reactor : Reactors)
	{
		find_if(MonsterInfo.begin(), MonsterInfo.end(), [&](STAGE_MONSTER_INFO& Info)->_bool {
			if (Info.iObjectIndex == Reactor->Get_ObjID())
			{
				if (Reactor->isObjectDead())
					iCount++;

				return true;
			}
		});
	}

	if (MonsterInfo.size() == iCount)
	{
		for (auto& pReactor : Reactors)
			pReactor->Set_Dead();

		return true;
	}*/

	//Set_TaxiStageDir();				//달리는 중에도 몬스터의 위치는 바뀔 수 있으므로, 각도 체인지

	if (Reactors.size() <= 0)
		return true;

	return false;
}

//플레이어 자리 이동
_bool CCarChase::End()
{
	// 다음 스테이지 방향과 지금 스테이지 방향이 다르다면
	if (m_Info.iPlayerLine != m_Info.iNextPlayerLine)
	{
		CHighway_Taxi* pPlayer = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Taxi"), 0));
		pPlayer->Set_NavigationRouteIndex(m_Info.iNextPlayerLine);
		pPlayer->Sit_Swap();
	}

	return true;
}

void CCarChase::Set_TaxiStageDir()
{
	CHighway_Taxi* pPlayer = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Taxi"), 0));

	//플레이어 스테이지 방향 값 넣어줌
	if (nullptr == m_pUIManager->Get_Target())
		pPlayer->Set_Dir(m_Info.iStageDir);
	else
	{
		_vector vTaxiPos = XMLoadFloat3(pPlayer->Get_Kiryu()->Get_Pos());
		_vector vTargetPos = m_pUIManager->Get_Target()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

		_vector vDir = XMVector3Normalize(vTargetPos- vTaxiPos);
		_vector vTaxiLook = XMLoadFloat3(pPlayer->Get_Kiryu()->Get_Right());

		_float fDot = acos(XMVectorGetX(XMVector3Dot(vDir, vTaxiLook)));

		_uint iDir = m_Info.iStageDir;

		//const float PI = 3.14159265359f;
		//const float FRONT_ANGLE = PI / 8;          // 22.5도
		//const float FRONT_DIAGONAL_ANGLE = 3 * PI / 8;  // 67.5도
		//const float SIDE_ANGLE = 5 * PI / 8;       // 112.5도
		//const float BACK_DIAGONAL_ANGLE = 7 * PI / 8;  // 157.5도

		const float FRONT_ANGLE = XMConvertToRadians(10.f);
		const float FRONT_DIAGONAL_ANGLE = XMConvertToRadians(45.f);
		const float SIDE_ANGLE = XMConvertToRadians(160.f);
		const float BACK_DIAGONAL_ANGLE = XMConvertToRadians(170.f);

		if (fDot <= FRONT_ANGLE) {
			// 앞 (Front)
			iDir = 0;
		}
		else if (fDot <= FRONT_DIAGONAL_ANGLE) {
			// 앞대각선 (Front Diagonal)
			iDir = 1;
		}
		else if (fDot <= SIDE_ANGLE) {
			// 옆 (Side)
			iDir = 2;
		}
		else if (fDot <= BACK_DIAGONAL_ANGLE) {
			// 뒷대각선 (Back Diagonal)
			iDir = 3;
		}
		else {
			// 뒤 (Back)
			iDir = 4;
		}

		// 0 앞, 1 앞 대각, 2 옆, 3 뒷 대각, 4 뒤
		pPlayer->Set_Dir(iDir);
	}
}

CCarChase* CCarChase::Create(void* pArg)
{
	CCarChase* pInstance = new CCarChase();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCarChase::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pUIManager);
}
