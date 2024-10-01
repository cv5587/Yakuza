#pragma once
#include "LandObject.h"

BEGIN(Engine)
class CAnim;
class CNavigation;
END

BEGIN(Client)
class CCarChase_Reactor abstract :
    public CLandObject
{
public:
    typedef struct tMapHighwayObjDesc : public CGameObject::GAMEOBJECT_DESC
    {
        XMMATRIX		vStartPos;
        wstring			wstrModelName;
        _int				iShaderPass;
        _int             iNaviNum;
        _int             iNaviRouteNum;
        _uint iStageDir;
        _uint iLineDir;
        _int iWaypointIndex;
        
        _int iMonsterWeaponType[2];
    }HIGHWAY_IODESC;

protected:
    CCarChase_Reactor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCarChase_Reactor(const CCarChase_Reactor& rhs);
    virtual ~CCarChase_Reactor() = default;

public:
    virtual string Get_CurrentAnimationName();
    _bool isStart() { return m_isStart; }
    virtual const vector<class CCarChase_Monster*>& const Get_Monsters() {
        return m_Monsters;
    }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

protected:
    _bool m_isAnimLoop = { false };
    _bool m_isStop = { false };
    _bool m_isStart = { true };
    _bool   m_isColl = { false };
    _bool m_isFinishEffect = { false };

    _uint m_iState = { 0 };
    _float m_fChangeInterval = { 4.f };

    string  m_strAnimName = "";
    _int   m_iAnim = { 0 };

    
protected:
    vector<class CCarChase_Monster*> m_Monsters;
    _uint m_iStageDir = { DIR_END };
    _uint m_iLineDir = { DIR_END };

    _int     m_iNaviRouteNum = { 0 };
    _int m_iWaypointIndex = { -1 };
    _float m_fSpeed = { 35.f };
    _float m_fMaxSpeed = { 50.f };
    _float m_fMinSpeed = { 35.5f };

    _bool m_isSound = { false };
    _float m_fSound = { 1.f };

protected:
    virtual HRESULT Ready_Monster(_int* pMonsterTypes) = 0;
    virtual void Change_Animation();
    virtual _bool Check_Dead();

protected:
    void Move_Waypoint(const _float& fTimeDelta);

protected:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;

public:
    virtual void Free();
};
END
