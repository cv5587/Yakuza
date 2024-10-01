#pragma once
#include "UI_Object.h"

#ifdef _TOOL
#include "UITool_Defines.h"
#else
#include "Client_Defines.h"
#endif // _TOOL



BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
END

#ifdef _TOOL
BEGIN(UITool)
#else
BEGIN(Client)
#endif

class CUI_Texture :
    public CUI_Object
{
public:
    typedef struct tUITextureDesc : public CUI_Object::UI_OBJECT_DESC {
        wstring strTextureFilePath;
        wstring strTextureFileName;
        _bool isParent = { false };
        const _float4x4* pParentMatrix;
        _float2 fStartUV;
        _float2 fEndUV = { 1.f , 1.f };
        _bool isColor;
        _float4 vColor;
        _float4 vEndColor;
        string strParentName;
        _uint iShaderPass;
        _float4x4 WorldMatrix;

        _bool isAnimLoop;
        _bool bAnim;
        _float2 fAnimTime;
        _float3 vStartPos;//최종위치는 worldpos 
        _float2 vAnimScale;//최종크기는 world
        _float2 fControlAlpha;
        _bool isReverse;

        _float4 vUpPoint;
        _float4 vDownPoint;
    } UI_TEXTURE_DESC;

public:
    void Set_SizeX(_float fSizeX) { m_fSizeX = fSizeX; }
    void Set_SizeY(_float fSizeY) { m_fSizeY = fSizeY; }
    void Set_ShaderPass(_uint iPass) { m_iShaderPass = iPass; }
    void Set_isColor(_bool isColor) { m_isColor = isColor; }
    void Set_Color(_float4 vColor) { m_vColor = vColor; }
    void Set_EndColor(_float4 vEndColor) { m_vEndColor = vEndColor; }
    void Set_WorldMatrix(_float4x4 World) { m_WorldMatrix = World; }

    void Set_StartPos(_float3 vStartPos) { m_vStartPos = vStartPos;}
    void Set_AnimScale(_float2 vAnimScale) { m_vAnimScale = vAnimScale;}
    void Set_AnimTime(_float2 fAnimTime) { m_fAnimTime = fAnimTime; }
    void Set_isAnim(_bool isAnim) { m_isAnim = isAnim; }
    void Set_isAnimLoop(_bool isAnimLoop) { m_isAnimLoop = isAnimLoop; }

#ifdef _TOOL
    void Set_isPlay(_bool isPlay) { m_isPlay = isPlay; }
#endif // _TOOL
    void Set_ControlAlpha(_float2 ControlAlpha) { m_fControlAlpha = ControlAlpha; }
    void Set_isReverse(_bool isReverse) { m_isReverse = isReverse; }

    void Set_UpPoint(_float4 UpPoint) { m_fUpPoint = UpPoint; }
    void Set_DownPoint(_float4 DownPoint) { m_fDownPoint = DownPoint; }

public:
    _float Get_SizeX() { return m_fSizeX; }
    _float Get_SizeY() { return m_fSizeY; }
    _uint Get_ShaderPass() { return m_iShaderPass; }

    _float2 Get_StartUV() { return m_fStartUV; }
    _float2 Get_EndUV() { return m_fEndUV; }

    wstring Get_FileName() { return m_strTextureName; }
    wstring Get_FilePath() { return m_strTextureFilePath; }

    _bool Get_isColor(){ return m_isColor; }
    _float4 Get_Color() { return m_vColor; }
    _float4 Get_EndColor() { return m_vEndColor; }

    _float3 Get_StartPos() { return m_vStartPos; }
    _float2 Get_AnimScale() { return m_vAnimScale; }
    _float2 Get_AnimTime() { return m_fAnimTime; }
    _bool Get_isAnim() { return m_isAnim; }
    _bool Get_isAnimLoop() { return m_isAnimLoop; }
    
    _float4 Get_UpPoint() { return m_fUpPoint; }
    _float4 Get_DownPoint() { return m_fDownPoint; }

#ifdef _TOOL
    _bool Get_isPlay() { return m_isPlay; }
#endif // _TOOL

    _float2 Get_ControlAlpha() { return m_fControlAlpha; }
    _bool Get_isReverse() { return m_isReverse; }

protected:
    CUI_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_Texture(const CUI_Texture& rhs);
    virtual ~CUI_Texture() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
     HRESULT Change_UV(_float2 fStartUV, _float2 fEndUV);
     HRESULT Change_Point(_float4 vUpPoint, _float4 vDownPotint);

public:
    virtual HRESULT Show_UI() override;
    virtual HRESULT Close_UI() override;
protected:
    _float							m_fX, m_fY, m_fSizeX, m_fSizeY;
    _float4x4						m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

    CShader* m_pShaderCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };

    const _float4x4* m_pParentMatrix = { nullptr };

protected:
    _bool m_isParent = { false };
    string m_strParentName = { "" };

    wstring m_strTextureFilePath = { L"" };
    wstring m_strTextureName = { L"" };

    _float2                         m_fStartUV = { 0.f, 0.f }, m_fEndUV = { 1.f, 1.f };
    _float4                         m_fUpPoint = { 0.f,0.f, 0.f, 0.f }, m_fDownPoint = { 0.f,0.f, 0.f, 0.f };
    _float4                         m_vColor = { 1.f, 1.f, 1.f, 1.f }, m_vEndColor = { 1.f, 1.f, 1.f, 1.f };
    _bool                           m_isColor = { false };

    _uint m_iShaderPass = { 1 };

    _bool m_isAnim;
    _float2 m_fAnimTime;
    _float3 m_vStartPos;
    _float2 m_vAnimScale = { 1.f, 1.f };//x스타트,y엔드

    _bool m_isPlay = { true };//버튼 만 over시에 사용

    //시작하면 0-1 사라질땐 반대로
    _float2 m_fControlAlpha = { 0.f, 1.f };//시작,종료 알파(애님일경우)]
    _bool m_isReverse = { false };

    _bool m_isAnimLoop = { false };

public:
    virtual HRESULT Save_binary(const string strDirectory)override; 
    virtual HRESULT Save_Groupbinary( ofstream& out)override;
    virtual HRESULT  Load_binary(ifstream& in)override;

protected:
    virtual HRESULT Add_Components();
    virtual HRESULT Bind_ResourceData();

public:
    virtual void Free();
};
END
