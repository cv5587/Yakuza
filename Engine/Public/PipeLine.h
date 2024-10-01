#pragma once
#include "Base.h"

BEGIN(Engine)
class CPipeLine final :
    public CBase
{
public:
    enum D3DTRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

private:
    CPipeLine();
    virtual ~CPipeLine() = default;

public:
    const _float4x4* Get_Transform_Float4x4(D3DTRANSFORMSTATE eState);
    const _float4x4* Get_Old_Transform_Float4x4(D3DTRANSFORMSTATE eState);
    _matrix Get_Transform_Matrix(D3DTRANSFORMSTATE eState);
    const _float4x4* Get_Transform_Inverse_Float4x4(D3DTRANSFORMSTATE eState);
    _matrix Get_Transform_Inverse_Matrix(D3DTRANSFORMSTATE eState);
    const _float4x4* Get_Shadow_Transform_View_Float4x4();
    const _float4x4* Get_Shadow_Transform_Proj_Float4x4();
    const _float4* Get_CamPosition_Float4();
    _vector Get_CamPosition();
    const _float4* Get_ComLook_Float4();
    _vector Get_CamLook();
    _vector Get_CamRight();
    const _float* Get_CamFar() { return &m_fFar; }
    const _float4x4* Get_ReflectViewMatrix() { return &m_ReflectMatrix; }
    _float4 Get_ShadowLightDir() { return m_vLightDir; }


public:
    void Set_Transform(D3DTRANSFORMSTATE eState, _fmatrix matTransform) {
        m_OldTransformStateMatrix[eState] = m_TransformStateMatrix[eState];
        XMStoreFloat4x4(&m_TransformStateMatrix[eState], matTransform);
    }

    void Set_CamFar(_float fFar) {
        m_fFar = fFar;
    }

    void Set_ReflectViewMatrix(_fmatrix matTransform) {
        XMStoreFloat4x4(&m_ReflectMatrix, matTransform);
    }

    void Set_ShadowTransformViewMatrix(_float4x4* ViewMatrixArray) {
        memcpy(m_ShadowTransformViewMatrix, ViewMatrixArray, sizeof(_float4x4) * 3);
    }

    void Set_ShadowTransformProjMatrix(_float4x4* ProjMatrixArray) {
        memcpy(m_ShadowTransformProjMatrix, ProjMatrixArray, sizeof(_float4x4) * 3);
    }
    
    void Set_ShadowLightDir(_float4 vLightDir) { m_vLightDir = vLightDir; }

public:
    HRESULT Initialize();
    void Tick();

private: //Shadow
    void Ready_ShadowFurstum();

private:
    _float4x4 m_TransformStateMatrix[D3DTS_END];
    _float4x4 m_OldTransformStateMatrix[D3DTS_END];
    _float4x4 m_TransformStateInverseMatrix[D3DTS_END];
    _float4x4 m_ReflectMatrix;
    _float4 m_vCamPosition;
    _float4 m_vCamLook;
    _float4 m_vCamRight;
    _float m_fFar;

    _float4x4 m_ShadowTransformViewMatrix[3];
    _float4x4 m_ShadowTransformProjMatrix[3];

private:
    _float4x4 m_OldWorldMatrix = {};
    _float4x4 m_OldViewMatrix = {};

    _float4 m_vLightDir = { -0.6f, -0.7f, 0.1f, 0.f };

public:
    static CPipeLine* Create();
    virtual void Free();
};

END