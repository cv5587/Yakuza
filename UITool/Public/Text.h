#pragma once
#include "UI_Texture.h"



#ifdef _TOOL
BEGIN(UITool)
#else
BEGIN(Client)
#endif
class CText :
    public CUI_Texture
{
public:
    typedef struct tTextDesc : public UI_TEXTURE_DESC
    {
        wstring strText;
        _uint iAlign;
        wstring Font;
    }TEXT_DESC;

public:
    wstring Get_Text() { return m_strText; }
    _uint Get_Align() { return m_iAlign; }
    wstring Get_Font() { return m_Font; }
public:
    void Set_Text(wstring strText) { m_strText = strText; }
    void Set_Align(_uint iAlign) { m_iAlign = iAlign; }
    void Set_Font(wstring Font) { m_Font = Font; }
private:
    CText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CText(const CText& rhs);
    virtual ~CText() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
     HRESULT Initialize_Prototype(ifstream& in) ;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    virtual HRESULT Save_binary(const string strDirectory)override;
    virtual HRESULT Save_Groupbinary(ofstream& out)override;
    virtual HRESULT  Load_binary(ifstream& in)override;

private:
    _uint m_iAlign = { 1 };//우측정렬
    wstring m_strText = { L"" };
    wstring m_Font = {TEXT("")};
public:
    static CText* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    static CText* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,ifstream& in);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
