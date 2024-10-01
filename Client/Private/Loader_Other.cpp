#include "Loader_Other.h"

#include "GameInstance.h"

#include "Background.h"
#include "Subtitle.h"

#pragma region Camera
#include "PlayerCamera.h"
#include "DebugCamera.h"
#include "CineCamera.h"
#include "CutSceneCamera.h"
#include "CarChaseCamera.h"
#pragma endregion

#pragma region Effect
#include "Particle_Point.h"
#include "TRailEffect.h"
#include "Aura.h"
#include "Particle_Mesh.h"
#pragma endregion

CLoader_Other::CLoader_Other(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLoader{ pDevice, pContext }
{
}

_uint APIENTRY LoadingOther(void* pArg)
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	CLoader_Other* pLoader = (CLoader_Other*)pArg;

	if (FAILED(pLoader->Loading()))
		return 1;

	CoUninitialize();

	return 0;
}

HRESULT CLoader_Other::Initialize(LEVEL eNextLevel)
{
	if (FAILED(__super::Initialize(eNextLevel)))
		return E_FAIL;

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingOther, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader_Other::Loading()
{
	if (FAILED(__super::Loading()))
		return E_FAIL;

	return S_OK;
}

/* 공통적인 저장 객체를 넣어주는 함수. */
HRESULT CLoader_Other::Loading_Default()
{
#pragma region Effect_Texture
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_Sphere"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/lm000.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Trail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_Trail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/001_trc_n.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_KuzeTrail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_KuzeTrail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/kuze/008_trc_n.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_KuzeHand */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_KuzeHand"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/kuze/008_trc_na.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_KuzeHandFire */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_KuzeHandFire"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/kuze/008_Kuze_handFire.dds"), 1))))
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

	/* Prototype_Component_Texture_Coin*/
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_Dissolve_0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Dissovle_%d.dds"), 1))))
		return E_FAIL;
#pragma endregion

#pragma region Component
	lstrcpy(m_szLoadingText, TEXT("컴포넌트 원형 를(을) 로딩 중 입니다."));
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

		/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Collider"), CCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Instance_Mesh */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_Instance_Mesh"),
		CVIBuffer_Instance_Mesh::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Instance_Point */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_VIBuffer_Instance_Point"),
		CVIBuffer_Instance_Point::Create(m_pDevice, m_pContext))))
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
	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Shader_VtxCube_Occulusion"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube_Occulusion.hlsl"), VTXCUBE_OCCULUSION::Elements, VTXCUBE_OCCULUSION::iNumElements))))
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

#pragma region Model
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


#pragma region Fire_Texture
	/* Prototype_Component_Texture_FireRoundA */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_FireRoundA"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Fire/e_j_0081_fireGround02.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_FireRoundB */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_FireRoundB"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Fire/e_j_0081_fireGround11.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_FireRoundC */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_FireRoundC"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Fire/e_j_0070_fireani_aa.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_RoopSmoke */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_RoopSmoke"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Particle/Fire/e_f_0014_roop_smoke01.dds"), 1))))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CLoader_Other::Loading_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("Other 객체원형을 로딩 중 입니다."));

	Add_GameObject_Particle_On_Path(TEXT("../../Client/Bin/DataFiles/Particle/"));

#pragma region Camera
	/* For.Prototype_GameObject_DebugCamera */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_DebugCamera"), CDebugCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_PlayerCamera */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_PlayerCamera"), CPlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CCineCamera */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_CCineCamera"), CCineCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CutSceneCamera */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_CutSceneCamera"), CCutSceneCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CarChaseCamera */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_CarChaseCamera"), CCarChaseCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

	/* For.Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Subtitle */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_Subtitle"), CSubtitle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Other::Loading_For_Office_1F()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Other::Loading_For_Office_2F()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Other::Loading_For_Office_Boss()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader_Other::Loading_For_Dogimazo()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader_Other::Loading_For_Dogimazo_Stairs()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader_Other::Loading_For_Dogimazo_Lobby()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Other::Loading_For_Dogimazo_Boss()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Other::Loading_For_Street()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Other::Loading_For_CarChase()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Other::Loading_For_Karaoke()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Othre 로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Other::Loading_For_NishikiWalk()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Othre 로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Other::Loading_For_Tutorial()
{
	if (FAILED(Loading_Default()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component_Prototype(m_eNextLevel, TEXT("Prototype_Component_Texture_Nishiki_Beeper"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/Store/2d_yk_adv01-010.dds"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Othre 로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

CLoader_Other* CLoader_Other::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLoader_Other* pInstance = new CLoader_Other(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed To Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader_Other::Free()
{
	__super::Free();
}