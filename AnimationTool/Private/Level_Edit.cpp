#include "Level_Edit.h"
#include "GameInstance.h"
#include "Camera.h"

CLevel_Edit::CLevel_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Edit::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	
	if (FAILED(Ready_Object(TEXT("Layer_Object"))))
		return E_FAIL;

	//if (FAILED(Ready_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;


	return S_OK;
}

void CLevel_Edit::Tick(const _float& fTimeDelta)
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("제작 레벨"));
#endif
}

HRESULT CLevel_Edit::Ready_Lights()
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

HRESULT CLevel_Edit::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera::tCameraDesc		CameraDesc{};

	CameraDesc.vEye = _float4(0.5f, 0.8f, -2.f, 1.f);
	CameraDesc.vFocus = _float4(0.5f, 0.8f, 0, 1.f);
	CameraDesc.fFovY = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPecSec = 1.f;
	CameraDesc.fRotatePecSec = XMConvertToRadians(90.f);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_EDIT, TEXT("Prototype_GameObject_Free_Camera"), strLayerTag, &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Edit::Ready_Object(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_EDIT, TEXT("Prototype_GameObject_AnimModel"), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Edit::Ready_Effect(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_EDIT, TEXT("Prototype_GameObject_Particle_Point_Hit1_Part0"), strLayerTag, nullptr)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_EDIT, TEXT("Prototype_GameObject_Particle_Aura_asd"), strLayerTag, nullptr)))
		return E_FAIL;

	return S_OK;
}

CLevel_Edit* CLevel_Edit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Edit* pInstance = new CLevel_Edit(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLevel_Edit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Edit::Free()
{
	__super::Free();
}
