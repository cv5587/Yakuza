#include "GameInstance.h"
#include "Kiryu_KRS_Sway.h"
#include "Player.h"

CKiryu_KRS_Sway::CKiryu_KRS_Sway()
	:CBehaviorAnimation{}
{
	// 스웨이는 그냥 키입력이 있으면 back 방향이 기본임
	// 카메라 기준 wasd를 입력받아 스웨이하는지 그 방향으로 스웨이를 하는건 맞는데, 
	// 출력되는 애니메이션은 캐릭터의 Look을 기준으로 나온다.
	m_AnimationIndices.push_back(570);	//[570]	p_krs_sway_f[p_krs_sway_f]
	m_AnimationIndices.push_back(569);	//+[569]	p_krs_sway_b[p_krs_sway_b]
	m_AnimationIndices.push_back(571);	//[571]	p_krs_sway_l[p_krs_sway_l]
	m_AnimationIndices.push_back(572);	//[572]	p_krs_sway_r[p_krs_sway_r]
}

void CKiryu_KRS_Sway::Tick(const _float& fTimeDelta)
{
	const _bool* pDirection = m_pPlayer->Get_MoveDirection();

	if (pDirection[CPlayer::F])
		m_iCurrentIndex = CPlayer::F;
	else if (pDirection[CPlayer::L])
		m_iCurrentIndex = CPlayer::L;
	else if (pDirection[CPlayer::R])
		m_iCurrentIndex = CPlayer::R;
	else
		m_iCurrentIndex = CPlayer::B;
}

void CKiryu_KRS_Sway::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex]);
}

_bool CKiryu_KRS_Sway::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		m_pPlayer->Reset_MoveDirection();
		return true;
	}
	return false;
}

CBehaviorAnimation* CKiryu_KRS_Sway::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_Sway* pInstnace = new CKiryu_KRS_Sway();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_Sway");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_Sway::Free()
{
	__super::Free();
}