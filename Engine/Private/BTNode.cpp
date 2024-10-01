#include "BTNode.h"

CBTNode::CBTNode()
{
}

CBTNode::CBTNode(void* pArg)
{
	m_isClone = true;
}

CBTNode* CBTNode::Clone(void* pArg)
{
	return nullptr;
}

void CBTNode::Free()
{
}
