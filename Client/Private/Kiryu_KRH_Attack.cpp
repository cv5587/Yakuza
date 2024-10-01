#include "GameInstance.h"
#include "Kiryu_KRH_Attack.h"
#include "Player.h"
#include "Monster.h"
#include "Camera.h"

CKiryu_KRH_Attack::CKiryu_KRH_Attack()
	:CBehaviorAnimation{}
{
	// 전투 중 아이들 모션은 어드벤처와 동일하다
	
	/* 0 ~ 7 */
	m_AnimationIndices.push_back(422);	//[422]	p_krh_cmb_01[p_krh_cmb_01]
	m_AnimationIndices.push_back(429);	//[429]	p_krh_cmb_02[p_krh_cmb_02]
	m_AnimationIndices.push_back(432);	//[432]	p_krh_cmb_03[p_krh_cmb_03]
	m_AnimationIndices.push_back(435);	//[435]	p_krh_cmb_04[p_krh_cmb_04]
	m_AnimationIndices.push_back(438);	//[438]	p_krh_cmb_05[p_krh_cmb_05]
	m_AnimationIndices.push_back(441);	//[441]	p_krh_cmb_06[p_krh_cmb_06]
	m_AnimationIndices.push_back(444);	//[444]	p_krh_cmb_07[p_krh_cmb_07]
	m_AnimationIndices.push_back(447);	//[447]	p_krh_cmb_08[p_krh_cmb_08]

	/* 8 ~ 15 */
	m_AnimationIndices.push_back(423);	//[423]	p_krh_cmb_01_fin[p_krh_cmb_01_fin]
	m_AnimationIndices.push_back(430);	//[430]	p_krh_cmb_02_fin[p_krh_cmb_02_fin]
	m_AnimationIndices.push_back(433);	//[433]	p_krh_cmb_03_fin[p_krh_cmb_03_fin]
	m_AnimationIndices.push_back(436);	//[436]	p_krh_cmb_04_fin[p_krh_cmb_04_fin]
	m_AnimationIndices.push_back(439);	//[439]	p_krh_cmb_05_fin[p_krh_cmb_05_fin]
	m_AnimationIndices.push_back(442);	//[442]	p_krh_cmb_06_fin[p_krh_cmb_06_fin]
	m_AnimationIndices.push_back(445);	//[445]	p_krh_cmb_07_fin[p_krh_cmb_07_fin]
	m_AnimationIndices.push_back(448);	//[448]	p_krh_cmb_08_fin[p_krh_cmb_08_fin]

	/* 16 ~ 23 */
	m_AnimationIndices.push_back(428);	//[428]	p_krh_cmb_01_tame[p_krh_cmb_01_tame]
	m_AnimationIndices.push_back(431);	//[431]	p_krh_cmb_02_tame[p_krh_cmb_02_tame]
	m_AnimationIndices.push_back(434);	//[434]	p_krh_cmb_03_tame[p_krh_cmb_03_tame]
	m_AnimationIndices.push_back(437);	//[437]	p_krh_cmb_04_tame[p_krh_cmb_04_tame]
	m_AnimationIndices.push_back(440);	//[440]	p_krh_cmb_05_tame[p_krh_cmb_05_tame]
	m_AnimationIndices.push_back(443);	//[443]	p_krh_cmb_06_tame[p_krh_cmb_06_tame]
	m_AnimationIndices.push_back(446);	//[446]	p_krh_cmb_07_tame[p_krh_cmb_07_tame]
	m_AnimationIndices.push_back(449);	//[449]	p_krh_cmb_08_tame[p_krh_cmb_08_tame]

	/* 24 */
	m_AnimationIndices.push_back(411);	//[411]	p_krh_atk_down[p_krh_atk_down]

	/* 25 */
	m_AnimationIndices.push_back(412);	//[412]	p_krh_atk_heavy_f[p_krh_atk_heavy_f]

	/*
		[265]	p_krh_cmb_01_sway_b[p_krh_cmb_01_sway_b]
		[266]	p_krh_cmb_01_sway_f[p_krh_cmb_01_sway_f]
		[267]	p_krh_cmb_01_sway_l[p_krh_cmb_01_sway_l]
		[268]	p_krh_cmb_01_sway_r[p_krh_cmb_01_sway_r]
	*/
}

