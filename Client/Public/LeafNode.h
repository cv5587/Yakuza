#pragma once
#include "BTNode.h"

#include "Client_Defines.h"

/* 트리의 마지막 노드 (행동 노드)*/

BEGIN(Client)
class CLeafNode :
    public CBTNode
{
private:
	CLeafNode();
	virtual ~CLeafNode() = default;

public:
	HRESULT Initialize(function<CBTNode::NODE_STATE()> Action);
	virtual CBTNode::NODE_STATE Execute();

private:
	function<CBTNode::NODE_STATE()> m_pAction = { nullptr };

public:
	static CBTNode* Create(function<CBTNode::NODE_STATE()> Action);
	virtual void Free() override;
};

END