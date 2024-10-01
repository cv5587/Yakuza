#include "Sequance.h"

#include "BTNode.h"

CSequance::CSequance()
{
}

CBTNode::NODE_STATE CSequance::Execute()
{
	// Add하면 children에 넣어짐
	for (auto& pChildren : m_Childrens)
	{
		// execute가 매tick 마다 돌기
		// for문 돌기
		CBTNode::NODE_STATE eResult = pChildren->Execute();

		//running : 실행중
		if (eResult == CBTNode::FAIL || eResult == CBTNode::RUNNING)
			return eResult;
	}

	// success가 나오면 자식노드를 돌기
	return CBTNode::SUCCESS;
}

void CSequance::Add_Children(CBTNode* pChildren)
{
	m_Childrens.emplace_back(pChildren);
}

CSequance* CSequance::Create()
{
    return new CSequance();
}

void CSequance::Free()
{
    __super::Free();

    for (auto& pChildren : m_Childrens)
        Safe_Release(pChildren);
}

