#pragma once
#include "AI_CarChase.h"

/*
* DRV, GUN, RKT
* DRV : ¿îÀü¸¸ ÇÔ
* GUN : ¿îÀü¼®¿¡¼­ ÃÑ ½ô
* RKT : µÞ ÀÚ¼®¿¡¼­ ÃÑ½ô 
* 
* DRV, RKT : ÇÑ¼Â
* GUN : È¥ÀÚ
*/

BEGIN(Client)
class CAI_Bike :
    public CAI_CarChase
{
private:
    CAI_Bike();
    CAI_Bike(const CAI_Bike& rhs);
    virtual ~CAI_Bike() = default;

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

    CBTNode::NODE_STATE Check_Ready();
    CBTNode::NODE_STATE Ready();

public:
    static CAI_Bike* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free();
};
END
