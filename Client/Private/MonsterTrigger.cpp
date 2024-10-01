#include "MonsterTrigger.h"
#include "Player.h"
#include "Level_Loading.h"

CMonsterTrigger::CMonsterTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTrigger{ pDevice, pContext }
	, m_pFileMgr {CFileTotalMgr::GetInstance()}
{
	Safe_AddRef(m_pFileMgr);
}

CMonsterTrigger::CMonsterTrigger(const CMonsterTrigger& rhs)
	: CTrigger{ rhs }
	, m_pFileMgr{ rhs.m_pFileMgr }
{
	Safe_AddRef(m_pFileMgr);
}

HRESULT CMonsterTrigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonsterTrigger::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMonsterTrigger::Priority_Tick(const _float& fTimeDelta)
{


}

void CMonsterTrigger::Tick(const _float& fTimeDelta)
{
	m_pColliderCom->Tick(m_WorldMatrix);

	__super::Tick(fTimeDelta);
}

void CMonsterTrigger::Late_Tick(const _float& fTimeDelta)
{

	__super::Late_Tick(fTimeDelta);

	if (false == m_bTriggerDead)
	{
		// 조건 충족
		if (m_pColliderCom->Intersect(dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Player"), 0))->Get_PlayerCollider(), 5))
		{
			vector<CGameObject*> pMonsters = m_pGameInstance->Get_GameObjects(m_iCurrentLevel, TEXT("Layer_Monster"));

			bool		bDeadCheck = true;
			for (int i = 0; i < pMonsters.size(); i++)
			{
				if (false == pMonsters[i]->isObjectDead())
				{
					bDeadCheck = false;
				}
			}

			if (true == bDeadCheck)
			{
				m_pFileMgr->Set_MapObj_In_Client_Trigger(m_tTriggerDesc.iTriggerID, m_iCurrentLevel);
				m_bTriggerDead = true;
			}

		}

	}
	
}

HRESULT CMonsterTrigger::Render()
{
	return S_OK;
}

bool CMonsterTrigger::Move_Scene(int& iLevelNum)
{
	// 넘어갈 레벨
	iLevelNum = m_tTriggerDesc.iLevelNum;

	return m_bMoveScene;
}

HRESULT CMonsterTrigger::Add_Components(void* pArg)
{

	TRIGGEROBJ_DESC* triggerDesc = (TRIGGEROBJ_DESC*)pArg;

	m_tTriggerDesc = triggerDesc->tTriggerDesc;


	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_AABB;
	ColliderDesc.vExtents = _float3(triggerDesc->tTriggerDesc.vTransform._11, triggerDesc->tTriggerDesc.vTransform._22, triggerDesc->tTriggerDesc.vTransform._33);
	ColliderDesc.vCenter = _float3(triggerDesc->tTriggerDesc.vTransform._41, triggerDesc->tTriggerDesc.vTransform._42, triggerDesc->tTriggerDesc.vTransform._43);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CMonsterTrigger* CMonsterTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterTrigger* pInstance = new CMonsterTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CLevelTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonsterTrigger::Clone(void* pArg)
{
	CMonsterTrigger* pInstance = new CMonsterTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CLevelTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterTrigger::Free()
{
	__super::Free();

	Safe_Release(m_pFileMgr);
}
