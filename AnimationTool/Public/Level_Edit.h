#pragma once
#include "Level.h"
#include "Client_Defines.h"
BEGIN(Client)
class CLevel_Edit final:
    public CLevel
{
private:
    CLevel_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CLevel_Edit() = default;

public:
    virtual HRESULT Initialize();
    virtual void Tick(const _float& fTimeDelta) override;

private:
    HRESULT Ready_Lights();

    HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
    HRESULT Ready_Object(const wstring& strLayerTag);
    HRESULT Ready_Effect(const wstring& strLayerTag);

public:
    static CLevel_Edit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

END