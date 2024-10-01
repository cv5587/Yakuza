#include "AI_Heli.h"

#include "GameInstance.h"

#include "Player.h"
#include "CarChase_Monster.h"
#include "CarChase_CATBullet.h"

#include "LeafNode.h"
#include "Sequance.h"
#include "Selector.h"

#include "Highway_Taxi.h"

CAI_Heli::CAI_Heli()
	: CAI_CarChase{}
{
}

CAI_Heli::CAI_Heli(const CAI_Heli& rhs)
	: CAI_CarChase{ rhs }
{
}

HRESULT CAI_Heli::Initialize_Prototype()
{
	return S_OK	;
}

HRESULT CAI_Heli::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Tree();

	if (*m_pWeaponType == CCarChase_Monster::GUN)
		m_fDelayAttackDuration = 8.f;
	else if (*m_pWeaponType == CCarChase_Monster::RKT)
		m_fDelayAttackDuration = 4.f;
	else if (*m_pWeaponType == CCarChase_Monster::GAT)
		m_fDelayAttackDuration = 3.f;

	m_fDelayAttackReadyDuration = m_fDelayAttackDuration + 2.f;

	return S_OK;
}

void CAI_Heli::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

CBTNode::NODE_STATE CAI_Heli::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_Heli::Ready_Tree()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Death
	CSequance* pDownSeq = CSequance::Create();
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Heli::Check_Down, this)));
	pDownSeq->Add_Children(CLeafNode::Create(bind(&CAI_Heli::Dead, this)));
#pragma endregion

#pragma region HIT
	CSequance* pHitSeq = CSequance::Create();
	pHitSeq->Add_Children(CLeafNode::Create(bind(&CAI_Heli::Check_Hit, this)));
	pHitSeq->Add_Children(CLeafNode::Create(bind(&CAI_Heli::Hit, this)));
#pragma endregion

#pragma region Attack
	CSequance* pAttackSeq = CSequance::Create();
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_Heli::Check_Attack, this)));
	pAttackSeq->Add_Children(CLeafNode::Create(bind(&CAI_Heli::ATK_Shot, this)));
#pragma endregion

#pragma region Ready
	CSequance* pReadySeq = CSequance::Create();
	pReadySeq->Add_Children(CLeafNode::Create(bind(&CAI_Heli::Check_Idle, this)));
	pReadySeq->Add_Children(CLeafNode::Create(bind(&CAI_Heli::Idle, this)));
#pragma endregion

#pragma region Root
	pRoot->Add_Children(pDownSeq);
	//pRoot->Add_Children(pSyncSeq);
	pRoot->Add_Children(pHitSeq);
	pRoot->Add_Children(pAttackSeq);
	pRoot->Add_Children(pReadySeq);
#pragma endregion

	m_pRootNode = pRoot;
}

