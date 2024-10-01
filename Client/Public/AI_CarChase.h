#pragma once
#include "BTNode.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CAnim;
class CGameInstance;
END

BEGIN(Client)
class CAI_CarChase abstract:
    public CBTNode
{
public:
    typedef struct tAICarChaseDesc {
        CAnim* pAnim[2];

        _uint* pState;
        _uint* pDir;
        _uint* pCurrentAnimType;
        const _uint* pWeaponType;
        class CCarChase_Monster* pThis;
    } AI_CARCHASE_DESC;

    enum SKILL {
        SKILL_HIT,
        SKILL_AIM,
        SKILL_SHOT,
        SKILL_DEAD,
        SKILL_END
    };

protected:
    CAI_CarChase();
    CAI_CarChase(const CAI_CarChase& rhs);
    virtual ~CAI_CarChase() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute() = 0;

    _bool Find_PlayerCurrentAnimationName(string strAnimName);

protected:
    virtual void Ready_Tree() = 0;
    _float DistanceFromPlayer();
    _uint AngleFromPlayer();
    _bool isBehine();
    void LookAtPlayer();

    void Play_Hit();

protected:
    CBTNode* m_pRootNode = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };

    CAnim* m_pAnimCom[2] = {nullptr};
    class CHighway_Taxi* m_pPlayer = { nullptr };
    class CCarChase_Monster* m_pThis = { nullptr };

    _uint* m_pState = { nullptr };
    _uint* m_pDir = { nullptr };
    _uint* m_pCurrentAnimType = { nullptr };
    const _uint* m_pWeaponType = { nullptr };

protected:
    _uint m_iSkill = { SKILL_END };

    _bool m_isAttack = { false };
    _bool m_isAtkReady = { false };
    _bool m_isSit = { false };
    _bool m_isHit = { false };

protected:
    _float m_fDelayAttackDuration = { 3.f };
    _float m_fAttackDelayTime = { 0.f }; // 공격이 끝난 후 지속시간

    _float m_fDelayAttackReadyDuration = { 4.f };
    _float m_fAttackReadyDelayTime = { 0.f }; // 공격 준비 시간

    _float m_fSitDuration = { 7.f };
    _float m_fUpDuration = { 7.f };
    _float m_fSitUpTime = { 0.f };

protected:
    virtual CBTNode::NODE_STATE Check_Down();
    virtual CBTNode::NODE_STATE Dead();

    virtual CBTNode::NODE_STATE Check_Hit();
    virtual CBTNode::NODE_STATE Hit();

    virtual CBTNode::NODE_STATE Check_Sync();
    virtual CBTNode::NODE_STATE Sync();

public:
    virtual void Free();
};
END
