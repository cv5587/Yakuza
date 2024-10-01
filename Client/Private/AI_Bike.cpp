#include "AI_Bike.h"

#include "GameInstance.h"

#include "Player.h"
#include "CarChase_Monster.h"

#include "LeafNode.h"
#include "Sequance.h"
#include "Selector.h"

#include "Highway_Taxi.h"

CAI_Bike::CAI_Bike()
	: CAI_CarChase{}
{
}

CAI_Bike::CAI_Bike(const CAI_Bike& rhs)
	: CAI_CarChase{ rhs }
{
}

HRESULT CAI_Bike::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_Bike::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Tree();

	return S_OK;
}

void CAI_Bike::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

CBTNode::NODE_STATE CAI_Bike::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_Bike::Ready_Tree()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Death
	CSequance* pDownSeq = CSequance::Create();
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Bike::Check_Down, this)));
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Bike::Dead, this)));
#pragma endregion

#pragma region HIT
	CSequance* pHitSeq = CSequance::Create();
	pHitSeq->Add_Children(CLeafNode::Create(bind(&CAI_Bike::Check_Hit, this)));
	pHitSeq->Add_Children(CLeafNode::Create(bind(&CAI_Bike::Hit, this)));
#pragma endregion

#pragma region Attack
	CSequance* pAttackSeq = CSequance::Create();
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_Bike::Check_Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_Bike::ATK_Shot, this)));
#pragma endregion

#pragma region Ready
	CSequance* pReadySeq = CSequance::Create();
	pReadySeq->Add_Children(CLeafNode::Create(bind(&CAI_Bike::Check_Ready, this)));
	pReadySeq->Add_Children(CLeafNode::Create(bind(&CAI_Bike::Ready, this)));
#pragma endregion

#pragma region Root
	pRoot->Add_Children(pDownSeq);
	//pRoot->Add_Children(pSyncSeq);
	pRoot->Add_Children(pHitSeq);
	pRoot->Add_Children(pAttackSeq);
	pRoot->Add_Children(pReadySeq);
#pragma endregion

	m_pRootNode = pRoot;
}

CBTNode::NODE_STATE CAI_Bike::Dead()
{
	if (m_iSkill == SKILL_DEAD)
	{
		if ((*m_pState == CCarChase_Monster::CARCHASE_DED_L || *m_pState == CCarChase_Monster::CARCHASE_TENTOU_B) && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_pThis->Set_Dead();

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	m_iSkill = SKILL_DEAD;

	if (*m_pWeaponType == CCarChase_Monster::RKT && !m_pThis->isReactorDead())
		*m_pState = CCarChase_Monster::CARCHASE_DED_L;
	else
		*m_pState = CCarChase_Monster::CARCHASE_TENTOU_B;


	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Bike::Hit()
{
	if (m_isHit)
	{
		if (*m_pState == CCarChase_Monster::CARCHASE_DAM_LV_2 && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isHit = false;
			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	*m_pState = CCarChase_Monster::CARCHASE_DAM_LV_2;
	m_isHit = true;

	Play_Hit();

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Bike::Check_Attack()
{
	if (m_isAttack) return CBTNode::SUCCESS;

	if (*m_pWeaponType == CCarChase_Monster::DRV || !m_isAtkReady)
	{
		m_fAttackDelayTime = 0.f;
		return CBTNode::FAIL;
	}

	if (m_fDelayAttackDuration <= m_fAttackDelayTime)
	{
		m_iSkill = SKILL_SHOT;
		m_fAttackDelayTime = 0.f;
		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Bike::ATK_Shot()
{
	if (m_iSkill == SKILL_SHOT && m_isAttack)
	{
		if (*m_pWeaponType == CCarChase_Monster::RKT)
		{
			if (m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
			{
				m_isAttack = false;
				m_isAtkReady = false;
				m_fAttackReadyDelayTime = 0.f;
				return CBTNode::SUCCESS;
			}
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_SHOT)
	{
		m_isAttack = true;

		if (*m_pWeaponType == CCarChase_Monster::RKT)
		{
			if (*m_pDir == DIR_L)
				*m_pState = CCarChase_Monster::CARCHASE_AIM_SHOT_L;
			else if (*m_pDir == DIR_R)
				*m_pState = CCarChase_Monster::CARCHASE_AIM_SHOT_R;

			m_pGameInstance->PlaySound_W(TEXT("e_gunshot_rocket_0.wav"), SOUND_ENEMY_EFFECT, DEFAULT_EFFECT_VOLUME);
		}
		else if (*m_pWeaponType == CCarChase_Monster::GUN)
		{
			if (*m_pDir == DIR_F)
				*m_pState = CCarChase_Monster::CARCHASE_AIM_SHOT_F;

			_uint iDir = AngleFromPlayer();
			if (iDir == DIR_R)
				*m_pState = CCarChase_Monster::CARCHASE_AIM_SHOT_R90;
			else if (iDir == DIR_FR)
				*m_pState = CCarChase_Monster::CARCHASE_AIM_SHOT_R;
			else if (iDir == DIR_L)
				*m_pState = CCarChase_Monster::CARCHASE_AIM_SHOT_L90;
			else if (iDir == DIR_FL)
				*m_pState = CCarChase_Monster::CARCHASE_AIM_SHOT_L;

			m_pGameInstance->PlaySound_W(TEXT("e_gunshot_handgun_0.wav"), SOUND_ENEMY_EFFECT, DEFAULT_EFFECT_VOLUME);
		}

		m_pPlayer->OnHit(5.f);

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Bike::Check_Ready()
{
	if (m_fDelayAttackReadyDuration < m_fAttackReadyDelayTime)
	{
		//true이면 Ready 준비하는 중
		m_isAtkReady = !m_isAtkReady;
		m_fAttackReadyDelayTime = 0.f;
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Bike::Ready()
{
	if (m_isAtkReady && *m_pWeaponType != CCarChase_Monster::DRV)
	{
		if (*m_pWeaponType == CCarChase_Monster::RKT)
		{
			if (*m_pDir == DIR_L)
				*m_pState = CCarChase_Monster::CARCHASE_AIM_L_LP;
			else if (*m_pDir == DIR_R)
				*m_pState = CCarChase_Monster::CARCHASE_AIM_R_LP;
		}
		else if (*m_pWeaponType == CCarChase_Monster::GUN)
		{
			if (*m_pDir == DIR_F)
				*m_pState = CCarChase_Monster::CARCHASE_AIM_F_LP;

			_uint iDir = AngleFromPlayer();
			if (iDir == DIR_R)
				*m_pState = CCarChase_Monster::CARCHASE_AIM_R90_LP;
			else if (iDir == DIR_FR)
				*m_pState = CCarChase_Monster::CARCHASE_AIM_R_LP;
			else if (iDir == DIR_L)
				*m_pState = CCarChase_Monster::CARCHASE_AIM_L90_LP;
			else if (iDir == DIR_FL)
				*m_pState = CCarChase_Monster::CARCHASE_AIM_L_LP;
		}
	}
	else
	{
		*m_pState = CCarChase_Monster::CARCHASE_STAND_UP;
	}


	return CBTNode::SUCCESS;
}

CAI_Bike* CAI_Bike::Create()
{
	CAI_Bike* pInstance = new CAI_Bike();

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_Bike::Clone(void* pArg)
{
	CAI_Bike* pInstance = new CAI_Bike(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_Bike::Free()
{
	__super::Free();
}
