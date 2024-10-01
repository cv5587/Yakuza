#pragma once
#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)
class CLevel_DogimazoStairs :
    public CLevel
{
private:
    CLevel_DogimazoStairs(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CLevel_DogimazoStairs() = default;

public:
    virtual HRESULT Initialize();
    virtual void Tick(const _float& fTimeDelta) override;

private:
    class CSystemManager* m_pSystemManager = { nullptr };
    class CFileTotalMgr* m_pFileTotalManager = { nullptr };
    class CFightManager* m_pFightManager = { nullptr };
    class CUIManager* m_pUIManager = { nullptr };

private:
    _bool m_isLevelStarted = { false };
private:
    HRESULT Ready_Camera(const wstring& strLayerTag);
    HRESULT Ready_Player(const wstring& strLayerTag);

public:
    static CLevel_DogimazoStairs* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free();
};
END
