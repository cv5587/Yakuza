#include "Yoneda.h"

#include "GameInstance.h"
#include "Mesh.h"

#include "AI_Yoneda.h"
#include "CharacterData.h"


CYoneda::CYoneda(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext}
{
}

CYoneda::CYoneda(const CYoneda& rhs)
	: CMonster { rhs }
{
}

void CYoneda::Set_TriggerQte(_uint iWeaponChange, _uint iTriggerID)
{
	m_iWeaponType = iWeaponChange;

	if (iTriggerID == 1000)
	{
		//액션 변경 가능
		m_iState = CMonster::MONSTER_A60300_000_2;
	}
	else if (iTriggerID == 1001)
	{
		//액션 변경 가능
		m_iState = CMonster::MONSTER_A60330_000_2;
	}
	else if (iTriggerID == 1002)
	{
		//액션 변경 가능
		m_iState = CMonster::MONSTER_A60350_000_2;
	}              

	m_isSynchronizing = true;
	m_pTree->Set_Sync(true);
	m_iCurrentAnimType = CUTSCENE;

	Change_Animation();			// 애니메이션 이름 변경을 위해 한번 실행시킴
	m_pData->Set_CurrentCutSceneAnimation(m_strAnimName);
}

HRESULT CYoneda::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CYoneda::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_wstrModelName = TEXT("Jimu");
	m_wstrScndModelName = TEXT("Yoneda");

	if (FAILED(Add_CharacterData()))
		return E_FAIL;

	m_iMonsterType = CMonster::YONEDA;

	m_iWeaponType = KNIFE; // 무기 체인지 값

	m_iHandAnimIndex = HAND_MIN;
	On_Separation_Hand(0);			// 양손 분리 켜둠

	m_iFaceAnimIndex = 0;
	On_Separation_Face();

	return S_OK;
}

void CYoneda::Priority_Tick(const _float& fTimeDelta)
{
}

void CYoneda::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pModelCom->Play_Animation_Separation(fTimeDelta, m_iHandAnimIndex, m_SeparationAnimComs[HAND_ANIM], false, (_int)HAND_ANIM);
	m_pModelCom->Play_Animation_Separation(fTimeDelta, m_iFaceAnimIndex, m_SeparationAnimComs[FACE_ANIM], false, (_int)FACE_ANIM);

}

void CYoneda::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CYoneda::Off_Sync()
{
	__super::Off_Sync();

	//m_pNavigationCom->Set_Index(m_pNavigationCom->Find_PlayerMonster_Index(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Player"), 0)->Get_TransformCom()->Get_State(CTransform::STATE_POSITION)));
	m_pNavigationCom->Set_Index(m_pNavigationCom->Find_PlayerMonster_Index(m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
}

HRESULT CYoneda::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Yoneda"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	//Prototype_Component_Anim_YonedaFace
	CAnim* pAnimCom = { nullptr };
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim_YonedaFace"),
		TEXT("Com_Anim_Face"), reinterpret_cast<CComponent**>(&pAnimCom))))
		return E_FAIL;
	m_SeparationAnimComs.push_back(pAnimCom);

	//Prototype_Component_Anim_Hand
	pAnimCom = { nullptr };
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim_Hand"),
		TEXT("Com_Anim_Hand"), reinterpret_cast<CComponent**>(&pAnimCom))))
		return E_FAIL;
	m_SeparationAnimComs.push_back(pAnimCom);

	//행동트리 저장
	CAI_Yoneda::AI_YONEDA_DESC AIDesc{};
	AIDesc.pState = &m_iState;
	memcpy(AIDesc.pAnim, m_pAnimCom, sizeof(CAnim*) * ANIM_TYPE_END);
	AIDesc.pCurrentAnimType = &m_iCurrentAnimType;
	AIDesc.pThis = this;
	AIDesc.pWeaponType = &m_iWeaponType;

	m_pTree = dynamic_cast<CAI_Yoneda*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_Yoneda"), &AIDesc));
	if (nullptr == m_pTree)
		return E_FAIL;

	return S_OK;
}

