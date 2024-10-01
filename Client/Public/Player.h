#pragma once
#include "LandObject.h"
#include "Navigation.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CNavigation;
class CAnim;
END

BEGIN(Client)
class CPlayer :
    public CLandObject
{
public:
    const _float PLAYER_HITGAUGE_LEVEL_INTERVAL = 50.f;

#pragma region 이넘값들
public:
    //KRS: 불한당, KRH: 러쉬, KRC: 파괴자
    enum BATTLE_STYLE
    {
        ADVENTURE, KRS, KRH, KRC, BATTLE_STYLE_END
    };

    /* 06.28 혜원 추가
    *  스타일마다 겹치는 행동이 있어서 enum class 사용 
    *  행동은 임시로 일부만 잡아둔 상태로 지속적으로 추가될 예정임
    */
    enum class ADVENTURE_BEHAVIOR_STATE
    {
        IDLE, WALK, RUN, ADVENTURE_BEHAVIOR_END
    };    

    // 슈아: SKILL_FLY_KICK
    enum class KRS_BEHAVIOR_STATE
    {
        BTL_START, IDLE, WALK, RUN, ATTACK, HIT, 
        SWAY, DOWN, GRAB, PICK_UP,
        SKILL_FLY_KICK, SKILL_KICK_COMBO,
        KRS_BEHAVIOR_END
    };

    // 슈아: -
    enum class KRH_BEHAVIOR_STATE
    {
        BTL_START, IDLE, WALK, RUN, ATTACK, HIT,
        SWAY, DOWN, 
        KRH_BEHAVIOR_END
    };

    // 슈아: GUARD
    enum class KRC_BEHAVIOR_STATE
    {
        BTL_START, IDLE, WALK, RUN, ATTACK, HIT,
        SWAY, DOWN, GRAB, PICK_UP,
        GUARD,
        KRC_BEHAVIOR_END
    };

    enum MOVE_DIRECTION
    {
        F, B, L, R, MOVE_DIRECTION_END
    };

    enum CUTSCENE_ANIMATION_TYPE
    {
        /* 불한당 히트액션 */
        FINISHBLOW,                 //h23320 피니시 블로의 극
        GOUGEKI_C,                  //h20021 잡기 후 히트액션 (주먹으로 얼굴때리고 박치기)
        HAIHEKI_KICK,               //h23070 벽 등지고 무릎치는 스킬

        /* 러쉬 */
        KOBUSHIKUDAKI,              //h10111 팔꿈치로 공격막는 스킬
        HAIHEKI_PUNCH,              //h23060 벽 등지고 나오는 스킬
        OI_TRAMPLE_AO,              //h1500 다운된 상대 얼굴 밟기 (누워있는 상태), 러쉬
        OI_KICKOVER_UTU_C,          //h1511 다운된 상대 얼굴 차기 (엎드려있는 상태), 러쉬

        /* 파괴자 */
        KIRYU_GSWING,               //h1010 다리잡고 돌려서 스플릿 공격
        DORYU_MIN,                  //h11285 멱살잡고 돌려서 스플릿 공격
        LAPEL_OIUCHI_NECK,          //h1530 들어서 바닥에 내던짐(앞잡)
        NAGE_OIUCHI_NECK,           //h1540 들어서 바닥에 내던짐(뒤잡)
        POLE_KNOCK_LAPEL,           //h2040 근처에 기둥이 있다면 기둥에 박게하고 밟음
        DORAMUKAN_88,               //h3261 큰 무기 (간판)을 들고 벽에 밀고 내려침
        MONZETSU,                   //h11250 들어다가 무릎으로 똥꼬찍음 (뒤에서 잡기했을때 사용)

        /* 기본 히트액션 */
        WALL_KNOCK_NECK_C,          //h2011 벽에 머리박게하고 밟음 (아마 잡기 이후 히트액션했을 때 근처에 벽이있다면)
        KABE_AIRON,                 //h23000 벽으로 밀치고 때림
        OI_KICK,                    //h23010 머리채 잡고 들어서 발로참 (엎드린 상태)
        OI_UPPER,                   //h23020 머리채잡고 들어서 주먹으로 침 (누워있는 상태)

        /* 요네다 컷신 */
        YONEDA_H,                   //A60300 요네다 등장
        YONEDA_DOWN_ATTACK,         //A60330 요네다 복도
        YONEDA_DOSU,                //a60350 요네다 화장실

        KUZE_RUSH,                  //h23250 요네다 화장실

        CUTSCENE_ANIMATION_END
    };

    // 표정 애니메이션
    // [0] f201_special_01[f_kiryu]             // 총격전에서 로켓 격파하는 컷신에서 쓰임
    // [1] f202_special_02[f_kiryu]             // 인상씀(화난)
    // [2] f203_special_03[f_kiryu]             // 인상씀(뭐야저건 이런표정) 황당해하는듯
    // [3] f204_special_04[f_kiryu]             // 인상쓰고있는데 오묘하게 웃음
    // [4] f205_special_05[f_kiryu]             // 뭔지모르겠는 오묘한 표정
    // [5] f206_special_06[f_kiryu]             // 미심쩍게 웃음
    // [6] f207_special_07[f_kiryu]             // 아파할 때 쓰일듯?
    // [7] f208_special_08[f_kiryu]             // 뭔가 안쓰럽게보는거같아보임
    // [8] f209_special_09[f_kiryu]             // 크게 화낼 때
    // [9] f210_special_10[f_kiryu]             // 눈 부릅뜨는
    // [10] f211_special_11[f_kiryu]            // 인상쓰고 실눈뜨고봄
    // [11] f212_special_12[f_kiryu]            // 눈 크게뜸 (놀란듯)

    struct PLAYER_INFO {
        _uint           iCurrentHitLevel = { 0 };
        _float          fHitGauge = { 0.f };
        _uint           iMoney = { 0 };
        LAND_OBJ_INFO   Info;
    };
    static PLAYER_INFO PlayerInfo;

#pragma endregion

private:
    const _float ANIM_INTERVAL = 4.f;
    
private:
    CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CPlayer(const CPlayer& rhs);
    virtual ~CPlayer() = default;
    
#ifdef _DEBUG


public:
    void Set_AnimStart(_bool isAnim) { m_isAnimStart = isAnim; }

    _bool isAnimStart() { return m_isAnimStart; }


#endif // DEBUG
public:
    _bool CanHitAction() { return m_CanHitAction; }

    const map<CUTSCENE_ANIMATION_TYPE, string>& Get_CutSceneAnims() const
    {
        return m_CutSceneAnimation;
    }

    void Set_CutSceneIndex(CUTSCENE_ANIMATION_TYPE eAnim)
    {
        m_eCutSceneType = eAnim;
    }

    void Set_PlayerStop(_bool isStop) { m_isStop = isStop; }


    void Set_TargetObject(class CGameObject* pTargetObject) {
        m_pTargetObject = reinterpret_cast<CMonster*>(m_pTargetObject);
    }


    /* Getter */
public:
    _uint Get_BattleStyle() { return m_eCurrentStyle; }

    _bool isAttack() { return m_iCurrentBehavior == static_cast<_uint>(KRS_BEHAVIOR_STATE::ATTACK); }
    _bool isDown() { return m_iCurrentBehavior == static_cast<_uint>(KRS_BEHAVIOR_STATE::DOWN); }
    _bool isGrab() { return (m_eCurrentStyle == KRS || m_eCurrentStyle == KRC) &&
        m_iCurrentBehavior == static_cast<_uint>(KRS_BEHAVIOR_STATE::GRAB); }
    _uint Get_CurrentHitLevel() { return CPlayer::PlayerInfo.iCurrentHitLevel; }
    _float Get_HitGauage() {  return CPlayer::PlayerInfo.fHitGauge;}

    const _bool* Get_MoveDirection() {
        return m_MoveDirection;
    }

    class CLandObject* Get_TargetObject() {
        return reinterpret_cast<CLandObject*>(m_pTargetObject);
    }

    CCollider* Get_PlayerCollider() {
        return m_pColliderCom;
    }

    class CItem* Get_CurrentItem() {
        return m_pTargetItem;
    }

    _uint Get_CurrentAnimType() {
        return m_eAnimComType;
    }

    /* Setter */
public:
    //void    Set_StartPos(XMMATRIX    vStartPos) { m_pTransformCom->Set_WorldMatrix(vStartPos); }
    //void    Set_NavigationIndex(int iIndex) { m_pNavigationCom->Set_Index(iIndex); }
    //void    Set_NaviRouteIndex(int iIndex) { m_iNaviRouteNum = iIndex; }
    void    Set_SeizeOff(_bool isOff = true);
    void    Set_ItemOff();
    void    Set_CurrentStyle(BATTLE_STYLE iStyle) { m_eCurrentStyle = iStyle; }
    void    Set_CurrentBehavior(_uint iStyle) { m_iCurrentBehavior = iStyle; }

    void Battle_Start();

    void Play_Kuze_QTE(class CMonster* pMonster);


    /* Virtual Funtion */
public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

    virtual void Sound_Event() override;

    // 충돌함수
    virtual void Attack_Event(CGameObject* pHitObject, _bool isItem = false) override;
    virtual void Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack = false) override;

    virtual string Get_CurrentAnimationName() override;

    /* Initial Function */
