#pragma once
#include "AI_Monster.h"

BEGIN(Client)
class CAI_Kuze :
    public CAI_Monster
{
private:
    CAI_Kuze();
    CAI_Kuze(const CAI_Kuze& rhs);
    virtual ~CAI_Kuze() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:



    _float m_fAttackCount = 0;
    

private:
    void Ready_Tree();

private:
    virtual CBTNode::NODE_STATE Check_Sway();

    CBTNode::NODE_STATE Check_QTE();
    CBTNode::NODE_STATE QTE();

    CBTNode::NODE_STATE Check_Attack(); // 공격 가능한 상태인지 체크
    CBTNode::NODE_STATE Attack(); //공격 가능할때 공격 선택
    CBTNode::NODE_STATE ATK_Heavy();
    CBTNode::NODE_STATE ATK_Hiji_2Ren();
    CBTNode::NODE_STATE ATK_Jab();
    CBTNode::NODE_STATE ATK_CMD_A();
    CBTNode::NODE_STATE ATK_CMD_B();
    CBTNode::NODE_STATE ATK_HeadButt();
    CBTNode::NODE_STATE ATK_Renda();

    virtual CBTNode::NODE_STATE Check_Distance();
    virtual CBTNode::NODE_STATE Run();


public:
    static CAI_Kuze* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free() override;
};
END
