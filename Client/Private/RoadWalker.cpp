#include "RoadWalker.h"

#include "GameInstance.h"

#include "AI_RoadWalker.h"

CRoadWalker::CRoadWalker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAdventure{pDevice, pContext }
{
}

CRoadWalker::CRoadWalker(const CRoadWalker& rhs)
	: CAdventure{ rhs }
{
}

HRESULT CRoadWalker::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRoadWalker::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//if (FAILED(__super::Add_Components()))
	//	return E_FAIL;


	return S_OK;
}

void CRoadWalker::Priority_Tick(const _float& fTimeDelta)
{
}

void CRoadWalker::Tick(const _float& fTimeDelta)
{
	m_pTree->Tick(fTimeDelta);

	Change_Animation();

	m_pModelCom->Play_Animation(fTimeDelta * m_fOffset, m_pAnimCom, m_isAnimLoop);

	Synchronize_Root(fTimeDelta);
}

void CRoadWalker::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CRoadWalker::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CRoadWalker::Change_Animation()
{
	__super::Change_Animation();

	switch (m_iState)
	{
	case ADVENTURE_IDLE:
	{
		m_strAnimName = "m_nml_tlk_stand_kamae";
		m_isAnimLoop = true;
		break;
	}
	case ADVENTURE_WALK:
	{
		m_strAnimName = "p_mov_walk_fast";
		m_isAnimLoop = true;
		m_fOffset = 0.8f;
		break;
	}
	case ADVENTURE_WALK_ST:
	{
		m_strAnimName = "p_mov_walk_st";
		break;
	}
	case ADVENTURE_WALK_EN:
	{
		m_strAnimName = "p_mov_walk_en";
		break;
	}
	case ADVENTURE_HIT_L:
	{
		m_strAnimName = "m_hml_act_walk_hit_l";
		break;
	}
	case ADVENTURE_HIT_R:
	{
		m_strAnimName = "m_hml_act_walk_hit_r";
		break;
	}
	case ADVENTURE_TURN:
	{
		if (m_pNavigationCom->Get_RouteDir() == DIR_F)
			m_strAnimName = "p_mov_turnl";
		else if (m_pNavigationCom->Get_RouteDir() == DIR_B)
			m_strAnimName = "p_mov_turnr";
		break;
	}
	case ADVENTURE_TURN90_L:
	{
		m_strAnimName = "p_kru_tlk_stand_turnr90"; // 몬스터 기준으로 왼, 오 결정해서 반대로 둠
		break;
	}
	case ADVENTURE_TURN90_R:
	{
		m_strAnimName = "p_kru_tlk_stand_turnl90";
		break;
	}
	default:
		break;
	}

	m_iAnim = m_pAnimCom->Get_AnimationIndex(m_strAnimName.c_str());

	if (m_iAnim == -1)
		return;

	if (FAILED(Setup_Animation()))
		return;
}

void CRoadWalker::Free()
{
	__super::Free();

	Safe_Release(m_pTree);
}
