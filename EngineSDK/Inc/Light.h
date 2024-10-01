#pragma once
#include "Base.h"

BEGIN(Engine)
class CLight :
    public CBase
{
public:
    struct LIGHT_BUFFER {
        _float4 vDirection;
        _float4 vDiffuse;
        _float4 vAmbient;
        _float4 vSpecular;
    };

private:
    CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CLight() = default;

public:
    const LIGHT_DESC*   Get_LightDesc() const { return &m_LightDesc; }
    void                Set_LightDesc(LIGHT_DESC& LightDesc) { m_LightDesc = LightDesc; }

public:
    HRESULT             Initialize(const LIGHT_DESC& LightDesc);
    HRESULT             Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

public:
    HRESULT Bind_LightBuffer(_uint iSlot);

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

    LIGHT_DESC          m_LightDesc{};
    
    ID3D11Buffer* m_pLightBuffer = { nullptr };

private:
    void Ready_Buffer();

public:
    static CLight*      Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc);
    virtual void        Free() override;
};
END
