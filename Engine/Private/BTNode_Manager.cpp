#include "BTNode_Manager.h"

#include "BTNode.h"

CBTNode_Manager::CBTNode_Manager()
{
}

HRESULT CBTNode_Manager::Initialize(_uint iMaxLevelIndex)
{
	m_iMaxLevelIndex = iMaxLevelIndex;

	m_pPrototypes = new map<const wstring, CBTNode*>[iMaxLevelIndex];

	return S_OK;
}

HRESULT CBTNode_Manager::Add_BTNode_Prototype(_uint iLevelIndex, const wstring strBTNodeTag, CBTNode* pBTNode)
{
	if (nullptr != Find_BTNode_Prototype(iLevelIndex, strBTNodeTag))
		return E_FAIL;


	m_pPrototypes[iLevelIndex].emplace(strBTNodeTag, pBTNode);

	return S_OK;
}

CBTNode* CBTNode_Manager::Add_BTNode(_uint iLevelIndex, const wstring strBTNodeTag, void* pArg)
{
	CBTNode* pBTNode = Find_BTNode_Prototype(iLevelIndex, strBTNodeTag);

	if (pBTNode == nullptr)
		return nullptr;
	
	return pBTNode->Clone(pArg);
}

void CBTNode_Manager::Clear(_uint iLevelIndex)
{
	if (m_iMaxLevelIndex <= iLevelIndex)
		return;

	for (auto& Pair : m_pPrototypes[iLevelIndex])
	{
		Safe_Release(Pair.second);
	}


	m_pPrototypes[iLevelIndex].clear();
}

CBTNode* CBTNode_Manager::Find_BTNode_Prototype(_uint iLevelIndex, const wstring strBTNodeTag)
{
	auto pPrototype = m_pPrototypes[iLevelIndex].find(strBTNodeTag);

	if (m_pPrototypes[iLevelIndex].end() == pPrototype)
		return nullptr;

	return pPrototype->second;
}

CBTNode_Manager* CBTNode_Manager::Create(_uint iMaxLevelIndex)
{
	CBTNode_Manager* pInstance = new CBTNode_Manager();

	if (FAILED(pInstance->Initialize(iMaxLevelIndex)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBTNode_Manager::Free()
{
	for (size_t i = 0; i < m_iMaxLevelIndex; ++i)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);

		m_pPrototypes[i].clear();
	}
}
