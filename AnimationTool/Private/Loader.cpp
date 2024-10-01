#include "Loader.h"

#include "GameInstance.h"

#include "PlayerCamera.h"
#include "AnimModel.h"
#include "ModelBoneSphere.h"

#pragma region Effect
#include "Particle_Point.h"
#include "TRailEffect.h"
#include "Aura.h"
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
	case LEVEL_EDIT:
		hr = Loading_For_EditLevel();
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

HRESULT CLoader::Loading_For_EditLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));

	/* Prototype_Component_Texture_BoneSphere */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_BoneSphere"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Models/NonAnim/Bone_Sphere/Color_%d.png"), 2))))
		return E_FAIL;


#pragma region Effect_Texture
	/* Prototype_Component_Texture_Sphere */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Sphere"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/lm000.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Trail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Trail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/001_trc_n.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5))))
		return E_FAIL;

	/* Prototype_Component_Texture_Test */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Test"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/9923j.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_HitSpark */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_HitSpark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/015_trc_na.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_AuraAnim */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_AuraAnim"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0076_aura_g.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_AuraFluid */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_AuraFluid"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0309_fluidFlowA01.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_AuraBase */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_AuraBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0291_smoke_jA8L8.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_AuraTone */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_AuraTone"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/auratone24.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_HitBase */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_HitBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0233_radialpat_d.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_SmokeBase */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_SmokeBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_o_2208_smoke_j.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_AuraToneRush */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_AuraToneRush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/auratone22.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_AuraToneDestroy */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_AuraToneDestroy"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/auratone17.dds"), 1))))
		return E_FAIL;
	
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("컴포넌트 원형 를(을) 로딩 중 입니다."));
#pragma region Effect

	/* For.Prototype_Component_VIBuffer_Instance_Point */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_VIBuffer_Instance_Point"),
		CVIBuffer_Instance_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Trail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	Add_Models_On_Path(TEXT("../../Client/Bin/Resources/Models/Anim/Player/"));
	Add_Models_On_Path(TEXT("../../Client/Bin/Resources/Models/Anim/Monster/"));
	Add_Models_On_Path(TEXT("../../Client/Bin/Resources/Models/Anim/NPC/"));
	Add_Models_On_Path(TEXT("../../Client/Bin/Resources/Models/NonAnim/"), false);
	Add_Animation_Component_On_Path(TEXT("../../Client/Bin/Resources/Models/NonAnim/"));		// 패스 노상관

#pragma region Effect
	Add_GameObject_Particle_On_Path(TEXT("../../Client/Bin/DataFiles/Particle/"));
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));

	// Prototype_Component_Shader_VtxMesh
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	// Prototype_Component_Shader_VtxAnimMesh
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;


#pragma region Effect
	/* For.Prototype_Component_Shader_Trail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Shader_Trail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Trail.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxInstance_Point */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Point.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_Aura*/
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Shader_Aura"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Aura.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;
#pragma endregion


	lstrcpy(m_szLoadingText, TEXT("충돌체 원형을 로딩 중 입니다."));
	/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Collider"), CCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("객체 원형을(를) 로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Free_Camera"), CPlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_AnimModel"), CAnimModel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_BoneSphere"), CModelBoneSphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Add_Models_On_Path(const wstring& strPath, _bool bAnim)
{
	vector<wstring> vecDirectorys;
	m_pGameInstance->Get_DirectoryName(strPath, vecDirectorys);

	_matrix		PreTransformMatrix;

	for (auto& strDirlName : vecDirectorys)
	{
		//string pBinPath = pGameInstance->Get_Directory(pModelFilePath) + "/Bin/" + pGameInstance->Get_FileName(pModelFilePath) + ".dat";

		//wstring strComponentName = TEXT("Prototype_Component_") + strName;
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
				if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, strComponentName,
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, strTransPath.c_str(), PreTransformMatrix, false, true))))
					return E_FAIL;
			}
			else
			{
				PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
				if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, strComponentName,
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
					if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, strComponentName,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, file_path.c_str(), PreTransformMatrix, true, true))))
						return E_FAIL;
				}
				else
				{
					PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
					if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, strComponentName,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, file_path.c_str(), PreTransformMatrix, true, true))))
						return E_FAIL;
				}

			}
		}

	}

	return S_OK;
}

