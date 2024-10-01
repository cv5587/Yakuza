#include "AI_Kuze.h"

#include "GameInstance.h"

#include "Selector.h"
#include "Sequance.h"
#include "LeafNode.h"

#include "Monster.h"
#include "Player.h"
#include "Kuze.h"


/*
쿠제 스킬
1. Down -> 플레이어 다운 시 사용
2. Heavy -> 근접일 시 사용
3. Hiji_2Ren
4. Jab -> 잘안쓰이는것 같기도 하고
5. Cmb_a
6. Cmb_b
7. Cmb_HeadButt
8. Cmb_Renda

1페이즈
1. Jab
2. CMD_A
3. CMD_B
4. Cmb_Renda

2페이즈
1. Heavy
2. Hiji_2Ren
3. CMD_A
4. CMD_B
5. CMB_HeadButt


도발 -> 플레이어가 넘어지면 도발함 ...
플레이어가 도발 -> 기게이지


총 2페이즈
마지막 페이즈때는 몸에서 오라가 나온다. (붉은색)
참고 영상 : https://www.youtube.com/watch?v=Fp63CzT7IBY

일정 범위안에 들어오면 공격함.
랜덤으로 idle 상태를 유지하거나 플레이어에게 뛰어오도록 구현
*/

CAI_Kuze::CAI_Kuze()
	: CAI_Monster{}
{
}

CAI_Kuze::CAI_Kuze(const CAI_Kuze& rhs)
	: CAI_Monster{ rhs }
{
}

HRESULT CAI_Kuze::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_Kuze::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Tree();

	m_fDelayAttackDuration = 2.f;
	m_iMonsterType = CMonster::KUZE;

	m_fSwayDistance = _float2(1.2f, 1.9f);

	return S_OK;
}

void CAI_Kuze::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	this->Execute();
}

CBTNode::NODE_STATE CAI_Kuze::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_Kuze::Ready_Tree()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Sync
	CSequance* pSyncSeq = CSequance::Create();
	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Chcek_Sync, this)));
#pragma endregion

#pragma region Death
	CSequance* pDownSeq = CSequance::Create();
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Check_Down, this)));
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::StandUpAndDead, this)));

	CSelector* pDownSelector = CSelector::Create();
	pDownSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::StandUp, this)));
	pDownSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Dead, this)));
	pDownSeq->Add_Children(pDownSelector);
#pragma endregion

#pragma region PlayerDown
	CSequance* pPlayerDownSeq = CSequance::Create();
	pPlayerDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Check_PlayerDown, this)));
	pPlayerDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::ATK_Down, this)));
#pragma endregion

#pragma region Sway
	CSequance* pSwaySeq = CSequance::Create();
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Check_Sway, this)));
	pSwaySeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Sway, this)));
#pragma endregion

#pragma region HIT/GUARD
	CSequance* pHitGuardSeq = CSequance::Create();
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Check_Hit, this)));
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::HitAndGuard, this)));

	CSelector* pHitGuard = CSelector::Create();
	pHitGuard->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Guard, this)));
	pHitGuard->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Hit, this)));

	pHitGuardSeq->Add_Children(pHitGuard);
#pragma endregion

#pragma region HIT/GUARD
	CSequance* pQTESeq = CSequance::Create();
	pQTESeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Check_QTE, this)));
	pQTESeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::QTE, this)));
#pragma endregion

#pragma region Attack
	CSequance* pAttackSeq = CSequance::Create();
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Check_Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Attack, this)));

	CSelector* pAttackSelector = CSelector::Create();
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::ATK_Heavy, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::ATK_Hiji_2Ren, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::ATK_Jab, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::ATK_CMD_A, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::ATK_CMD_B, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::ATK_HeadButt, this)));
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::ATK_Renda, this)));

	pAttackSeq->Add_Children(pAttackSelector);
#pragma endregion

#pragma region Run
	CSequance* pStepSeq = CSequance::Create();
	pStepSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Check_Distance, this)));
	pStepSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Run, this)));
#pragma endregion


#pragma region Shift/Idle
	CSequance* pBreakSeq = CSequance::Create();
	pBreakSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Check_Break, this)));
	pBreakSeq->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::ShiftAndIdle, this)));

	CSelector* pBreakSelector = CSelector::Create();
	pBreakSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Shift, this)));
	pBreakSelector->Add_Children(CLeafNode::Create(bind(&CAI_Kuze::Idle, this)));

	pBreakSeq->Add_Children(pBreakSelector);
