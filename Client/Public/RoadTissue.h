#pragma once
#include "RoadStanding.h"

BEGIN(Client)
class CRoadTissue :
    public CRoadStanding
{
private:
    CRoadTissue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CRoadTissue(const CRoadTissue& rhs);
    virtual ~CRoadTissue() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

public:
    virtual HRESULT Add_Components() override;
    virtual void Change_Animation() override;
    
public:
    static CRoadTissue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
