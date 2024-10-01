#include "AI_WPAYakuza.h"

#include "GameInstance.h"

#include "Sequance.h"
#include "Selector.h"
#include "LeafNode.h"

#include "Monster.h"


CAI_WPAYakuza::CAI_WPAYakuza()
	: CAI_Monster{}
{
}

CAI_WPAYakuza::CAI_WPAYakuza(const CAI_WPAYakuza& rhs)
	: CAI_Monster{ rhs }
{
}

HRESULT CAI_WPAYakuza::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_WPAYakuza::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Tree();

	m_fDelayAttackDuration = m_pGameInstance->Get_Random(6.f, 9.f);
	m_iMonsterType = CMonster::WPA;

	return S_OK;
}

void CAI_WPAYakuza::Ready_Tree()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Sync
	CSequance* pSyncSeq = CSequance::Create();
	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Chcek_Sync, this)));
#pragma endregion

#pragma region Death
	CSequance* pDownSeq = CSequance::Create();
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Check_Down, this)));
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::StandUpAndDead, this)));

	CSelector* pDownSelector = CSelector::Create();
	pDownSelector->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::StandUp, this)));
	pDownSelector->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Dead, this)));
	pDownSeq->Add_Children(pDownSelector);
#pragma endregion

#pragma region Sway
	CSequance* pSwaySeq = CSequance::Create();
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Check_Sway, this)));
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Sway, this)));
#pragma endregion

#pragma region HIT/GUARD
	CSequance* pHitGuardSeq = CSequance::Create();
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Check_Hit, this)));
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::HitAndGuard, this)));

	CSelector* pHitGuard = CSelector::Create();
	//pHitGuard->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Guard, this)));
	pHitGuard->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Hit, this)));

	pHitGuardSeq->Add_Children(pHitGuard);
#pragma endregion

#pragma region Angry
	CSequance* pAngrySeq = CSequance::Create();
	pAngrySeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Check_Angry, this)));
	pAngrySeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Angry, this)));
#pragma endregion

#pragma region Attack
	CSequance* pAttackSeq = CSequance::Create();
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Check_Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Angry_Attack, this)));

	CSelector* pAttackSelector = CSelector::Create();
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::ATK_Heavy, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::ATK_CMD, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::ATK_Angry_Punch, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::ATK_Angry_Kick, this)));

	pAttackSeq->Add_Children(pAttackSelector);
#pragma endregion

#pragma region Shift/Idle
	CSequance* pBreakSeq = CSequance::Create();
	pBreakSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Check_Break, this)));
	pBreakSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::ShiftAndIdle, this)));

	CSelector* pBreakSelector = CSelector::Create();
	pBreakSelector->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Shift, this)));
	pBreakSelector->Add_Children(CLeafNode::Create(bind(&CAI_WPAYakuza::Idle, this)));

	pBreakSeq->Add_Children(pBreakSelector);
#pragma endregion

#pragma region Root
	pRoot->Add_Children(pSyncSeq);
	pRoot->Add_Children(pDownSeq);
	pRoot->Add_Children(pSwaySeq);
	pRoot->Add_Children(pHitGuardSeq);
	//pRoot->Add_Children(pAngrySeq);
	pRoot->Add_Children(pAttackSeq);
	pRoot->Add_Children(pBreakSeq);
#pragma endregion

	m_pRootNode = pRoot;
}

CBTNode::NODE_STATE CAI_WPAYakuza::HitAndGuard()
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

CBTNode::NODE_STATE CAI_WPAYakuza::Attack()
{
	if (m_isAngry || m_isAttack) return CBTNode::SUCCESS;

	//화가 나지 않앗을때 스킬 선택 (임시)
	LookAtPlayer();
	Reset_State();

	static _uint iCount = rand() % 3;

	if (iCount == 0 || iCount == 2)
		m_iSkill = SKILL_CMD;
	else
		m_iSkill = SKILL_HEAVY;

	iCount++;
	if (iCount >= 3)
		iCount = 0;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_WPAYakuza::Angry_Attack()
{
	if (!m_isAngry || m_isAttack) return CBTNode::SUCCESS;

	//화낫을때 스킬 선택 (임시)
	LookAtPlayer();
	Reset_State();

	static _uint iCount = 0;

	if (iCount == 0 || iCount == 1)
		m_iSkill = SKILL_ANGRY_KICK;
	else
		m_iSkill = SKILL_ANGRY_CHOP;

	iCount++;

	if (iCount >= 3)
		iCount = 0;

	return CBTNode::SUCCESS;
}

//한방 강공격
CBTNode::NODE_STATE CAI_WPAYakuza::ATK_Heavy()
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
		*m_pState = CMonster::MONSTER_HEAVY_ATTACK;
		m_isAttack = true;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

//2번 연속기
CBTNode::NODE_STATE CAI_WPAYakuza::ATK_CMD()
{
	if (m_iSkill == SKILL_CMD && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_CMB_1 && *(m_pAnimCom[*m_pCurrentAnimType]->Get_AnimPosition()) >= 22.0)
		{
			*m_pState = CMonster::MONSTER_CMB_2;
		}
		else if (*m_pState == CMonster::MONSTER_CMB_2 && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			//콤보 끝.
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	//첫 진입 시
	if (m_iSkill == SKILL_CMD)
	{
		*m_pState = CMonster::MONSTER_CMB_1;
		m_isAttack = true;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

void CAI_WPAYakuza::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	this->Execute();
}

CBTNode::NODE_STATE CAI_WPAYakuza::Execute()
{
	return m_pRootNode->Execute();
}

CAI_WPAYakuza* CAI_WPAYakuza::Create()
{
	CAI_WPAYakuza* pInstance = new CAI_WPAYakuza();

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_WPAYakuza::Clone(void* pArg)
{
	CAI_WPAYakuza* pInstance = new CAI_WPAYakuza();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_WPAYakuza::Free()
{
	__super::Free();
}
