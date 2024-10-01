#pragma once
#include "GameObject.h"

BEGIN(Engine)
class ENGINE_DLL CCamera abstract:
    public CGameObject
{
public:
    typedef struct tCameraDesc : public GAMEOBJECT_DESC
    {
        _float4 vEye, vFocus;
        _float fFovY, fAspect, fNear, fFar;

    }CAMERA_DESC;

public:
    _bool isShaking() { return m_isShaking; }

public:
    void Set_Shaking(_bool isShaking, _float3 vShakeDir = { 1.f, 1.f, 0.f}, _float fShakeDuration = 0.3f, _float fShakeMagnitude = 0.3f)
    { 
        m_isShaking = isShaking; 
        m_fShakeDuration = fShakeDuration;
        m_fShakeMagnitude = fShakeMagnitude;
        XMStoreFloat3(&m_vShakeDir, XMVector3Normalize(XMLoadFloat3(&vShakeDir)));
    }

protected:
    CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCamera(const CCamera& rhs);
    virtual ~CCamera() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta, _bool isAuto = true);
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    void Start_Zoom() {
        m_isZooming = true;
    };

protected:
    virtual void Turn(_float4x4* OrbitMatrix, _fvector vAxis, const _float& fTimeDelta);
    virtual void Rotation(_float4x4* OrbitMatrix, _fvector vAxis, _float fRadian);
    virtual void Zoom(const _float& fTimeDelta);
    virtual void LookAt(_float4x4* OrbitMatrix, _fvector vTargetPos, _fvector vPosition);

protected:
    void Shaking(_float fTimeDelta);
    void Reset_ZoomVariables();

public:
    const _float4x4* Get_WorldMatrix() const
    {
        return &m_WorldMatrix;
    }

    _float Get_Fov()
    {
        return m_fFovY;
    }

    void Set_FoV(_float fFov) {
        m_fFovY = fFov;
    }

    void Set_TargetFoV(_float fFov) {
        m_fTargetFovY = fFov;
    }

    void Set_CustomRatio(_float fCustomRatio) {
        m_fCustomLerpRatio = fCustomRatio;
    }

    void Set_WorldMatrix(_fmatrix WorldMatrix)
    {
        XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
    }

protected:
    _float m_fDefaultFovY;
    _float m_fDefaultNear;
    _float m_fDefaultFar;

    _float m_fFovY;
    _float m_fAspect;
    _float m_fNear;
    _float m_fFar;

    // Fov 변형 관련
    _float       m_fTargetFovY;
    _float       m_fLerpRatio = { 0.f };    // 보간 비율
    _float       m_fCustomLerpRatio = { -1.f };    // 사용자 정의 보간 비율
    _float       m_fElapsedTime = 0.0f; // 경과 시간
    _float       m_fTotalLerpTime = 0.5f; // 보간에 걸리는 총 시간 (초 단위)

    _float4x4 m_WorldMatrix;

protected:
    _bool m_isShaking = { false };
    _bool m_isZooming = { false };

    _float m_fShakeTime = { 0.f };
    _float m_fShakeDuration = {}; //지속시간
    _float m_fShakeMagnitude = {}; //강도
    _float3 m_vShakeDir = {}; //방향

public:
    virtual CGameObject* Clone(void* pArg) = 0;
    virtual void Free() override;
};

END