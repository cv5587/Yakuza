#include "Highway_Kiryu.h"

#include "GameInstance.h"
#include "Anim.h"
#include "Mesh.h"
#include "Bone.h"

#include "CarChase_Reactor.h"
#include "SocketModel.h"
#include "Weapon_Gun_Cz75.h"
#include "UIManager.h"
#include "CarChaseCamera.h"
#include"EffectManager.h"

CHighway_Kiryu::CHighway_Kiryu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext }
{
}

CHighway_Kiryu::CHighway_Kiryu(const CHighway_Kiryu& rhs)
	: CLandObject{ rhs }
{
}

HRESULT CHighway_Kiryu::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHighway_Kiryu::Initialize(void* pArg)
{
	CARCHASE_KIRYU_DESC* pDesc = static_cast<CARCHASE_KIRYU_DESC*>(pArg);
	pTaxiMatrix = pDesc->pTaxiWorldMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_wstrModelName = TEXT("Kiryu_CarChase");

	if (FAILED(Add_CharacterData()))
		return E_FAIL;

	if (FAILED(Add_Objects()))
		return E_FAIL;

	Set_HandAnimIndex(HAND_GUN);

	//m_pModelCom->Set_Separation_ParentBone("center_c_n", DEFAULT_ANIM);
	//m_pModelCom->Set_Separation_SingleBone(m_isLeft ? "ude2_l_n" : "ude2_r_n", -1);

	return S_OK;
}

void CHighway_Kiryu::Priority_Tick(const _float& fTimeDelta)
{
	if(m_isLeft)
		m_pGun_L->Priority_Tick(fTimeDelta);
	else
		m_pGun_R->Priority_Tick(fTimeDelta);
}

void CHighway_Kiryu::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Key_Input();

	m_pModelCom->Play_Animation_Separation(fTimeDelta, m_iFaceAnimIndex, m_SeparationAnimComs[FACE_ANIM], false, (_int)FACE_ANIM);
	m_pModelCom->Play_Animation_Separation(fTimeDelta, m_iHandAnimIndex, m_SeparationAnimComs[HAND_ANIM], false, (_int)HAND_ANIM);

	Play_CurrentAnimation(fTimeDelta);

	m_ModelMatrix = *pTaxiMatrix;

	if(m_isLeft)
		m_pGun_L->Tick(fTimeDelta);
	else
		m_pGun_R->Tick(fTimeDelta);

	//if (m_isFirstTick)
	//{
	//	m_isFirstTick = false;
	//	On_Separation_Hand(m_isLeft ? 1 : 2);
	//}

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_ModelMatrix));
}

