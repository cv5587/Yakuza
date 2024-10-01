#pragma once
#include "Base.h"

#include "Engine_Defines.h"
#include "GameInstance.h"
BEGIN(Engine)
class CCustomFont final : public CBase
{
public:
    struct LineInfo {
        wstring text;
        _float2 size;
        float cumulativeHeight;
    };
    enum ALIGN{MIDDLE, RIGHT, LEFT, ALIGN_END};

private:
    CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CCustomFont() = default;

public:
    HRESULT Initialize(const wstring& strFontFilePath);
    HRESULT Render(const wstring& strText, const _float2& vPosition, _fvector vColor); // 직교
    HRESULT Align_Render(const wstring& strText, const _float2& vPosition, _fvector vColor, _uint iAlignment);//직교 정렬
    HRESULT Perspective_Render(const wstring& strText, const _float2& vPosition, _fvector vColor, _float fScale, const _float& fTimeDelta); // 원근 
    HRESULT Blend_Render(const wstring& strText, const _float2& vPosition, _fvector vColor);

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    SpriteBatch* m_pBatch = { nullptr };
    SpriteFont* m_pFont = { nullptr };

    CGameInstance* m_pGameInstance = { nullptr };
    ID3D11BlendState* m_pBlendState = { nullptr };

public:
    static CCustomFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontFilePath);
    virtual void Free() override;
};
END