CBTNode::NODE_STATE CAI_Heli::Dead()
{
	if (m_iSkill == SKILL_DEAD)
	{
		if (*m_pState == CCarChase_Monster::CARCHASE_DED && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_pThis->Set_Dead();

			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	*m_pState = CCarChase_Monster::CARCHASE_DED;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Heli::Hit()
{
	if (m_isHit)
	{
		if (*m_pState == CCarChase_Monster::CARCHASE_DAM_LV_2 && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
		{
			m_isHit = false;
			return CBTNode::SUCCESS;
		}

		return CBTNode::RUNNING;
	}

	*m_pState = CCarChase_Monster::CARCHASE_DAM_LV_2;
	m_isHit = true;

	Play_Hit();

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Heli::Check_Attack()
{
	if (m_isAttack) return CBTNode::SUCCESS;

	if (*m_pWeaponType == CCarChase_Monster::GUN && !m_isAtkReady)
	{
		m_fAttackDelayTime = 0.f;
		return CBTNode::FAIL;
	}

	if (m_fDelayAttackDuration <= m_fAttackDelayTime)
	{
		m_iSkill = SKILL_SHOT;
		m_fAttackDelayTime = 0.f;
		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Heli::ATK_Shot()
{
	if (m_iSkill == SKILL_SHOT && m_isAttack)
	{
		if (*m_pWeaponType == CCarChase_Monster::GAT)
		{
			if (m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
			{
				if (*m_pState == CCarChase_Monster::CARCHASE_SHOT_F_ST)
					*m_pState = CCarChase_Monster::CARCHASE_SHOT_F_LP;
				else if (*m_pState == CCarChase_Monster::CARCHASE_SHOT_F_LP)
					*m_pState = CCarChase_Monster::CARCHASE_SHOT_F_EN;
				else if (*m_pState == CCarChase_Monster::CARCHASE_SHOT_F_EN)
				{
					m_isAttack = false;
					return CBTNode::SUCCESS;
				}
			}
		}
		else
		{
			if (*m_pState == CCarChase_Monster::CARCHASE_AIM_SHOT && m_pAnimCom[*m_pCurrentAnimType]->Get_AnimFinished())
			{
				m_isAttack = false;
				m_fAttackReadyDelayTime = 0.f;
				return CBTNode::SUCCESS;
			}
		}

		return CBTNode::RUNNING;
	}

	if (m_iSkill == SKILL_SHOT)
	{
		m_isAttack = true;

		if (*m_pWeaponType == CCarChase_Monster::GAT)
		{
			m_pGameInstance->PlaySound_W(TEXT("e_gunshot_gatling_0.wav"), SOUND_ENEMY_EFFECT, DEFAULT_EFFECT_VOLUME);

			*m_pState = CCarChase_Monster::CARCHASE_SHOT_F_ST;
		}	
		else if (*m_pWeaponType == CCarChase_Monster::RKT)
		{
			*m_pState = CCarChase_Monster::CARCHASE_AIM_SHOT;

			m_pGameInstance->PlaySound_W(TEXT("e_gunshot_rocket_0.wav"), SOUND_ENEMY_EFFECT, DEFAULT_EFFECT_VOLUME);
			//불렛 생성
			CCarChase_CATBullet::BULLET_DESC Desc{};
			Desc.pParentMatrix = m_pThis->Get_ModelMatrix();
			if (FAILED(m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_RcktGunBullet"), TEXT("Layer_Bullet"), &Desc)))
				return CBTNode::FAIL;
		}
		else
		{
			m_pGameInstance->PlaySound_W(TEXT("e_gunshot_handgun_0.wav"), SOUND_ENEMY_EFFECT, DEFAULT_EFFECT_VOLUME);
			*m_pState = CCarChase_Monster::CARCHASE_AIM_SHOT;
		}

		m_pPlayer->OnHit(5.f);

		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_Heli::Check_Idle()
{
	if (m_fDelayAttackReadyDuration < m_fAttackReadyDelayTime)
	{
		//true이면 Ready 준비하는 중
		m_isAtkReady = !m_isAtkReady;
		m_fAttackReadyDelayTime = 0.f;
	}

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_Heli::Idle()
{
	if (*m_pWeaponType == CCarChase_Monster::RKT)
		*m_pState = CCarChase_Monster::CARCHASE_AIM_LP;
	else if (*m_pWeaponType == CCarChase_Monster::GUN)
	{
		if (m_isAtkReady)
			*m_pState = CCarChase_Monster::CARCHASE_AIM_LP;
		else 
			*m_pState = CCarChase_Monster::CARCHASE_STAND_UP;
	}
	else
		*m_pState = CCarChase_Monster::CARCHASE_STAND_UP;

	return CBTNode::SUCCESS;
}

CAI_Heli* CAI_Heli::Create()
{
	CAI_Heli* pInstance = new CAI_Heli();
	
	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_Heli::Clone(void* pArg)
{
	CAI_Heli* pInstance = new CAI_Heli(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_Heli::Free()
{
	__super::Free();
}
