#include "RoadStanding_NML.h"

#include "GameInstance.h"

#include "AI_RoadStanding_NML.h"

CRoadStanding_NML::CRoadStanding_NML(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CRoadStanding{ pDevice, pContext }
{
}

CRoadStanding_NML::CRoadStanding_NML(const CRoadStanding_NML& rhs)
	: CRoadStanding{ rhs }
{
}

HRESULT CRoadStanding_NML::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRoadStanding_NML::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iMotionType = m_pGameInstance->Get_Random(0, _int(MOTION_LISTEN));

	if (FAILED(Add_Components()))
		return E_FAIL;
	
	dynamic_cast<CAI_RoadStanding_NML*>(m_pTree)->Set_MotionPointer(&m_iMotionType);

	m_fChangeInterval = 18.f;

	return S_OK;
}

void CRoadStanding_NML::Priority_Tick(const _float& fTimeDelta)
{
}

void CRoadStanding_NML::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CRoadStanding_NML::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	__super::Late_Tick(fTimeDelta);
}

HRESULT CRoadStanding_NML::Add_Components()
{
	//if (FAILED(__super::Add_Components()))
	//	return E_FAIL;

	//행동트리 저장
	CAI_RoadStanding_NML::AI_ADVENTURE_DESC AIDesc{};
	AIDesc.pAnimCom = m_pAnimCom;
	AIDesc.pState = &m_iState;
	AIDesc.pThis = this;
	AIDesc.pAStartCom = m_pAStartCom;

	m_pTree = dynamic_cast<CAI_RoadStanding_NML*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_RoadStanding_NML"), &AIDesc));
	if (nullptr == m_pTree)
		return E_FAIL;

	return S_OK;
}

void CRoadStanding_NML::Change_Animation()
{
	__super::Change_Animation();

	switch (m_iState)
	{
	case ADVENTURE_STAND:
	{
		if (m_iGender == GENDER_F)
		{
			if (m_iMotionType == MOTION_TALK_A)
				m_strAnimName = "f_nml_act_talk_a_st";
			else if (m_iMotionType == MOTION_TALK_B)
				m_strAnimName = "f_nml_act_talk_b_st";
			//else if (m_iMotionType == MOTION_THINK)
			//	m_strAnimName = "f_nml_act_stand_atc_think_st";
			else if (m_iMotionType == MOTION_LISTEN)
				m_strAnimName = "f_nml_act_stand_atc_listen_st";


			if (m_iMotionType == MOTION_TALK_A)
				m_iAnim = 0;
			else if (m_iMotionType == MOTION_TALK_B)
				m_iAnim = 1;
			//else if (m_iMotionType == MOTION_THINK)
			//	m_strAnimName = "f_nml_act_stand_atc_think_st";
			else if (m_iMotionType == MOTION_LISTEN)
				m_iAnim = 2;
		}
		else if (m_iGender == GENDER_M)
		{
			if (m_iMotionType == MOTION_TALK_A)
				m_strAnimName = "m_nml_act_talk_a_st";
			else if (m_iMotionType == MOTION_TALK_B)
				m_strAnimName = "m_nml_act_talk_b_st";
			//else if (m_iMotionType == MOTION_THINK)
			//	m_strAnimName = "m_nml_act_stand_atc_think_st";
			else if (m_iMotionType == MOTION_LISTEN)
				m_strAnimName = "m_nml_act_stand_atc_listen_st";
		}
		break;
	}
	case ADVENTURE_STAND_ST:
	{
		if (m_iGender == GENDER_F)
		{
			if (m_iMotionType == MOTION_TALK_A)
				m_strAnimName = "f_nml_act_talk_a_lp";
			else if (m_iMotionType == MOTION_TALK_B)
				m_strAnimName = "f_nml_act_talk_b_lp";
			//else if (m_iMotionType == MOTION_THINK)
			//	m_strAnimName = "f_nml_act_stand_atc_think_lp";
			else if (m_iMotionType == MOTION_LISTEN)
				m_strAnimName = "f_nml_act_stand_atc_listen_lp";

			if (m_iMotionType == MOTION_TALK_A)
				m_iAnim = 3;
			else if (m_iMotionType == MOTION_TALK_B)
				m_iAnim = 4;
			//else if (m_iMotionType == MOTION_THINK)
			//	m_strAnimName = "f_nml_act_stand_atc_think_lp";
			else if (m_iMotionType == MOTION_LISTEN)
				m_iAnim = 5;
		}
		else if (m_iGender == GENDER_M)
		{
			if (m_iMotionType == MOTION_TALK_A)
				m_strAnimName = "m_nml_act_talk_a_lp";
			else if (m_iMotionType == MOTION_TALK_B)
				m_strAnimName = "m_nml_act_talk_b_lp";
			//else if (m_iMotionType == MOTION_THINK)
			//	m_strAnimName = "m_nml_act_stand_atc_think_lp";
			else if (m_iMotionType == MOTION_LISTEN)
				m_strAnimName = "m_nml_act_stand_atc_listen_lp";
		}

		break;
	}
	default:
		break;
	}

	if(m_iGender == GENDER_M)
		m_iAnim = m_pAnimCom->Get_AnimationIndex(m_strAnimName.c_str());

	if (m_iAnim == -1)
		return;

	if (FAILED(Setup_Animation()))
		return;
}

CRoadStanding_NML* CRoadStanding_NML::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRoadStanding_NML* pInstance = new CRoadStanding_NML(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CRoadStanding_NML::Clone(void* pArg)
{
	CRoadStanding_NML* pInstance = new CRoadStanding_NML(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CRoadStanding_NML::Free()
{
	__super::Free();
}
