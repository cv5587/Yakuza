#pragma once
#include "Base.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CMainApp :
    public CBase
{
private:
    CMainApp();
    virtual ~CMainApp() = default;

public:
    HRESULT Initialize();
    void Tick(const _float& fTimeDelta);
	HRESULT Render();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	CGameInstance* m_pGameInstance = { nullptr };
	class CSystemManager* m_pSystemManager = { nullptr };
	class CFileTotalMgr* m_pFileTotalManager = { nullptr };
	class CCollision_Manager* m_pCollisionManager = { nullptr };
	class CUIManager* m_pUIManager = { nullptr };
	class CQuestManager* m_pQuestManager = { nullptr };

#ifdef _DEBUG
private:
	class CDebugManager* m_pDebugMananger = { nullptr };
#endif // _DEBUG

private:
	_tchar					m_szFPS[MAX_PATH] = TEXT("");
	_float					m_fTimeAcc = {};
	_uint					m_iNumRender = {};

	_bool m_isDebug = { false };

public:
	HRESULT Open_Level(LEVEL eLevelID);
	HRESULT Ready_Font();
	HRESULT Ready_Prototype_GameObject();
	HRESULT Ready_Prototype_Component();
	HRESULT Add_UI_On_Path(const wstring& strPath);
	void Render_Colsole(_bool isOpen);

public:
    static CMainApp* Create();
    virtual void Free() override;
};
END
