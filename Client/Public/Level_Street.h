#pragma once
#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)
class CLevel_Street :
    public CLevel
{
private:
    CLevel_Street(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CLevel_Street() = default;

public:
    virtual HRESULT Initialize();
    virtual void Tick(const _float& fTimeDelta) override;

private:
    class CSystemManager* m_pSystemManager = { nullptr };
    class CFileTotalMgr* m_pFileTotalManager = { nullptr };
    class CQuestManager* m_pQuestManager = { nullptr };
    class CUIManager* m_pUIManager = { nullptr };

private:
    HRESULT Ready_Camera(const wstring& strLayerTag);
    HRESULT Ready_Player(const wstring& strLayerTag);

    void    Play_EnvironmentSound();

public:
    static CLevel_Street* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free();
};
END
