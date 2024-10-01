#include "CarChase_Sedan.h"

#include "GameInstance.h"
#include "Highway_Taxi.h"

#include "AI_Sedan.h"

#include "SocketModel.h"

CCarChase_Sedan::CCarChase_Sedan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCarChase_Monster{ pDevice, pContext }
{
}

CCarChase_Sedan::CCarChase_Sedan(const CCarChase_Sedan& rhs)
	: CCarChase_Monster{ rhs }
{
}

HRESULT CCarChase_Sedan::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCarChase_Sedan::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iDir = Change_Dir();

	if (FAILED(Add_Objects()))
		return E_FAIL;

	return S_OK;
}

void CCarChase_Sedan::Priority_Tick(const _float& fTimeDelta)
{
}

void CCarChase_Sedan::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CCarChase_Sedan::Late_Tick(const _float& fTimeDelta)
{
	Set_ParentMatrix(fTimeDelta);
	__super::Late_Tick(fTimeDelta);
}

void CCarChase_Sedan::Change_Animation()
{
	switch (m_iState)
	{
	case CARCHASE_CURVA_L:
	{
		if (m_iWeaponType == DRV)
			m_strAnimName = "mngcar_e_car_drv_sit_curvel";

		break;
	}
	case CARCHASE_CURVA_R:
	{
		if (m_iWeaponType == DRV)
			m_strAnimName = "mngcar_e_car_drv_sit_curver";
		break;
	}
	case CARCHASE_SIT_DWN:
	{
		if (m_iWeaponType == DRV)
			m_strAnimName = "mngcar_e_car_drv_sit_dwn";
		break;
	}
	case CARCHASE_SIT_LP:
	{
		if (m_iWeaponType == DRV)
			m_strAnimName = "mngcar_e_car_drv_sit_lp";

		m_isAnimLoop = true;
		break;
	}
	case CARCHASE_AIML_CURVE_L:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_curvel";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_curvel";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_curvel";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_curvel";
		}
			
		break;
	}
	case CARCHASE_AIML_CURVE_R:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_curver";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_curver";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_curver";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_curver";
		}
			
		break;
	}
	case CARCHASE_AIML_DAM:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_dam";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_dam";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_dam";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_dam";
		}
			
		break;
	}
	case CARCHASE_AIML_DED:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_ded";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_ded";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_ded";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_ded";
		}
			
		break;
	}
	case CARCHASE_AIML_DWN:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_dwn";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_dwn";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_dwn";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_dwn";
		}
			
		break;
	}
	case CARCHASE_AIML_HI:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_hi";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_hi";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_hi";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_hi";
		}
			
		break;
	}
	case CARCHASE_AIML_L45:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_l45";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_l45";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_l45";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_l45";
		}
			
		break;
	}
	case CARCHASE_AIML_LP:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_lp";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_lp";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_lp";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_lp";
		}
			
		break;
	}
	case CARCHASE_AIML_SHOT:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_shot";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_shot";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_shot";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_shot";
			//else if (m_iDir == DIR_FL)
			//	m_strAnimName = "mngcar_c_car_gun_aiml_l_r45";
			//else if (m_iDir == DIR_BL)
			//	m_strAnimName = "mngcar_c_car_gun_aiml_l_l45";
			//else if (m_iDir == DIR_FR)
			//	m_strAnimName = "mngcar_c_car_gun_aiml_r_l45";
			//else if (m_iDir == DIR_BR)
			//	m_strAnimName = "mngcar_c_car_gun_aiml_r_r45";
		}
			
		break;
	}
	case CARCHASE_AIML_STD_LP:
	{
		if (m_iWeaponType == GUN_L)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aiml_b_std_lp";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aiml_f_std_lp";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aiml_l_std_lp";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aiml_r_std_lp";
		}
			
		break;
	}
	case CARCHASE_AIML_BTOL:
	{
		if (m_iWeaponType == GUN_L)
			m_strAnimName = "mngcar_c_car_gun_aiml_btol";
			
		break;
	}
	case CARCHASE_AIML_FTOL:
	{
		if (m_iWeaponType == GUN_L)
			m_strAnimName = "mngcar_c_car_gun_aiml_ftol";
		break;
	}
	case CARCHASE_AIML_LTOB:
	{
		if (m_iWeaponType == GUN_L)
			m_strAnimName = "mngcar_c_car_gun_aiml_ltol";
		break;
	}
	case CARCHASE_AIML_LTOF:
	{
		if (m_iWeaponType == GUN_L)
			m_strAnimName = "mngcar_c_car_gun_aiml_ltol";
		break;
	}
	case CARCHASE_AIMR_CURVE_L:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_curvel";
			else if (m_iDir == DIR_F)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_f_curvel";
			else if (m_iDir == DIR_L)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_l_curvel";
			else if (m_iDir == DIR_R)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_r_curvel";
		}

		break;
	}
	case CARCHASE_AIMR_CURVE_R:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_curver";
			else if (m_iDir == DIR_F)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_f_curver";
			else if (m_iDir == DIR_L)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_l_curver";
			else if (m_iDir == DIR_R)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_r_curver";
		}

		break;
	}
	case CARCHASE_AIMR_DAM:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_dam";
			else if (m_iDir == DIR_F)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_b_dam";
			else if (m_iDir == DIR_L)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_l_dam";
			else if (m_iDir == DIR_R)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_r_dam";
		}

		break;
	}
	case CARCHASE_AIMR_DED:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_ded";
			else if (m_iDir == DIR_F)
				m_strAnimName = "mngcar_c_car_gun_aimr_f_ded";
			else if (m_iDir == DIR_L)
				m_strAnimName = "mngcar_c_car_gun_aimr_l_ded";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aimr_r_ded";
		}

		break;
	}
	case CARCHASE_AIMR_DWN:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_dwn";
			else if (m_iDir == DIR_F)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_f_dwn";
			else if (m_iDir == DIR_L)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_l_dwn";
			else if (m_iDir == DIR_R)
				m_strAnimName = "mngcar_c_car_gun_aimr_r_dwn";
		}

		break;
	}
	case CARCHASE_AIMR_HI:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_hi";
			else if (m_iDir == DIR_F)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_f_hi";
			else if (m_iDir == DIR_L)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_l_hi";
			else if (m_iDir == DIR_R)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_r_hi";
		}

		break;
	}
	case CARCHASE_AIMR_L45:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_l45";
			else if (m_iDir == DIR_F)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_f_l45";
			else if (m_iDir == DIR_L)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_l_l45";
			else if (m_iDir == DIR_R)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_r_l45";
		}

		break;
	}
	case CARCHASE_AIMR_LP:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_lp";
			else if (m_iDir == DIR_F)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_f_lp";
			else if (m_iDir == DIR_L)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_l_lp";
			else if (m_iDir == DIR_R)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_r_lp";
		}

		break;
	}
	case CARCHASE_AIMR_SHOT:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_shot";
			else if (m_iDir == DIR_F)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_f_shot";
			else if (m_iDir == DIR_L)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_l_shot";
			else if (m_iDir == DIR_R)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_r_shot";
		}

		break;
	}
	case CARCHASE_AIMR_STD_LP:
	{
		if (m_iWeaponType == GUN_R)
		{
			if (m_iDir == DIR_B)
				m_strAnimName = "mngcar_c_car_gun_aimr_b_std_lp";
			else if (m_iDir == DIR_F)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_f_std_lp";
			else if (m_iDir == DIR_L)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_l_std_lp";
			else if (m_iDir == DIR_R)				 
				m_strAnimName = "mngcar_c_car_gun_aimr_r_std_lp";
		}

		break;
	}
	case CARCHASE_AIMR_BTOL:
	{
		if (m_iWeaponType == GUN_R)
			m_strAnimName = "mngcar_c_car_gun_aimr_btol";

		break;
	}
	case CARCHASE_AIMR_FTOL:
	{
		if (m_iWeaponType == GUN_R)
			m_strAnimName = "mngcar_c_car_gun_aimr_ftol";
		break;
	}
	case CARCHASE_AIMR_LTOB:
	{
		if (m_iWeaponType == GUN_R)
			m_strAnimName = "mngcar_c_car_gun_aimr_ltol";
		break;
	}
	case CARCHASE_AIMR_LTOF:
	{
		if (m_iWeaponType == GUN_R)
			m_strAnimName = "mngcar_c_car_gun_aimr_ltol";
		break;
	}
	case CARCHASE_SITL_EN:
	{
		if (m_iWeaponType == GUN_L)
			m_strAnimName = "mngcar_c_car_gun_sitl_en";
		break;
	}
	case CARCHASE_SITL_LP:
	{
		if (m_iWeaponType == GUN_L)
			m_strAnimName = "mngcar_c_car_gun_sitl_lp";
		break;
	}
	case CARCHASE_SITL_ST:
	{
		if (m_iWeaponType == GUN_L)
			m_strAnimName = "mngcar_c_car_gun_sitl_st";
		break;
	}
	case CARCHASE_SITR_EN:
	{
		if (m_iWeaponType == GUN_R)
			m_strAnimName = "mngcar_c_car_gun_sitr_en";
		break;
	}
	case CARCHASE_SITR_LP:
	{
		if (m_iWeaponType == GUN_R)
			m_strAnimName = "mngcar_c_car_gun_sitr_lp";
		break;
	}
	case CARCHASE_SITR_ST:
	{
		if (m_iWeaponType == GUN_R)
			m_strAnimName = "mngcar_c_car_gun_sitr_st";
		break;
	}
	default:
		break;
	}

	Setup_Animation();
}

