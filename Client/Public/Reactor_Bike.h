#pragma once
#include "CarChase_Reactor.h"

BEGIN(Client)
class CReactor_Bike :
    public CCarChase_Reactor
{
private:
    CReactor_Bike(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CReactor_Bike(const CReactor_Bike& rhs);
    virtual ~CReactor_Bike() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

private:
    virtual HRESULT Ready_Monster(_int* pMonsterTypes);
    virtual void Change_Animation();

private:
    virtual HRESULT Add_Components() override;

public:
    static CReactor_Bike* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
