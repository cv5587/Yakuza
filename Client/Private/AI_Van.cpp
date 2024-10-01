#include "AI_Van.h"

#include "GameInstance.h"

#include "Player.h"
#include "CarChase_Monster.h"

#include "LeafNode.h"
#include "Sequance.h"
#include "Selector.h"
#include "EffectManager.h"

#include "Highway_Taxi.h"

CAI_Van::CAI_Van()
	: CAI_CarChase{}
{
}

CAI_Van::CAI_Van(const CAI_Van& rhs)
	: CAI_CarChase{ rhs }
{
}

HRESULT CAI_Van::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_Van::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Tree();

	return S_OK;
}

void CAI_Van::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

CBTNode::NODE_STATE CAI_Van::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_Van::Ready_Tree()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Death
	CSequance* pDownSeq = CSequance::Create();
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Van::Check_Down, this)));
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Van::Dead, this)));
#pragma endregion

//#pragma region Sync
//	CSequance* pSyncSeq = CSequance::Create();
//	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_Van::Check_Sync, this)));
//	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_Van::Sync, this)));
//#pragma endregion

#pragma region HIT
	CSequance* pHitSeq = CSequance::Create();
	pHitSeq->Add_Children(CLeafNode::Create(bind(&CAI_Van::Check_Hit, this)));
	pHitSeq->Add_Children(CLeafNode::Create(bind(&CAI_Van::Hit, this)));
#pragma endregion

#pragma region Attack
	CSequance* pAttackSeq = CSequance::Create();
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_Van::Check_Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_Van::ATK_Shot, this)));
#pragma endregion

#pragma region Sit/Up
	CSequance* pSitSeq = CSequance::Create();
	pSitSeq->Add_Children(CLeafNode::Create(bind(&CAI_Van::Check_Sit, this)));
	pSitSeq->Add_Children(CLeafNode::Create(bind(&CAI_Van::SitAndUp, this)));
#pragma endregion

#pragma region Shift/Idle
	CSequance* pBreakSeq = CSequance::Create();
	pBreakSeq->Add_Children(CLeafNode::Create(bind(&CAI_Van::Idle, this)));
#pragma endregion

#pragma region Root
	pRoot->Add_Children(pDownSeq);
	//pRoot->Add_Children(pSyncSeq);
	pRoot->Add_Children(pHitSeq);
	pRoot->Add_Children(pAttackSeq);
	if (*m_pWeaponType == CCarChase_Monster::WPJ) pRoot->Add_Children(pSitSeq);
	pRoot->Add_Children(pBreakSeq);
#pragma endregion

	m_pRootNode = pRoot;
}

CBTNode::NODE_STATE CAI_Van::Dead()
{
	if (m_iSkill == SKILL_DEAD)
	{
		if (*m_pState == CCarChase_Monster::CARCHASE_DED && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_pThis->Set_Dead();

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (*m_pWeaponType != CCarChase_Monster::DRV)
		*m_pState = CCarChase_Monster::CARCHASE_DED;

	m_iSkill = SKILL_DEAD;

	//이펙트 붙이는 구간
	CEffect::EFFECT_DESC EffectDesc;
	
	_matrix WorldMatrix = XMLoadFloat4x4(m_pThis->Get_TransformCom()->Get_WorldFloat4x4());

	_float4x4 matrix;
	XMStoreFloat4x4(&matrix, WorldMatrix);
	EffectDesc.pWorldMatrix = &matrix;

	CEffectManager::GetInstance()->Car_Explosion(EffectDesc);

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Van::Hit()
{
	if (m_isHit)
	{
		if (m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isHit = false;
			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (*m_pWeaponType != CCarChase_Monster::DRV)
		*m_pState = CCarChase_Monster::CARCHASE_DAM_LV_2;
	else if (*m_pWeaponType == CCarChase_Monster::DRV)
		*m_pState = CCarChase_Monster::CARCHASE_SIT_DWN;

	Play_Hit();

	m_isHit = true;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Van::Sync()
{
	return CBTNode::NODE_STATE();
}

CBTNode::NODE_STATE CAI_Van::Check_Attack()
{
	if (m_isAttack) return CBTNode::SUCCESS;

	//무조건 FAIL인 경우	
	if (*m_pWeaponType == CCarChase_Monster::DRV || (*m_pWeaponType == CCarChase_Monster::WPJ && m_isSit))
	{
		m_fAttackDelayTime = 0.f;
		return CBTNode::FAIL;
	}

	//m_isAttack이 false일 경우 공격을 할지 하지 않을지 결정한다.
	if (m_fDelayAttackDuration <= m_fAttackDelayTime)
	{
		m_iSkill = SKILL_SHOT;
		m_fAttackDelayTime = 0.f;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Van::ATK_Shot()
{
	if (m_iSkill == SKILL_SHOT && m_isAttack)
	{
		if (m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;
			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_SHOT)
	{
		m_isAttack = true;
		if (*m_pWeaponType == CCarChase_Monster::WPR)
		{
			*m_pState = CCarChase_Monster::CARCHASE_SHOT_ST;
			m_pGameInstance->PlaySound_W(TEXT("e_gunshot_shotgun_0.wav"), SOUND_ENEMY_EFFECT, 0.8f);
		}
		else if (*m_pWeaponType == CCarChase_Monster::WPJ)
		{
			*m_pState = CCarChase_Monster::CARCHASE_AIM_SHOT;
			m_pGameInstance->PlaySound_W(TEXT("e_gunshot_gatling_0.wav"), SOUND_ENEMY_EFFECT, 0.8f);
		}
			

		m_pPlayer->OnHit(5.f);

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

//일어나 있는지 아닌지 체크함 (WPJ)
CBTNode::NODE_STATE CAI_Van::Check_Sit()
{
	//일어나 있는 경우 특정한 조건을 만족하면 일어나게 함.
	if (m_isSit)
	{
		if (m_fUpDuration < m_fSitUpTime)
		{
			m_fSitUpTime = 0.f;
			return CBTNode::SUCCESS;
		}
			
	}
	else
	{
		if (m_fSitDuration < m_fSitUpTime)
		{
			m_fSitUpTime = 0.f;
			return CBTNode::SUCCESS;
		}
			
	}
	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Van::SitAndUp()
{
	if (m_isSit)
		*m_pState = CCarChase_Monster::CARCHASE_AIM_ST;
	else
		*m_pState = CCarChase_Monster::CARCHASE_AIM_EN;

	m_isSit = !m_isSit;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Van::Idle()
{
	if (*m_pWeaponType == CCarChase_Monster::WPJ)
	{
		if (!m_isSit) *m_pState = CCarChase_Monster::CARCHASE_AIM_LP;
	}	
	else if (*m_pWeaponType == CCarChase_Monster::WPR)
		*m_pState = CCarChase_Monster::CARCHASE_AIM_LP;
	else if (*m_pWeaponType == CCarChase_Monster::DRV)
		*m_pState = CCarChase_Monster::CARCHASE_SIT_LP;
		
	return CBTNode::SUCCESS;
}

CAI_Van* CAI_Van::Create()
{
	CAI_Van* pInstance = new CAI_Van();
	
	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_Van::Clone(void* pArg)
{
	CAI_Van* pInstance = new CAI_Van(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_Van::Free()
{
	__super::Free();
}
