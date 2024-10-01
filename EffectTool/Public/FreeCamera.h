#pragma once
#include "Camera.h"
#include "Client_Defines.h"
BEGIN(Client)
class CFreeCamera :
    public CCamera
{
public:
    typedef struct FREE_CAMERA_DESC : public CCamera::CAMERA_DESC
    {
        _float		fSensor;
        _float4x4      vPrePosition;
    };
private:
    CFreeCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CFreeCamera(const CFreeCamera& rhs);
    virtual ~CFreeCamera() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    void Mouse_Fix();

private:
    _float		m_fSensor = { 0.0f };
    _bool		m_bTab = { true };

public:
    static CFreeCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};

END