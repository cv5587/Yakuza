#include "GameInstance.h"
#include "Kiryu_KRH_Idle.h"
#include "Player.h"

CKiryu_KRH_Idle::CKiryu_KRH_Idle()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(682);	//+[682]	p_stand_nml[p_stand_nml]
}

void CKiryu_KRH_Idle::Tick(const _float& fTimeDelta)
{
	
}

void CKiryu_KRH_Idle::Change_Animation()
{
	m_pPlayer->Set_HandAnimIndex(CPlayer::HAND_MIDDLE2);
	m_pPlayer->On_Separation_Hand();
	m_pPlayer->Change_Animation(m_AnimationIndices[0]);
}

_bool CKiryu_KRH_Idle::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		return true;
	}

	return false;
}

CBehaviorAnimation* CKiryu_KRH_Idle::Create(CPlayer* pPlayer)
{
	CKiryu_KRH_Idle* pInstnace = new CKiryu_KRH_Idle();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_Idle");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRH_Idle::Free()
{
	__super::Free();
}