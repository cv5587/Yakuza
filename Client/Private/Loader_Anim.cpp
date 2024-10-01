#include "Loader_Anim.h"

#include "GameInstance.h"

#pragma region Player
#include "Player.h"
#include "SocketCollider.h"
#include "SocketEffect.h"
#include "QteManager.h"

#include "Highway_Taxi.h"
#include "Highway_Kiryu.h"
#pragma endregion

#pragma region Monster
#include "RushYakuza.h"
#include "WPAYakuza.h"
#include "Shakedown.h"
#include "Kuze.h"
#include "Yoneda.h"
#include "WPHYakuza.h"
#include "DefaultYakuza.h"

#include "CarChase_Van.h"
#include "CarChase_Sedan.h"
#include "CarChase_Bike.h"
#include "CarChase_Heli.h"

#include "MonsterGroup.h"
#pragma endregion

#pragma region NPC
#include "Nishiki.h"
#pragma endregion


#pragma region Reactor
#include "Reactor_Van.h"
#include "Reactor_Sedan.h"
#include "Reactor_Bike.h"
#include "Reactor_Heli.h"
#pragma endregion

#pragma region Adventure
#include "RoadNML.h"
#include "RoadYOP.h"

#include "RoadCAB.h"
#include "RoadTissue.h"
#include "RoadStanding_NML.h"
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
#include "AI_RoadCAB.h"
#include "AI_RoadTissue.h"
#include "AI_RoadStanding_NML.h"
#include "AI_RoadYOP.h"
#include "RoadYOP.h"

#include "AI_Van.h"
#include "AI_Bike.h"
#include "AI_Sedan.h"
#include "AI_Heli.h"
#pragma endregion

#pragma region Karaoke
#include "Karaoke_Kiryu.h"
#pragma endregion

CLoader_Anim::CLoader_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLoader{ pDevice, pContext }
{
}

_uint APIENTRY LoadingAnim(void* pArg)
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	CLoader_Anim* pLoader = (CLoader_Anim*)pArg;

	if (FAILED(pLoader->Loading()))
		return 1;

	CoUninitialize();

	return 0;
}

HRESULT CLoader_Anim::Initialize(LEVEL eNextLevel)
{
	if (FAILED(__super::Initialize(eNextLevel)))
		return E_FAIL;

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingAnim, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader_Anim::Loading()
{
	if (FAILED(__super::Loading()))
		return E_FAIL;

	return S_OK;
}

/* 공통적인 저장 객체를 넣어주는 함수. */
HRESULT CLoader_Anim::Loading_Default()
{
#pragma region Model
	Add_Models_On_Path(m_eNextLevel, TEXT("../Bin/Resources/Models/Anim/Player/"));
	Add_Models_On_Path(m_eNextLevel, TEXT("../Bin/Resources/Models/Anim/Monster/"));
	Add_Models_On_Path(m_eNextLevel, TEXT("../Bin/Resources/Models/Anim/NPC/"));
#pragma endregion

#pragma region Meterial
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Char/Player"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Char/Monster")); 
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Reactor"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Char/NPC"));
#pragma endregion

