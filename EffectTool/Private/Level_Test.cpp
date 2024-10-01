#include "Level_Test.h"
#include "GameInstance.h"

#pragma region "객체 원형"
#include "FreeCamera.h"
#include "Particle_Point.h"
#pragma endregion 
CLevel_Test::CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Test::Initialize()
{
	if (FAILED(Ready_Object(TEXT("Layer_EditObject"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Test::Tick(const _float& fTimeDelta)
{



#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("테스트 레벨"));
#endif
}

HRESULT CLevel_Test::Ready_Object(const wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Prototype_GameObject_Sky"), strLayerTag)))
	//	return E_FAIL;


	LIGHT_DESC			LightDesc = {};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(0.f, -1.f, 0.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.7f, 0.7f, 0.7f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	m_pGameInstance->Add_Light(LightDesc);


	CFreeCamera::FREE_CAMERA_DESC CameraDesc{};

	CameraDesc.vEye = _float4(0.0f, 10.f, -10.f, 1.f);
	CameraDesc.vFocus = _float4(0.0f, 0.0f, 0.0f, 1.f);
	CameraDesc.fFovY = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.01f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fRotatePecSec = XMConvertToRadians(90.f);
	CameraDesc.fSpeedPecSec = 2.f;
	CameraDesc.fSensor = 0.1f;

	
	XMStoreFloat4x4(&CameraDesc.vPrePosition, XMMatrixRotationX(XMConvertToRadians(25.f)) * XMMatrixRotationY(XMConvertToRadians(135.f)) * XMMatrixTranslation(-2.f, 2.f, 2.f));

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Prototype_GameObject_FreeCamera"), strLayerTag, &CameraDesc)))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Prototype_GameObject_Land"), strLayerTag)))
		return E_FAIL;


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
}
