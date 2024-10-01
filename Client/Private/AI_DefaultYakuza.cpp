#include "AI_DefaultYakuza.h"

#include "GameInstance.h"

#include "Sequance.h"
#include "Selector.h"
#include "LeafNode.h"

#include "Monster.h"

CAI_DefaultYakuza::CAI_DefaultYakuza()
	: CAI_Monster{}
{
}

CAI_DefaultYakuza::CAI_DefaultYakuza(const CAI_DefaultYakuza& rhs)
	: CAI_Monster{ rhs }
{
}

HRESULT CAI_DefaultYakuza::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_DefaultYakuza::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Tree();

	m_fDelayAttackDuration = m_pGameInstance->Get_Random(8.f, 10.f);
	m_iMonsterType = CMonster::MONSTER_DEFAULT;

	return S_OK;
}

void CAI_DefaultYakuza::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	this->Execute();
}

CBTNode::NODE_STATE CAI_DefaultYakuza::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_DefaultYakuza::Ready_Tree()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Sync
	CSequance* pSyncSeq = CSequance::Create();
	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::Chcek_Sync, this)));
#pragma endregion

#pragma region Death
	CSequance* pDownSeq = CSequance::Create();
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::Check_Down, this)));
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::StandUpAndDead, this)));

	CSelector* pDownSelector = CSelector::Create();
	pDownSelector->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::StandUp, this)));
	pDownSelector->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::Dead, this)));
	pDownSeq->Add_Children(pDownSelector);
#pragma endregion

#pragma region PlayerDown
	CSequance* pPlayerDownSeq = CSequance::Create();
	pPlayerDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::Check_PlayerDown, this)));
	pPlayerDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::ATK_Down, this)));
#pragma endregion

#pragma region Sway
	CSequance* pSwaySeq = CSequance::Create();
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::Check_Sway, this)));
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::Sway, this)));
#pragma endregion

#pragma region HIT/GUARD
	CSequance* pHitGuardSeq = CSequance::Create();
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::Check_Hit, this)));
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::HitAndGuard, this)));

	CSelector* pHitGuard = CSelector::Create();
	pHitGuard->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::Hit, this)));

	pHitGuardSeq->Add_Children(pHitGuard);
#pragma endregion

#pragma region Attack
	CSequance* pAttackSeq = CSequance::Create();
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::Check_Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::Attack, this)));

	CSelector* pAttackSelector = CSelector::Create();
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::ATK_Heavy, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::ATK_CMB, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::ATK_Kick, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::ATK_Run_Attck, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::ATK_Heavy_Renda, this)));

	pAttackSeq->Add_Children(pAttackSelector);
#pragma endregion

#pragma region Shift/Idle
	CSequance* pBreakSeq = CSequance::Create();
	pBreakSeq->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::Check_Break, this)));
	pBreakSeq->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::ShiftAndIdle, this)));

	CSelector* pBreakSelector = CSelector::Create();
	pBreakSelector->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::Shift, this)));
	pBreakSelector->Add_Children(CLeafNode::Create(bind(&CAI_DefaultYakuza::Idle, this)));

	pBreakSeq->Add_Children(pBreakSelector);
#pragma endregion

#pragma region Root
	pRoot->Add_Children(pSyncSeq);
	pRoot->Add_Children(pDownSeq);
	pRoot->Add_Children(pSwaySeq);
	pRoot->Add_Children(pHitGuardSeq);
	pRoot->Add_Children(pAttackSeq);
	pRoot->Add_Children(pBreakSeq);
#pragma endregion

	m_pRootNode = pRoot;
}

CBTNode::NODE_STATE CAI_DefaultYakuza::HitAndGuard()
{
	if (m_pThis->isColl())
	{
		//충돌되면 플레이어 공격인지 아닌지 체크가 풀림
		Reset_State();
		m_iSkill = SKILL_HIT;

		return CBTNode::SUCCESS;
	}
	else
	{
		//충돌하지 않은 상태에서 히트 모션이 끝나면?
		if (m_iSkill == SKILL_HIT && !m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
			return CBTNode::RUNNING;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_DefaultYakuza::Attack()
{
	if (m_isAttack) return CBTNode::SUCCESS;

	//화가 나지 않앗을때 스킬 선택 (임시)
	LookAtPlayer();
	Reset_State();

	static _uint iCount = rand() % 10;

	if (DistanceFromPlayer() > 5.f)
	{
		m_iSkill = SKILL_RUN_ATK;
	}
	else
	{
		if (iCount == 0 || iCount == 5)
			m_iSkill = SKILL_KICK;
		else if (iCount == 2 || iCount == 6 || iCount == 8)
			m_iSkill = SKILL_CMD;
		else if (iCount == 4 || iCount == 9)
			m_iSkill = SKILL_RENDA;
		else
			m_iSkill = SKILL_HEAVY;
	}

	iCount++;
	if (iCount >= 10)
		iCount = 0;


	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_DefaultYakuza::ATK_CMB()
{
	if (m_iSkill == SKILL_CMD && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_CMB_1 && *(m_pAnimCom[*m_pCurrentAnimType]->Get_AnimPosition()) >= 12.0)
			*m_pState = CMonster::MONSTER_CMB_2;
		else if (*m_pState == CMonster::MONSTER_CMB_2 && *(m_pAnimCom[*m_pCurrentAnimType]->Get_AnimPosition()) >= 12.0)
			*m_pState = CMonster::MONSTER_CMB_3;
		else if (*m_pState == CMonster::MONSTER_CMB_3 && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_CMD)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_CMB_1;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_DefaultYakuza::ATK_Kick()
{
	if (m_iSkill == SKILL_KICK && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_KICK && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}
		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_KICK)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_KICK;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_DefaultYakuza::ATK_Run_Attck()
{
	if (m_iSkill == SKILL_RUN_ATK && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_RUN_ATK && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}
		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_RUN_ATK)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_RUN_ATK;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_DefaultYakuza::ATK_Heavy()
{
	if (m_iSkill == SKILL_HEAVY && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_HEAVY_ATTACK && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}
		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_HEAVY)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_HEAVY_ATTACK;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_DefaultYakuza::ATK_Heavy_Renda()
{
	if (m_iSkill == SKILL_RENDA && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_HEAVY_RENDA && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}
		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_RENDA)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_HEAVY_RENDA;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CAI_DefaultYakuza* CAI_DefaultYakuza::Create()
{
	CAI_DefaultYakuza* pInstance = new CAI_DefaultYakuza();
	
	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_DefaultYakuza::Clone(void* pArg)
{
	CAI_DefaultYakuza* pInstance = new CAI_DefaultYakuza(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_DefaultYakuza::Free()
{
	__super::Free();
}
