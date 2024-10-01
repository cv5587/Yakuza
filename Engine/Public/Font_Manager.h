#pragma once
#include "Base.h"

BEGIN(Engine)
class CFont_Manager :
    public CBase
{
public:
    enum FONT {NANUM24 , NANUM28, NANUM30 , NANUM36 , FONT_END};
private:
    CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CFont_Manager() = default;

public:
    HRESULT Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);
    HRESULT Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor);
    HRESULT AlignRender_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor, _uint iAlignment);
    HRESULT Perspective_Render(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor, _float fScale, const _float& fTimeDelta);
    HRESULT Blend_Render(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor);

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    map<const wstring, class CCustomFont*> m_Fonts;

private:
    class CCustomFont* Find_Font(const wstring& strFontTag);

public:
    static CFont_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};
END
