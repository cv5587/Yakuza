#include "GameInstance.h"
#include "Kiryu_KRC_Sway.h"
#include "Player.h"

CKiryu_KRC_Sway::CKiryu_KRC_Sway()
	:CBehaviorAnimation{}
{
	/* 0 ~ 3 */
	m_AnimationIndices.push_back(294);	//[294]	p_krc_kaihi_f[p_krc_kaihi_f]
	m_AnimationIndices.push_back(293);	//[293]	p_krc_kaihi_b[p_krc_kaihi_b]
	m_AnimationIndices.push_back(295);	//[295]	p_krc_kaihi_l[p_krc_kaihi_l]
	m_AnimationIndices.push_back(296);	//[296]	p_krc_kaihi_r[p_krc_kaihi_r]
}

void CKiryu_KRC_Sway::Tick(const _float& fTimeDelta)
{
	const _bool* pDirection = m_pPlayer->Get_MoveDirection();

	if (pDirection[CPlayer::F])
		m_iDirectionIndex = CPlayer::F;
	else if (pDirection[CPlayer::L])
		m_iDirectionIndex = CPlayer::L;
	else if (pDirection[CPlayer::R])
		m_iDirectionIndex = CPlayer::R;
	else
		m_iDirectionIndex = CPlayer::B;
}

void CKiryu_KRC_Sway::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iDirectionIndex + (4 * m_iComboCount)]);
}

_bool CKiryu_KRC_Sway::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		m_pPlayer->Reset_MoveDirection();
		return true;
	}
	return false;
}

void CKiryu_KRC_Sway::Reset()
{
	m_iComboCount = 0;
}

void CKiryu_KRC_Sway::Combo_Count(_bool isFinAction)
{
	if (Changeable_Combo_Animation())
	{
		m_iComboCount++;

		if (m_iComboCount > 2)
		{
			m_iComboCount = 2;
			return;
		}
	}
}

_bool CKiryu_KRC_Sway::Changeable_Combo_Animation()
{
	_float fInterval = 0.4f;

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (fInterval < *pModelCom->Get_AnimationCurrentPosition() / *pModelCom->Get_AnimationDuration())
		return true;

	return false;
}

CBehaviorAnimation* CKiryu_KRC_Sway::Create(CPlayer* pPlayer)
{
	CKiryu_KRC_Sway* pInstnace = new CKiryu_KRC_Sway();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRH_Sway");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRC_Sway::Free()
{
	__super::Free();
}