#pragma once
#include "AI_RoadStanding.h"

#include "Client_Defines.h"

BEGIN(Client)
class CAI_RoadStanding_NML:
    public CAI_RoadStanding
{
public:
    //enum MOTION_TYPE { MOTION_A, MOTION_B, MOTION_END };

    enum MOTION_TYPE {
        MOTION_TALK_A,
        MOTION_TALK_B,
        MOTION_LISTEN,
        MOTION_END
    };

private:
    CAI_RoadStanding_NML();
    CAI_RoadStanding_NML(const CAI_RoadStanding_NML& rhs);
    virtual ~CAI_RoadStanding_NML() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

public:
    void    Set_MotionPointer(_uint* pMotion) { m_pMotionType = pMotion; }

private:
    _uint*      m_pMotionType = { 0 };
    int         m_iDelayDelta = { 0 };

    _int         m_iPreSound = { -1 };

private:
    virtual void Ready_Root();

private:
    //서있는 상태
    virtual CBTNode::NODE_STATE Stand() override;

    void    Execute_Anim();

public:
    static CAI_RoadStanding_NML* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free();
};
END