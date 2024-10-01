#pragma once
#include "Effect.h"
#include "Client_Defines.h"
#include "VIBuffer_Trail.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Trail;
END

BEGIN(Client)
class CTRailEffect final :
    public CEffect
{
public:
    typedef struct tTRAIL_DESC :public CEffect::EFFECT_DESC
    {
        CVIBuffer_Trail::VIBUFFER_TRAIL_DESC Trail_Desc;
    }TRAIL_DESC;
private:
    CTRailEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CTRailEffect(const CTRailEffect& rhs);
    virtual ~CTRailEffect() = default;

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
public:
    virtual HRESULT Save_Data(const string strDirectory)override;
    virtual HRESULT Load_Data(const string strDirectory)override;

private:
    CShader* m_pShaderCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };
    CVIBuffer_Trail* m_pVIBufferCom = { nullptr };

private:
    CVIBuffer_Trail::VIBUFFER_TRAIL_DESC m_TrailDesc = {};
private:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();
public:
    static CTRailEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    static CTRailEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, string strFilePath);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

END