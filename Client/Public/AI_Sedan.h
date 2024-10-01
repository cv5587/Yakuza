#pragma once
#include "AI_CarChase.h"

BEGIN(Client)
class CAI_Sedan :
    public CAI_CarChase
{
private:
    CAI_Sedan();
    CAI_Sedan(const CAI_Sedan& rhs);
    virtual ~CAI_Sedan() = default;
    
public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

protected:
    virtual void Ready_Tree();

private:
    _uint m_iCurrentDir = { DIR_END };
    _uint m_iPrevDir = { DIR_END };

private:
    virtual CBTNode::NODE_STATE Dead() override;
    virtual CBTNode::NODE_STATE Hit() override;

    CBTNode::NODE_STATE Check_Attack();
    CBTNode::NODE_STATE ATK_Shot();

    CBTNode::NODE_STATE Check_Idle();
    CBTNode::NODE_STATE Idle();

public:
    static CAI_Sedan* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free();
};
END
