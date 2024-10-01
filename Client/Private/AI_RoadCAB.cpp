#include "AI_RoadCAB.h"

#include "GameInstance.h"

#include "Sequance.h"
#include "Selector.h"
#include "LeafNode.h"

#include "Adventure.h"

CAI_RoadCAB::CAI_RoadCAB()
	: CAI_RoadStanding{}
{
}

CAI_RoadCAB::CAI_RoadCAB(const CAI_RoadCAB& rhs)
	: CAI_RoadStanding{ rhs }
{
}

HRESULT CAI_RoadCAB::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_RoadCAB::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Root();

	return S_OK;
}

void CAI_RoadCAB::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	this->Execute();
}

CBTNode::NODE_STATE CAI_RoadCAB::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_RoadCAB::Ready_Root()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Coll
	CSequance* pCollSeq = CSequance::Create();
	pCollSeq->Add_Children(CLeafNode::Create(bind(&CAI_RoadCAB::Check_Coll, this)));
	pCollSeq->Add_Children(CLeafNode::Create(bind(&CAI_RoadCAB::Coll, this)));
#pragma endregion

#pragma region Walk
	CSequance* pStandSeq = CSequance::Create();
	pStandSeq->Add_Children(CLeafNode::Create(bind(&CAI_RoadCAB::Check_Stand, this)));

	CSelector* pStandSelector = CSelector::Create();
	pStandSelector->Add_Children(CLeafNode::Create(bind(&CAI_RoadCAB::Ground_The_Player, this)));
	pStandSelector->Add_Children(CLeafNode::Create(bind(&CAI_RoadCAB::Stand, this)));
	pStandSeq->Add_Children(pStandSelector);
#pragma endregion

	pRoot->Add_Children(pCollSeq);
	pRoot->Add_Children(pStandSeq);

	m_pRootNode = pRoot;
}

CBTNode::NODE_STATE CAI_RoadCAB::Ground_The_Player()
{
	if (m_isGround)
	{
		m_iDelayDelta++;

		if (210 < m_iDelayDelta)
		{
			m_iDelayDelta = 0;
			m_isGround = false;
		}

		return CBTNode::FAIL;
	}

	if (m_iSkill == SKILL_GROUND &&
		(*m_pState == CAdventure::ADVENTURE_REACT_A ||
			*m_pState == CAdventure::ADVENTURE_REACT_B ||
			*m_pState == CAdventure::ADVENTURE_REACT_C ||
			*m_pState == CAdventure::ADVENTURE_REACT_D))
	{
		if (m_pAnimCom->Get_AnimFinished())
		{
			m_isGround = true;
			m_iDelayDelta = 0;
			*m_pState = CAdventure::ADVENTURE_IDLE;
			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_GROUND)
	{
		_uint iRandom = m_pGameInstance->Get_Random(0, 3);

		if (iRandom == 0)
			*m_pState = CAdventure::ADVENTURE_REACT_A;
		else if (iRandom == 1)
			*m_pState = CAdventure::ADVENTURE_REACT_B;
		else if (iRandom == 2)
			*m_pState = CAdventure::ADVENTURE_REACT_C;
		else if (iRandom == 3)
			*m_pState = CAdventure::ADVENTURE_REACT_D;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CAI_RoadCAB* CAI_RoadCAB::Create()
{
	CAI_RoadCAB* pInstance = new CAI_RoadCAB();

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_RoadCAB::Clone(void* pArg)
{
	CAI_RoadCAB* pInstance = new CAI_RoadCAB(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_RoadCAB::Free()
{
	__super::Free();
}
