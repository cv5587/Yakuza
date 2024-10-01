#pragma once
#include "UIScene.h"
BEGIN(Client)
class CUIMainMenu :
    public CUIScene
{
protected:
    CUIMainMenu();
    CUIMainMenu(const CUIMainMenu& rhs);
    virtual ~CUIMainMenu() = default;

public:
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

    virtual void Action() override;
    virtual void OverAction() override;

private:
    _bool m_isBGM = { false };
public:
    static CUIMainMenu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END
