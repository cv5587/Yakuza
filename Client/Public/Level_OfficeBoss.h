#pragma once
#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)
class CLevel_OfficeBoss :
    public CLevel
{
private:
    CLevel_OfficeBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CLevel_OfficeBoss() = default;

public:
    virtual HRESULT Initialize();
    virtual void Tick(const _float& fTimeDelta) override;

private:
    class CSystemManager* m_pSystemManager = { nullptr };
    class CFileTotalMgr* m_pFileTotalManager = { nullptr };
    class CFightManager* m_pFightManager = { nullptr };
    class CUIManager* m_pUIManager = { nullptr };
    class CQuestManager* m_pQuestManager = { nullptr };

    _bool m_isFightEnd = { false };
private:
    HRESULT Ready_Camera(const wstring& strLayerTag);
    HRESULT Ready_Player(const wstring& strLayerTag);

public:
    static CLevel_OfficeBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free();
};
END
