#pragma once
#include "CarChase_Monster.h"

BEGIN(Client)
class CCarChase_Van :
    public CCarChase_Monster
{
private:
    CCarChase_Van(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCarChase_Van(const CCarChase_Van& rhs);
    virtual ~CCarChase_Van() = default;

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
    class CShotGun* m_pShotGun = { nullptr };

public:
    static CCarChase_Van* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
