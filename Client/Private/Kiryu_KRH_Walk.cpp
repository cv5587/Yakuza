#include "GameInstance.h"
#include "Kiryu_KRH_Walk.h"
#include "Player.h"

CKiryu_KRH_Walk::CKiryu_KRH_Walk()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(457);			// 0 [457]	p_krh_shift_b[p_krh_shift_b]
	m_AnimationIndices.push_back(459);			// 1 [459]	p_krh_shift_bl[p_krh_shift_bl]
	m_AnimationIndices.push_back(461);			// 2 [461]	p_krh_shift_br[p_krh_shift_br]
	m_AnimationIndices.push_back(463);			// 3 [463]	p_krh_shift_f[p_krh_shift_f]
	m_AnimationIndices.push_back(465);			// 4 [465]	p_krh_shift_fl[p_krh_shift_fl]
	m_AnimationIndices.push_back(467);			// 5 [467]	p_krh_shift_fr[p_krh_shift_fr]
	m_AnimationIndices.push_back(469);			// 6 [469]	p_krh_shift_l[p_krh_shift_l]
	m_AnimationIndices.push_back(471);			// 7 [471]	p_krh_shift_r[p_krh_shift_r]
}

void CKiryu_KRH_Walk::Tick(const _float& fTimeDelta)
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
		m_iCurrentIndex = 3;  // p_KRH_shift_f[p_KRH_shift_f]
	}
	else if (direction == CPlayer::B)
	{
		m_iCurrentIndex = 0;  // p_KRH_shift_b[p_KRH_shift_b]
	}
	else if (direction == CPlayer::L)
	{
		m_iCurrentIndex = 6;  // p_KRH_shift_l[p_KRH_shift_l]
	}
	else if (direction == CPlayer::R)
	{
		m_iCurrentIndex = 7;  // p_KRH_shift_r[p_KRH_shift_r]
	}
	else if ((direction & (CPlayer::F | CPlayer::L)) == (CPlayer::F | CPlayer::L))
	{
		m_iCurrentIndex = 4;  // p_KRH_shift_fl[p_KRH_shift_fl]
	}
	else if ((direction & (CPlayer::F | CPlayer::R)) == (CPlayer::F | CPlayer::R))
	{
		m_iCurrentIndex = 5;  // p_KRH_shift_fr[p_KRH_shift_fr]
	}
	else if ((direction & (CPlayer::B | CPlayer::L)) == (CPlayer::B | CPlayer::L))
	{
		m_iCurrentIndex = 1;  // p_KRH_shift_bl[p_KRH_shift_bl]
	}
	else if ((direction & (CPlayer::B | CPlayer::R)) == (CPlayer::B | CPlayer::R))
	{
		m_iCurrentIndex = 2;  // p_KRH_shift_br[p_KRH_shift_br]
	}

	if (0 == direction)
		m_isStop = true;
}

void CKiryu_KRH_Walk::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex]);
}

_bool CKiryu_KRH_Walk::Get_AnimationEnd()
{
	if (m_isStop) {
		m_isStop = false;
		return true;
	}

	return false;
}

CBehaviorAnimation* CKiryu_KRH_Walk::Create(CPlayer* pPlayer)
{
	CKiryu_KRH_Walk* pInstnace = new CKiryu_KRH_Walk();

	if (nullptr == pInstnace)
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRH_Walk");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRH_Walk::Free()
{
	__super::Free();
}