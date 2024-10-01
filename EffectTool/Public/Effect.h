#pragma once
#include "BlendObject.h"

#include "Client_Defines.h"
#include "VIBuffer_Instance.h"

BEGIN(Engine)
class CVIBuffer_Instance;
END


BEGIN(Client)
class CEffect abstract :
    public CBlendObject
{
public:
    typedef struct tEFFECT_DESC :public CBlendObject::GAMEOBJECT_DESC
    {
        _uint eType;
        _float4			vStartPos;
        _float           fStartTime;
        _float          fRotate;
        wstring ParticleTag;
        wstring TextureTag;
        _float4 vStartColor;
        _float4 vEndColor;
        _float2 fLifeAlpha;
        _int iShaderPass;

        _float fDistortion;

        _bool isNormal;
        wstring NormalTag;

        const _float4x4* pWorldMatrix = { nullptr };
    }EFFECT_DESC;

    enum TYPE { TYPE_POINT, TYPE_TRAIL, TYPE_GLOW, TYPE_AURA, TYPE_MESH,TYPE_MESHEFFECT, TYPE_END };
    enum ACTION { ACTION_SPREAD,  ACTION_SIZEUP, ACTION_SIZEDOWN,  ACTION_NOBILLBOARD, ACTION_FALLSPREAD,ACTION_BLOOD, ACTION_INTERVAL, ACTION_END };
    static const _uint iAction[ACTION_END];

protected:
    CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CEffect(const CEffect& rhs);
    virtual ~CEffect() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg)override;
    virtual void Priority_Tick(const _float& fTimeDelta)override;
    virtual void Tick(const _float& fTimeDelta)override;
    virtual void Late_Tick(const _float& fTimeDelta)override;
    virtual HRESULT Render()override;
public:
    HRESULT Edit_Action(ACTION iEditAction);

    virtual void* Get_Instance();

    void Set_WorldMatrix(const _float4x4* mat) { m_pWorldMatrix = mat; }

public:
    void Set_StartPos(_float4 Pos) {
        m_vStartPos = Pos;
    }


    _uint Get_Type() const { return m_eType; }
    wstring Get_Tag() { return m_ParticleTag; }
    _float* Get_pStartTime() { return &m_fStartTime; }
    _float4 Get_StartPos() { return m_vStartPos; }
    _uint Get_Action() { return m_iAction; }
    _float4 Get_SColor() { return m_vStartColor; }
    _float4 Get_EColor() { return m_vEndColor; }
    _int Get_ShaderPass() { return m_iShaderPass; }
    wstring Get_TextureTag() { return m_TextureTag; }
    _float2 Get_LifeAlpha() { return m_fLifeAlpha; }
    _float Get_Rotate() { return m_fRotate; }
    _float Get_fDistortion() { return m_fDistortion; }
    _bool Get_isNormal() {        return m_isNormal;    }
    wstring Get_NormalTag() { return m_NormalTag; }

    virtual void Reset_Buffer();
public:
    virtual HRESULT Save_Data(const string strDirectory);
    virtual HRESULT Load_Data(const string strDirectory);

protected:

    _uint          m_eType = { TYPE_END };
    wstring     m_ParticleTag = { TEXT("") };
    wstring     m_TextureTag = { TEXT("") };
    _int          m_iShaderPass = { 0 };

    _float       m_fStartTime = { 0.f };//작업용파싱x(세이브로드엔 들어가기)
    _float4     m_vStartPos = {};
    _float       m_fRotate = { 0.f };
    _float2     m_fLifeAlpha = { 0.f,0.f };
    _uint			m_iAction = { 0 };
    _float4     m_vStartColor = { 0.f , 0.f , 0.f , 0.f };
    _float4     m_vEndColor = { 0.f , 0.f , 0.f , 0.f };
    _float      m_fDistortion = { 0.f };

    _bool       m_isNormal = { false };
    wstring    m_NormalTag = { TEXT("") };

    const _float4x4* m_pWorldMatrix = { nullptr };

public:
    virtual void Free() override;
};
END
