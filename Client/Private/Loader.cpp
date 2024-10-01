#include "Loader.h"

#include "GameInstance.h"

#pragma region Player
#include "Player.h"
#include "RushYakuza.h"

#include "SocketCollider.h"
#include "SocketEffect.h"
#include "QteManager.h"
#pragma endregion

#pragma region Monster
#include "RushYakuza.h"
#include "WPAYakuza.h"
#include "Shakedown.h"
#include "Kuze.h"
#include "Yoneda.h"
#include "WPHYakuza.h"
#include "DefaultYakuza.h"
#pragma endregion

#pragma region Weapon_Common
#include "Weapon_Sofa.h"
#include "Weapon_Knife.h"
#pragma endregion

#pragma region CarChase
#pragma region Player
#include "Highway_Taxi.h"
#include "Highway_Kiryu.h"
#pragma endregion

#pragma region Monster
#include "CarChase_Van.h"
#include "CarChase_Sedan.h"
#include "CarChase_Bike.h"
#include "CarChase_Heli.h"
#pragma endregion

#pragma region Weapon
#include "Weapon_Gun_Cz75.h"
#include "Weapon_MchnGun.h"
#include "Weapon_RcktGun.h"
#include "Weapon_ShotGun.h"
#include "CarChase_CATBullet.h"
#pragma endregion

#pragma region Reactor
#include "Reactor_Van.h"
#include "Reactor_Sedan.h"
#include "Reactor_Bike.h"
#include "Reactor_Heli.h"
#pragma endregion

#pragma region Adventure
#include "RoadNML.h"
#pragma endregion

#pragma region BTNode
#include "AI_RushYakuza.h"
#include "AI_WPAYakuza.h"
#include "AI_Shakedown.h"
#include "AI_Kuze.h"
#include "AI_WPHYakuza.h"
#include "AI_DefaultYakuza.h"
#include "AI_Yoneda.h"

#include "AI_RoadNML.h"

#include "AI_Van.h"
#include "AI_Bike.h"
#include "AI_Sedan.h"
#include "AI_Heli.h"
#pragma endregion

#pragma region Camera
#include "PlayerCamera.h"
#include "DebugCamera.h"
#include "CineCamera.h"
#include "CutSceneCamera.h"
#include "CarChaseCamera.h"
#pragma endregion

#pragma region Map
#include "Terrain.h"
#include "Construction.h"
#include "MapCollider.h"
#include "SkyDome.h"
#include "MapColliderObj.h"
#include "Decal.h"
#include "LightConstruction.h"
#include "Map.h"
#include "Item.h"
#pragma endregion

#pragma region MyRegion
#include "LevelTrigger.h"
#include "MonsterTrigger.h"
#include "QuestTrigger.h"
#include "YonedaTrigger.h"
#pragma endregion


#pragma region Effect
#include "Particle_Point.h"
#include "TRailEffect.h"
#include "Aura.h"
#include "Particle_Mesh.h"
#pragma endregion

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice },
	m_pContext{ pContext },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_Critical_Section);

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
	case LEVEL_OFFICE_1F:
		hr = Loading_For_Office_1F();
		break;
	case LEVEL_OFFICE_2F:
		hr = Loading_For_Office_2F();
		break;
	case LEVEL_OFFICE_BOSS:
		hr = Loading_For_Office_Boss();
		break;
	case LEVEL_DOGIMAZO:
		hr = Loading_For_Dogimazo();
		break;
	case LEVEL_DOGIMAZO_STAIRS:
		hr = Loading_For_Dogimazo_Stairs();
		break;
	case LEVEL_DOGIMAZO_LOBBY:
		hr = Loading_For_Dogimazo_Lobby();
		break;
	case LEVEL_DOGIMAZO_BOSS:
		hr = Loading_For_Dogimazo_Boss();
		break;
	case LEVEL_CARCHASE:
		hr = Loading_For_CarChase();
		break;
	case LEVEL_KARAOKE:
		hr = Loading_For_Karaoke();
		break;
	case LEVEL_TUTORIAL:
		hr = Loading_For_Tutorial();
		break;
	case LEVEL_NISHIKIWALK:
		hr = Loading_For_NishikiWalk();
		break;
	case LEVEL_TOKOSTREET:
		hr = Loading_For_Street();
		break;
	case LEVEL_KARAOKE_START:
		hr = Loading_For_Karaoke();
		break;
	case LEVEL_KARAOKE_END:
		hr = Loading_For_Karaoke();
		break;
	case LEVEL_STREET:
		hr = Loading_For_Street();
		break;
	}

	LeaveCriticalSection(&m_Critical_Section);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

/* 공통적인 저장 객체를 넣어주는 함수. */
HRESULT CLoader::Loading_Default()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_LogoLevel()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_Office_1F()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_Office_2F()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_Office_Boss()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_Dogimazo()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_Dogimazo_Stairs()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_Dogimazo_Lobby()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_Dogimazo_Boss()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_Street()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_CarChase()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_Karaoke()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_Tutorial()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_NishikiWalk()
{
	return S_OK;
}

