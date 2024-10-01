#pragma once
#include "RoadWalker.h"

BEGIN(Client)
class CRoadNML :
    public CRoadWalker
{
private:
    CRoadNML(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CRoadNML(const CRoadNML& rhs);
    virtual ~CRoadNML() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

public:
    virtual HRESULT Add_Components() override;
    
public:
    static CRoadNML* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};

END
