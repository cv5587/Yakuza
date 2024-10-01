#pragma once
#include "AI_Monster.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)
class CAI_RushYakuza :
    public CAI_Monster
{
private:
    CAI_RushYakuza();
    CAI_RushYakuza(const CAI_RushYakuza& rhs);
    virtual ~CAI_RushYakuza() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    CModel* m_pModelCom = { nullptr };

    _uint m_iAdventureIdelMotion = { 0 };

private:
    void Ready_Tree();

private:
    //어드벤쳐 일 경우
    //가만히 서있는다.
    //플레이어를 발견하면 뛰어온다.
    CBTNode::NODE_STATE Check_Adventure();
    CBTNode::NODE_STATE Adventure();

    CBTNode::NODE_STATE Attack(); //공격 가능한 상태일때 공격 선택
    CBTNode::NODE_STATE Angry_Attack(); // 화가 난 상태일때 공격 선택

    CBTNode::NODE_STATE ATK_Punch();
    CBTNode::NODE_STATE ATK_CMD();

public:
    static CAI_RushYakuza* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free() override;
};
END
