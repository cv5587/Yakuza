#include "Reactor_Sedan.h"

#include "GameInstance.h"

#include "CarChase_Van.h"
#include "CarChaseCamera.h"

CReactor_Sedan::CReactor_Sedan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCarChase_Reactor{ pDevice, pContext }
{
}

CReactor_Sedan::CReactor_Sedan(const CReactor_Sedan& rhs)
	: CCarChase_Reactor{ rhs }
{
}

HRESULT CReactor_Sedan::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CReactor_Sedan::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//m_strAnimName = "w_mngcar_e_van_wpr_aim_st_1";

	m_pGameInstance->Play_Loop(TEXT("e_car_appear_run_loop1_0.wav"), SOUND_SEDAN, m_fSound);

	m_iAnim = 7;

	return S_OK;
}

void CReactor_Sedan::Priority_Tick(const _float& fTimeDelta)
{
}

void CReactor_Sedan::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_isObjectDead = Check_Dead();
}

void CReactor_Sedan::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CReactor_Sedan::Ready_Monster(_int* pMonsterTypes)
{
	for (size_t i = 0; i < 3; ++i)
	{
		if (i > 0 && pMonsterTypes[i - 1] == -1)
			break;

		CCarChase_Van::CARCHASE_MONSTER_DESC Desc{};
		Desc.iWeaponType = i == 0 ? CCarChase_Monster::DRV : pMonsterTypes[i - 1];
		Desc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
		Desc.pBoneMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("anm_root");
		Desc.iLineDir = m_iLineDir;
		Desc.iObjectIndex = m_iObjectIndex + i;
		Desc.iStageDir = m_iStageDir;

		CCarChase_Monster* pMonster = dynamic_cast<CCarChase_Monster*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CarChaseSedan"), &Desc));
		if (nullptr == pMonster)
			return E_FAIL;

		m_Monsters.emplace_back(pMonster);
	}

	return S_OK;
}

void CReactor_Sedan::Change_Animation()
{
	if (m_isObjectDead)
	{
		m_strAnimName = "w_mngcar_c_car_ded_b_1";
		if (!m_isSound)
		{
			m_pGameInstance->StopSound(SOUND_SEDAN);
			m_pGameInstance->PlaySound_W(TEXT("467e [3].wav"), SOUND_SEDAN, m_fSound);
			m_isSound = true;
		}
	}

	//벤에 관한 애니메이션 넣기
	if (m_strAnimName == "w_mngcar_c_car_ded_b_1")
		m_iAnim = 2;

	if (m_iAnim == 1 && Checked_Animation_Ratio(0.3f))
	{
		// 카메라 쉐이킹
		CCarChaseCamera* pCamera = dynamic_cast<CCarChaseCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_CARCHASE));
		pCamera->Set_Shaking(true, { 1.f, 1.f, 0.f }, 0.3, 0.5);
	}


	if (m_iAnim == 2 && m_pModelCom->Get_AnimFinished(2))
		m_isDead = true;
}

HRESULT CReactor_Sedan::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Sedan"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Material_Sedan"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	return S_OK;
}

CReactor_Sedan* CReactor_Sedan::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CReactor_Sedan* pInstance = new CReactor_Sedan(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CReactor_Sedan::Clone(void* pArg)
{
	CReactor_Sedan* pInstance = new CReactor_Sedan(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CReactor_Sedan::Free()
{
	__super::Free();
	
}
