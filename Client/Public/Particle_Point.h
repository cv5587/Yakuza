#pragma once

#include "Effect.h" 

#include "Client_Defines.h"
#include "VIBuffer_Instance_Point.h" 

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Instance_Point;
END

BEGIN(Client)
class CParticle_Point final :
    public CEffect
{
public:
    typedef struct tPARTICLE_POINT_DESC :public CEffect::EFFECT_DESC
    {
        CVIBuffer_Instance::INSTANCE_DESC BufferInstance;
        _float2 fUVCount;
    }PARTICLE_POINT_DESC;


private:
    CParticle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CParticle_Point(const CParticle_Point& rhs);
    virtual ~CParticle_Point() = default;


public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize_Prototype(string strFilePath);
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Tick(const _float& fTimeDelta);
    virtual void Tick(const _float& fTimeDelta);
    virtual void Late_Tick(const _float& fTimeDelta);
    virtual HRESULT Render();

    virtual void* Get_Instance()override;

    virtual void Reset_Buffer() override;
    _float2  Get_UVCount() { return m_fUVCount; }
private:
    CShader* m_pShaderCom = { nullptr };
    CTexture* m_pTextureCom[2] = {nullptr};
    CVIBuffer_Instance_Point* m_pVIBufferCom = { nullptr };

    CVIBuffer_Instance::INSTANCE_DESC m_BufferInstance;
    _float       m_fCurTime = { 0.f };
    _float2     m_fUVCount = { 0.f, 0.f };//uv °¹¼ö

public:
    virtual HRESULT Save_Data(const string strDirectory)override;
    virtual HRESULT Load_Data(const string strDirectory)override;
private:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();

public:
    static CParticle_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    static CParticle_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, string strFilePath);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END
