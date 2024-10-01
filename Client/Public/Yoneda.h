#pragma once
#include "Monster.h"

/*
* WPB : ³ªÀÌÇÁ
*/
BEGIN(Client)
class CYoneda :
    public CMonster
{
public:
    enum WEAPON { BAREHAND, KNIFE, WEAPON_END };
private:
    CYoneda(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CYoneda(const CYoneda& rhs);
    virtual ~CYoneda() = default;
    
public:
    void Set_TriggerQte(_uint iWeaponChange, _uint iTriggerID);

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

    virtual void Off_Sync() override;

private:
    _uint m_iWeaponType = { WEAPON_END };

private:
    virtual HRESULT Add_Components() override;
    virtual void Change_Animation();
    

public:
    static CYoneda* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};
END
