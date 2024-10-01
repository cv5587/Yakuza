#pragma once
#include "Level.h"

#include "UITool_Defines.h"

BEGIN(UITool)

class CLevel_RunMap 
    : public CLevel
{
private:
	CLevel_RunMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_RunMap() = default;

public:
    virtual HRESULT Initialize();
    virtual void Tick(const _float& fTimeDelta) override;

private:
    HRESULT Ready_Lights();
    HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
    HRESULT Ready_Layer_GameObject(const wstring& strLayerTag);

public:
    static CLevel_RunMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

END