// 경로를 넣어주면 디렉토리 이름을 읽어와서 bin존재 유무를 파악해 바이너리화 or bin파일 읽기를 자동으로 해준다
// 하지만 해당 함수를 사용하려면 넣어준 경로 안에 폴더명과 똑같은 이름의 fbx를 가지고 있어야 한다.
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
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, strTransPath.c_str(), PreTransformMatrix, false))))
					return E_FAIL;
			}
			else
			{
				PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
				if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, strTransPath.c_str(), PreTransformMatrix, false))))
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
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, file_path.c_str(), PreTransformMatrix, true))))
						return E_FAIL;
				}
				else
				{
					PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
					if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
						CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, file_path.c_str(), PreTransformMatrix, true))))
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
		else if (TEXT("Mesh") == strChannelName)
		{
			for (const auto& entry : fs::directory_iterator(strDirectory))
			{

				string FileName = entry.path().filename().string();
				string AllPath = strDirectory + FileName;

				string Tag;
				_int dotPos = FileName.find_last_of(".");
				Tag = FileName.substr(0, dotPos);


				/* For.Prototype_GameObject_Mesh */
				if (FAILED(m_pGameInstance->Add_GameObject_Prototype(m_pGameInstance->StringToWstring(Tag),
					CParticle_Mesh::Create(m_pDevice, m_pContext, AllPath))))
					return E_FAIL;

			}
		}


	}
	return S_OK;
}

HRESULT CLoader::Add_Models_On_Path_NonAnim(_uint iLevel, const wstring& strPath)
{
	//vector<wstring> vecDirectorys;
	//m_pGameInstance->Get_DirectoryName(strPath, vecDirectorys);

	//for (int i = 0; i < vecDirectorys.size(); i++)
	//{
	//	wstring original = vecDirectorys[i];
	//	wstring to_remove = L"\\";
	//	int pos = original.find(to_remove);

	//	if (pos != wstring::npos) {
	//		original.erase(pos, to_remove.length());
	//	}

	//	vecDirectorys[i] = original;
	//}

	//_matrix		NonAnimPreTransformMatrix;

	//for (auto& strDirlName : vecDirectorys)
	//{
	//	vector<string> fbxFilesName;
	//	wstring		wstrFullPath = strPath + L"/" + strDirlName;

	//	// fbx 제외하고 fbx 파일 이름들 저장
	//	for (const auto& entry : fs::directory_iterator(wstrFullPath)) {
	//		if (entry.is_regular_file() && entry.path().extension() == L".fbx") {

	//			string strFileName = entry.path().filename().string();

	//			size_t lastDot = entry.path().filename().string().find_last_of(".");

	//			strFileName = entry.path().filename().string().substr(0, lastDot);

	//			fbxFilesName.push_back(strFileName);
	//		}
	//	}

	//	for (const auto& fbxNames : fbxFilesName)
	//	{
	//		wstring strFilePath = strPath + TEXT("/") + strDirlName + TEXT("/");

	//		string strDirectory = m_pGameInstance->WstringToString(strFilePath);
	//		string strBinPath = strDirectory + "Bin/" + fbxNames + ".dat";

	//		if (!fs::exists(strBinPath))
	//		{
	//			wstring strFbxName = m_pGameInstance->StringToWstring(fbxNames);
	//			wstring strComponentName = TEXT("Prototype_Component_Model_") + strFbxName;
	//			wstring strFbxPath = strFilePath + strFbxName + TEXT(".fbx");
	//			string strTransPath = m_pGameInstance->WstringToString(strFbxPath);


	//			NonAnimPreTransformMatrix = XMMatrixIdentity();
	//			if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
	//				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, strTransPath.c_str(), NonAnimPreTransformMatrix, false))))
	//				return E_FAIL;
	//		}
	//		else
	//		{
	//			wstring strComponentName = TEXT("Prototype_Component_Model_") + m_pGameInstance->StringToWstring(fbxNames);

	//			NonAnimPreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	//			if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
	//				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, strBinPath.c_str(), NonAnimPreTransformMatrix, true))))
	//				return E_FAIL;
	//		}
	//	}
	//}

	//vecDirectorys.clear();

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
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, strTransPath.c_str(), NonAnimPreTransformMatrix, false))))
				return E_FAIL;
		}
		else
		{
			wstring strComponentName = TEXT("Prototype_Component_Model_") + m_pGameInstance->StringToWstring(fbxNames);

			NonAnimPreTransformMatrix = XMMatrixScaling(0.01004f, 0.01004f, 0.01004f);
			if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, strBinPath.c_str(), NonAnimPreTransformMatrix, true))))
				return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CLoader::Add_Components_On_Path_Material(_uint iLevel, const wstring& strPath)
{
	vector<string> fbxFilesName;
	wstring		wstrFullPath = strPath;
	_matrix		NonAnimPreTransformMatrix;

	// fbx 제외하고 fbx 파일 이름들 저장
	for (const auto& entry : fs::directory_iterator(wstrFullPath)) {
		if (entry.is_regular_file() && entry.path().extension() == L".dat") {

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
		string strBinPath = strDirectory + fbxNames + ".dat";

		wstring strComponentName = TEXT("Prototype_Component_Material_") + m_pGameInstance->StringToWstring(fbxNames);

		if (FAILED(m_pGameInstance->Add_Component_Prototype(iLevel, strComponentName,
			CNeoShader::Create(m_pDevice, m_pContext, strBinPath.c_str()))))
			return E_FAIL;
	}
	return S_OK;
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