void CYoneda::Change_Animation()
{
	m_isAnimLoop = false;

	__super::Change_Animation();

	switch (m_iState)
	{
	case MONSTER_IDLE:
	{
		if (m_iWeaponType == BAREHAND)
			m_strAnimName = "e_stand";
		else
			m_strAnimName = "e_wpb_knife_stand_btl";

		m_isAnimLoop = true;
		m_isDown = false;
		break;
	}
	case MONSTER_SHIFT_F:
	{
		if (m_iWeaponType == BAREHAND)
			m_strAnimName = "e_sae_shift_f";
		else
			m_strAnimName = "e_wpb_knife_shift_f";

		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_L:
	{
		if (m_iWeaponType == BAREHAND)
			m_strAnimName = "e_sae_shift_l";
		else
			m_strAnimName = "e_wpb_knife_shift_l";

		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_R:
	{
		if (m_iWeaponType == BAREHAND)
			m_strAnimName = "e_sae_shift_r";
		else
			m_strAnimName = "e_wpb_knife_shift_r";

		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_B:
	{
		if (m_iWeaponType == BAREHAND)
			m_strAnimName = "e_sae_shift_b";
		else
			m_strAnimName = "e_wpb_knife_shift_b";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SWAY_B:
	{
		if (m_iWeaponType == BAREHAND)
			m_strAnimName = "e_sway_b";
		else
			m_strAnimName = "e_wpb_knife_sway_b";

		break;
	}
	case MONSTER_SWAY_F:
	{
		if (m_iWeaponType == BAREHAND)
			m_strAnimName = "e_sway_f";
		else
			m_strAnimName = "e_wpb_knife_sway_f";

		break;
	}
	case MONSTER_SWAY_R:
	{
		if (m_iWeaponType == BAREHAND)
			m_strAnimName = "e_sway_r";
		else
			m_strAnimName = "e_wpb_knife_sway_r";

		break;
	}
	case MONSTER_SWAY_L:
	{
		if (m_iWeaponType == BAREHAND)
			m_strAnimName = "e_sway_l";
		else
			m_strAnimName = "e_wpb_knife_sway_l";

		break;
	}
	case MONSTER_CMB_1:
	{
		if (m_iWeaponType == BAREHAND)
		{
			m_strAnimName = "e_sae_cmb_01";
			Shaking(0.3f, 0.2f, 0.2f);
		}
		else
		{
			m_strAnimName = "e_wpb_knife_cmb_01";
			Shaking(0.3f, 0.2f, 0.2f);
		}

		break;
	}
	case MONSTER_CMB_2:
	{
		if (m_iWeaponType == BAREHAND)
		{
			m_strAnimName = "e_sae_cmb_02";
			Shaking(0.3f, 0.2f, 0.2f);
		}
		else
		{
			m_strAnimName = "e_wpb_knife_cmb_02";
			Shaking(0.3f, 0.2f, 0.2f);
		}

		break;
	}
	case MONSTER_CMB_3:
	{
		m_strAnimName = "e_sae_cmb_03";
		Shaking(0.3f, 0.2f, 0.2f);
		break;
	}
	case MONSTER_HEAVY_ATTACK:
	{
		m_strAnimName = "e_sae_atk_heavy_f";
		Shaking(0.3f, 0.3f, 0.3f);
		break;
	}
	case MONSTER_HEAVY_RENDA:
	{
		m_strAnimName = "e_sae_atk_heavy_renda";
		Shaking(0.3f, 0.3f, 0.3f);
		break;
	}
	case MONSTER_KICK:
	{
		m_strAnimName = "e_sae_atk_kick_f";
		Shaking(0.3f, 0.3f, 0.3f);
		break;
	}
	case MONSTER_RUN_ATK:
	{
		m_strAnimName = "e_sae_atk_run";
		Shaking(0.3f, 0.3f, 0.3f);
		break;
	}
	case MONSTER_3REN:
	{
		m_strAnimName = "e_wpb_knife_atk_sting_3ren";
		break;
	}
	case MONSTER_ATK_NORAML:
	{
		m_strAnimName = "e_wpb_knife_atk";
		break;
	}
	case MONSTER_ATK_DOWN:
	{
		m_strAnimName = "e_sae_atk_down";
		break;
	}
	case MONSTER_BTLST:
	{
		m_strAnimName = "e_btlst_nml_02";
		break;
	}
	default:
		break;
	}

	if (FAILED(Setup_Animation()))
		return;
}

CYoneda* CYoneda::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CYoneda* pInstance = new CYoneda(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CYoneda::Clone(void* pArg)
{
	CYoneda* pInstance = new CYoneda(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CYoneda::Free()
{
	__super::Free();

	Safe_Release(m_pTree);
}
