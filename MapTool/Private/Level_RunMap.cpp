#include "../Public/Level_RunMap.h"
#include "GameInstance.h"
#include "SystemManager.h"
#include "PlayerCamera.h"

CLevel_RunMap::CLevel_RunMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_RunMap::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_GameObject(TEXT("Layer_GameObject"))))
		return E_FAIL;
	
	m_pGameInstance->Set_RimLight(false);

	return S_OK;
}

void CLevel_RunMap::Tick(const _float& fTimeDelta)
{

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("테스트 레벨"));
#endif
}

HRESULT CLevel_RunMap::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	m_pGameInstance->Add_Light(LightDesc);

	return S_OK;
}

HRESULT CLevel_RunMap::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CPlayerCamera::PLAYER_CAMERA_DESC		CameraDesc{};

	CameraDesc.fSensor = 0.1f;
	CameraDesc.vEye = _float4(1.0f, 20.0f, -20.f, 1.f);
	CameraDesc.vFocus = _float4(0.f, 0.0f, 0.0f, 1.f);
	CameraDesc.fFovY = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000;
	CameraDesc.fSpeedPecSec = 10.f;
	CameraDesc.fRotatePecSec = XMConvertToRadians(90.f);


	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_RUNMAP, TEXT("Prototype_GameObject_PlayerCamera"), strLayerTag, &CameraDesc)))
		return E_FAIL;

	CSystemManager::GetInstance()->Set_Camera(CAMERA_PLAYER);

	return S_OK;
}

HRESULT CLevel_RunMap::Ready_Layer_GameObject(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_RUNMAP, TEXT("Prototype_GameObject_Terrain"), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

CLevel_RunMap* CLevel_RunMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_RunMap* pInstance = new CLevel_RunMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLevel_Test");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_RunMap::Free()
{
	__super::Free();
}
