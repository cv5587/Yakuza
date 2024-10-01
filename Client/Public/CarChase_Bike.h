#pragma once
#include "CarChase_Monster.h"

BEGIN(Client)
class CCarChase_Bike :
    public CCarChase_Monster
{
private:
    CCarChase_Bike(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCarChase_Bike(const CCarChase_Bike& rhs);
    virtual ~CCarChase_Bike() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

private:
    virtual void Change_Animation() override;
    virtual HRESULT Add_Components() override;
    virtual void Set_ParentMatrix(const _float& fTimeDelta) override;
    HRESULT Add_Objects();

private:
    class CSocketModel* m_pWeapon = { nullptr };

public:
    static CCarChase_Bike* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
