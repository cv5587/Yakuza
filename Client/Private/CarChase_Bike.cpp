#include "CarChase_Bike.h"

#include "GameInstance.h"

#include "AI_Bike.h"

#include "SocketModel.h"

CCarChase_Bike::CCarChase_Bike(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCarChase_Monster{ pDevice, pContext }
{
}

CCarChase_Bike::CCarChase_Bike(const CCarChase_Bike& rhs)
	: CCarChase_Monster{ rhs }
{
}

HRESULT CCarChase_Bike::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCarChase_Bike::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Objects()))
		return E_FAIL;

	return S_OK;
}

void CCarChase_Bike::Priority_Tick(const _float& fTimeDelta)
{
}

void CCarChase_Bike::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (nullptr != m_pWeapon)
		m_pWeapon->Tick(fTimeDelta);
}

void CCarChase_Bike::Late_Tick(const _float& fTimeDelta)
{
	Set_ParentMatrix(fTimeDelta);

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pWeapon)
		m_pWeapon->Late_Tick(fTimeDelta);
}

void CCarChase_Bike::Change_Animation()
{
	m_isAnimLoop = false;

	switch (m_iState)
	{
	case CARCHASE_CURVA_L:
	{
		if (m_iWeaponType == DRV || m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_bik_drv_curve_l_lp";
		else if (m_iWeaponType == RKT)
			m_strAnimName = "mngcar_e_bik_rkt_curve_l_lp";

		m_isAnimLoop = true;
		break;
	}
	case CARCHASE_CURVA_R:
	{
		if (m_iWeaponType == DRV || m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_bik_drv_curve_r_lp";
		else if (m_iWeaponType == RKT)
			m_strAnimName = "mngcar_e_bik_rkt_curve_r_lp";

		m_isAnimLoop = true;
		break;
	}
	case CARCHASE_TENTOU_A:
	{
		if (m_iWeaponType == DRV || m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_bik_drv_tentou_a";
		if (m_iWeaponType == RKT)
			m_strAnimName = "mngcar_e_bik_rkt_tentou_a";
		break;
	}
	case CARCHASE_TENTOU_B:
	{
		if (m_iWeaponType == DRV || m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_bik_drv_tentou_b";
		if (m_iWeaponType == RKT)
			m_strAnimName = "mngcar_e_bik_rkt_tentou_b";
		break;
	}
	case CARCHASE_TENTOU_C:
	{
		if (m_iWeaponType == DRV || m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_bik_drv_tentou_c";
		if (m_iWeaponType == RKT)
			m_strAnimName = "mngcar_e_bik_rkt_tentou_c";
		break;
	}
	case CARCHASE_AIM_F_LP:
	{
		if (m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_bik_gun_aim_f_lp";

		m_isAnimLoop = true;
		break;
	}
	case CARCHASE_AIM_L90_LP:
	{
		if (m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_bik_gun_aim_l90_lp";

		m_isAnimLoop = true;
		break;
	}
	case CARCHASE_AIM_R90_LP:
	{
		if (m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_bik_gun_aim_r90_lp";

		m_isAnimLoop = true;
		break;
	}
	case CARCHASE_AIM_L_LP:
	{
		if (m_iWeaponType == RKT)
			m_strAnimName = "mngcar_e_bik_rkt_aim_l_lp";

		m_isAnimLoop = true;
		break;
	}
	case CARCHASE_AIM_R_LP:
	{
		if (m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_bik_gun_aim_r_lp";
		else if (m_iWeaponType == RKT)
			m_strAnimName = "mngcar_e_bik_rkt_aim_r_lp";

		m_isAnimLoop = true;
		break;
	}
	case CARCHASE_AIM_SHOT_F:
	{
		if (m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_bik_gun_aim_shot_f";

		break;
	}
	case CARCHASE_AIM_SHOT_R:
	{
		if (m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_bik_gun_aim_shot_r";
		else if (m_iWeaponType == RKT)
			m_strAnimName = "mngcar_e_bik_rkt_aim_shot_r";

		break;
	}
	case CARCHASE_AIM_SHOT_L:
	{
		if (m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_bik_gun_aim_shot_l";
		else if (m_iWeaponType == RKT)
			m_strAnimName = "mngcar_e_bik_rkt_aim_shot_l";
		break;
	}
	case CARCHASE_AIM_SHOT_L90:
	{
		if (m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_bik_gun_aim_shot_l90";
		break;
	}
	case CARCHASE_AIM_SHOT_R90:
	{
		if (m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_bik_gun_aim_shot_r90";
		break;
	}
	case CARCHASE_DAM_LV_2:
	{
		if (m_iWeaponType == DRV || m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_bik_drv_dam_lv02";
		else if (m_iWeaponType == RKT)
			m_strAnimName = "mngcar_e_bik_rkt_dam_lv02";
		break;
	}
	case CARCHASE_DED_L:
	{
		if (m_iWeaponType == RKT)
			m_strAnimName = "mngcar_e_bik_rkt_ded_l";

		break;
	}
	case CARCHASE_DED_R:
	{
		if (m_iWeaponType == RKT)
			m_strAnimName = "mngcar_e_bik_rkt_ded_r";
		break;
	}
	case CARCHASE_RELOAD:
	{
		if (m_iWeaponType == RKT)
			m_strAnimName = "mngcar_e_bik_rkt_reload";
		break;
	}
	case CARCHASE_BATTOU:
	{
		if (m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_bik_gun_battou";
		break;
	}
	case CARCHASE_NOUTOU:
	{
		if (m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_bik_gun_noutou";
		break;
	}
	case CARCHASE_STAND_UP:
	{
		if (m_iWeaponType == DRV)
			m_strAnimName = "mngcar_e_bik_drv_stand_lp";
		else if (m_iWeaponType == GUN)
			m_strAnimName = "mngcar_e_bik_gun_stand_lp";
		else if (m_iWeaponType == RKT)
			m_strAnimName = "mngcar_e_bik_rkt_stand_lp";

		m_isAnimLoop = true;
		break;
	}
	default:
		break;
	}

	Setup_Animation();
}

HRESULT CCarChase_Bike::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Shakedown"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CAI_Bike::AI_CARCHASE_DESC Desc{};
	memcpy(Desc.pAnim, m_pAnimCom, sizeof(CAnim*) * ANIM_TYPE_END);
	Desc.pCurrentAnimType = &m_iCurrentAnimType;
	Desc.pDir = &m_iDir;
	Desc.pState = &m_iState;
	Desc.pThis = this;
	Desc.pWeaponType = &m_iWeaponType;

	m_pTree = dynamic_cast<CAI_Bike*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_Bike"), &Desc));
	if (nullptr == m_pTree)
		return E_FAIL;

	return S_OK;
}

void CCarChase_Bike::Set_ParentMatrix(const _float& fTimeDelta)
{
	m_pModelCom->Play_Animation_Monster(fTimeDelta, m_pAnimCom[m_iCurrentAnimType], m_isAnimLoop, false);

	if (m_iWeaponType == RKT && (m_iState == CARCHASE_DED_R || m_iState == CARCHASE_DED_L) && *(m_pAnimCom[m_iCurrentAnimType]->Get_AnimPosition()) >= 33.f)
		return;

	_matrix ParentMatrix = XMLoadFloat4x4(m_pParentBoneMatrix) * XMLoadFloat4x4(m_pParentMatrix);
	//ParentMatrix.r[3] = (XMLoadFloat4x4(m_pParentBoneMatrix) * XMLoadFloat4x4(m_pParentMatrix)).r[3];
	//ParentMatrix.r[2] = XMLoadFloat4x4(m_pParentMatrix).r[2];
	_matrix		RotationMatrix = XMMatrixRotationAxis(ParentMatrix.r[0], XMConvertToRadians(-90.f));

	ParentMatrix.r[0] = XMVector3TransformNormal(ParentMatrix.r[0], RotationMatrix);
	ParentMatrix.r[1] = XMVector3TransformNormal(ParentMatrix.r[1], RotationMatrix);
	ParentMatrix.r[2] = XMVector3TransformNormal(ParentMatrix.r[2], RotationMatrix);

	XMStoreFloat4x4(&m_ModelWorldMatrix, m_pTransformCom->Get_WorldMatrix() * ParentMatrix);
}

HRESULT CCarChase_Bike::Add_Objects()
{
	if (GUN == m_iWeaponType)
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

CCarChase_Bike* CCarChase_Bike::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCarChase_Bike* pInstance = new CCarChase_Bike(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CCarChase_Bike::Clone(void* pArg)
{
	CCarChase_Bike* pInstance = new CCarChase_Bike(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCarChase_Bike::Free()
{
	__super::Free();

	Safe_Release(m_pWeapon);
}