#pragma region BTNode
	lstrcpy(m_szLoadingText, TEXT("행동트리 원형 를(을) 로딩 중 입니다."));
	/* For.Prototype_BTNode_RushYakuza*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_RushYakuza"),
		CAI_RushYakuza::Create())))
		return E_FAIL;

	/* For.Prototype_BTNode_WPAYakuza*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_WPAYakuza"),
		CAI_WPAYakuza::Create())))
		return E_FAIL;

	/* For.Prototype_BTNode_Shakedown*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_Shakedown"),
		CAI_Shakedown::Create())))
		return E_FAIL;

	/* For.Prototype_BTNode_Kuze*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_Kuze"),
		CAI_Kuze::Create())))
		return E_FAIL;

	/* For.Prototype_BTNode_WPHYakuza*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_WPHYakuza"),
		CAI_WPHYakuza::Create())))
		return E_FAIL;

	/* For.Prototype_BTNode_DefaultYakuza*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_DefaultYakuza"),
		CAI_DefaultYakuza::Create())))
		return E_FAIL;

	/* For.Prototype_BTNode_Yoneda*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_Yoneda"),
		CAI_Yoneda::Create())))
		return E_FAIL;

	/* For.Prototype_BTNode_RoadNML*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_RoadNML"),
		CAI_RoadNML::Create())))
		return E_FAIL;

	/* For.Prototype_BTNode_RoadYOP*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_RoadYOP"),
		CAI_RoadYOP::Create())))
		return E_FAIL;

	/* For.Prototype_BTNode_RoadStanding_NML*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_RoadStanding_NML"),
		CAI_RoadStanding_NML::Create())))
		return E_FAIL;

	/* For.Prototype_BTNode_RoadCAB*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_RoadCAB"),
		CAI_RoadCAB::Create())))
		return E_FAIL;

	/* For.Prototype_BTNode_RoadTissue*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_RoadTissue"),
		CAI_RoadTissue::Create())))
		return E_FAIL;

#pragma endregion

#pragma region Component
	lstrcpy(m_szLoadingText, TEXT("컴포넌트 원형 를(을) 로딩 중 입니다."));
	/* For.Prototype_Component_Anim */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation.dat", false))))
		return E_FAIL;
	/*if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation.fbx", true))))
		return E_FAIL;*/
	 
		/* For.Prototype_Component_SyncAnim */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_SyncAnim"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_Sync.dat", false))))
		return E_FAIL;
	/*if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_SyncAnim"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Sync.fbx", true))))
		return E_FAIL;*/

		/* For.Prototype_Component_Anim_NPC */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_NPC"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Monster/Jimu/Animation_NPC.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_NPC"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Monster/Animation_NPC.fbx", true))))
	//	return E_FAIL;

		/* For.Prototype_Component_AStart */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_AStart"),
		CAStart::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_Component_CutSceneAnim_ForPlayer */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_CutSceneAnim_ForPlayer"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_CutScene_ForPlayer.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_CutSceneAnim_ForPlayer"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Kiryu_CamAction.fbx", true))))
	//	return E_FAIL;

	/* For.Prototype_Component_Anim_Kiryu */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Kiryu"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Player/Kiryu/Animation_Kiryu.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Kiryu"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Kiryu.fbx", true))))
	//	return E_FAIL;

	///* For.Prototype_Component_Anim_KiryuFace */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_KiryuFace"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_Kiryu_Face.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_KiryuFace"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Kiryu_Face.fbx", true))))
	//	return E_FAIL;

	///* For.Prototype_Component_Anim_Hand */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Hand"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_Hand.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Hand"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Hand.fbx", true))))
	//	return E_FAIL;

	///* For.Prototype_Component_Kiryu_CarChase */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Kiryu_CarChase"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_Kiryu_CarChase.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Kiryu_CarChase"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Kiryu_CarChase.fbx", true))))
	//	return E_FAIL;

	///* For.Prototype_Component_Anim_KuzeFace */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_KuzeFace"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_Kuze_Face.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_KuzeFace"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Kuze_Face.fbx", true))))
	//	return E_FAIL;

	///* For.Prototype_Component_Anim_YonedaFace */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_YonedaFace"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_Yoneda_Face.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_YonedaFace"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Yoneda_Face.fbx", true))))
	//	return E_FAIL;


#pragma endregion

	return S_OK;
}

