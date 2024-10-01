#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CComputeShader final :
    public CComponent
{
public:
    typedef struct tCOMPUTE_DESC {
        ID3D11UnorderedAccessView* OutputBuffer;
        _uint NumElement;
    }COMPUTE_DESC;

private:
    CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CComputeShader(const CComputeShader& rhs);
    virtual ~CComputeShader() = default;

public:
    virtual HRESULT Initialize_Prototype(const wstring& strShaderFilePath);
    virtual HRESULT Initialize(void* pArg) override;
    HRESULT Render(_uint iGroupX, _uint iGroupY, _uint iGroupZ);

private:
    ID3D11ComputeShader* m_pComputeShader;

    _float4 m_vClearColor = {};

public:
    static CComputeShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

END