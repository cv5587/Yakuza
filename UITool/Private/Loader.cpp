#include "Loader.h"

#include "GameInstance.h"


#pragma region GameObject
#include "PlayerCamera.h"
#include "Terrain.h"

#include "Image_Texture.h"
#include "Text.h"
#include "Group.h"
#include "Btn.h"
#include "UI_Effect.h"
#pragma endregion


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{pDevice}, 
	m_pContext{pContext},
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(0, COINIT_MULTITHREADED);

	CLoader* pLoader = (CLoader*)pArg;

	if (FAILED(pLoader->Loading()))
		return 1;

	CoUninitialize();

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_Critical_Section);

	HRESULT			hr{};

	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_For_LogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlayLevel();
		break;
	case LEVEL_RUNMAP:
		hr = Loading_For_RunMapLevel(LEVEL_RUNMAP);
		break;
	}

	LeaveCriticalSection(&m_Critical_Section);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlayLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Static()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_RunMapLevel(int iLevel)
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));

	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));


	_matrix		PreTransformMatrix;

	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("컴포넌트를(을) 로딩 중 입니다."));
	/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_Collider"),
		CCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("오브젝트를(을) 로딩 중 입니다."));

	/* For.Prototype_GameObject_PlayerCamera*/

	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_PlayerCamera"),
		CPlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Image_Texture */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Image_Texture"),
		CImage_Texture::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Text */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Text"),
		CText::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Group */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Group"),
		CGroup::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Btn */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Btn"),
		CBtn::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UIEffect */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_UIEffect"),
		CUI_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed To Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_Critical_Section);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
