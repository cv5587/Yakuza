#include "Level_Roadway.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "FileTotalMgr.h"
#include "Collision_Manager.h"
#include "CarChaseManager.h"
#include "UIManager.h"
#include "FightManager.h"

#include "PlayerCamera.h"
#include "CineCamera.h"
#include "DebugCamera.h"
#include "CarChaseCamera.h"

#include "Highway_Taxi.h"

#include "Level_Loading.h"

CLevel_Roadway::CLevel_Roadway(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext },
    m_pSystemManager{ CSystemManager::GetInstance() },
    m_pFileTotalManager{ CFileTotalMgr::GetInstance() },
	m_pUIManager{CUIManager::GetInstance()},
	m_pFightManager{ CFightManager::GetInstance() }
{
    Safe_AddRef(m_pSystemManager);
    Safe_AddRef(m_pFileTotalManager);
	Safe_AddRef(m_pUIManager);
	Safe_AddRef(m_pFightManager);
}

HRESULT CLevel_Roadway::Initialize()
{
	m_pGameInstance->PlayBGM(TEXT("Ultimate Road Rage.mp3"), DEFAULT_VOLUME);
	m_pCarChaseManager = CCarChaseManager::Create(m_pDevice, m_pContext);
	if (nullptr == m_pCarChaseManager)
		return E_FAIL;

    /* 클라 파싱 */
    m_pFileTotalManager->Set_MapObj_In_Client(STAGE_ROADWAY, LEVEL_CARCHASE);
    m_pFileTotalManager->Set_Lights_In_Client(7);


	if (FAILED(Ready_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	m_pFightManager->Initialize();

	m_pUIManager->Fade_Out();

	m_pSystemManager->Set_Camera(CAMERA_CARCHASE);

	m_fRadialTime = m_pGameInstance->Get_Random(MIN_TIME, MAX_TIME);

	/* 셰이더 옵션값 */
	//m_pGameInstance->Set_HDRLight(1.6f);
	//m_pGameInstance->Set_AdjectTint(_float4(0.814f, 0.810f, 0.782f, 0.f));

    return S_OK;
}

void CLevel_Roadway::Tick(const _float& fTimeDelta)
{
	if (m_isStart == false)
	{
		if (m_pUIManager->isFindFinished())
		{
			m_pFightManager->Set_FightStage(true);
			m_isStart = true;
		}
	}

	if (m_pUIManager->isTitleEnd() && !m_isTitleEnd)
	{
		m_pUIManager->Open_Scene(TEXT("Carchase"));
		m_isTitleEnd = true;

		// 상시 레디얼블러 켜기
		m_pGameInstance->Set_RadialBlur(true);
		m_pGameInstance->Set_RadialSample(static_cast<_uint>(RADIAL_SAMPLE_MIN));
	}
	
	if (m_isTitleEnd)
	{
		m_pCarChaseManager->Tick();

		if (m_isRadialOnEventPlay)
			RadialOnTimer(fTimeDelta);
		else
			RadialOffTimer(fTimeDelta);

		RadialValue_Control();

		if (m_pFightManager->Tick(fTimeDelta))
		{
			if (m_isFadeFin && m_pUIManager->isFindFinished())
			{
				m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_DOGIMAZO));
				return;
			}

			if (!m_pUIManager->isOpen(TEXT("Fade")))
			{
				m_pUIManager->Fade_In();
				m_isFadeFin = true;
			}
		}
	}
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("총격전 맵"));
#endif
}

void CLevel_Roadway::RadialOnTimer(const _float& fTimeDelta)
{
	m_fRadialTimer += fTimeDelta;

	if (RADIAL_TIME <= m_fRadialTimer)
	{
		m_fRadialTimer = 0.f;
		m_isRadialOnEventPlay = false;

		m_fRadialTime = m_pGameInstance->Get_Random(MIN_TIME, MAX_TIME);

		// 켜져있다가 꺼질 때
		dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Taxi"), 0))->Set_BoosterSpeed(1.f);
	}
}

void CLevel_Roadway::RadialOffTimer(const _float& fTimeDelta)
{
	m_fRadialTimer += fTimeDelta;

	if (m_fRadialTime <= m_fRadialTimer)
	{
		m_fRadialTimer = 0.f;
		m_isRadialOnEventPlay = true;

		// 꺼져있다가 켜질때
		dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Taxi"), 0))->Set_BoosterSpeed(1.2f);
	}

}

