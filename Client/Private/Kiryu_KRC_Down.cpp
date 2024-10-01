#include "GameInstance.h"
#include "Kiryu_KRC_Down.h"
#include "Player.h"

CKiryu_KRC_Down::CKiryu_KRC_Down()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(74); //[74]	c_dwn_b[c_dwn_b]
	m_AnimationIndices.push_back(115); //[115]	c_dwn_f[c_dwn_f]

	m_AnimationIndices.push_back(216); //[216]	c_standup_dnb_fast[c_standup_dnb_fast]
	m_AnimationIndices.push_back(218); //[218]	c_standup_dnf_fast[c_standup_dnf_fast]
}

void CKiryu_KRC_Down::Tick(const _float& fTimeDelta)
{
	if (!m_isStop && (m_pGameInstance->GetKeyState(DIK_W) == HOLD || m_pGameInstance->GetKeyState(DIK_S) == HOLD
		|| m_pGameInstance->GetKeyState(DIK_A) == HOLD || m_pGameInstance->GetKeyState(DIK_D) == HOLD))
	{
		Stop();
	}

	switch (m_eAnimState)
	{
	case CKiryu_KRC_Down::ANIM_LOOP:
		if (m_isStop)
		{
			m_iCurrentIndex += 2;
			m_eAnimState = ANIM_END;
		}
		break;
	}
}

void CKiryu_KRC_Down::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex]);
}

_bool CKiryu_KRC_Down::Get_AnimationEnd()
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

void CKiryu_KRC_Down::Stop()
{
	m_isStop = true;
}

void CKiryu_KRC_Down::Setting_Value(void* pValue)
{
	_bool* pDownState = static_cast<_bool*>(pValue);

	// 넘겨받은 값이 false면 뒤, true면 앞
	if (*pDownState)
		m_iCurrentIndex = 1;
	else
		m_iCurrentIndex = 0;
}

_bool CKiryu_KRC_Down::Stopping()
{
	if (ANIM_END == m_eAnimState && !Get_AnimationEnd()) return true;

	return false;
}

void CKiryu_KRC_Down::Reset()
{
	m_iCurrentIndex = 0;
	m_eAnimState = { ANIM_LOOP };
	m_isStop = false;
}

CBehaviorAnimation* CKiryu_KRC_Down::Create(CPlayer* pPlayer)
{
	CKiryu_KRC_Down* pInstnace = new CKiryu_KRC_Down();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_Run");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRC_Down::Free()
{
	__super::Free();
}