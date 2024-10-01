#pragma once
#include "Level.h"

#include "UITool_Defines.h"

BEGIN(UITool)
class CLevel_Test :
    public CLevel
{
private:
    CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CLevel_Test() = default;

public:
    virtual HRESULT Initialize();
    virtual void Tick(const _float& fTimeDelta) override;

private:
    HRESULT Ready_Object(const wstring& strLayerTag);

public:
    static CLevel_Test* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};
END
