#include "Level_Dogimazo.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "FileTotalMgr.h"
#include "Collision_Manager.h"
#include "FightManager.h"
#include "UIManager.h"

#include "PlayerCamera.h"
#include "CineCamera.h"
#include "DebugCamera.h"

#include "Level_Loading.h"
#include "Trigger.h"
#include "Player.h"

CLevel_Dogimazo::CLevel_Dogimazo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext },
    m_pSystemManager{ CSystemManager::GetInstance() },
    m_pFileTotalManager{ CFileTotalMgr::GetInstance() },
	m_pFightManager{ CFightManager::GetInstance() },
	m_pUIManager{ CUIManager::GetInstance() }
{
    Safe_AddRef(m_pSystemManager);
    Safe_AddRef(m_pFileTotalManager);
	Safe_AddRef(m_pFightManager);
	Safe_AddRef(m_pUIManager);
}

HRESULT CLevel_Dogimazo::Initialize()
{
	if (FAILED(Ready_Player(TEXT("Layer_Player"))))
		return E_FAIL;

    /* 클라 파싱 */
    m_pFileTotalManager->Set_MapObj_In_Client(STAGE_DOGIMAZO, LEVEL_DOGIMAZO);
    m_pFileTotalManager->Set_Lights_In_Client(STAGE_DOGIMAZO);
    m_pFileTotalManager->Set_Collider_In_Client(STAGE_DOGIMAZO, LEVEL_DOGIMAZO);
	m_pFileTotalManager->Set_Trigger_In_Client(STAGE_DOGIMAZO, LEVEL_DOGIMAZO);			// 요네다 트리거용

	/* 쇼파 몬스터 */
	m_pFileTotalManager->Set_Trigger_In_Client(40, LEVEL_DOGIMAZO);
	/* 계단 오른 후 몬스터 */
	m_pFileTotalManager->Set_Trigger_In_Client(41, LEVEL_DOGIMAZO);
	/* 계단 오른 후 몬스터 */
	m_pFileTotalManager->Set_Trigger_In_Client(43, LEVEL_DOGIMAZO);


	if (FAILED(Ready_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	m_pFightManager->Initialize();
	m_pSystemManager->Set_Camera(CAMERA_PLAYER);
	m_pUIManager->Fade_Out();
//	m_pFightManager->Set_FightStage(true);

		/*셰이더 옵션값*/
	//m_pGameInstance->Set_HDRLight(1.3f);
	//m_pGameInstance->Set_AdjectTint(_float4(0.873f, 0.820f, 0.753f, 0.f));

	m_pGameInstance->PlayBGM(TEXT("Dozimazo_BGM.mp3"), DEFAULT_VOLUME);

    return S_OK;
}

void CLevel_Dogimazo::Tick(const _float& fTimeDelta)
{
	/*//기존
	if (m_isStart == false)
	{
		if (m_pUIManager->isFindFinished())
		{
			m_pUIManager->Close_Scene(TEXT("Fade"));
		}
	}

	if (m_bSceneChange)
	{
		if (!m_pUIManager->isOpen(TEXT("Fade")))
		{
			m_pUIManager->Open_Scene(TEXT("Fade"));
			m_pUIManager->Fade_In();
		}
		else
		{
			if (m_pUIManager->isFindFinished())
			{
				m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_DOGIMAZO_STAIRS));
				return;
			}
		}
	}*/
	if (m_isStart == false)
	{
		if (m_pUIManager->isFindFinished())
		{
			m_pFightManager->Set_FightStage(true);
			m_isStart = true;
		}
	}

	if (m_bSceneChange)
	{

		if (m_isFadeFin && m_pUIManager->isFindFinished())
		{
			m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_DOGIMAZO_STAIRS));
			CCollision_Manager::GetInstance()->All_Clear();
			return;
		}

		if (!m_pUIManager->isOpen(TEXT("Fade")))
		{
			m_pUIManager->Fade_In();
			m_isFadeFin = true;
		}
	}


	// 트리거 체크 - 씬 이동
	vector<CGameObject*> pTriggers = m_pGameInstance->Get_GameObjects(LEVEL_DOGIMAZO, TEXT("Layer_Trigger"));
	for (int i = 0; i < pTriggers.size(); i++)
	{
		int		iLevelNum;
		if (true == dynamic_cast<CTrigger*>(pTriggers[i])->Move_Scene(iLevelNum))
		{
			m_bSceneChange = true;
		}
	}

#ifdef _DEBUG
	if (false == m_bSceneChange)
	{
		if (m_pGameInstance->GetKeyState(DIK_RETURN) == TAP)
		{
			if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_DOGIMAZO_STAIRS))))
				return;
		}
	}
#endif // _DEBUG
	
	m_pFightManager->Tick(fTimeDelta);

	if (!m_isTitleEnd && m_pUIManager->isBattleStart())
	{
		m_isTitleEnd = true;
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
		pPlayer->Battle_Start();
	}

#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("도지마조"));
#endif
}

HRESULT CLevel_Dogimazo::Ready_Camera(const wstring& strLayerTag)
{
	m_pFileTotalManager->Reset_Cinemachine();

	/* 카메라 추가 시 Debug Camera를 첫번째로 놔두고 추가해주세요 (디버깅 툴에서 사용중)*/
	const _float4x4* pPlayerFloat4x4 = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(LEVEL_DOGIMAZO, TEXT("Layer_Player"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();

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

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_DOGIMAZO, TEXT("Prototype_GameObject_DebugCamera"), strLayerTag, &CameraDesc)))
		return E_FAIL;

	/* 초기화 할때는 -1 */
	/* 1. 씬용 카메라 */
	CCineCamera::CINE_CAMERA_DESC		cineDesc;
	cineDesc.iFileNum = -1;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_DOGIMAZO, TEXT("Prototype_GameObject_CCineCamera"), strLayerTag, &cineDesc)))
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
	PlayerCameraDesc.iCurLevel = LEVEL_DOGIMAZO;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_DOGIMAZO, TEXT("Prototype_GameObject_PlayerCamera"), strLayerTag, &PlayerCameraDesc)))
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

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_DOGIMAZO, TEXT("Prototype_GameObject_CutSceneCamera"), strLayerTag, &CutSceneCameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Dogimazo::Ready_Player(const wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC Desc{};
	Desc.fSpeedPecSec = 10.f;
	//Desc.fRotatePecSec = XMConvertToRadians(0.f);
	Desc.fRotatePecSec = XMConvertToRadians(180.f);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_DOGIMAZO, TEXT("Prototype_GameObject_Player"), strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}

CLevel_Dogimazo* CLevel_Dogimazo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Dogimazo* pInstance = new CLevel_Dogimazo(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
        Safe_Release(pInstance);

    return pInstance;
}

void CLevel_Dogimazo::Free()
{
    __super::Free();

	CCollision_Manager::GetInstance()->All_Clear();

    Safe_Release(m_pSystemManager);
    Safe_Release(m_pFileTotalManager);
	Safe_Release(m_pFightManager);
	Safe_Release(m_pUIManager);
}
