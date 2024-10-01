#pragma once
#include "BTNode.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CAnim;
END

BEGIN(Client)
class CAI_Monster :
    public CBTNode
{
public:
    //스킬 리스트
    enum SKILL {
        SKILL_IDLE,
        SKILL_RUN,
        SKILL_SHIFT,
        SKILL_SWAY,
        SKILL_HIT,
        SKILL_ATK_NORMAL,
        SKILL_CMD,
        SKILL_CMD_A,
        SKILL_CMD_B,
        SKILL_CMD_HEADBUTT,
        SKILL_CMD_RENDA,
        SKILL_JAB,
        SKILL_HIJI_2REN,
        SKILL_RENDA,
        SKILL_PUNCH,
        SKILL_HEAVY,
        SKILL_GUARD_RUN,
        SKILL_RUN_ATK,
        SKILL_KICK,
        SKILL_RARIATTO,
        SKILL_DOWN,
        SKILL_ANGRY_CHOP,
        SKILL_ANGRY_KICK,
        SKILL_QTE,

        SKILL_GUARD,
        SKILL_DEAD,
        SKILL_END
    };
    enum PLAYER_ATK_DIR { F, B, L, R, PLAYER_ATK_DIR_END };

    typedef struct tAIMonster {
        class CAnim* pAnim[2];
        class CMonster* pThis;
        _uint* pState;
        _uint* pCurrentAnimType;
    } AI_MONSTER_DESC;

protected:
    CAI_Monster();
    CAI_Monster(const CAI_Monster& rhs);
    virtual ~CAI_Monster() = default;

public:
    _bool isSway() { return m_isSway; }
    _bool isGuard() { return m_isGuard; }
    _bool isAttack() { return m_isAttack; }
    _uint Get_DownDir();

public:
    void Set_Sync(_bool isSync) { m_isSync = isSync; }
    void Set_Start(_bool isStart) { m_isStart = isStart; }
    void Set_Adventure(_bool isAdventure) { m_isAdventer = isAdventure; }
    void Reset_AI();

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute() = 0;

    _bool Find_PlayerCurrentAnimationName(string strAnimName);

protected:
    CBTNode* m_pRootNode = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };
    CAnim* m_pAnimCom[2] = {nullptr};

    class CPlayer* m_pPlayer = { nullptr }; // 플레이어의 주소를 가지고 있도록 한다.
    class CMonster* m_pThis = { nullptr }; // 자기 자신의 주소를 가지고 있도록 한다.

    _uint* m_pState = { nullptr };
    _uint* m_pCurrentAnimType = { nullptr };

protected:
    _bool m_isStart = { false };
    _bool m_isAdventer = { false };
    _bool m_isAttack = { false };
    _bool m_isAngry = { false };
    _bool m_isSync = { false };
    _bool m_isBreak = { false };
    _bool m_isGuard = { false };
    _bool m_isSway = { false };
    _bool m_isRun = { false };

    _bool m_isQTE = { false }; // qte가 한번만 켜져야 해서 필요함.

    _bool m_isPlayerDownAtk = { false }; // 플레이어에게 다운 어택을 한번 사용했는가를 체크

    _uint m_iSkill = { SKILL_END };
    _uint m_iMonsterType = { 0 };

    _bool m_isSycnSound = { false };

protected:
    _float m_fDelayAttackDuration = { 2.f };
    _float m_fAttackDelayTime = { 0.f }; // 공격이 끝난 후 지속시간

    _float m_fBreakDuration = { 0.f }; // 랜덤으로 부여받는다.
    _float m_fBreakTime = { 0.f };

    _float m_fGuardDuration = { 4.f }; // 가드 지속시간
    _float m_fGuardTime = { 0.f };

    _float m_fSyncDuration = { 3.f }; // 싱크액션 지속시간
    _float m_fSyncTime = { 0.f };

protected:
    _float m_fCmbNum = { 0.f }; // 스킬 발동시 확률적으로 몇번 공격할것인지 저장.
    _float m_fCmbCount = { 0.f }; // 콤보 누적 카운트

    _float m_fGuardBroken = { 20.f };
    _float m_fGuardAtkAcc = { 0.f };

protected:
    _float m_fSoundVoice = { 1.f };
    _float m_fSoundEffect = { 1.f };

protected:
    _float2 m_fSwayDistance = {};

protected:
    //전투 시작 시 시작 모션
    virtual CBTNode::NODE_STATE Check_Start();
    virtual CBTNode::NODE_STATE Start();

    //플레이어에게 잡혀있는가?
    virtual CBTNode::NODE_STATE Chcek_Sync(); // 싱크 모션을 해야하는가?

    //다운상태
    virtual CBTNode::NODE_STATE Check_Down();
    virtual CBTNode::NODE_STATE StandUpAndDead();
    virtual CBTNode::NODE_STATE StandUp();
    virtual CBTNode::NODE_STATE Dead();

    //플레이어 다운 시 (다운어택)
    CBTNode::NODE_STATE Check_PlayerDown();
    CBTNode::NODE_STATE ATK_Down();

    //스웨이
    virtual CBTNode::NODE_STATE Check_Sway();
    virtual CBTNode::NODE_STATE Sway();

    //데미지 DMD
    virtual CBTNode::NODE_STATE Check_Hit();
    virtual CBTNode::NODE_STATE HitAndGuard(); //히트모션을 할 것인지, 가드를 할 것인지

    virtual CBTNode::NODE_STATE Hit();
    virtual CBTNode::NODE_STATE Guard();

    //분노
    virtual CBTNode::NODE_STATE Check_Angry(); // 화가 난 상태인지 체크
    virtual CBTNode::NODE_STATE Angry();

    virtual CBTNode::NODE_STATE ATK_Angry_Punch(); // 화가 난 상태일때 펀치
    virtual CBTNode::NODE_STATE ATK_Angry_Kick(); // 화가 난 상태일때 발차기

    //공격
    CBTNode::NODE_STATE Check_Attack(); // 공격 가능한 상태인지 체크

    //Break
    virtual CBTNode::NODE_STATE Check_Break(); // 쉬는 타임인가?
    virtual CBTNode::NODE_STATE ShiftAndIdle(); // 걸을 것인지 분기처리
    virtual CBTNode::NODE_STATE Shift(); // 쉬프트(걷기)
    virtual CBTNode::NODE_STATE Idle(); // 정좌

protected:
    virtual void Ready_Tree();
    _float DistanceFromPlayer();
    _bool isBehine();
    void LookAtPlayer();

protected:
    _uint Check_KRH(_uint iPlayerLv, _bool isBehine, _bool isAnimChange = true);
    _uint Check_KRS(_uint iPlayerLv, _bool isBehine, _bool isAnimChange = true);
    _uint Check_KRC(_uint iPlayerLv, _bool isBehine, _bool isAnimChange = true);
    _bool Check_StandUp();

protected:
    void Reset_State() {
        m_isAttack = false;
        m_isBreak = false;
        m_isSway = false;
        m_isGuard = false;
        m_isRun = false;

        m_fBreakTime = 0.f;
        m_fAttackDelayTime = 0.f;
    }

public:
    virtual void Free();
};

END