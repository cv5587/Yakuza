#pragma once

#include "Base.h"
#include "imgui.h"
#include "Navigation_Manager.h"
#include "ObjPlace_Manager.h"
#include "LightTool_Mgr.h"
#include "Camera_Manager.h"
#include "Collider_Manager.h"
#include "Trigger_Manager.h"
#include "TriggerObj_Manager.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager);

public:
	enum IDWRIE {
		OBJPLACE,
		COLLIDER,
		LIGHT,
		TRIGGER,
		TRIGGEROBJ,
		NAVIOBJ,
		IDWRITE_END
	};

private:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Render();

public:
	IDWRIE				Get_Write() { return m_eWrieID; }

	
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };


private:
	/* Terrain에 대한 수정 */
	void	Set_Terrain_IMGUI();
	/* 설치된 Object 선택하게끔 */
	void	Show_Object_List();
	/* Navi Tool 나오기 */
	void	Set_NaviTool_IMGUI();
	void	Show_MapObj_Place_IMGUI();
	void	Show_LightTool_IMGUI();
	void	Show_CameraTool_IMGUI();
	void	Show_Collider_IMGUI();
	void	Show_Trigger_IMGUI();
	void	Show_TriggerMapObj_Place_IMGUI();

private:
	CNavigation_Manager*		m_pNavigationMgr;
	CObjPlace_Manager*			m_pObjPlace_Manager;
	CLightTool_Mgr*				m_pLightTool_Mgr;
	CCamera_Manager*			m_pCameraToolMgr;
	CCollider_Manager*			m_pColliderMgr;
	CTrigger_Manager*			m_pTriggerMgr;
	CTriggerObj_Manager*		m_pTriggerObjMgr;

	CGameInstance*				m_pGameInstance;

	/* imgui 창 관련 bool 변수 */
private:
	_bool						m_bMain_IMGUI = { true };
	_bool						m_bTerrain_IMGUI = { false };
	_bool						m_bNaviTool_IMGUI = { false }; /* 네비게이션 툴 화면 */
	_bool						m_bObject_Place_IMGUI = { false };
	_bool						m_bLightMgr_IMGUI = { false };
	_bool						m_bCameraMgr_IMGUI = { false };
	_bool						m_bColliderMgr_imgui = { false };
	_bool						m_bTriggerMgr_IMGUI = { false };
	_bool						m_bTriggerObjMgr_IMGUI = { false };

	IDWRIE						m_eWrieID = { IDWRITE_END };


public:
	virtual void Free() override;

};

