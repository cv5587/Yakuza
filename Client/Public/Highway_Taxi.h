#pragma once
#include "LandObject.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)
class CHighway_Taxi :
    public CLandObject
{
public:
    typedef struct tHighWayTaxiDesc : public CLandObject::GAMEOBJECT_DESC {
        XMMATRIX		vStartPos;
        _int             iNaviNum;
        _int             iNaviRouteNum;
    } HIGHWAY_TEXI_DESC;

private:
    CHighway_Taxi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CHighway_Taxi(const CHighway_Taxi& rhs);
    virtual ~CHighway_Taxi() = default;

public:
    _uint Get_CurrentWaypointIndex();
    _uint Get_WaypointSize(); // 플레이어가 있는 위치의 웨이포인트 갯수
    const _float4x4* Get_KiryuBoneMatrix(const _char* pBoneName);

public:
    void Set_NavigationRouteIndex(_uint iLine); // 네비게이션 Route 변경
    void Set_Dir(_uint iStageDir);
    void Sit_Swap();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    // CLandObject을(를) 통해 상속됨
    string  Get_CurrentAnimationName() override;
    void    Set_StartPos(XMMATRIX    vStartPos) { m_pTransformCom->Set_WorldMatrix(vStartPos); }
    void    Set_NavigationIndex(int iIndex);
    void    Set_NaviRouteIndex(int iIndex) { m_iNaviRouteNum = iIndex; }

    void    OnHit(_float fDamage);

    void    Set_BoosterSpeed(_float fSpeed) { m_fBooster = fSpeed; }


protected:
    class CHighway_Kiryu* m_pKiryu = { nullptr };
    _int        m_iNaviRouteNum = { 0 }; // 몬스터가 이동하는 네비게이션 위치
    _uint       m_iStageDir = { DIR_END }; // 스테이지 위치 (앞, 뒤, 왼, 오)
    _float m_fSpeed = { 35.f };
    _bool m_isStop = { false };

    //ui 연동용으로 제작
    _float m_fCarHp = { 400.f };
    _float m_fMaxCarHp = { 400.f };

    _float m_fBooster = { 1.f };

protected:
    virtual void Change_Animation();
    void Move_Waypoint(const _float& fTimeDelta);

protected:
    virtual HRESULT Add_Components() override;
    HRESULT Add_Objects();
    virtual HRESULT Bind_ResourceData() override;
public:
    _float Get_CarHp() { return m_fCarHp; }
    _float Get_CarMaxHp() { return m_fMaxCarHp; }
    class CHighway_Kiryu* Get_Kiryu() { return m_pKiryu; }

public:
    static CHighway_Taxi* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
