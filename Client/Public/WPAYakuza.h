#pragma once
#include "Monster.h"

BEGIN(Client)
class CWPAYakuza :
    public CMonster
{
private:
    CWPAYakuza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CWPAYakuza(const CWPAYakuza& rhs);
    virtual ~CWPAYakuza() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

private:
    virtual HRESULT Add_Components() override;
    virtual void Change_Animation() override;

public:
    static CWPAYakuza* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
