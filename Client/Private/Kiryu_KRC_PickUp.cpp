#include "GameInstance.h"
#include "Kiryu_KRC_PickUp.h"
#include "Player.h"
#include "Camera.h"
#include "Monster.h"
#include "Item.h"

CKiryu_KRC_PickUp::CKiryu_KRC_PickUp()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(742);	// 0 [742]	[p_wpc_pickup]			// 직접 주웠을 때
	m_AnimationIndices.push_back(367);	// 1 [367]	[p_krc_wpc_stand]

	// 이동
	m_AnimationIndices.push_back(362);	// 2 [362]	[p_krc_wpc_shift_f]

	//이미 들고있는 상태에서의 콤보
	m_AnimationIndices.push_back(344);	// 3 [344]	[p_krc_wpc_cmb_01]
	m_AnimationIndices.push_back(348);	// 4 [348]	[p_krc_wpc_cmb_02]
	m_AnimationIndices.push_back(352);	// 5 [352]	[p_krc_wpc_cmb_03]
	m_AnimationIndices.push_back(371);	// 6 [371]	[p_krc_wpc_throw_sp]

	//이미 들고있는 상태에서의 콤보
	m_AnimationIndices.push_back(345);	// 7 [345]	[p_krc_wpc_cmb_01_fin]
	m_AnimationIndices.push_back(349);	// 8 [349]	[p_krc_wpc_cmb_02_fin]

	// 공격하면서 주웠을 때
	m_AnimationIndices.push_back(339);	// 9 [339]	[p_krc_wpc_atk_pickup_l]
	m_AnimationIndices.push_back(346);	// 10 [346]	[p_krc_wpc_cmb_01_pickup_l]
	m_AnimationIndices.push_back(350);	// 11 [350]	[p_krc_wpc_cmb_02_pickup_l]
	m_AnimationIndices.push_back(353);	// 12 [353]	[p_krc_wpc_cmb_03_pickup_l]

	m_AnimationIndices.push_back(340);	// 13 [340]	[p_krc_wpc_atk_pickup_r]
	m_AnimationIndices.push_back(347);	// 14 [347]	[p_krc_wpc_cmb_01_pickup_r]
	m_AnimationIndices.push_back(351);	// 15 [351]	[p_krc_wpc_cmb_02_pickup_r]
	m_AnimationIndices.push_back(354);	// 16 [354]	[p_krc_wpc_cmb_03_pickup_r]

	m_AnimationIndices.push_back(34);	// 17 Damage
}

void CKiryu_KRC_PickUp::Tick(const _float& fTimeDelta)
{
	// 콤보중일때만 Look 시킨다.
	if (0 < m_iComboCount)
	{
		CLandObject* pTargetObject = m_pPlayer->Get_TargetObject();

		if (nullptr != pTargetObject)
		{
			_vector vLookPos = pTargetObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
			m_pPlayer->Get_TransformCom()->LookAt_For_LandObject(vLookPos);
		}
	}

	//KRC 픽업 애니메이션은 이미 손 애니메이션이 들어가있어서 기존에 적용되어있는걸 제거
	m_pPlayer->Off_Separation_Hand();

	switch (m_eAnimState)
	{
	case ANIM_START:
	{
		if (Changeable_Combo_Animation())
		{
			m_eAnimState = ANIM_LOOP;
			m_iCurrentIndex = 1;
		}

		break;
	}
	case ANIM_LOOP:
		Move_KeyInput(fTimeDelta);
		Attack_KeyInput(fTimeDelta);


		break;
	case ANIM_ONCE:
		Attack_KeyInput(fTimeDelta);

		if (m_eBehaviorState == ATTACK)
		{
			if (m_iComboCount == 3)
			{
				if (Changeable_Combo_Animation())
				{
					_float4 vDir;
					XMStoreFloat4(&vDir, m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK));

					CItem::THROW_INFO_DESC Desc{
						vDir, 25.f
					};

					CItem* pItem = m_pPlayer->Get_CurrentItem();
					pItem->Throw_On(Desc);
					m_isStop = true;
				}
			}
		}

		break;
	default:
		break;
	}

	if (m_isHit)
		Play_Hit();

	//Shaking();
}

void CKiryu_KRC_PickUp::Change_Animation()
{
	if (0 > m_iCurrentIndex) return;

	m_pPlayer->Off_Separation_Hand();
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex + m_iComboCount]);
}

