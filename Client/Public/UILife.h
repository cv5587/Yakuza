#pragma once
#include "UIScene.h"
BEGIN(Client)
class CUILife :
    public CUIScene
{
public:
    enum TYPE {GAUAGE=3 , NUM, LIGHT , TYPE_END};
    enum GAUAGE{ HP , SKILL0, SKILL1 , SKILL2 , GAUAGE_END};
protected:
    CUILife();
    CUILife(const CUILife& rhs);
    virtual ~CUILife() = default;

public:

    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

public:
    void Update_PlayerInfo();
private:
    _uint m_iBts = { 0 };
    _uint m_iHitLevel = { 0 };

    _float m_fEndHit[3] = { -0.019 , -0.016, -0.017 };

public:
    void Change_Style(_uint iStyle);
public:
    static CUILife* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg=nullptr);
    virtual void Free();
};
END
