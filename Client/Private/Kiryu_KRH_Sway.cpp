#include "GameInstance.h"
#include "Kiryu_KRH_Sway.h"
#include "Player.h"

CKiryu_KRH_Sway::CKiryu_KRH_Sway()
	:CBehaviorAnimation{}
{
	// 스웨이는 그냥 키입력이 있으면 back 방향이 기본임
	// 카메라 기준 wasd를 입력받아 스웨이하는지 그 방향으로 스웨이를 하는건 맞는데, 
	// 출력되는 애니메이션은 캐릭터의 Look을 기준으로 나온다.

	/* 0 ~ 3 */
	m_AnimationIndices.push_back(487);	//[487]	p_krh_sway_f[p_krh_sway_f]
	m_AnimationIndices.push_back(486);	//[486]	p_krh_sway_b[p_krh_sway_b]
	m_AnimationIndices.push_back(488);	//[488]	p_krh_sway_l[p_krh_sway_l]
	m_AnimationIndices.push_back(489);	//[489]	p_krh_sway_r[p_krh_sway_r]

	/* 4 ~ 7 */
	m_AnimationIndices.push_back(479);	//[479]	p_krh_sway_02_f[p_krh_sway_02_f]
	m_AnimationIndices.push_back(478);	//[478]	p_krh_sway_02_b[p_krh_sway_02_b]
	m_AnimationIndices.push_back(480);	//[480]	p_krh_sway_02_l[p_krh_sway_02_l]
	m_AnimationIndices.push_back(481);	//[481]	p_krh_sway_02_r[p_krh_sway_02_r]

	/* 8 ~ 11 */
	m_AnimationIndices.push_back(483);	//[483]	p_krh_sway_03_f[p_krh_sway_03_f]
	m_AnimationIndices.push_back(482);	//[482]	p_krh_sway_03_b[p_krh_sway_03_b]
	m_AnimationIndices.push_back(484);	//[484]	p_krh_sway_03_l[p_krh_sway_03_l]
	m_AnimationIndices.push_back(485);	//[485]	p_krh_sway_03_r[p_krh_sway_03_r]
}

void CKiryu_KRH_Sway::Tick(const _float& fTimeDelta)
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

void CKiryu_KRH_Sway::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iDirectionIndex + (4 * m_iComboCount)]);
}

_bool CKiryu_KRH_Sway::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		m_pPlayer->Reset_MoveDirection();
		return true;
	}
	return false;
}

void CKiryu_KRH_Sway::Reset()
{
	m_iComboCount = 0;
}

void CKiryu_KRH_Sway::Combo_Count(_bool isFinAction)
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

_bool CKiryu_KRH_Sway::Changeable_Combo_Animation()
{
	_float fInterval = 0.4f;

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (fInterval < *pModelCom->Get_AnimationCurrentPosition() / *pModelCom->Get_AnimationDuration())
		return true;

	return false;
}

CBehaviorAnimation* CKiryu_KRH_Sway::Create(CPlayer* pPlayer)
{
	CKiryu_KRH_Sway* pInstnace = new CKiryu_KRH_Sway();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRH_Sway");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRH_Sway::Free()
{
	__super::Free();
}