void CHighway_Kiryu::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if(m_isLeft)
		m_pGun_L->Late_Tick(fTimeDelta);
	else
		m_pGun_R->Late_Tick(fTimeDelta);

	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CHighway_Kiryu::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		if (!m_pGameInstance->isShadow())
		{
			m_pModelCom->Bind_BoneMatrices(i);
			m_pModelCom->Bind_Compute(m_pComputeShaderCom, i);
		}

		if (nullptr != m_pMaterialCom)
		{
			if (FAILED(m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(pMesh->Get_MaterialIndex()))))
				return E_FAIL;
		}

		_float fFar = *m_pGameInstance->Get_CamFar();
		m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		_bool isRS = true;
		_bool isRD = true;
		_bool isRM = true;
		_bool isRT = true;
		_bool isMulti = true;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MultiDiffuseTexture", i, aiTextureType_SHININESS)))
			isMulti = false;
		m_pShaderCom->Bind_RawValue("g_isMulti", &isMulti, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RSTexture", i, aiTextureType_SPECULAR)))
			isRS = false;
		m_pShaderCom->Bind_RawValue("g_isRS", &isRS, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RDTexture", i, aiTextureType_OPACITY)))
			isRD = false;
		m_pShaderCom->Bind_RawValue("g_isRD", &isRD, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RMTexture", i, aiTextureType_METALNESS)))
			isRM = false;
		m_pShaderCom->Bind_RawValue("g_isRM", &isRM, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RTTexture", i, aiTextureType_EMISSIVE)))
			isRT = false;
		m_pShaderCom->Bind_RawValue("g_isRT", &isRT, sizeof(_bool));

		if (pMesh->Get_AlphaApply())
			m_pShaderCom->Begin(1);     //블랜드
		else
			m_pShaderCom->Begin(0);		//디폴트

		m_pModelCom->Render(i);

		i++;
	}

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

void CHighway_Kiryu::OnHit(_float fDamage)
{
	m_fHP -= fDamage;
	m_eCurrentBehavior = HIT;
}

void CHighway_Kiryu::Change_Animation()
{
}

void CHighway_Kiryu::Key_Input()
{
	//공격 가능한 환경인지 체크한 후 진행한다.
	//다른 스킬들도 막기 위해서 return;
	// 발사
	//if (m_pGameInstance->GetMouseState(DIM_LB) == TAP)
	//{
	//	if(m_iCurrentAmmo > 0)
	//		Change_Behavior(SHOT);
	//}

	// 히트아이 사용
	if (m_pGameInstance->GetKeyState(DIK_LSHIFT) == HOLD)
	{
		if (0 >= m_fHitEye)
		{
			m_fHitEye = 0.f;
			m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
		}
		else
		{
			m_isHitEyeCharging = false;
			m_fHitEye -= HITEYE_DECREASE_SPEED * m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));
			m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 0.2f);
		}
	}
	else if (m_pGameInstance->GetKeyState(DIK_LSHIFT) == AWAY)
	{
		m_isHitEyeCharging = true;
		m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
	}

	if (m_isHitEyeCharging)
	{
		m_fHitEye += HITEYE_DECREASE_SPEED * m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));
		if (100.f < m_fHitEye)
			m_fHitEye = 100.f;
	}

	// 장전/숨기
	if (m_pGameInstance->GetKeyState(DIK_E) == HOLD)
	{
		m_fAccReloadTimer += m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));

		m_pGameInstance->PlaySoundIfNotPlay(L"gun_reload_0.wav", SOUND_EFFECT, DEFAULT_EFFECT_VOLUME);

		if(m_iCurrentAmmo< MAX_AMMO)
		{
			if (RELOAD_TIME <= m_fAccReloadTimer)
			{
				m_iCurrentAmmo = MAX_AMMO;
				m_fAccReloadTimer = 0.f;
			}
		}

		Change_Behavior(HIDE);

		CCarChaseCamera* pCamera = dynamic_cast<CCarChaseCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_CARCHASE));
		pCamera->ShoulderView_Off();
	}
	else if (m_pGameInstance->GetKeyState(DIK_E) == AWAY)
	{
		m_fAccReloadTimer = 0.f;

		Change_Behavior(AIMING);

		CCarChaseCamera* pCamera = dynamic_cast<CCarChaseCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_CARCHASE));
		pCamera->ShoulderView_On();
	}

	// 자리 바꾸기 (플레이어가 직접 조작하는 것이 아니라 디버깅용으로 추가한 버튼)
	if (m_pGameInstance->GetKeyState(DIK_C) == HOLD)
	{
		Change_Behavior(SWAP);
	}
}

void CHighway_Kiryu::Play_CurrentAnimation(_float fTimeDelta)
{
	switch (m_eCurrentBehavior)
	{
	case CHighway_Kiryu::AIMING:
	{
		Play_Animing(fTimeDelta);
		break;
	}
	case CHighway_Kiryu::HIDE:
	{
		Play_HideReload(fTimeDelta);
		break;
	}
	case CHighway_Kiryu::HIT:
	{
		Play_Hit(fTimeDelta);
		break;
	}
	case CHighway_Kiryu::SHOT:
	{
		Play_Shot(fTimeDelta);
		break;
	}
	case CHighway_Kiryu::SWAP:
	{
		Play_Swap(fTimeDelta);
		break;
	}
	}

	m_pModelCom->Play_Animation(fTimeDelta, false);
}

