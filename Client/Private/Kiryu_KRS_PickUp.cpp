#include "GameInstance.h"
#include "Kiryu_KRS_PickUp.h"
#include "Player.h"
#include "Camera.h"
#include "Monster.h"
#include "Item.h"

CKiryu_KRS_PickUp::CKiryu_KRS_PickUp()
	:CBehaviorAnimation{}
{
	/*
		[733]	[p_wpc_cmb_01]
		[734]	[p_wpc_cmb_02]
		[735]	[p_wpc_cmb_03]

		[742]	[p_wpc_pickup]

		[743]	[p_wpc_shift_b]
		[744]	[p_wpc_shift_bl]
		[745]	[p_wpc_shift_br]
		[746]	[p_wpc_shift_f]
		[747]	[p_wpc_shift_fl]
		[748]	[p_wpc_shift_fr]
		[749]	[p_wpc_shift_l]
		[750]	[p_wpc_shift_r]

		[751]	[p_wpc_stand]
		[755]	[p_wpc_throw]
	*/

	m_AnimationIndices.push_back(742);	// 0 [742]	[p_wpc_pickup]
	m_AnimationIndices.push_back(751);	// 1 [751]	[p_wpc_stand]

	m_AnimationIndices.push_back(733);	// 2 [733]	[p_wpc_cmb_01]
	m_AnimationIndices.push_back(734);	// 3 [734]	[p_wpc_cmb_02]
	m_AnimationIndices.push_back(735);	// 4 [735]	[p_wpc_cmb_03]

	m_AnimationIndices.push_back(746);	// 5 [746]	[p_wpc_shift_f]

	m_AnimationIndices.push_back(755);	// 6 [755]	[p_wpc_throw]

	m_AnimationIndices.push_back(34);	// Damage
}

void CKiryu_KRS_PickUp::Tick(const _float& fTimeDelta)
{
	// 콤보중이거나 던질때만 Look 시킨다.
	if (0 < m_iComboCount || 6 == m_iCurrentIndex)
	{
		CLandObject* pTargetObject = m_pPlayer->Get_TargetObject();

		if (nullptr != pTargetObject)
		{
			_vector vLookPos = pTargetObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
			m_pPlayer->Get_TransformCom()->LookAt_For_LandObject(vLookPos);
		}
	}

	switch (m_eAnimState)
	{
	case ANIM_START:
	{
		m_pPlayer->Set_HandAnimIndex(CPlayer::HAND_HAKO);
		m_pPlayer->On_Separation_Hand();
		if (Changeable_Combo_Animation())
		{
			m_eAnimState = ANIM_LOOP;
			m_iCurrentIndex = 1;
		}

		break;
	}
	case ANIM_LOOP:
		m_pPlayer->Set_HandAnimIndex(CPlayer::HAND_HAKO);
		m_pPlayer->On_Separation_Hand();
		Move_KeyInput(fTimeDelta);
		Attack_KeyInput(fTimeDelta);


		break;
	case ANIM_ONCE:
		m_pPlayer->Set_HandAnimIndex(CPlayer::HAND_HAKO);
		m_pPlayer->On_Separation_Hand();
		Attack_KeyInput(fTimeDelta);

		if (m_iCurrentIndex == 6)
		{
			// 던지는 모션일 때, 애니메이션이 일정 이상 실행된 뒤, 플레이어가 바라보는 방향으로 날아간다.
			if (Changeable_Combo_Animation())
			{
				_float4 vDir;
				XMStoreFloat4(&vDir, m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK));

				CItem::THROW_INFO_DESC Desc{
					vDir, 25.f
				};

				CItem* pItem = m_pPlayer->Get_CurrentItem();
				pItem->Throw_On(Desc);


				m_pPlayer->Get_CurrentItem()->Set_Dissolve();
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

void CKiryu_KRS_PickUp::Change_Animation()
{
	if (0 > m_iCurrentIndex) return;

	m_pPlayer->Off_Separation_Hand();
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex + m_iComboCount]);
}

_bool CKiryu_KRS_PickUp::Get_AnimationEnd()
{
	// 아이템 라이프 끝났으면 상태 되돌리기
	if (Checked_Animation_Ratio(0.6f))
	{
		if (m_pPlayer->Get_CurrentItem()->Get_ItemLife() <= 0)
		{
			m_pPlayer->Set_CurrentBehavior(1);
			Reset();
			return true;
		}
	}

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));

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

