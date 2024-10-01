#include "MonsterGroup.h"

#include "GameInstance.h"
#include "FightManager.h"
#include "FileTotalMgr.h"

#include "LandObject.h"
#include "Monster.h"

#include "Player.h"

CMonsterGroup::CMonsterGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext },
	m_pFightManager{CFightManager::GetInstance() }
{
	Safe_AddRef(m_pFightManager);
}

CMonsterGroup::CMonsterGroup(const CMonsterGroup& rhs)
	: CLandObject{ rhs },
	m_pFightManager{CFightManager::GetInstance()}
{
	Safe_AddRef(m_pFightManager);
}

HRESULT CMonsterGroup::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonsterGroup::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MONSTERGROUPDESC* monsterDesc = (MONSTERGROUPDESC*)pArg;
	m_iGroupNum = monsterDesc->iGroupNum;

	for (int i = 0; i < monsterDesc->vMonsters.size(); i++)
	{
		//Rush
		if (CFileTotalMgr::OBJECT_TYPE::MONSTER_RUSH == monsterDesc->vMonsters[i].iObjectType)
		{		
			m_Monsters.push_back(dynamic_cast<CMonster*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_RushYakuza"), &monsterDesc->vMonsters[i])));
			m_Monsters.back()->Set_Adventure(true);
		}
		else if (CFileTotalMgr::OBJECT_TYPE::MONSTER_DEFAULT == monsterDesc->vMonsters[i].iObjectType)
		{
			m_Monsters.push_back(dynamic_cast<CMonster*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Default"), &monsterDesc->vMonsters[i])));
			m_Monsters.back()->Set_Adventure(true);
		}
	}

	return S_OK;
}

void CMonsterGroup::Priority_Tick(const _float& fTimeDelta)
{
	for (auto& pMonster : m_Monsters)
		pMonster->Priority_Tick(fTimeDelta);
}

void CMonsterGroup::Tick(const _float& fTimeDelta)
{
	for (auto& pMonster : m_Monsters)
		pMonster->Tick(fTimeDelta);

	Check_Fight();
}

void CMonsterGroup::Late_Tick(const _float& fTimeDelta)
{
	for (auto& pMonster : m_Monsters)
		pMonster->Late_Tick(fTimeDelta);
}

void CMonsterGroup::Check_Fight()
{
	if (m_isFight) return;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Player"), 0));
	_vector vPlayerPos = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	//몬스터 
	for (auto& pMonster : m_Monsters)
	{
		_vector vMonsterPos = pMonster->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

		float fDistance = XMVectorGetX(XMVector3Length(vPlayerPos - vMonsterPos));
		
		if (fDistance < 4.f)
		{
			m_isFight = true;
			break;
		}
	}

	//한 몬스터라도 플레이어와 거리가 가까우면 공격
	if (m_isFight == true)
	{
		m_pFightManager->Set_FightStage(true, this);
		pPlayer->Set_CurrentStyle(CPlayer::KRS);

		for (auto& pMonster : m_Monsters)
		{
			pMonster->Set_Adventure(false);
			pMonster->Set_Start(true);
		}
	}
}

CMonsterGroup* CMonsterGroup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterGroup* pInstance = new CMonsterGroup(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CMonsterGroup::Clone(void* pArg)
{
	CMonsterGroup* pInstance = new CMonsterGroup(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CMonsterGroup::Free()
{
	for (auto& iter : m_Monsters)
	{
		Safe_Release(iter);
	}

	Safe_Release(m_pFightManager);

	__super::Free();
}

string CMonsterGroup::Get_CurrentAnimationName()
{
	return string();
}
