#include "Reactor_Bike.h"

#include "GameInstance.h"

#include "CarChase_Bike.h"
#include "CarChaseCamera.h"

CReactor_Bike::CReactor_Bike(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCarChase_Reactor{ pDevice, pContext }
{
}

CReactor_Bike::CReactor_Bike(const CReactor_Bike& rhs)
	: CCarChase_Reactor{ rhs }
{
}

HRESULT CReactor_Bike::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CReactor_Bike::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iAnim = 3;

	m_pGameInstance->Play_Loop(TEXT("e_bike_appear_run_loop1_0.wav"), SOUND_BIKE, m_fSound);

	return S_OK;
}

void CReactor_Bike::Priority_Tick(const _float& fTimeDelta)
{
}

void CReactor_Bike::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_isObjectDead = Check_Dead();
}

void CReactor_Bike::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CReactor_Bike::Ready_Monster(_int* pMonsterTypes)
{
	for (size_t i = 0; i < 2; ++i)
	{
		if (pMonsterTypes[i] == -1)
			break;

		CCarChase_Bike::CARCHASE_MONSTER_DESC Desc{};
		Desc.iWeaponType = pMonsterTypes[i];
		Desc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
		Desc.pBoneMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("anm_root");
		Desc.iLineDir = m_iLineDir;
		Desc.iStageDir = m_iStageDir;
		Desc.iObjectIndex = m_iObjectIndex + i;

		CCarChase_Monster* pMonster = dynamic_cast<CCarChase_Monster*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CarChaseBike"), &Desc));
		if (nullptr == pMonster)
			return E_FAIL;

		m_Monsters.emplace_back(pMonster);
	}

	return S_OK;
}

void CReactor_Bike::Change_Animation()
{
	if (m_isObjectDead)
	{
		m_strAnimName = "w_mngcar_bik_tentou_b_1";
		if (!m_isSound)
		{
			m_pGameInstance->StopSound(SOUND_BIKE);
			m_pGameInstance->PlaySound_W(TEXT("e_bike_crush_0.wav"), SOUND_BIKE, m_fSound);
			m_isSound = true;
		}
	}

	//바이크에 관한 애니메이션 넣기
	if (m_strAnimName == "w_mngcar_bik_tentou_b_1")
	{
		m_iAnim = 6;
		m_pModelCom->Set_AnimationIndex(m_iAnim, 4.f);
	}

	if (m_iAnim == 6 && Checked_Animation_Ratio(0.3f))
	{
		// 카메라 쉐이킹
		CCarChaseCamera* pCamera = dynamic_cast<CCarChaseCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_CARCHASE));
		pCamera->Set_Shaking(true, { 1.f, 1.f, 0.f }, 0.3, 0.4);
	}

	if (m_iAnim == 6 && m_pModelCom->Get_AnimFinished(6))
		m_isDead = true;
}

HRESULT CReactor_Bike::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Bike"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Material_Bike"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	return S_OK;
}

CReactor_Bike* CReactor_Bike::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CReactor_Bike* pInstance = new CReactor_Bike(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CReactor_Bike::Clone(void* pArg)
{
	CReactor_Bike* pInstance = new CReactor_Bike(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CReactor_Bike::Free()
{
	__super::Free();
	
}
