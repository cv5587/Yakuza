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
class CKaraoke_Kiryu :
    public CLandObject
{
public:
    enum MOUTH_ANIM
    {
        CLOSE, OPEN, MOUTH_ANIM_END
    };

    enum SINGING_ANIMATION
    {
        CLAP, PASSIONATE_SINGING, IDLE, SINGING, DANCING, SINGING_ANIMATION_END
    };

    enum RH_OBJ_TYPE
    {
        MIC, GLASS, RH_OBJ_TYPE_END
    };

    struct PLAY_TIME_EVENT
    {
        _float fTime;
        _int iEventType;               // -1: 입애님, 나머지는 애님인덱스
        _bool isChanged = { false };
    };

private:
    const _float ANIM_INTERVAL = 4.f;
    
private:
    CKaraoke_Kiryu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CKaraoke_Kiryu(const CKaraoke_Kiryu& rhs);
    virtual ~CKaraoke_Kiryu() = default;
    
    /* Virtual Funtion */
public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

    // CLandObject을(를) 통해 상속됨
    string Get_CurrentAnimationName() override;

    virtual _bool Checked_Animation_Ratio(_float fRatio) override;

public:
    void Change_SingingAnimaition(SINGING_ANIMATION eType);
    void Set_CutSceneAnim();
    void Play_CutScene(const _float& fTimeDelta);
    void Reset_CutSceneEvent();
    void PlayTime_Event();

private:
    void Ready_SingingInterval();
    void Ready_PlayTimeEvent();

    void SingOff() {
        m_isSinging = false;
        m_iFaceAnimIndex = CLOSE;
    }
    void Play_SingingAnim(const _float& fTimeDelta);

    void Change_MouthAnim();
    
    /* 출력, 행동 관련 포인터 변수들 */
private:
    CAnim* m_pAnimCom = { nullptr };                // 컷신 돌릴 때 사용하는 애니메이션 컴포넌트
    CModel* m_pCameraModel = { nullptr };           // 카메라 돌릴 때 쓰는 모델 (랜더하지않음)

    //ui
    class CUIManager* m_pUIManager = { nullptr };

    vector<class CSocketObject*> m_pRightHand;
    
#ifdef _DEBUG
    class CDebugManager* m_pDebugManager = { nullptr };
#endif // _DEBUG


    /* 애니메이션 관련 */
private:
    ANIMATION_COMPONENT_TYPE    m_eAnimComType = { DEFAULT };
    RH_OBJ_TYPE                 m_eRHType = { MIC };

    _uint                       m_iCutSceneAnimIndex = { 0 };
    _uint                       m_iCutSceneCamAnimIndex = { 0 };

    _uint                       m_iDefaultAnimIndex = { 0 };

    _float                      m_fPrevSpeed = { 0.f };
    _float4                     m_vPrevMove;
    _float4                     m_vPrevRotation;
    _float4x4                   m_ModelWorldMatrix;

    list<_float>                m_fMouthChangeInterval;

    _bool                       m_isSinging = { false };
    _float                      m_fMouthTimer = { 0.f };

    SINGING_ANIMATION           m_eSingingAnimType = { IDLE };
    vector<PLAY_TIME_EVENT>     m_PlayTimeEvent;

private:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;
    virtual void Compute_Height() override;
    HRESULT Add_Objects();

public:
    static CKaraoke_Kiryu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;


};
END
