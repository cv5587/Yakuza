#include "GameInstance.h"
#include "Kiryu_KRC_BattleStart.h"
#include "Player.h"
#include "Camera.h"

CKiryu_KRC_BattleStart::CKiryu_KRC_BattleStart()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(320);	// [320]	p_krc_style_st[p_krc_style_st]
	m_AnimationIndices.push_back(318);	// [318]	p_krc_stand_btl_lp[p_krc_stand_btl_lp]
	//m_AnimationIndices.push_back(319);	//[319]	p_krc_stand_btl_st[p_krc_stand_btl_st]
	m_AnimationIndices.push_back(317);	// [317]	p_krc_stand_btl_en[p_krc_stand_btl_en]
}

void CKiryu_KRC_BattleStart::Tick(const _float& fTimeDelta)
{
	Off_Battle(fTimeDelta);

	Shaking();
}

void CKiryu_KRC_BattleStart::Change_Animation()
{
	if (ANIM_START == m_eAnimState)
	{
		m_pPlayer->Off_Separation_Hand();
		m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 0.3f);
	}
	m_pPlayer->Change_Animation(m_AnimationIndices[m_eAnimState]);
}

_bool CKiryu_KRC_BattleStart::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		if (ANIM_START == m_eAnimState)
		{
			m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
			m_eAnimState = ANIM_LOOP;
		}
		else if (ANIM_END == m_eAnimState)
			return true;
	}

	return false;
}

void CKiryu_KRC_BattleStart::Reset()
{
	m_eAnimState = ANIM_START;
	m_fOffBattleTimer = 0.f;
	m_isShaked = false;
}

void CKiryu_KRC_BattleStart::Off_Battle(const _float& fTimeDelta)
{
	if (ANIM_LOOP != m_eAnimState) return;

	m_fOffBattleTimer += fTimeDelta;

	if (ANIM_CHANGE_TIME <= m_fOffBattleTimer)
	{
		m_fOffBattleTimer = { 0.f };

		m_eAnimState = ANIM_END;
	}
}

void CKiryu_KRC_BattleStart::Shaking()
{
	if (m_isShaked) return;

	_float fInterval = 0.6f;

	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (fInterval < *pModelCom->Get_AnimationCurrentPosition() / *pModelCom->Get_AnimationDuration())
	{
		m_isShaked = true;

		_float3 vLook;
		XMStoreFloat3(&vLook, m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK));
		//카메라 쉐이킹
		CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_PLAYER));
		pCamera->Set_Shaking(true, { 1.f, 1.f, 1.f }, 0.1, 0.1);
	}
}

CBehaviorAnimation* CKiryu_KRC_BattleStart::Create(CPlayer* pPlayer)
{
	CKiryu_KRC_BattleStart* pInstnace = new CKiryu_KRC_BattleStart();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_BattleStart");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRC_BattleStart::Free()
{
	__super::Free();
}