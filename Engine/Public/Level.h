#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CLevel abstract:
    public CBase
{
protected:
    CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CLevel() = default;

public:
    virtual HRESULT Initialize();
    virtual void Tick(const _float& fTimeDelta);
    virtual void Render();

protected:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };

protected:
    bool                m_bSceneChange = { false };
    _bool m_isStart = { false };
    _bool m_isTitleEnd = { false };
    _bool m_isFadeFin = { false };

public:
    virtual void Free() override;
};

END