void CHighway_Kiryu::Play_Animing(_float fTimeDelta)
{
	// 시작 모션 (차에서 일어서는 것)
	// [71] [mngcar_c_car_gun_sitl_en]
	// [74] [mngcar_c_car_gun_sitr_en]
	// 
	// [5] [mngcar_c_car_gun_aiml_b_hi]
	// [16] [mngcar_c_car_gun_aiml_f_hi]
	// [27] [mngcar_c_car_gun_aiml_l_hi]
	// [40] [mngcar_c_car_gun_aimr_b_hi]
	// [51] [mngcar_c_car_gun_aimr_f_hi]
	// [62] [mngcar_c_car_gun_aimr_r_hi]
	// 
	// [7] [mngcar_c_car_gun_aiml_b_lp]
	// [6] [mngcar_c_car_gun_aiml_b_l45]
	// [29] [mngcar_c_car_gun_aiml_l_lp]		// 이 모션만 뭔가 잘못된건지 허리가 비틀려있음
	// [18] [mngcar_c_car_gun_aiml_f_r45]
	// [17] [mngcar_c_car_gun_aiml_f_lp]
	// 
	// [42] [mngcar_c_car_gun_aimr_b_lp]
	// [41] [mngcar_c_car_gun_aimr_b_l45]
	// [64] [mngcar_c_car_gun_aimr_r_lp]
	// [53] [mngcar_c_car_gun_aimr_f_r45]
	// [52] [mngcar_c_car_gun_aimr_f_lp]

	// 몬스터에 에임띄운거 나오고 나서 각도조절기능 추가예정

	_uint iAnimIndex = (m_isLeft ? 29 : 64);

	//// 0 앞, 1 앞 대각, 2 옆, 3 뒷 대각, 4 뒤
	//switch (m_iStageDir)
	//{
	//case 0:			//앞
	//	iAnimIndex = (m_isLeft ? 17 : 52);
	//	break;
	//case 1:		//앞 대각
	//	iAnimIndex = (m_isLeft ? 18 : 53);
	//	break;
	//case 2:		//옆
	//	iAnimIndex = (m_isLeft ? 29 : 64);
	//	break;
	//case 3:		//뒷 대각
	//	iAnimIndex = (m_isLeft ? 6 : 41);
	//	break;
	//case 4:		//뒤
	//	iAnimIndex = (m_isLeft ? 7 : 42);
	//	break;
	//}

	if (!m_isStarted)
		m_pModelCom->Set_AnimationIndex((m_isLeft ? 71 : 74), 4.f);
	else
		m_pModelCom->Set_AnimationIndex(iAnimIndex, 4.f);

	if (m_pModelCom->Get_AnimFinished())
		m_isStarted = true;
}

void CHighway_Kiryu::Play_HideReload(_float fTimeDelta)
{
	// 시작 모션 (차에 들어가는 것)
	// [73] [mngcar_c_car_gun_sitl_st]
	// [76] [mngcar_c_car_gun_sitr_st]
	// [72] [mngcar_c_car_gun_sitl_lp]
	// [75] [mngcar_c_car_gun_sitr_lp]
	if (!m_isStarted)
		m_pModelCom->Set_AnimationIndex((m_isLeft ? 73 : 76), 4.f);
	else
		m_pModelCom->Set_AnimationIndex((m_isLeft ? 72 : 75), 4.f);

	if (m_pModelCom->Get_AnimFinished())
		m_isStarted = true;
}