HRESULT CLoader::Add_GameObject_Particle_On_Path(const wstring& strPath)
{
	vector<wstring> vecDirectorys;
	m_pGameInstance->Get_DirectoryName(strPath, vecDirectorys);

	for (auto& strChannelName : vecDirectorys)
	{
		wstring strFilePath = strPath + strChannelName + TEXT("/");
		string strDirectory = m_pGameInstance->WstringToString(strFilePath);

		if (TEXT("Point") == strChannelName)
		{
			for (const auto& entry : fs::directory_iterator(strDirectory))
			{

				string FileName = entry.path().filename().string();
				string AllPath = strDirectory + FileName;

				string Tag;
				_int dotPos = FileName.find_last_of(".");
				Tag = FileName.substr(0, dotPos);


				/* For.Prototype_GameObject_Particle_Point */
				if (FAILED(m_pGameInstance->Add_GameObject_Prototype(m_pGameInstance->StringToWstring(Tag),
					CParticle_Point::Create(m_pDevice, m_pContext, AllPath))))
					return E_FAIL;

			}
		}
		else if (TEXT("Trail") == strChannelName)
		{
			for (const auto& entry : fs::directory_iterator(strDirectory))
			{

				string FileName = entry.path().filename().string();
				string AllPath = strDirectory + FileName;

				string Tag;
				_int dotPos = FileName.find_last_of(".");
				Tag = FileName.substr(0, dotPos);


				/* For.Prototype_GameObject_Particle_Trail */
				if (FAILED(m_pGameInstance->Add_GameObject_Prototype(m_pGameInstance->StringToWstring(Tag),
					CTRailEffect::Create(m_pDevice, m_pContext, AllPath))))
					return E_FAIL;

			}
		}
		else if (TEXT("Aura") == strChannelName)
		{
			for (const auto& entry : fs::directory_iterator(strDirectory))
			{

				string FileName = entry.path().filename().string();
				string AllPath = strDirectory + FileName;

				string Tag;
				_int dotPos = FileName.find_last_of(".");
				Tag = FileName.substr(0, dotPos);


				/* For.Prototype_GameObject_Particle_Aura */
				if (FAILED(m_pGameInstance->Add_GameObject_Prototype(m_pGameInstance->StringToWstring(Tag),
					CAura::Create(m_pDevice, m_pContext, AllPath))))
					return E_FAIL;

			}
		}


	}
	return S_OK;
}

HRESULT CLoader::Add_Animation_Component_On_Path(const wstring& strPath)
{
	/* For.Prototype_Component_Anim*/
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Anim"), CAnim::Create(m_pDevice, m_pContext, "../../Client/Bin/DataFiles/AnimationData/Animation.dat", false))))
		return E_FAIL;
	/*if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation.fbx", true))))
		return E_FAIL;*/

	/* For.Prototype_Component_Anim_Common */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Anim_Common"), CAnim::Create(m_pDevice, m_pContext, "../../Client/Bin/DataFiles/AnimationData/Animation.dat", false))))
		return E_FAIL;
	/*if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation.fbx", true))))
		return E_FAIL;*/

	/* For.Prototype_Component_Anim_SyncAnim */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Anim_SyncAnim"), CAnim::Create(m_pDevice, m_pContext, "../../Client/Bin/DataFiles/AnimationData/Animation_Sync.dat", false))))
		return E_FAIL;
	/*if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_SyncAnim"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Sync.fbx", true))))
		return E_FAIL;*/

	///* For.Prototype_Component_Anim_CutSceneAnim_ForPlayer */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Anim_CutSceneAnim_ForPlayer"), CAnim::Create(m_pDevice, m_pContext, "../../Client/Bin/DataFiles/AnimationData/Animation_CutScene_ForPlayer.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_CutSceneAnim_ForPlayer"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_CutScene_ForPlayer.fbx", true))))
	//	return E_FAIL;

	///* For.Prototype_Component_Anim_Kiryu */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Anim_Kiryu"), CAnim::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/Models/Anim/Kiryu/Animation_Kiryu.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Kiryu"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Kiryu.fbx", true))))
	//	return E_FAIL;

	///* For.Prototype_Component_Anim_KiryuFace */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Anim_KiryuFace"), CAnim::Create(m_pDevice, m_pContext, "../../Client/Bin/DataFiles/AnimationData/Animation_Kiryu_Face.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_KiryuFace"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Kiryu_Face.fbx", true))))
	//	return E_FAIL;

	///* For.Prototype_Component_Anim_Hand */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Anim_Hand"), CAnim::Create(m_pDevice, m_pContext, "../../Client/Bin/DataFiles/AnimationData/Animation_Hand.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Hand"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Hand.fbx", true))))
	//	return E_FAIL;

	///* For.Prototype_Component_Kiryu_CarChase */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Kiryu_CarChase"), CAnim::Create(m_pDevice, m_pContext, "../../Client/Bin/DataFiles/AnimationData/Animation_Kiryu_CarChase.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Kiryu_CarChase"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Kiryu_CarChase.fbx", true))))
	//	return E_FAIL;

	/* For.Prototype_Component_Anim_NPC */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Anim_NPC"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_NPC.dat", false))))
		return E_FAIL;
	/*if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_NPC"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_NPC.fbx", true))))
		return E_FAIL;*/

	/* For.Prototype_Component_Anim_ReactorHighwayAnim */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Anim_ReactorHighwayAnim"), CAnim::Create(m_pDevice, m_pContext, "../../Client/Bin/DataFiles/AnimationData/Animation_Reactor_Highway.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_ReactorHighwayAnim"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Reactor_Highway.fbx", true))))
	//	return E_FAIL;

	/* For.Prototype_Component_CarChaseAnim */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_CarChaseAnim"), CAnim::Create(m_pDevice, m_pContext, "../../Client/Bin/DataFiles/AnimationData/Animation_CarChase.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_CarChaseAnim"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_CarChase.fbx", true))))
	//	return E_FAIL;

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
