#pragma once
#include "LandObject.h"

BEGIN(Engine)
class CAnim;
END

BEGIN(Client)
class CHighway_Kiryu :
    public CLandObject
{
public:
    struct CARCHASE_KIRYU_DESC
    {
        _float4x4* pTaxiWorldMatrix;
    };

    enum BEHAVIOR_TYPE
    {
        AIMING, HIDE, HIT, SHOT, SWAP,
    };

    /*
    * 0~34 : 좌측 관련 애니메이션
    * 35~70: 우측 관련 애니메이션
    * 71~73: 좌측 앉기 관련 애니메이션 (앉기 관련은 _en 붙은게 일어서는 애니메이션)
    * 74~76: 우측 앉기 관련 애니메이션
    * 77~78: 자리 이동 (좌, 우)
    */

private:
    const _uint MAX_AMMO = 15;
    const _float HITEYE_DECREASE_SPEED = 20.f;
    const _float RELOAD_TIME = 2.f;

private:
    CHighway_Kiryu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CHighway_Kiryu(const CHighway_Kiryu& rhs);
    virtual ~CHighway_Kiryu() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    void OnHit(_float fDamage);

public:
    void Set_StageDir(_uint iStageDir) {
        m_iStageDir = iStageDir;
    }

public:
    // CLandObject을(를) 통해 상속됨
    string Get_CurrentAnimationName() override;
    const _float4x4* Get_BoneMatrix(const char* strBoneName);
    const _float4x4* Get_ModelMatrix() { return &m_ModelMatrix; }

    const _float3* Get_Pos();
    const _float3* Get_Look();
    const _float3* Get_Right();

private:
    virtual void Change_Animation();
    
    void Key_Input();

    /* 애니메이션 */
private:
    void Play_CurrentAnimation(_float fTimeDelta);

    void Play_Animing(_float fTimeDelta);
    void Play_HideReload(_float fTimeDelta);
    void Play_Hit(_float fTimeDelta);
    void Play_Shot(_float fTimeDelta);
    void Play_Swap(_float fTimeDelta);

public:
    void Change_Behavior(BEHAVIOR_TYPE eType);

private:
    _bool isAttackPossible();

private:
    virtual HRESULT Add_Components() override;
    HRESULT Add_Objects();
    virtual HRESULT Bind_ResourceData() override;

private:
    const _float4x4* pTaxiMatrix = { nullptr };
    _float4x4 m_ModelMatrix;

    class CGun_Cz75* m_pGun_R = { nullptr };
    class CGun_Cz75* m_pGun_L = { nullptr };

    _float m_fHP = { 200.f };
    _float m_fHitEye = { 100.f };

    _float m_fMaxHP = { 200.f };
    _float m_fMaxHitEye = { 100.f };

    _uint m_iCurrentAmmo = { MAX_AMMO };

    _bool           m_isLeft = { false };           // 시작 시 오른쪽부터 시작한다
    BEHAVIOR_TYPE   m_eCurrentBehavior = { AIMING };
    _uint           m_iStageDir = { DIR_END };
    _bool           m_isStarted = { false };
    _bool           m_isHitEyeCharging = { true };

    _float m_fAccReloadTimer = { 0.f };


    _bool m_isShot = { false };
public:
    _float Get_MaxHP() { return m_fMaxHP; }
    _float Get_HP() { return m_fHP; }
    _float Get_MaxHitEye() { return m_fMaxHitEye; }
    _float Get_HitEye() { return m_fHitEye; }
   const _uint Get_MaxAmmo() { return MAX_AMMO; }
    _uint Get_Ammo() { return m_iCurrentAmmo; }
    _uint Get_CurrentBehavior() { return m_eCurrentBehavior; }
    CCollider* Get_KiryuCollier() { return m_pColliderCom; }

    //아래는 ui에서 확인한 코드 지워도됨
    _bool Shot();
    void Damage() { m_fHP -= 2.f; }

public:
    static CHighway_Kiryu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();


};
END
