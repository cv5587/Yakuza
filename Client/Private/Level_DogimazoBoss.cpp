#include "Level_DogimazoBoss.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "FileTotalMgr.h"
#include "Collision_Manager.h"
#include "FightManager.h"
#include "UIManager.h"
#include "QuestManager.h"

#include "PlayerCamera.h"
#include "CineCamera.h"
#include "DebugCamera.h"

#include "Level_Loading.h"
#include "Player.h"

CLevel_DogimazoBoss::CLevel_DogimazoBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext },
    m_pSystemManager{ CSystemManager::GetInstance() },
    m_pFileTotalManager{ CFileTotalMgr::GetInstance() },
	m_pFightManager{ CFightManager::GetInstance() },
	m_pUIManager{ CUIManager::GetInstance() },
	m_pQuestManager{ CQuestManager::GetInstance() }
{
    Safe_AddRef(m_pSystemManager);
    Safe_AddRef(m_pFileTotalManager);
	Safe_AddRef(m_pFightManager);
	Safe_AddRef(m_pUIManager);
	Safe_AddRef(m_pQuestManager);
}

HRESULT CLevel_DogimazoBoss::Initialize()
{
    if (FAILED(Ready_Player(TEXT("Layer_Player"))))
        return E_FAIL;

    /* 클라 파싱 */
    m_pFileTotalManager->Set_MapObj_In_Client(STAGE_DOGIMAZO_BOSS, LEVEL_DOGIMAZO_BOSS);
    m_pFileTotalManager->Set_Lights_In_Client(STAGE_DOGIMAZO_BOSS);
    m_pFileTotalManager->Set_Collider_In_Client(STAGE_DOGIMAZO_BOSS, LEVEL_DOGIMAZO_BOSS);

	if (FAILED(Ready_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	//m_pFightManager->Initialize();
	m_pFileTotalManager->Load_Cinemachine(25, LEVEL_DOGIMAZO_BOSS);
	m_pFileTotalManager->Load_Cinemachine(26, LEVEL_DOGIMAZO_BOSS);
	m_pFileTotalManager->Load_Cinemachine(27, LEVEL_DOGIMAZO_BOSS);
	m_pFileTotalManager->Load_Cinemachine(28, LEVEL_DOGIMAZO_BOSS);
	
	m_pQuestManager->Start_Quest(CQuestManager::CHAPTER_8);

	m_pUIManager->Fade_Out();

	m_pGameInstance->PlayBGM(TEXT("Kuze_BGM.mp3"), DEFAULT_VOLUME);

	/*셰이더 옵션값*/
	//m_pGameInstance->Set_HDRLight(1.3f);
	//m_pGameInstance->Set_AdjectTint(_float4(0.873f, 0.820f, 0.753f, 0.f));


    return S_OK;
}

void CLevel_DogimazoBoss::Tick(const _float& fTimeDelta)
{
	if (!m_isFightStart && m_pQuestManager->Execute())
	{
		m_pFightManager->Set_FightStage(true);
		m_pSystemManager->Set_Camera(CAMERA_PLAYER);
		m_isFightStart = true;
	}

	if (m_isFightStart)
	{
		m_pFightManager->Tick(fTimeDelta);

		if (!m_isTitleEnd && m_pUIManager->isBattleStart())
		{
			m_isTitleEnd = true;
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
			pPlayer->Battle_Start();
		}
	}

#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("도지마조 보스 스테이지"));
#endif
}

HRESULT CLevel_DogimazoBoss::Ready_Camera(const wstring& strLayerTag)
{
	m_pFileTotalManager->Reset_Cinemachine();

	/* 카메라 추가 시 Debug Camera를 첫번째로 놔두고 추가해주세요 (디버깅 툴에서 사용중)*/
	const _float4x4* pPlayerFloat4x4 = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(LEVEL_DOGIMAZO_BOSS, TEXT("Layer_Player"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();

	/* 0. 디버그용 카메라 */
	CDebugCamera::DEBUG_CAMERA_DESC		CameraDesc{};
	CameraDesc.fSensor = 0.1f;
	CameraDesc.vEye = _float4(1.0f, 20.0f, -20.f, 1.f);
	CameraDesc.vFocus = _float4(0.f, 0.0f, 0.0f, 1.f);
	CameraDesc.fFovY = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPecSec = 10.f;
	CameraDesc.fRotatePecSec = XMConvertToRadians(90.f);
	CameraDesc.pPlayerMatrix = pPlayerFloat4x4;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_DOGIMAZO_BOSS, TEXT("Prototype_GameObject_DebugCamera"), strLayerTag, &CameraDesc)))
		return E_FAIL;

	/* 초기화 할때는 -1 */
	/* 1. 씬용 카메라 */
	CCineCamera::CINE_CAMERA_DESC		cineDesc;
	cineDesc.iFileNum = -1;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_DOGIMAZO_BOSS, TEXT("Prototype_GameObject_CCineCamera"), strLayerTag, &cineDesc)))
		return E_FAIL;

	/* 2. 플레이어 카메라 */
	CPlayerCamera::PLAYER_CAMERA_DESC		PlayerCameraDesc{};
	PlayerCameraDesc.fSensor = 5.f;
	PlayerCameraDesc.vEye = _float4(0.f, 2.0f, -3.f, 1.f);
	PlayerCameraDesc.vFocus = _float4(0.f, 0.0f, 0.0f, 1.f);
	PlayerCameraDesc.fFovY = XMConvertToRadians(60.0f);
	PlayerCameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	PlayerCameraDesc.fNear = 0.1f;
	PlayerCameraDesc.fFar = 3000.f;
	PlayerCameraDesc.fSpeedPecSec = 20.f;
	PlayerCameraDesc.fRotatePecSec = XMConvertToRadians(90.f);
	PlayerCameraDesc.pPlayerMatrix = pPlayerFloat4x4;
	PlayerCameraDesc.iCurLevel = LEVEL_DOGIMAZO_BOSS;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_DOGIMAZO_BOSS, TEXT("Prototype_GameObject_PlayerCamera"), strLayerTag, &PlayerCameraDesc)))
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

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_DOGIMAZO_BOSS, TEXT("Prototype_GameObject_CutSceneCamera"), strLayerTag, &CutSceneCameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_DogimazoBoss::Ready_Player(const wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC Desc{};
	Desc.fSpeedPecSec = 10.f;
	//Desc.fRotatePecSec = XMConvertToRadians(0.f);
	Desc.fRotatePecSec = XMConvertToRadians(180.f);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_DOGIMAZO_BOSS, TEXT("Prototype_GameObject_Player"), strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}

CLevel_DogimazoBoss* CLevel_DogimazoBoss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_DogimazoBoss* pInstance = new CLevel_DogimazoBoss(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
        Safe_Release(pInstance);

    return pInstance;
}

void CLevel_DogimazoBoss::Free()
{
    __super::Free();

	CCollision_Manager::GetInstance()->All_Clear();

    Safe_Release(m_pSystemManager);
    Safe_Release(m_pFileTotalManager);
	Safe_Release(m_pFightManager);
	Safe_Release(m_pUIManager);
	Safe_Release(m_pQuestManager);
}
