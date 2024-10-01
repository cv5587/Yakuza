#pragma once
#include "CarChase_Monster.h"

BEGIN(Client)
class CCarChase_Heli :
    public CCarChase_Monster
{
private:
    CCarChase_Heli(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCarChase_Heli(const CCarChase_Heli& rhs);
    virtual ~CCarChase_Heli() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

private:
    virtual void Change_Animation();
    virtual HRESULT Add_Components() override;
    HRESULT Add_Objects();

private:
    class CSocketModel* m_pWeapon = { nullptr };

public:
    static CCarChase_Heli* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
