#include "QuestTrigger.h"

#include "Player.h"
#include "Level_Loading.h"

#include "QuestManager.h"
#include "UIManager.h"


CQuestTrigger::CQuestTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTrigger { pDevice, pContext },
	m_pQuestManager { CQuestManager::GetInstance() },
	m_pUIManager { CUIManager::GetInstance() }
{
	Safe_AddRef(m_pQuestManager);
	Safe_AddRef(m_pUIManager);
}

CQuestTrigger::CQuestTrigger(const CQuestTrigger& rhs)
	: CTrigger{ rhs },
	m_pQuestManager{ rhs.m_pQuestManager },
	m_pUIManager{ rhs.m_pUIManager }
{
	Safe_AddRef(m_pQuestManager);
	Safe_AddRef(m_pUIManager);
}

HRESULT CQuestTrigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQuestTrigger::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CQuestTrigger::Priority_Tick(const _float& fTimeDelta)
{


}

void CQuestTrigger::Tick(const _float& fTimeDelta)
{
	m_pColliderCom->Tick(m_WorldMatrix);


	__super::Tick(fTimeDelta);
}

void CQuestTrigger::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (false == m_bTriggerDead)
	{
		// 조건 충족
		if (m_pColliderCom->Intersect(dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Player"), 0))->Get_PlayerCollider(), 3))
		{
			if (m_pGameInstance->GetKeyState(DIK_G) == TAP)
			{
				if (m_pQuestManager->Get_CurrentQuestType() == CQuestManager::QUEST_TALK &&
					m_pQuestManager->Get_CurrentQuestTriggerIndex() == m_tTriggerDesc.iTriggerID)
				{
					m_pQuestManager->Set_CurrentColl();
				}
			}
		}
	}
}

HRESULT CQuestTrigger::Render()
{
	return S_OK;
}

bool CQuestTrigger::Move_Scene(int& iLevelNum)
{
	// 넘어갈 레벨
	iLevelNum = m_tTriggerDesc.iLevelNum;

	return m_bMoveScene;
}

HRESULT CQuestTrigger::Add_Components(void* pArg)
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

CQuestTrigger* CQuestTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuestTrigger* pInstance = new CQuestTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CQuestTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CQuestTrigger::Clone(void* pArg)
{
	CQuestTrigger* pInstance = new CQuestTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CQuestTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuestTrigger::Free()
{
	__super::Free();

	Safe_Release(m_pQuestManager);
	Safe_Release(m_pUIManager);
}
