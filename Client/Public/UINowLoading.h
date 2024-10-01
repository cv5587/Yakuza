#pragma once
#include "UIScene.h"
BEGIN(Client)
class CUINowLoading :
    public CUIScene
{
protected:
    CUINowLoading();
    CUINowLoading(const CUINowLoading& rhs);
    virtual ~CUINowLoading() = default;

public:
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

    virtual void Action() override;
    virtual void OverAction() override;
public:
    static CUINowLoading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};

END