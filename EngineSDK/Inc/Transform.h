#pragma once
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CTransform final:
    public CComponent
{
public:
    enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

    typedef struct
    {
        _float fSpeedPecSec;
        _float fRotatePecSec;
    }TRANSFORM_DESC;


private:
    const _float MAX_DISTANCE = 256.f;

private:
    CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CTransform(const CTransform& rhs);
    virtual ~CTransform() = default;

public:
    _vector Get_State(STATE eState);
    _float3 Get_Scaled() {
        return _float3(XMVector3Length(Get_State(STATE_RIGHT)).m128_f32[0],
            XMVector3Length(Get_State(STATE_UP)).m128_f32[0],
            XMVector3Length(Get_State(STATE_LOOK)).m128_f32[0]);
    }

    const _float4x4* Get_WorldFloat4x4() { return &m_WorldMatrix; }
    _matrix Get_WorldMatrix() { return XMLoadFloat4x4(&m_WorldMatrix); }
    _float Get_Speed() { return m_fSpeedPerSec; }

public:
    void Set_WorldMatrix(_fmatrix WorldMatrix) { XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix); }
    void Set_OldWorldMatrix(_fmatrix OldWorldMatrix) { XMStoreFloat4x4(&m_WorldMatrix, OldWorldMatrix); }
    void Set_State(STATE eState, _fvector vState);

    void Scaling(_float fScaleX, _float fScaleY, _float fScaleZ);
    void Set_Scale(_float fScaleX, _float fScaleY, _float fScaleZ);
    void Set_Speed(_float fSpeed) { m_fSpeedPerSec = fSpeed; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

public:
    HRESULT Bind_ShaderMatrix(class CShader* pShader, const _char* strConstantName);
    HRESULT Bind_ShaderOldMatrix(class CShader* pShader, const _char* strConstantName);

public:
    void Go_Straight(const _float& fTimeDelta);
    void Go_Straight_CustumSpeed(const _float& fSpeed, const _float& fTimeDelta, class CNavigation* pNavi = nullptr);
    void Go_Move_Custum(const _float4& vDir, const _float& fSpeed, const _float& fTimeDelta, class CNavigation* pNavi = nullptr);
    void Go_Backward(const _float& fTimeDelta);
    void Go_Left(const _float& fTimeDelta);
    void Go_Right(const _float& fTimeDelta);
    void Go_Up(const _float& fTimeDelta);          //0625 혜원 추가 
    void Go_Down(const _float& fTimeDelta);        //0625 혜원 추가
    void LookAt(_fvector vTargetPosition, _bool isDir = false);
    void LookAt_For_LandObject(_fvector vTargetPosition, _bool isDir = false);   // 0630 혜원 추가, 땅 위에 있는 오브젝트들을 LookAt시킴 (y축으로만 회전함)
    void LookForCamera(_fvector vCamLook, _float fRadian);
    void Turn(_fvector vAxis, _float fTimeDelta);
    void Rotation(_fvector vAxis, _float fRadian); // 방향 초기화 후 회전
    void Change_Rotation(_fvector vAxis, _float fRadian); // 오브젝트가 바라보는 방향을 기준으로 회전
    _vector Get_Rotation(_fvector vAxis, _float fRadian); // 오브젝트가 바라보는 방향을 기준으로 회전값을 반환
    void Change_Rotation_Quaternion(const _float4& vQuaternion); // 오브젝트가 바라보는 방향을 기준으로 회전

private:
    _float4x4 m_WorldMatrix;
    _float4x4 m_OldWorldMatrix;
    _float m_fSpeedPerSec = { 0 };
    _float m_fRotationPerSec = { 0 };

public:
    static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;
};

END