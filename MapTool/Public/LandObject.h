#pragma once
#include "GameObject.h"

#include "Client_Defines.h"

BEGIN(Client)
class CLandObject abstract :
    public CGameObject
{
protected:
    CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CLandObject(const CLandObject& rhs);
    virtual ~CLandObject() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

protected:
    class CSystemManager* m_pSystemManager = { nullptr };

private:
    virtual HRESULT Add_Componenets();
    virtual HRESULT Bind_ResourceData();

public:
    virtual void Free() override;
};
END
