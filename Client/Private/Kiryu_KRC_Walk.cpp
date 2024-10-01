#include "GameInstance.h"
#include "Kiryu_KRC_Walk.h"
#include "Player.h"

CKiryu_KRC_Walk::CKiryu_KRC_Walk()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(301);			// 0 [301]	p_krc_shift_b[p_krc_shift_b]
	m_AnimationIndices.push_back(303);			// 1 [303]	[p_krc_shift_bl]
	m_AnimationIndices.push_back(305);			// 2 [305]	p_krc_shift_br[p_krc_shift_br]
	m_AnimationIndices.push_back(307);			// 3 [307]	p_krc_shift_f[p_krc_shift_f]
	m_AnimationIndices.push_back(309);			// 4 [309]	p_krc_shift_fl[p_krc_shift_fl]
	m_AnimationIndices.push_back(311);			// 5 [311]	p_krc_shift_fr[p_krc_shift_fr]
	m_AnimationIndices.push_back(313);			// 6 [313]	p_krc_shift_l[p_krc_shift_l]
	m_AnimationIndices.push_back(315);			// 7 [315]	p_krc_shift_r[p_krc_shift_r]
}

void CKiryu_KRC_Walk::Tick(const _float& fTimeDelta)
{
	CLandObject* pTargetObject = reinterpret_cast<CLandObject*>(m_pPlayer->Get_TargetObject());

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
		m_iCurrentIndex = 3;  // p_KRC_shift_f[p_KRC_shift_f]
	}
	else if (direction == CPlayer::B)
	{
		m_iCurrentIndex = 0;  // p_KRC_shift_b[p_KRC_shift_b]
	}
	else if (direction == CPlayer::L)
	{
		m_iCurrentIndex = 6;  // p_KRC_shift_l[p_KRC_shift_l]
	}
	else if (direction == CPlayer::R)
	{
		m_iCurrentIndex = 7;  // p_KRC_shift_r[p_KRC_shift_r]
	}
	else if ((direction & (CPlayer::F | CPlayer::L)) == (CPlayer::F | CPlayer::L))
	{
		m_iCurrentIndex = 4;  // p_KRC_shift_fl[p_KRC_shift_fl]
	}
	else if ((direction & (CPlayer::F | CPlayer::R)) == (CPlayer::F | CPlayer::R))
	{
		m_iCurrentIndex = 5;  // p_KRC_shift_fr[p_KRC_shift_fr]
	}
	else if ((direction & (CPlayer::B | CPlayer::L)) == (CPlayer::B | CPlayer::L))
	{
		m_iCurrentIndex = 1;  // p_KRC_shift_bl[p_KRC_shift_bl]
	}
	else if ((direction & (CPlayer::B | CPlayer::R)) == (CPlayer::B | CPlayer::R))
	{
		m_iCurrentIndex = 2;  // p_KRC_shift_br[p_KRC_shift_br]
	}

	if (0 == direction)
		m_isStop = true;
}

void CKiryu_KRC_Walk::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex]);
}

_bool CKiryu_KRC_Walk::Get_AnimationEnd()
{
	if (m_isStop) {
		m_isStop = false;
		return true;
	}

	return false;
}

CBehaviorAnimation* CKiryu_KRC_Walk::Create(CPlayer* pPlayer)
{
	CKiryu_KRC_Walk* pInstnace = new CKiryu_KRC_Walk();

	if (nullptr == pInstnace)
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRC_Walk");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRC_Walk::Free()
{
	__super::Free();
}