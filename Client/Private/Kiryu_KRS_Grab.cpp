#include "GameInstance.h"
#include "Kiryu_KRS_Grab.h"
#include "Player.h"
#include "Camera.h"
#include "Monster.h"

CKiryu_KRS_Grab::CKiryu_KRS_Grab()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(647);	// 0 [647]	[p_seize_miss]

	/* lapel */
	m_AnimationIndices.push_back(584);	//1 [584] [p_kru_sync_lapel_st]
	m_AnimationIndices.push_back(578);	//2 [578] [p_kru_sync_lapel_lp]
	m_AnimationIndices.push_back(585);	//3 [585] [p_kru_sync_lapel_walk]
	
	m_AnimationIndices.push_back(580);	//4 [580] [p_kru_sync_lapel_off]			// 잡고있다 놓침
	m_AnimationIndices.push_back(581);	//5 [581] [p_kru_sync_lapel_off_hiza]		// 잡고있다가 놓치면서 배맞음
	m_AnimationIndices.push_back(582);	//6 [582] [p_kru_sync_lapel_press]			// 잡고있다가 얼굴맞으면서 놓치고 넘어짐
	
	m_AnimationIndices.push_back(583);	// 7 [583] [p_kru_sync_lapel_resist]		// 잡고 흔들림

	m_AnimationIndices.push_back(574);	// 8 [574] [p_kru_sync_lapel_atk_punch]
	m_AnimationIndices.push_back(575);	// 9 [575] [p_kru_sync_lapel_cmb_01]
	m_AnimationIndices.push_back(576);	// 10 [576] [p_kru_sync_lapel_cmb_02]
	m_AnimationIndices.push_back(577);	// 11 [577] [p_kru_sync_lapel_cmb_03]

	m_AnimationIndices.push_back(579);	// 12 [579] [p_kru_sync_lapel_nage]

	/* neck */
	m_AnimationIndices.push_back(596);	// 13 [596] [p_kru_sync_neck_st]
	m_AnimationIndices.push_back(590);	// 14 [590] [p_kru_sync_neck_lp]
	m_AnimationIndices.push_back(597);	// 15 [597] [p_kru_sync_neck_walk]

	m_AnimationIndices.push_back(592);	// 16 [592] [p_kru_sync_neck_off]
	m_AnimationIndices.push_back(593);	// 17 [593] [p_kru_sync_neck_off_uraken]	// 잡고있다 끌려가면서 놓침
	m_AnimationIndices.push_back(594);	// 18 [594] [p_kru_sync_neck_press]			// 잡고있다가 놓치고 넘어짐
	
	m_AnimationIndices.push_back(595);	// 19 [595] [p_kru_sync_neck_resist]

	m_AnimationIndices.push_back(586);	// 20 [586] [p_kru_sync_neck_atk_kick]
	m_AnimationIndices.push_back(587);	// 21 [587] [p_kru_sync_neck_cmb_01]
	m_AnimationIndices.push_back(588);	// 22 [588] [p_kru_sync_neck_cmb_02]
	m_AnimationIndices.push_back(589);	// 23 [589] [p_kru_sync_neck_cmb_03]
	
	m_AnimationIndices.push_back(591);	// 24 [591] [p_kru_sync_neck_nage]

	/* leg */
	m_AnimationIndices.push_back(700);	// 25 [700]	[p_sync_leg_st_b]
	m_AnimationIndices.push_back(701);	// 26 [701]	[p_sync_leg_st_f]
	m_AnimationIndices.push_back(697);	// 27 [697]	[p_sync_leg_lp]
	m_AnimationIndices.push_back(702);	// 28 [702]	[p_sync_leg_walk]

	m_AnimationIndices.push_back(699);	// 29 [699]	[p_sync_leg_off]

	m_AnimationIndices.push_back(698);	// 30 [698]	[p_sync_leg_nage]

	/* etc */
	m_AnimationIndices.push_back(695);	// 31 [695]	[p_sync_lapel_to_neck]
	
	m_AnimationIndices.push_back(693);	// 32 [693]	[p_sync_head_b]			// 일으켜 세우는거, 누워있는 상대의 머리쪽에서 잡기키 한번 더 누르면 실행된다.
	m_AnimationIndices.push_back(694);	// 33 [694]	[p_sync_head_f]
}

void CKiryu_KRS_Grab::Tick(const _float& fTimeDelta)
{
	CLandObject* pTargetObject = m_pPlayer->Get_TargetObject();

	if (m_iCurrentIndex != 12 && !m_isCutScene)
	{
		if (nullptr != pTargetObject)
		{
			_vector vLookPos = pTargetObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
			m_pPlayer->Get_TransformCom()->LookAt_For_LandObject(vLookPos);
		}
	}

	if (m_isGrabed)
	{
		switch (m_eAnimState)
		{
		case SEIZE_TRY:
		{
			m_pPlayer->Off_Separation_Hand();
			if (Changeable_Combo_Animation())
			{
				m_eAnimState = ANIM_START;
				m_iCurrentIndex = 1;
			}

			break;
		}
		case ANIM_START:
		{
			m_pPlayer->Set_HandAnimIndex(CPlayer::HAND_GU);
			m_pPlayer->On_Separation_Hand();

			CMonster* pTargetMonster = static_cast<CMonster*>(m_pPlayer->Get_TargetObject());
			if (nullptr != pTargetMonster)
				pTargetMonster->Set_Sync("p_kru_sync_lapel_st", true);

			if (Changeable_Combo_Animation())
			{
				m_eAnimState = ANIM_LOOP;
				m_iCurrentIndex = 2;
			}

			break;
		}
		case ANIM_LOOP:
		{
			m_pPlayer->Set_HandAnimIndex(CPlayer::HAND_GU);
			m_pPlayer->On_Separation_Hand();

			CMonster* pTargetMonster = static_cast<CMonster*>(m_pPlayer->Get_TargetObject());
			if (nullptr != pTargetMonster)
			{
				if(m_pPlayer->Get_CurrentAnimType() != CPlayer::CUTSCENE)
					pTargetMonster->Set_Sync("p_kru_sync_lapel_lp", true);
			}

			// 여기에 키인풋
			Move_KeyInput(fTimeDelta);
			break;
		}
		case ANIM_ONCE:
		{
			if (m_isStop)
			{
				CMonster* pTargetMonster = static_cast<CMonster*>(m_pPlayer->Get_TargetObject());
				if (nullptr != pTargetMonster)
					pTargetMonster->Set_Sync("p_kru_sync_lapel_nage");
			}
			break;
		}
		default:
			break;
		}
	}

	Shaking();
}

