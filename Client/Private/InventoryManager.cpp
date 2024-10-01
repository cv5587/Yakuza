#include "InventoryManager.h"
#include "GameInstance.h"
#include "ItemData.h"


CInventoryManager::CInventoryManager()
	:m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CInventoryManager::CInventoryManager(const CInventoryManager& rhs)
{
}

HRESULT CInventoryManager::Initialize()
{
	return S_OK;
}

HRESULT CInventoryManager::Tick(const _float& fTimeDelta)
{
	return S_OK;
}

HRESULT CInventoryManager::Late_Tick(const _float& fTimeDelta)
{
	return S_OK;
}

CInventoryManager* CInventoryManager::Create()
{
	CInventoryManager* pInstance = new CInventoryManager();
	if(FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CInventoryManager::Free()
{
	Safe_Release(m_pGameInstance);

	for (auto& iter : m_Items)
		Safe_Release(iter);
}