void CHighway_Kiryu::Play_Hit(_float fTimeDelta)
{
	/*
	* [2] [mngcar_c_car_gun_aiml_b_dam]
	* [13] [mngcar_c_car_gun_aiml_f_dam]
	* [24] [mngcar_c_car_gun_aiml_l_dam]
	* 
	* [37] [mngcar_c_car_gun_aimr_b_dam]
	* [48] [mngcar_c_car_gun_aimr_f_dam]
	* [59] [mngcar_c_car_gun_aimr_r_dam]
	*/

	_uint iAnimIndex = (m_isLeft ? 24 : 59);
	//switch (m_iStageDir)
	//{
	//case DIR_F:
	//	iAnimIndex = (m_isLeft ? 13 : 48);
	//	break;
	//case DIR_B:
	//	iAnimIndex = (m_isLeft ? 2 : 37);
	//	break;
	//case DIR_L:
	//	iAnimIndex = (m_isLeft ? 24 : 59);
	//	break;
	//case DIR_R:
	//	iAnimIndex = (m_isLeft ? 24 : 59);
	//	break;
	//}

	m_pModelCom->Set_AnimationIndex(iAnimIndex, 4.f);

	if (m_pModelCom->Get_AnimFinished())
	{
		Change_Behavior(AIMING);
	}


}

void CHighway_Kiryu::Play_Shot(_float fTimeDelta)
{
	if (!m_isShot)
	{
		//이펙트 생성
		CEffect::EFFECT_DESC EffectDesc;


		if (m_isLeft)
			EffectDesc.pWorldMatrix = m_pGun_L->Get_PartWorldMatrix();
		else
			EffectDesc.pWorldMatrix = m_pGun_R->Get_PartWorldMatrix();


		CEffectManager::GetInstance()->Shot_Flash(EffectDesc);
		m_isShot = true;
	}

	// [31] [mngcar_c_car_gun_aiml_l_shot]
	// [19] [mngcar_c_car_gun_aiml_f_shot]
	// [8] [mngcar_c_car_gun_aiml_b_shot]
	// [43] [mngcar_c_car_gun_aimr_b_shot]
	// [54] [mngcar_c_car_gun_aimr_f_shot]
	// [66] [mngcar_c_car_gun_aimr_r_shot]

	_uint iAnimIndex = (m_isLeft ? 31 : 66);

	//// 0 앞, 1 앞 대각, 2 옆, 3 뒷 대각, 4 뒤
	//switch (m_iStageDir)
	//{
	//case 0:			//앞
	//	iAnimIndex = (m_isLeft ? 17 : 52);
	//	break;
	//case 1:		//앞 대각
	//	iAnimIndex = (m_isLeft ? 31 : 66);
	//	break;
	//case 2:		//옆
	//	iAnimIndex = (m_isLeft ? 31 : 66);
	//	break;
	//case 3:		//뒷 대각
	//	iAnimIndex = (m_isLeft ? 31 : 66);
	//	break;
	//case 4:		//뒤
	//	iAnimIndex = (m_isLeft ? 8 : 43);
	//	break;
	//}
	m_pModelCom->Set_AnimationIndex(iAnimIndex, 4.f);

	if (m_pModelCom->Get_AnimFinished())
	{
		Change_Behavior(AIMING);
		m_isShot = false;

	}

	//if(m_SeparationAnimComs[DEFAULT_ANIM]->Get_AnimFinished())
	//	Change_Behavior(AIMING);
}

void CHighway_Kiryu::Play_Swap(_float fTimeDelta)
{
	/*
	*	[77] [mngcar_c_car_gun_sync_aimltor]
		[78] [mngcar_c_car_gun_sync_aimrtol]
		// [73] [mngcar_c_car_gun_sitl_st]
	*/
	if (!m_isStarted)
		m_pModelCom->Set_AnimationIndex((m_isLeft ? 73 : 76), 4.f);
	else
		m_pModelCom->Set_AnimationIndex((m_isLeft ? 77 : 78), 4.f);

	// 차로 들어가는 모션
	if (!m_isStarted && Checked_Animation_Ratio(0.6))
	{
		_uint iRandom = m_pGameInstance->Get_Random(0, 1);
		if (iRandom == 0)
			m_pGameInstance->PlaySound_W(TEXT("4680#1 (kiryu_switch).wav"), SOUND_PLAYER_VOICE, DEFAULT_EFFECT_VOLUME);
		else if (iRandom == 1)
			m_pGameInstance->PlaySound_W(TEXT("4680#2 (kiryu_switch).wav"), SOUND_PLAYER_VOICE, DEFAULT_EFFECT_VOLUME);

		m_isStarted = true;
	}
	else if (Checked_Animation_Ratio(0.6))
	{
		m_isLeft = !m_isLeft;
		Change_Behavior(AIMING);
	}

}

