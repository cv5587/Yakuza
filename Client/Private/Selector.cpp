#include "Selector.h"

CSelector::CSelector()
{
}

CBTNode::NODE_STATE CSelector::Execute()
{
    // Failure 상태와 Running 상태일때 return
    // Sccess일때 계속 루프
    for (auto& pChildren : m_ChildNode)
    {
        CBTNode::NODE_STATE eState = pChildren->Execute();

        // Success와 Running일때 바로나옴
        // coll 돌렸는데 coll->success면? : root에서 succes나 run이면 return돼서 walk라는게 실행이 안됨
        // Selector : 자식노드가 하나라도 success가 나오면 실행안시킴
        // Sequence : and 연산
        if (eState == CBTNode::SUCCESS || eState == CBTNode::RUNNING)
            return eState;
    }

    return CBTNode::FAIL;
}

void CSelector::Add_Children(CBTNode* pChildren)
{
    m_ChildNode.emplace_back(pChildren);
}

CSelector* CSelector::Create()
{
    return new CSelector();
}

void CSelector::Free()
{
    __super::Free();

    for (auto& pChildren : m_ChildNode)
        Safe_Release(pChildren);
}
