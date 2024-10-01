#pragma once
#include "RoadStanding.h"

/* ÀþÀº »ç¶÷ */
BEGIN(Client)
class CRoadStanding_NML :
    public CRoadStanding
{
enum MOTION_TYPE {
    MOTION_TALK_A,  
    MOTION_TALK_B,
    MOTION_LISTEN,
    MOTION_END
};

private:
    CRoadStanding_NML(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CRoadStanding_NML(const CRoadStanding_NML& rhs);
    virtual ~CRoadStanding_NML() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

public:
    _uint m_iMotionType = { MOTION_END };

public:
    virtual HRESULT Add_Components() override;
    virtual void Change_Animation() override;

public:
    static CRoadStanding_NML* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
