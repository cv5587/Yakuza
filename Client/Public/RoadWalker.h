#pragma once
#include "Adventure.h"

/*걷고 있는 사람들*/
BEGIN(Client)
class CRoadWalker abstract :
    public CAdventure
{
protected:
    CRoadWalker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CRoadWalker(const CRoadWalker& rhs);
    virtual ~CRoadWalker() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

protected:
    class CAI_RoadWalker* m_pTree = { nullptr };

protected:
    virtual HRESULT Add_Components() override;
    virtual void Change_Animation() override;
    
public:
    virtual void Free();
};
END
