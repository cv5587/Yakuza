#include "AI_RoadStanding.h"

#include "GameInstance.h"

#include "Player.h"
#include "Adventure.h"

CAI_RoadStanding::CAI_RoadStanding()
	: CBTNode{},
	m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CAI_RoadStanding::CAI_RoadStanding(const CAI_RoadStanding& rhs)
	: CBTNode{ rhs },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CAI_RoadStanding::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_RoadStanding::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	AI_ADVENTURE_DESC* pDesc = static_cast<AI_ADVENTURE_DESC*>(pArg);
	m_pAnimCom = pDesc->pAnimCom;
	m_pThis = pDesc->pThis;
	m_pState = pDesc->pState;
	m_pAStartCom = pDesc->pAStartCom;
	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

	return S_OK;
}

void CAI_RoadStanding::Tick(const _float& fTimeDelta)
{
}

_float CAI_RoadStanding::DistanceFromPlayer()
{
	_vector vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	_vector vThisPos = m_pThis->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	_float fDistance = XMVectorGetX(XMVector3Length(vPlayerPos - vThisPos));

	return fDistance;
}

CBTNode::NODE_STATE CAI_RoadStanding::Check_Coll()
{
	if (m_pThis->isColl())
	{

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_RoadStanding::Coll()
{

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_RoadStanding::Check_Stand()
{
	//플레이어가 근처에 있다면 맞는 동작을 한다.
	if (DistanceFromPlayer() < 10.f)
	{
		m_iSkill = SKILL_GROUND;
	}
	else
		m_isGround = false;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_RoadStanding::Ground_The_Player()
{
	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_RoadStanding::Stand()
{
	*m_pState = CAdventure::ADVENTURE_IDLE;

	return CBTNode::SUCCESS;
}

void CAI_RoadStanding::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRootNode);
}
