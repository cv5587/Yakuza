#pragma once
#include "AI_Monster.h"

BEGIN(Client)
class CAI_Shakedown :
    public CAI_Monster
{
private:
    CAI_Shakedown();
    CAI_Shakedown(const CAI_Shakedown& rhs);
    virtual ~CAI_Shakedown() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    void Ready_Tree();

private:
    //공격
    CBTNode::NODE_STATE Attack(); //공격 가능할때 공격 선택
    CBTNode::NODE_STATE ATK_CMD();
    CBTNode::NODE_STATE ATK_GuardRun(); 
    CBTNode::NODE_STATE ATK_Rariatto();

public:
    static CAI_Shakedown* Create();
    virtual CBTNode* Clone(void* pArg) override;
    virtual void Free() override;
};
END
