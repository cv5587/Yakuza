#pragma once
#include "BTNode.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CAnim;
class CAStart;
class CNavigation;
END

BEGIN(Client)
class CAI_RoadWalker abstract :
    public CBTNode
{
public:
    typedef struct tAIAdventureDesc {
        CAnim* pAnimCom;
        CAStart* pAStartCom;
        CNavigation* pNavigation;
        class CAdventure* pThis;

        _uint* pState;
    } AI_ADVENTURE_DESC;

    enum SKILL {
        SKILL_IDLE,
        SKILL_WALK,
        SKILL_TURN,
        SKILL_BACK,
        SKILL_END
    };

protected:
    CAI_RoadWalker();
    CAI_RoadWalker(const CAI_RoadWalker& rhs);
    virtual ~CAI_RoadWalker() = default;

public:
    _bool isWalk() { return m_isWalk; }

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute() = 0;

protected:
    CBTNode* m_pRootNode = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };
    CAnim* m_pAnimCom = { nullptr };
    CAStart* m_pAStartCom = { nullptr };
    CNavigation* m_pNavigationCom = { nullptr };

    class CPlayer* m_pPlayer = { nullptr }; // 플레이어의 주소를 가지고 있도록 한다.
    class CAdventure* m_pThis = { nullptr }; // 자기 자신의 주소를 가지고 있도록 한다.

    _uint* m_pState = { nullptr };

protected:
    _uint m_iSkill = { SKILL_END };
    _bool m_isWayPointFinish = { true };

protected:
    _bool m_isWalk = { true };
    _bool m_isTurn = { false }; // 옆으로 턴 해야 하는가
    _bool m_isBack = { false }; //뒤를 돌아야 하는가
    _int m_iDir = { DIR_END };

    _float  m_fDegree = { 0 };
    _vector m_vNextDir;

    _float  m_fTurnTimer = { 0 };


protected:
    virtual void Ready_Root() = 0;

protected:
    //충돌 감지
    virtual CBTNode::NODE_STATE Check_Coll();
    virtual CBTNode::NODE_STATE Coll();

    virtual CBTNode::NODE_STATE Check_Walk();
    virtual CBTNode::NODE_STATE Back();
    virtual CBTNode::NODE_STATE Turn();
    virtual CBTNode::NODE_STATE Walk();


    void    Rotate_Dir(XMVECTOR& currentDirection, XMVECTOR targetDirection, bool& bEnd);

public:
    virtual void Free();
};
END