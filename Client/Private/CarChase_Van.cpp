#include "CarChase_Van.h"

#include "GameInstance.h"

#include "AI_Van.h"
#include "Weapon_ShotGun.h"

CCarChase_Van::CCarChase_Van(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCarChase_Monster{ pDevice, pContext }
{
}

CCarChase_Van::CCarChase_Van(const CCarChase_Van& rhs)
	: CCarChase_Monster{ rhs }
{
}

HRESULT CCarChase_Van::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCarChase_Van::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (m_iWeaponType == DRV)
		m_iState = CCarChase_Monster::CARCHASE_SIT_LP;
	else 
		m_iState = CCarChase_Monster::CARCHASE_AIM_EN;

	if (FAILED(Add_Objects()))
		return E_FAIL;

	return S_OK;
}

void CCarChase_Van::Priority_Tick(const _float& fTimeDelta)
{
	if (nullptr != m_pShotGun)
		m_pShotGun->Priority_Tick(fTimeDelta);
}

void CCarChase_Van::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (nullptr != m_pShotGun)
		m_pShotGun->Tick(fTimeDelta);
}

void CCarChase_Van::Late_Tick(const _float& fTimeDelta)
{
	Set_ParentMatrix(fTimeDelta);

	//m_pModelCom->Play_Animation_Monster(fTimeDelta, m_pAnimCom[m_iCurrentAnimType], m_isAnimLoop, false);
	//XMStoreFloat4x4(&m_ModelWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentBoneMatrix) * XMLoadFloat4x4(m_pParentMatrix));

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pShotGun)
		m_pShotGun->Late_Tick(fTimeDelta);
}

void CCarChase_Van::Change_Animation()
{
	m_isAnimLoop = false;

	switch (m_iState)
	{
	case CARCHASE_CURVA_L:
	{
		if (m_iWeaponType == DRV)
			m_strAnimName = "mngcar_e_van_drv_sit_curvel";
		break;
	}
	case CARCHASE_CURVA_R:
	{
		if (m_iWeaponType == DRV)
			m_strAnimName = "mngcar_e_van_drv_sit_curver";
		break;
	}
	case CARCHASE_SIT_DWN:
	{
		if (m_iWeaponType == DRV)
			m_strAnimName = "mngcar_e_van_drv_sit_dwn";

		break;
	}
	case CARCHASE_SIT_LP:
	{
		if (m_iWeaponType == DRV)
			m_strAnimName = "mngcar_e_van_drv_sit_lp";

		m_isAnimLoop = { true };
		break;
	}
	case CARCHASE_AIM_EN:
	{
		if (m_iWeaponType == WPJ)
			m_strAnimName = "mngcar_e_van_wpj_aim_en";
		break;
	}
	case CARCHASE_AIM_L_LP:
	{
		if (m_iWeaponType == WPR)
			m_strAnimName = "mngcar_e_van_wpr_aim_l_lp";

		m_isAnimLoop = { true };
		break;
	}
	case CARCHASE_AIM_LP:
	{
		if (m_iWeaponType == WPJ)
			m_strAnimName = "mngcar_e_van_wpj_aim_lp";
		else if (m_iWeaponType == WPR)
			m_strAnimName = "mngcar_e_van_wpr_aim_lp";

		m_isAnimLoop = { true };
		break;
	}
	case CARCHASE_AIM_R_LP:
	{
		if (m_iWeaponType == WPR)
			m_strAnimName = "mngcar_e_van_wpr_aim_r_lp";

		m_isAnimLoop = { true };

		break;
	}
	case CARCHASE_AIM_SHOT:
	{
		if (m_iWeaponType == WPJ)
			m_strAnimName = "mngcar_e_van_wpj_aim_shot";
		break;
	}
	case CARCHASE_AIM_ST:
	{
		if (m_iWeaponType == WPJ)
			m_strAnimName = "mngcar_e_van_wpj_aim_st";
		else if (m_iWeaponType == WPR)
			m_strAnimName = "mngcar_e_van_wpr_aim_st";

		break;
	}
	case CARCHASE_DAM_LV_2:
	{
		if (m_iWeaponType == WPJ)
			m_strAnimName = "mngcar_e_van_wpj_dam_lv02";
		else if (m_iWeaponType == WPR)
			m_strAnimName = "mngcar_e_van_wpr_dam_lv02";

		break;
	}
	case CARCHASE_DED:
	{
		if (m_iWeaponType == WPJ)
			m_strAnimName = "mngcar_e_van_wpj_ded";
		else if (m_iWeaponType == WPR)
			m_strAnimName = "mngcar_e_van_wpr_ded";

		break;
	}
	case CARCHASE_RELOAD:
	{
		if (m_iWeaponType == WPR)
			m_strAnimName = "mngcar_e_van_wpr_reload";

		break;
	}
	case CARCHASE_SHOT_L_LP:
	{
		if (m_iWeaponType == WPR)
			m_strAnimName = "mngcar_e_van_wpr_shot_l_lp";

		m_isAnimLoop = { true };
		break;
	}
	case CARCHASE_SHOT_LP:
	{
		if (m_iWeaponType == WPR)
			m_strAnimName = "mngcar_e_van_wpr_shot_lp";
		break;
	}
	case CARCHASE_SHOT_R_LP:
	{
		if (m_iWeaponType == WPR)
			m_strAnimName = "mngcar_e_van_wpr_shot_r_lp";

		m_isAnimLoop = { true };
		break;
	}
	case CARCHASE_SHOT_ST:
	{
		if (m_iWeaponType == WPR)
			m_strAnimName = "mngcar_e_van_wpr_shot_st";

		break;
	}
	default:
		break;
	}

	Setup_Animation();
}

HRESULT CCarChase_Van::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Shakedown"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CAI_Van::AI_CARCHASE_DESC Desc{};
	memcpy(Desc.pAnim, m_pAnimCom, sizeof(CAnim*) * ANIM_TYPE_END);
	Desc.pCurrentAnimType = &m_iCurrentAnimType;
	Desc.pDir = &m_iDir;
	Desc.pState = &m_iState;
	Desc.pThis = this;
	Desc.pWeaponType = &m_iWeaponType;

	m_pTree = dynamic_cast<CAI_Van*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_Van"), &Desc));
	if (nullptr == m_pTree)
		return E_FAIL;

	return S_OK;
}

HRESULT CCarChase_Van::Add_Objects()
{
	if (m_iWeaponType != DRV)
	{
		CSocketObject::SOCKETOBJECT_DESC Desc{};
		Desc.pParentMatrix = m_pParentMatrix;
		Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("buki_r_n");
		Desc.fRotatePecSec = XMConvertToRadians(90.f);
		Desc.fSpeedPecSec = 1.f;
		m_pShotGun = dynamic_cast<CShotGun*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ShotGun"), &Desc));
	}

	return S_OK;
}

CCarChase_Van* CCarChase_Van::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCarChase_Van* pInstance = new CCarChase_Van(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CCarChase_Van::Clone(void* pArg)
{
	CCarChase_Van* pInstance = new CCarChase_Van(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCarChase_Van::Free()
{
	__super::Free();

	Safe_Release(m_pShotGun);
}
