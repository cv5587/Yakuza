#include "Level_Street.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "FileTotalMgr.h"
#include "Collision_Manager.h"
#include "QuestManager.h"
#include "UIManager.h"

#include "PlayerCamera.h"
#include "CineCamera.h"
#include "DebugCamera.h"

#include "Level_Loading.h"

#include "Player.h"

CLevel_Street::CLevel_Street(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext },
    m_pSystemManager{ CSystemManager::GetInstance() },
    m_pFileTotalManager{ CFileTotalMgr::GetInstance() },
	m_pQuestManager { CQuestManager::GetInstance() },
	m_pUIManager {CUIManager::GetInstance() }
{
    Safe_AddRef(m_pSystemManager);
    Safe_AddRef(m_pFileTotalManager);
	Safe_AddRef(m_pQuestManager);
	Safe_AddRef(m_pUIManager);
}

HRESULT CLevel_Street::Initialize()
{
    if (FAILED(Ready_Player(TEXT("Layer_Player"))))
        return E_FAIL;

	m_pUIManager->Close_Scene();

    /* 클라 파싱 */
	if (m_pGameInstance->Get_CurrentLevel() == LEVEL_TOKOSTREET)
	{
		m_pFileTotalManager->Set_MapObj_In_Client(STAGE_TOKOSTREET, m_pGameInstance->Get_CurrentLevel());
		m_pFileTotalManager->Set_Trigger_In_Client(STAGE_TOKOSTREET, m_pGameInstance->Get_CurrentLevel());
	}
	else if (m_pGameInstance->Get_CurrentLevel() == LEVEL_STREET)
	{
		m_pFileTotalManager->Set_MapObj_In_Client(STAGE_STREET, m_pGameInstance->Get_CurrentLevel());
		m_pFileTotalManager->Set_Trigger_In_Client(STAGE_STREET, m_pGameInstance->Get_CurrentLevel());
	}
    
	m_pFileTotalManager->Set_Lights_In_Client(7);

	m_pUIManager->Fade_Out();

	m_pSystemManager->Set_Camera(CAMERA_PLAYER);
	if (FAILED(Ready_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	
	/*셰이더 옵션값*/
	m_pGameInstance->Set_HDRLight(0.8f);

	m_pGameInstance->Play_Loop(L"48e6 [1].wav", SOUND_BGM, DEFAULT_VOLUME);
	m_pGameInstance->Play_Loop(L"48a1 [1].wav", SOUND_BGM_2, DEFAULT_VOLUME);
	m_pGameInstance->Play_Loop(L"4887 [1].wav", SOUND_BGM_3, DEFAULT_VOLUME);
	m_pGameInstance->Play_Loop(L"4899 [1].wav", SOUND_BGM_1, DEFAULT_VOLUME);

    return S_OK;
}

void CLevel_Street::Tick(const _float& fTimeDelta)
{
	if (m_isStart == false)
	{
		if (m_pUIManager->isFindFinished())
		{
			if (m_pGameInstance->Get_CurrentLevel() == LEVEL_TOKOSTREET)
				m_pQuestManager->Start_Quest(CQuestManager::CHAPTER_5);
			else if (m_pGameInstance->Get_CurrentLevel() == LEVEL_STREET)
				m_pQuestManager->Start_Quest(CQuestManager::CHAPTER_7);

			m_isStart = true;
		}
	}

	if (m_pQuestManager->Execute())
	{
		if (m_isFadeFin && m_pUIManager->isFindFinished())
		{
			if (m_pGameInstance->Get_CurrentLevel() == LEVEL_TOKOSTREET)
				m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_OFFICE_1F));
			else if (m_pGameInstance->Get_CurrentLevel() == LEVEL_STREET)
			{
				m_pGameInstance->PlaySound_W(TEXT("0001 [53].wav"), SOUND_EFFECT, DEFAULT_EFFECT_VOLUME);
				m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CARCHASE));

				return;
			}
		}
		if (!m_pUIManager->isOpen(TEXT("Fade")))
		{
			m_pUIManager->Fade_In();
			m_isFadeFin = true;
		}
	}
#ifdef _DEBUG
		SetWindowText(g_hWnd, TEXT("길거리 맵"));
#endif
}

HRESULT CLevel_Street::Ready_Camera(const wstring& strLayerTag)
{
	m_pFileTotalManager->Reset_Cinemachine();

	/* 카메라 추가 시 Debug Camera를 첫번째로 놔두고 추가해주세요 (디버깅 툴에서 사용중)*/
	const _float4x4* pPlayerFloat4x4 = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();

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

	if (FAILED(m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_DebugCamera"), strLayerTag, &CameraDesc)))
		return E_FAIL;

	/* 초기화 할때는 -1 */
	/* 1. 씬용 카메라 */
	CCineCamera::CINE_CAMERA_DESC		cineDesc;
	cineDesc.iFileNum = -1;
	if (FAILED(m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_CCineCamera"), strLayerTag, &cineDesc)))
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
	PlayerCameraDesc.iCurLevel = m_pGameInstance->Get_CurrentLevel();

	if (FAILED(m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_PlayerCamera"), strLayerTag, &PlayerCameraDesc)))
		return E_FAIL;

	/* 3. 컷신용 카메라 */
	CCamera::CAMERA_DESC		CutSceneCameraDesc{};
	CutSceneCameraDesc.vEye = _float4(1.0f, 20.0f, -20.f, 1.f);
	CutSceneCameraDesc.vFocus = _float4(0.f, 0.0f, 0.0f, 1.f);
	CutSceneCameraDesc.fFovY = XMConvertToRadians(60.0f);
	CutSceneCameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CutSceneCameraDesc.fNear = 0.1f;
	CutSceneCameraDesc.fFar = 3000.f;
	CutSceneCameraDesc.fSpeedPecSec = 10.f;
	CutSceneCameraDesc.fRotatePecSec = XMConvertToRadians(90.f);

	if (FAILED(m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_CutSceneCamera"), strLayerTag, &CutSceneCameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Street::Ready_Player(const wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC Desc{};
	Desc.fSpeedPecSec = 10.f;
	//Desc.fRotatePecSec = XMConvertToRadians(0.f);
	Desc.fRotatePecSec = XMConvertToRadians(180.f);

	if (FAILED(m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Player"), strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Street::Play_EnvironmentSound()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STREET, TEXT("Layer_Player"), 0));

	_vector		vPlayerPos = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	if (-7 > vPlayerPos.m128_f32[2])
	{
		m_pGameInstance->PlaySoundIfNotPlay(L"4899 [1].wav", SOUND_ENVIRONMENT, 0.5f);
	}
}

CLevel_Street* CLevel_Street::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Street* pInstance = new CLevel_Street(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
        Safe_Release(pInstance);

    return pInstance;
}

void CLevel_Street::Free()
{
    __super::Free();

	CCollision_Manager::GetInstance()->All_Clear();

    Safe_Release(m_pSystemManager);
    Safe_Release(m_pFileTotalManager);
	Safe_Release(m_pQuestManager);
	Safe_Release(m_pUIManager);
}
