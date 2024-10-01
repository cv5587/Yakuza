#include "GameInstance.h"
#include "Kiryu_Adventure_Run.h"
#include "Player.h"
#include "Model.h"

CKiryu_Adventure_Run::CKiryu_Adventure_Run()
	:CBehaviorAnimation{}
{
	// 시작 애니메이션들
	m_AnimationIndices.push_back(629);	//	[629]	[p_mov_run_start_l180]
	m_AnimationIndices.push_back(630);	//	[630]	[p_mov_run_start_l90]
	m_AnimationIndices.push_back(631);	//	[631]	[p_mov_run_start_r180]
	m_AnimationIndices.push_back(632);	//	[632]	[p_mov_run_start_r90]

	// 달리는 루프 애니메이션들
	m_AnimationIndices.push_back(624); 	//	[624]	[p_mov_run_kyoro]
	m_AnimationIndices.push_back(625); 	//	[625]	[p_mov_run_l]
	m_AnimationIndices.push_back(626); 	//	[626]	[p_mov_run_r]

	// 끝날 때 애니메이션
	m_AnimationIndices.push_back(633);	//	[633]	[p_mov_run_stop]
	m_AnimationIndices.push_back(622);	//	[622]	[p_mov_run_dash_tired_stop_l]
	m_AnimationIndices.push_back(623);	//	[623]	[p_mov_run_dash_tired_stop_s]

	// 대시 (어따쓰는진 모르겟음)
	m_AnimationIndices.push_back(620);	// [620]	[p_mov_run_dash]
	m_AnimationIndices.push_back(621);	// [621]	[p_mov_run_dash_tired]

	// 범프 (부딪힐때)
	m_AnimationIndices.push_back(601); //[601]	[p_mov_bump_car_f]
	m_AnimationIndices.push_back(602); //[602]	[p_mov_bump_car_l]
	m_AnimationIndices.push_back(603); //[603]	[p_mov_bump_car_r]
	m_AnimationIndices.push_back(604); //[604]	[p_mov_bump_f]
	m_AnimationIndices.push_back(605); //[605]	[p_mov_bump_l]
	m_AnimationIndices.push_back(606); //[606]	[p_mov_bump_r]
	m_AnimationIndices.push_back(607); //[607]	[p_mov_bump_wall_l]
	m_AnimationIndices.push_back(608); //[608]	[p_mov_bump_wall_r]


	//데미지 입은 상태일 때																 
	m_AnimationIndices.push_back(610); //[610]	[p_mov_dam_lp]
	m_AnimationIndices.push_back(611); //[611]	[p_mov_dam_run]
	m_AnimationIndices.push_back(613); //[613]	[p_mov_dam_walk]
	m_AnimationIndices.push_back(614); //[614]	[p_mov_dam_walk_turnl]
	m_AnimationIndices.push_back(615); //[615]	[p_mov_dam_walk_turnr]

	m_iCurrentIndex = 0;
}

void CKiryu_Adventure_Run::Tick(const _float& fTimeDelta)
{
	switch (m_eAnimState)
	{
	case CKiryu_Adventure_Run::ANIM_START:
	{
		CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));

		if (pModelCom->Get_AnimFinished())
			m_eAnimState = ANIM_LOOP;
	}
		break;
	case CKiryu_Adventure_Run::ANIM_LOOP:
			m_eAnimState = ANIM_END;
		break;
	}
}

void CKiryu_Adventure_Run::Change_Animation()
{
	m_pPlayer->Set_HandAnimIndex(CPlayer::HAND_MIDDLE2);
	m_pPlayer->On_Separation_Hand();

	const _bool* pDirction = m_pPlayer->Get_MoveDirection();

	if (m_eAnimState == ANIM_START)
	{
		//// 정면은 달리기 시작하는 모션이 없다!
		//if (pDirction[CPlayer::F])
		//{
		//	m_eAnimState = ANIM_LOOP;
		//	m_iCurrentIndex = 4;
		//}
		//if (pDirction[CPlayer::B])
		//	m_iCurrentIndex = 0;
		//if (pDirction[CPlayer::L])
		//	m_iCurrentIndex = 1;
		//if (pDirction[CPlayer::R])
		//	m_iCurrentIndex = 3;

		m_iCurrentIndex = 4;

	}
	else if (m_eAnimState == ANIM_LOOP)
	{
		//if ((pDirction[CPlayer::B] || pDirction[CPlayer::F]) && pDirction[CPlayer::L])
		//	m_iCurrentIndex = 5;
		//else if ((pDirction[CPlayer::B] || pDirction[CPlayer::F]) && pDirction[CPlayer::R])
		//	m_iCurrentIndex = 6;
		//else
			m_iCurrentIndex = 4;
	}
	else if (m_eAnimState == ANIM_END)
	{
		m_iCurrentIndex = 7;
	}
		
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex]);
}

_bool CKiryu_Adventure_Run::Get_AnimationEnd()
{
	if (m_eAnimState != ANIM_END) return false;

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));

	if (pModelCom->Get_AnimFinished())
	{
		m_eAnimState = { ANIM_START };

		return true;
	}

	return false;
}

void CKiryu_Adventure_Run::Stop()
{
	m_eAnimState = ANIM_END;
}


CBehaviorAnimation* CKiryu_Adventure_Run::Create(CPlayer* pPlayer)
{
	CKiryu_Adventure_Run* pInstnace = new CKiryu_Adventure_Run();

	if (nullptr == pInstnace)
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_Adventure_Run");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_Adventure_Run::Free()
{
	__super::Free();
}