private:
    void Ready_AnimationTree();
    void Ready_CutSceneAnimation();
    void Ready_AuraEffect();
    void Ready_RootFalseAnimation();

    /* Compute Function */
private:
    void Synchronize_Root(const _float& fTimeDelta);
    // 각 객체의 Look끼리연산해서 마주보고있는지, 옆에있는지 등을 판단
    // 상대 객체가 나를 바라보고 있음을 기준으로, 좌/우/앞/뒤 어디서 나를 보는지를 판단하는 함수
    _int Compute_Target_Direction(CLandObject* pAttackedObject);                

    // 상대 객체의 포지션을 기준으로 계산함
    // 내 위치를 기준으로 좌/우/앞/뒤 어디에 존재하는지를 판단
    _int Compute_Target_Direction_Pos(_fvector vTargetPos);


    //키 입력관련함수들
private:
    void KeyInput(const _float& fTimeDelta);       //현재 플레이어의 상태에 따른 키입력처리 하는 함수

    // 어드벤처 모드에서의 키입력
    void Adventure_KeyInput(const _float& fTimeDelta);      
    // 불한당 모드 키입력
    void KRS_KeyInput(const _float& fTimeDelta);  
    // 러쉬 모드 키입력
    void KRH_KeyInput(const _float& fTimeDelta);      
    // 파괴자 모드 키입력
    void KRC_KeyInput(const _float& fTimeDelta);

    /* 애니메이션 관련 함수들 */
