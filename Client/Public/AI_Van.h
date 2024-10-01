#pragma once
#include "AI_CarChase.h"

BEGIN(Client)
class CAI_Van :
    public CAI_CarChase
{
private:
    CAI_Van();
    CAI_Van(const CAI_Van& rhs);
    virtual ~CAI_Van() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

protected:
    virtual void Ready_Tree();

private:
    virtual CBTNode::NODE_STATE Dead() override;
    virtual CBTNode::NODE_STATE Hit() override;
    virtual CBTNode::NODE_STATE Sync() override;

    CBTNode::NODE_STATE Check_Attack();
    CBTNode::NODE_STATE ATK_Shot();

    CBTNode::NODE_STATE Check_Sit();
    CBTNode::NODE_STATE SitAndUp();

    CBTNode::NODE_STATE Idle();

public:
    static CAI_Van* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free();
};
END
