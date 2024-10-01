#include "AI_Yoneda.h"

#include "GameInstance.h"

#include "Sequance.h"
#include "Selector.h"
#include "LeafNode.h"

#include "Monster.h"
#include "Yoneda.h"

CAI_Yoneda::CAI_Yoneda()
	: CAI_Monster{}
{
}

CAI_Yoneda::CAI_Yoneda(const CAI_Yoneda& rhs)
	: CAI_Monster{ rhs }
{
}

HRESULT CAI_Yoneda::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_Yoneda::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	AI_YONEDA_DESC* pDesc = static_cast<AI_YONEDA_DESC*>(pArg);
	m_pWeaponType = pDesc->pWeaponType;
	m_iMonsterType = CMonster::YONEDA;

	Ready_Tree();

	return S_OK;
}

void CAI_Yoneda::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	this->Execute();
}

CBTNode::NODE_STATE CAI_Yoneda::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_Yoneda::Ready_Tree()
{
	//특정 스테이지에서 칼로 공격함
	//나머지 스테이지에서는 default 몬스터와 공격 스킬이 동일함
	CSelector* pRoot = CSelector::Create();

#pragma region Sync
	CSequance* pSyncSeq = CSequance::Create();
	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::Chcek_Sync, this)));
#pragma endregion

#pragma region Death
	CSequance* pDownSeq = CSequance::Create();
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::Check_Down, this)));
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::StandUpAndDead, this)));

	CSelector* pDownSelector = CSelector::Create();
	pDownSelector->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::StandUp, this)));
	pDownSelector->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::Dead, this)));
	pDownSeq->Add_Children(pDownSelector);
#pragma endregion

#pragma region PlayerDown
	CSequance* pPlayerDownSeq = CSequance::Create();
	pPlayerDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::Check_PlayerDown, this)));
	pPlayerDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::ATK_Down, this)));
#pragma endregion

#pragma region Sway
	CSequance* pSwaySeq = CSequance::Create();
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::Check_Sway, this)));
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::Sway, this)));
#pragma endregion

#pragma region HIT/GUARD
	CSequance* pHitGuardSeq = CSequance::Create();
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::Check_Hit, this)));
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::HitAndGuard, this)));

	CSelector* pHitGuard = CSelector::Create();
	pHitGuard->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::Hit, this)));

	pHitGuardSeq->Add_Children(pHitGuard);
#pragma endregion

#pragma region Attack
	CSequance* pAttackSeq = CSequance::Create();
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::Check_Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::Attack, this)));

	CSelector* pAttackSelector = CSelector::Create();
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::ATK_Heavy, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::ATK_CMB, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::ATK_Kick, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::ATK_Run_Attck, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::ATK_Heavy_Renda, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::ATK_Normal, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::ATK_3Ren, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::ATK_Knife_CMB, this)));

	pAttackSeq->Add_Children(pAttackSelector);
#pragma endregion

#pragma region Shift/Idle
	CSequance* pBreakSeq = CSequance::Create();
	pBreakSeq->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::Check_Break, this)));
	pBreakSeq->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::ShiftAndIdle, this)));

	CSelector* pBreakSelector = CSelector::Create();
	pBreakSelector->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::Shift, this)));
	pBreakSelector->Add_Children(CLeafNode::Create(bind(&CAI_Yoneda::Idle, this)));

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

CBTNode::NODE_STATE CAI_Yoneda::Attack()
{
	if (m_isAttack) return CBTNode::SUCCESS;

	//화가 나지 않앗을때 스킬 선택 (임시)
	LookAtPlayer();
	Reset_State();

	static _uint iCount = rand() % 10;

	if (*m_pWeaponType == CYoneda::BAREHAND && DistanceFromPlayer() > 5.f)
	{
		m_iSkill = SKILL_RUN_ATK;
	}
	else
	{
		//맨손 일 경우
		if (*m_pWeaponType == CYoneda::BAREHAND)
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
		else if (*m_pWeaponType == CYoneda::KNIFE)
		{
			//칼을 든 경우
			if (iCount == 0 || iCount == 5)
				m_iSkill = SKILL_RENDA;
			else if (iCount == 2 || iCount == 6 || iCount == 8)
				m_iSkill = SKILL_CMD;
			else
				m_iSkill = SKILL_ATK_NORMAL;
		}

	}

	iCount++;
	if (iCount >= 10)
		iCount = 0;


	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Yoneda::ATK_CMB()
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

CBTNode::NODE_STATE CAI_Yoneda::ATK_Kick()
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

CBTNode::NODE_STATE CAI_Yoneda::ATK_Run_Attck()
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

CBTNode::NODE_STATE CAI_Yoneda::ATK_Heavy()
{
	if (m_iSkill == SKILL_HEAVY && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_HEAVY_ATTACK && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;

			//진짜 끝
			return CBTNode::SUCCESS;
		}

		// 상태 지속을 위해 빠져나감
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

CBTNode::NODE_STATE CAI_Yoneda::ATK_Heavy_Renda()
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

CBTNode::NODE_STATE CAI_Yoneda::ATK_Normal()
{
	if (m_iSkill == SKILL_ATK_NORMAL && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_ATK_NORAML && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_ATK_NORMAL)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_ATK_NORAML;
		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Yoneda::ATK_3Ren()
{
	if (m_iSkill == SKILL_RENDA && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_3REN && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_RENDA)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_3REN;
		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Yoneda::ATK_Knife_CMB()
{
	if (m_iSkill == SKILL_CMD && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_CMB_1 && *(m_pAnimCom[*m_pCurrentAnimType]->Get_AnimPosition()) >= 16.0)
			*m_pState = CMonster::MONSTER_CMB_2;
		if (*m_pState == CMonster::MONSTER_CMB_2 && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
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

CAI_Yoneda* CAI_Yoneda::Create()
{
	CAI_Yoneda* pInstance = new CAI_Yoneda();

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_Yoneda::Clone(void* pArg)
{
	CAI_Yoneda* pInstance = new CAI_Yoneda(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_Yoneda::Free()
{
	__super::Free();
}