public:
    void Change_Animation(_uint iAnimIndex, _float fInterval = 4.f);
    void Change_ResetAnimaition(_uint iAnimIndex, _float fInterval = 4.f);
    void Style_Change(BATTLE_STYLE eStyle);
    void Reset_MoveDirection();

    void Set_CutSceneStartMotion(CUTSCENE_ANIMATION_TYPE eType);
    void Set_CutSceneAnim(CUTSCENE_ANIMATION_TYPE eType);
    void Play_CutScene();
    void Reset_CutSceneEvent();

    void HitAction_Down();
    void HitAction_WallBack();
    void HitAction_CounterElbow();

    void Update_RootFalseAnim();

private:
    void Compute_MoveDirection_FB();
    void Compute_MoveDirection_RL();

    void Effect_Control_Aura();
    void On_Aura(BATTLE_STYLE eStyle);
    void Off_Aura(BATTLE_STYLE eStyle);

    void Setting_Target_Enemy();
    void Setting_Target_Item();
    void Setting_Target_Wall();

    void Radial_Event();
    void Face_Event();


    /* 캐릭터 스테이터스 관련 함수 */ 
public:
    void AccHitGauge();

    /* 쉐이더 바인딩 관련 함수 */
public:
    void Setting_RimLight();
    HRESULT Bind_RimLight();

    /* 기타 기능 */
