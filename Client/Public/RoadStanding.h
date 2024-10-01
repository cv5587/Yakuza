#pragma once
#include "Adventure.h"

BEGIN(Client)
class CRoadStanding abstract :
    public CAdventure
{
protected:
    CRoadStanding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CRoadStanding(const CRoadStanding& rhs);
    virtual ~CRoadStanding() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

protected:
    class CAI_RoadStanding* m_pTree = { nullptr };

protected:
    virtual HRESULT Add_Components() override;
    virtual void Change_Animation() override;

public:
    virtual void Free();
};
END
