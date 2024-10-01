#pragma once
#include "CarChase_Monster.h"

/*
* 플레이어가 오른쪽에 있을 때, AIML_L,B / AIMR_L,F;
* 플레이어가 왼쪽에 있을 때, AIMR_R,B / AIML_R,F
*/
BEGIN(Client)
class CCarChase_Sedan :
    public CCarChase_Monster
{
public:


private:
    CCarChase_Sedan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCarChase_Sedan(const CCarChase_Sedan& rhs);
    virtual ~CCarChase_Sedan() = default;

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

private:
    _uint Change_Dir();
    

public:
    static CCarChase_Sedan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
