#pragma once
#include "UIScene.h"
#include "Client_Defines.h"
BEGIN(Client)
class CUILogo :
    public CUIScene
{
protected:
    CUILogo();
    CUILogo(const CUILogo& rhs);
    virtual ~CUILogo() = default;
public:


    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;
    virtual void Action() override;
    virtual void OverAction() override;

private:
    _bool m_isBGMStart = { false };

public:
    static CUILogo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END