void CKiryu_KRS_Grab::Change_Animation()
{
	if (0 > m_iCurrentIndex) return;

	m_pPlayer->Off_Separation_Hand();
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex]);
}

_bool CKiryu_KRS_Grab::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));

	if (pModelCom->Get_AnimFinished())
	{
		// 잡지못했을 때에는 애니메이션 스테이트 관계없이 종료를 반환한다.
		if (!m_isGrabed)
		{
			Reset();
			return true;
		}

		// (ANIM_START == m_eAnimState는 그랩 시작 모션이라는 뜻
		if (ANIM_START == m_eAnimState)
		{
			m_eAnimState = ANIM_LOOP;
		}
		// 그랩 시도 모션이고 그랩에 성공했다면 그랩 시작모션으로 인덱스를 세팅해준다.
		if (SEIZE_TRY == m_eAnimState)
		{
			m_eAnimState = ANIM_START;
		}

		if (ANIM_ONCE == m_eAnimState)
		{
			if(m_isStop)
				m_eAnimState = ANIM_END;
			else
				m_eAnimState = ANIM_LOOP;
				
		}

		else if (ANIM_END == m_eAnimState)
		{
			Reset();
			return true;
		}
	}

	return false;
}

void CKiryu_KRS_Grab::Reset()
{
	m_iComboCount = 0;
	m_iDirection = -1;
	m_eAnimState = SEIZE_TRY;
	m_isGrabed = false;
	m_isShaked = false;
	m_isStop = false;
	m_isCutScene = false;
	m_iCurrentIndex = 0;
}

void CKiryu_KRS_Grab::Combo_Count(_bool isFinAction)
{
	if (Changeable_Combo_Animation())
	{
		m_iComboCount++;

		if (m_iComboCount > 2)
			m_iComboCount = 0;
	}
}

void CKiryu_KRS_Grab::Stop()
{
	m_isStop = true;
}

// 여기서는 따로 넘겨받는 값 없이 
void CKiryu_KRS_Grab::Setting_Value(void* pValue)
{
	KRS_Grab_DESC* pDesc = static_cast<KRS_Grab_DESC*>(pValue);
	m_isGrabed = pDesc->isGrabed;
	m_iDirection = pDesc->iDirection;
}

void CKiryu_KRS_Grab::Event(void* pValue)
{
	_bool* pIsOff = static_cast<_bool*>(pValue);

	if (pIsOff)
		Play_Off();
}

_bool CKiryu_KRS_Grab::Changeable_Combo_Animation()
{
	_float fInterval = 0.8f;

	if (0 == m_iCurrentIndex)
	{
		fInterval = 0.2f;
	}

	if (Checked_Animation_Ratio(fInterval))
	{
		m_isShaked = false;
		return true;
	}

	return false;
}

void CKiryu_KRS_Grab::Shaking()
{
	if (m_iCurrentIndex == 12)						// 잡고 바닥에 내려찍는 애니메이션
	{
		if (!m_isShaked && Checked_Animation_Ratio(0.4))
		{
			m_isShaked = true;
			//카메라 쉐이킹
			CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
			pCamera->Set_Shaking(true, { 1.f, 1.f, 0.f }, 0.3, 0.4);
		}
	}
}

void CKiryu_KRS_Grab::Play_Off()
{
}

void CKiryu_KRS_Grab::Move_KeyInput(const _float& fTimeDelta)
{
	if (m_pGameInstance->GetMouseState(DIM_RB) == TAP)
	{
		if (2 < m_pPlayer->Get_CurrentHitLevel())	// 히트게이지가 3단까지 풀이라면
		{
			m_pPlayer->Set_CutSceneAnim(CPlayer::GOUGEKI_C);

			m_isCutScene = true;
		}
	}
	if (m_pGameInstance->GetKeyState(DIK_Q) == TAP)
	{
		m_iCurrentIndex = 12;
		m_eAnimState = ANIM_ONCE;
		m_isStop = true;

		CMonster* pTargetMonster = static_cast<CMonster*>(m_pPlayer->Get_TargetObject());
		if (nullptr != pTargetMonster)
			pTargetMonster->Set_Sync("p_kru_sync_lapel_nage");
	}

	//3 [585] [p_kru_sync_lapel_walk]
	if (m_pGameInstance->GetKeyState(DIK_S) == HOLD)
	{
		m_iCurrentIndex = 3;
		m_eAnimState = ANIM_LOOP;
	}
	else if (m_pGameInstance->GetKeyState(DIK_S) == AWAY)
	{
		m_iCurrentIndex = 2;
		m_eAnimState = ANIM_LOOP;
	}
}

CBehaviorAnimation* CKiryu_KRS_Grab::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_Grab* pInstnace = new CKiryu_KRS_Grab();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_KickCombo");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_Grab::Free()
{
	__super::Free();
}