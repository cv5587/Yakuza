#pragma once
#include "RoadWalker.h"

BEGIN(Client)
class CRoadYOP :
    public CRoadWalker
{
private:
    CRoadYOP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CRoadYOP(const CRoadYOP& rhs);
    virtual ~CRoadYOP() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

public:
    virtual HRESULT Add_Components() override;
    
public:
    static CRoadYOP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
