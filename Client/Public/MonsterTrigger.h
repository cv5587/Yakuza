#pragma once

#include "Trigger.h"
#include "FileTotalMgr.h"

class CMonsterTrigger : public CTrigger
{
private:
    CMonsterTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CMonsterTrigger(const CMonsterTrigger& rhs);
    virtual ~CMonsterTrigger() = default;

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
    CFileTotalMgr*              m_pFileMgr = { nullptr };

private:
    virtual HRESULT Add_Components(void* pArg) override;

public:
    static CMonsterTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};