_bool CKiryu_KRC_PickUp::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));

	// 아이템 라이프 끝났으면 상태 되돌리기
	if (m_pPlayer->Get_CurrentItem()->Get_ItemLife() < 0)
	{
		Reset();
		return true;
	}

	if (pModelCom->Get_AnimFinished())
	{
		// 잡지못했을 때에는 애니메이션 스테이트 관계없이 종료를 반환한다.
		if (ANIM_START == m_eAnimState)
		{
			m_eAnimState = ANIM_LOOP;
		}
		if (ANIM_ONCE == m_eAnimState)
		{
			if(m_isStop)
				m_eAnimState = ANIM_END;
			else
			{
				m_iComboCount = 0;			// 뭐든지 루프로 돌아갈때에는 콤보카운트를 초기화해야한다
				m_eAnimState = ANIM_LOOP;
			}
		}

		else if (ANIM_END == m_eAnimState)
		{
			Reset();
			return true;
		}
	}

	return false;
}

void CKiryu_KRC_PickUp::Reset()
{
	m_iComboCount = 0;
	m_iDirection = -1;
	m_eAnimState = ANIM_START;
	m_isShaked = false;
	m_isStop = false;
	m_iCurrentIndex = 0;
}

void CKiryu_KRC_PickUp::Combo_Count(_bool isFinAction)
{
	m_iCurrentIndex = 3;

	m_eBehaviorState = ATTACK;

	if (Changeable_Combo_Animation())
	{
		m_iComboCount++;

		if (m_iComboCount > 3)
		{
			m_iComboCount = 0;
			m_isStop = true;
		}
	}
}

void CKiryu_KRC_PickUp::Stop()
{
	m_isStop = true;
}

// 여기서는 따로 넘겨받는 값 없이 
void CKiryu_KRC_PickUp::Setting_Value(void* pValue)
{
	// 어깨 이후로 뼈를 분리해서 어깨 이후뼈는 그대로 유지하고, 그 전 뼈들은 히트모션에 맞는 애니메이션을 꺼내와서 실행하는 식으로 해야한다
	KRC_PICK_UP_HIT_DESC* pDesc = static_cast<KRC_PICK_UP_HIT_DESC*>(pValue);

	m_pPlayer->Separation_Bone("mune_c_n", CPlayer::DEFAULT_ANIM, false);		// 기존뼈들을 현재 들고있는 모션대로 돌린다.
	//m_pPlayer->Separation_Bone("ude1_r_n", CPlayer::DEFAULT_ANIM);

	m_AnimationIndices[17] = pDesc->iHitAnimIndex;
	m_iCurrentIndex = 17;		// 모델에서 돌리는 애니메이션 
	m_iComboCount = 0;

	m_isHit = true;
}

void CKiryu_KRC_PickUp::Event(void* pValue)
{
	KRC_PICK_UP_DESC* pDesc = static_cast<KRC_PICK_UP_DESC*>(pValue);

	m_iComboCount = pDesc->iComboCount;
	Play_PickupCombo(pDesc->isLeft);
}

_bool CKiryu_KRC_PickUp::Changeable_Combo_Animation()
{
	_float fInterval = 0.4f;

	if (m_iComboCount == 2)
		fInterval = 0.6f;

	if (m_eBehaviorState == ATTACK)
	{
		if (m_iComboCount == 3)
		{
			fInterval = 0.3f;
		}
	}

	if (Checked_Animation_Ratio(fInterval))
	{
		m_eBehaviorState = NONE_ATTAK;
		m_isShaked = false;
		return true;
	}

	return false;
}

void CKiryu_KRC_PickUp::Shaking()
{
	if (m_iCurrentIndex == 6)
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

void CKiryu_KRC_PickUp::Play_Hit()
{
	//CAnim* pAnim = static_cast<CAnim*>(m_pPlayer->Get_Component(TEXT("Com_Anim_Default")));
	//if (pAnim->Get_AnimFinished())

	CModel* pModel = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));

	if (pModel->Get_AnimFinished())
	{
		m_isHit = false;
		m_pPlayer->Separation_Bone("mune_c_n", -1);		// 기존뼈들을 현재 들고있는 모션대로 돌린다.
		//m_pPlayer->Separation_Bone("ude1_r_n", -1);
		//pModel->Reset_Animation();
	}
}

