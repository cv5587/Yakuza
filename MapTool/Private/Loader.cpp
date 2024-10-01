#include "Loader.h"

#include "GameInstance.h"


#pragma region GameObject
#include "PlayerCamera.h"
#include "Terrain.h"
#include "Construction.h"
#include "LightObj.h"
#include "CameraObj.h"
#include "CameraInstallObj.h"
#include "CineCamera.h"
#include "Decal.h"
#include "ColliderObj.h"
#include "RushYakuza.h"
#include "TriggerObj.h"
#include "NaviObj.h"
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

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/T_Sand_06_A.png"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));

	Add_Models_On_Path(iLevel, TEXT("../../Client/Bin/Resources/Models/Anim/Car"), true);
	Add_Models_On_Path(iLevel, TEXT("../../Client/Bin/Resources/Models/Anim/Monster"), true);
	Add_Models_On_Path(iLevel, TEXT("../../Client/Bin/Resources/Models/Anim/Player/"), true);
	Add_Models_On_Path(iLevel, TEXT("../../Client/Bin/Resources/Models/Anim/NPC"), true);

	Add_Models_On_Path_NonAnim(iLevel, TEXT("../../Client/Bin/Resources/Models/NonAnim/Map/Map0"));

	Add_Models_On_Path_NonAnim(iLevel, TEXT("../../Client/Bin/Resources/Models/NonAnim/Map/Map1"));

	Add_Models_On_Path_NonAnim(iLevel, TEXT("../../Client/Bin/Resources/Models/NonAnim/Map/Map2"));

	Add_Models_On_Path_NonAnim(iLevel, TEXT("../../Client/Bin/Resources/Models/NonAnim/Map/Map3"));

	Add_Models_On_Path_NonAnim(iLevel, TEXT("../../Client/Bin/Resources/Models/NonAnim/Gun_Cz75"));

	//Add_Models_On_Path_NonAnim(iLevel, TEXT("../../Client/Bin/Resources/Models/NonAnim/Bone_Sphere"));
	Add_Models_On_Path_NonAnim(iLevel, TEXT("../../Client/Bin/Resources/Models/NonAnim/Map/KaraokeMap"));

	/* For.Prototype_Component_VIBuffer_Terrain */

	if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_VIBuffer_Terrain_Flat"),
		CVIBuffer_Terrain_Flat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_Model_Bone_Sphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Models/NonAnim/Bone_Sphere/Bin/Square.dat", PreTransformMatrix, true, true))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_Model_Sphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Models/NonAnim/Bone_Sphere/Bin/Bone_Sphere.dat", PreTransformMatrix, true, true))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_Model_Bone_Sphere"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Models/NonAnim/Bone_Sphere/Square.fbx", PreTransformMatrix, false, true))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_Model_Sphere"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Models/NonAnim/Bone_Sphere/Bone_Sphere.fbx", PreTransformMatrix, false, true))))
	//	return E_FAIL;

	//PreTransformMatrix = XMMatrixIdentity();
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_Model_soto"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Models/NonAnim/Map/Map1/Bin/soto01.dat", PreTransformMatrix, true, true))))
	//	return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_Shader_VtxAnim"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh_CamObj */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_Shader_VtxMesh_CamObj"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh_CamObj.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("충돌체 원형을 로딩 중 입니다."));
	/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, TEXT("Prototype_Component_Collider"), CCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	lstrcpy(m_szLoadingText, TEXT("오브젝트를(을) 로딩 중 입니다."));

	/* For.Prototype_GameObject_PlayerCamera*/

	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_PlayerCamera"),
		CPlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Construction"),
		CConstruction::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LightObj */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_LightObj"),
		CLightObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CameraObj */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_CameraObj"),
		CCameraObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CameraInstallObj */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_CameraInstallObj"),
		CCameraInstallObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CCineCamera */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_CCineCamera"),
		CCineCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Decal */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Decal"),
		CDecal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ColliderObj */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_ColliderObj"),
		CColliderObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_TriggerObj */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_TriggerObj"),
		CTriggerObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_NaviObj */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_NaviObj"),
		CNaviObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Add_Models_On_Path(_uint iLevel, const wstring& strPath, _bool bAnim)
{
	vector<wstring> vecDirectorys;
	m_pGameInstance->Get_DirectoryName(strPath, vecDirectorys);

	_matrix		PreTransformMatrix;

	for (auto& strDirlName : vecDirectorys)
	{
		wstring strFilePath = strPath + strDirlName + TEXT("/");

		string strDirectory = m_pGameInstance->WstringToString(strFilePath);
		string strBinPath = strDirectory + "Bin/";

		if (!fs::exists(strBinPath))
		{
			wstring strComponentName = TEXT("Prototype_Component_Model_") + strDirlName;
			wstring strFbxPath = strFilePath + strDirlName + TEXT(".fbx");
			string strTransPath = m_pGameInstance->WstringToString(strFbxPath);

			if (!bAnim)
			{
				PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
				if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, strTransPath.c_str(), PreTransformMatrix, false, true))))
					return E_FAIL;
			}
			else
			{
				PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
				if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, strTransPath.c_str(), PreTransformMatrix, false, true))))
					return E_FAIL;
			}
		}
		else
		{
			for (const auto& entry : std::filesystem::directory_iterator(strBinPath))
			{
				string file_path = entry.path().string();
				string strFileName = m_pGameInstance->Get_FileName(file_path);
				wstring strComponentName = TEXT("Prototype_Component_Model_") + m_pGameInstance->StringToWstring(strFileName);

				if (!bAnim)
				{
					PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
					if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, file_path.c_str(), PreTransformMatrix, true, true))))
						return E_FAIL;
				}
				else
				{
					PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
					if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, file_path.c_str(), PreTransformMatrix, true,true))))
						return E_FAIL;
				}

			}
		}

	}

	return S_OK;
}

