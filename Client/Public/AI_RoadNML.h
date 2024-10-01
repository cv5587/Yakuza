#pragma once
#include "AI_RoadWalker.h"

BEGIN(Client)
class CAI_RoadNML :
    public CAI_RoadWalker
{
private:
    CAI_RoadNML();
    CAI_RoadNML(const CAI_RoadNML& rhs);
    virtual ~CAI_RoadNML() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    virtual void Ready_Root();

public:
    static CAI_RoadNML* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free();
};
END
