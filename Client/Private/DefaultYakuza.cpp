#include "DefaultYakuza.h"

#include "GameInstance.h"

#include "AI_DefaultYakuza.h"

CDefaultYakuza::CDefaultYakuza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster {pDevice, pContext}
{
}

CDefaultYakuza::CDefaultYakuza(const CDefaultYakuza& rhs)
	: CMonster { rhs }
{
}

HRESULT CDefaultYakuza::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDefaultYakuza::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_wstrModelName = TEXT("Jimu");

	m_iMonsterType = CMonster::MONSTER_DEFAULT;

	if (FAILED(Add_CharacterData()))
		return E_FAIL;

	return S_OK;
}

void CDefaultYakuza::Priority_Tick(const _float& fTimeDelta)
{
}

void CDefaultYakuza::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CDefaultYakuza::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CDefaultYakuza::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, m_wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	//행동트리 저장
	CAI_DefaultYakuza::AI_MONSTER_DESC AIDesc{};
	AIDesc.pState = &m_iState;
	memcpy(AIDesc.pAnim, m_pAnimCom, sizeof(CAnim*) * ANIM_TYPE_END);
	AIDesc.pCurrentAnimType = &m_iCurrentAnimType;
	AIDesc.pThis = this;

	m_pTree = dynamic_cast<CAI_DefaultYakuza*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_DefaultYakuza"), &AIDesc));
	if (nullptr == m_pTree)
		return E_FAIL;

	return S_OK;
}

void CDefaultYakuza::Change_Animation()
{
	m_isAnimLoop = false;

	__super::Change_Animation();

	switch (m_iState)
	{
	case MONSTER_IDLE:
	{
		m_strAnimName = "e_stand";
		m_isAnimLoop = true;
		m_isDown = false;
		break;
	}
	case MONSTER_SHIFT_F:
	{
		m_strAnimName = "e_sae_shift_f";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_L:
	{
		m_strAnimName = "e_sae_shift_l";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_R:
	{
		m_strAnimName = "e_sae_shift_r";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_B:
	{
		m_strAnimName = "e_sae_shift_b";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SWAY_B:
	{
		m_strAnimName = "e_sway_b";
		break;
	}
	case MONSTER_SWAY_F:
	{
		m_strAnimName = "e_sway_f";
		break;
	}
	case MONSTER_SWAY_R:
	{
		m_strAnimName = "e_sway_r";
		break;
	}
	case MONSTER_SWAY_L:
	{
		m_strAnimName = "e_sway_l";
		break;
	}
	case MONSTER_CMB_1:
	{
		m_strAnimName = "e_sae_cmb_01";
		Shaking(0.3f, 0.2f, 0.2f);
		break;
	}
	case MONSTER_CMB_2:
	{
		m_strAnimName = "e_sae_cmb_02";
		Shaking(0.3f, 0.2f, 0.2f);
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
	case MONSTER_ATK_DOWN:
	{
		m_strAnimName = "e_sae_atk_down";
		break;
	}
	default:
		break;
	}

	if (FAILED(Setup_Animation()))
		return;
}

CDefaultYakuza* CDefaultYakuza::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDefaultYakuza* pInstance = new CDefaultYakuza(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CDefaultYakuza::Clone(void* pArg)
{
	CDefaultYakuza* pInstance = new CDefaultYakuza(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CDefaultYakuza::Free()
{
	__super::Free();
}
