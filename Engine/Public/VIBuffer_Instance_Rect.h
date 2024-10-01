#pragma once
#include "VIBuffer_Instance.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Instance_Rect final:
    public CVIBuffer_Instance
{
public:
    typedef struct tINSTANCE_RECT_DESC :public CVIBuffer_Instance::INSTANCE_DESC
    {
        const _float4x4* pNeckMatrix;
        const _float4x4* pLHandMatrix;
        const _float4x4* pRHandMatrix;
        const _float4x4* pLFootMatrix;
        const _float4x4* pRFootMatrix;
    }INSTANCE_RECT_DESC;
private:
    CVIBuffer_Instance_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer_Instance_Rect(const CVIBuffer_Instance_Rect& rhs);
    virtual ~CVIBuffer_Instance_Rect() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg) override;

private:
    const _float4x4* m_pParentMatrix[5];

public:
    void Rim_tick(_float fTimeDelta);
public:
    static CVIBuffer_Instance_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;
};

END