#pragma once
#include "Monster.h"

BEGIN(Client)
class CKuze :
    public CMonster
{
public:
    enum KUZE_PAGE { ONE, TWO, PAGE_END };

private:
    CKuze(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CKuze(const CKuze& rhs);
    virtual ~CKuze() = default;

public:
    _uint Get_KuzePage() { return m_iPage; }

public:
    void Set_KuzePage(_uint iPage) { m_iPage = iPage; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

private:
    _uint m_iPage = { ONE };


private:
    virtual void Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack = false) override;
    virtual HRESULT Add_Components() override;
    virtual void Change_Animation() override;
    virtual void Trail_Event() override;

public:
    static CKuze* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};
END
