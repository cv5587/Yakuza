#include "GameInstance.h"
#include "Kiryu_Adventure_Idle.h"
#include "Player.h"

CKiryu_Adventure_Idle::CKiryu_Adventure_Idle()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(682);		// [682]	[p_stand_nml]
	m_AnimationIndices.push_back(681);		// [681]	[p_stand_idle_lookaround]
}

void CKiryu_Adventure_Idle::Tick(const _float& fTimeDelta)
{
	switch (m_eCurrentBehavior)
	{
	case Client::CKiryu_Adventure_Idle::IDLE:
	{
		LookAroundTimer(fTimeDelta);
		break;
	}
	case Client::CKiryu_Adventure_Idle::LOOK_AROUND:
	{
		CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));

		if (pModelCom->Get_AnimFinished())
		{
			m_eCurrentBehavior = IDLE;
		}

		break;
	}
	}

}

void CKiryu_Adventure_Idle::Change_Animation()
{
	m_pPlayer->Set_HandAnimIndex(CPlayer::HAND_MIDDLE2);
	m_pPlayer->On_Separation_Hand();

	// 아이들은 모션을 하나만 둔다 (추후 추가예정)
	m_pPlayer->Change_Animation(m_AnimationIndices[m_eCurrentBehavior]);
}

CBehaviorAnimation* CKiryu_Adventure_Idle::Create(CPlayer* pPlayer)
{
	CKiryu_Adventure_Idle* pInstnace = new CKiryu_Adventure_Idle();

	if (nullptr == pInstnace)
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_Adventure_Idle");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_Adventure_Idle::Free()
{
	__super::Free();
}

void CKiryu_Adventure_Idle::LookAroundTimer(const _float& fTimeDelta)
{
	m_fLookAroundTimer += fTimeDelta;

	if (ANIM_CHANGE_TIME <= m_fLookAroundTimer)
	{
		m_fLookAroundTimer = { 0.f };

		m_eCurrentBehavior = LOOK_AROUND;
	}
}
