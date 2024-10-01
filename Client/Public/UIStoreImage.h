#pragma once
#include "UIScene.h"
BEGIN(Client)
class CUIStoreImage :
    public CUIScene
{
protected:
    CUIStoreImage();
    CUIStoreImage(const CUIStoreImage& rhs);
    virtual ~CUIStoreImage() = default;

public:
    void Close_Image();

public:
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;


public:
    static CUIStoreImage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END