public:
    void HitFreeze_On();
    void HitFreeze_Off();
    void HitFreeze_Timer(const _float& fTimeDelta);

    void HitRadial_On();
    void HitRadial_Off();
    void HitRadial_Timer(const _float& fTimeDelta);

    void HitZoomIn_On();
    void HitZoomIn_Off();
    void HitZoomIn_Timer(const _float& fTimeDelta);

    
    /* 출력, 행동 관련 포인터 변수들 */
private:
    CAnim* m_pAnimCom = { nullptr };                // 컷신 돌릴 때 사용하는 애니메이션 컴포넌트
    CModel* m_pCameraModel = { nullptr };           // 카메라 돌릴 때 쓰는 모델 (랜더하지않음)

    // 이 때, 사용하는 키 값은 행동에 대한 키값을 가진다. (스타일은 배열 인덱스)
    map<_uint, class CBehaviorAnimation*> m_AnimationTree[BATTLE_STYLE_END];
    //ui
    class CUIManager* m_pUIManager = { nullptr };
    
#ifdef _DEBUG
    class CDebugManager* m_pDebugManager = { nullptr };
#endif // _DEBUG

    /* 플레이어 기능 관련 포인터 변수들 */
private:
    class CMonster* m_pTargetObject = { nullptr };
    class CItem* m_pTargetItem = { nullptr };
    CCollider* m_pTargetWall = { nullptr };

    /* 행동, 이동 관련 변수들 */
private:
    BATTLE_STYLE    m_eCurrentStyle = { ADVENTURE };
    // 스타일마다 겹치는 행동이 있을 수 있어서 int값으로 저장하고 형변환하여 저장한다.
    _uint           m_iCurrentBehavior = static_cast<_uint>(KRS_BEHAVIOR_STATE::BTL_START);

    // 스웨이용 키입력 디렉션 (카메라 기준으로 처리하기 위함)
    _bool                       m_InputDirection[MOVE_DIRECTION_END];
    _bool                       m_MoveDirection[MOVE_DIRECTION_END];

    _float                      m_fPrevSpeed = { 0.f };
    _float4                     m_vPrevMove;
    _float4                     m_vPrevRotation;
    _float4x4                   m_ModelWorldMatrix;

    _bool                       m_isAuraOn = { false };

    /* 애니메이션 관련 */
private:
    ANIMATION_COMPONENT_TYPE    m_eAnimComType = { DEFAULT };
    _bool                       m_isCutSceneStartMotion = { false };
    map<CUTSCENE_ANIMATION_TYPE, string> m_CutSceneAnimation;
    vector<_uint>               m_RootFalseAnims;

    _uint                       m_iCutSceneAnimIndex = { 0 };
    _uint                       m_iCutSceneCamAnimIndex = { 0 };

    _uint                       m_iDefaultAnimIndex = { 0 };

    _bool                       m_CanHitAction = { false };

    int                         m_iNaviRouteNum = { 0 }; //루트

    _bool                       m_isRootAnim = { true };


    /* 플레이어 오라 이펙트 모아두기 */
private:
    vector<CSocketEffect*> m_HooliganAura;
    vector<CSocketEffect*> m_RushAura;
    vector<CSocketEffect*> m_DestroyerAura;

private:
    _bool m_isHitFreeze = { false };
    const _float m_fHitFreezeTime = { 0.5f };
    _float m_fHitFreezeTimer = { 0.f };

    _bool m_isHitRadial = { false };
    const _float m_fHitRadialTime = { 0.2f };
    _float m_fHitRadialTimer = { 0.f };

    _bool m_isHitZoom = { false };
    const _float m_fHitZoomTime = { 0.2f };
    _float m_fHitZoomTimer = { 0.f };

#ifdef _DEBUG
    _bool m_isAnimStart = { true };

#endif // _DEBUG
    CUTSCENE_ANIMATION_TYPE m_eCutSceneType = { OI_UPPER };

    class CQteManager* m_pQTEMgr = { nullptr };

    public:
        void Set_Money(_uint iMoney) { m_Money += iMoney; }
        _uint Get_Money() { return m_Money; }
private:
    _uint m_Money = { 1000 };
    _bool m_isStop = { 0 };

private:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;
    virtual void Compute_Height() override;

public:
    static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};
END