void CHighway_Kiryu::Change_Behavior(BEHAVIOR_TYPE eType)
{
	// 양손 다 해제 후 필요한 손만 다시 설정
	Off_Separation_Hand();

	switch (eType)
	{
	case CHighway_Kiryu::AIMING:
	{
		if (m_eCurrentBehavior != SHOT && m_eCurrentBehavior != HIT)
			m_isStarted = false;

		break;
	}
	case CHighway_Kiryu::HIDE:
	{
		if (eType != m_eCurrentBehavior)
			m_isStarted = false;

		break;
	}
	case CHighway_Kiryu::HIT:
	{
		m_isStarted = true;
		break;
	}
	case CHighway_Kiryu::SHOT:
	{
		m_isStarted = true;
		
		if(m_eCurrentBehavior == SHOT && Checked_Animation_Ratio(0.4f))
			m_pModelCom->Reset_Animation((m_isLeft ? 31 : 66));

		// 카메라 쉐이킹
		CCarChaseCamera* pCamera = dynamic_cast<CCarChaseCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_CARCHASE));
		pCamera->Set_Shaking(true, { 1.f, 1.f, 1.f }, 0.3, 0.5);

		// 어깨 분리
		//m_pModelCom->Set_Separation_ParentBone(m_isLeft ? "ude2_l_n" : "ude2_r_n", DEFAULT_ANIM);
		////m_pModelCom->Set_Separation_SingleBone(m_isLeft ? "ude2_l_n" : "ude2_r_n", -1);

		//m_iUdeIndex = m_isLeft ? 34 : 70;

		//m_SeparationAnimComs[DEFAULT_ANIM]->Reset_Animation(m_iUdeIndex);
		//m_SeparationAnimComs[DEFAULT_ANIM]->Set_CurrentAnimIndex(m_iUdeIndex);
		break;
	}
	case CHighway_Kiryu::SWAP:
	{
		m_isStarted = false;
		break;
	}
	}

	m_eCurrentBehavior = eType;

	On_Separation_Hand(m_isLeft ? 1 : 2);
}

//몬스터가 제자리에 존재하지 않으면 공격 불가능
_bool CHighway_Kiryu::isAttackPossible()
{
	vector<CGameObject*> Reactors = m_pGameInstance->Get_GameObjects(m_iCurrentLevel, TEXT("Layer_Reactor"));

	for (auto& pReactor : Reactors)
	{
		CCarChase_Reactor* pMonster = dynamic_cast<CCarChase_Reactor*>(pReactor);
		if (pMonster->isStart())
			return false;
	}

	return true;
}

