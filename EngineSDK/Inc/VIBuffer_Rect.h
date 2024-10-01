#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Rect :
    public CVIBuffer
{
public:
    typedef struct tRect_Desc {
        _float4 vUpPoint;
        _float4 vDownPoint;
        _float2 fStartUV;
        _float2 fEndUV;
    }RECT_DESC;

private:
    CVIBuffer_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer_Rect(const CVIBuffer_Rect& rhs);
    virtual ~CVIBuffer_Rect() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    
public:
    HRESULT EditUV(_float2 fStartUV, _float2 fEndUV);
    HRESULT Edit_Point(_float4 vUpPoint, _float4 vDownPoint);
public:
    static CVIBuffer_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};
END
