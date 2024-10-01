#pragma once
#include "AI_RoadStanding.h"

#include "Client_Defines.h"

BEGIN(Client)
class CAI_RoadCAB:
    public CAI_RoadStanding
{
private:
    CAI_RoadCAB();
    CAI_RoadCAB(const CAI_RoadCAB& rhs);
    virtual ~CAI_RoadCAB() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    virtual void Ready_Root();

private:
    //서있는 상태
    virtual CBTNode::NODE_STATE Ground_The_Player() override;

private:
    int         m_iDelayDelta = { 0 };

public:
    static CAI_RoadCAB* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free();
};
END