#include "MultiLoader.h"

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

#include "MonsterGroup.h"
#pragma endregion

#pragma region Weapon
#include "Weapon_Gun_Cz75.h"
#include "Weapon_MchnGun.h"
#include "Weapon_RcktGun.h"
#include "Weapon_ShotGun.h"
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

#pragma region NPC
#include "Nishiki.h"
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
#include "RoadYOP.h"
#include "AI_RoadYOP.h"

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

CMultiLoader::CMultiLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice },
	m_pContext{ pContext },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

_uint APIENTRY Loading_Anim(void* pArg)
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	CMultiLoader* pMultiLoader = (CMultiLoader*)pArg;

	if (FAILED(pMultiLoader->Loading(CMultiLoader::LOADING_ANIM)))
		return 1;

	CoUninitialize();

	return 0;
}

_uint APIENTRY Loading_NonAnim(void* pArg)
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	CMultiLoader* pMultiLoader = (CMultiLoader*)pArg;

	if (FAILED(pMultiLoader->Loading(CMultiLoader::LOADING_NONANIM)))
		return 1;

	CoUninitialize();

	return 0;
}

_uint APIENTRY Loading_Main(void* pArg)
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	CMultiLoader* pMultiLoader = (CMultiLoader*)pArg;

	if (FAILED(pMultiLoader->Loading(CMultiLoader::LOADING_DEFAULT)))
		return 1;

	CoUninitialize();

	return 0;
}

HRESULT CMultiLoader::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_Critical_Section);

	m_hAnimThread = (HANDLE)_beginthreadex(nullptr, 0, Loading_Anim, this, 0, nullptr);
	if (0 == m_hAnimThread)
		return E_FAIL;

	m_hNonAnimThread = (HANDLE)_beginthreadex(nullptr, 0, Loading_NonAnim, this, 0, nullptr);
	if (0 == m_hNonAnimThread)
		return E_FAIL;

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Loading_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CMultiLoader::Loading(_uint iType)
{
	//EnterCriticalSection(&m_Critical_Section);

	HRESULT			hr{};

	if (iType == LOADING_ANIM)
		hr = Loading_For_Anim();
	else if (iType == LOADING_NONANIM)
		hr = Loading_For_NonAnim();
	else if (iType == LOADING_DEFAULT)
		hr = Loading_For_Default();

	//LeaveCriticalSection(&m_Critical_Section);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

/* 공통적인 저장 객체를 넣어주는 함수. */

/* 공통적인 저장 객체를 넣어주는 함수. */
HRESULT CMultiLoader::Loading_Default()
{

#pragma region Texture
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/T_Sand_06_A.png"), 1))))
		return E_FAIL;
#pragma endregion

	/* Prototype_Component_Texture_Sphere */
#pragma region Effect_Texture
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_Sphere"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/lm000.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Trail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_Trail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/001_trc_n.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5))))
		return E_FAIL;

	/* Prototype_Component_Texture_Test */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_Test"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/9923j.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_HitSpark */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_HitSpark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/015_trc_na.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GuardParticle */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_GuardParticle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/001_trc_na.dds"), 1))))
		return E_FAIL;
	/* Prototype_Component_Texture_GuardSmoke */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_GuardSmoke"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0351_2d_moya.dds"), 1))))
		return E_FAIL;
#pragma endregion

#pragma region Aura_Texture
	/* Prototype_Component_Texture_AuraAnim */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_AuraAnim"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0076_aura_g.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_FireAnim */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_FireAnim"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0085_fireanim_f.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_ExpAnimA */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_ExpAnimA"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0252_expAnimG.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_ExpAnimB */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_ExpAnimB"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0252_explosionB.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_CarBackLight */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_CarBackLight"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Backlight.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BrokenGlass0 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BrokenGlass0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0090_glass_0.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BrokenGlass1 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BrokenGlass1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0090_glass_1.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BrokenGlass2 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BrokenGlass2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0090_glass_2.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BrokenGlass3 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BrokenGlass3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0090_glass_3.dds"), 1))))
		return E_FAIL;


	/* Prototype_Component_Texture_AuraFluid */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_AuraFluid"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0309_fluidFlowA01.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_AuraBase */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_AuraBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0291_smoke_jA8L8.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_AuraTone */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_AuraTone"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/auratone24.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_AuraToneRush */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_AuraToneRush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/auratone22.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_AuraToneDestroy */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_AuraToneDestroy"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/auratone17.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_HitBase */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_HitBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0233_radialpat_d.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_SmokeBase */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_SmokeBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_o_2208_smoke_j.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GuardDist */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_GuardDist"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0238_radialPtnG.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Money*/
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_Money"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Money/banknote1000.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Coin*/
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_Coin"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Money/coin100.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Coin*/
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_WaveFace"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/e_j_0011_waveface.dds"), 1))))
		return E_FAIL;

