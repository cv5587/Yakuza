#pragma once
#include "AI_RoadWalker.h"

BEGIN(Client)
class CAI_RoadYOP :
    public CAI_RoadWalker
{
private:
    CAI_RoadYOP();
    CAI_RoadYOP(const CAI_RoadYOP& rhs);
    virtual ~CAI_RoadYOP() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    virtual void Ready_Root();

public:
    static CAI_RoadYOP* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free();
};
END