HRESULT CLoader_Anim::Loading_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("플레이어 원형객체를 로딩 중 입니다."));
	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// TODO: 테스트용으로 넣은거라 테스트 끝나면 제거필요
	/* For.Prototype_GameObject_Kiryu_Karaoke */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Kiryu_Karaoke"), CKaraoke_Kiryu::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SoketCollider */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_SoketCollider"),
		CSocketCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SoketEffect */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_SoketEffect"),
		CSocketEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region Etc
	/* For.Prototype_GameObject_QTEManager*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_QTEManager"),
		CQteManager::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Monster
	/* For.Prototype_GameObject_RushYakuza */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_RushYakuza"),
		CRushYakuza::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_WPAYakuza */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_WPAYakuza"),
		CWPAYakuza::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Shakedown */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Shakedown"),
		CShakedown::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Kuze */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Kuze"),
		CKuze::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Yoneda */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Yoneda"),
		CYoneda::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_WPH */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_WPH"),
		CWPHYakuza::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Default */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Default"),
		CDefaultYakuza::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region GameObject
	lstrcpy(m_szLoadingText, TEXT("객체원형를(을) 로딩 중 입니다."));

	/* For.Prototype_GameObject_Taxi (For.Player) */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Taxi"),
		CHighway_Taxi::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CarChase_Kiryu (For.Player) */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_CarChase_Kiryu"),
		CHighway_Kiryu::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ReactorVan */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_ReactorVan"),
		CReactor_Van::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ReactorSedan */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_ReactorSedan"),
		CReactor_Sedan::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ReactorBike */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_ReactorBike"),
		CReactor_Bike::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ReactorHeli */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_ReactorHeli"),
		CReactor_Heli::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CarChaseVan*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_CarChaseVan"),
		CCarChase_Van::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CarChaseSedan*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_CarChaseSedan"),
		CCarChase_Sedan::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CarChaseBike*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_CarChaseBike"),
		CCarChase_Bike::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CarChaseHeli*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_CarChaseHeli"),
		CCarChase_Heli::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region NPC/Adventure
	/* For.Prototype_GameObject_RoadNML */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_RoadNML"),
		CRoadNML::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_RoadCAB*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_RoadCAB"),
		CRoadCAB::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_RoadStanding_NML*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_RoadStanding_NML"),
		CRoadStanding_NML::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_RoadTissue*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_RoadTissue"),
		CRoadTissue::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_RoadYOP*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_RoadYOP"),
		CRoadYOP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MonsterGroup*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_MonsterGroup"),
		CMonsterGroup::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Nishiki*/
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Nishiki"),
		CNishiki::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion


	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Anim::Loading_For_Office_1F()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("애님 로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Anim::Loading_For_Office_2F()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;
	lstrcpy(m_szLoadingText, TEXT("애님 로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Anim::Loading_For_Office_Boss()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;
	lstrcpy(m_szLoadingText, TEXT("애님 로딩이 완료되었습니다."));
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader_Anim::Loading_For_Dogimazo()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;
	lstrcpy(m_szLoadingText, TEXT("애님 로딩이 완료되었습니다."));
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader_Anim::Loading_For_Dogimazo_Stairs()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;
	lstrcpy(m_szLoadingText, TEXT("애님 로딩이 완료되었습니다."));
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader_Anim::Loading_For_Dogimazo_Lobby()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;
	lstrcpy(m_szLoadingText, TEXT("애님 로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Anim::Loading_For_Dogimazo_Boss()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;
	lstrcpy(m_szLoadingText, TEXT("애님 로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Anim::Loading_For_Street()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;
	
	//행인 액션/모델 로드하기
	lstrcpy(m_szLoadingText, TEXT("애님 로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Anim::Loading_For_CarChase()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

#pragma region Model
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	Add_Models_On_Path(LEVEL_CARCHASE, TEXT("../Bin/Resources/Models/Anim/Car/"));
	Add_Models_On_Path_NonAnim(LEVEL_CARCHASE, TEXT("../Bin/Resources/Models/NonAnim/Taxi")); // 논애님이지만 플레이어로 구분
#pragma endregion

#pragma region Component
	lstrcpy(m_szLoadingText, TEXT("컴포넌트 원형 를(을) 로딩 중 입니다."));
	/* For.Prototype_Component_CarChaseAnim */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_CarChaseAnim"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_CarChase.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_CarChaseAnim"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_CarChase.fbx", true))))
	//	return E_FAIL;
#pragma endregion

#pragma region BT
	lstrcpy(m_szLoadingText, TEXT("행동트리 원형 를(을) 로딩 중 입니다."));
		/* For.Prototype_BTNode_Van*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_Van"),
		CAI_Van::Create())))
		return E_FAIL;

	/* For.Prototype_BTNode_Bike*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_Bike"),
		CAI_Bike::Create())))
		return E_FAIL;

	/* For.Prototype_BTNode_Sedan*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_Sedan"),
		CAI_Sedan::Create())))
		return E_FAIL;

	/* For.Prototype_BTNode_Heli*/
	if (FAILED(m_pGameInstance->Add_BTNode_Prototype(m_eNextLevel, TEXT("Prototype_BTNode_Heli"),
		CAI_Heli::Create())))
		return E_FAIL;
#pragma endregion


	lstrcpy(m_szLoadingText, TEXT("애님 로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Anim::Loading_For_Karaoke()
{
	//if (FAILED(Loading_Default()))
	//	return E_FAIL;

	///* For.Prototype_Component_Anim_Kiryu_Karaoke_CutScene */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Kiryu_Karaoke_CutScene"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_Kiryu_Karaoke_CutScene.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Kiryu_Karaoke_CutScene"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Kiryu_Karaoke_CutScene.fbx", true))))
	//	return E_FAIL;

		///* For.Prototype_Component_Anim_Kiryu_Karaoke_Face */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Kiryu_Karaoke_Face"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_Kiryu_Karaoke_Face.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Kiryu_Karaoke_Face"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Kiryu_Karaoke_Face.fbx", true))))

	//if (FAILED(Loading_Default()))
	//	return E_FAIL;
	Add_Models_On_Path(m_eNextLevel, TEXT("../Bin/Resources/Models/Anim/Player/"));
	Add_Models_On_Path(m_eNextLevel, TEXT("../Bin/Resources/Models/Anim/NPC/"));

	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Char/NPC"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Char/Player"));


	/* For.Prototype_Component_Anim_NPC */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_NPC"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Monster/Jimu/Animation_NPC.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_NPC"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Monster/Animation_NPC.fbx", true))))
	//	return E_FAIL;

	///* For.Prototype_Component_CutSceneAnim_ForPlayer */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_CutSceneAnim_ForPlayer"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_CutScene_ForPlayer.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_CutSceneAnim_ForPlayer"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Kiryu_CamAction.fbx", true))))
	//	return E_FAIL;

	/* For.Prototype_Component_Anim_Kiryu */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Kiryu"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Player/Kiryu/Animation_Kiryu.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Kiryu"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Kiryu.fbx", true))))
	//	return E_FAIL;

	///* For.Prototype_Component_Anim_KiryuFace */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_KiryuFace"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_Kiryu_Face.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_KiryuFace"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Kiryu_Face.fbx", true))))
	//	return E_FAIL;

	///* For.Prototype_Component_Anim_Hand */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Hand"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_Hand.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Hand"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Hand.fbx", true))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("애님 로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Anim::Loading_For_Tutorial()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("애님 로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Anim::Loading_For_NishikiWalk()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("애님 로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

CLoader_Anim* CLoader_Anim::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLoader_Anim* pInstance = new CLoader_Anim(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed To Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader_Anim::Free()
{
	__super::Free();
}