#pragma endregion

#pragma region Blood_Texture
	/* Prototype_Component_Texture_BloodD0 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodD0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0042_chi_sibuki_e.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BloodN0 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodN0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0043_chi_sibuki_e_nml.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BloodD1 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodD1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0050_chiato_c.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BloodN1 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodN1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0051_chiato_c_nml.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BloodD2 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodD2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0052_chiato_d.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BloodN2 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodN2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0053_chiato_d_nml.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BloodD3 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodD3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0054_chiato_e.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BloodN3 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodN3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0055_chiato_e_nml.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BloodD4 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodD4"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0060_chiato_g.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BloodN4 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodN4"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0060_chiato_g_nml.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BloodD5 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodD5"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_o_2510_blood_a.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BloodN5 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodN5"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_o_2510_blood_a_nml.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BloodD6 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodD6"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_o_2513_blood_d.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BloodN6 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodN6"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_o_2513_blood_d_nml.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BloodD7 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodD7"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_o_2514_blood_e.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BloodN7 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodN7"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_o_2514_blood_e_nml.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BloodD8 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodD8"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_o_2515_blood_f.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BloodN8 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodN8"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_o_2515_blood_f_nml.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LiquidD */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_LiquidD"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0015_fluid01.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LiquidN */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_LiquidN"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0015_fluid01Nml.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LiquidAnimAD */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_LiquidAnimAD"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0017_liquidanim_beb.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LiquidAnimAN */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_LiquidAnimAN"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0017_liquidanim_bebNml.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LiquidAnimBD */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_LiquidAnimBD"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0017_liquidanim_bf.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LiquidAnimBN */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_LiquidAnimBN"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0017_liquidanim_bfNml.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LiquidAnimCD */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_LiquidAnimCD"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0017_liquidanim_bf2.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LiquidAnimCN */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_LiquidAnimCN"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0017_liquidanim_bf2Nml.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LiquidAnimDD */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_LiquidAnimDD"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0017_liquidanim_g.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LiquidAnimDN */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_LiquidAnimDN"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0017_liquidanim_gNml.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LiquidAnimED */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_LiquidAnimED"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0017_liquidanim_g_boke.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LiquidAnimEN */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_LiquidAnimEN"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0017_liquidanim_gNml2.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LiquidAnimFD */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_LiquidAnimFD"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0017_liquidanim_hb.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LiquidAnimFN */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_LiquidAnimFN"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0017_liquidanim_hbNml.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_BloodAnim */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_BloodAnim"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Blood/e_j_0033_bfBloodSplash01.dds"), 1))))
		return E_FAIL;
#pragma endregion


#pragma region DissolveTexture
	/* Prototype_Component_Texture_Coin*/
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_Dissolve_0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Dissovle_%d.dds"), 1))))
		return E_FAIL;
#pragma endregion


