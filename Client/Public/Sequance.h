#pragma once
#include "BTNode.h"

#include "Client_Defines.h"

/* or ¿¬»ê*/
BEGIN(Client)
class CSequance :
    public CBTNode
{
private:
	CSequance();
	virtual ~CSequance() = default;

public:
	virtual NODE_STATE Execute();
	void Add_Children(CBTNode* pChildren);

private:
	vector<CBTNode*> m_Childrens;

public:
	static CSequance* Create();
	virtual void Free() override;
};
END