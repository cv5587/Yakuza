#include "Level_Edit.h"
#include "GameInstance.h"

#pragma region "객체 원형"
#include "FreeCamera.h"
#include "Particle_Point.h"
#pragma endregion 
CLevel_Edit::CLevel_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Edit::Initialize()
{
	if (FAILED(Ready_Object(TEXT("Layer_EditObject"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Edit::Tick(const _float& fTimeDelta)
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("제작 레벨"));
#endif
}

HRESULT CLevel_Edit::Ready_Object(const wstring& strLayerTag)
{

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_EDIT, TEXT("Prototype_GameObject_Sky"),strLayerTag)))
		return E_FAIL;


	LIGHT_DESC			LightDesc = {};	

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(0.f, -1.f, 0.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.7f, 0.7f, 0.7f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	m_pGameInstance->Add_Light(LightDesc);


	CFreeCamera::FREE_CAMERA_DESC CameraDesc{};

	CameraDesc.vEye = _float4(0.0f, 5.f, -20.f, 1.f);
	CameraDesc.vFocus = _float4(0.0f, 0.0f, 0.0f, 1.f);
	CameraDesc.fFovY = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;	
	CameraDesc.fNear = 0.01f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fRotatePecSec = XMConvertToRadians(90.f);
	CameraDesc.fSpeedPecSec = 30.f;
	CameraDesc.fSensor = 0.1f;
	XMStoreFloat4x4(&CameraDesc.vPrePosition, XMMatrixTranslation(0.f, 10.f, 0.f));

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_EDIT, TEXT("Prototype_GameObject_FreeCamera"), strLayerTag, &CameraDesc)))
		return E_FAIL;

	//CParticle_Point::PARTICLE_POINT_DESC PointDesc{};
	//PointDesc.BufferInstance.iNumInstance = 660;
	//PointDesc.BufferInstance.isLoop = true;
	//PointDesc.BufferInstance.vLifeTime = _float2(3.f, 5.f);
	//PointDesc.BufferInstance.vOffsetPos = _float3(0.f, -1.f, 0.f);
	//PointDesc.BufferInstance.vPivotPos = _float3(0.f, 1.f, 0.f);
	//PointDesc.BufferInstance.vRange = _float3(1.f, 0.f, 1.f);
	//PointDesc.BufferInstance.vSize = _float2(0.05f, 0.5f);
	//PointDesc.BufferInstance.vSpeed = _float2(0.01f, 0.02f);
	//PointDesc.BufferInstance.vRectSize = _float2(0.2f, 2.0f);
	//PointDesc.BufferInstance.fRadius = 1.f;	
	//PointDesc.vStartPos = _float4(0.f, 0.f, 0.f, 1.f);
	//PointDesc.eType = CEffect::TYPE::TYPE_POINT;
	//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_EDIT, TEXT("Prototype_GameObject_Particle_Point"), strLayerTag, &PointDesc)))
	//	return E_FAIL;



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
