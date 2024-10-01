#include "AI_RoadTissue.h"

#include "GameInstance.h"

#include "Sequance.h"
#include "Selector.h"
#include "LeafNode.h"

#include "Adventure.h"

CAI_RoadTissue::CAI_RoadTissue()
	: CAI_RoadStanding{}
{
}

CAI_RoadTissue::CAI_RoadTissue(const CAI_RoadTissue& rhs)
	: CAI_RoadStanding{ rhs }
{
}

HRESULT CAI_RoadTissue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_RoadTissue::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Root();

	*m_pState == CAdventure::ADVENTURE_TISSUE_ST;

	return S_OK;
}

void CAI_RoadTissue::Tick(const _float& fTimeDelta)
{
	//__super::Tick(fTimeDelta);

	//this->Execute();

	if (DistanceFromPlayer() < 8.f)
	{
		if (SKILL_IDLE == m_iSkill)
		{
			m_iSkill = SKILL_GROUND;
			*m_pState = CAdventure::ADVENTURE_TISSUE_ST;
			m_fGroundDelta = 0;
		}

		if (*m_pState == CAdventure::ADVENTURE_IDLE)
		{
				m_fGroundDelta++;
				if (150 < m_fGroundDelta)
				{
					m_fGroundDelta = 0;
					*m_pState = CAdventure::ADVENTURE_TISSUE_ST;
					m_bStay = true;

					m_pGameInstance->PlaySound_W(TEXT("13ff [1].wav"), SOUND_NPC_3, m_fSound);
				}
		}
		else
		{
			if (m_pAnimCom->Get_AnimFinished())
			{
				*m_pState = CAdventure::ADVENTURE_IDLE;
			}
		}


	}
	else
	{
		m_iSkill = SKILL_IDLE;

		if (m_pAnimCom->Get_AnimFinished() && *m_pState == CAdventure::ADVENTURE_TISSUE_ST)
		{
			*m_pState = CAdventure::ADVENTURE_IDLE;
		}
	}


	

}

CBTNode::NODE_STATE CAI_RoadTissue::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_RoadTissue::Ready_Root()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Coll
	CSequance* pCollSeq = CSequance::Create();
	pCollSeq->Add_Children(CLeafNode::Create(bind(&CAI_RoadTissue::Check_Coll, this)));
	pCollSeq->Add_Children(CLeafNode::Create(bind(&CAI_RoadTissue::Coll, this)));
#pragma endregion

#pragma region Walk
	CSequance* pStandSeq = CSequance::Create();
	pStandSeq->Add_Children(CLeafNode::Create(bind(&CAI_RoadTissue::Check_Stand, this)));

	CSelector* pStandSelector = CSelector::Create();
	pStandSelector->Add_Children(CLeafNode::Create(bind(&CAI_RoadTissue::Ground_The_Player, this)));
	pStandSelector->Add_Children(CLeafNode::Create(bind(&CAI_RoadTissue::Stand, this)));
	pStandSeq->Add_Children(pStandSelector);
#pragma endregion

	pRoot->Add_Children(pCollSeq);
	pRoot->Add_Children(pStandSeq);

	m_pRootNode = pRoot;
}

CBTNode::NODE_STATE CAI_RoadTissue::Check_Stand()
{
	//플레이어가 근처에 있다면 맞는 동작을 한다.
	if (DistanceFromPlayer() > 5.f)
	{
		//들어오면 대화 가능함.
		m_iSkill = SKILL_GROUND;
	}
	else
	{
		// 티슈 주는 모션이었다면?
		if (m_iSkill == SKILL_GROUND && *m_pState == CAdventure::ADVENTURE_TISSUE_LP)
		{
			*m_pState = CAdventure::ADVENTURE_TISSUE_EN;
			return CBTNode::RUNNING;
		}
		
		//모션이 끝나고 Idle 상태로 진입
		if (m_iSkill == SKILL_GROUND && *m_pState == CAdventure::ADVENTURE_TISSUE_EN)
		{
			if (m_pAnimCom->Get_AnimFinished())
			{
				m_isGround = false;

				return CBTNode::SUCCESS;
			}

			return CBTNode::RUNNING;
		}
	}
		

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_RoadTissue::Ground_The_Player()
{
	if (m_isGround) return CBTNode::FAIL;

	if (m_iSkill == SKILL_GROUND && *m_pState == CAdventure::ADVENTURE_TISSUE_ST)
	{
		if (m_pAnimCom->Get_AnimFinished())
			*m_pState = CAdventure::ADVENTURE_TISSUE_LP;

		return CBTNode::RUNNING;
	}
	
	if (m_iSkill == SKILL_GROUND && *m_pState == CAdventure::ADVENTURE_TISSUE_LP)
	{
		//대화창이 끝나면? -> 대화창이 없다고 생각하고 진행.
		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_GROUND)
	{
		*m_pState = CAdventure::ADVENTURE_TISSUE_ST;

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CAI_RoadTissue* CAI_RoadTissue::Create()
{
	CAI_RoadTissue* pInstance = new CAI_RoadTissue();

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_RoadTissue::Clone(void* pArg)
{
	CAI_RoadTissue* pInstance = new CAI_RoadTissue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_RoadTissue::Free()
{
	__super::Free();
}
