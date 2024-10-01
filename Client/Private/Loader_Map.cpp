#include "Loader_Map.h"

#include "GameInstance.h"

#pragma region Weapon_Common
#include "Weapon_Sofa.h"
#include "Weapon_Knife.h"
#pragma endregion

#pragma region Weapon
#include "Weapon_Gun_Cz75.h"
#include "Weapon_MchnGun.h"
#include "Weapon_RcktGun.h"
#include "Weapon_ShotGun.h"
#include "CarChase_CATBullet.h"

#include "Adventure_Reactor.h"

#include "Karaoke_Glass.h"
#include "Karaoke_Mic.h"
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

CLoader_Map::CLoader_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLoader{ pDevice, pContext }
{
}

_uint APIENTRY LoadingMap(void* pArg)
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	CLoader_Map* pLoader = (CLoader_Map*)pArg;

	if (FAILED(pLoader->Loading()))
		return 1;

	CoUninitialize();

	return 0;
}

HRESULT CLoader_Map::Initialize(LEVEL eNextLevel)
{
	if (FAILED(__super::Initialize(eNextLevel)))
		return E_FAIL;

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMap, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader_Map::Loading()
{
	if (FAILED(__super::Loading()))
		return E_FAIL;

	return S_OK;
}

/* 공통적인 저장 객체를 넣어주는 함수. */
HRESULT CLoader_Map::Loading_Default()
{
#pragma region Texture
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/T_Sand_06_A.png"), 1))))
		return E_FAIL;
#pragma endregion

#pragma region Component
	lstrcpy(m_szLoadingText, TEXT("컴포넌트 원형 를(을) 로딩 중 입니다."));
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_Terrain_Flat"), CVIBuffer_Terrain_Flat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Trail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_AABBCube */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_AABBCube"), CVIBuffer_AABBCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Bone_Sphere"));

	return S_OK;
}

HRESULT CLoader_Map::Loading_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("맵의 객체원형을 로딩 중 입니다."));

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Construction */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Construction"),
		CConstruction::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Decal */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Decal"),
		CDecal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapCollider */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_MapCollider"),
		CMapCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SkyDome */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_SkyDome"),
		CSkyDome::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapColliderObj */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_MapColliderObj"),
		CMapColliderObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LightConstruction */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_LightConstruction"),
		CLightConstruction::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LevelTrigger */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_LevelTrigger"),
		CLevelTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MonsterTrigger */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_MonsterTrigger"),
		CMonsterTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_QuestTrigger */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_QuestTrigger"),
		CQuestTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Map */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Map"),
		CMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Item */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Item"),
		CItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_YonedaTrigger */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_YonedaTrigger"),
		CYonedaTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;


#pragma region GameObject
	/* For.Prototype_GameObject_Gun_Cz75*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Gun_Cz75"),
		CGun_Cz75::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ShotGun*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_ShotGun"),
		CShotGun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MchnGun*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_MchnGun"),
		CMchnGun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_RcktGun*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_RcktGun"),
		CRcktGun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_RcktGunBullet*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_RcktGunBullet"),
		CCarChase_CATBullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Karaoke_Mic*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Karaoke_Mic"),
		CKaraoke_Mic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Karaoke_Glass*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Karaoke_Glass"),
		CKaraoke_Glass::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("객체 원형 를(을) 로딩 중 입니다."));
	/* For.Prototype_GameObject_Sofa*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Sofa"),
		CSofa::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

	/* For.Prototype_GameObject_AdventureReactor*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_AdventureReactor"),
		CAdventure_Reactor::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Map::Loading_For_Office_1F()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_0.dat")))))
		return E_FAIL;

#pragma region Model
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Map/Map0"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Map/Map0"));
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Map::Loading_For_Office_2F()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_1.dat")))))
		return E_FAIL;

#pragma region Model
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Map/Map0"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Map/Map0"));
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Map::Loading_For_Office_Boss()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_2.dat")))))
		return E_FAIL;

#pragma region Model
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));

	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Map/Map0"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Map/Map0"));
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader_Map::Loading_For_Dogimazo()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_3.dat")))))
		return E_FAIL;

#pragma region Model
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Map/Map1"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Map/Map1"));
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Map::Loading_For_Dogimazo_Stairs()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_4.dat")))))
		return E_FAIL;

#pragma region Model
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Map/Map1"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Map/Map1"));
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader_Map::Loading_For_Dogimazo_Lobby()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_5.dat")))))
		return E_FAIL;

#pragma region Model
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Map/Map1"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Map/Map1"));
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Map::Loading_For_Dogimazo_Boss()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_6.dat")))))
		return E_FAIL;

#pragma region Model
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Map/Map1"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Map/Map1"));
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Map::Loading_For_Street()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_7.dat")))))
		return E_FAIL;

#pragma region Model
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Map/Map2"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Map/Map2"));

	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Reactor/Moving_Sedan"));
	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Reactor/Moving_Sedan_2"));
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Map::Loading_For_CarChase()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_CARCHASE, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_8.dat")))))
		return E_FAIL;

#pragma region Model
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	Add_Models_On_Path_NonAnim(LEVEL_CARCHASE, TEXT("../Bin/Resources/Models/NonAnim/Map/Map3"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Map/Map3"));

	Add_Models_On_Path_NonAnim(LEVEL_CARCHASE, TEXT("../Bin/Resources/Models/NonAnim/Weapon/Gun_Cz75"));
	Add_Models_On_Path_NonAnim(LEVEL_CARCHASE, TEXT("../Bin/Resources/Models/NonAnim/Weapon/Knife"));
	Add_Models_On_Path_NonAnim(LEVEL_CARCHASE, TEXT("../Bin/Resources/Models/NonAnim/Weapon/MchnGun"));
	Add_Models_On_Path_NonAnim(LEVEL_CARCHASE, TEXT("../Bin/Resources/Models/NonAnim/Weapon/RcktGun"));
	Add_Models_On_Path_NonAnim(LEVEL_CARCHASE, TEXT("../Bin/Resources/Models/NonAnim/Weapon/RcktGunBullet"));
	Add_Models_On_Path_NonAnim(LEVEL_CARCHASE, TEXT("../Bin/Resources/Models/NonAnim/Weapon/ShotGun"));
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Map::Loading_For_Karaoke()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_9.dat")))))
		return E_FAIL;

	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Map/KaraokeMap"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Map/KaraokeMap"));

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Map::Loading_For_Tutorial()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_7.dat")))))
		return E_FAIL;

	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Map/Map2"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Map/Map2"));

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Map::Loading_For_NishikiWalk()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviData/Navigation_7.dat")))))
		return E_FAIL;

	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Map/Map2"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Map/Map2"));

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

CLoader_Map* CLoader_Map::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLoader_Map* pInstance = new CLoader_Map(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed To Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader_Map::Free()
{
	__super::Free();
}