void CKiryu_KRH_Attack::Tick(const _float& fTimeDelta)
{
	CLandObject* pTargetObject = m_pPlayer->Get_TargetObject();

	if (nullptr != pTargetObject)
	{
		_vector vLookPos = pTargetObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		m_pPlayer->Get_TransformCom()->LookAt_For_LandObject(vLookPos);
	}

	if (m_isPunch)
		m_iComboCount = 25;

	Shaking();
}

void CKiryu_KRH_Attack::Change_Animation()
{
	if (0 > m_iComboCount) return;

	m_pPlayer->Change_Animation(m_AnimationIndices[m_iComboCount]);
}

_bool CKiryu_KRH_Attack::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		Reset();
		return true;
	}

	return false;
}

void CKiryu_KRH_Attack::Reset()
{
	m_iComboCount = 0;
	m_isPunch = false;
	m_isShaked = false;
}

void CKiryu_KRH_Attack::Combo_Count(_bool isFinAction)
{
	// 단독 우클 펀치면 카운트하지않는다
	if (25 == m_iComboCount)
	{
		m_pPlayer->Set_DamageAmplify(1.5f);
		return;
	}

	// 러쉬기본공격은 기본적으로 0.7배함
	m_pPlayer->Set_DamageAmplify(0.7f);

	if (isFinAction)
	{
		m_pPlayer->Set_DamageAmplify(1.2f);
		//if (m_iComboCount == 7)
		//{
		//	if (Changeable_Combo_Animation())
		//		m_iComboCount = 9;
		//}

		if (Changeable_Combo_Animation())
		{
			if (m_iComboCount < 8)
				m_iComboCount += 8;
		}
	}
	else
	{
		if (Changeable_Combo_Animation())
		{
			m_iComboCount++;

			if (m_iComboCount > 7)
				m_iComboCount = 0;
		}
	}
	

	CLandObject* pTargetObject = m_pPlayer->Get_TargetObject();
	if (nullptr != pTargetObject)
	{
		if (static_cast<CMonster*>(pTargetObject)->isDown())
			m_iComboCount = 24;
	}
}

void CKiryu_KRH_Attack::Setting_Value(void* pValue)
{
	_bool* pIsPunch = static_cast<_bool*>(pValue);
	m_isPunch = *pIsPunch;
}

_bool CKiryu_KRH_Attack::IsFinishBlow()
{
	return (7 < m_iComboCount) && (16 > m_iComboCount);
}

_bool CKiryu_KRH_Attack::Changeable_Combo_Animation()
{
	_float fInterval = 0.18f;

	//if (m_iComboCount > 0)			// 3타째 콤보
	//	fInterval = 0.3f;

	//if (m_iComboCount > 2)			// 마지막 발차기 콤보
	//	fInterval = 0.4f;

	//if (m_iComboCount > 3)			// 그 이상은 피니시 블로우기때문에 중간이 끊지않는다
	//	fInterval = 1.f;			// 1로 해두면 비율 연산 결과가 1이 넘을일이 없기때문에 아예 안끊김

	if (m_iComboCount == 5)			// 6타 발차기
		fInterval = 0.27f;

	if (m_iComboCount == 6)			// 7타 발차기 (스킬 찍어야 쓸 수 있음)
		fInterval = 0.34f;

	if (m_iComboCount == 7)			// 8타 발차기 (스킬 찍어야 쓸 수 있음)
		fInterval = 0.5f;			

	if (Checked_Animation_Ratio(fInterval))
	{
		m_isShaked = false;
		return true;
	}

	return false;
}

void CKiryu_KRH_Attack::Shaking()
{
	if (m_iComboCount > 6)
	{
		if (!m_isShaked && Checked_Animation_Ratio(0.3))
		{
			m_isShaked = true;
			//카메라 쉐이킹
			CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
			pCamera->Set_Shaking(true, { 1.f, 1.f, 0.f }, 0.2, 0.4);
		}
	}
}

CBehaviorAnimation* CKiryu_KRH_Attack::Create(CPlayer* pPlayer)
{
	CKiryu_KRH_Attack* pInstnace = new CKiryu_KRH_Attack();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRH_Attack");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRH_Attack::Free()
{
	__super::Free();
}