void CLevel_Roadway::RadialValue_Control()
{
	// 켜져있을 때
	if (m_isRadialOnEventPlay)
	{
		if (m_iRadialValue < RADIAL_SAMPLE_MAX)
		{
			m_iRadialValue++;
			m_pGameInstance->Set_RadialSample(m_iRadialValue);
		}
	}
	else
	{
		if (m_iRadialValue > RADIAL_SAMPLE_MIN)
		{
			m_iRadialValue--;
			m_pGameInstance->Set_RadialSample(m_iRadialValue);
		}
	}
}

HRESULT CLevel_Roadway::Ready_Camera(const wstring& strLayerTag)
{
	m_pFileTotalManager->Reset_Cinemachine();

	/* 카메라 추가 시 Debug Camera를 첫번째로 놔두고 추가해주세요 (디버깅 툴에서 사용중)*/
	const _float4x4* pPlayerFloat4x4 = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(LEVEL_CARCHASE, TEXT("Layer_Taxi"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();

	/* 0. 디버그용 카메라 */
	CDebugCamera::DEBUG_CAMERA_DESC		CameraDesc{};
	CameraDesc.fSensor = 0.1f;
	CameraDesc.vEye = _float4(1.0f, 20.0f, -20.f, 1.f);
	CameraDesc.vFocus = _float4(0.f, 0.0f, 0.0f, 1.f);
	CameraDesc.fFovY = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 300.f;
	CameraDesc.fSpeedPecSec = 40.f;
	CameraDesc.fRotatePecSec = XMConvertToRadians(90.f);
	CameraDesc.pPlayerMatrix = pPlayerFloat4x4;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_CARCHASE, TEXT("Prototype_GameObject_DebugCamera"), strLayerTag, &CameraDesc)))
		return E_FAIL;

	/* 초기화 할때는 -1 */
	/* 1. 씬용 카메라 */
	CCineCamera::CINE_CAMERA_DESC		cineDesc;
	cineDesc.iFileNum = -1;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_CARCHASE, TEXT("Prototype_GameObject_CCineCamera"), strLayerTag, &cineDesc)))
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
	PlayerCameraDesc.pPlayerMatrix = pPlayerFloat4x4;
	PlayerCameraDesc.iCurLevel = LEVEL_CARCHASE;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_CARCHASE, TEXT("Prototype_GameObject_PlayerCamera"), strLayerTag, &PlayerCameraDesc)))
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

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_CARCHASE, TEXT("Prototype_GameObject_CutSceneCamera"), strLayerTag, &CutSceneCameraDesc)))
		return E_FAIL;

	///* 4. 추격전용 카메라 */
	CCarChaseCamera::CARCHASE_CAMERA_DESC		CarChaseCameraDesc{};
	CarChaseCameraDesc.vEye = _float4(3.f, 2.f, 0.f, 1.f);
	CarChaseCameraDesc.vFocus = _float4(0.f, 0.0f, 0.0f, 1.f);
	CarChaseCameraDesc.fFovY = XMConvertToRadians(60.0f);
	CarChaseCameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CarChaseCameraDesc.fNear = 0.1f;
	CarChaseCameraDesc.fFar = 300.f;
	CarChaseCameraDesc.fSpeedPecSec = 10.f;
	CarChaseCameraDesc.fRotatePecSec = XMConvertToRadians(90.f);
	CarChaseCameraDesc.fSensor = 0.1f;
	CHighway_Taxi* pTaxi = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(LEVEL_CARCHASE, TEXT("Layer_Taxi"), 0));
	CarChaseCameraDesc.pPlayerMatrix = pTaxi->Get_TransformCom()->Get_WorldFloat4x4();
	CarChaseCameraDesc.pPlayerBoneMatrix = pTaxi->Get_KiryuBoneMatrix("kubi_c_n");

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_CARCHASE, TEXT("Prototype_GameObject_CarChaseCamera"), strLayerTag, &CarChaseCameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Roadway::Ready_Player(const wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC Desc{};
	Desc.fSpeedPecSec = 10.f;
	//Desc.fRotatePecSec = XMConvertToRadians(0.f);
	Desc.fRotatePecSec = XMConvertToRadians(180.f);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_CARCHASE, TEXT("Prototype_GameObject_Player"), strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}

CLevel_Roadway* CLevel_Roadway::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Roadway* pInstance = new CLevel_Roadway(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
        Safe_Release(pInstance);

    return pInstance;
}

void CLevel_Roadway::Free()
{
    __super::Free();

    Safe_Release(m_pSystemManager);
    Safe_Release(m_pFileTotalManager);
	Safe_Release(m_pCarChaseManager);
	Safe_Release(m_pUIManager);
	Safe_Release(m_pFightManager);
}