#pragma region Component
	lstrcpy(m_szLoadingText, TEXT("컴포넌트 원형 를(을) 로딩 중 입니다."));
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_Terrain_Flat"), CVIBuffer_Terrain_Flat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_AABBCube */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_AABBCube"), CVIBuffer_AABBCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

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

		///* For.Prototype_Component_CutSceneAnim_ForPlayer */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_CutSceneAnim_ForPlayer"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_CutScene_ForPlayer.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_CutSceneAnim_ForPlayer"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_CutScene_ForPlayer.fbx", true))))
	//	return E_FAIL;

	///* For.Prototype_Component_Anim_Kiryu_Karaoke_CutScene */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Kiryu_Karaoke_CutScene"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_Kiryu_Karaoke_CutScene.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Kiryu_Karaoke_CutScene"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Kiryu_Karaoke_CutScene.fbx", true))))
	//	return E_FAIL;

	///* For.Prototype_Component_Anim_Kiryu */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Kiryu"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Player/Kiryu/Animation_Kiryu.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Kiryu"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Kiryu.fbx", true))))
	//	return E_FAIL;

	///* For.Prototype_Component_Anim_KiryuFace */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_KiryuFace"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_Kiryu_Face.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_KiryuFace"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Kiryu_Face.fbx", true))))
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

	///* For.Prototype_Component_Anim_Hand */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Hand"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_Hand.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Hand"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Hand.fbx", true))))
	//	return E_FAIL;

	///* For.Prototype_Component_Anim_Kiryu_Karaoke_Face */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Kiryu_Karaoke_Face"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_Kiryu_Karaoke_Face.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Anim_Kiryu_Karaoke_Face"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Kiryu_Karaoke_Face.fbx", true))))
	//	return E_FAIL;

	///* For.Prototype_Component_Kiryu_CarChase */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Kiryu_CarChase"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_Kiryu_CarChase.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Kiryu_CarChase"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_Kiryu_CarChase.fbx", true))))
	//	return E_FAIL;

		/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Collider"), CCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Instance_Point */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_Instance_Point"),
		CVIBuffer_Instance_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Instance_Mesh */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_Instance_Mesh"),
		CVIBuffer_Instance_Mesh::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Trail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_AStart */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_AStart"),
		CAStart::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region Effect_Model

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Model_ParticleSphere"),
		//CModel::Create(m_pDevice, m_pContext, CModel::TYPE_PARTICLE, "../../Client/Bin/Resources/Models/NonAnim/Particle/Bone_Sphere.fbx", PreTransformMatrix, false, true))))
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_PARTICLE, "../../Client/Bin/Resources/Models/NonAnim/Particle/Bin/Bone_Sphere.dat", PreTransformMatrix, true))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Model_ParticleMoney"),
		//CModel::Create(m_pDevice, m_pContext, CModel::TYPE_PARTICLE, "../../Client/Bin/Resources/Models/NonAnim/Particle/Money.fbx", PreTransformMatrix, false, true))))
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_PARTICLE, "../../Client/Bin/Resources/Models/NonAnim/Particle/Bin/Money.dat", PreTransformMatrix, true))))
		return E_FAIL;

#pragma endregion

#pragma endregion

#pragma region Meterial
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Char/Player"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Char/Monster"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Char/NPC"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Reactor"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Map/Map0"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Map/Map1"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Map/Map2"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Map/Map3"));
	Add_Components_On_Path_Material(m_eNextLevel, TEXT("../Bin/DataFiles/MaterialData/Map/KaraokeMap"));
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

#pragma region Shader
	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));
	/* For.Prototype_Component_Shader_VtxAnim */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMBONE::Elements, VTXANIMBONE::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_Mesh */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_Mesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_MeshItem */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_MeshItem"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshItem.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_MeshMap */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_MeshMap"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshMap.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxMeshSky */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_VtxMeshSky"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshSky.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxMeshLight */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_VtxMeshLight"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshLight.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_VtxCube_Occulusion"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube_Occulusion.hlsl"), VTXCUBE_OCCULUSION::Elements, VTXCUBE_OCCULUSION::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_Aura*/
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_Aura"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Aura.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_Trail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_Trail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Trail.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxInstance_Point */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Point.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Mesh*/
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_VtxInstance_Mesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Mesh.hlsl"), VTXINSTANCE_MESH::Elements, VTXINSTANCE_MESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_BoneCompute */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_BoneCompute"),
		CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_BoneCompute.hlsl")))))
		return E_FAIL;


	/* For.Prototype_Component_Shader_OcculusionCulling */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_OcculusionCulling"),
		CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_OcculusionCulling.hlsl")))))
		return E_FAIL;


#pragma endregion

	return S_OK;

}


