#pragma once
#include "Level.h"

#include "UITool_Defines.h"

BEGIN(UITool)
class CLevel_Logo :
    public CLevel
{
private:
    CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CLevel_Logo() = default;

public:
    virtual HRESULT Initialize() override;
    virtual void Tick(const _float& fTimeDelta) override;

public:
    static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};
END