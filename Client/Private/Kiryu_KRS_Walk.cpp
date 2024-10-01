#include "GameInstance.h"
#include "Kiryu_KRS_Walk.h"
#include "Player.h"
#include "Monster.h"

CKiryu_KRS_Walk::CKiryu_KRS_Walk()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(548);			// 0 [548]	p_krs_shift_b[p_krs_shift_b]
	m_AnimationIndices.push_back(550);			// 1 [550]	p_krs_shift_bl[p_krs_shift_bl]
	m_AnimationIndices.push_back(552);			// 2 [552]	p_krs_shift_br[p_krs_shift_br]
	m_AnimationIndices.push_back(554);			// 3 [554]	p_krs_shift_f[p_krs_shift_f]
	m_AnimationIndices.push_back(556);			// 4 [556]	p_krs_shift_fl[p_krs_shift_fl]
	m_AnimationIndices.push_back(558);			// 5 [558]	p_krs_shift_fr[p_krs_shift_fr]
	m_AnimationIndices.push_back(560);			// 6 [560]	p_krs_shift_l[p_krs_shift_l]
	m_AnimationIndices.push_back(562);			// 7 [562] p_krs_shift_r[p_krs_shift_r]
}

void CKiryu_KRS_Walk::Tick(const _float& fTimeDelta)
{
	CLandObject* pTargetObject = m_pPlayer->Get_TargetObject();

	if (nullptr != pTargetObject)
	{
		_vector vLookPos = pTargetObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		m_pPlayer->Get_TransformCom()->LookAt_For_LandObject(vLookPos);
	}

	_uint direction = 0;
	if (m_pGameInstance->GetKeyState(DIK_W) == HOLD)
		direction |= CPlayer::F;
	if (m_pGameInstance->GetKeyState(DIK_S) == HOLD)
		direction |= CPlayer::B;
	if (m_pGameInstance->GetKeyState(DIK_A) == HOLD)
		direction |= CPlayer::L;
	if (m_pGameInstance->GetKeyState(DIK_D) == HOLD)
		direction |= CPlayer::R;

	if (direction == CPlayer::F)
	{
		m_iCurrentIndex = 3;  // p_krs_shift_f[p_krs_shift_f]
	}
	else if (direction == CPlayer::B)
	{
		m_iCurrentIndex = 0;  // p_krs_shift_b[p_krs_shift_b]
	}
	else if (direction == CPlayer::L)
	{
		m_iCurrentIndex = 6;  // p_krs_shift_l[p_krs_shift_l]
	}
	else if (direction == CPlayer::R)
	{
		m_iCurrentIndex = 7;  // p_krs_shift_r[p_krs_shift_r]
	}
	else if ((direction & (CPlayer::F | CPlayer::L)) == (CPlayer::F | CPlayer::L))
	{
		m_iCurrentIndex = 4;  // p_krs_shift_fl[p_krs_shift_fl]
	}
	else if ((direction & (CPlayer::F | CPlayer::R)) == (CPlayer::F | CPlayer::R))
	{
		m_iCurrentIndex = 5;  // p_krs_shift_fr[p_krs_shift_fr]
	}
	else if ((direction & (CPlayer::B | CPlayer::L)) == (CPlayer::B | CPlayer::L))
	{
		m_iCurrentIndex = 1;  // p_krs_shift_bl[p_krs_shift_bl]
	}
	else if ((direction & (CPlayer::B | CPlayer::R)) == (CPlayer::B | CPlayer::R))
	{
		m_iCurrentIndex = 2;  // p_krs_shift_br[p_krs_shift_br]
	}

	if (0 == direction)
		m_isStop = true;
}

void CKiryu_KRS_Walk::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex]);
}

_bool CKiryu_KRS_Walk::Get_AnimationEnd()
{
	if (m_isStop) {
		m_isStop = false;
		return true;
	}

	return false;
}

CBehaviorAnimation* CKiryu_KRS_Walk::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_Walk* pInstnace = new CKiryu_KRS_Walk();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_Walk");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_Walk::Free()
{
	__super::Free();
}