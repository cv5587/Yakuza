#pragma once
#include "AI_CarChase.h"

BEGIN(Client)
class CAI_Heli :
    public CAI_CarChase
{
private:
    CAI_Heli();
    CAI_Heli(const CAI_Heli& rhs);
    virtual ~CAI_Heli() = default;

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

    CBTNode::NODE_STATE Check_Attack();
    CBTNode::NODE_STATE ATK_Shot();

    CBTNode::NODE_STATE Check_Idle();
    CBTNode::NODE_STATE Idle();

public:
    static CAI_Heli* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free();
};
END
