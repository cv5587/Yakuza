#include "Level_Test.h"

#include "GameInstance.h"

#include "SystemManager.h"
#include "DebugManager.h"
#include "FileTotalMgr.h"
#include "CarChaseManager.h"
#include "TutorialManager.h"
#include "FightManager.h"
#include "UIManager.h"
#include "KaraokeManager.h"

#include "PlayerCamera.h"
#include "DebugCamera.h"
#include "CineCamera.h"
#include "CutSceneCamera.h"
#include "CarChaseCamera.h"
#include "Highway_Taxi.h"
#include "Player.h"

CLevel_Test::CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() },
	m_pFileTotalManager{ CFileTotalMgr::GetInstance() },
	m_pFightManager{ CFightManager::GetInstance() },
	m_pUIManager{CUIManager::GetInstance()}
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pFileTotalManager);
	Safe_AddRef(m_pFightManager);
	Safe_AddRef(m_pUIManager);
}

HRESULT CLevel_Test::Initialize()
{
	// 14 : 가라오케
	// 11 : 도로

	// 테스트 client 로드 숫자랑 동일하게 설정해주기!!!!!!!!!!!!!!!!!!!!!!!
	// 네비 다르면 터짐
	// 테스트 다하면 지워라
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_7.dat")))))
		return E_FAIL;

	//m_pKaraokeManager = CKaraokeManager::Create();
	//if (nullptr == m_pKaraokeManager)
	//	return E_FAIL;

	//m_pUIManager->Open_Scene(TEXT("Carchase"));

	///* 클라 파싱 */
	//m_pFileTotalManager->Set_MapObj_In_Client(STAGE_ROADWAY, LEVEL_TEST);
	//m_pFileTotalManager->Set_Lights_In_Client(90);
	//m_pFileTotalManager->Set_Collider_In_Client(STAGE_ROADWAY, LEVEL_CARCHASE);

	//if (FAILED(Ready_Camera(TEXT("Layer_Camera"))))
	//	return E_FAIL;

	if (FAILED(Ready_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	///* 클라 파싱 */
	//if (FAILED(Ready_Test_Load()))
	//	return E_FAIL;

	// 혜원테스트용
	//if (FAILED(Ready_Test_Hyewon()))
	//	return E_FAIL;

	m_pFileTotalManager->Set_MapObj_In_Client(10, LEVEL_TEST);
	m_pFileTotalManager->Set_Lights_In_Client(7);
	m_pFileTotalManager->Set_Collider_In_Client(3, LEVEL_TEST);
	m_pFileTotalManager->Set_Trigger_In_Client(80, LEVEL_TEST);

	if (FAILED(Ready_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	m_pFightManager->Initialize();
	m_pFightManager->Set_FightStage(true);
	m_pSystemManager->Set_Camera(CAMERA_PLAYER);

	return S_OK;
}

void CLevel_Test::Tick(const _float& fTimeDelta)
{
	if (!m_isTitleEnd && m_pUIManager->isBattleStart())
	{
		m_isTitleEnd = true;
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
		pPlayer->Battle_Start();
	}

	m_pFightManager->Tick(fTimeDelta);
	//m_pKaraokeManager->Tick(fTimeDelta);
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("테스트 레벨"));
#endif
}

HRESULT CLevel_Test::Ready_Camera(const wstring& strLayerTag)
{
	/* 카메라 추가 시 Debug Camera를 첫번째로 놔두고 추가해주세요 (디버깅 툴에서 사용중)*/

	/* 0. 디버그용 카메라 */
	CDebugCamera::DEBUG_CAMERA_DESC		CameraDesc{};
	CameraDesc.fSensor = 0.1f;
	CameraDesc.vEye = _float4(1.0f, 20.0f, -20.f, 1.f);
	CameraDesc.vFocus = _float4(0.f, 0.0f, 0.0f, 1.f);
	CameraDesc.fFovY = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 300.f;
	CameraDesc.fSpeedPecSec = 10.f;
	CameraDesc.fRotatePecSec = XMConvertToRadians(90.f);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Prototype_GameObject_DebugCamera"), strLayerTag, &CameraDesc)))
		return E_FAIL;

	/* 초기화 할때는 -1 */
	/* 1. 씬용 카메라 */
	CCineCamera::CINE_CAMERA_DESC		cineDesc;
	cineDesc.iFileNum = -1;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Prototype_GameObject_CCineCamera"), strLayerTag, &cineDesc)))
		return E_FAIL;

	/* 2. 플레이어 카메라 */
	CPlayerCamera::PLAYER_CAMERA_DESC		PlayerCameraDesc{};
	PlayerCameraDesc.fSensor = 5.f;
	PlayerCameraDesc.vEye = _float4(0.f, 2.0f, -3.f, 1.f);
	PlayerCameraDesc.vFocus = _float4(0.f, 0.0f, 0.0f, 1.f);
	PlayerCameraDesc.fFovY = XMConvertToRadians(60.0f);
	PlayerCameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	PlayerCameraDesc.fNear = 0.1f;
	PlayerCameraDesc.fFar = 300.f;
	PlayerCameraDesc.fSpeedPecSec = 20.f;
	PlayerCameraDesc.fRotatePecSec = XMConvertToRadians(90.f);
	PlayerCameraDesc.pPlayerMatrix = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(LEVEL_TEST, TEXT("Layer_Player"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Prototype_GameObject_PlayerCamera"), strLayerTag, &PlayerCameraDesc)))
		return E_FAIL;

	/* 3. 컷신용 카메라 */
	CCamera::CAMERA_DESC		CutSceneCameraDesc{};
	CutSceneCameraDesc.vEye = _float4(1.0f, 20.0f, -20.f, 1.f);
	CutSceneCameraDesc.vFocus = _float4(0.f, 0.0f, 0.0f, 1.f);
	CutSceneCameraDesc.fFovY = XMConvertToRadians(60.0f);
	CutSceneCameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CutSceneCameraDesc.fNear = 0.1f;
	CutSceneCameraDesc.fFar = 300.f;
	CutSceneCameraDesc.fSpeedPecSec = 10.f;
	CutSceneCameraDesc.fRotatePecSec = XMConvertToRadians(90.f);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Prototype_GameObject_CutSceneCamera"), strLayerTag, &CutSceneCameraDesc)))
		return E_FAIL;

	/////* 4. 추격전용 카메라 */
	//CCarChaseCamera::CARCHASE_CAMERA_DESC		CarChaseCameraDesc{};
	//CarChaseCameraDesc.vEye = _float4(3.f, 2.f, 0.f, 1.f);
	//CarChaseCameraDesc.vFocus = _float4(0.f, 0.0f, 0.0f, 1.f);
	//CarChaseCameraDesc.fFovY = XMConvertToRadians(60.0f);
	//CarChaseCameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	//CarChaseCameraDesc.fNear = 0.1f;
	//CarChaseCameraDesc.fFar = 3000.f;
	//CarChaseCameraDesc.fSpeedPecSec = 10.f;
	//CarChaseCameraDesc.fRotatePecSec = XMConvertToRadians(90.f);
	//CarChaseCameraDesc.fSensor = 0.1f;
	////CHighway_Taxi* pTaxi = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(LEVEL_TEST, TEXT("Layer_Taxi"), 0));
	////CarChaseCameraDesc.pPlayerMatrix = pTaxi->Get_TransformCom()->Get_WorldFloat4x4();
	////CarChaseCameraDesc.pPlayerBoneMatrix = pTaxi->Get_KiryuBoneMatrix("kubi_c_n");

	//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Prototype_GameObject_CarChaseCamera"), strLayerTag, &CarChaseCameraDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Test::Ready_Player(const wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC Desc{};
	Desc.fSpeedPecSec = 10.f;
	//Desc.fRotatePecSec = XMConvertToRadians(0.f);
	Desc.fRotatePecSec = XMConvertToRadians(180.f);

	// 원래 플레이어
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Prototype_GameObject_Player"), strLayerTag, &Desc)))
		return E_FAIL;

	// 가라오케 플레이어
	//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Prototype_GameObject_Kiryu_Karaoke"), strLayerTag, &Desc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Test::Ready_Test_Load()
{
	m_pFileTotalManager->Set_MapObj_In_Client(9, LEVEL_TEST);
	m_pFileTotalManager->Set_Lights_In_Client(99);
	m_pFileTotalManager->Set_Collider_In_Client(0, LEVEL_TEST);
	m_pFileTotalManager->Set_Trigger_In_Client(79, LEVEL_TEST);

	return S_OK;
}

HRESULT CLevel_Test::Ready_Test_Hyewon()
{
	// 가라오케 네비는 9번ㄴ
	// 가라오케 맵 81번
	//m_pFileTotalManager->Set_MapObj_In_Client(81, LEVEL_TEST);
	//m_pFileTotalManager->Set_Lights_In_Client(9);

	// 몬스터 깔린 맵: 92
	// 몬스터 깔린 네비: 3
	// 과거의 길거리맵 79
	// 과거의 길거리맵 네비: 79

	// 빈 맵: 93
	// 네비: 90
	m_pFileTotalManager->Set_MapObj_In_Client(9, LEVEL_TEST);
	m_pFileTotalManager->Set_Lights_In_Client(99);
	m_pFileTotalManager->Set_Collider_In_Client(0, LEVEL_TEST);
	m_pFileTotalManager->Set_Trigger_In_Client(79, LEVEL_TEST);

	return S_OK;
}


CLevel_Test* CLevel_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Test* pInstance = new CLevel_Test(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLevel_Test");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Test::Free()
{
	__super::Free();

	Safe_Release(m_pSystemManager);
	Safe_Release(m_pFileTotalManager);
	Safe_Release(m_pUIManager);
	Safe_Release(m_pKaraokeManager);
	Safe_Release(m_pFightManager);
}
