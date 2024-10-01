#include "GameInstance.h"
#include "Kiryu_KRC_Run.h"
#include "Player.h"

CKiryu_KRC_Run::CKiryu_KRC_Run()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(297); // [297]	p_krc_mov_run_btl[p_krc_mov_run_btl]
	m_AnimationIndices.push_back(298); // [298]	p_krc_mov_run_stop_btl[p_krc_mov_run_stop_btl]
}

void CKiryu_KRC_Run::Tick(const _float& fTimeDelta)
{
	switch (m_eAnimState)
	{
	case CKiryu_KRC_Run::ANIM_LOOP:
		if (m_isStop)
			m_eAnimState = ANIM_END;
		break;
	}
}

void CKiryu_KRC_Run::Change_Animation()
{
	m_pPlayer->Set_HandAnimIndex(CPlayer::HAND_MIDDLE2);
	m_pPlayer->On_Separation_Hand();
	m_pPlayer->Change_Animation(m_AnimationIndices[m_eAnimState]);
}

_bool CKiryu_KRC_Run::Get_AnimationEnd()
{
	if (m_eAnimState != ANIM_END) return false;

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));

	if (pModelCom->Get_AnimFinished())
	{
		Reset();
		return true;
	}

	return false;
}

void CKiryu_KRC_Run::Stop()
{
	m_isStop = true;
}

void CKiryu_KRC_Run::Reset()
{
	m_eAnimState = { ANIM_LOOP };
	m_isStop = false;
}

_bool CKiryu_KRC_Run::Stopping()
{
	if (ANIM_END == m_eAnimState && Changeable_Combo_Animation()) return true;

	return false;
}

_bool CKiryu_KRC_Run::Changeable_Combo_Animation()
{
	_float fInterval = 0.7f;

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (fInterval < *pModelCom->Get_AnimationCurrentPosition() / *pModelCom->Get_AnimationDuration())
		return true;

	return false;
}

CBehaviorAnimation* CKiryu_KRC_Run::Create(CPlayer* pPlayer)
{
	CKiryu_KRC_Run* pInstnace = new CKiryu_KRC_Run();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRH_Run");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRC_Run::Free()
{
	__super::Free();
}