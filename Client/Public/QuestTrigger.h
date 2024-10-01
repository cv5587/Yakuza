#pragma once

#include "Trigger.h"

BEGIN(Client)
class CQuestTrigger : public CTrigger
{
private:
    CQuestTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CQuestTrigger(const CQuestTrigger& rhs);
    virtual ~CQuestTrigger() = default;

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
    class CQuestManager* m_pQuestManager = { nullptr };
    class CUIManager* m_pUIManager = { nullptr };

private:
    virtual HRESULT Add_Components(void* pArg) override;

public:
    static CQuestTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};

END