HRESULT CMultiLoader::Loading_Highway()
{
#pragma region BTNode
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

	return S_OK;
}
HRESULT CMultiLoader::Loading_For_Anim()
{
	Add_Models_On_Path(LEVEL_TEST, TEXT("../Bin/Resources/Models/Anim/Player/"));
	Add_Models_On_Path(LEVEL_TEST, TEXT("../Bin/Resources/Models/Anim/Monster/"));
	Add_Models_On_Path(LEVEL_TEST, TEXT("../Bin/Resources/Models/Anim/NPC/"));
	Add_Models_On_Path(LEVEL_TEST, TEXT("../Bin/Resources/Models/Anim/Car/"));

	/* For.Prototype_Component_CarChaseAnim */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_CarChaseAnim"), CAnim::Create(m_pDevice, m_pContext, "../Bin/DataFiles/AnimationData/Animation_CarChase.dat", false))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_CarChaseAnim"), CAnim::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Anim/Animation_CarChase.fbx", true))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("애님 로딩이 완료되었습니다."));

	m_isFinished[0] = true;

	return S_OK;
}

HRESULT CMultiLoader::Loading_For_NonAnim()
{
#pragma region Model
	Add_Models_On_Path_NonAnim(LEVEL_TEST, TEXT("../Bin/Resources/Models/NonAnim/Map/Map0"));

	Add_Models_On_Path_NonAnim(LEVEL_TEST, TEXT("../Bin/Resources/Models/NonAnim/Map/Map1"));

	Add_Models_On_Path_NonAnim(LEVEL_TEST, TEXT("../Bin/Resources/Models/NonAnim/Map/Map2"));

	Add_Models_On_Path_NonAnim(LEVEL_TEST, TEXT("../Bin/Resources/Models/NonAnim/Map/Map3"));

	Add_Models_On_Path_NonAnim(LEVEL_TEST, TEXT("../Bin/Resources/Models/NonAnim/Map/KaraokeMap"));

	Add_Models_On_Path_NonAnim(LEVEL_TEST, TEXT("../Bin/Resources/Models/NonAnim/Bone_Sphere"));

	Add_Models_On_Path_NonAnim(LEVEL_TEST, TEXT("../Bin/Resources/Models/NonAnim/Gun_Cz75"));

	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Reactor/Moving_Sedan"));
	Add_Models_On_Path_NonAnim(m_eNextLevel, TEXT("../Bin/Resources/Models/NonAnim/Reactor/Moving_Sedan_2"));

	Add_Models_On_Path_NonAnim(LEVEL_TEST, TEXT("../Bin/Resources/Models/NonAnim/Taxi")); // 논애님이지만 플레이어로 구분
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("논애님 로딩이 완료되었습니다."));

	m_isFinished[1] = true;

	return S_OK;
}

HRESULT CMultiLoader::Loading_For_Default()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	if (FAILED(Loading_Highway()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("디폴트 로딩이 완료되었습니다."));

	m_isFinished[2] = true;

	return S_OK;
}

// 경로를 넣어주면 디렉토리 이름을 읽어와서 bin존재 유무를 파악해 바이너리화 or bin파일 읽기를 자동으로 해준다
// 하지만 해당 함수를 사용하려면 넣어준 경로 안에 폴더명과 똑같은 이름의 fbx를 가지고 있어야 한다.
HRESULT CMultiLoader::Add_Models_On_Path(_uint iLevel, const wstring& strPath, _bool bAnim)
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

HRESULT CMultiLoader::Add_GameObject_Particle_On_Path(const wstring& strPath)
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

HRESULT CMultiLoader::Add_Models_On_Path_NonAnim(_uint iLevel, const wstring& strPath)
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

HRESULT CMultiLoader::Add_Components_On_Path_Material(_uint iLevel, const wstring& strPath)
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

CMultiLoader* CMultiLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CMultiLoader* pInstance = new CMultiLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed To Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMultiLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	WaitForSingleObject(m_hAnimThread, INFINITE);
	WaitForSingleObject(m_hNonAnimThread, INFINITE);

	DeleteObject(m_hThread);
	DeleteObject(m_hAnimThread);
	DeleteObject(m_hNonAnimThread);

	CloseHandle(m_hThread);
	CloseHandle(m_hAnimThread);
	CloseHandle(m_hNonAnimThread);

	DeleteCriticalSection(&m_Critical_Section);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}