void CKiryu_KRC_PickUp::Play_PickupCombo(_bool isLeft)
{
	/*
	m_AnimationIndices.push_back(339);	// 9 [339]	[p_krc_wpc_atk_pickup_l]
	m_AnimationIndices.push_back(346);	// 10 [346]	[p_krc_wpc_cmb_01_pickup_l]
	m_AnimationIndices.push_back(350);	// 11 [350]	[p_krc_wpc_cmb_02_pickup_l]
	m_AnimationIndices.push_back(353);	// 12 [353]	[p_krc_wpc_cmb_03_pickup_l]

	m_AnimationIndices.push_back(340);	// 13 [340]	[p_krc_wpc_atk_pickup_r]
	m_AnimationIndices.push_back(347);	// 14 [347]	[p_krc_wpc_cmb_01_pickup_r]
	m_AnimationIndices.push_back(351);	// 15 [351]	[p_krc_wpc_cmb_02_pickup_r]
	m_AnimationIndices.push_back(354);	// 16 [354]	[p_krc_wpc_cmb_03_pickup_r]
	*/
	if (isLeft)
		m_iCurrentIndex = 9;
	else
		m_iCurrentIndex = 13;
}

void CKiryu_KRC_PickUp::Move_KeyInput(const _float& fTimeDelta)
{
	_bool isMove = false;

	if (m_pGameInstance->GetKeyState(DIK_W) == HOLD)
	{
		_vector vCamLook = m_pGameInstance->Get_CamLook();
		_vector vLookPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK) + vCamLook);

		vLookPos = XMVectorSetY(vLookPos, XMVectorGetY(m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION)));

		if (!XMVector3Equal(vLookPos, XMVectorZero())) {
			m_pPlayer->Get_TransformCom()->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}
	}

	if (m_pGameInstance->GetKeyState(DIK_S) == HOLD)
	{
		_vector vCamLook = m_pGameInstance->Get_CamLook();
		_vector vLookPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK) - vCamLook);

		vLookPos = XMVectorSetY(vLookPos, XMVectorGetY(m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION)));

		if (!XMVector3Equal(vLookPos, XMVectorZero())) {
			m_pPlayer->Get_TransformCom()->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}
	}

	if (m_pGameInstance->GetKeyState(DIK_A) == HOLD)
	{
		_vector vCamLook = m_pGameInstance->Get_CamLook();

		_vector vLookPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize((m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK) - m_pGameInstance->Get_CamRight()));

		vLookPos = XMVectorSetY(vLookPos, XMVectorGetY(m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION)));

		if (!XMVector3Equal(vLookPos, XMVectorZero())) {
			m_pPlayer->Get_TransformCom()->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}
	}

	if (m_pGameInstance->GetKeyState(DIK_D) == HOLD)
	{
		_vector vCamLook = m_pGameInstance->Get_CamLook();

		_vector vLookPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK) + m_pGameInstance->Get_CamRight());

		vLookPos = XMVectorSetY(vLookPos, XMVectorGetY(m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION)));

		if (!XMVector3Equal(vLookPos, XMVectorZero())) {
			m_pPlayer->Get_TransformCom()->LookAt_For_LandObject(vLookPos);
			isMove = true;
		}
	}

	if (isMove && !m_isHit)
		m_iCurrentIndex = 2;
	else if(!m_isHit)
		m_iCurrentIndex = 1;
}

void CKiryu_KRC_PickUp::Attack_KeyInput(const _float& fTimeDelta)
{
	if (m_pGameInstance->GetMouseState(DIM_LB) == TAP)
	{
		Combo_Count();
		m_eAnimState = ANIM_ONCE;
		m_pPlayer->Get_CurrentItem()->Decrease_Life();
	}
	if (m_pGameInstance->GetMouseState(DIM_RB) == TAP)
	{
		if (m_pPlayer->CanHitAction())
		{
			m_pPlayer->Set_CutSceneAnim(CPlayer::DORAMUKAN_88);
		}
	}
}

CBehaviorAnimation* CKiryu_KRC_PickUp::Create(CPlayer* pPlayer)
{
	CKiryu_KRC_PickUp* pInstnace = new CKiryu_KRC_PickUp();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_KickCombo");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRC_PickUp::Free()
{
	__super::Free();
}