void CKiryu_KRS_PickUp::Reset()
{
	m_iComboCount = 0;
	m_iDirection = -1;
	m_eAnimState = ANIM_START;
	m_isShaked = false;
	m_isStop = false;
	m_iCurrentIndex = 0;

	m_pPlayer->Get_CurrentItem()->Attacking(false);
}

void CKiryu_KRS_PickUp::Combo_Count(_bool isFinAction)
{
	m_pPlayer->Get_CurrentItem()->Attacking();
	m_iCurrentIndex = 2;
	if (Changeable_Combo_Animation())
	{
		m_iComboCount++;

		if (m_iComboCount > 2)
			m_iComboCount = 0;
	}
}

void CKiryu_KRS_PickUp::Stop()
{
	m_isStop = true;
}

// 여기서는 따로 넘겨받는 값 없이 
void CKiryu_KRS_PickUp::Setting_Value(void* pValue)
{
	// 어깨 이후로 뼈를 분리해서 어깨 이후뼈는 그대로 유지하고, 그 전 뼈들은 히트모션에 맞는 애니메이션을 꺼내와서 실행하는 식으로 해야한다
	KRS_PICK_UP_HIT_DESC* pDesc = static_cast<KRS_PICK_UP_HIT_DESC*>(pValue);

	m_pPlayer->Separation_Bone("mune_c_n", CPlayer::DEFAULT_ANIM, false);		// 기존뼈들을 현재 들고있는 모션대로 돌린다.
	//m_pPlayer->Separation_Bone("ude1_r_n", CPlayer::DEFAULT_ANIM);

	m_AnimationIndices[7] = pDesc->iHitAnimIndex;
	m_iCurrentIndex = 7;		// 모델에서 돌리는 애니메이션 
	m_iComboCount = 0;

	m_isHit = true;
}

void CKiryu_KRS_PickUp::Event(void* pValue)
{

}

_bool CKiryu_KRS_PickUp::Changeable_Combo_Animation()
{
	_float fInterval = 0.3f;

	if (m_iComboCount > 1)
		fInterval = 0.8f;

	if(m_iCurrentIndex == 6)
		fInterval = 0.4f;

	if (Checked_Animation_Ratio(fInterval))
	{
		m_isShaked = false;
		return true;
	}

	return false;
}

void CKiryu_KRS_PickUp::Shaking()
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

void CKiryu_KRS_PickUp::Play_Hit()
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

void CKiryu_KRS_PickUp::Move_KeyInput(const _float& fTimeDelta)
{
	/*
	* 	m_AnimationIndices.push_back(743);	// 5 [743]	[p_wpc_shift_b]
		m_AnimationIndices.push_back(744);	// 6 [744]	[p_wpc_shift_bl]
		m_AnimationIndices.push_back(745);	// 7 [745]	[p_wpc_shift_br]
		m_AnimationIndices.push_back(746);	// 8 [746]	[p_wpc_shift_f]
		m_AnimationIndices.push_back(747);	// 9 [747]	[p_wpc_shift_fl]
		m_AnimationIndices.push_back(748);	// 10 [748]	[p_wpc_shift_fr]
		m_AnimationIndices.push_back(749);	// 11 [749]	[p_wpc_shift_l]
		m_AnimationIndices.push_back(750);	// 12 [750]	[p_wpc_shift_r]
	*/

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
		m_iCurrentIndex = 5;
	else if(!m_isHit)
		m_iCurrentIndex = 1;
}

void CKiryu_KRS_PickUp::Attack_KeyInput(const _float& fTimeDelta)
{
	if (m_pGameInstance->GetMouseState(DIM_LB) == TAP)
	{
		Combo_Count();
		m_eAnimState = ANIM_ONCE;

		m_pPlayer->Get_CurrentItem()->Decrease_Life();
	}

	if (m_pGameInstance->GetKeyState(DIK_Q) == TAP)
	{
		if (m_iComboCount <= 0 && Changeable_Combo_Animation())
		{
			m_iComboCount = 0;
			m_iCurrentIndex = 6;
			m_eAnimState = ANIM_ONCE;
			m_isStop = true;
		}

		m_pPlayer->Get_CurrentItem()->Set_Dissolve();
	}
}

CBehaviorAnimation* CKiryu_KRS_PickUp::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_PickUp* pInstnace = new CKiryu_KRS_PickUp();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_KickCombo");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_PickUp::Free()
{
	__super::Free();
}