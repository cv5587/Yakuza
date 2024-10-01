#include "Monster.h"

#include "GameInstance.h"
#include "SystemManager.h"

#include "Camera.h"
#include "Mesh.h"
#include "AI_Monster.h";

#include "CharacterData.h"
#include "SocketCollider.h"
#include "SocketEffect.h"
#include "Collision_Manager.h"
#include "EffectManager.h"

#include "Player.h"	


CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext },
	m_pEffectManager{ CEffectManager::GetInstance() }
{
	Safe_AddRef(m_pEffectManager);
}

CMonster::CMonster(const CMonster& rhs)
	: CLandObject{ rhs },
	m_pEffectManager{ CEffectManager::GetInstance() }
{
	Safe_AddRef(m_pEffectManager);
}

_bool CMonster::isAttack()
{
	return m_pTree->isAttack();
}

void CMonster::Set_Sync(string strPlayerAnim, _bool isKeepSynchronizing)
{
	string_view strAnim = strPlayerAnim;

	m_isDown = true;
	m_isSynchronizing = true;
	m_isKeepSynchronizing = isKeepSynchronizing;

	//싱크 액션
	if (strAnim == string_view("p_krc_sync_guard_counter_f"))
		m_iState = MONSTER_KRC_SYNC1_GUARD_COUNTER_F;
	else if (strAnim == string_view("p_krc_sync_kaihi_nage_b"))
		m_iState = MONSTER_KRC_SYNC1_KAIHI_NAGE_B;
	else if (strAnim == string_view("p_krc_sync_kaihi_nage_f"))
		m_iState = MONSTER_KRC_SYNC1_KAIHI_NAGE_F;
	else if (strAnim == string_view("p_krc_sync_lapel_atk_heavy"))
		m_iState = MONSTER_KRC_SYNC1_LAPEL_ATK_HEAVY;
	else if (strAnim == string_view("p_krc_sync_lapel_atk_punch"))
		m_iState = MONSTER_KRC_SYNC1_LAPEL_ATK_PUNCH;
	else if (strAnim == string_view("p_krc_sync_lapel_nage"))
		m_iState = MONSTER_KRC_SYNC1_LAPEL_NAGE;
	else if (strAnim == string_view("p_krc_sync_neck_atk_heavy"))
		m_iState = MONSTER_KRC_SYNC1_NECK_ATK_HEAVY;
	else if (strAnim == string_view("p_krc_sync_neck_atk_punch"))
		m_iState = MONSTER_KRC_SYNC1_NECK_ATK_PUNCH;
	else if (strAnim == string_view("p_krc_sync_neck_nage"))
		m_iState = MONSTER_KRC_SYNC1_NECK_NAGE;
	else if (strAnim == string_view("p_krh_sync_guard_sabaki"))
	{
		m_iState = MONSTER_KRH_SYNC1_GUARD_SABAKI;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_krs_sync_cmb_03_fin"))
		m_iState = MONSTER_KRS_SYNC1_CMB_03_FIN;
	else if (strAnim == string_view("p_kru_sync_lapel_atk_punch"))
		m_iState = MONSTER_KRU_SYNC1_LAPEL_ATK_PUNCH;
	else if (strAnim == string_view("p_kru_sync_lapel_cmb_01"))
	{
		m_iState = MONSTER_KRU_SYNC1_LAPEL_CMB_01;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_kru_sync_lapel_cmb_02"))
	{
		m_iState = MONSTER_KRU_SYNC1_LAPEL_CMB_02;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_kru_sync_lapel_cmb_03"))
		m_iState = MONSTER_KRU_SYNC1_LAPEL_CMB_03;
	else if (strAnim == string_view("p_kru_sync_lapel_lp"))
	{
		m_iState = MONSTER_KRU_SYNC1_LAPEL_LP;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_kru_sync_lapel_nage"))
		m_iState = MONSTER_KRU_SYNC1_LAPEL_NAGE;
	else if (strAnim == string_view("p_kru_sync_lapel_off"))
	{
		m_iState = MONSTER_KRU_SYNC1_LAPEL_OFF;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_kru_sync_lapel_off_hiza"))
	{
		m_iState = MONSTER_KRU_SYNC1_LAPEL_OFF_HIZA;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_kru_sync_lapel_press"))
	{
		m_iState = MONSTER_KRU_SYNC1_LAPEL_PRESS;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_kru_sync_lapel_resist"))
	{
		m_iState = MONSTER_KRU_SYNC1_LAPEL_RESIST;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_kru_sync_lapel_st"))
	{
		m_iState = MONSTER_KRU_SYNC1_LAPEL_ST;
		m_isDown = false;
		m_isGrabStandup = true;
	}
	else if (strAnim == string_view("p_kru_sync_lapel_walk"))
	{
		m_iState = MONSTER_KRU_SYNC1_LAPEL_WALK;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_kru_sync_neck_atk_kick"))
		m_iState = MONSTER_KRU_SYNC1_NECK_ATK_KICK;
	else if (strAnim == string_view("p_kru_sync_neck_cmb_01"))
	{
		m_iState = MONSTER_KRU_SYNC1_NECK_CMB_01;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_kru_sync_neck_cmb_02"))
	{
		m_iState = MONSTER_KRU_SYNC1_NECK_CMB_02;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_kru_sync_neck_cmb_03"))
		m_iState = MONSTER_KRU_SYNC1_NECK_CMB_03;
	else if (strAnim == string_view("p_kru_sync_neck_lp"))
	{
		m_iState = MONSTER_KRU_SYNC1_NECK_LP;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_kru_sync_neck_nage"))
		m_iState = MONSTER_KRU_SYNC1_NECK_NAGE;
	else if (strAnim == string_view("p_kru_sync_neck_off"))
	{
		m_iState = MONSTER_KRU_SYNC1_NECK_OFF;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_kru_sync_neck_off_uraken"))
	{
		m_iState = MONSTER_KRU_SYNC1_NECK_OFF_URAKEN;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_kru_sync_neck_press"))
	{
		m_iState = MONSTER_KRU_SYNC1_NECK_PRESS;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_kru_sync_neck_resist"))
	{
		m_iState = MONSTER_KRU_SYNC1_NECK_RESIST;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_kru_sync_neck_st"))
	{
		m_iState = MONSTER_KRU_SYNC1_NECK_ST;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_kru_sync_neck_walk"))
	{
		m_iState = MONSTER_KRU_SYNC1_NECK_WALK;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_sync_counter_f"))
	{
		m_iState = MONSTER_SYNC1_COUNTER_F;
		m_isDown = false;
	}

	else if (strAnim == string_view("p_sync_head_b"))
	{
		m_iState = MONSTER_SYNC1_HEAD_B;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_sync_head_f"))
	{
		m_iState = MONSTER_SYNC1_HEAD_F;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_sync_lapel_to_neck"))
	{
		m_iState = MONSTER_SYNC1_LAPEL_TO_NECK;
		m_isDown = false;
	}
	else if (strAnim == string_view("p_sync_leg_atk_kick"))
		m_iState = MONSTER_SYNC1_LEG_ATK_KICK;
	else if (strAnim == string_view("p_sync_leg_lp"))
		m_iState = MONSTER_SYNC1_LEG_LP;
	else if (strAnim == string_view("p_sync_leg_nage"))
		m_iState = MONSTER_SYNC1_LEG_NAGE;
	else if (strAnim == string_view("p_sync_leg_off"))
		m_iState = MONSTER_SYNC1_LEG_OFF;
	else if (strAnim == string_view("p_sync_leg_st_b"))
		m_iState = MONSTER_SYNC1_LEG_ST_B;
	else if (strAnim == string_view("p_sync_leg_st_f"))
		m_iState = MONSTER_SYNC1_LEG_ST_F;
	else if (strAnim == string_view("p_sync_leg_walk"))
		m_iState = MONSTER_SYNC1_LEG_WALK;
	else if (strAnim == string_view("p_sync_neck_to_lapel"))
	{
		m_iState = MONSTER_SYNC1_NECK_TO_LAPEL;
		m_isDown = false;
	}

	if (m_isDown == true)
	{
		if (m_iMonsterType != KUZE && m_iState != MONSTER_KRU_SYNC1_LAPEL_NAGE)
		{
			m_pGameInstance->StopSound(SOUND_ENEMY_VOICE);
			m_pGameInstance->PlaySound_W(TEXT("46a0 [22].wav"), SOUND_ENEMY_VOICE, DEFAULT_EFFECT_VOLUME);
		}
	}

	//컷 액션
	if (strAnim == string_view("h11250"))
		m_iState = MONSTER_H11250_000_1;
	else if (strAnim == string_view("h20021"))
		m_iState = MONSTER_H20021_000_2;
	else if (strAnim == string_view("h23000"))
		m_iState = MONSTER_H23000_000_6;
	else if (strAnim == string_view("a60300"))
		m_iState = MONSTER_A60300_000_2;
	else if (strAnim == string_view("a60320"))
		m_iState = MONSTER_A60320_000_4;
	else if (strAnim == string_view("a60330"))
		m_iState = MONSTER_A60330_000_2;
	else if (strAnim == string_view("a60350"))
		m_iState = MONSTER_A60350_000_2;
	else if (strAnim == string_view("h1010"))
		m_iState = MONSTER_H1010_000_1;
	else if (strAnim == string_view("h10111"))
		m_iState = MONSTER_H10111_000_1;
	else if (strAnim == string_view("h11285"))
		m_iState = MONSTER_H11285_000_1;
	else if (strAnim == string_view("h1511"))
	{
		m_isDown = true;
		m_iState = MONSTER_H1511_000_4;
	}
	else if (strAnim == string_view("h1540"))
		m_iState = MONSTER_H1540_000_1;
	else if (strAnim == string_view("h1620"))
		m_iState = MONSTER_H1620_000_1;
	else if (strAnim == string_view("h2011"))
		m_iState = MONSTER_H2011_000_1;
	else if (strAnim == string_view("h2040"))
		m_iState = MONSTER_H2040_000_1;
	else if (strAnim == string_view("h23010"))
	{
		m_iState = MONSTER_H23010_000_4;
		m_isDown = true;
	}
	else if (strAnim == string_view("h23020"))
	{
		m_iState = MONSTER_H23020_000_4;
		m_isDown = true;
	}
	else if (strAnim == string_view("h23060"))
		m_iState = MONSTER_H23060_000_4;
	else if (strAnim == string_view("h23070"))
		m_iState = MONSTER_H23070_000_4;
	else if (strAnim == string_view("h23250"))
		m_iState = MONSTER_H23250_000_2;
	else if (strAnim == string_view("h23320"))
		m_iState = MONSTER_H23320_000_4;
	else if (strAnim == string_view("h3261"))
		m_iState = MONSTER_H3261_000_4;
	else if (strAnim == string_view("h1500"))
	{
		m_iState = MONSTER_H1500_000_1;
		m_isDown = true;
	}

	m_strAnimName = strPlayerAnim;
	m_iCurrentAnimType = CMonster::CUTSCENE;

	m_pTree->Set_Sync(true);
	Change_Animation();

	m_vPlayerDistance = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(m_iCurrentLevel, TEXT("Layer_Player"), TEXT("Com_Transform"), 0))->Get_WorldMatrix().r[CTransform::STATE_POSITION];
}

void CMonster::Set_Adventure(_bool isAdventure)
{
	m_pTree->Set_Adventure(isAdventure);
}

void CMonster::Off_Sync()
{
	m_isSynchronizing = false;
	m_isKeepSynchronizing = false;
	m_iCurrentAnimType = CMonster::DEFAULT;

	Setting_SyncAnim_EndPosition();
}

void CMonster::Set_Start(_bool isStart)
{
	m_pTree->Set_Start(isStart);
}

void CMonster::Set_Animation(string strAnimName, _bool isLoop)
{
	m_iCurrentAnimType = CUTSCENE;
	m_strAnimName = strAnimName;
	m_isAnimLoop = isLoop;

	if (FAILED(Setup_Animation()))
		return;
}

void CMonster::Reset_Monster()
{
	//테스트 데이터
	m_Info.fMaxHP = 100.f;
	m_Info.fHp = m_Info.fMaxHP;

	m_iCurrentAnimType = DEFAULT;
	m_iPreAnimType = DEFAULT;

	m_pModelCom->Set_PreAnimations(m_pAnimCom[m_iPreAnimType]->Get_Animations());

	m_isDead = false;
	m_isObjectDead = false;

	m_pTree->Reset_AI();
}

void CMonster::Setting_SyncAnim_EndPosition()
{
	// 싱크 끝낼 때 위치 잡아줌.
	_matrix mat = XMLoadFloat4x4(m_pModelCom->Get_BoneCombinedTransformationMatrix("center_c_n"));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, (mat * m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Player"), 0)->Get_TransformCom()->Get_WorldMatrix()).r[CTransform::STATE_POSITION]);
}

/*
* DIR_F : 앞으로 누워잇음
* DIR_B : 뒤로 엎어져잇음
* DIR_END : 방향을 가져오지 못함
*/
_uint CMonster::Get_DownDir()
{
	if (m_isDown = false)
		return DIR_END;

	return m_pTree->Get_DownDir();
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MONSTER_IODESC* gameobjDesc = (MONSTER_IODESC*)pArg;
	m_pTransformCom->Set_WorldMatrix(gameobjDesc->vStartPos);
	m_wstrModelName = gameobjDesc->wstrModelName;
	m_iNaviRouteNum = gameobjDesc->iNaviRouteNum;
	m_iNPCDirection = gameobjDesc->iNPCDirection;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (nullptr != m_pNavigationCom)
		m_pNavigationCom->Set_Index(gameobjDesc->iNaviNum);

	m_pTransformCom->Set_Scale(0.95f, 0.95f, 0.95f);

	//테스트 데이터
	m_Info.fMaxHP = 70.f;
	m_Info.fHp = m_Info.fMaxHP;

	m_iCurrentAnimType = DEFAULT;
	m_iPreAnimType = DEFAULT;

	m_pModelCom->Set_PreAnimations(m_pAnimCom[m_iPreAnimType]->Get_Animations());

	return S_OK;
}

void CMonster::Priority_Tick(const _float& fTimeDelta)
{
}

void CMonster::Tick(const _float& fTimeDelta)
{
	// 싱크액션 맞추는중에는 플레이어의 0.0에 맞춰줘야해서 그거 맞춰주는 코드
	if (m_isSynchronizing)
	{
		if (m_strAnimName == "p_kru_sync1_lapel_st")
		{
			_matrix PlayerMat = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(m_iCurrentLevel, TEXT("Layer_Player"), TEXT("Com_Transform"), 0))->Get_WorldMatrix();

			_matrix TransMat = PlayerMat;

			_vector vTransDistance = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - PlayerMat.r[CTransform::STATE_POSITION];

			TransMat.r[CTransform::STATE_POSITION] = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_vPlayerDistance - vTransDistance);
			m_pTransformCom->Set_WorldMatrix(TransMat);
		}
		else
		{
			CTransform* pTrasnform = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(m_iCurrentLevel, TEXT("Layer_Player"), TEXT("Com_Transform"), 0));
			m_pTransformCom->Set_WorldMatrix(pTrasnform->Get_WorldMatrix());
		}

		if (!m_isKeepSynchronizing && m_pAnimCom[m_iCurrentAnimType]->Get_AnimFinished())
		{
			Setting_SyncAnim_EndPosition();
			m_isSynchronizing = false;
		}
	}

	if (!m_isScript)
	{
		m_pTree->Tick(fTimeDelta);

		Change_Animation(); //애니메이션 변경
	}

	_bool isRoot = m_iCurrentAnimType != CUTSCENE;

	if (CMonster::CUTSCENE == m_iCurrentAnimType)
	{
		if (m_strAnimName == "p_kru_sync1_lapel_st")
			m_pModelCom->Play_Animation_CutScene(fTimeDelta, m_pAnimCom[m_iCurrentAnimType], false, -1, true);
		else
			m_pModelCom->Play_Animation_CutScene(fTimeDelta, m_pAnimCom[m_iCurrentAnimType], m_isAnimLoop, -1, false);
	}
	else
	{
		m_pModelCom->Play_Animation_Monster(fTimeDelta, m_pAnimCom[m_iCurrentAnimType], m_isAnimLoop, isRoot);
	}

	if(m_strAnimName != "e_pnc_stand")
		Synchronize_Root(fTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	//충돌처리 초기화
	m_isColl = false;

	for (auto& pCollider : m_pColliders)
		pCollider.second->Tick(fTimeDelta);

	for (auto& pEffect : m_pEffects)
		pEffect.second->Tick(fTimeDelta);

	for (auto& pEffect : m_pBloodEffects)
		pEffect.second->Tick(fTimeDelta);

	Animation_Event();
	BloodEffect_Event();

	// 현재 켜져있는 Attack용 콜라이더 삽입
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::ATTACK && pPair.second->IsOn())
			m_pCollisionManager->Add_AttackCollider(pPair.second, CCollision_Manager::ENEMY);
	}

	// 현재 켜져있는 Hit용 콜라이더 삽입 (아직까지는 Hit용 콜라이더는 항상 켜져있음)
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::HIT && pPair.second->IsOn())
			m_pCollisionManager->Add_HitCollider(pPair.second, CCollision_Manager::ENEMY);
	}
}

void CMonster::Late_Tick(const _float& fTimeDelta)
{
	if (m_iPreAnimType == m_iCurrentAnimType)
		m_iPreAnimType = m_iCurrentAnimType;

	//컬링
	//if (m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.5f))
	//{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	if (!m_isSynchronizing)
		m_pCollisionManager->Add_ImpulseResolution(this);

	// 현재 켜져있는 Attack용 콜라이더 삽입
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::ATTACK && pPair.second->IsOn())
			m_pCollisionManager->Add_AttackCollider(pPair.second, CCollision_Manager::ENEMY);
	}

	// 현재 켜져있는 Hit용 콜라이더 삽입 (아직까지는 Hit용 콜라이더는 항상 켜져있음)
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::HIT && pPair.second->IsOn())
			m_pCollisionManager->Add_HitCollider(pPair.second, CCollision_Manager::ENEMY);

	}

	//높이값 태우기
	_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (nullptr != m_pNavigationCom)
	{
		float fHeight = m_pNavigationCom->Compute_Height(vCurrentPos);
		vCurrentPos = XMVectorSetY(vCurrentPos, fHeight);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPos);
	}

	for (auto& pCollider : m_pColliders)
		pCollider.second->Late_Tick(fTimeDelta);

	for (auto& pEffect : m_pEffects)
		pEffect.second->Late_Tick(fTimeDelta);

	for (auto& pEffect : m_pBloodEffects)
		pEffect.second->Late_Tick(fTimeDelta);

}

HRESULT CMonster::Render()
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

		if (!strcmp("[l0]jacketw1", pMesh->Get_Name()))
		{
			if (m_isRimLight)
				if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &m_isRimLight, sizeof(_bool))))
					return E_FAIL;
		}
		else if (!strcmp("[l0]body_naked1", pMesh->Get_Name()))
		{
			if (m_isRimLight)
				if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &m_isRimLight, sizeof(_bool))))
					return E_FAIL;
		}
		else
		{
			_bool isfalse = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &isfalse, sizeof(_bool))))
				return E_FAIL;
		}

		if (nullptr != m_pMaterialCom)
		{
			if (FAILED(m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(pMesh->Get_MaterialIndex()))))
				return E_FAIL;
		}

		_float fFar = *m_pGameInstance->Get_CamFar();
		m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

		/*	if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
				return E_FAIL;*/

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

