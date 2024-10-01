#include "GameInstance.h"
#include "Kiryu_KRC_Attack.h"
#include "Player.h"
#include "Monster.h"
#include "Camera.h"

CKiryu_KRC_Attack::CKiryu_KRC_Attack()
	:CBehaviorAnimation{}
{
	// 전투 중 아이들 모션은 어드벤처와 동일하다
	
	/* 0 ~ 2 */
	m_AnimationIndices.push_back(275);	//[275]	p_krc_cmb_01[p_krc_cmb_01]
	m_AnimationIndices.push_back(277);	//[277]	p_krc_cmb_02[p_krc_cmb_02]
	m_AnimationIndices.push_back(280);	//[280]	p_krc_cmb_03[p_krc_cmb_03]

	/* 3 ~ 5 */
	m_AnimationIndices.push_back(276);	//[276]	p_krc_cmb_01_fin[p_krc_cmb_01_fin]
	m_AnimationIndices.push_back(278);	//[278]	p_krc_cmb_02_fin[p_krc_cmb_02_fin]
	m_AnimationIndices.push_back(281);	//[281]	p_krc_cmb_03_fin[p_krc_cmb_03_fin]

	/* 6 */
	m_AnimationIndices.push_back(264);	//[264]	p_krc_atk_down_trample[p_krc_atk_down_trample]

	/* 7 */
	m_AnimationIndices.push_back(271);	//[271]	p_krc_atk_tackle_f[p_krc_atk_tackle_f]

	/*
		[279]	p_krc_cmb_02_fin_hit[p_krc_cmb_02_fin_hit]
		[282]	p_krc_cmb_03_heavy[p_krc_cmb_03_heavy]
	*/
}

void CKiryu_KRC_Attack::Tick(const _float& fTimeDelta)
{
	CLandObject* pTargetObject = m_pPlayer->Get_TargetObject();

	if (nullptr != pTargetObject)
	{
		_vector vLookPos = pTargetObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		m_pPlayer->Get_TransformCom()->LookAt_For_LandObject(vLookPos);
	}

	if (m_isBut)
		m_iComboCount = 7;

	Shaking();
}

void CKiryu_KRC_Attack::Change_Animation()
{
	if (0 > m_iComboCount) return;

	m_pPlayer->Change_Animation(m_AnimationIndices[m_iComboCount]);
}

_bool CKiryu_KRC_Attack::Get_AnimationEnd()
{
	if (m_isFinished) return true;

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		Reset();
		return true;
	}

	return false;
}

void CKiryu_KRC_Attack::Reset()
{
	m_iComboCount = 0;
	m_isBut = false;
	m_isShaked = false;
}

void CKiryu_KRC_Attack::Combo_Count(_bool isFinAction)
{
	// 단독 강공격 (몸통박치기)
	if (7 == m_iComboCount) 
	{
		m_pPlayer->Set_DamageAmplify(3.f);
		return;
	}

	// 파괴자 스타일은 기본적으로 1.5배해둔다
	m_pPlayer->Set_DamageAmplify(1.5f);

	if (isFinAction)
	{
		m_pPlayer->Set_DamageAmplify(3.f);

		if (Changeable_Combo_Animation())
		{
			if (m_iComboCount < 3)
				m_iComboCount += 3;
		}
	}
	else
	{
		if (Changeable_Combo_Animation())
		{
			m_iComboCount++;

			if (m_iComboCount > 5)
				m_iComboCount = 0;
		}
	}
	
	CLandObject* pTargetObject = m_pPlayer->Get_TargetObject();
	if (nullptr != pTargetObject)
	{
		if (static_cast<CMonster*>(pTargetObject)->isDown())
			m_iComboCount = 6;
	}
	
}

void CKiryu_KRC_Attack::Setting_Value(void* pValue)
{
	_bool* pIsBut = static_cast<_bool*>(pValue);
	m_isBut = *pIsBut;
}

_bool CKiryu_KRC_Attack::IsFinishBlow()
{
	return 2 < m_iComboCount;
}

_bool CKiryu_KRC_Attack::Changeable_Combo_Animation()
{
	_float fInterval = 0.4f;

	if (Checked_Animation_Ratio(fInterval))
	{
		m_isShaked = false;
		return true;
	}

	return false;
}

void CKiryu_KRC_Attack::Shaking()
{
	if (!m_isShaked && Checked_Animation_Ratio(0.2))
	{
		m_isShaked = true;
		//카메라 쉐이킹
		CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));

		if(m_iComboCount < 2)
			pCamera->Set_Shaking(true, { 1.f, 1.f, 0.f }, 0.2, 0.5);
		else
			pCamera->Set_Shaking(true, { 1.f, 1.f, 0.f }, 0.3, 0.3);
	}
}

CBehaviorAnimation* CKiryu_KRC_Attack::Create(CPlayer* pPlayer)
{
	CKiryu_KRC_Attack* pInstnace = new CKiryu_KRC_Attack();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRH_Attack");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRC_Attack::Free()
{
	__super::Free();
}