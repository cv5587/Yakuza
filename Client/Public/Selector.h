#pragma once
#include "BTNode.h"

#include "Client_Defines.h"

/*  Or ¿¬»ê */
BEGIN(Client)
class CSelector :
    public CBTNode
{
private:
    CSelector();
    virtual ~CSelector() = default;

public:
    virtual NODE_STATE Execute();
    void Add_Children(CBTNode* pChildren);

private:
    vector<CBTNode*> m_ChildNode;

public:
    static CSelector* Create();
    virtual void Free() override;
};

END