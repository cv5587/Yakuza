#include "RandomManager.h"

CRandomManager::CRandomManager()
{
}

HRESULT CRandomManager::Initialize()
{
	return S_OK;
}

CRandomManager* CRandomManager::Create()
{
	CRandomManager* pInstance = new CRandomManager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CRandomManager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRandomManager::Free()
{
}