#pragma endregion

#pragma region Root
	pRoot->Add_Children(pSyncSeq);
	pRoot->Add_Children(pDownSeq);
	pRoot->Add_Children(pPlayerDownSeq);
	pRoot->Add_Children(pSwaySeq);
	pRoot->Add_Children(pHitGuardSeq);
	pRoot->Add_Children(pQTESeq);
	pRoot->Add_Children(pAttackSeq);
	pRoot->Add_Children(pStepSeq);
	pRoot->Add_Children(pBreakSeq);
#pragma endregion

	m_pRootNode = pRoot;
}

CBTNode::NODE_STATE CAI_Kuze::Check_Sway()
{
	m_pThis->Set_Down(false);

	if (m_isGuard || m_iSkill == SKILL_HIT || m_isAttack || m_isPlayerDownAtk || m_iSkill == SKILL_QTE)
	{
		m_isSway = false;
		return CBTNode::FAIL;
	}

	if (m_isSway)
	{
		if (m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isSway = false;
			//랜덤으로 반격함
			_bool isAttack = m_pGameInstance->Get_Random(0, 1);
			if (isAttack == 1)
			{
				*m_pState = CMonster::MONSTER_CMB_RENDA_1;
				m_iSkill = SKILL_CMD_RENDA;
				m_isAttack = true;
				return CBTNode::SUCCESS;
			}

			return CBTNode::FAIL;
		}

		return CBTNode::RUNNING;
	}

	//플래이어가 현재 공격중인가? && 플레이어와 충돌하지 않았는가?
	if (m_pPlayer->isAttack() && !m_pThis->isColl())
	{
		//플레이어와의 거리가 어느정도 있는 상태여야만 함
		if (DistanceFromPlayer() >= m_fSwayDistance.x && DistanceFromPlayer() < m_fSwayDistance.y)
		{
			Reset_State();
			return CBTNode::SUCCESS;
		}
	}

	m_isSway = false;

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Kuze::Check_QTE()
{
	CKuze* pKuze = dynamic_cast<CKuze*>(m_pThis);
	if (pKuze->Get_KuzePage() == CKuze::TWO && !m_isQTE)
	{
		//쿠제가 2페이즈일 시 QTE 실행
		m_iSkill = SKILL_QTE;
		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Kuze::QTE()
{
	if (m_iSkill == SKILL_QTE)
	{
		m_pPlayer->Play_Kuze_QTE(m_pThis);
		//*m_pState = CMonster::MONSTER_H23250_000_2;
		//*m_pCurrentAnimType = CLandObject::CUTSCENE;
		return CBTNode::SUCCESS;
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Kuze::Check_Attack()
{
	//플레이어가 다운상태면 공격하지 않음
	if (m_pPlayer->isDown())
		return CBTNode::FAIL;

	if (m_fAttackCount > 0)
		return CBTNode::SUCCESS;

	if (!m_isAttack)
	{
		if (m_fDelayAttackDuration > m_fAttackDelayTime)
			return CBTNode::FAIL;

		m_fAttackCount = m_pGameInstance->Get_Random(1.f, 3.f);
		m_fAttackDelayTime = 0.f;
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Kuze::Attack()
{
	LookAtPlayer();

	if (m_isAttack)
		return CBTNode::SUCCESS;

	//어택 정하기
	if (DistanceFromPlayer() > 2.f)
	{
		//근접이 아니라면 공격하지않고 대기 상태로 있게 된다.
		//뛰어온다.
		m_isRun = true;
		return CBTNode::FAIL;
	}

	CKuze* pKuze = dynamic_cast<CKuze*>(m_pThis);
	if (pKuze->Get_KuzePage() == CKuze::ONE)
	{
		//1페이즈
		m_isRun = false;
		static _uint iOneCount = 0;

		m_fAttackCount--;

		switch (iOneCount)
		{
		case 0:
			m_iSkill = SKILL_JAB;
			break;
		case 1:
		case 2:
		case 3:
			m_iSkill = SKILL_CMD_A;
			break;
		case 4:
		case 5:
			m_iSkill = SKILL_CMD_B;
			break;
		case 6:
			m_iSkill = SKILL_CMD_RENDA;
			break;
		default:
			break;
		}

		if (iOneCount >= 6)
			iOneCount = 0.f;
		else
			iOneCount++;
	}
	else if (pKuze->Get_KuzePage() == CKuze::TWO)
	{
		//2페이즈
		static _uint iTwoCount = 0;

		switch (iTwoCount)
		{
		case 0:
			m_iSkill = SKILL_HEAVY;
			break;
		case 1:
			m_iSkill = SKILL_HIJI_2REN;
			break;
		case 2:
		case 3:
		case 4:
			m_iSkill = SKILL_CMD_A;
			break;
		case 5:
		case 6:
			m_iSkill = SKILL_CMD_B;
			break;
		case 7:
			m_iSkill = SKILL_CMD_HEADBUTT;
			break;
		}

		if (iTwoCount >= 7)
			iTwoCount = 0;
		else
			iTwoCount++;
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Kuze::ATK_Heavy()
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

CBTNode::NODE_STATE CAI_Kuze::ATK_Hiji_2Ren()
{
	if (m_iSkill == SKILL_HIJI_2REN && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_HIJI_2REN && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_HIJI_2REN)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_HIJI_2REN;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Kuze::ATK_Jab()
{
	if (m_iSkill == SKILL_JAB && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_JAB && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_JAB)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_JAB;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Kuze::ATK_CMD_A()
{
	if (m_iSkill == SKILL_CMD_A && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_CMB_A_1 && *(m_pAnimCom[*m_pCurrentAnimType]->Get_AnimPosition()) >= 20.0)
		{
			*m_pState = CMonster::MONSTER_CMB_A_2;
		}
		else if (*m_pState == CMonster::MONSTER_CMB_A_2 && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_CMD_A)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_CMB_A_1;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Kuze::ATK_CMD_B()
{
	if (m_iSkill == SKILL_CMD_B && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_CMB_B_1 && *(m_pAnimCom[*m_pCurrentAnimType]->Get_AnimPosition()) >= 20.0)
			*m_pState = CMonster::MONSTER_CMB_B_2;
		else if (*m_pState == CMonster::MONSTER_CMB_B_2 && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_CMD_B)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_CMB_B_1;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Kuze::ATK_HeadButt()
{
	if (m_iSkill == SKILL_CMD_HEADBUTT && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_CMB_HEADBUTT_1 && *(m_pAnimCom[*m_pCurrentAnimType]->Get_AnimPosition()) >= 13.f)
			*m_pState = CMonster::MONSTER_CMB_HEADBUTT_2;
		else if (*m_pState == CMonster::MONSTER_CMB_HEADBUTT_2 && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_CMD_HEADBUTT)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_CMB_HEADBUTT_1;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Kuze::ATK_Renda()
{
	if (m_iSkill == SKILL_CMD_RENDA && m_isAttack)
	{
		if (*m_pState == CMonster::MONSTER_CMB_RENDA_1 && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
			*m_pState = CMonster::MONSTER_CMB_RENDA_2;
		else if (*m_pState == CMonster::MONSTER_CMB_RENDA_2 && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
			*m_pState = CMonster::MONSTER_CMB_RENDA_3;
		else if (*m_pState == CMonster::MONSTER_CMB_RENDA_3 && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isAttack = false;

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_CMD_RENDA)
	{
		m_isAttack = true;
		*m_pState = CMonster::MONSTER_CMB_RENDA_1;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Kuze::Check_Distance()
{
	if (m_isRun)
	{
		_float fDistance = DistanceFromPlayer();
		if (fDistance > 2.f)
		{
			if (m_iSkill == SKILL_RUN)
				return CBTNode::SUCCESS;

			_uint iRandom = m_pGameInstance->Get_Random(0, 100);

			if (iRandom == 84 || iRandom == 34 || iRandom == 67)
				return CBTNode::SUCCESS;
		}
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Kuze::Run()
{
	LookAtPlayer();

	m_isBreak = false;
	m_iSkill = SKILL_RUN;
	*m_pState = CMonster::MONSTER_RUN;

	return CBTNode::SUCCESS;
}

CAI_Kuze* CAI_Kuze::Create()
{
	CAI_Kuze* pInstance = new CAI_Kuze();

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_Kuze::Clone(void* pArg)
{
	CAI_Kuze* pInstance = new CAI_Kuze(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_Kuze::Free()
{
	__super::Free();
}