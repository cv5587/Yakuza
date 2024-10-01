#pragma once
#include "AI_RoadStanding.h"

#include "Client_Defines.h"

BEGIN(Client)
class CAI_RoadTissue:
    public CAI_RoadStanding
{
private:
    CAI_RoadTissue();
    CAI_RoadTissue(const CAI_RoadTissue& rhs);
    virtual ~CAI_RoadTissue() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    virtual void Ready_Root();

private:
    //서있는 상태
    virtual CBTNode::NODE_STATE Check_Stand() override;
    virtual CBTNode::NODE_STATE Ground_The_Player() override;

private:
    _float      m_fGroundDelta = { 0.f };
    _bool       m_bStay = { false };

public:
    static CAI_RoadTissue* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free();
};
END