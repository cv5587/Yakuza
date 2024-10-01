#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Instance abstract :
    public CVIBuffer
{
public:
    typedef struct tVIBufferInstanceDesc
    {
        _uint iNumInstance;
        _float3 vOffsetPos;
        _float3 vPivotPos;
        _bool bRadius;
        _float3 vRange;//정육면체 생성
        _float fRadius;//구 생성
        _float2 vSize;//최초 사이즈(가로*세로)(지오메트리)
        _float2 vRectSize;//각 파티클 사이즈
        _float2 vSpeed;
        _float2 vLifeTime;
        _bool isLoop;
        const _float4x4* WorldMatrix;//뼈+부모(위치 기록후 따라오지 않게 하기 위해 생성)
        
        //돈파티클(동전,지폐)
        _float3 LowStartRot;
        _float3 HighStartRot;
        _float3 LowAngleVelocity;
        _float3 HighAngleVelocity;
        _float GravityScale;
        _float CrossArea;//단면적가로저항
        _bool isBillboard;

        _float3 vMinSpeed;
        _float3 vMaxSpeed;
        _float2 fWeight;//min,max
        _float3 vMinFrequency;//진동 주파수
        _float3 vMaxFrequency;
        _float3 vMinAmplitude;
        _float3 vMaxAmplitude;//진동 범위
        //아우라
        _bool isAttach;
        //blood
        _float fDelay;

    }INSTANCE_DESC;

protected:
    CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer_Instance(const CVIBuffer_Instance& rhs);
    virtual ~CVIBuffer_Instance() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg) override;
    virtual HRESULT Render() override;
    virtual HRESULT Compute_Render() ;

protected:
    //기본 정보 가지고 있게 해야됨.
    ID3D11Buffer* m_pVBInstance = { nullptr };
    D3D11_BUFFER_DESC	m_InstanceBufferDesc = {};
    INSTANCE_DESC* m_InstanceDesc = {};

    _uint m_iNumInstance = { 0 };
    _uint m_iIndexCountPerInstance = { 0 };
    _uint m_iInstanceStride = { 0 };

    _float* m_pSpeeds = { nullptr };
    _float3* m_pOriginalPositions = { nullptr };
    _float* m_pOriginalSize = { nullptr };
    _float3* m_pOriginalOffsets = { nullptr };
    _float3* m_pOriginalAngleVelocity = { nullptr };
    _float3* m_pMeshSpeed = { nullptr };
    _float* m_pWeight = { nullptr };
    _float3* m_pFrequency = { nullptr };
    _float3* m_pAmplitude = { nullptr };
    _bool* m_pIsPlay = { nullptr };

    const _float4x4* m_pCurrentWorldMatrix = { nullptr };

    //기존 cpu 블렌드 소트용
    VTXMATRIX* m_pTempVertices;

    //컴퓨트 셰이더(인스턴스로 생성하면 모두 정령 필요.)
    class CComputeShader* m_pComputeShader = { nullptr };


    //계산후 받아올 버퍼(읽기/쓰기 버퍼)
    ID3D11Buffer* m_pOutBuffer = { nullptr };
    D3D11_BUFFER_DESC m_OutBufferDesc = {};


    //읽기/쓰기 버퍼 바인딩용으로 변환용
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;

    ID3D11UnorderedAccessView* mOutputUAV = {};

    //계산후 복사한 버퍼를 렌더에 필요한 부분만 담아주기.
    ID3D11Buffer* m_pRenderBuffer = { nullptr };
    D3D11_BUFFER_DESC m_RenderBufferDesc = {};

    _bool m_isReset = { false };

    _float m_iCurrentTime = { 0.f };
/// <summary>
/// vRectSize.y 오라에서는 회전
///     포인트에서는 딜레이
/// </summary>
public:
    void Spread(_float fTimeDelta);
    void RotSpread(_float fTimeDelta);
    void FallSpread(_float fTimeDelta);
    void BloodSpread(_float fTimeDelta);
    void MeshSpread(_float fTimeDelta);
    void IntervalSpread(_float fTimeDelta);
    void Reset();

    _bool LifeTime_Check();
    void SizeUp_Time(_float fTimeDelta);
    void SizeDown_Time(_float fTimeDelta);

    void Compute_Sort();

    
public:
    virtual CComponent* Clone(void* pArg) = 0;
    virtual void Free() override;
};
END
