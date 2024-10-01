#pragma once
#include "Component.h"

#include "Cell.h"

BEGIN(Engine)
class ENGINE_DLL CNavigation :
    public CComponent
{
public:
    enum WAYPOINT_OPTION { DEFAULT, CORNEL, OPTION_END };

    typedef struct tNavigationDesc {
        _uint iCurrentRouteDir = { DIR_END }; // 걸어가는 방향 (정방향인지, 역방향인지)
        _int iCurrentLine = { -1 };
        _int iWayPointIndex = { -1 };
        _vector vPosition;
    }NAVIGATION_DESC;

private:
    CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CNavigation(const CNavigation& rhs);
    virtual ~CNavigation() = default;

public:
    void Set_Points(const _float3* vPoints, _int OptionType);
    void Set_Index(_uint iIndex) { m_iCurrentIndex = iIndex; }
    //플레이어 루트 이동 시 
    //웨이포인트도 멀리 이동한다.
    void Set_NavigationRouteIndex(_uint iIndex) { 
        m_iCurrentRouteIndex = iIndex;
        m_iCurrentWayPointIndex += 2;
    }

public:
    CCell::OPTION Get_OptionType() { return m_Cells[m_iCurrentIndex]->Get_Option(); }
    _uint Get_Index() {
        return m_iCurrentIndex;
    }
    vector<CCell*> Get_Cells() { return m_Cells; }
    vector<ROUTE_IO> Get_RouteIndexs(_int iIndex);
    _uint Get_WaypointIndex() { return m_iCurrentWayPointIndex; }
    _vector Get_CurrentWaypointPos() { return XMLoadFloat4(&m_Routes[m_iCurrentRouteIndex][m_iCurrentWayPointIndex].vPosition); }
    _vector Get_WaypointPos(_uint iIndex) { return XMLoadFloat4(&m_Routes[m_iCurrentRouteIndex][iIndex].vPosition); }
    _vector Get_SlidingNormal() { return m_vSlidingNormal; }
    _uint Get_RouteSize() { return m_Routes[m_iCurrentRouteIndex].size(); }
    _int Get_RouteDir() { return m_iRouteDir; }

public:
    virtual HRESULT Initialize_Prototype(); // Tool용
    virtual HRESULT Initialize_Prototype(const wstring strFilePath);
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Tick() override;

public:
    int Find_PlayerMonster_Index(_fvector vTargetPos);
    _bool isMove(_fvector vMovePos);
    _vector Compute_WayPointDir(_vector vPosition, const _float& fTimeDelta, _bool isStart = false);
    _vector Compute_WayPointDir_Adv(_vector vPosition, const _float& fTimeDelta, _bool* isTurn, _int* iDir, _bool* isBack);
    _vector Compute_NishikiDir(_fvector vPosition, const _float& fTimeDelta, _bool* isFinished);
    _float Compute_Height(_fvector vPosition);


#ifdef _DEBUG
public:
    HRESULT Render();
    _bool isRemove(_int iIndex);
#endif // _DEBUG

private:
    class CShader* m_pShaderCom = { nullptr };
    vector<CCell*> m_Cells; // 삼각형들의 집합 저장
    map<_int, vector<ROUTE_IO>>					m_Routes;
    _uint m_iRouteDir = { DIR_END };

    _uint m_iIndexCount = { 0 };

    _float4x4 m_WorldMatrix = {};

private:
    _int m_iCurrentIndex = { -1 };
    _vector     m_vSlidingNormal;

    //총격전용
private:
    _uint m_iCurrentRouteIndex = { 0 }; //루트 인덱스
    _uint m_iPreRouteIndex = { 0 }; //루트 인덱스
    _int m_iCurrentWayPointIndex = { 0 };
    _int m_iPreWayPointIndex = { 0 };
    _float m_fMaxDistance = { 5.f };

    _vector m_vPreDir = {};
    _vector m_vNextDir = {};

    _float m_fTime = {0.f};


    _vector m_vTargetPoint;
    _float  m_fMoveDistance = { 0.f };
    _float  m_fTargetDelta = { 0 };


    _vector m_vPrePosition; // 루트 찾기 전
    _vector m_vPreWayPointPosition;

private:
    bool        m_bOrigin = { false };


private:
    HRESULT Load_File(const wstring strFilePath);
    HRESULT SetUp_Neighbors();
    void Find_WayPointIndex(_vector vPosition);
    void Swap_Route(vector<ROUTE_IO> CurrentRoute, _vector vCurrnetDir, _bool* isTurn, _int* iDir);


public:
    static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring strFilePath);
    static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext); // Tool용
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

END