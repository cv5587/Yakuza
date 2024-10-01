#pragma once

#include "Trigger.h"

class CLevelTrigger : public CTrigger
{
private:
    CLevelTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CLevelTrigger(const CLevelTrigger& rhs);
    virtual ~CLevelTrigger() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    virtual bool				Move_Scene(int& iLevelNum);

private:
    virtual HRESULT Add_Components(void* pArg) override;

private:
    _float                      m_fMaxScale;

public:
    static CLevelTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};

