#include "Reactor_Heli.h"

#include "GameInstance.h"

#include "CarChase_Heli.h"
#include "CarChaseCamera.h"

#include "Effect.h"
#include "EffectManager.h"

CReactor_Heli::CReactor_Heli(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCarChase_Reactor{ pDevice, pContext }
{
}

CReactor_Heli::CReactor_Heli(const CReactor_Heli& rhs)
	: CCarChase_Reactor{ rhs }
{
}

HRESULT CReactor_Heli::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CReactor_Heli::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strAnimName = "w_mngcar_e_hel_rkt_stand";

	m_pGameInstance->Play_Loop(TEXT("467e [28].wav"), SOUND_HELI, m_fSound);

	return S_OK;
}

void CReactor_Heli::Priority_Tick(const _float& fTimeDelta)
{
}

void CReactor_Heli::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (!m_isObjectDead) m_pTransformCom->Change_Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	m_isObjectDead = Check_Dead();
}

void CReactor_Heli::Late_Tick(const _float& fTimeDelta)
{
	//컬링
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	//높이값 태우기
	_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (nullptr != m_pNavigationCom)
	{
		float fHeight = m_pNavigationCom->Compute_Height(vCurrentPos);
		vCurrentPos = XMVectorSetY(vCurrentPos, fHeight + 3.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPos);
	}

	for (auto& pMonster : m_Monsters)
		pMonster->Late_Tick(fTimeDelta);
}

HRESULT CReactor_Heli::Ready_Monster(_int* pMonsterTypes)
{
	for (size_t i = 0; i < 3; ++i)
	{
		if (i > 0 && pMonsterTypes[i - 1] == -1)
			break;

		CCarChase_Heli::CARCHASE_MONSTER_DESC Desc{};
		Desc.iWeaponType = i == 0 ? CCarChase_Monster::GUN : pMonsterTypes[i - 1];
		Desc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
		Desc.pBoneMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("anm_root");
		Desc.iLineDir = m_iLineDir;
		Desc.iObjectIndex = m_iObjectIndex + i;
		Desc.iStageDir = m_iStageDir;

		CCarChase_Monster* pMonster = dynamic_cast<CCarChase_Monster*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CarChaseHeli"), &Desc));
		if (nullptr == pMonster)
			return E_FAIL;

		m_Monsters.emplace_back(pMonster);
	}

	return S_OK;
}

void CReactor_Heli::Change_Animation()
{
	if (m_isObjectDead)
	{
		m_strAnimName = "w_mngcar_e_hel_rkt_reload_1";
	}

	//벤에 관한 애니메이션 넣기
	if (m_strAnimName == "w_mngcar_e_hel_rkt_stand")
		m_iAnim = 1;
	if (m_strAnimName == "w_mngcar_e_hel_rkt_reload_1")
		m_iAnim = 0;

	if (m_iAnim == 0 && Checked_Animation_Ratio(0.3f))
	{
		// 카메라 쉐이킹
		CCarChaseCamera* pCamera = dynamic_cast<CCarChaseCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_CARCHASE));
		pCamera->Set_Shaking(true, { 1.f, 1.f, 0.f }, 0.4, 0.7);
	}

	if (m_iAnim == 0 && m_pModelCom->Get_AnimFinished())
	{
		m_isDead = true;
		m_pGameInstance->StopSound(SOUND_HELI);
	}
		
}

HRESULT CReactor_Heli::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Heli"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Material_Heli"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	return S_OK;
}

_bool CReactor_Heli::Check_Dead()
{
	if (m_isObjectDead)
	{
		if (m_isFinishEffect)
			return true;

		m_isFinishEffect = true;
		//헬기 폭파 이펙트
		CEffect::EFFECT_DESC EffectDesc;

		EffectDesc.pWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		CEffectManager::GetInstance()->Heli_Exp(EffectDesc);
		CEffectManager::GetInstance()->Heli_Fire(EffectDesc);

		return true;
	}

	for (auto& pMonster : m_Monsters)
	{
		if (!pMonster->isObjectDead())
			return false;
	}

	return true;
}

CReactor_Heli* CReactor_Heli::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CReactor_Heli* pInstance = new CReactor_Heli(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CReactor_Heli::Clone(void* pArg)
{
	CReactor_Heli* pInstance = new CReactor_Heli(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CReactor_Heli::Free()
{
	__super::Free();
	
}
