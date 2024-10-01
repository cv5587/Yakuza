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
class CAura final :
    public CEffect
{
public:
    typedef struct tAURA_DESC :public CEffect::EFFECT_DESC
    {
        CVIBuffer_Instance::INSTANCE_DESC BufferInstance;
        _float2 fUVCount;
        _float2 fToneUVCount;
        wstring ToneTextureTag;
        _float fFlowPow;
        _float fFlowSpeed;
    }AURA_DESC;
private:
    CAura(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CAura(const CAura& rhs);
    virtual ~CAura() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize_Prototype(string strFilePath);
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Tick(const _float& fTimeDelta);
    virtual void Tick(const _float& fTimeDelta);
    virtual void Late_Tick(const _float& fTimeDelta);
    virtual HRESULT Render();

    virtual void* Get_Instance()override;
    _float2  Get_UVCount() { return m_fUVCount; }
    _float2  Get_ToneUVCount() { return m_fToneUVCount; }
    wstring Get_ToneTextureTag() { return m_ToneTextureTag; }
    _float Get_FlowPow() { return m_fFlowPow; }
    _float Get_FlowSpeed() { return m_fFlowSpeed; }
    virtual void Reset_Buffer() override;
public:
    virtual HRESULT Save_Data(const string strDirectory)override;
    virtual HRESULT Load_Data(const string strDirectory)override;

private:
    CShader* m_pShaderCom = { nullptr };
    CTexture* m_pTextureCom[4] = { nullptr };
    CVIBuffer_Instance_Point* m_pVIBufferCom = { nullptr };

private:
    CVIBuffer_Instance::INSTANCE_DESC m_BufferInstance;
    _float2     m_fUVCount = { 0.f, 0.f };//uv °¹¼ö
    _float2     m_fToneUVCount = { 0.f, 0.f };//uv °¹¼ö Åæ
    _float       m_fCurTime = { 0.f };
    wstring     m_ToneTextureTag;
    _float      m_fFlowPow;
    _float      m_fFlowSpeed;

private:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();

public:
    static CAura* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    static CAura* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, string strFilePath);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

END