HRESULT CHighway_Kiryu::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_BoneCompute"),
		TEXT("Com_ComputeShader"), reinterpret_cast<CComponent**>(&m_pComputeShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Kiryu_CarChase"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_AABB;
	ColliderDesc.vExtents = _float3(1, 1, 1);
	ColliderDesc.vCenter = _float3(0, ColliderDesc.vExtents.y, 0);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	//Prototype_Component_Anim_KiryuFace
	CAnim* pAnimCom = { nullptr };
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim_KiryuFace"),
		TEXT("Com_Anim_Face"), reinterpret_cast<CComponent**>(&pAnimCom))))
		return E_FAIL;
	m_SeparationAnimComs.push_back(pAnimCom);

	//Prototype_Component_Anim_Hand
	pAnimCom = { nullptr };
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim_Hand"),
		TEXT("Com_Anim_Hand"), reinterpret_cast<CComponent**>(&pAnimCom))))
		return E_FAIL;
	m_SeparationAnimComs.push_back(pAnimCom);

	//Prototype_Component_Kiryu_CarChase
	pAnimCom = { nullptr };
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Kiryu_CarChase"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&pAnimCom))))
		return E_FAIL;
	m_SeparationAnimComs.push_back(pAnimCom);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Material_Kiryu"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHighway_Kiryu::Add_Objects()
{
	CGun_Cz75::CZ75_DESC Desc{};
	Desc.pParentMatrix = pTaxiMatrix;
	Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("buki_r_n");
	Desc.fRotatePecSec = XMConvertToRadians(90.f);
	Desc.fSpeedPecSec = 1.f;
	Desc.fLocalAngle = XMConvertToRadians(-90.f);
	Desc.iLocalRotAxis = 0;
	Desc.vLocalPos = _float3(0, 0.04, -0.03);
	m_pGun_R = dynamic_cast<CGun_Cz75*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Gun_Cz75"), &Desc));

	CGun_Cz75::CZ75_DESC Desc_L{};
	Desc_L.pParentMatrix = pTaxiMatrix;
	Desc_L.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("buki_l_n");
	Desc_L.fRotatePecSec = XMConvertToRadians(90.f);
	Desc_L.fSpeedPecSec = 1.f;
	Desc_L.fLocalAngle = XMConvertToRadians(-90.f);
	Desc_L.iLocalRotAxis = 0;
	Desc_L.vLocalPos = _float3(0, 0.04, -0.03);

	m_pGun_L = dynamic_cast<CGun_Cz75*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Gun_Cz75"), &Desc_L));

	return S_OK;
}

HRESULT CHighway_Kiryu::Bind_ResourceData()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_ModelMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

_bool CHighway_Kiryu::Shot()
{
	if (1 > m_iCurrentAmmo)		//0이면
	{
		m_iCurrentAmmo = 0;
		return false;
	} 

	if ((m_eCurrentBehavior == SHOT && Checked_Animation_Ratio(0.7f)) || (m_eCurrentBehavior != SHOT))
	{
		Change_Behavior(SHOT);
		m_iCurrentAmmo--;
		return true;
	}

	return false;
}

CHighway_Kiryu* CHighway_Kiryu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHighway_Kiryu* pInstance = new CHighway_Kiryu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CHighway_Kiryu::Clone(void* pArg)
{
	CHighway_Kiryu* pInstance = new CHighway_Kiryu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CHighway_Kiryu::Free()
{
	__super::Free();

	Safe_Release(m_pGun_R);
	Safe_Release(m_pGun_L);
}

string CHighway_Kiryu::Get_CurrentAnimationName()
{
	return m_pModelCom->Get_AnimationName(m_pModelCom->Get_CurrentAnimationIndex());
}

const _float4x4* CHighway_Kiryu::Get_BoneMatrix(const char* strBoneName)
{
	return m_pModelCom->Get_BoneCombinedTransformationMatrix(strBoneName);
}

const _float3* CHighway_Kiryu::Get_Pos()
{
	_float3 vPos;

	memcpy(&vPos, m_ModelMatrix.m[CTransform::STATE_POSITION], sizeof(_float3));
	return &vPos;
}

const _float3* CHighway_Kiryu::Get_Look()
{
	_float3 vLook;

	memcpy(&vLook, m_ModelMatrix.m[CTransform::STATE_LOOK], sizeof(_float3));
	return &vLook;
}

const _float3* CHighway_Kiryu::Get_Right()
{
	_float3 vRight;

	memcpy(&vRight, m_ModelMatrix.m[CTransform::STATE_RIGHT], sizeof(_float3));
	return &vRight;
}
