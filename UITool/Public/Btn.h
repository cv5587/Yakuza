#pragma once
#include "UI_Texture.h"
#ifdef _TOOL
#include "UITool_Defines.h"
#else
#include "Client_Defines.h"
#endif // _TOOL
BEGIN(Engine)
class CCollider;
END

#ifdef _TOOL
BEGIN(UITool)
#else
BEGIN(Client)
#endif
class CBtn :
    public CUI_Texture
{
public:
    typedef struct tBtnDesc : public UI_TEXTURE_DESC {
        wstring strClickFilePath; // 클릭했을때의 이미지
        wstring strClickFileName;
        _float2 ClickStartUV;
        _float2 ClickEndUV = { 1.f,1.f };
        _float4 vClickColor;
    }BTN_DESC;
private:
    CBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBtn(const CBtn& rhs);
    virtual ~CBtn() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
     HRESULT Initialize_Prototype(ifstream& in) ;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    HRESULT Chage_ClickUV(_float2 StartUV, _float2 EndUV);
    
    _float2 Get_ClickStartUV() { return m_ClickStartUV; }
    _float2 Get_ClickEndUV() { return m_ClickEndUV; }

    wstring Get_ClickFileName() { return m_StrClickFileName; }
    wstring Get_ClickFilePath() { return m_strClickFilePath; }

    void Set_Click(_bool Click) { m_isClick = Click; }
    _bool Get_Click() { return m_isClick; }


    void Set_ClickColor(_float4 ClickColor) { m_vClickColor = ClickColor; }
    _float4 Get_ClickColor() { return m_vClickColor; }
public:
    virtual HRESULT Save_binary(const string strDirectory)override;
    virtual HRESULT Save_Groupbinary(ofstream& out)override;
    virtual HRESULT Load_binary(ifstream& in)override;
private:
    CTexture* m_pClickTextureCom = { nullptr };
    CVIBuffer_Rect* m_pClickVIBufferCom = { nullptr };

private:
    wstring m_strClickFilePath = { L"" };
    wstring m_StrClickFileName = { L"" };
    
    _bool m_isClick = { false };
    _float2 m_ClickStartUV = { 0.f ,0.f };
    _float2 m_ClickEndUV = { 1.f, 1.f };
    _float4 m_vClickColor;


private:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;

public:
    static CBtn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    static CBtn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,ifstream& in);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
