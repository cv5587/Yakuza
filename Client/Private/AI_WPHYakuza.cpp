#include "AI_WPHYakuza.h"

#include "GameInstance.h"


#include "Sequance.h"
#include "Selector.h"
#include "LeafNode.h"

#include "Monster.h"

CAI_WPHYakuza::CAI_WPHYakuza()
    : CAI_Monster{}
{
}

CAI_WPHYakuza::CAI_WPHYakuza(const CAI_WPHYakuza& rhs)
    : CAI_Monster{ rhs }
{
}

HRESULT CAI_WPHYakuza::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAI_WPHYakuza::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Ready_Tree();

    m_fDelayAttackDuration = m_pGameInstance->Get_Random(6.f, 9.f);
    m_iMonsterType = CMonster::WPH;

    return S_OK;
}

void CAI_WPHYakuza::Tick(const _float& fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (m_isHit)
    {
        m_fHitTime += fTimeDelta;

        if (m_fHitDuration < m_fHitTime)
        {
            m_isHit = false;
            m_fHitTime = 0.f;
        }
    }

    this->Execute();
}

CBTNode::NODE_STATE CAI_WPHYakuza::Execute()
{
    return m_pRootNode->Execute();
}

void CAI_WPHYakuza::Ready_Tree()
{
	CSelector* pRoot = CSelector::Create();

//#pragma region Sync
//	CSequance* pSyncSeq = CSequance::Create();
//	pSyncSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPHYakuza::Chcek_Sync, this)));
//#pragma endregion

#pragma region Death
	CSequance* pDownSeq = CSequance::Create();
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPHYakuza::Check_Down, this)));
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPHYakuza::Dead, this)));
#pragma endregion

#pragma region HIT/GUARD
	CSequance* pHitGuardSeq = CSequance::Create();
	pHitGuardSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPHYakuza::Check_Hit, this)));
#pragma endregion

#pragma region Attack
	CSequance* pAttackSeq = CSequance::Create();
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPHYakuza::Attack, this)));

	CSelector* pAttackSelector = CSelector::Create();
	pAttackSelector->Add_Children(CLeafNode::Create(bind(&CAI_WPHYakuza::ATK_Heavy, this)));

	pAttackSeq->Add_Children(pAttackSelector);
#pragma endregion

#pragma region Shift/Idle
    CSequance* pBreakSeq = CSequance::Create();
    pBreakSeq->Add_Children(CLeafNode::Create(bind(&CAI_WPHYakuza::Idle, this)));
#pragma endregion

#pragma region Root
	pRoot->Add_Children(pDownSeq);
	pRoot->Add_Children(pHitGuardSeq);
	pRoot->Add_Children(pAttackSeq);
	pRoot->Add_Children(pBreakSeq);
#pragma endregion

	m_pRootNode = pRoot;
}

CBTNode::NODE_STATE CAI_WPHYakuza::Check_Down()
{
    if (m_pThis->isObjectDead())
    {
        return CBTNode::SUCCESS;
    }

    return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_WPHYakuza::Dead()
{
    *m_pState = CMonster::MONSTER_DED_L;

    return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_WPHYakuza::Check_Hit()
{
    if (m_pThis->isColl())
        m_isHit = true;

    m_pThis->Set_RimLight(m_isHit ? 0.4f : 0.f);

    return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_WPHYakuza::Attack()
{
    if (DistanceFromPlayer() < 3.f)
    {
        m_iSkill = SKILL_HEAVY;

        return CBTNode::SUCCESS;
    }

    return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_WPHYakuza::ATK_Heavy()
{
    if (m_iSkill == SKILL_HEAVY && m_isAttack)
    {
        if (*m_pState == CMonster::MONSTER_HEAVY_ATTACK && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
        {
            m_isAttack = false;
            return CBTNode::FAIL;
        }

        return CBTNode::RUNNING;
    }

    if (m_iSkill == SKILL_HEAVY)
    {
        m_isAttack = true;
        *m_pState = CMonster::MONSTER_HEAVY_ATTACK;
    }

    return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_WPHYakuza::Idle()
{
    if (m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
        *m_pState = CMonster::MONSTER_IDLE;

    m_isAttack = false;

    return CBTNode::SUCCESS;
}

CAI_WPHYakuza* CAI_WPHYakuza::Create()
{
    CAI_WPHYakuza* pInstance = new CAI_WPHYakuza();

    if (FAILED(pInstance->Initialize_Prototype()))
        Safe_Release(pInstance);

    return pInstance;
}

CBTNode* CAI_WPHYakuza::Clone(void* pArg)
{
    CAI_WPHYakuza* pInstance = new CAI_WPHYakuza(*this);

    if (FAILED(pInstance->Initialize(pArg)))
        Safe_Release(pInstance);

    return pInstance;
}

void CAI_WPHYakuza::Free()
{
    __super::Free();
}