HRESULT CCarChase_Sedan::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Shakedown"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CAI_Sedan::AI_CARCHASE_DESC Desc{};
	memcpy(Desc.pAnim, m_pAnimCom, sizeof(CAnim*) * ANIM_TYPE_END);
	Desc.pCurrentAnimType = &m_iCurrentAnimType;
	Desc.pDir = &m_iDir;
	Desc.pState = &m_iState;
	Desc.pThis = this;
	Desc.pWeaponType = &m_iWeaponType;

	m_pTree = dynamic_cast<CAI_Sedan*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_Sedan"), &Desc));
	if (nullptr == m_pTree)
		return E_FAIL;

	return S_OK;
}

void CCarChase_Sedan::Set_ParentMatrix(const _float& fTimeDelta)
{
	m_pModelCom->Play_Animation_Monster(fTimeDelta, m_pAnimCom[m_iCurrentAnimType], m_isAnimLoop, false);

	if ((m_iState == CARCHASE_AIML_DED || m_iState == CARCHASE_AIMR_DED) && *(m_pAnimCom[m_iCurrentAnimType]->Get_AnimPosition()) >= 38.f)
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

HRESULT CCarChase_Sedan::Add_Objects()
{
	if (GUN_L == m_iWeaponType)
	{
		CSocketObject::SOCKETOBJECT_DESC Desc{};
		Desc.pParentMatrix = m_pParentMatrix;
		Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("buki_l_n");
		Desc.fRotatePecSec = XMConvertToRadians(90.f);
		Desc.fSpeedPecSec = 1.f;
		m_pWeapon = dynamic_cast<CSocketModel*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Gun_Cz75"), &Desc));
	}
	else if (GUN_R == m_iWeaponType)
	{
		CSocketObject::SOCKETOBJECT_DESC Desc{};
		Desc.pParentMatrix = m_pParentMatrix;
		Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("buki_r_n");
		Desc.fRotatePecSec = XMConvertToRadians(90.f);
		Desc.fSpeedPecSec = 1.f;
		m_pWeapon = dynamic_cast<CSocketModel*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Gun_Cz75"), &Desc));
	}

	return S_OK;
}

_uint CCarChase_Sedan::Change_Dir()
{
	//몬스터가 플레이어 왼쪽에 있다면
	//왼쪽에 존재하는 몬스터의 앞에 있는지, 오른쪽에 있는지
	if (m_iWeaponType == GUN_L)
	{
		if (m_iStageDir == DIR_F)
			return DIR_B;
		else if (m_iStageDir == DIR_B)
			return DIR_F;
		else if (m_iStageDir == DIR_R)
			return DIR_L;
		else if (m_iStageDir == DIR_L)
			return DIR_R;
	}
	else if (m_iWeaponType == GUN_R)
	{
		if (m_iStageDir == DIR_F)
			return DIR_B;
		else if (m_iStageDir == DIR_B)
			return DIR_F;
		else if (m_iStageDir == DIR_R)
			return DIR_L;
		else if (m_iStageDir == DIR_L)
			return DIR_R;

	}

	return DIR_END;
}

CCarChase_Sedan* CCarChase_Sedan::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCarChase_Sedan* pInstance = new CCarChase_Sedan(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CCarChase_Sedan::Clone(void* pArg)
{
	CCarChase_Sedan* pInstance = new CCarChase_Sedan(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCarChase_Sedan::Free()
{
	__super::Free();

	Safe_Release(m_pWeapon);
}
