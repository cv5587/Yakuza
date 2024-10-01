#include "GameInstance.h"
#include "Kiryu_Adventure_Walk.h"
#include "Player.h"

CKiryu_Adventure_Walk::CKiryu_Adventure_Walk()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(642);		//[642]	[p_mov_walk_fast]
}

void CKiryu_Adventure_Walk::Tick(const _float& fTimeDelta)
{
	m_pPlayer->Change_Animation(m_AnimationIndices[0]);
}

void CKiryu_Adventure_Walk::Change_Animation()
{
	m_pPlayer->Set_HandAnimIndex(CPlayer::HAND_MIDDLE2);
	m_pPlayer->On_Separation_Hand();
}

_bool CKiryu_Adventure_Walk::Get_AnimationEnd()
{
	if (m_eAnimState != ANIM_END) return false;

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));

	if (pModelCom->Get_AnimFinished())
	{
		m_iCurrentIndex = 0;
		m_eAnimState = ANIM_LOOP;
		return true;
	}

	return false;
}

void CKiryu_Adventure_Walk::Stop()
{

}

CBehaviorAnimation* CKiryu_Adventure_Walk::Create(CPlayer* pPlayer)
{
	CKiryu_Adventure_Walk* pInstnace = new CKiryu_Adventure_Walk();

	if (nullptr == pInstnace)
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_Adventure_Walk");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_Adventure_Walk::Free()
{
	__super::Free();
}
