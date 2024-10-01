#include "GameInstance.h"
#include "Kiryu_KRC_Guard.h"
#include "Kiryu_KRC_Hit.h"
#include "Player.h"
#include "Camera.h"

CKiryu_KRC_Guard::CKiryu_KRC_Guard()
	:CBehaviorAnimation{}
{
	/*
	*	// 피격
		[166]	p_krc_guard_body_b[p_krc_guard_body_b]
		[167]	p_krc_guard_body_f[p_krc_guard_body_f]
		[169]	p_krc_guard_head_l[p_krc_guard_head_l]
		[170]	p_krc_guard_head_r[p_krc_guard_head_r]

		// 빠른 가드 (스킬임)
		[172]	p_krc_guard_run_en[p_krc_guard_run_en]
		[173]	p_krc_guard_run_lp[p_krc_guard_run_lp]
		[174]	p_krc_guard_run_st[p_krc_guard_run_st]

		// 기본 모션
		[168]	p_krc_guard_en[p_krc_guard_en]
		[171]	p_krc_guard_lp[p_krc_guard_lp]
		[175]	p_krc_guard_st[p_krc_guard_st]

		//가드 반격기들
		[149]	p_krc_atk_guard_tame[p_krc_atk_guard_tame]		//점프해서 내려찍음 (우클)
		[148]	p_krc_atk_guard_heavy[p_krc_atk_guard_heavy]	// 돌면서 주먹 휘두름 (좌클)

	*/

	/* 0 ~ 2 */
	m_AnimationIndices.push_back(292); // [292]	p_krc_guard_st[p_krc_guard_st]
	m_AnimationIndices.push_back(288); // [288]	p_krc_guard_lp[p_krc_guard_lp]
	m_AnimationIndices.push_back(285); // [285]	p_krc_guard_en[p_krc_guard_en]

	/* 3 ~ 6 */
	m_AnimationIndices.push_back(283);	//[283]	p_krc_guard_body_b[p_krc_guard_body_b]
	m_AnimationIndices.push_back(284);	//[284]	p_krc_guard_body_f[p_krc_guard_body_f]
	m_AnimationIndices.push_back(286);	//[286]	p_krc_guard_head_l[p_krc_guard_head_l]
	m_AnimationIndices.push_back(287);	//[287]	p_krc_guard_head_r[p_krc_guard_head_r]

	/* 7 ~ 8 */
	m_AnimationIndices.push_back(265);	//[265]	p_krc_atk_guard_heavy[p_krc_atk_guard_heavy]
	m_AnimationIndices.push_back(266);	//[266]	p_krc_atk_guard_tame[p_krc_atk_guard_tame]
}

void CKiryu_KRC_Guard::Tick(const _float& fTimeDelta)
{
	switch (m_eAnimState)
	{
	case CKiryu_KRC_Guard::ANIM_START:
		if (Get_AnimationEnd())
		{
			m_eAnimState = ANIM_LOOP;
		}
		break;
	case CKiryu_KRC_Guard::ANIM_LOOP:
		if (m_isStop)
			m_eAnimState = ANIM_END;
		break;
	}


	if (m_eCurrentType == HIT)
	{
		if (m_pGameInstance->GetMouseState(DIM_LB) == TAP)
		{
			m_isShaked = true;
			m_eCurrentType = COUNTER_ATTACK;
			m_iIndex = 7;
		}

		if (m_pGameInstance->GetMouseState(DIM_RB) == TAP)
		{
			m_isShaked = true;
			m_eCurrentType = COUNTER_ATTACK;
			m_iIndex = 8;
		}
	}
	else if (m_eCurrentType == COUNTER_ATTACK)
	{
		CLandObject* pTargetObject = reinterpret_cast<CLandObject*>(m_pPlayer->Get_TargetObject());

		if (nullptr != pTargetObject)
		{
			_vector vLookPos = pTargetObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
			m_pPlayer->Get_TransformCom()->LookAt_For_LandObject(vLookPos);
		}
	}

	Shaking();
}

void CKiryu_KRC_Guard::Change_Animation()
{
	if (m_iCurrentIndex > 0) m_eAnimState = ANIM_LOOP;

	if(m_eCurrentType != COUNTER_ATTACK)
		m_iIndex = m_eAnimState + m_iCurrentIndex;


	m_pPlayer->Change_Animation(m_AnimationIndices[m_iIndex >= m_AnimationIndices.size() ? 3 : m_iIndex]);
}

_bool CKiryu_KRC_Guard::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));

	if (pModelCom->Get_AnimFinished())
	{
		if (m_eCurrentType == COUNTER_ATTACK)
		{
			m_eAnimState = ANIM_LOOP;
			m_eCurrentType = GUARD;
		}
		
		m_iCurrentIndex = 0;
		m_iIndex = 0;
		m_isShaked = false;

		return true;
	}

	return false;
}

void CKiryu_KRC_Guard::Stop()
{
	m_isStop = true;
}

// 유일하게 Stop했는지 용도로 사용할 것이다.
_bool CKiryu_KRC_Guard::Stopping()
{
	if (ANIM_END == m_eAnimState && Get_AnimationEnd()) return true;

	return false;
}

void CKiryu_KRC_Guard::Reset()
{
	m_eAnimState = ANIM_START;
	m_eCurrentType = GUARD;
	m_iCurrentIndex = 0;
	m_isStop = false;
	m_isShaked = false;
}

void CKiryu_KRC_Guard::Setting_Value(void* pValue)
{
	if (m_eCurrentType != COUNTER_ATTACK)
	{
		CKiryu_KRC_Hit::KRC_Hit_DESC* pDesc = static_cast<CKiryu_KRC_Hit::KRC_Hit_DESC*>(pValue);

		// m_eAnimState 에 합해서 쓸려고 그냥 2 뺀 값으로 세팅해줌 
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 3;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 2;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 4;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 5;
			break;
		}
		}
		m_eCurrentType = HIT;

		// 쉐이킹
		m_isShaked = true;

		// 애니메이션을 초기화하고 다시시작하는 함수를 실행시켜야한다.
		m_pPlayer->Change_ResetAnimaition(m_AnimationIndices[m_iIndex]);
	}
}

void CKiryu_KRC_Guard::Shaking()
{
	if (m_isShaked && Checked_Animation_Ratio(0.2))
	{
		//카메라 쉐이킹
		CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));

		if(7 == m_iIndex)
			pCamera->Set_Shaking(true, { 1.f, 1.f, 0.f }, 0.2, 0.4);
		else if (8 == m_iIndex)
		{
			if (Checked_Animation_Ratio(0.4))
			{
				pCamera->Set_Shaking(true, { 1.f, 1.f, 0.f }, 0.2, 0.4);
			}
		}
		else
			pCamera->Set_Shaking(true, { 1.f, 1.f, 0.f }, 0.2, 0.3);
	}
}

CBehaviorAnimation* CKiryu_KRC_Guard::Create(CPlayer* pPlayer)
{
	CKiryu_KRC_Guard* pInstnace = new CKiryu_KRC_Guard();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRH_Gaurd");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRC_Guard::Free()
{
	__super::Free();
}