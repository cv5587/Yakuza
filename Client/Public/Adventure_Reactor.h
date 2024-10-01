#pragma once
#include "LandObject.h"

BEGIN(Client)
class CAdventure_Reactor :
    public CLandObject
{
public:
    typedef struct tMapAdventureReactorObjDesc : public CLandObject::LANDOBJ_MAPDESC
    {

    }ADVENTURE_REACTOR_IODESC;

private:
    CAdventure_Reactor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CAdventure_Reactor(const CAdventure_Reactor& rhs);
    virtual ~CAdventure_Reactor() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    _float m_fSpeed = { 30.f };

private:
    virtual string Get_CurrentAnimationName() override;
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;

private:
    void Move(const _float& fTimeDelta);

public:
    static CAdventure_Reactor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