/*
	1인자 : 어디에 맞았는지 전달받을 수 있음 (CSocketCollider::COLLIDER_PART_TYPE)
	2인자 : 이전 틱과 현재 틱의 방향
	3인자 : 데미지 값
	4인자 : 때리는 상태가 누군가?
	5인자 : 넘어지는 어택인가?
*/

void CMonster::Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack)
{
	//스웨이를 사용하고 있을 경우 충돌 x
	if (m_pTree->isSway())
		return;

	//튜토리얼 용 (카운트용)
	if (m_iCurrentLevel == LEVEL_TUTORIAL || m_iCurrentLevel == LEVEL_TEST)
		m_isTutorialAttack = true;

	//하는역활 -> 충돌이 일어났을때?
	m_isColl = true;
	m_fHitDamage = fDamage;


	//데미지 처리하기 (가드사용하고있지 않을때)
	if (!m_isObjectDead && !m_pTree->isGuard())
	{
		if (dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Player"), 0))->isGrab()) return;				// 그랩시에는 데미지X

		m_Info.fHp -= fDamage;
		if (m_Info.fHp <= 0.f)
			m_isObjectDead = true;
	}
}

void CMonster::Animation_Event()
{
	auto& pCurEvents = m_pData->Get_CurrentEvents();
	for (auto& pEvent : pCurEvents)
	{
		_double CurPos = *(m_pAnimCom[m_iCurrentAnimType]->Get_AnimPosition());
		_double Duration = *(m_pAnimCom[m_iCurrentAnimType]->Get_AnimDuration());

		if (CurPos >= pEvent.fPlayPosition && CurPos < Duration)
		{
			CSocketCollider* pCollider = m_pColliders.at(pEvent.iBoneIndex);

			switch (pEvent.iType)
			{
			case 0:
				pCollider->On();
				break;
			case 1:
				pCollider->Off();
				break;
			case 2:
#ifdef _DEBUG
				cout << "사운드 재생" << endl;
#endif // _DEBUG
				break;
			case 3:
#ifdef _DEBUG
				cout << "이펙트 재생" << endl;
#endif // _DEBUG
				break;
			}
		}

	}
}

string CMonster::Get_CurrentAnimationName()
{
	return m_strAnimName;
}

void CMonster::Shaking(_float fRatio, _float fShakeDuration, _float fShakeMagnitude)
{
	if (!m_isShaked && Checked_Animation_Ratio(fRatio))
	{
		m_isShaked = true;
		CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
		pCamera->Set_Shaking(true, { 1.f, 1.f, 0.f }, fShakeDuration, fShakeMagnitude);
	}
}

_bool CMonster::Checked_Animation_Ratio(_float fRatio)
{
	if (fRatio < *m_pModelCom->Get_AnimationCurrentPosition(m_pAnimCom[m_iCurrentAnimType]) / *m_pModelCom->Get_AnimationDuration(m_pAnimCom[m_iCurrentAnimType]))
		return true;

	return false;
}

void CMonster::BloodEffect_Event()
{
	if (m_iCurrentAnimType == CUTSCENE)
	{
		auto& pCurEvents = m_pData->Get_Current_BloodEffectEvents();
		for (auto& pEvent : pCurEvents)
		{
			_double CurPos = *m_pModelCom->Get_AnimationCurrentPosition(m_pAnimCom[m_iCurrentAnimType]);
			_double Duration = *m_pModelCom->Get_AnimationDuration(m_pAnimCom[m_iCurrentAnimType]);

			if (CurPos >= pEvent.fAinmPosition && CurPos < Duration)
			{
				CEffect::EFFECT_DESC EffectDesc;

				_float4x4 worldMat;

				_uint iBoneIndex = pEvent.iBoneIndex;
				_matrix BoneMatrix = XMLoadFloat4x4(m_pModelCom->Get_BoneCombinedTransformationMatrix_AtIndex(iBoneIndex));
				XMStoreFloat4x4(&worldMat, (BoneMatrix * m_pTransformCom->Get_WorldMatrix()));
				EffectDesc.pWorldMatrix = &worldMat;

				if (pEvent.isLoop)
				{
					if (pEvent.isOn)
					{
						// 해당하는 이펙트를 켜는 함수
						m_pBloodEffects.find(pEvent.strBonelName)->second->On();
					}
					else
					{
						//해당하는 이펙트를 끄는 함수
						m_pBloodEffects.find(pEvent.strBonelName)->second->Off();
					}
				}
				else                                  // 루프가 아닌 이펙트라면
				{
					if (!pEvent.isPlayed)
					{
						pEvent.isPlayed = true;
						m_pEffectManager->Cine_BloodEffect(EffectDesc, pEvent.iBloodEffectType);
					}
				}
			}
			else if (CurPos >= Duration)
			{
				pEvent.isPlayed = false;
			}
		}
	}
}

void CMonster::Sound_Event()
{
	auto& pCurEvents = m_pData->Get_Current_SoundEvents();
	for (auto& pEvent : pCurEvents)
	{
		_double CurPos = *(m_pModelCom->Get_AnimationCurrentPosition(m_pAnimCom[m_iCurrentAnimType]));
		_double Duration = *(m_pModelCom->Get_AnimationDuration(m_pAnimCom[m_iCurrentAnimType]));

		if (!pEvent->isPlayed && CurPos >= pEvent->fAinmPosition && CurPos < Duration)
		{
			pEvent->isPlayed = true;
			wstring wstrSoundFile = m_pGameInstance->StringToWstring(pEvent->strSoundFileName);
			m_pGameInstance->StopAndPlaySound(wstrSoundFile, static_cast<CHANNELID>(pEvent->iChannel), pEvent->fSoundVolume);
		}
	}
}

void CMonster::Synchronize_Root(const _float& fTimeDelta)
{
	_vector vFF = XMVectorSetZ(XMLoadFloat3(m_pModelCom->Get_AnimationCenterMove(m_pAnimCom[m_iCurrentAnimType])), 0);

	// m_pModelCom->Get_AnimChanged()  선형보간이 끝났는지
	// m_pModelCom->Get_AnimLerp() 선형보간이 필요한 애니메이션인지
	if (m_pAnimCom[m_iCurrentAnimType]->Get_AnimChanged() || !m_pAnimCom[m_iCurrentAnimType]->Get_AnimLerp(m_fChangeInterval))
	{
		if (m_pAnimCom[m_iCurrentAnimType]->Get_AnimRestart(m_isAnimLoop))
		{
			XMStoreFloat4(&m_vPrevMove, XMVectorZero());
			m_fPrevSpeed = 0.f;
		}
		else
		{
			_float4 fMoveDir;
			_float fMoveSpeed = XMVectorGetX(XMVector3Length(vFF - XMLoadFloat4(&m_vPrevMove)));

			//Y값 이동을 죽인 방향으로 적용해야한다.
			_vector vTemp = XMVector3Normalize((vFF - XMLoadFloat4(&m_vPrevMove)));

			//Z가 Y처럼 쓰임
			vTemp = XMVectorSetZ(vTemp, XMVectorGetY(vTemp));
			XMStoreFloat4(&fMoveDir, XMVector3TransformNormal(XMVectorSetY(vTemp, 0.f), m_pTransformCom->Get_WorldMatrix()));

			if (0.01 > m_fPrevSpeed)
				m_fPrevSpeed = 0.f;

			m_pTransformCom->Go_Move_Custum(fMoveDir, m_fPrevSpeed, 1, m_pNavigationCom);
			m_fPrevSpeed = fMoveSpeed;

			XMStoreFloat4(&m_vPrevMove, vFF);
		}
	}
	else
	{
		// 선형보간중일때는 무조건 초기화
		XMStoreFloat4(&m_vPrevMove, XMVectorZero());
	}

	XMStoreFloat4x4(&m_ModelWorldMatrix, m_pTransformCom->Get_WorldMatrix());
}

HRESULT CMonster::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_BoneCompute"),
		TEXT("Com_ComputeShader"), reinterpret_cast<CComponent**>(&m_pComputeShaderCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_AABB;
	ColliderDesc.vExtents = _float3(0.5, 0.8, 0.5);
	ColliderDesc.vCenter = _float3(0, ColliderDesc.vExtents.y, 0);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom[DEFAULT]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_SyncAnim"),
		TEXT("Com_SyncAnim"), reinterpret_cast<CComponent**>(&m_pAnimCom[CUTSCENE]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim_NPC"),
		TEXT("Com_AdventureAnim"), reinterpret_cast<CComponent**>(&m_pAnimCom[ADVENTURE]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	/*머테리얼 넣어주기*/

	string strModelName = m_pGameInstance->WstringToString(m_wstrModelName);
	string strRemoveName = "Prototype_Component_Model_";
	_int iPos = strModelName.find(strRemoveName);

	//if (iPos == string::npos)
	//	return E_FAIL;

	//strModelName = strModelName.erase(iPos, strRemoveName.size());

	wstring strMaterialName = TEXT("Prototype_Component_Material_") + m_pGameInstance->StringToWstring(strModelName);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, strMaterialName,
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		m_pMaterialCom = nullptr;

	return S_OK;
}

HRESULT CMonster::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &m_isRimLight, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CMonster::Change_Animation()
{
	m_isAnimLoop = false;
	//히트, 데미지 관련 공통 애니메이션
	switch (m_iState)
	{
	case MONSTER_ADVENTURE_IDLE_1:
	{
		m_strAnimName = "m_nml_set_stand_listen_01";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_ADVENTURE_IDLE_2:
	{
		m_strAnimName = "m_nml_set_stand_listen_02";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_ADVENTURE_IDLE_3:
	{
		m_strAnimName = "m_nml_set_stand_lookfor_02";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_ADVENTURE_IDLE_4:
	{
		m_strAnimName = "m_nml_set_stand_lookfor";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_DWN_DNF_BOUND:
	{
		m_strAnimName = "c_dwn_dnb_bound";
		break;
	}
	case MONSTER_DWN_DNB_BOUND:
	{
		m_strAnimName = "c_dwn_dnf_bound";
		break;
	}
	case MONSTER_DWN_DNF_BOUND_G:
	{
		m_strAnimName = "c_dwn_dnb_bound_g";
		break;
	}
	case MONSTER_DWN_DNB_BOUND_G:
	{
		m_strAnimName = "c_dwn_dnf_bound_g";
		break;
	}
	case MONSTER_DAM_HEAD_LV01_R:
	{
		m_strAnimName = "c_dam_head_lv01_r";
		break;
	}
	case MONSTER_DAM_HEAD_LV01_L:
	{
		m_strAnimName = "c_dam_head_lv01_l";
		break;
	}
	case MONSTER_DAM_HEAD_LV01_F:
	{
		m_strAnimName = "c_dam_head_lv01_f";
		break;
	}
	case MONSTER_DAM_HEAD_LV01_B:
	{
		m_strAnimName = "c_dam_head_lv01_b";
		break;
	}
	case MONSTER_DAM_HEAD_LV02_R:
	{
		m_strAnimName = "c_dam_head_lv02_r";
		break;
	}
	case MONSTER_DAM_HEAD_LV02_L:
	{
		m_strAnimName = "c_dam_head_lv02_l";
		break;
	}
	case MONSTER_DAM_HEAD_LV02_F:
	{
		m_strAnimName = "c_dam_head_lv02_f";
		break;
	}
	case MONSTER_DAM_HEAD_LV02_B:
	{
		m_strAnimName = "c_dam_head_lv02_b";
		break;
	}
	case MONSTER_DAM_BODY_LV01_F:
	{
		m_strAnimName = "c_dam_body_lv01_f";
		break;
	}
	case MONSTER_DAM_BODY_LV01_B:
	{
		m_strAnimName = "c_dam_body_lv01_b";
		break;
	}
	case MONSTER_DAM_BODY_LV01_D:
	{
		m_strAnimName = "c_dam_body_lv01_b";
		break;
	}
	case MONSTER_DAM_BODY_LV02_F:
	{
		m_strAnimName = "c_dam_body_lv02_f";
		break;
	}
	case MONSTER_DAM_BODY_LV02_B:
	{
		m_strAnimName = "c_dam_body_lv02_b";
		break;
	}
	case MONSTER_DAM_BODY_LV02_D:
	{
		m_strAnimName = "c_dam_body_lv02_b";
		break;
	}
	case MONSTER_DWN_DIRECT_B:
	{
		m_strAnimName = "c_dwn_direct_b";
		break;
	}
	case MONSTER_DWN_DIRECT_F:
	{
		m_strAnimName = "c_dwn_direct_f";
		break;
	}
	case MONSTER_DWN_BODY_F:
	{
		m_strAnimName = "c_dwn_body_f";
		break;
	}
	case MONSTER_DWN_BODY_F_SP:
	{
		m_strAnimName = "c_dwn_body_f_sp";
		break;
	}
	case MONSTER_DWN_EXPLODE_F:
	{
		m_strAnimName = "c_dwn_explode_f";
		break;
	}
	case MONSTER_DWN_BODY_B:
	{
		m_strAnimName = "c_dwn_body_b";
		break;
	}
	case MONSTER_DWN_BODY_B_SP:
	{
		m_strAnimName = "c_dwn_body_b_sp";
		break;
	}
	case MONSTER_DWN_EXPLODE_B:
	{
		m_strAnimName = "c_dwn_explode_b";
		break;
	}
	case MONSTER_DWN_DIRECT_F_BOUND_G:
	{
		m_strAnimName = "c_dwn_direct_f_bound_g";
		break;
	}
	case MONSTER_DWN_DIRECT_B_BOUND_G:
	{
		m_strAnimName = "c_dwn_direct_b_bound_g";
		break;
	}
	case MONSTER_STANDUP_DNF_FAST:
	{
		m_strAnimName = "c_standup_dnf_fast";
		break;
	}
	case MONSTER_STANDUP_DNB_FAST:
	{
		m_strAnimName = "c_standup_dnb_fast";
		break;
	}
	case MONSTER_DWN_STANDUP_HEAD_R_EN:
	{
		m_strAnimName = "c_dwn_standup_head_r_en";
		break;
	}
	case MONSTER_ANGRY_START:
	{
		//e_angry_typec[e_angry_typec]
		m_strAnimName = "e_angry_typec";
		break;
	}
	case MONSTER_ANGRY_CHOP:
	{
		//e_knk_atk_chop[e_knk_atk_chop]
		m_strAnimName = "e_knk_atk_chop";
		Shaking(0.3, 0.2, 0.3);
		break;
	}
	case MONSTER_ANGRY_KICK:
	{
		//e_knk_atk_kick[e_knk_atk_kick]
		m_strAnimName = "e_knk_atk_kick";
		Shaking(0.3, 0.2, 0.3);
		break;
	}
	case MONSTER_DED_L:
	{
		m_strAnimName = "c_ded_l";
		break;
	}
	case MONSTER_DED_R:
	{
		m_strAnimName = "c_ded_r";
		break;
	}
	case MONSTER_DED_F_1:
	{
		m_strAnimName = "c_ded_f_01";
		break;
	}
	case MONSTER_DED_F_2:
	{
		m_strAnimName = "c_ded_f_02";
		break;
	}
	case MONSTER_DED_F_3:
	{
		m_strAnimName = "c_ded_f_03";
		break;
	}
	case MONSTER_DED_B_1:
	{
		m_strAnimName = "c_ded_b_01";
		break;
	}
	case MONSTER_DED_B_2:
	{
		m_strAnimName = "c_ded_b_02";
		break;
	}
	case MONSTER_KTA_SYNC1_ATK_TO_MOUNT_ST:
	{
		m_strAnimName = "e_kta_sync1_atk_to_mount_st";
		break;
	}
	case MONSTER_KTA_SYNC1_GURAD_REVERSE_EN:
	{
		m_strAnimName = "e_kta_sync1_gurad_reverse_en";
		break;
	}
	case MONSTER_KTA_SYNC1_GURAD_REVERSE_LP:
	{
		m_strAnimName = "e_kta_sync1_gurad_reverse_lp";
		break;
	}
	case MONSTER_KTA_SYNC1_GURAD_REVERSE_ST:
	{
		m_strAnimName = "e_kta_sync1_gurad_reverse_st";
		break;
	}
	case MONSTER_KTA_SYNC1_LAPEL_RESIST:
	{
		m_strAnimName = "e_kta_sync1_lapel_resist";
		break;
	}
	case MONSTER_KTA_SYNC1_LEG_NAGE_B:
	{
		m_strAnimName = "e_kta_sync1_leg_nage_b";
		break;
	}
	case MONSTER_KTA_SYNC1_LEG_NAGE_F:
	{
		m_strAnimName = "e_kta_sync1_leg_nage_f";
		break;
	}
	case MONSTER_KTA_SYNC1_NECK_RESIST:
	{
		m_strAnimName = "e_kta_sync1_neck_resist";
		break;
	}
	case MONSTER_KUZ_PIPE_SYNC1_ANTI_SWAY:
	{
		m_strAnimName = "e_kuz_pipe_sync1_anti_sway";
		break;
	}
	case MONSTER_KUZ_SYNC1_ANTI_SWAY:
	{
		m_strAnimName = "e_kuz_sync1_anti_sway";
		break;
	}
	case MONSTER_NSK_SYNC1_ANTIWPN_NAGE:
	{
		m_strAnimName = "e_nsk_sync1_antiwpn_nage";
		break;
	}
	case MONSTER_NSK_SYNC1_LAPEL_YA_NAGE:
	{
		m_strAnimName = "e_nsk_sync1_lapel_ya_nage";
		break;
	}
	case MONSTER_NSK_SYNC1_RUNTACKLE_NAGE:
	{
		m_strAnimName = "e_nsk_sync1_runtackle_nage";
		break;
	}
	case MONSTER_SAE_SYNC1_AIR_HEAD_B:
	{
		m_strAnimName = "e_sae_sync1_air_head_b";
		break;
	}
	case MONSTER_SAE_SYNC1_AIR_HEAD_F:
	{
		m_strAnimName = "e_sae_sync1_air_head_f";
		break;
	}
	case MONSTER_SAE_SYNC1_AIR_LEG_B:
	{
		m_strAnimName = "e_sae_sync1_air_leg_b";
		break;
	}
	case MONSTER_SAE_SYNC1_AIR_LEF_F:
	{
		m_strAnimName = "e_sae_sync1_air_leg_f";
		break;
	}
	case MONSTER_SAE_SYNC1_LAPEL_ATK_EN:
	{
		m_strAnimName = "e_sae_sync1_lapel_atk_en";
		break;
	}
	case MONSTER_SAE_SYNC1_LAPEL_ATK_TAME_EN:
	{
		m_strAnimName = "e_sae_sync1_lapel_atk_tame_en";
		break;
	}
	case MONSTER_SAE_SYNC1_LAPEL_ATK_TAME_ST:
	{
		m_strAnimName = "e_sae_sync1_lapel_atk_tame_st";
		break;
	}
	case MONSTER_SAE_SYNC1_LAPEL_CMB_3:
	{
		m_strAnimName = "e_sae_sync1_lapel_cmb_03";
		break;
	}
	case MONSTER_SAE_SYNC1_LAPEL_NAGE:
	{
		m_strAnimName = "e_sae_sync1_lapel_nage";
		break;
	}
	case MONSTER_SAE_SYNC1_LEG_ATK_EN:
	{
		m_strAnimName = "e_sae_sync1_leg_atk_en";
		break;
	}
	case MONSTER_SAE_SYNC1_LEG_ATK_TAME_EN:
	{
		m_strAnimName = "e_sae_sync1_leg_atk_tame_en";
		break;
	}
	case MONSTER_SAE_SYNC1_LEG_ATK_TAME_ST:
	{
		m_strAnimName = "e_sae_sync1_leg_atk_tame_st";
		break;
	}
	case MONSTER_SAE_SYNC1_LEG_CMB_01:
	{
		m_strAnimName = "e_sae_sync1_leg_cmb_01";
		break;
	}
	case MONSTER_SAE_SYNC1_LEG_CMB_02:
	{
		m_strAnimName = "e_sae_sync1_leg_cmb_02";
		break;
	}
	case MONSTER_SAE_SYNC1_LEG_CMB_03:
	{
		m_strAnimName = "e_sae_sync1_leg_cmb_03";
		break;
	}
	case MONSTER_SAE_SYNC1_LEG_NAGE:
	{
		m_strAnimName = "e_sae_sync1_leg_nage";
		break;
	}
	case MONSTER_SAE_SYNC1_NECK_ATK_EN:
	{
		m_strAnimName = "e_sae_sync1_neck_atk_en";
		break;
	}
	case MONSTER_SAE_SYNC1_NECK_ATK_TAME_EN:
	{
		m_strAnimName = "e_sae_sync1_neck_atk_tame_en";
		break;
	}
	case MONSTER_SAE_SYNC1_NECK_ATK_TAME_ST:
	{
		m_strAnimName = "e_sae_sync1_neck_atk_tame_st";
		break;
	}
	case MONSTER_SAE_SYNC1_NECK_CMB_03:
	{
		m_strAnimName = "e_sae_sync1_neck_cmb_03";
		break;
	}
	case MONSTER_SAE_SYNC1_NECK_NAGE:
	{
		m_strAnimName = "e_sae_sync1_neck_nage";
		break;
	}
	case MONSTER_SAE_SYNC1_RUN_B:
	{
		m_strAnimName = "e_sae_sync1_run_b";
		break;
	}
	case MONSTER_SAE_SYNC1_RUN_F:
	{
		m_strAnimName = "e_sae_sync1_run_f";
		break;
	}
	case MONSTER_KRC_SYNC1_GUARD_COUNTER_F:
	{
		m_strAnimName = "p_krc_sync1_guard_counter_f";
		break;
	}
	case MONSTER_KRC_SYNC1_KAIHI_NAGE_B:
	{
		m_strAnimName = "p_krc_sync1_kaihi_nage_b";
		break;
	}
	case MONSTER_KRC_SYNC1_KAIHI_NAGE_F:
	{
		m_strAnimName = "p_krc_sync1_kaihi_nage_f";
		break;
	}
	case MONSTER_KRC_SYNC1_LAPEL_ATK_HEAVY:
	{
		m_strAnimName = "p_krc_sync1_lapel_atk_heavy";
		break;
	}
	case MONSTER_KRC_SYNC1_LAPEL_ATK_PUNCH:
	{
		m_strAnimName = "p_krc_sync1_lapel_atk_punch";
		break;
	}
	case MONSTER_KRC_SYNC1_LAPEL_NAGE:
	{
		m_strAnimName = "p_krc_sync1_lapel_nage";
		break;
	}
	case MONSTER_KRC_SYNC1_NECK_ATK_HEAVY:
	{
		m_strAnimName = "p_krc_sync1_neck_atk_heavy";
		break;
	}
	case MONSTER_KRC_SYNC1_NECK_ATK_PUNCH:
	{
		m_strAnimName = "p_krc_sync1_neck_atk_punch";
		break;
	}
	case MONSTER_KRC_SYNC1_NECK_NAGE:
	{
		m_strAnimName = "p_krc_sync1_neck_nage";
		break;
	}
	case MONSTER_KRH_SYNC1_GUARD_SABAKI:
	{
		m_strAnimName = "p_krh_sync1_guard_sabaki";
		break;
	}
	case MONSTER_KRS_SYNC1_CMB_03_FIN:
	{
		m_strAnimName = "p_krs_sync1_cmb_03_fin";
		break;
	}
	case MONSTER_KRS_SYNC1_CMB_03_FIN_B:
	{
		m_strAnimName = "p_krs_sync1_cmb_03_fin_b";
		break;
	}
	case MONSTER_KRU_SYNC1_LAPEL_ATK_PUNCH:
	{
		m_strAnimName = "p_kru_sync1_lapel_atk_punch";
		break;
	}
	case MONSTER_KRU_SYNC1_LAPEL_CMB_01:
	{
		m_strAnimName = "p_kru_sync1_lapel_cmb_01";
		break;
	}
	case MONSTER_KRU_SYNC1_LAPEL_CMB_02:
	{
		m_strAnimName = "p_kru_sync1_lapel_cmb_02";
		break;
	}
	case MONSTER_KRU_SYNC1_LAPEL_CMB_03:
	{
		m_strAnimName = "p_kru_sync1_lapel_cmb_03";
		break;
	}
	case MONSTER_KRU_SYNC1_LAPEL_LP:
	{
		m_strAnimName = "p_kru_sync1_lapel_lp";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_KRU_SYNC1_LAPEL_NAGE:
	{
		m_strAnimName = "p_kru_sync1_lapel_nage";
		break;
	}
	case MONSTER_KRU_SYNC1_LAPEL_OFF:
	{
		m_strAnimName = "p_kru_sync1_lapel_off";
		break;
	}
	case MONSTER_KRU_SYNC1_LAPEL_OFF_HIZA:
	{
		m_strAnimName = "p_kru_sync1_lapel_off_hiza";
		break;
	}
	case MONSTER_KRU_SYNC1_LAPEL_PRESS:
	{
		m_strAnimName = "p_kru_sync1_lapel_press";
		break;
	}
	case MONSTER_KRU_SYNC1_LAPEL_RESIST:
	{
		m_strAnimName = "p_kru_sync1_lapel_resist";
		break;
	}
	case MONSTER_KRU_SYNC1_LAPEL_ST:
	{
		m_strAnimName = "p_kru_sync1_lapel_st";
		break;
	}
	case MONSTER_KRU_SYNC1_LAPEL_WALK:
	{
		m_strAnimName = "p_kru_sync1_lapel_walk";
		break;
	}
	case MONSTER_KRU_SYNC1_NECK_ATK_KICK:
	{
		m_strAnimName = "p_kru_sync1_neck_atk_kick";
		break;
	}
	case MONSTER_KRU_SYNC1_NECK_CMB_01:
	{
		m_strAnimName = "p_kru_sync1_neck_cmb_01";
		break;
	}
	case MONSTER_KRU_SYNC1_NECK_CMB_02:
	{
		m_strAnimName = "p_kru_sync1_neck_cmb_02";
		break;
	}
	case MONSTER_KRU_SYNC1_NECK_CMB_03:
	{
		m_strAnimName = "p_kru_sync1_neck_cmb_03";
		break;
	}
	case MONSTER_KRU_SYNC1_NECK_LP:
	{
		m_strAnimName = "p_kru_sync1_neck_lp";
		break;
	}
	case MONSTER_KRU_SYNC1_NECK_NAGE:
	{
		m_strAnimName = "p_kru_sync1_neck_nage";
		break;
	}
	case MONSTER_KRU_SYNC1_NECK_OFF:
	{
		m_strAnimName = "p_kru_sync1_neck_off";
		break;
	}
	case MONSTER_KRU_SYNC1_NECK_OFF_URAKEN:
	{
		m_strAnimName = "p_kru_sync1_neck_off_uraken";
		break;
	}
	case MONSTER_KRU_SYNC1_NECK_PRESS:
	{
		m_strAnimName = "p_kru_sync1_neck_press";
		break;
	}
	case MONSTER_KRU_SYNC1_NECK_RESIST:
	{
		m_strAnimName = "p_kru_sync1_neck_resist";
		break;
	}
	case MONSTER_KRU_SYNC1_NECK_ST:
	{
		m_strAnimName = "p_kru_sync1_neck_st";
		break;
	}
	case MONSTER_KRU_SYNC1_NECK_WALK:
	{
		m_strAnimName = "p_kru_sync_neck_walk";
		break;
	}
	case MONSTER_SYNC1_COUNTER_F:
	{
		m_strAnimName = "p_sync1_counter_f";
		break;
	}
	case MONSTER_SYNC1_HEAD_B:
	{
		m_strAnimName = "p_sync1_head_b";
		break;
	}
	case MONSTER_SYNC1_HEAD_F:
	{
		m_strAnimName = "p_sync1_head_f";
		break;
	}
	case MONSTER_SYNC1_LAPEL_TO_NECK:
	{
		m_strAnimName = "p_sync1_lapel_to_neck";
		break;
	}
	case MONSTER_SYNC1_LEG_ATK_KICK:
	{
		m_strAnimName = "p_sync1_leg_atk_kick";
		break;
	}
	case MONSTER_SYNC1_LEG_LP:
	{
		m_strAnimName = "p_sync1_leg_lp";
		break;
	}
	case MONSTER_SYNC1_LEG_NAGE:
	{
		m_strAnimName = "p_sync1_leg_nage";
		break;
	}
	case MONSTER_SYNC1_LEG_OFF:
	{
		m_strAnimName = "p_sync1_leg_off";
		break;
	}
	case MONSTER_SYNC1_LEG_ST_B:
	{
		m_strAnimName = "p_sync1_leg_st_b";
		break;
	}
	case MONSTER_SYNC1_LEG_ST_F:
	{
		m_strAnimName = "p_sync1_leg_st_f";
		break;
	}
	case MONSTER_SYNC1_LEG_WALK:
	{
		m_strAnimName = "p_sync1_leg_walk";
		break;
	}
	case MONSTER_SYNC1_NECK_TO_LAPEL:
	{
		m_strAnimName = "p_sync1_neck_to_lapel";
		break;
	}
	case MONSTER_H11250_000_1:
	{
		m_strAnimName = "h11250_000_3";
		break;
	}
	case MONSTER_H20021_000_2:
	{
		m_strAnimName = "h20021_000_4";
		break;
	}
	case MONSTER_H23000_000_6:
	{
		m_strAnimName = "h23000_000_4";
		break;
	}
	case MONSTER_A60300_000_2:
	{
		m_strAnimName = "a60300_000_4";
		break;
	}
	case MONSTER_A60320_000_4:
	{
		m_strAnimName = "a60320_000_6";
		break;
	}
	case MONSTER_A60330_000_2:
	{
		m_strAnimName = "a60330_000_4";
		break;
	}
	case MONSTER_A60350_000_2:
	{
		m_strAnimName = "a60350_000_4";
		break;
	}
	case MONSTER_H1010_000_1:
	{
		m_strAnimName = "h1010_000_3";
		break;
	}
	case MONSTER_H10111_000_1:
	{
		m_strAnimName = "h10111_000_3";
		break;
	}
	case MONSTER_H11285_000_1:
	{
		m_strAnimName = "h11285_000_3";
		break;
	}
	case MONSTER_H1511_000_4:
	{
		m_strAnimName = "h1511_000_6";
		break;
	}
	case MONSTER_H1540_000_1:
	{
		m_strAnimName = "h1540_000_3";
		break;
	}
	case MONSTER_H1620_000_1:
	{
		m_strAnimName = "h1620_000_3";
		break;
	}
	case MONSTER_H2011_000_1:
	{
		m_strAnimName = "h2011_000_3";
		break;
	}
	case MONSTER_H2040_000_1:
	{
		m_strAnimName = "h2040_000_3";
		break;
	}
	case MONSTER_H23010_000_4:
	{
		m_strAnimName = "h23010_000_6";
		break;
	}
	case MONSTER_H23020_000_4:
	{
		m_strAnimName = "h23020_000_6";
		break;
	}
	case MONSTER_H23060_000_4:
	{
		m_strAnimName = "h23060_000_6";
		break;
	}
	case MONSTER_H23070_000_4:
	{
		m_strAnimName = "h23070_000_6";
		break;
	}
	case MONSTER_H23250_000_2:
	{
		m_strAnimName = "h23250_000_4";
		break;
	}
	case MONSTER_H23320_000_4:
	{
		m_strAnimName = "h23320_000_6";
		break;
	}
	case MONSTER_H3261_000_4:
	{
		m_strAnimName = "h3261_000_6";
		break;
	}
	case MONSTER_H1500_000_1:
	{
		m_strAnimName = "h1500_000_3";
		break;
	}
	}
}

HRESULT CMonster::Setup_Animation()
{
	_uint iPrevAnimIndex = m_pAnimCom[m_iCurrentAnimType]->Get_CurrentAnimIndex();

	m_iAnim = m_pAnimCom[m_iCurrentAnimType]->Get_AnimationIndex(m_strAnimName.c_str());

	if (m_iAnim == -1)
		return E_FAIL;

	// 실제로 애니메이션 체인지가 일어났을 때 켜져있던 어택 콜라이더를 전
	// 부 끈다
	if (m_pModelCom->Set_AnimationIndex(m_iAnim, m_pAnimCom[m_iCurrentAnimType]->Get_Animations(), m_fChangeInterval))
	{
		// 잡혔다가 일어날 때에만 회전값 맞춰주기. (다른 상황에는 회전값 맞춰주지 않아도 됨)
		if (m_isGrabStandup && (m_strAnimName == "c_standup_dnf_fast" || m_strAnimName == "c_standup_dnb_fast"))
		{
			m_isGrabStandup = false;
			_vector vLastPos = XMLoadFloat3(m_pModelCom->Get_LastKeyframe_Position("center_c_n", m_pAnimCom[m_iPreAnimType], m_pAnimCom[m_iCurrentAnimType]->Get_PrevAnimIndex()));
			// 높이값 죽이기
			vLastPos = XMVectorSetY(vLastPos, 0);

			// 이전 애니메이션의 마지막 키프레임에서 위치를 꺼내, 사이각을 구한다.
			_vector vDir = XMVector3Normalize(vLastPos - XMVectorSet(0, 0, 0, 1));
			vDir = XMVector3TransformNormal(vDir, m_pTransformCom->Get_WorldMatrix());

			_float fTheta = acos(XMVectorGetX(XMVector3Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), vDir)));

			m_pTransformCom->Change_Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), fTheta);
		}

		// 실제 체인지가 일어났을 때, 이전 애님 인덱스를 저장해준다.
		m_pAnimCom[m_iCurrentAnimType]->Set_PrevAnimIndex(iPrevAnimIndex);

		m_pModelCom->Set_PreAnimations(m_pAnimCom[m_iPreAnimType]->Get_Animations());

		Off_Attack_Colliders();
		Reset_Shaking_Variable();
	}

	if (nullptr != m_pData)
		m_pData->Set_CurrentAnimation(m_strAnimName);

	return S_OK;
}

void CMonster::Free()
{
	__super::Free();

	for (auto& pAnim : m_pAnimCom)
		Safe_Release(pAnim);

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pEffectManager);
}
