#include "AI_RushYakuza.h"

#include "GameInstance.h"

#include "LeafNode.h"
#include "Selector.h"
#include "Sequance.h"

#include "RushYakuza.h"
#include "Player.h"

CAI_RushYakuza::CAI_RushYakuza()
	: CAI_Monster{}
{
}

CAI_RushYakuza::CAI_RushYakuza(const CAI_RushYakuza& rhs)
	: CAI_Monster{}
{
}

HRESULT CAI_RushYakuza::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_RushYakuza::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//트리 구현
	Ready_Tree();

	m_fDelayAttackDuration = m_pGameInstance->Get_Random(6.f, 9.f);
	m_iMonsterType = CMonster::RUSH;
	
	_uint iRandom = m_pGameInstance->Get_Random(0, 3);
	if (iRandom == 0)
		m_iAdventureIdelMotion = CMonster::MONSTER_ADVENTURE_IDLE_1;
	else if (iRandom == 1)
		m_iAdventureIdelMotion = CMonster::MONSTER_ADVENTURE_IDLE_2;
	else if (iRandom == 2)
		m_iAdventureIdelMotion = CMonster::MONSTER_ADVENTURE_IDLE_3;
	else if (iRandom == 3)
		m_iAdventureIdelMotion = CMonster::MONSTER_ADVENTURE_IDLE_4;

	return S_OK;
}

void CAI_RushYakuza::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	this->Execute();
}

CBTNode::NODE_STATE CAI_RushYakuza::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_RushYakuza::Ready_Tree()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Adventure
	CSequance* pAdventureSeq = CSequance::Create();
	pAdventureSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Check_Adventure, this)));
	pAdventureSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Adventure, this)));
#pragma endregion

#pragma region Sync
	CSequance* pSyncSeq = CSequance::Create();
	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Chcek_Sync, this)));
#pragma endregion
	
#pragma region Death
	CSequance* pDownSeq = CSequance::Create();
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Check_Down, this)));
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::StandUpAndDead, this)));

	CSelector* pDownSelector = CSelector::Create();
	pDownSelector->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::StandUp, this)));
	pDownSelector->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Dead, this)));
	pDownSeq->Add_Children(pDownSelector);
#pragma endregion

#pragma region Sway
	CSequance* pSwaySeq = CSequance::Create();
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Check_Sway, this)));
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Sway, this)));
#pragma endregion

#pragma region HIT/GUARD
	CSequance* pHitGuardSeq = CSequance::Create();
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Check_Hit, this)));
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::HitAndGuard, this)));

	CSelector* pHitGuard = CSelector::Create();
	pHitGuard->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Guard, this)));
	pHitGuard->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Hit, this)));

	pHitGuardSeq->Add_Children(pHitGuard);
#pragma endregion

#pragma region Angry
	CSequance* pAngrySeq = CSequance::Create();
	pAngrySeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Check_Angry, this)));
	pAngrySeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Angry, this)));
#pragma endregion

#pragma region Attack
	CSequance* pAttackSeq = CSequance::Create();
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Check_Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Angry_Attack, this)));

	CSelector* pAttackSelector = CSelector::Create();
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::ATK_Punch, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::ATK_CMD, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::ATK_Angry_Punch, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::ATK_Angry_Kick, this)));

	pAttackSeq->Add_Children(pAttackSelector);
#pragma endregion

#pragma region Shift/Idle
	CSequance* pBreakSeq = CSequance::Create();
	pBreakSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Check_Break, this)));
	pBreakSeq->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::ShiftAndIdle, this)));

	CSelector* pBreakSelector = CSelector::Create();
	pBreakSelector->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Shift, this)));
	pBreakSelector->Add_Children(CLeafNode::Create(bind(&CAI_RushYakuza::Idle, this)));

	pBreakSeq->Add_Children(pBreakSelector);
#pragma endregion

#pragma region Root
	pRoot->Add_Children(pAdventureSeq);
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

CBTNode::NODE_STATE CAI_RushYakuza::Check_Adventure()
{
	if (m_isAdventer)
	{
		*m_pCurrentAnimType = CLandObject::ADVENTURE;
		return CBTNode::SUCCESS;
	}

	//Reset_State();
	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_RushYakuza::Adventure()
{
	*m_pState = m_iAdventureIdelMotion;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_RushYakuza::Attack()
{
	if (m_isAngry || m_isAttack) return CBTNode::SUCCESS;

	LookAtPlayer();
	//화가 나지 않앗을때 스킬 선택 (임시)
	static _uint iCount = rand() % 3;

	if (iCount == 0 || iCount == 1)
		m_iSkill = SKILL_CMD;
	else
		m_iSkill = SKILL_PUNCH;

	iCount++;
	if (iCount >= 3)
		iCount = 0;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_RushYakuza::Angry_Attack()
{
	if (!m_isAngry || m_isAttack) return CBTNode::SUCCESS;

	LookAtPlayer();
	//화낫을때 스킬 선택 (임시)
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

CBTNode::NODE_STATE CAI_RushYakuza::ATK_Punch()
{
	if (m_iSkill == SKILL_PUNCH && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_PUNCH && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_PUNCH)
	{
		*m_pState = CMonster::MONSTER_PUNCH;
		m_isAttack = true;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_RushYakuza::ATK_CMD()
{
	if (m_iSkill == SKILL_CMD && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_CMB_1 && *(m_pAnimCom[*m_pCurrentAnimType]->Get_AnimPosition()) >= 10.0)
		{
			*m_pState = CMonster::MONSTER_CMB_2;
		}
		else if (*m_pState == CMonster::MONSTER_CMB_2 && *(m_pAnimCom[*m_pCurrentAnimType]->Get_AnimPosition()) >= 10.0)
		{
			*m_pState = CMonster::MONSTER_CMB_3;
		}
		else if (*m_pState == CMonster::MONSTER_CMB_3 && *(m_pAnimCom[*m_pCurrentAnimType]->Get_AnimPosition()) >= 10.0)
		{
			*m_pState = CMonster::MONSTER_CMB_4;
		}
		else if (*m_pState == CMonster::MONSTER_CMB_4 && *(m_pAnimCom[*m_pCurrentAnimType]->Get_AnimPosition()) >= 10.0)
		{
			*m_pState = CMonster::MONSTER_CMB_5;
		}
		else if (*m_pState == CMonster::MONSTER_CMB_5 && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
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

CAI_RushYakuza* CAI_RushYakuza::Create()
{
	CAI_RushYakuza* pInstance = new CAI_RushYakuza();


	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_RushYakuza::Clone(void* pArg)
{
	CAI_RushYakuza* pInstance = new CAI_RushYakuza();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_RushYakuza::Free()
{
	__super::Free();
}
