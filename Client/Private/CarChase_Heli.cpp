#include "CarChase_Heli.h"

#include "GameInstance.h"
#include "AI_Heli.h"

#include "SocketModel.h"

CCarChase_Heli::CCarChase_Heli(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCarChase_Monster{ pDevice, pContext }
{
}

CCarChase_Heli::CCarChase_Heli(const CCarChase_Heli& rhs)
	: CCarChase_Monster{ rhs }
{
}

HRESULT CCarChase_Heli::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCarChase_Heli::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if(FAILED(Add_Objects()))
		return E_FAIL;

	return S_OK;
}

void CCarChase_Heli::Priority_Tick(const _float& fTimeDelta)
{
}

void CCarChase_Heli::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (nullptr != m_pWeapon)
		m_pWeapon->Tick(fTimeDelta);
}

void CCarChase_Heli::Late_Tick(const _float& fTimeDelta)
{
	Set_ParentMatrix(fTimeDelta);
	
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pWeapon)
		m_pWeapon->Late_Tick(fTimeDelta);
}

void CCarChase_Heli::Change_Animation()
{
	m_isAnimLoop = false;

	switch (m_iState)
	{
	case CARCHASE_AIM_LP:
	{
		if (m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_hel_gun_aim_lp";
		else if (m_iWeaponType == RKT)
			m_strAnimName = "mngcar_e_hel_rkt_aim_lp";

		break;
	}
	case CARCHASE_SHOT_F_EN:
	{
		if (m_iWeaponType == GAT)
			m_strAnimName = "mngcar_e_hel_gat_shot_f_en";
		break;
	}
	case CARCHASE_SHOT_F_LP:
	{
		if (m_iWeaponType == GAT)
			m_strAnimName = "mngcar_e_hel_gat_shot_f_lp";
		break;
	}
	case CARCHASE_SHOT_F_ST:
	{
		if (m_iWeaponType == GAT)
			m_strAnimName = "mngcar_e_hel_gat_shot_f_st";
		break;
	}
	case CARCHASE_AIM_SHOT:
	{
		if (m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_hel_gun_aim_shot";
		else if (m_iWeaponType == RKT)
			m_strAnimName = "mngcar_e_hel_rkt_aim_shot";
		break;
	}
	case CARCHASE_DAM_LV_2:
	{
		if (m_iWeaponType == GAT)
			m_strAnimName = "mngcar_e_hel_gat_dam_lv02";
		else if (m_iWeaponType == RKT)
			m_strAnimName = "mngcar_e_hel_rkt_dam_lv02";

		break;
	}
	case CARCHASE_DED:
	{
		if (m_iWeaponType == GAT)
			m_strAnimName = "mngcar_e_hel_gat_ded";
		else if (m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_hel_gun_ded";
		else if (m_iWeaponType == RKT)
			m_strAnimName = "mngcar_e_hel_rkt_ded";

		break;
	}
	case CARCHASE_BATTOU:
	{
		if (m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_hel_gun_battou";
		break;
	}
	case CARCHASE_NOUTOU:
	{
		if (m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_hel_gun_noutou";
		break;
	}
	case CARCHASE_STAND_UP:
	{
		if (m_iWeaponType == GAT)
			m_strAnimName = "mngcar_e_hel_gat_stand_lp";
		else if (m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_hel_gun_stand_lp";

		break;
	}
	default:
		break;
	}

	Setup_Animation();
}

HRESULT CCarChase_Heli::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Shakedown"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CAI_Heli::AI_CARCHASE_DESC Desc{};
	memcpy(Desc.pAnim, m_pAnimCom, sizeof(CAnim*) * ANIM_TYPE_END);
	Desc.pCurrentAnimType = &m_iCurrentAnimType;
	Desc.pDir = &m_iDir;
	Desc.pState = &m_iState;
	Desc.pThis = this;
	Desc.pWeaponType = &m_iWeaponType;

	m_pTree = dynamic_cast<CAI_Heli*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_Heli"), &Desc));
	if (nullptr == m_pTree)
		return E_FAIL;

	return S_OK;
}

HRESULT CCarChase_Heli::Add_Objects()
{
	// 머신건
	if (GAT == m_iWeaponType)
	{
		CSocketObject::SOCKETOBJECT_DESC Desc{};
		Desc.pParentMatrix = m_pParentMatrix;
		Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("buki_r_n");
		Desc.fRotatePecSec = XMConvertToRadians(90.f);
		Desc.fSpeedPecSec = 1.f;
		m_pWeapon = dynamic_cast<CSocketModel*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_MchnGun"), &Desc));
	}
	// 권총
	else if (GUN == m_iWeaponType)
	{
		CSocketObject::SOCKETOBJECT_DESC Desc{};
		Desc.pParentMatrix = m_pParentMatrix;
		Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("buki_r_n");
		Desc.fRotatePecSec = XMConvertToRadians(90.f);
		Desc.fSpeedPecSec = 1.f;
		m_pWeapon = dynamic_cast<CSocketModel*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Gun_Cz75"), &Desc));
	}
	// 로켓런처
	else if (RKT == m_iWeaponType)
	{
		CSocketObject::SOCKETOBJECT_DESC Desc{};
		Desc.pParentMatrix = m_pParentMatrix;
		Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("buki_r_n");
		Desc.fRotatePecSec = XMConvertToRadians(90.f);
		Desc.fSpeedPecSec = 1.f;
		m_pWeapon = dynamic_cast<CSocketModel*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_RcktGun"), &Desc));
	}

	return S_OK;
}

CCarChase_Heli* CCarChase_Heli::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCarChase_Heli* pInstance = new CCarChase_Heli(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CCarChase_Heli::Clone(void* pArg)
{
	CCarChase_Heli* pInstance = new CCarChase_Heli(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCarChase_Heli::Free()
{
	__super::Free();

	Safe_Release(m_pWeapon);
}
