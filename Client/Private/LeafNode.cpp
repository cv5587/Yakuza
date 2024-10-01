#include "LeafNode.h"

CLeafNode::CLeafNode()
{
}

HRESULT CLeafNode::Initialize(function<CBTNode::NODE_STATE()> Action)
{
    m_pAction = Action;

    return S_OK;
}

CBTNode::NODE_STATE CLeafNode::Execute()
{
    return m_pAction();
}

CBTNode* CLeafNode::Create(function<CBTNode::NODE_STATE()> Action)
{
    CLeafNode* pInstance = new CLeafNode();
    
    if (FAILED(pInstance->Initialize(Action)))
        Safe_Release(pInstance);

    return pInstance;
}

void CLeafNode::Free()
{
    __super::Free();
}