//HRESULT CLoader::Add_Models_On_Path_NonAnim(_uint iLevel, const wstring& strPath)
//{
//	vector<wstring> vecDirectorys;
//	m_pGameInstance->Get_DirectoryName(strPath, vecDirectorys);
//
//	for (int i = 0; i < vecDirectorys.size(); i++)
//	{
//		wstring original = vecDirectorys[i];
//		wstring to_remove = L"\\";
//		int pos = original.find(to_remove);
//
//		if (pos != wstring::npos) {
//			original.erase(pos, to_remove.length());
//		}
//
//		vecDirectorys[i] = original;
//	}
//
//	_matrix		NonAnimPreTransformMatrix;
//
//	for (auto& strDirlName : vecDirectorys)
//	{
//
//		vector<string> fbxFilesName;
//		wstring		wstrFullPath = strPath + L"/" + strDirlName;
//
//		// fbx 제외하고 fbx 파일 이름들 저장
//		for (const auto& entry : fs::directory_iterator(wstrFullPath)) {
//			if (entry.is_regular_file() && entry.path().extension() == L".fbx") {
//
//				string strFileName = entry.path().filename().string();
//
//				size_t lastDot = entry.path().filename().string().find_last_of(".");
//				
//				strFileName = entry.path().filename().string().substr(0, lastDot);
//
//				fbxFilesName.push_back(strFileName);
//			}
//		}
//
//		for (const auto& fbxNames : fbxFilesName)
//		{
//			wstring strFilePath = strPath + TEXT("/") + strDirlName + TEXT("/");
//
//			string strDirectory = m_pGameInstance->WstringToString(strFilePath);
//			string strBinPath = strDirectory + "Bin/" + fbxNames +".dat";
//
//			if (!fs::exists(strBinPath))
//			{
//				wstring strFbxName = m_pGameInstance->StringToWstring(fbxNames);
//				wstring strComponentName = TEXT("Prototype_Component_Model_") + strFbxName;
//				wstring strFbxPath = strFilePath + strFbxName + TEXT(".fbx");
//				string strTransPath = m_pGameInstance->WstringToString(strFbxPath);
//
//				
//				NonAnimPreTransformMatrix = XMMatrixIdentity();
//				if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
//					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, strTransPath.c_str(), NonAnimPreTransformMatrix, false, true))))
//					return E_FAIL;
//				
//			}
//			else
//			{
//
//				wstring strComponentName = TEXT("Prototype_Component_Model_") + m_pGameInstance->StringToWstring(fbxNames);
//
//				NonAnimPreTransformMatrix = XMMatrixScaling(0.01004f, 0.01004f, 0.01004f);
//				if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
//					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, strBinPath.c_str(), NonAnimPreTransformMatrix, true, true))))
//					return E_FAIL;
//					
//
//				
//			}
//		}
//
//	}
//
//	vecDirectorys.clear();
//
//	return E_NOTIMPL;
//}

HRESULT CLoader::Add_Models_On_Path_NonAnim(_uint iLevel, const wstring& strPath)
{
	vector<string> fbxFilesName;
	wstring		wstrFullPath = strPath;
	_matrix		NonAnimPreTransformMatrix;

	// fbx 제외하고 fbx 파일 이름들 저장
	for (const auto& entry : fs::directory_iterator(wstrFullPath)) {
		if (entry.is_regular_file() && entry.path().extension() == L".fbx") {

			string strFileName = entry.path().filename().string();

			size_t lastDot = entry.path().filename().string().find_last_of(".");

			strFileName = entry.path().filename().string().substr(0, lastDot);

			fbxFilesName.push_back(strFileName);
		}
	}

	for (const auto& fbxNames : fbxFilesName)
	{
		wstring strFilePath = strPath + TEXT("/");

		string strDirectory = m_pGameInstance->WstringToString(strFilePath);
		string strBinPath = strDirectory + "Bin/" + fbxNames + ".dat";

		if (!fs::exists(strBinPath))
		{
			wstring strFbxName = m_pGameInstance->StringToWstring(fbxNames);
			wstring strComponentName = TEXT("Prototype_Component_Model_") + strFbxName;
			wstring strFbxPath = strFilePath + strFbxName + TEXT(".fbx");
			string strTransPath = m_pGameInstance->WstringToString(strFbxPath);


			NonAnimPreTransformMatrix = XMMatrixIdentity();
			if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, strTransPath.c_str(), NonAnimPreTransformMatrix, false, true))))
				return E_FAIL;
		}
		else
		{
			wstring strComponentName = TEXT("Prototype_Component_Model_") + m_pGameInstance->StringToWstring(fbxNames);

			NonAnimPreTransformMatrix = XMMatrixScaling(0.01004f, 0.01004f, 0.01004f);
			if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, strBinPath.c_str(), NonAnimPreTransformMatrix, true, true))))
				return E_FAIL;
		}
	}
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
