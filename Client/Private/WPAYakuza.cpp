#include "WPAYakuza.h"

#include "GameInstance.h"
#include "Collision_Manager.h"
#include "AI_WPAYakuza.h"
#include "Mesh.h"

#include "SocketCollider.h"

CWPAYakuza::CWPAYakuza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CWPAYakuza::CWPAYakuza(const CWPAYakuza& rhs)
	: CMonster{ rhs }
{
}

HRESULT CWPAYakuza::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWPAYakuza::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_wstrModelName = TEXT("Jimu");

	m_iMonsterType = CMonster::WPA;

	if (FAILED(Add_CharacterData()))
		return E_FAIL;

	return S_OK;
}

void CWPAYakuza::Priority_Tick(const _float& fTimeDelta)
{
}

void CWPAYakuza::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CWPAYakuza::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWPAYakuza::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, m_wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	//행동트리 저장
	CAI_WPAYakuza::AI_MONSTER_DESC AIDesc{};
	AIDesc.pState = &m_iState;
	memcpy(AIDesc.pAnim, m_pAnimCom, sizeof(CAnim*) * ANIM_TYPE_END);
	AIDesc.pCurrentAnimType = &m_iCurrentAnimType;
	AIDesc.pThis = this;

	m_pTree = dynamic_cast<CAI_WPAYakuza*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_WPAYakuza"), &AIDesc));
	if (nullptr == m_pTree)
		return E_FAIL;

	return S_OK;
}

void CWPAYakuza::Change_Animation()
{
	m_isAnimLoop = false;

	__super::Change_Animation();

 	switch (m_iState)
	{
	case MONSTER_IDLE:
	{
		m_strAnimName = "e_wpa_stand_btl";
		m_isAnimLoop = true;
		m_isDown = false;
		break;
	}
	case MONSTER_SHIFT_F:
	{
		//e_wpa_shift_f[e_wpa_shift_f]
		m_strAnimName = "e_wpa_shift_f";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_L:
	{
		//e_wpa_shift_l[e_wpa_shift_l]
		m_strAnimName = "e_wpa_shift_l";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_R:
	{
		//e_wpa_shift_r[e_wpa_shift_r]
		m_strAnimName = "e_wpa_shift_r";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_B:
	{
		//e_wpa_shift_b[e_wpa_shift_b]
		m_strAnimName = "e_wpa_shift_b";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SWAY_B:
	{
		//e_wpa_sway_b[e_wpa_sway_b]
		m_strAnimName = "e_wpa_sway_b";
		break;
	}
	case MONSTER_SWAY_F:
	{
		//e_wpa_sway_f[e_wpa_sway_f]
		m_strAnimName = "e_wpa_sway_f";
		break;
	}
	case MONSTER_SWAY_R:
	{
		//e_wpa_sway_r[e_wpa_sway_r]
		m_strAnimName = "e_wpa_sway_r";
		break;
	}
	case MONSTER_SWAY_L:
	{
		//e_wpa_sway_l[e_wpa_sway_l]
		m_strAnimName = "e_wpa_sway_l";
		break;
	}
	case MONSTER_CMB_1:
	{
		//e_wpa_cmb_01[e_wpa_cmb_01]
		m_strAnimName = "e_wpa_cmb_01";
		Shaking(0.3f, 0.2f, 0.2f);
		break;
	}
	case MONSTER_CMB_2:
	{
		//e_wpa_cmb_02[e_wpa_cmb_02]
		m_strAnimName = "e_wpa_cmb_02";
		Shaking(0.3f, 0.2f, 0.2f);
		break;
	}
	case MONSTER_HEAVY_ATTACK:
	{
		m_strAnimName = "e_wpa_atk_heavy";
		Shaking(0.3f, 0.3f, 0.3f);
		break;
	}
	default:
		break;
	}

	if (FAILED(Setup_Animation()))
		return;
}

CWPAYakuza* CWPAYakuza::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWPAYakuza* pInstance = new CWPAYakuza(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CWPAYakuza::Clone(void* pArg)
{
	CWPAYakuza* pInstance = new CWPAYakuza(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CWPAYakuza::Free()
{
	__super::Free();
}
