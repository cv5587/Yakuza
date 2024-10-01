#include "LevelTrigger.h"
#include "Player.h"
#include "Level_Loading.h"

CLevelTrigger::CLevelTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTrigger { pDevice, pContext }
{
}

CLevelTrigger::CLevelTrigger(const CLevelTrigger& rhs)
	: CTrigger{ rhs }
{
}

HRESULT CLevelTrigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLevelTrigger::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CLevelTrigger::Priority_Tick(const _float& fTimeDelta)
{


}

void CLevelTrigger::Tick(const _float& fTimeDelta)
{
	m_pColliderCom->Tick(m_WorldMatrix);

	__super::Tick(fTimeDelta);
}

void CLevelTrigger::Late_Tick(const _float& fTimeDelta)
{

	__super::Late_Tick(fTimeDelta);

	if (false == m_bTriggerDead)
	{

		// 조건 충족
		if (m_pColliderCom->Intersect(dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Player"), 0))->Get_PlayerCollider(), m_fMaxScale * 2))
		{
			// 몬스터 확인
			vector<CGameObject*> pMonsters = m_pGameInstance->Get_GameObjects(m_iCurrentLevel, TEXT("Layer_Monster"));

			bool		bDeadCheck = true;

			if (0 != pMonsters.size())
			{
				for (int i = 0; i < pMonsters.size(); i++)
				{
					if (false == pMonsters[i]->isObjectDead())
					{
						bDeadCheck = false;
					}
				}
			}
			

			pMonsters.clear();

			// 요네다 확인
			pMonsters = m_pGameInstance->Get_GameObjects(m_iCurrentLevel, TEXT("Layer_Yoneda"));

			if (0 != pMonsters.size())
			{
				for (int i = 0; i < pMonsters.size(); i++)
				{
					if (false == pMonsters[i]->isObjectDead())
					{
						bDeadCheck = false;
					}
				}
			}

			m_bMoveScene = bDeadCheck;

			if(true == m_bMoveScene)
				m_bTriggerDead = true;
		}
	}
	

}

HRESULT CLevelTrigger::Render()
{
	return S_OK;
}

bool CLevelTrigger::Move_Scene(int& iLevelNum)
{
	// 넘어갈 레벨
	iLevelNum = m_tTriggerDesc.iLevelNum;

	return m_bMoveScene;
}

HRESULT CLevelTrigger::Add_Components(void* pArg)
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

	float	fMaxScale = triggerDesc->tTriggerDesc.vTransform._11;

	fMaxScale = fMaxScale < triggerDesc->tTriggerDesc.vTransform._22 ? triggerDesc->tTriggerDesc.vTransform._22 : fMaxScale;
	fMaxScale = fMaxScale < triggerDesc->tTriggerDesc.vTransform._33 ? triggerDesc->tTriggerDesc.vTransform._33 : fMaxScale;

	m_fMaxScale = fMaxScale;


	return S_OK;
}

CLevelTrigger* CLevelTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevelTrigger* pInstance = new CLevelTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CLevelTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLevelTrigger::Clone(void* pArg)
{
	CLevelTrigger* pInstance = new CLevelTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CLevelTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevelTrigger::Free()
{
	__super::Free();
}
