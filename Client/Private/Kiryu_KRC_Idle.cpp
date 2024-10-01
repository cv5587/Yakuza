#include "GameInstance.h"
#include "Kiryu_KRC_Idle.h"
#include "Player.h"

CKiryu_KRC_Idle::CKiryu_KRC_Idle()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(682);	//+[682]	p_stand_nml[p_stand_nml]
}

void CKiryu_KRC_Idle::Tick(const _float& fTimeDelta)
{
	
}

void CKiryu_KRC_Idle::Change_Animation()
{
	m_pPlayer->Set_HandAnimIndex(CPlayer::HAND_MIDDLE2);
	m_pPlayer->On_Separation_Hand();
	m_pPlayer->Change_Animation(m_AnimationIndices[0]);
}

_bool CKiryu_KRC_Idle::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		return true;
	}

	return false;
}

CBehaviorAnimation* CKiryu_KRC_Idle::Create(CPlayer* pPlayer)
{
	CKiryu_KRC_Idle* pInstnace = new CKiryu_KRC_Idle();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_Idle");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRC_Idle::Free()
{
	__super::Free();
}